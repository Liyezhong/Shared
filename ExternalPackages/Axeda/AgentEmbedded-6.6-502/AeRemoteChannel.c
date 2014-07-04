/* $Id: AeRemoteChannel.c,v 1.1.4.1 2010/08/13 16:01:38 dkhodos Exp $ */

#include "Ae.h"

#if defined(ENABLE_REMOTE_SESSION)

static AeBool AeRemoteChannelIsDone(AeRemoteChannel *pChannel);
static AeChar *AeRemoteChannelGetHost(AeRemoteChannel *pChannel);
static AeError AeRemoteChannelOnConnected(AeRemoteChannel *pChannel, AeBool bReused);
static AeError AeRemoteChannelOnDisconnected(AeRemoteChannel *pChannel);
static AeError AeRemoteChannelOnRx(AeRemoteChannel *pChannel, AeInt32 iOctets);
static AeError AeRemoteChannelOnTx(AeRemoteChannel *pChannel, AeInt32 iOctets);
static void AeRemoteChannelOnIOError(AeRemoteChannel *pChannel, AeError iError);
static AeBool AeRemoteChannelOnConnectionReadable(AeRemoteChannel *pChannel);
static AeBool AeRemoteChannelOnConnectionWritable(AeRemoteChannel *pChannel);

AeWebTaskVTable g_aeRemoteChannelVTable =
{
    (void (*)(AeWebTask *))             AeRemoteChannelDestroy,
    (AeBool (*)(AeWebTask *))           AeRemoteChannelIsDone,
    (AeChar* (*)(AeWebTask *))          AeRemoteChannelGetHost,
    (AeError (*)(AeWebTask *, AeBool))  AeRemoteChannelOnConnected,
    (AeError (*)(AeWebTask *))          AeRemoteChannelOnDisconnected,
    (AeError (*)(AeWebTask *, AeInt32)) AeRemoteChannelOnRx,
    (AeError (*)(AeWebTask *, AeInt32)) AeRemoteChannelOnTx,
    (void (*)(AeWebTask *, AeError))    AeRemoteChannelOnIOError,
    (AeBool (*)(AeWebTask *))           AeRemoteChannelOnConnectionReadable,
    (AeBool (*)(AeWebTask *))           AeRemoteChannelOnConnectionWritable
};

/******************************************************************************/
AeRemoteChannel *AeRemoteChannelNew(AeRemoteSession *pSession, AeInt32 iId)
{
    AeRemoteChannel *pChannel;

    pChannel = (AeRemoteChannel *) AeAlloc(sizeof(AeRemoteChannel));
    if (!pChannel)
        return NULL;
    memset(pChannel, 0, sizeof(AeRemoteChannel));

    pChannel->pVTable = &g_aeRemoteChannelVTable;
    
    pChannel->pSession = pSession;
    pChannel->iState = AeRemoteChannelInitial;
    pChannel->iId = iId;

    return pChannel;
}

/******************************************************************************/
void AeRemoteChannelDestroy(AeRemoteChannel *pChannel)
{
    AeFree(pChannel);
}

/******************************************************************************/
AeInt AeRemoteChannelCompare(AePointer pData1, AePointer pData2)
{
	if (((AeRemoteChannel *) pData1)->iId == ((AeRemoteChannel *) pData2)->iId)
		return 0;
	return 1;
}

/******************************************************************************/
AeError AeRemoteChannelConnect(AeRemoteChannel *pChannel)
{
    AeTimeValue timeOut;
    AeError rc;

    /* channel connection should not time out */
    timeOut.iSec = AE_TIME_MAX_SECONDS;
    timeOut.iMicroSec = 0;

    /* get a new connection from user agent */
	pChannel->pConnection =
        AeWebUserAgentGetNewDirectConnection(g_pWebUA,
                                             (AeWebTask *) pChannel,
                                             pChannel->pSession->pInterface->pServer,
                                             pChannel->pSession->pInterface->iPort,
                                             &timeOut);

    if (!pChannel->pConnection)
        return AeEMemory;

    pChannel->iState = AeRemoteChannelConnecting;

    /* try to connect */
    rc = AeWebConnectionConnectV(pChannel->pConnection);
    if (rc != AeEOK)
    {
        AeListRemove(g_pWebUA->pConnections, pChannel->pConnection);
        AeWebConnectionDestroyV(pChannel->pConnection);
        pChannel->bError = AeTrue;
        pChannel->pConnection = NULL;
    }

    return rc;
}

/******************************************************************************/
AeError AeRemoteChannelDeactivate(AeRemoteChannel *pChannel)
{
    if (pChannel->pConnection)
    {
        AeWebConnectionDisconnectV(pChannel->pConnection);
        pChannel->pConnection = NULL;
        pChannel->iState = AeRemoteChannelInactive;
    }
    else
        AeRemoteChannelDestroy(pChannel);

    return AeEOK;
}

/******************************************************************************/
AeError AeRemoteChannelSend(AeRemoteChannel *pChannel, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft)
{
    *piBytesLeft = iSize;
    pChannel->piTxLeft = piBytesLeft;

    /* when sending to the channel, we want the entire buffer sent
      (bWaitAll == AeTrue) */
    AeWebConnectionSendV(pChannel->pConnection, pData, iSize, AeTrue);

    return AeEOK;
}

/******************************************************************************/
AeError AeRemoteChannelReceive(AeRemoteChannel *pChannel, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesDone)
{
    *piBytesDone = 0;
    pChannel->piRxDone = piBytesDone;

    /* when receiving from the channel, we want at least one byte received
      (bWaitAll == AeFalse) */
    AeWebConnectionReceiveV(pChannel->pConnection, pData, iSize, AeFalse);

    return AeEOK;
}

/******************************************************************************/
AeBool AeRemoteChannelIsReceiving(AeRemoteChannel *pChannel)
{
    return AeWebConnectionIsReceiving(pChannel->pConnection);
}

/******************************************************************************/
static AeBool AeRemoteChannelIsDone(AeRemoteChannel *pChannel)
{
    return (pChannel->iState == AeRemoteChannelInactive);
}

/******************************************************************************/
static AeChar *AeRemoteChannelGetHost(AeRemoteChannel *pChannel)
{
    if (pChannel->pConnection)
        return pChannel->pConnection->pHost;

    return NULL;
}

/******************************************************************************/
static AeError AeRemoteChannelOnConnected(AeRemoteChannel *pChannel, AeBool bReused)
{
    AeError rc;

    rc = AeEOK;
    if (pChannel->iState == AeRemoteChannelConnecting)
    {
        pChannel->iState = AeRemoteChannelConnected;

        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): channel %d connected",
                      pChannel->pSession->pId, pChannel->iId);
    }
    else
        rc = AeEInternal;

    return rc;
}

/******************************************************************************/
static AeError AeRemoteChannelOnDisconnected(AeRemoteChannel *pChannel)
{
    AeError rc = AeEOK;

    pChannel->iState = AeRemoteChannelClosed;
    pChannel->pConnection = NULL;

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                  "Remote session (id=%s): channel %d disconnected",
                  pChannel->pSession->pId, pChannel->iId);

    return rc;
}

/******************************************************************************/
static AeError AeRemoteChannelOnRx(AeRemoteChannel *pChannel, AeInt32 iOctets)
{
    *pChannel->piRxDone = iOctets;
    pChannel->pSession->channelThread.iState = AeRemoteSessionUploadingData;
    pChannel->pSession->channelThread.message.header.msgChannel = pChannel->iId;

    return AeEOK;
}

/******************************************************************************/
static AeError AeRemoteChannelOnTx(AeRemoteChannel *pChannel, AeInt32 iOctets)
{
    *pChannel->piTxLeft -= iOctets;

    return AeEOK;
}

/******************************************************************************/
static void AeRemoteChannelOnIOError(AeRemoteChannel *pChannel, AeError iError)
{
    if (pChannel->pConnection)
    {
        AeWebConnectionDisconnectV(pChannel->pConnection);
        pChannel->pConnection = NULL;
    }

    if (g_drmSettings.iLogLevel >= AeLogError)
        AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_REMOTE_SESSION,
                  "Remote session (id=%s): error on channel %d: %s",
                  pChannel->pSession->pId, pChannel->iId, AE_ERROR_STRING(iError));

    pChannel->iState = AeRemoteChannelClosed;
    pChannel->bError = AeTrue;
}

/******************************************************************************/
static AeBool AeRemoteChannelOnConnectionReadable(AeRemoteChannel *pChannel)
{
    if (pChannel->pSession->channelThread.iState == AeRemoteSessionPollingChannels)
        return AeTrue;

    /* cancel reading from this channel, when the channel thread is not in
       polling mode */
    pChannel->pConnection->iRxLeft = 0;

    return AeFalse;
}

/******************************************************************************/
static AeBool AeRemoteChannelOnConnectionWritable(AeRemoteChannel *pChannel)
{
    return AeTrue;
}

/* defined(ENABLE_REMOTE_SESSION) */
#endif

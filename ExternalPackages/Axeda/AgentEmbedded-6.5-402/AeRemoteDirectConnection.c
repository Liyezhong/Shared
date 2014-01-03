/* $Id: AeRemoteDirectConnection.c,v 1.1.4.1 2010/08/13 16:01:38 dkhodos Exp $ */

#include "Ae.h"

#if defined(ENABLE_REMOTE_SESSION)

static AeBool AeRemoteDirectConnectionIsDone(AeRemoteDirectConnection *pDC);
static AeChar *AeRemoteDirectConnectionGetHost(AeRemoteDirectConnection *pDC);
static AeError AeRemoteDirectConnectionOnConnected(AeRemoteDirectConnection *pDC, AeBool bReused);
static AeError AeRemoteDirectConnectionOnDisconnected(AeRemoteDirectConnection *pDC);
static AeError AeRemoteDirectConnectionOnRx(AeRemoteDirectConnection *pDC, AeInt32 iOctets);
static AeError AeRemoteDirectConnectionOnTx(AeRemoteDirectConnection *pDC, AeInt32 iOctets);
static void AeRemoteDirectConnectionOnIOError(AeRemoteDirectConnection *pDC, AeError iError);
static AeBool AeRemoteDirectConnectionOnConnectionReadable(AeRemoteDirectConnection *pDC);
static AeBool AeRemoteDirectConnectionOnConnectionWritable(AeRemoteDirectConnection *pDC);

static AeBool AeRemoteDirectConnectionSetPreamble(AeRemoteDirectConnection *pDC, AeRemoteSession *pSession);

AeWebTaskVTable g_aeRemoteDirectConnectionVTable =
{
    (void (*)(AeWebTask *))             AeRemoteDirectConnectionDestroy,
    (AeBool (*)(AeWebTask *))           AeRemoteDirectConnectionIsDone,
    (AeChar* (*)(AeWebTask *))          AeRemoteDirectConnectionGetHost,
    (AeError (*)(AeWebTask *, AeBool))  AeRemoteDirectConnectionOnConnected,
    (AeError (*)(AeWebTask *))          AeRemoteDirectConnectionOnDisconnected,
    (AeError (*)(AeWebTask *, AeInt32)) AeRemoteDirectConnectionOnRx,
    (AeError (*)(AeWebTask *, AeInt32)) AeRemoteDirectConnectionOnTx,
    (void (*)(AeWebTask *, AeError))    AeRemoteDirectConnectionOnIOError,
    (AeBool (*)(AeWebTask *))           AeRemoteDirectConnectionOnConnectionReadable,
    (AeBool (*)(AeWebTask *))           AeRemoteDirectConnectionOnConnectionWritable
};

/******************************************************************************/
AeRemoteDirectConnection *AeRemoteDirectConnectionNew(AeRemoteSession *pSession)
{
    AeRemoteDirectConnection *pDC;

    pDC = (AeRemoteDirectConnection *) AeAlloc(sizeof(AeRemoteDirectConnection));
    if (!pDC)
        return NULL;
    memset(pDC, 0, sizeof(AeRemoteDirectConnection));

    if (!AeRemoteDirectConnectionSetPreamble(pDC, pSession))
    {
        AeRemoteDirectConnectionDestroy(pDC);
        return NULL;
    }

    pDC->pVTable = &g_aeRemoteDirectConnectionVTable;
    
    pDC->pSession = pSession;
    pDC->iState = AeRemoteDirectConnectionInitial;

    return pDC;
}

/******************************************************************************/
void AeRemoteDirectConnectionDestroy(AeRemoteDirectConnection *pDC)
{
    if (pDC->pPreamble)
        AeFree(pDC->pPreamble);

    AeFree(pDC);
}

/******************************************************************************/
AeError AeRemoteDirectConnectionConnect(AeRemoteDirectConnection *pDC)
{
    AeTimeValue timeOut;
    AeUInt16 iPort;
    AeError rc;

    /* direct connection should not time out */
    timeOut.iSec = 0x7fffffffL;
    timeOut.iMicroSec = 0;

    iPort = pDC->pSession->pConfig->bSecure ? pDC->pSession->pConfig->iDSCSSLPort :
        pDC->pSession->pConfig->iDSCPlainPort;

    /* get a new connection from user agent */
	pDC->pConnection =
        AeWebUserAgentGetNewDirectConnection(g_pWebUA,
                                             (AeWebTask *) pDC,
                                             pDC->pSession->pConfig->pServer, iPort,
                                             &timeOut);

    if (!pDC->pConnection)
        return AeEMemory;

    /* try to connect */
    rc = AeWebConnectionConnectV(pDC->pConnection);
    if (rc != AeEOK)
    {
        AeListRemove(g_pWebUA->pConnections, pDC->pConnection);
        AeWebConnectionDestroyV(pDC->pConnection);
        pDC->bError = AeTrue;
        pDC->pConnection = NULL;
    }

    pDC->iState = AeRemoteDirectConnectionConnecting;

    return rc;
}

/******************************************************************************/
AeError AeRemoteDirectConnectionDeactivate(AeRemoteDirectConnection *pDC)
{
    if (pDC->pConnection)
    {
        AeWebConnectionDisconnectV(pDC->pConnection);
        pDC->pConnection = NULL;
        pDC->iState = AeRemoteDirectConnectionInactive;
    }
    else
        AeRemoteDirectConnectionDestroy(pDC);

    return AeEOK;
}

/******************************************************************************/
AeError AeRemoteDirectConnectionSend(AeRemoteDirectConnection *pDC, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft)
{
    *piBytesLeft = iSize;
    pDC->piTxLeft = piBytesLeft;

    AeWebConnectionSendV(pDC->pConnection, pData, iSize, AeTrue);

    return AeEOK;
}

/******************************************************************************/
AeError AeRemoteDirectConnectionReceive(AeRemoteDirectConnection *pDC, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft)
{
    *piBytesLeft = iSize;
    pDC->piRxLeft = piBytesLeft;

    AeWebConnectionReceiveV(pDC->pConnection, pData, iSize, AeTrue);

    return AeEOK;
}

/******************************************************************************/
AeBool AeRemoteDirectConnectionIsSending(AeRemoteDirectConnection *pDC)
{
    if (pDC->pConnection)
        return AeWebConnectionIsSending(pDC->pConnection);

    return AeFalse;
}

/******************************************************************************/
static AeBool AeRemoteDirectConnectionIsDone(AeRemoteDirectConnection *pDC)
{
    return (pDC->iState == AeRemoteDirectConnectionInactive);
}

/******************************************************************************/
static AeChar *AeRemoteDirectConnectionGetHost(AeRemoteDirectConnection *pDC)
{
    if (pDC->pConnection)
        return pDC->pConnection->pHost;

    return NULL;
}

/******************************************************************************/
static AeError AeRemoteDirectConnectionOnConnected(AeRemoteDirectConnection *pDC, AeBool bReused)
{
    AeError rc;

    rc = AeEOK;
    if (pDC->iState == AeRemoteDirectConnectionConnecting)
    {
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
        if (pDC->pSession->pConfig->bSecure)
            AeWebConnectionEnableSSL(pDC->pConnection);
#endif

        pDC->iState = AeRemoteDirectConnectionConnected;

        AeWebConnectionSendV(pDC->pConnection, pDC->pPreamble, pDC->iPreambleSize, AeTrue);
    }
    else
        rc = AeEInternal;

    return rc;
}

/******************************************************************************/
static AeError AeRemoteDirectConnectionOnDisconnected(AeRemoteDirectConnection *pDC)
{
    AeError rc = AeEOK;

    pDC->iState = AeRemoteDirectConnectionClosed;
    pDC->pConnection = NULL;

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                  "Remote session (id=%s): direct connection closed",
                  pDC->pSession->pId);

    return rc;
}

/******************************************************************************/
static AeError AeRemoteDirectConnectionOnRx(AeRemoteDirectConnection *pDC, AeInt32 iOctets)
{
    *pDC->piRxLeft -= iOctets;

    return AeEOK;
}

/******************************************************************************/
static AeError AeRemoteDirectConnectionOnTx(AeRemoteDirectConnection *pDC, AeInt32 iOctets)
{
    if (pDC->iState == AeRemoteDirectConnectionConnected)
    {
        pDC->iState = AeRemoteDirectConnectionPreambleSent;

        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): direct connection established",
                      pDC->pSession->pId);
    }
    else
        *pDC->piTxLeft -= iOctets;

    return AeEOK;
}

/******************************************************************************/
static void AeRemoteDirectConnectionOnIOError(AeRemoteDirectConnection *pDC, AeError iError)
{
    if (pDC->pConnection)
    {
        AeWebConnectionDisconnectV(pDC->pConnection);
        pDC->pConnection = NULL;
    }

    if (g_drmSettings.iLogLevel >= AeLogError)
        AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_REMOTE_SESSION,
                  "Remote session (id=%s): direct connection error: %s",
                  pDC->pSession->pId, AE_ERROR_STRING(iError));

    pDC->iState = AeRemoteDirectConnectionClosed;
    pDC->bError = AeTrue;
}

/******************************************************************************/
static AeBool AeRemoteDirectConnectionOnConnectionReadable(AeRemoteDirectConnection *pDC)
{
    return AeTrue;
}

/******************************************************************************/
static AeBool AeRemoteDirectConnectionOnConnectionWritable(AeRemoteDirectConnection *pDC)
{
    return AeTrue;
}

/******************************************************************************/
static AeBool AeRemoteDirectConnectionSetPreamble(AeRemoteDirectConnection *pDC, AeRemoteSession *pSession)
{
    AeInt32 iIdSize = strlen(pSession->pId);
    pDC->iPreambleSize = iIdSize + 2;
    pDC->pPreamble = AeAlloc(pDC->iPreambleSize);
    if (!pDC->pPreamble)
        return AeFalse;

    pDC->pPreamble[0] = 1;
    pDC->pPreamble[1] = (AeChar) iIdSize;
    memcpy(pDC->pPreamble + 2, pSession->pId, iIdSize);

    return AeTrue;
}

/* defined(ENABLE_REMOTE_SESSION) */
#endif

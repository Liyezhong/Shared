/**************************************************************************
 *
 *  Copyright © 1999-2003 Axeda Systems.  All rights reserved.
 *
 **************************************************************************
 *
 *  Filename   :  AeRemoteSession.c
 *  
 *  Subsystem  :  Axeda Agent Embedded
 *  
 *  Description:  Remote session implementations
 *
 **************************************************************************/

#include "Ae.h"

#if defined(ENABLE_REMOTE_SESSION)

static AeBool g_bAnnounced = AeFalse;

static AeRemoteSession *AeRemoteSessionNew(void);
static void AeRemoteSessionDestroy(AeRemoteSession *pSession);
static void AeRemoteSessionProcess(AeRemoteSession *pSession, AeTimeValue *pCurrentTime);

static void AeRemoteSessionBegin(AeRemoteSession *pSession);
static void AeRemoteSessionProbeDesktop(AeRemoteSession *pSession);
static void AeRemoteSessionSelectTransport(AeRemoteSession *pSession);
static void AeRemoteSessionConnect(AeRemoteSession *pSession);
static void AeRemoteSessionSendStart(AeRemoteSession *pSession, AeTimeValue *pCurrentTime);
static void AeRemoteSessionWaitForStart(AeRemoteSession *pSession, AeTimeValue *pCurrentTime);
static void AeRemoteSessionRunThreads(AeRemoteSession *pSession, AeTimeValue *pCurrentTime);
static void AeRemoteSessionRunTransportThread(AeRemoteSession *pSession, AeTimeValue *pCurrentTime);
static void AeRemoteSessionReceiveHeader(AeRemoteSession *pSession);
static void AeRemoteSessionReceiveBody(AeRemoteSession *pSession);
static void AeRemoteSessionConnectChannel(AeRemoteSession *pSession);
static void AeRemoteSessionProcessCommand(AeRemoteSession *pSession);
static void AeRemoteSessionProcessSessionStart(AeRemoteSession *pSession);
static void AeRemoteSessionProcessSessionStop(AeRemoteSession *pSession);
static void AeRemoteSessionProcessPing(AeRemoteSession *pSession);
static void AeRemoteSessionProcessPingResponse(AeRemoteSession *pSession);
static void AeRemoteSessionProcessOpenSocket(AeRemoteSession *pSession);
static void AeRemoteSessionProcessCloseSocket(AeRemoteSession *pSession);
static void AeRemoteSessionRemoveChannel(AeRemoteSession *pSession, AeRemoteChannel *pChannel);
static void AeRemoteSessionDownloadData(AeRemoteSession *pSession);
static void AeRemoteSessionSendSyncMessage(AeRemoteSession *pSession, AeInt32 iSize);
static void AeRemoteSessionRunChannelThread(AeRemoteSession *pSession, AeTimeValue *pCurrentTime);
static void AeRemoteSessionPollChannels(AeRemoteSession *pSession, AeTimeValue *pCurrentTime);
static void AeRemoteSessionPollChannel(AeRemoteSession *pSession, AeRemoteChannel *pChannel);
static void AeRemoteSessionCheckPing(AeRemoteSession *pSession, AeTimeValue *pCurrentTime);
static void AeRemoteSessionUploadData(AeRemoteSession *pSession, AeTimeValue *pCurrentTime);
static void AeRemoteSessionSendAsyncMessage(AeRemoteSession *pSession, AeInt32 iSize);
static void AeRemoteSessionSendStop(AeRemoteSession *pSession);
static AeBool AeRemoteSessionProcessTransport(AeRemoteSession *pSession, AeInt32 *piBytesLeft);

static AeInt32 AeRemoteSessionWriteStartCommand(AeRemoteSessionMessage *pMessage);
static AeInt32 AeRemoteSessionWriteStopCommand(AeRemoteSessionMessage *pMessage);
static AeInt32 AeRemoteSessionWritePingCommand(AeRemoteSessionMessage *pMessage);
static AeInt32 AeRemoteSessionWritePingResponseCommand(AeRemoteSessionMessage *pMessage);
static AeInt32 AeRemoteSessionWriteCloseSocketCommand(AeRemoteSessionMessage *pMessage, AeInt32 iChannel);
static AeInt32 AeRemoteSessionWriteBasicCommand(AeRemoteSessionMessage *pMessage, AeInt32 iChannel, AeInt32 iType, AeInt32 iParam);
static AeInt32 AeRemoteSessionWriteErrorCommand(AeRemoteSessionMessage *pMessage, AeInt32 iChannel, AeInt32 iCode);
static AeInt32 AeRemoteSessionWriteDataMessage(AeRemoteSessionMessage *pMessage, AeInt32 iChannel, AeInt32 iSize);
static void AeRemoteSessionCloseTransport(AeRemoteSession *pSession);

static void AeRemoteAnnounce(void);
static void AeRemotePostDesktopProbeResponse(AeRemoteSession *pSession);

static AeInt AeRemoteInterfaceCompare(AePointer pData1, AePointer pData2);
static AeInt AeRemoteSessionCompare(AePointer pData1, AePointer pData2);

static AeRemoteTransport *(*g_pTransports[])(AeRemoteSession *) =

{
    (AeRemoteTransport *(*)(AeRemoteSession *)) AeRemoteDirectTransportNew,
    (AeRemoteTransport *(*)(AeRemoteSession *)) AeRemoteHTTPTransportNew,
    NULL
};

/******************************************************************************/
AeError AeRemoteInitialize(void)
{
	g_pRemoteInterfaces = AeListNew((AeCompareFunc) AeRemoteInterfaceCompare,
                                    (AeReleaseFunc) AeRemoteInterfaceDestroy);
    if (!g_pRemoteInterfaces)
        return AeEMemory;

	g_pRemoteSessions  = AeListNew((AeCompareFunc) AeRemoteSessionCompare,
                                   (AeReleaseFunc) AeRemoteSessionDestroy);
    if (!g_pRemoteSessions)
        return AeEMemory;

    return AeEOK;
}

/******************************************************************************/
void AeRemoteShutdown(void)
{
	if (g_pRemoteInterfaces)
        AeListDestroy(g_pRemoteInterfaces);

	if (g_pRemoteSessions)
        AeListDestroy(g_pRemoteSessions);

    g_bAnnounced = AeFalse;
}

/**************************************************************************/
AeBool AeRemoteStartSession(AeInt32 iDeviceId, AeInt32 iConfigId, AeHandle pMethod, AeBool bSecure, AeDRMSOAPCommandStatus *pStatus)
{
    AeRemoteSession *pSession;
    AeRemoteInterface keyInterface, *pInterface;
    AeListItem *pListItem; 
    AeChar *pTmp;

    pStatus->iStatusCode = 0;
    pStatus->pStatusReason = NULL;

    /* interface name */
    keyInterface.pName = AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_INTNAME);
    if (!keyInterface.pName)
        keyInterface.pName = AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_APPNAME);

    /* lookup requested interface */
    keyInterface.iDeviceId = iDeviceId;
    if (!keyInterface.pName || !(pListItem = AeListFind(g_pRemoteInterfaces, &keyInterface)))
    {
        pStatus->iStatusCode = AE_DRM_SOAP_COMMAND_STATUS_FAILED;
        pStatus->pStatusReason = AE_DRM_SOAP_COMMAND_REASON_FAILED;
        return AeTrue;
    }

    pInterface = AeListData(pListItem, AeRemoteInterface);

    if (g_drmCallbacks.pOnRemoteSessionStart)
    {
        AeBool bRemoteStartOK = (*g_drmCallbacks.pOnRemoteSessionStart)(pInterface);

        if (!bRemoteStartOK)
        {
            pStatus->iStatusCode = AE_DRM_SOAP_COMMAND_STATUS_KE_INHIBIT;
            pStatus->pStatusReason = AE_DRM_SOAP_COMMAND_REASON_FAILED;
            return AeTrue;
        }
    }

    /* allocate session */
    pSession = AeRemoteSessionNew();
    if (!pSession)
    {
        pStatus->iStatusCode = AE_DRM_SOAP_COMMAND_STATUS_FAILED;
        pStatus->pStatusReason = AE_DRM_SOAP_COMMAND_REASON_FAILED;
        return AeTrue;
    }

    pSession->pInterface = pInterface;
    pSession->iConfigId = iConfigId;

    /* session id */
    pTmp = AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_SESSIONID);
    AeSetString(&pSession->pId, pTmp, -1);

    /* timeout */
    pTmp = AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_TIMEOUT);
    if (pTmp)
        pSession->timeout.iSec = strtol(pTmp, NULL, 10);

    /* transport configuration */
    pSession->pConfig = AeRemoteSessionConfigNew(bSecure, pMethod);

    /* session id is mandatory */
    if (!pSession->pId || !pSession->pConfig)
    {
        AeRemoteSessionDestroy(pSession);
        return AeFalse;
    }

    if (g_drmSettings.iLogLevel >= AeLogInfo)
        AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_REMOTE_SESSION,
            "Remote session (id=%s): created", pSession->pId);

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
            "Remote session (id=%s): postUrl=%s, getUrl=%s, server=%s, secure=%d, httpPlainPort=%d, httpSslPort=%d, dscPlainPort=%d, dscSslPort=%d",
            pSession->pId, pSession->pConfig->pPostURL, pSession->pConfig->pGetURL,
            pSession->pConfig->pServer, pSession->pConfig->bSecure,
            pSession->pConfig->iPlainPort, pSession->pConfig->iSSLPort,
            pSession->pConfig->iDSCPlainPort, pSession->pConfig->iDSCSSLPort);

    AeListAppend(g_pRemoteSessions, pSession);

    return AeTrue;
}

/******************************************************************************/
AeError AeRemoteProcess(AeTimeValue *pCurrentTime)
{
    AeListItem *pListItem;

    /* announce configured interfaces */
    if (!g_bAnnounced)
    {
        AeRemoteAnnounce();
        g_bAnnounced = AeTrue;
    }

    /* go through all active sessions */
    pListItem = AeListFirst(g_pRemoteSessions);
    while (pListItem)
    {
        AeListItem *pSessionItem = pListItem;
        AeRemoteSession *pSession = AeListData(pSessionItem, AeRemoteSession);
        pListItem = AeListNext(pListItem);

        AeRemoteSessionProcess(pSession, pCurrentTime);

        if (pSession->iSessionState == AeRemoteSessionDone)
        {
            if (g_drmCallbacks.pOnRemoteSessionEnd)
                (*g_drmCallbacks.pOnRemoteSessionEnd)(pSession->pInterface);

            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_REMOTE_SESSION,
                          "Remote session (id=%s): removed",
                          pSession->pId);

            AeListRemoveItem(g_pRemoteSessions, pSessionItem);
            AeRemoteSessionDestroy(pSession);
        }
    }

    return AeEOK;
}

/******************************************************************************/
AeRemoteInterface *AeRemoteInterfaceNew(AeInt32 iDeviceId, AeChar *pName, AeChar *pDescription, AeChar *pType, AeChar *pServer, AeUInt16 iPort)
{
    AeRemoteInterface *pInterface = (AeRemoteInterface *) AeAlloc(sizeof(AeRemoteInterface));
    if (!pInterface)
        return NULL;
    memset(pInterface, 0, sizeof(AeRemoteInterface));

    pInterface->iDeviceId = iDeviceId;
    pInterface->iPort = iPort;
    AeSetString(&pInterface->pName, pName, -1);
    AeSetString(&pInterface->pDescription, pDescription, -1);
    AeSetString(&pInterface->pRemoteType, pType, -1);
    AeSetString(&pInterface->pServer, pServer, -1);

    return pInterface;
}

/******************************************************************************/
void AeRemoteInterfaceDestroy(AeRemoteInterface *pInterface)
{
    if (pInterface->pName)
        AeFree(pInterface->pName);
    if (pInterface->pDescription)
        AeFree(pInterface->pDescription);
    if (pInterface->pRemoteType)
        AeFree(pInterface->pRemoteType);
    if (pInterface->pServer)
        AeFree(pInterface->pServer);

    AeFree(pInterface);
}

/******************************************************************************/
AeBool AeRemoteTransportInitialize(AeRemoteTransport *pTransport, AeChar *pName, AeRemoteSession *pSession)
{
    pTransport->pName = pName;
    pTransport->pSession = pSession;

    return AeEOK;
}

/******************************************************************************/
/* There are two "threads" that may be using transport for sending: "channel  */
/* thread" and "transport thread". Sending via transport must be serialized.  */
/* Therefore, when transport is being used by one thread and another thread   */
/* wants to send, the second thread's request becomes pending until the first */
/* request is complete. See AeRemoteTransportProcess().                       */
/******************************************************************************/
void AeRemoteTransportSend(AeRemoteTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft)
{
    if (AeRemoteTransportIsSendingV(pTransport))
    {
        pTransport->pTxPendingPtr = pData;
        pTransport->piTxPendingLeft = piBytesLeft;
        *pTransport->piTxPendingLeft = iSize;
    }
    else
        AeRemoteTransportSendV(pTransport, pData, iSize, piBytesLeft);
}

/******************************************************************************/
void AeRemoteTransportProcess(AeRemoteTransport *pTransport)
{
    if (pTransport->piTxPendingLeft &&
        !AeRemoteTransportIsSendingV(pTransport))
    {
        AeRemoteTransportSendV(pTransport, pTransport->pTxPendingPtr,
                               *pTransport->piTxPendingLeft,
                               pTransport->piTxPendingLeft);
        pTransport->pTxPendingPtr = NULL;
        pTransport->piTxPendingLeft = NULL;
    }
}

/******************************************************************************/
static AeRemoteSession *AeRemoteSessionNew(void)
{
    AeRemoteSession *pSession = (AeRemoteSession *) AeAlloc(sizeof(AeRemoteSession));
    if (!pSession)
        return NULL;
    memset(pSession, 0, sizeof(AeRemoteSession));

	pSession->pChannels = AePtrArrayNew((AeCompareFunc) AeRemoteChannelCompare,
                                        (AeReleaseFunc) AeRemoteChannelDeactivate);
    if (!pSession->pChannels)
    {
        AeRemoteSessionDestroy(pSession);
        return NULL;
    }

    pSession->iSessionState = AeRemoteSessionBeginning;
    pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
    pSession->transportThread.iBytesLeft = -1;
    pSession->channelThread.iState = AeRemoteSessionPollingChannels;
    pSession->channelThread.iBytesLeft = -1;

    return pSession;
}

/******************************************************************************/
static void AeRemoteSessionDestroy(AeRemoteSession *pSession)
{
    if (pSession->pId)
        AeFree(pSession->pId);
    if (pSession->pConfig)
        AeRemoteSessionConfigDestroy(pSession->pConfig);
	if (pSession->pChannels)
        AePtrArrayDestroy(pSession->pChannels);
    if (pSession->pTransport)
        AeRemoteTransportDestroyV(pSession->pTransport);
    if (pSession->pDesktopProbe)
        AeRemoteDesktopProbeDeactivate(pSession->pDesktopProbe);

    AeFree(pSession);
}

/**************************************************************************/
static void AeRemoteSessionProcess(AeRemoteSession *pSession, AeTimeValue *pCurrentTime)
{
    AeRemoteSessionState iPrevState;
    
    do
    {
        iPrevState = pSession->iSessionState;
        switch (pSession->iSessionState)
        {
            case AeRemoteSessionBeginning:
                AeRemoteSessionBegin(pSession);
                break;

            case AeRemoteSessionProbingDesktop:
                AeRemoteSessionProbeDesktop(pSession);
                break;

            case AeRemoteSessionSelectingTransport:
                AeRemoteSessionSelectTransport(pSession);
                break;

            case AeRemoteSessionConnecting:
                AeRemoteSessionConnect(pSession);
                break;

            case AeRemoteSessionSendingStart:
                AeRemoteSessionSendStart(pSession, pCurrentTime);
                break;

            case AeRemoteSessionWaitingForStart:
                AeRemoteSessionWaitForStart(pSession, pCurrentTime);
                break;

            case AeRemoteSessionStarted:
                AeRemoteSessionRunThreads(pSession, pCurrentTime);
                break;

            case AeRemoteSessionSendingStop:
                AeRemoteSessionSendStop(pSession);
                break;

            default:
                break;
        }
    }
    while (pSession->iSessionState != iPrevState);
}

/**************************************************************************/
static void AeRemoteSessionBegin(AeRemoteSession *pSession)
{
    if (!strcasecmp(pSession->pInterface->pRemoteType, "desktop"))
        pSession->iSessionState = AeRemoteSessionProbingDesktop;
    else
        pSession->iSessionState = AeRemoteSessionSelectingTransport;
}

/**************************************************************************/
static void AeRemoteSessionProbeDesktop(AeRemoteSession *pSession)
{
    if (!pSession->pDesktopProbe)
    {
        pSession->pDesktopProbe = AeRemoteDesktopProbeNew(pSession);
        if (!pSession->pDesktopProbe)
        {
            pSession->iSessionState = AeRemoteSessionDone;
            return;
        }

        AeRemoteDesktopProbeLaunch(pSession->pDesktopProbe);
    }

    if (AeRemoteDesktopProbeIsComplete(pSession->pDesktopProbe))
    {
        if (!pSession->pDesktopProbe->bError)
            AeRemotePostDesktopProbeResponse(pSession);

        AeRemoteDesktopProbeDeactivate(pSession->pDesktopProbe);
        pSession->pDesktopProbe = NULL;

        pSession->iSessionState = AeRemoteSessionSelectingTransport;
    }
}

/**************************************************************************/
static void AeRemoteSessionSelectTransport(AeRemoteSession *pSession)
{
    AeRemoteTransport *(*pTransportNew)(AeRemoteSession *) = g_pTransports[pSession->iTransportIndex++];
    if (pTransportNew)
        pSession->pTransport = (*pTransportNew)(pSession);
    else
        pSession->pTransport = NULL;

    if (pSession->pTransport)
        pSession->iSessionState = AeRemoteSessionConnecting;
    else
        pSession->iSessionState = AeRemoteSessionDone;
}

/**************************************************************************/
static void AeRemoteSessionConnect(AeRemoteSession *pSession)
{
    if (AeRemoteTransportIsErrorV(pSession->pTransport))
    {
        AeRemoteSessionCloseTransport(pSession);
        return;
    }

    if (AeRemoteTransportIsConnectingV(pSession->pTransport))
        return;

    if (AeRemoteTransportIsConnectedV(pSession->pTransport))
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): %s transport connected",
                      pSession->pId, pSession->pTransport->pName);

        pSession->iSessionState = AeRemoteSessionSendingStart;
    }
    else
        AeRemoteTransportConnectV(pSession->pTransport);
}

/**************************************************************************/
static void AeRemoteSessionSendStart(AeRemoteSession *pSession, AeTimeValue *pCurrentTime)
{
    if (pSession->transportThread.iBytesLeft == -1)
    {
        pSession->transportThread.iBytesLeft =
            AeRemoteSessionWriteStartCommand(&pSession->transportThread.message);
        AeRemoteTransportSend(pSession->pTransport,
                              (AeChar *) &pSession->transportThread.message,
                              pSession->transportThread.iBytesLeft,
                              &pSession->transportThread.iBytesLeft);

        pSession->lastUpdateTime = *pCurrentTime;
   }

    if (AeRemoteSessionProcessTransport(pSession, &pSession->transportThread.iBytesLeft))
    {
        pSession->startTime = *pCurrentTime;
        pSession->iSessionState = AeRemoteSessionWaitingForStart;
    }
}

/**************************************************************************/
static void AeRemoteSessionWaitForStart(AeRemoteSession *pSession, AeTimeValue *pCurrentTime)
{
    AeTimeValue elapsed;

    AE_TIME_VALUE_SUBTRACT(elapsed, *pCurrentTime, pSession->startTime);
    if (!AE_TIME_VALUE_GREATER_THAN(pSession->timeout, elapsed))
    {
        pSession->iSessionState = AeRemoteSessionSendingStop;

        pSession->transportThread.iBytesLeft = -1;
        pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
        return;
    }

    AeRemoteSessionRunTransportThread(pSession, pCurrentTime);
}

/**************************************************************************/
static void AeRemoteSessionRunThreads(AeRemoteSession *pSession, AeTimeValue *pCurrentTime)
{
    if (pSession->iSessionState == AeRemoteSessionStarted)
        AeRemoteSessionRunTransportThread(pSession, pCurrentTime);

    if (pSession->iSessionState == AeRemoteSessionStarted)
        AeRemoteSessionRunChannelThread(pSession, pCurrentTime);
}

/**************************************************************************/
static void AeRemoteSessionRunTransportThread(AeRemoteSession *pSession, AeTimeValue *pCurrentTime)
{
    AeRemoteSessionTransportThreadState iPrevState;
    
    do
    {
        iPrevState = pSession->transportThread.iState;
        switch (pSession->transportThread.iState)
        {
            case AeRemoteSessionReceivingHeader:
                AeRemoteSessionReceiveHeader(pSession);
                break;

            case AeRemoteSessionReceivingBody:
                AeRemoteSessionReceiveBody(pSession);
                break;

            case AeRemoteSessionConnectingChannel:
                AeRemoteSessionConnectChannel(pSession);
                break;

            case AeRemoteSessionDownloadingData:
                AeRemoteSessionDownloadData(pSession);
                break;

            case AeRemoteSessionSendingSyncMessage:
                AeRemoteSessionSendSyncMessage(pSession, -1);
                break;
        }
    }
    while (pSession->iSessionState == AeRemoteSessionStarted &&
           pSession->transportThread.iState != iPrevState);
}

/**************************************************************************/
static void AeRemoteSessionReceiveHeader(AeRemoteSession *pSession)
{
    if (pSession->transportThread.iBytesLeft == -1)
    {
        pSession->transportThread.iBytesLeft = sizeof(AeRemoteSessionHeader);
        AeRemoteTransportReceiveV(pSession->pTransport,
                                  (AeChar *) &pSession->transportThread.message.header,
                                  pSession->transportThread.iBytesLeft,
                                  &pSession->transportThread.iBytesLeft);
    }

    if (AeRemoteSessionProcessTransport(pSession, &pSession->transportThread.iBytesLeft))
    {
#ifdef AE_BIG_ENDIAN
        pSession->transportThread.message.header.msgType =
            AeNetLEToHL(pSession->transportThread.message.header.msgType);
        pSession->transportThread.message.header.msgLength =
            AeNetLEToHL(pSession->transportThread.message.header.msgLength);
        pSession->transportThread.message.header.msgChannel =
            AeNetLEToHL(pSession->transportThread.message.header.msgChannel);
#endif
        pSession->transportThread.iState = AeRemoteSessionReceivingBody;
    }
}

/**************************************************************************/
static void AeRemoteSessionReceiveBody(AeRemoteSession *pSession)
{
    if (pSession->transportThread.iBytesLeft == -1)
    {
        pSession->transportThread.iBytesLeft =
            AE_MIN(pSession->transportThread.message.header.msgLength,
                   AE_REMOTE_DATA_BUFFER_SIZE);
        pSession->transportThread.iBytesLeft =
            AE_REMOTE_ALIGN_SIZE(pSession->transportThread.iBytesLeft);
        AeRemoteTransportReceiveV(pSession->pTransport,
                                  (AeChar *) &pSession->transportThread.message.body,
                                  pSession->transportThread.iBytesLeft,
                                  &pSession->transportThread.iBytesLeft);
    }

    if (AeRemoteSessionProcessTransport(pSession, &pSession->transportThread.iBytesLeft))
    {
        if (pSession->transportThread.message.header.msgType == aProtocolData)
            pSession->transportThread.iState = AeRemoteSessionDownloadingData;
        else
        {
#ifdef AE_BIG_ENDIAN
        pSession->transportThread.message.body.command.iType =
            AeNetLEToHL(pSession->transportThread.message.body.command.iType);
#endif
            AeRemoteSessionProcessCommand(pSession);
        }
    }
}

/**************************************************************************/
static void AeRemoteSessionConnectChannel(AeRemoteSession *pSession)
{
    AeRemoteChannel *pChannel = (AeRemoteChannel *)
        AePtrArrayGet(pSession->pChannels,
                      pSession->transportThread.message.header.msgChannel);

    if (pChannel->iState == AeRemoteChannelInitial)
        AeRemoteChannelConnect(pChannel);

    if (pChannel->bError)
    {
        int iMessageSize =
            AeRemoteSessionWriteErrorCommand(&pSession->transportThread.message,
                                             pSession->transportThread.message.header.msgChannel,
                                             aErrorRefused);
        AeRemoteSessionSendSyncMessage(pSession, iMessageSize);
        AeRemoteSessionRemoveChannel(pSession, pChannel);
    }
    else if (pChannel->iState == AeRemoteChannelConnected)
        pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
}

/**************************************************************************/
static void AeRemoteSessionProcessCommand(AeRemoteSession *pSession)
{
    switch (pSession->transportThread.message.body.command.iType)
    {
        case aCmdSessionStart:
            AeRemoteSessionProcessSessionStart(pSession);
            break;
        case aCmdSessionStop:
            AeRemoteSessionProcessSessionStop(pSession);
            break;
        case aCmdPing:
            AeRemoteSessionProcessPing(pSession);
            break;
        case aCmdPingResponse:
            AeRemoteSessionProcessPingResponse(pSession);
            break;
        case aCmdOpenSocket:
            AeRemoteSessionProcessOpenSocket(pSession);
            break;
        case aCmdCloseSocket:
            AeRemoteSessionProcessCloseSocket(pSession);
            break;
    }
}

/**************************************************************************/
static void AeRemoteSessionProcessSessionStart(AeRemoteSession *pSession)
{
    if (pSession->iSessionState == AeRemoteSessionWaitingForStart)
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): received SessionStart",
                      pSession->pId);

        pSession->iSessionState = AeRemoteSessionStarted;
    }
    else
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): unexpectedly received SessionStart, ignored",
                      pSession->pId);
    }

    pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
}

/**************************************************************************/
static void AeRemoteSessionProcessSessionStop(AeRemoteSession *pSession)
{
    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                  "Remote session (id=%s): received SessionStop",
                  pSession->pId);

    pSession->iSessionState = AeRemoteSessionDone;
    pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
}

/**************************************************************************/
static void AeRemoteSessionProcessPing(AeRemoteSession *pSession)
{
    AeInt32 iMessageSize =
        AeRemoteSessionWritePingResponseCommand(&pSession->transportThread.message);
    AeRemoteSessionSendSyncMessage(pSession, iMessageSize);
}

/**************************************************************************/
static void AeRemoteSessionProcessPingResponse(AeRemoteSession *pSession)
{
    pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
}

/**************************************************************************/
static void AeRemoteSessionProcessOpenSocket(AeRemoteSession *pSession)
{
    AeRemoteChannel *pChannel;
    AeInt32 iMessageSize;

    if (pSession->iSessionState != AeRemoteSessionStarted)
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): received OpenSocket before SessionStart, ignored",
                      pSession->pId);

        pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
        return;
    }

    if (pSession->pInterface->iPort != pSession->transportThread.message.body.command.content.basic.iParam)
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): received OpenSocket for wrong port (%d), denied",
                      pSession->pId,
                      pSession->transportThread.message.body.command.content.basic.iParam);

        iMessageSize =
            AeRemoteSessionWriteErrorCommand(&pSession->transportThread.message,
                                             pSession->transportThread.message.header.msgChannel,
                                             aErrorAccess);
        AeRemoteSessionSendSyncMessage(pSession, iMessageSize);
        return;
    }

    pChannel = (AeRemoteChannel *)
        AePtrArrayGet(pSession->pChannels,
                      pSession->transportThread.message.header.msgChannel);
    if (pChannel)
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): received OpenSocket for channel already in use (%d), denied",
                      pSession->pId,
                      pSession->transportThread.message.header.msgChannel);

        iMessageSize =
            AeRemoteSessionWriteErrorCommand(&pSession->transportThread.message,
                                             pSession->transportThread.message.header.msgChannel,
                                             aErrorInuse);
        AeRemoteSessionSendSyncMessage(pSession, iMessageSize);
        return;
    }

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                  "Remote session (id=%s): received OpenSocket for channel %d",
                  pSession->pId,
                  pSession->transportThread.message.header.msgChannel);

    pChannel = AeRemoteChannelNew(pSession,
                                  pSession->transportThread.message.header.msgChannel);
    if (!pChannel)
    {
        iMessageSize =
            AeRemoteSessionWriteErrorCommand(&pSession->transportThread.message,
                                             pSession->transportThread.message.header.msgChannel,
                                             aErrorAgain);
        AeRemoteSessionSendSyncMessage(pSession, iMessageSize);
        return;
    }

    AePtrArraySet(pSession->pChannels,
                  pSession->transportThread.message.header.msgChannel, pChannel);
    pSession->transportThread.iState = AeRemoteSessionConnectingChannel;
}

/**************************************************************************/
static void AeRemoteSessionProcessCloseSocket(AeRemoteSession *pSession)
{
    AeRemoteChannel *pChannel;
    AeInt32 iMessageSize;

    if (pSession->iSessionState != AeRemoteSessionStarted)
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): received CloseSocket before SessionStart, ignored",
                      pSession->pId);

        pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
        return;
    }

    pChannel = (AeRemoteChannel *)
        AePtrArrayGet(pSession->pChannels,
                      pSession->transportThread.message.header.msgChannel);
    if (!pChannel)
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): received CloseSocket for unknown channel, ignored",
                      pSession->pId);

        iMessageSize =
            AeRemoteSessionWriteErrorCommand(&pSession->transportThread.message,
                                             pSession->transportThread.message.header.msgChannel,
                                             aErrorAccess);
        AeRemoteSessionSendSyncMessage(pSession, iMessageSize);
        return;
    }

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_REMOTE_SESSION,
                  "Remote session (id=%s): received CloseSocket for channel %d",
                  pSession->pId,
                  pSession->transportThread.message.header.msgChannel);

    AeRemoteSessionRemoveChannel(pSession, pChannel);
    pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
}

/**************************************************************************/
static void AeRemoteSessionRemoveChannel(AeRemoteSession *pSession, AeRemoteChannel *pChannel)
{
    /* AeRemoteChannel is a AeWebTask, therefore its destruction MAY BE carried out
       by AeWebUserAgent. */
    AePtrArraySet(pSession->pChannels, pChannel->iId, NULL);
    AeRemoteChannelDeactivate(pChannel);
}

/**************************************************************************/
static void AeRemoteSessionDownloadData(AeRemoteSession *pSession)
{
    AeInt32 iDataSize;

    AeRemoteChannel *pChannel = (AeRemoteChannel *)
        AePtrArrayGet(pSession->pChannels,
                      pSession->transportThread.message.header.msgChannel);
    if (!pChannel)
    {
        if (pSession->transportThread.iBytesLeft == -1)
        {
            AeInt32 iMessageSize = 
                AeRemoteSessionWriteErrorCommand(&pSession->transportThread.message,
                                                 pSession->transportThread.message.header.msgChannel,
                                                 aErrorAccess);
            AeRemoteSessionSendSyncMessage(pSession, iMessageSize);
        }
        else
        {
            pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
            pSession->transportThread.iBytesLeft = -1;
        }

        return;
    }

    iDataSize = AE_MIN(pSession->transportThread.message.header.msgLength,
                       AE_REMOTE_DATA_BUFFER_SIZE);

    if (pSession->transportThread.iBytesLeft == -1)
    {
        pSession->transportThread.iBytesLeft = iDataSize;
        AeRemoteChannelSend(pChannel, pSession->transportThread.message.body.data,
                            pSession->transportThread.iBytesLeft,
                            &pSession->transportThread.iBytesLeft);
    }

    if (pSession->transportThread.iBytesLeft == 0)
    {
        pSession->transportThread.message.header.msgLength -= iDataSize;
        if (pSession->transportThread.message.header.msgLength == 0)
            pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
        else
            pSession->transportThread.iState = AeRemoteSessionReceivingBody;

        pSession->transportThread.iBytesLeft = -1;
    }
}

/**************************************************************************/
static void AeRemoteSessionSendSyncMessage(AeRemoteSession *pSession, AeInt32 iSize)
{
    if (pSession->transportThread.iBytesLeft == -1)
    {
        pSession->transportThread.iBytesLeft = iSize;
        pSession->transportThread.iState = AeRemoteSessionSendingSyncMessage;
        AeRemoteTransportSend(pSession->pTransport,
                              (AeChar *) &pSession->transportThread.message,
                              pSession->transportThread.iBytesLeft,
                              &pSession->transportThread.iBytesLeft);
    }

    if (AeRemoteSessionProcessTransport(pSession, &pSession->transportThread.iBytesLeft))
    {
        pSession->transportThread.iState = AeRemoteSessionReceivingHeader;
    }
}

/**************************************************************************/
static void AeRemoteSessionRunChannelThread(AeRemoteSession *pSession, AeTimeValue *pCurrentTime)
{
    AeRemoteSessionChannelThreadState iPrevState;
    
    do
    {
        iPrevState = pSession->channelThread.iState;
        switch (pSession->channelThread.iState)
        {
            case AeRemoteSessionPollingChannels:
                AeRemoteSessionPollChannels(pSession, pCurrentTime);
                break;

            case AeRemoteSessionUploadingData:
                AeRemoteSessionUploadData(pSession, pCurrentTime);
                break;

            case AeRemoteSessionSendingAsyncMessage:
                AeRemoteSessionSendAsyncMessage(pSession, -1);
                break;
        }
    }
    while (pSession->iSessionState == AeRemoteSessionStarted &&
           pSession->channelThread.iState != iPrevState);
}

/**************************************************************************/
static void AeRemoteSessionPollChannels(AeRemoteSession *pSession, AeTimeValue *pCurrentTime)
{
    AeInt32 i;

    for (i = 0; i < AePtrArrayCount(pSession->pChannels); i++)
    {
        AeRemoteChannel *pChannel;

        if (pSession->channelThread.iState != AeRemoteSessionPollingChannels)
            return;
            
        pChannel = (AeRemoteChannel *) AePtrArrayGet(pSession->pChannels, i);
        if (pChannel)
            AeRemoteSessionPollChannel(pSession, pChannel);
    }

    AeRemoteSessionCheckPing(pSession, pCurrentTime);
}

/**************************************************************************/
static void AeRemoteSessionPollChannel(AeRemoteSession *pSession, AeRemoteChannel *pChannel)
{
    if (pChannel->iState == AeRemoteChannelInitial ||
        pChannel->iState == AeRemoteChannelConnecting)
        return;

    if (pChannel->bError ||
        pChannel->iState == AeRemoteChannelClosed)
    {
        AeInt32 iMessageSize =
            AeRemoteSessionWriteCloseSocketCommand(&pSession->channelThread.message,
                                                   pChannel->iId);
        AeRemoteSessionSendAsyncMessage(pSession, iMessageSize);
        AeRemoteSessionRemoveChannel(pSession, pChannel);
        return;
    }

    if (!AeRemoteChannelIsReceiving(pChannel))
        AeRemoteChannelReceive(pChannel, pSession->channelThread.message.body.data,
                               AE_REMOTE_DATA_BUFFER_SIZE,
                               &pSession->channelThread.message.header.msgLength);
}

/**************************************************************************/
static void AeRemoteSessionCheckPing(AeRemoteSession *pSession, AeTimeValue *pCurrentTime)
{
    static AeTimeValue pingRate = { AE_REMOTE_PING_RATE, 0 };
    AeTimeValue elapsed;

    AE_TIME_VALUE_SUBTRACT(elapsed, *pCurrentTime, pSession->lastUpdateTime);
    if (!AE_TIME_VALUE_GREATER_THAN(pingRate, elapsed))
    {

        AeInt32 iMessageSize =
            AeRemoteSessionWritePingCommand(&pSession->channelThread.message);
        AeRemoteSessionSendAsyncMessage(pSession, iMessageSize);

        pSession->lastUpdateTime = *pCurrentTime;
    }
}

/**************************************************************************/
static void AeRemoteSessionUploadData(AeRemoteSession *pSession, AeTimeValue *pCurrentTime)
{
    AeInt32 iMessageSize =
        AeRemoteSessionWriteDataMessage(&pSession->channelThread.message,
                                        pSession->channelThread.message.header.msgChannel,
                                        pSession->channelThread.message.header.msgLength);
    AeRemoteSessionSendAsyncMessage(pSession, iMessageSize);

    pSession->lastUpdateTime = *pCurrentTime;
}

/**************************************************************************/
static void AeRemoteSessionSendAsyncMessage(AeRemoteSession *pSession, AeInt32 iSize)
{
    if (pSession->channelThread.iBytesLeft == -1)
    {
        pSession->channelThread.iBytesLeft = iSize;
        pSession->channelThread.iState = AeRemoteSessionSendingAsyncMessage;
        AeRemoteTransportSend(pSession->pTransport,
                              (AeChar *) &pSession->channelThread.message,
                              pSession->channelThread.iBytesLeft,
                              &pSession->channelThread.iBytesLeft);
    }

    if (AeRemoteSessionProcessTransport(pSession, &pSession->channelThread.iBytesLeft))
    {
        pSession->channelThread.iState = AeRemoteSessionPollingChannels;
    }
}

/**************************************************************************/
static void AeRemoteSessionSendStop(AeRemoteSession *pSession)
{
    if (pSession->transportThread.iBytesLeft == -1)
    {
        pSession->transportThread.iBytesLeft =
            AeRemoteSessionWriteStopCommand(&pSession->transportThread.message);
        AeRemoteTransportSend(pSession->pTransport,
                              (AeChar *) &pSession->transportThread.message,
                              pSession->transportThread.iBytesLeft,
                              &pSession->transportThread.iBytesLeft);
    }

    if (AeRemoteSessionProcessTransport(pSession, &pSession->transportThread.iBytesLeft))
        pSession->iSessionState = AeRemoteSessionDone;
}

/**************************************************************************/
static AeBool AeRemoteSessionProcessTransport(AeRemoteSession *pSession, AeInt32 *piBytesLeft)
{
    if (AeRemoteTransportIsErrorV(pSession->pTransport) ||
        !AeRemoteTransportIsConnectedV(pSession->pTransport))
    {
        AeRemoteSessionCloseTransport(pSession);
        *piBytesLeft = -1;
    }
    else
    {
        AeRemoteTransportProcess(pSession->pTransport);

        if (*piBytesLeft == 0)
        {
            *piBytesLeft = -1;
            return AeTrue;
        }
    }

    return AeFalse;
}

/**************************************************************************/
static AeInt32 AeRemoteSessionWriteStartCommand(AeRemoteSessionMessage *pMessage)
{
    return AeRemoteSessionWriteBasicCommand(pMessage, 0, aCmdSessionStart, aProtocolVersion);
}

/**************************************************************************/
static AeInt32 AeRemoteSessionWriteStopCommand(AeRemoteSessionMessage *pMessage)
{
    return AeRemoteSessionWriteBasicCommand(pMessage, 0, aCmdSessionStop, 0);
}

/**************************************************************************/
static AeInt32 AeRemoteSessionWritePingCommand(AeRemoteSessionMessage *pMessage)
{
    return AeRemoteSessionWriteBasicCommand(pMessage, 0, aCmdPing, 0);
}

/**************************************************************************/
static AeInt32 AeRemoteSessionWritePingResponseCommand(AeRemoteSessionMessage *pMessage)
{
    return AeRemoteSessionWriteBasicCommand(pMessage, 0, aCmdPingResponse, 0);
}

/**************************************************************************/
static AeInt32 AeRemoteSessionWriteCloseSocketCommand(AeRemoteSessionMessage *pMessage, AeInt32 iChannel)
{
    return AeRemoteSessionWriteBasicCommand(pMessage, iChannel, aCmdCloseSocket, 0);
}

/**************************************************************************/
static AeInt32 AeRemoteSessionWriteBasicCommand(AeRemoteSessionMessage *pMessage, AeInt32 iChannel, AeInt32 iType, AeInt32 iParam)
{
    static AeInt32 iSize = 8;

    pMessage->header.msgType = AeNetHToLEL(aProtocolCommand);
    pMessage->header.msgLength = AeNetHToLEL(iSize);
    pMessage->header.msgChannel = AeNetHToLEL(iChannel);
    pMessage->header.padding = 0;

    pMessage->body.command.iType = AeNetHToLEL(iType);
    pMessage->body.command.content.basic.iParam = AeNetHToLEL(iParam);

    return (sizeof(AeRemoteSessionHeader) + iSize);
}

/**************************************************************************/
static AeInt32 AeRemoteSessionWriteErrorCommand(AeRemoteSessionMessage *pMessage, AeInt32 iChannel, AeInt32 iCode)
{
    AeChar *pText = NULL;
    AeInt32 iSize = 8;
    AeInt32 iPaddedSize = iSize;

    switch (iCode)
    {
        case aErrorRefused:
            pText = "Connection refused";
            break;

        case aErrorUnreachable:
            pText = "Destination unreachable";
            break;

        case aErrorAgain:
            pText = "Not enough system resources";
            break;

        case aErrorAccess:
            pText = "Access denied";
            break;

        case aErrorInuse:
            pText = "Channel already in use";
            break;
    }
    
    if (pText)
    {
        iSize = AE_MIN(iSize + strlen(pText), AE_REMOTE_DATA_BUFFER_SIZE);
        iPaddedSize = AE_REMOTE_ALIGN_SIZE(iSize);
    }
    
    pMessage->header.msgType = AeNetHToLEL(aProtocolCommand);
    pMessage->header.msgLength = AeNetHToLEL(iSize);
    pMessage->header.msgChannel = AeNetHToLEL(iChannel);
    pMessage->header.padding = 0;

    memset(&pMessage->body, 0, iPaddedSize);
    pMessage->body.command.iType = AeNetHToLEL(aCmdError);
    pMessage->body.command.content.error.iCode = iCode;

    if (pText)
        strncpy(pMessage->body.command.content.error.text, pText, iSize - 8);
    
    return (sizeof(AeRemoteSessionHeader) + iPaddedSize);
}

/**************************************************************************/
static AeInt32 AeRemoteSessionWriteDataMessage(AeRemoteSessionMessage *pMessage, AeInt32 iChannel, AeInt32 iSize)
{
    AeInt32 iPaddedSize = AE_REMOTE_ALIGN_SIZE(iSize);

    pMessage->header.msgType = AeNetHToLEL(aProtocolData);
    pMessage->header.msgLength = AeNetHToLEL(iSize);
    pMessage->header.msgChannel = AeNetHToLEL(iChannel);
    pMessage->header.padding = 0;

    return (sizeof(AeRemoteSessionHeader) + iPaddedSize);
}

/**************************************************************************/
static void AeRemoteSessionCloseTransport(AeRemoteSession *pSession)
{
    AeRemoteTransportDestroyV(pSession->pTransport);
    pSession->pTransport = NULL;

    if (pSession->iSessionState < AeRemoteSessionStarted)
        pSession->iSessionState = AeRemoteSessionSelectingTransport;
    else
        pSession->iSessionState = AeRemoteSessionDone;
}

/**************************************************************************/
static void AeRemoteAnnounce(void)
{
    AeInt32 i;

    /* go through all server configs */
    for (i = 0; i < AePtrArrayCount(g_pDRMServerConfigs); i++)
    {
        AeXMLElement *pContainerElement;
        AeListItem *pListItem;
        AeInt32 iPrevDeviceId;

        /* go through all configured interfaces */
        pContainerElement = NULL;
        iPrevDeviceId = AE_DRM_INVALID_DEVICE_ID;
        pListItem = AeListFirst(g_pRemoteInterfaces);
        while (pListItem)
        {
            AeRemoteInterface *pInterface;

            pInterface = AeListData(pListItem, AeRemoteInterface);
            pListItem = AeListNext(pListItem);

            /* push interface container element */
            if (iPrevDeviceId != pInterface->iDeviceId && iPrevDeviceId != AE_DRM_INVALID_DEVICE_ID)
            {
                AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemMisc, iPrevDeviceId, i,
                                  AeDRMGetFormattedContent((AeXMLContent *) pContainerElement),
                                  AeDRMQueuePriorityNormal);
                AeXMLContentDestroy((AeXMLContent *) pContainerElement);
                pContainerElement = NULL;
            }

            /* create interface container element */
            if (!pContainerElement)
            {
                pContainerElement = AeDRMEMessageCreateRemoteContainer();
                if (!pContainerElement)
                    return;
            }

            /* add item to interface container element */
            AeDRMEMessageAddRemoteItem(pContainerElement,
                                       pInterface->pName, pInterface->pDescription,
                                       pInterface->pRemoteType, pInterface->iPort);

            iPrevDeviceId = pInterface->iDeviceId;
        }

        /* push remaining interface container element */
        if (pContainerElement)
        {
            AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemMisc, iPrevDeviceId, i,
                              AeDRMGetFormattedContent((AeXMLContent *) pContainerElement),
                              AeDRMQueuePriorityNormal);
            AeXMLContentDestroy((AeXMLContent *) pContainerElement);
        }
    }
}

/******************************************************************************/
static void AeRemotePostDesktopProbeResponse(AeRemoteSession *pSession)
{
    AeXMLElement *pElement =
        AeDRMEMessageCreateRemoteDesktopInfo(pSession->pInterface->pName,
                                             &pSession->pDesktopProbe->response);
    if (!pElement)
        return;

    AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemMisc, pSession->pInterface->iDeviceId,
                      pSession->iConfigId,
                      AeDRMGetFormattedContent((AeXMLContent *) pElement),
                      AeDRMQueuePriorityNormal);

    AeXMLContentDestroy((AeXMLContent *) pElement);
}

/******************************************************************************/
static AeInt AeRemoteInterfaceCompare(AePointer pData1, AePointer pData2)
{
    /* return 0 if same */
    if (!strcmp(((AeRemoteInterface *) pData1)->pName, ((AeRemoteInterface *) pData2)->pName) &&
        ((AeRemoteInterface *) pData1)->iDeviceId == ((AeRemoteInterface *) pData2)->iDeviceId)
        return 0;
 
    return 1;
}

/******************************************************************************/
static AeInt AeRemoteSessionCompare(AePointer pData1, AePointer pData2)
{
    return !(pData1 == pData2);
}

/**************************************************************************/
AeRemoteSessionConfig *AeRemoteSessionConfigNew(AeBool bSecure, AeHandle pMethod)
{
    AeRemoteSessionConfig *pConfig;

    pConfig = (AeRemoteSessionConfig *) AeAlloc(sizeof(AeRemoteSessionConfig));
    if (!pConfig)
        return NULL;
    memset(pConfig, 0, sizeof(AeRemoteSessionConfig));

    pConfig->bSecure = bSecure;

    AeSetString(&pConfig->pPostURL,
                AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_POSTURL),
                -1);

    {
        static AeChar pParams[] = "&keepalive=infinite";
        AeChar *pGetURL = AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_GETURL);
        if (pGetURL)
        {
            AeSetString(&pConfig->pGetURL, pGetURL,
                        strlen(pGetURL) + strlen(pParams));
            strcat(pConfig->pGetURL, pParams);
        }
    }

    AeSetString(&pConfig->pServer,
                AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_SERVER),
                -1);

    {
        AeChar *pPlainPort = AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_PORT);
        if (pPlainPort)
            pConfig->iPlainPort = (AeUInt16) strtol(pPlainPort, NULL, 10);
    }
    
    {
        AeChar *pSSLPort = AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_SSLPORT);
        if (pSSLPort)
            pConfig->iSSLPort = (AeUInt16) strtol(pSSLPort, NULL, 10);
    }

    {
        AeChar *pDSCPlainPort = AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_DSC_PORT);
        if (pDSCPlainPort)
            pConfig->iDSCPlainPort = (AeUInt16) strtol(pDSCPlainPort, NULL, 10);
    }
    
    {
        AeChar *pDSCSSLPort = AeDRMSOAPGetAttributeValueByName(pMethod, AE_REMOTE_ATTR_DSC_SSLPORT);
        if (pDSCSSLPort)
            pConfig->iDSCSSLPort = (AeUInt16) strtol(pDSCSSLPort, NULL, 10);
    }

    return pConfig;
}

/**************************************************************************/
void AeRemoteSessionConfigDestroy(AeRemoteSessionConfig *pConfig)
{
    if (pConfig->pPostURL)
        AeFree(pConfig->pPostURL);
    if (pConfig->pGetURL)

        AeFree(pConfig->pGetURL);
    if (pConfig->pServer)
        AeFree(pConfig->pServer);

    AeFree(pConfig);
}

#endif /* defined(ENABLE_REMOTE_SESSION) */

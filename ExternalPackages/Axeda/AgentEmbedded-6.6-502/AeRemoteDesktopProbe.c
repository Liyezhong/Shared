/* $Id$ */

#include <stddef.h>
#include "Ae.h"

#if defined(ENABLE_REMOTE_SESSION)

static AeBool AeRemoteDesktopProbeIsDone(AeRemoteDesktopProbe *pDP);
static AeChar *AeRemoteDesktopProbeGetHost(AeRemoteDesktopProbe *pDP);
static AeError AeRemoteDesktopProbeOnConnected(AeRemoteDesktopProbe *pDP, AeBool bReused);
static AeError AeRemoteDesktopProbeOnDisconnected(AeRemoteDesktopProbe *pDP);
static AeError AeRemoteDesktopProbeOnRx(AeRemoteDesktopProbe *pDP, AeInt32 iOctets);
static AeError AeRemoteDesktopProbeOnTx(AeRemoteDesktopProbe *pDP, AeInt32 iOctets);
static void AeRemoteDesktopProbeOnIOError(AeRemoteDesktopProbe *pDP, AeError iError);
static AeBool AeRemoteDesktopProbeOnConnectionReadable(AeRemoteDesktopProbe *pDP);
static AeBool AeRemoteDesktopProbeOnConnectionWritable(AeRemoteDesktopProbe *pDP);

AeWebTaskVTable g_aeRemoteDesktopProbeVTable =
{
    (void (*)(AeWebTask *))             AeRemoteDesktopProbeDestroy,
    (AeBool (*)(AeWebTask *))           AeRemoteDesktopProbeIsDone,
    (AeChar* (*)(AeWebTask *))          AeRemoteDesktopProbeGetHost,
    (AeError (*)(AeWebTask *, AeBool))  AeRemoteDesktopProbeOnConnected,
    (AeError (*)(AeWebTask *))          AeRemoteDesktopProbeOnDisconnected,
    (AeError (*)(AeWebTask *, AeInt32)) AeRemoteDesktopProbeOnRx,
    (AeError (*)(AeWebTask *, AeInt32)) AeRemoteDesktopProbeOnTx,
    (void (*)(AeWebTask *, AeError))    AeRemoteDesktopProbeOnIOError,
    (AeBool (*)(AeWebTask *))           AeRemoteDesktopProbeOnConnectionReadable,
    (AeBool (*)(AeWebTask *))           AeRemoteDesktopProbeOnConnectionWritable
};

static const AeUInt8 g_aeRemoteDesktopProbeMagic[4] = { 0x4c, 0x39, 0xdb, 0xad};
static const AeInt32 g_aeRemoteDesktopProbeHeaderSize =
    (AeInt32) offsetof(AeRemoteDesktopProbeResponse, pAppName);

/******************************************************************************/
AeRemoteDesktopProbe *AeRemoteDesktopProbeNew(AeRemoteSession *pSession)
{
    AeRemoteDesktopProbe *pDP;

    pDP = (AeRemoteDesktopProbe *) AeAlloc(sizeof(AeRemoteDesktopProbe));
    if (!pDP)
        return NULL;
    memset(pDP, 0, sizeof(AeRemoteDesktopProbe));

    pDP->pVTable = &g_aeRemoteDesktopProbeVTable;
    
    pDP->pSession = pSession;
    pDP->iState = AeRemoteDesktopProbeInitial;

    return pDP;
}

/******************************************************************************/
void AeRemoteDesktopProbeDestroy(AeRemoteDesktopProbe *pDP)
{
    AeFree(pDP);
}

/******************************************************************************/
AeError AeRemoteDesktopProbeLaunch(AeRemoteDesktopProbe *pDP)
{
    AeTimeValue timeOut;
    AeError rc;

    timeOut.iSec = AE_REMOTE_DESKTOP_PROBE_TIMEOUT;
    timeOut.iMicroSec = 0;

    /* get a new connection from user agent */
	pDP->pConnection =
        AeWebUserAgentGetNewDirectConnection(g_pWebUA,
                                             (AeWebTask *) pDP,
                                             pDP->pSession->pInterface->pServer,
                                             AE_REMOTE_DESKTOP_PROBE_PORT,
                                             &timeOut);

    if (!pDP->pConnection)
        return AeEMemory;

    /* try to connect */
    rc = AeWebConnectionConnectV(pDP->pConnection);
    if (rc != AeEOK)
    {
        AeListRemove(g_pWebUA->pConnections, pDP->pConnection);
        AeWebConnectionDestroyV(pDP->pConnection);
        pDP->bError = AeTrue;
        pDP->pConnection = NULL;
    }

    pDP->iState = AeRemoteDesktopProbeConnecting;

    return rc;
}

/******************************************************************************/
AeBool AeRemoteDesktopProbeIsComplete(AeRemoteDesktopProbe *pDP)
{
    if (pDP->iState == AeRemoteDesktopProbeAppNameReceived ||
        pDP->iState == AeRemoteDesktopProbeClosed)
        return AeTrue;

    return AeFalse;
}

/******************************************************************************/
AeError AeRemoteDesktopProbeDeactivate(AeRemoteDesktopProbe *pDP)
{
    if (pDP->pConnection)
    {
        AeWebConnectionDisconnectV(pDP->pConnection);
        pDP->pConnection = NULL;
        pDP->iState = AeRemoteDesktopProbeInactive;
    }
    else
        AeRemoteDesktopProbeDestroy(pDP);

    return AeEOK;
}

/******************************************************************************/
static AeBool AeRemoteDesktopProbeIsDone(AeRemoteDesktopProbe *pDP)
{
    return (pDP->iState == AeRemoteDesktopProbeInactive);
}

/******************************************************************************/
static AeChar *AeRemoteDesktopProbeGetHost(AeRemoteDesktopProbe *pDP)
{
    if (pDP->pConnection)
        return pDP->pConnection->pHost;

    return NULL;
}

/******************************************************************************/
static AeError AeRemoteDesktopProbeOnConnected(AeRemoteDesktopProbe *pDP, AeBool bReused)
{
    AeError rc;

    rc = AeEOK;
    if (pDP->iState == AeRemoteDesktopProbeConnecting)
    {
        pDP->iState = AeRemoteDesktopProbeConnected;

        AeWebConnectionSendV(pDP->pConnection,
                             (AeChar *) g_aeRemoteDesktopProbeMagic,
                             sizeof(g_aeRemoteDesktopProbeMagic), AeTrue);
    }
    else
        rc = AeEInternal;

    return rc;
}

/******************************************************************************/
static AeError AeRemoteDesktopProbeOnDisconnected(AeRemoteDesktopProbe *pDP)
{
    AeError rc = AeEOK;

    if (pDP->iState != AeRemoteDesktopProbeAppNameReceived)
        pDP->bError = AeTrue;

    pDP->iState = AeRemoteDesktopProbeClosed;
    pDP->pConnection = NULL;

    return rc;
}

/******************************************************************************/
static AeError AeRemoteDesktopProbeOnRx(AeRemoteDesktopProbe *pDP, AeInt32 iOctets)
{
    if (pDP->iState == AeRemoteDesktopProbeQuerySent)
    {
        pDP->iState = AeRemoteDesktopProbeHeaderReceived;

        pDP->response.iMajor = AeNetBEToHS(pDP->response.iMajor);
        pDP->response.iMinor = AeNetBEToHS(pDP->response.iMinor);
        pDP->response.iBuild = AeNetBEToHS(pDP->response.iBuild);
        pDP->response.iRFBCompatFlag = AeNetBEToHS(pDP->response.iRFBCompatFlag);
        pDP->response.iPlatform = AeNetBEToHS(pDP->response.iPlatform);
        pDP->response.iAppNameLength = AeNetBEToHS(pDP->response.iAppNameLength);

        pDP->response.iAppNameLength =
            AE_MIN(pDP->response.iAppNameLength, sizeof(pDP->response.pAppName) - 1);

        AeWebConnectionReceiveV(pDP->pConnection, pDP->response.pAppName,
                                pDP->response.iAppNameLength, AeTrue);
    }
    else if (pDP->iState == AeRemoteDesktopProbeHeaderReceived)
    {
        pDP->iState = AeRemoteDesktopProbeAppNameReceived;

        pDP->response.pAppName[pDP->response.iAppNameLength] = 0;
    }

    return AeEOK;
}

/******************************************************************************/
static AeError AeRemoteDesktopProbeOnTx(AeRemoteDesktopProbe *pDP, AeInt32 iOctets)
{
    if (pDP->iState == AeRemoteDesktopProbeConnected)
    {
        pDP->iState = AeRemoteDesktopProbeQuerySent;

        AeWebConnectionReceiveV(pDP->pConnection, (AeChar *) &pDP->response,
                                g_aeRemoteDesktopProbeHeaderSize, AeTrue);
    }

    return AeEOK;
}

/******************************************************************************/
static void AeRemoteDesktopProbeOnIOError(AeRemoteDesktopProbe *pDP, AeError iError)
{
    if (pDP->pConnection)
    {
        AeWebConnectionDisconnectV(pDP->pConnection);
        pDP->pConnection = NULL;
    }

    if (g_drmSettings.iLogLevel >= AeLogError)
        AE_LOG_EX(AeLogWarning, AE_LOG_CATEGORY_REMOTE_SESSION,
                  "Remote session (id=%s): desktop probe error: %s",
                  pDP->pSession->pId, AE_ERROR_STRING(iError));

    pDP->iState = AeRemoteDesktopProbeClosed;
    pDP->bError = AeTrue;
}

/******************************************************************************/
static AeBool AeRemoteDesktopProbeOnConnectionReadable(AeRemoteDesktopProbe *pDP)
{
    return AeTrue;
}

/******************************************************************************/
static AeBool AeRemoteDesktopProbeOnConnectionWritable(AeRemoteDesktopProbe *pDP)
{
    return AeTrue;
}

/* defined(ENABLE_REMOTE_SESSION) */
#endif

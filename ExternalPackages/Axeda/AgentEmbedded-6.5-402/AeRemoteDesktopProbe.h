/* $Id$ */

#ifndef _AE_REMOTE_DESKTOP_PROBE_H_
#define _AE_REMOTE_DESKTOP_PROBE_H_

#if defined(ENABLE_REMOTE_SESSION)

#define AE_REMOTE_DESKTOP_PROBE_TIMEOUT 10
#define AE_REMOTE_DESKTOP_PROBE_PORT 8331

typedef enum _AeRemoteDesktopProbeState AeRemoteDesktopProbeState;

/* Remote session: desktop probe states */
enum _AeRemoteDesktopProbeState
{
    AeRemoteDesktopProbeInitial,
    AeRemoteDesktopProbeConnecting,
    AeRemoteDesktopProbeConnected,
    AeRemoteDesktopProbeQuerySent,
    AeRemoteDesktopProbeHeaderReceived,
    AeRemoteDesktopProbeAppNameReceived,
    AeRemoteDesktopProbeClosed,
    AeRemoteDesktopProbeInactive
};

/* Remote session: desktop probe */
struct _AeRemoteDesktopProbe
{
    AeWebTaskVTable         *pVTable;
    AeRemoteSession         *pSession;
	AeWebConnection         *pConnection;
    AeRemoteDesktopProbeState iState;
    AeRemoteDesktopProbeResponse response;
    AeBool                  bError;
};

#ifdef __cplusplus
extern "C" {
#endif

AeRemoteDesktopProbe *AeRemoteDesktopProbeNew(AeRemoteSession *pSession);
void AeRemoteDesktopProbeDestroy(AeRemoteDesktopProbe *pDP);
AeError AeRemoteDesktopProbeLaunch(AeRemoteDesktopProbe *pDP);
AeBool AeRemoteDesktopProbeIsComplete(AeRemoteDesktopProbe *pDP);
AeError AeRemoteDesktopProbeDeactivate(AeRemoteDesktopProbe *pDP);

#ifdef __cplusplus
}
#endif

/* ENABLE_REMOTE_SESSION */
#endif

/* _AE_REMOTE_DESKTOP_PROBE_H_*/
#endif

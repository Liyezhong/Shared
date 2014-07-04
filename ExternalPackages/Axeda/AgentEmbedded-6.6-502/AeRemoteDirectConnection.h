/* $Id: AeRemoteDirectConnection.h,v 1.1.4.1 2010/08/13 16:01:38 dkhodos Exp $ */

#ifndef _AE_REMOTE_DIRECT_CONNECTION_H_
#define _AE_REMOTE_DIRECT_CONNECTION_H_

#if defined(ENABLE_REMOTE_SESSION)

typedef struct _AeRemoteDirectConnection AeRemoteDirectConnection;
typedef enum _AeRemoteDirectConnectionState AeRemoteDirectConnectionState;

/* Remote session: direct connection states */
enum _AeRemoteDirectConnectionState
{
    AeRemoteDirectConnectionInitial,
    AeRemoteDirectConnectionConnecting,
    AeRemoteDirectConnectionConnected,
    AeRemoteDirectConnectionPreambleSent,
    AeRemoteDirectConnectionClosed,
    AeRemoteDirectConnectionInactive
};

/* Remote session: direct connection */
struct _AeRemoteDirectConnection
{
    AeWebTaskVTable         *pVTable;
    AeRemoteSession         *pSession;
    AeChar                  *pPreamble;
    AeInt32                 iPreambleSize;
	AeWebConnection         *pConnection;
    AeRemoteDirectConnectionState iState;
    AeInt32                 *piTxLeft;
    AeInt32                 *piRxLeft;
    AeBool                  bError;
};

#ifdef __cplusplus
extern "C" {
#endif

AeRemoteDirectConnection *AeRemoteDirectConnectionNew(AeRemoteSession *pSession);
void            AeRemoteDirectConnectionDestroy(AeRemoteDirectConnection *pDC);
AeError         AeRemoteDirectConnectionConnect(AeRemoteDirectConnection *pDC);
AeError         AeRemoteDirectConnectionDeactivate(AeRemoteDirectConnection *pDC);
AeError         AeRemoteDirectConnectionSend(AeRemoteDirectConnection *pDC, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
AeError         AeRemoteDirectConnectionReceive(AeRemoteDirectConnection *pDC, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
AeBool          AeRemoteDirectConnectionIsSending(AeRemoteDirectConnection *pDC);

#ifdef __cplusplus
}
#endif

/* ENABLE_REMOTE_SESSION */
#endif

/* _AE_REMOTE_DIRECT_CONNECTION_H_*/
#endif

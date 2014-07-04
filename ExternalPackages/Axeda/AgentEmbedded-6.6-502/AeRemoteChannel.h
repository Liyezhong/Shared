/* $Id: AeRemoteChannel.h,v 1.1.4.1 2010/08/13 16:01:38 dkhodos Exp $ */

#ifndef _AE_REMOTE_CHANNEL_H_
#define _AE_REMOTE_CHANNEL_H_

#if defined(ENABLE_REMOTE_SESSION)

typedef struct _AeRemoteChannel AeRemoteChannel;
typedef enum _AeRemoteChannelState AeRemoteChannelState;

/* Remote session channel states */
enum _AeRemoteChannelState
{
    AeRemoteChannelInitial,
    AeRemoteChannelConnecting,
    AeRemoteChannelConnected,
    AeRemoteChannelClosed,
    AeRemoteChannelInactive
};

/* Remote session channel */
struct _AeRemoteChannel
{
    AeWebTaskVTable         *pVTable;
    AeRemoteSession         *pSession;
	AeWebConnection         *pConnection;
	AeInt32                 iId;
    AeRemoteChannelState    iState;
    AeInt32                 *piTxLeft;
    AeInt32                 *piRxDone;
    AeBool                  bError;
};

#ifdef __cplusplus
extern "C" {
#endif

AeRemoteChannel *AeRemoteChannelNew(AeRemoteSession *pSession, AeInt32 iId);
void            AeRemoteChannelDestroy(AeRemoteChannel *pChannel);
AeError         AeRemoteChannelConnect(AeRemoteChannel *pChannel);
AeError         AeRemoteChannelDeactivate(AeRemoteChannel *pChannel);
AeError         AeRemoteChannelSend(AeRemoteChannel *pChannel, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
AeError         AeRemoteChannelReceive(AeRemoteChannel *pChannel, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesDone);
AeBool          AeRemoteChannelIsReceiving(AeRemoteChannel *pChannel);
AeInt           AeRemoteChannelCompare(AePointer pData1, AePointer pData2);

#ifdef __cplusplus
}
#endif

/* ENABLE_REMOTE_SESSION */
#endif

/* _AE_REMOTE_CHANNEL_H_*/
#endif

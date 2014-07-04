/**************************************************************************
 *
 *  Copyright © 1999-2007 Axeda Corporation.  All rights reserved.
 *
 **************************************************************************
 *
 *  Filename   :  AeRemoteSession.h
 *
 *  Subsystem  :  Axeda Agent Embedded
 *
 *  Description:  Remote session declarations
 *
 **************************************************************************/

#ifndef _AE_REMOTE_SESSION_H_
#define _AE_REMOTE_SESSION_H_

#if defined(ENABLE_REMOTE_SESSION)

#define AE_REMOTE_ATTR_INTNAME		"interfacename"
#define AE_REMOTE_ATTR_APPNAME		"applicationname"
#define AE_REMOTE_ATTR_SESSIONID	"sessionid"
#define AE_REMOTE_ATTR_POSTURL		"puturl"
#define AE_REMOTE_ATTR_GETURL		"geturl"
#define AE_REMOTE_ATTR_SERVER		"server"
#define AE_REMOTE_ATTR_PORT		    "port"
#define AE_REMOTE_ATTR_SSLPORT      "sslport"
#define AE_REMOTE_ATTR_DSC_PORT     "dscport"
#define AE_REMOTE_ATTR_DSC_SSLPORT  "dscsslport"
#define AE_REMOTE_ATTR_TIMEOUT		"timeout"

/* Protocol version					*/
#define aProtocolVersion            1

/* Message type						*/
#define aProtocolData				1
#define aProtocolCommand			2
#define aProtocolUser				3

/* Command types					*/
#define aCmdSessionStart			1
#define aCmdSessionStop				2
#define aCmdPing					3
#define aCmdPingResponse			4
#define aCmdOpenSocket				5
#define aCmdCloseSocket				6
#define aCmdError					7

/* Error types sent to DRM */
#define aErrorRefused				1
#define aErrorUnreachable			2
#define aErrorAgain					3
#define aErrorInternal				4
#define aErrorAccess				10
#define aErrorInuse					11

#define AE_REMOTE_PING_RATE         10
#define AE_REMOTE_DATA_BUFFER_SIZE  4096
#define AE_REMOTE_ALIGN_SIZE(x)     (((x) + 7) & ~7)

typedef struct _AeRemoteSessionHeader	AeRemoteSessionHeader;
typedef struct _AeRemoteSessionCommand  AeRemoteSessionCommand;
typedef struct _AeRemoteSessionMessage  AeRemoteSessionMessage;
typedef enum _AeRemoteSessionState      AeRemoteSessionState;
typedef enum _AeRemoteSessionTransportThreadState AeRemoteSessionTransportThreadState;
typedef struct _AeRemoteSessionTransportThread AeRemoteSessionTransportThread;
typedef enum _AeRemoteSessionChannelThreadState AeRemoteSessionChannelThreadState;
typedef struct _AeRemoteSessionChannelThread AeRemoteSessionChannelThread;
typedef struct _AeRemoteSession         AeRemoteSession;
typedef struct _AeRemoteSessionConfig   AeRemoteSessionConfig;
typedef struct _AeRemoteTransport       AeRemoteTransport;
typedef struct _AeRemoteTransportVTable AeRemoteTransportVTable;
typedef struct _AeRemoteDesktopProbe AeRemoteDesktopProbe;


/* Remote session message header */
struct _AeRemoteSessionHeader
{
    AeInt32 msgType;
    AeInt32 msgLength;
    AeInt32 msgChannel;
    AeInt32 padding;
};

/* Remote session command */
struct _AeRemoteSessionCommand
{
    AeInt32 iType;
    union
    {
        struct
        {
            AeInt32 iParam;
        } basic;
        struct
        {
            AeInt32 iCode;
            AeChar  text[1];
        } error;
    } content;
};

/* Remote session message */
struct _AeRemoteSessionMessage
{
    AeRemoteSessionHeader header;
    union
    {
        AeRemoteSessionCommand command;
        AeChar data[AE_REMOTE_DATA_BUFFER_SIZE];
    } body;
};

/* Remote session state */
enum _AeRemoteSessionState
{
    AeRemoteSessionBeginning,
    AeRemoteSessionProbingDesktop,
    AeRemoteSessionSelectingTransport,
    AeRemoteSessionConnecting,
    AeRemoteSessionSendingStart,
    AeRemoteSessionWaitingForStart,
    AeRemoteSessionStarted,
    AeRemoteSessionSendingStop,
    AeRemoteSessionDone
};

/* Remote session: transport thread state */
enum _AeRemoteSessionTransportThreadState
{
    AeRemoteSessionReceivingHeader,
    AeRemoteSessionReceivingBody,
    AeRemoteSessionConnectingChannel,
    AeRemoteSessionDownloadingData,
    AeRemoteSessionSendingSyncMessage
};

/* Remote session: transport thread */
struct _AeRemoteSessionTransportThread
{
    AeRemoteSessionTransportThreadState iState;
    AeRemoteSessionMessage  message;
    AeInt32                 iBytesLeft;
};

/* Remote session: channel thread state */
enum _AeRemoteSessionChannelThreadState
{
    AeRemoteSessionPollingChannels,
    AeRemoteSessionUploadingData,
    AeRemoteSessionSendingAsyncMessage
};

/* Remote session: channel thread */
struct _AeRemoteSessionChannelThread
{
    AeRemoteSessionChannelThreadState iState;
    AeRemoteSessionMessage  message;
    AeInt32                 iBytesLeft;
};

/* Remote session: desktop probe */
struct _AeRemoteDesktopProbe;

/* Remote session */
struct _AeRemoteSession
{
    AeRemoteInterface       *pInterface;
    AeInt32                 iConfigId;
	AeChar                  *pId;
    AeRemoteSessionConfig   *pConfig;
	AeTimeValue             timeout;
    AeTimeValue             startTime;
    AeTimeValue             lastUpdateTime;
    AeRemoteSessionState    iSessionState;
    AeRemoteSessionTransportThread transportThread;
    AeRemoteSessionChannelThread channelThread;
    AePtrArray              *pChannels;
    AeRemoteTransport       *pTransport;
    size_t                  iTransportIndex;
    AeRemoteDesktopProbe    *pDesktopProbe;
};

/* Remote session configuration */
struct _AeRemoteSessionConfig
{
    AeBool      bSecure;
	AeChar      *pPostURL;
	AeChar      *pGetURL;
	AeChar      *pServer;
    AeUInt16    iPlainPort;
    AeUInt16    iSSLPort;
    AeUInt16    iDSCPlainPort;
    AeUInt16    iDSCSSLPort;
};

/* Remote session transport mechanism */
struct _AeRemoteTransport
{
    AeRemoteTransportVTable *pVTable;
    AeChar                  *pName;
    AeRemoteSession         *pSession;
    AeChar                  *pTxPendingPtr;
    AeInt32                 *piTxPendingLeft;
};

/* AeRemoteTransport vtable */
struct _AeRemoteTransportVTable
{
    void    (*pDestroy)(AeRemoteTransport *pTransport);
    void    (*pConnect)(AeRemoteTransport *pTransport);
    void    (*pSend)(AeRemoteTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
    void    (*pReceive)(AeRemoteTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
    AeBool  (*pIsConnecting)(AeRemoteTransport *pTransport);
    AeBool  (*pIsConnected)(AeRemoteTransport *pTransport);
    AeBool  (*pIsSending)(AeRemoteTransport *pTransport);
    AeBool  (*pIsError)(AeRemoteTransport *pTransport);
};

#define AeRemoteTransportDestroyV(pTransport) \
    (*((AeRemoteTransport *) (pTransport))->pVTable->pDestroy)((AeRemoteTransport *) (pTransport))
#define AeRemoteTransportConnectV(pTransport) \
    (*((AeRemoteTransport *) (pTransport))->pVTable->pConnect)((AeRemoteTransport *) (pTransport))
#define AeRemoteTransportSendV(pTransport, pData, iSize, piBytesLeft) \
    (*((AeRemoteTransport *) (pTransport))->pVTable->pSend)((AeRemoteTransport *) (pTransport), pData, iSize, piBytesLeft)
#define AeRemoteTransportReceiveV(pTransport, pData, iSize, piBytesLeft) \
    (*((AeRemoteTransport *) (pTransport))->pVTable->pReceive)((AeRemoteTransport *) (pTransport), pData, iSize, piBytesLeft)
#define AeRemoteTransportIsConnectingV(pTransport) \
    (*((AeRemoteTransport *) (pTransport))->pVTable->pIsConnecting)((AeRemoteTransport *) (pTransport))
#define AeRemoteTransportIsConnectedV(pTransport) \
    (*((AeRemoteTransport *) (pTransport))->pVTable->pIsConnected)((AeRemoteTransport *) (pTransport))
#define AeRemoteTransportIsSendingV(pTransport) \
    (*((AeRemoteTransport *) (pTransport))->pVTable->pIsSending)((AeRemoteTransport *) (pTransport))
#define AeRemoteTransportIsErrorV(pTransport) \
    (*((AeRemoteTransport *) (pTransport))->pVTable->pIsError)((AeRemoteTransport *) (pTransport))

#ifdef __cplusplus
extern "C" {
#endif

AeError AeRemoteInitialize(void);
void    AeRemoteShutdown(void);
AeBool	AeRemoteStartSession(AeInt32 iDeviceId, AeInt32 iConfigId, AeHandle pMethod, AeBool bSecure, AeDRMSOAPCommandStatus *pStatus);
AeError AeRemoteProcess(AeTimeValue *pCurrentTime);

AeRemoteInterface *AeRemoteInterfaceNew(AeInt32 iDeviceId, AeChar *pName, AeChar *pDescription, AeChar *pType, AeChar *pServer, AeUInt16 iPort);
void	AeRemoteInterfaceDestroy(AeRemoteInterface *pInterface);

AeBool  AeRemoteTransportInitialize(AeRemoteTransport *pTransport, AeChar *pName, AeRemoteSession *pSession);
void    AeRemoteTransportSend(AeRemoteTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
void    AeRemoteTransportProcess(AeRemoteTransport *pTransport);

AeRemoteSessionConfig *AeRemoteSessionConfigNew(AeBool bSecure, AeHandle pMethod);
void    AeRemoteSessionConfigDestroy(AeRemoteSessionConfig *pConfig);

#ifdef __cplusplus
}
#endif

/* ENABLE_REMOTE_SESSION */
#endif

/* _AE_REMOTE_SESSION_H_ */
#endif






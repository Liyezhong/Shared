/* $Id: AeWebConnection.h,v 1.6.2.1 2010/08/13 16:01:39 dkhodos Exp $ */

#ifndef _AE_WEB_CONNECTION_H_
#define _AE_WEB_CONNECTION_H_

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
#define AE_SSL_BUFFER_SIZE  32768
#endif

typedef enum _AeWebConnectionState AeWebConnectionState;
typedef struct _AeWebConnectionVTable AeWebConnectionVTable;

enum _AeWebConnectionState
{
    AeWebConnectionClosed,
    AeWebConnectionConnecting,
    AeWebConnectionEstablished
};

struct _AeWebConnection
{
    AeWebConnectionVTable   *pVTable;

    AeWebTask               *pTask;

    AeWebConnectionState    iState;
    AeChar                  *pHost;
    AeNetAddress            netAddress;
    AeTimeValue             timeOut;

    AeChar                  *pOriginHost;
    AeUInt16                iOriginPort;

	AeSocket                sock;

    AeChar                  *pRxPtr;
    AeInt32                 iRxTotal;
    AeInt32                 iRxLeft;
	AeBool                  bRxWaitAll;
    AeChar                  *pTxPtr;
    AeInt32                 iTxTotal;
    AeInt32                 iTxLeft;
	AeBool                  bTxWaitAll;
    AeTimeValue             lastActivityTime;
    AeBool                  bSocketValid;

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    AeBool                  bSSLEnabled;
    AeBool                  bSSLNegotiated;
    SSL                     *pSSL;
#endif
};

struct _AeWebConnectionVTable
{
    void    (*pDestroy)(AeWebConnection *pConn);
    void    (*pInitialize)(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort, AeTimeValue *pTimeOut);
    AeError (*pConnect)(AeWebConnection *pConn);
    AeError (*pDisconnect)(AeWebConnection *pConn);
    AeError (*pSend)(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll);
    AeError (*pReceive)(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll);
    AeBool  (*pIsEqual)(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort);
    AeBool  (*pIsConnected)(AeWebConnection *pConn);
    AeError (*pOnReadable)(AeWebConnection *pConn, AeTimeValue *pTimeValue);
    AeError (*pOnWritable)(AeWebConnection *pConn, AeTimeValue *pTimeValue);
};

#define AeWebConnectionDestroyV(pConn) \
    (*(pConn)->pVTable->pDestroy)(pConn)
#define AeWebConnectionInitializeV(pConn, pHost, iPort, pOriginHost, iOriginPort, pTimeOut) \
    (*(pConn)->pVTable->pInitialize)(pConn, pHost, iPort, pOriginHost, iOriginPort, pTimeOut)
#define AeWebConnectionConnectV(pConn) \
    (*(pConn)->pVTable->pConnect)(pConn)
#define AeWebConnectionDisconnectV(pConn) \
    (*(pConn)->pVTable->pDisconnect)(pConn)
#define AeWebConnectionSendV(pConn, pData, iSize, bWaitAll) \
    (*(pConn)->pVTable->pSend)(pConn, pData, iSize, bWaitAll)
#define AeWebConnectionReceiveV(pConn, pData, iSize, bWaitAll) \
    (*(pConn)->pVTable->pReceive)(pConn, pData, iSize, bWaitAll)
#define AeWebConnectionIsEqualV(pConn, pHost, iPort, pOriginHost, iOriginPort) \
    (*(pConn)->pVTable->pIsEqual)(pConn, pHost, iPort, pOriginHost, iOriginPort)
#define AeWebConnectionIsConnectedV(pConn) \
    (*(pConn)->pVTable->pIsConnected)(pConn)
#define AeWebConnectionOnReadableV(pConn, pTimeValue) \
    (*(pConn)->pVTable->pOnReadable)(pConn, pTimeValue)
#define AeWebConnectionOnWritableV(pConn, pTimeValue) \
    (*(pConn)->pVTable->pOnWritable)(pConn, pTimeValue)

#ifdef __cplusplus
extern "C" {
#endif

AeWebConnection *AeWebConnectionNew(void);
void            AeWebConnectionConstruct(AeWebConnection *pConn);
void            AeWebConnectionDestroy(AeWebConnection *pConn);
void            AeWebConnectionFree(AeWebConnection *pConn);
void            AeWebConnectionInitialize(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort, AeTimeValue *pTimeOut);
AeError         AeWebConnectionConnect(AeWebConnection *pConn);
AeError         AeWebConnectionDisconnect(AeWebConnection *pConn);
AeError         AeWebConnectionSend(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll);
AeError         AeWebConnectionReceive(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll);
AeBool          AeWebConnectionIsEqual(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort);
AeBool          AeWebConnectionIsConnected(AeWebConnection *pConn);
AeError         AeWebConnectionOnReadable(AeWebConnection *pConn, AeTimeValue *pTimeValue);
AeError         AeWebConnectionOnWritable(AeWebConnection *pConn, AeTimeValue *pTimeValue);

AeBool          AeWebConnectionIsConnecting(AeWebConnection *pConn);
AeBool          AeWebConnectionIsClosed(AeWebConnection *pConn);
AeBool          AeWebConnectionIsReceiving(AeWebConnection *pConn);
AeBool          AeWebConnectionIsSending(AeWebConnection *pConn);
AeBool          AeWebConnectionIsRxPending(AeWebConnection *pConn);

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
AeError         AeWebConnectionEnableSSL(AeWebConnection *pConn);
#endif

#ifdef __cplusplus
}
#endif

#endif

/* $Id: AeWebCommon.h,v 1.8.2.2 2010/08/13 16:01:39 dkhodos Exp $ */

#ifndef _AE_WEB_COMMON_H_
#define _AE_WEB_COMMON_H_

#define AE_HTTP_TIMEOUT     30
#define AE_HOSTNAME_TTL     3600

typedef struct _AeWebSettings AeWebSettings;
typedef struct _AeWebConnection AeWebConnection;
typedef struct _AeWebTransaction AeWebTransaction;
typedef struct _AeWebUserAgent AeWebUserAgent;
typedef struct _AeWebHostEntry AeWebHostEntry;
typedef struct _AeWebHeader AeWebHeader;
typedef AeList AeWebHeaders;
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
typedef struct _AeWebSSLInfo AeWebSSLInfo;
typedef struct _AeWebSSLSession AeWebSSLSession;
#endif
typedef struct _AeWebTask AeWebTask;
typedef struct _AeWebTaskVTable AeWebTaskVTable;

struct _AeWebSettings
{
    AeLogLevel  iLogLevel;
};

struct _AeWebHostEntry
{
    AeChar      *pHostName;
    AeUInt32    iIPAddress;
    AeTimeValue timeStamp;
};

struct _AeWebHeader
{
    AeChar  *pName;
    AeChar  *pValue;
};

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
struct _AeWebSSLInfo
{
    AeWebCryptoLevel    iCryptoLevel;
    AeBool              bServerAuth;
	SSL_CTX             *pContext;
};

struct _AeWebSSLSession
{
    AeChar      *pHost;
    AeUInt16    iPort;
    SSL_SESSION *pSession;
};
#endif

struct _AeWebTask
{
    AeWebTaskVTable *pVTable;
};

struct _AeWebTaskVTable
{
    void    (*pDestroy)(AeWebTask *pTask);
    AeBool  (*pIsDone)(AeWebTask *pTask);
    AeChar* (*pGetHost)(AeWebTask *pTask);
    AeError (*pOnConnected)(AeWebTask *pTask, AeBool bReused);
    AeError (*pOnDisconnected)(AeWebTask *pTask);
    AeError (*pOnRx)(AeWebTask *pTask, AeInt32 iOctets);
    AeError (*pOnTx)(AeWebTask *pTask, AeInt32 iOctets);
    void    (*pOnIOError)(AeWebTask *pTask, AeError iError);
    AeBool  (*pOnConnectionReadable)(AeWebTask *pTask);
    AeBool  (*pOnConnectionWritable)(AeWebTask *pTask);
};

#define AeWebTaskDestroyV(pTask) \
    (*(pTask)->pVTable->pDestroy)(pTask)
#define AeWebTaskIsDoneV(pTask) \
    (*(pTask)->pVTable->pIsDone)(pTask)
#define AeWebTaskGetHostV(pTask) \
    (*(pTask)->pVTable->pGetHost)(pTask)
#define AeWebTaskOnConnectedV(pTask, bReused) \
    (*(pTask)->pVTable->pOnConnected)(pTask, bReused)
#define AeWebTaskOnDisconnectedV(pTask) \
    (*(pTask)->pVTable->pOnDisconnected)(pTask)
#define AeWebTaskOnRxV(pTask, iOctets) \
    (*(pTask)->pVTable->pOnRx)(pTask, iOctets)
#define AeWebTaskOnTxV(pTask, iOctets) \
    (*(pTask)->pVTable->pOnTx)(pTask, iOctets)
#define AeWebTaskOnIOErrorV(pTask, iError) \
    (*(pTask)->pVTable->pOnIOError)(pTask, iError)
#define AeWebTaskOnConnectionReadableV(pTask) \
    (*(pTask)->pVTable->pOnConnectionReadable)(pTask)
#define AeWebTaskOnConnectionWritableV(pTask) \
    (*(pTask)->pVTable->pOnConnectionWritable)(pTask)

#ifdef __cplusplus
extern "C" {
#endif

AeError         AeWebInitialize(void);

void            AeWebProxySet(AeWebProxyInfo **ppProxy, AeWebProxyProtocol iProto, AeChar *pHost, AeUInt16 iPort, AeChar *pUser, AeChar *pPassword);
void            AeWebProxyDestroy(AeWebProxyInfo *pProxy);

AeBool          AeWebGetCachedIPAddress(AeChar *pHostName, AeUInt32 *piIPAddress);
void            AeWebSetCachedIPAddress(AeChar *pHostName, AeUInt32 iIPAddress);

AeWebHeaders    *AeWebHeadersNew(void);
void            AeWebHeadersDestroy(AeWebHeaders *pHeaders);
void            AeWebHeadersSet(AeWebHeaders *pHeaders, AeChar *pName, AeChar *pValue);
AeChar          *AeWebHeadersGet(AeWebHeaders *pHeaders, AeChar *pName);
void            AeWebHeadersRemove(AeWebHeaders *pHeaders, AeChar *pName);
void            AeWebHeadersRemoveAll(AeWebHeaders *pHeaders);
void            AeWebHeadersWrite(AeWebHeaders *pHeaders, AeBuffer *pBuffer);

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
AeError         AeWebSSLInitialize(void);
void            AeWebSSLDestroy(void);
AeBool          AeWebGetSSLSession(AeChar *pHost, AeUInt16 iPort, SSL_SESSION **ppSession);
void            AeWebSetSSLSession(AeChar *pHost, AeUInt16 iPort, SSL_SESSION *pSession);
#endif

#ifdef __cplusplus
}
#endif

#endif

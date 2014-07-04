/* $Id: AeWebTransaction.h,v 1.9.2.3 2010/08/13 16:01:39 dkhodos Exp $ */

#ifndef _AE_WEB_TRANSACTION_H_
#define _AE_WEB_TRANSACTION_H_

#define HTTP_HEADER_HOST               "Host"
#define HTTP_HEADER_CONNECTION         "Connection"
#define HTTP_HEADER_PROXY_CONNECTION   "Proxy-Connection"
#define HTTP_HEADER_CONTENTTYPE        "Content-Type"
#define HTTP_HEADER_CONTENTLENGTH      "Content-Length"
#define HTTP_HEADER_TRANSFERENCODING   "Transfer-Encoding"
#define HTTP_HEADER_WWWAUTHENTICATE    "WWW-Authenticate"
#define HTTP_HEADER_PROXYAUTHENTICATE  "Proxy-Authenticate"

#define HTTP "HTTP"
#define CRLF "\r\n"

typedef enum _AeWebTransactionState AeWebTransactionState;
typedef enum _AeWebTransactionResult AeWebTransactionResult;

enum _AeWebTransactionState
{
    AeWebTransactionDormant,
    AeWebTransactionInitiated,
    AeWebTransactionConnected,
    AeWebTransactionHeadSent,
    AeWebTransactionHeadReceived,
    AeWebTransactionCompleted
};

enum _AeWebTransactionResult
{
    AeWebTransactionHead,
    AeWebTransactionEntity,
    AeWebTransactionAgain,
    AeWebTransactionContinue,
    AeWebTransactionTunnel
};

struct _AeWebTransaction
{
    AeWebTaskVTable         *pVTable;

    AeWebUserAgent          *pUserAgent;
    AeWebRequest            *pRequest;
	AeWebConnection         *pConnection;
    AeWebTransactionState   iState;

    AeBuffer                *pRequestData;
    AeBuffer                *pResponseData;

    AeWebHeaders            *pRequestHeaders;
    AeWebHeaders            *pResponseHeaders;

    AeChar                  *pResponseVersion;
    AeInt                   iResponseStatus;
    AeInt32                 iContentLength;
    AeInt32                 iResponseEntitySize;
    AeInt32                 iResponseChunkSize;
    AeBool                  bResponsePersistent;
    AeBool                  bResponseChunked;
    AeBool                  bSkipBody;
    AeBool                  bRestart;
    AeBool                  bReusedConnection;
    AeBool                  bRecoverFromAsynchClose;

    AeWebAuthParamsNTLM     *pNTLMParams;
};

#ifdef __cplusplus
extern "C" {
#endif

AeWebTransaction    *AeWebTransactionNew(AeWebUserAgent *pUserAgent, AeWebRequest *pRequest);
void                AeWebTransactionDestroy(AeWebTransaction *pTran);
AeError             AeWebTransactionInitiate(AeWebTransaction *pTran);
AeBool              AeWebTransactionIsDone(AeWebTransaction *pTran);
AeChar              *AeWebTransactionGetHost(AeWebTransaction *pTran);
AeBool              AeWebTransactionIsEqual(AeWebTransaction *pTran, AeWebRequest *pRequest);
AeError             AeWebTransactionOnConnected(AeWebTransaction *pTran, AeBool bReused);
AeError             AeWebTransactionOnDisconnected(AeWebTransaction *pTran);
AeError             AeWebTransactionOnRx(AeWebTransaction *pTran, AeInt32 iOctets);
AeError             AeWebTransactionOnTx(AeWebTransaction *pTran, AeInt32 iOctets);
void                AeWebTransactionOnIOError(AeWebTransaction *pTran, AeError iError);
AeBool              AeWebTransactionOnConnectionReadable(AeWebTransaction *pTran);
AeBool              AeWebTransactionOnConnectionWritable(AeWebTransaction *pTran);

#ifdef __cplusplus
}
#endif

#endif

/* $Id: AeRemoteHTTPTransport.h,v 1.1.4.1 2010/08/13 16:01:38 dkhodos Exp $ */

#ifndef _AE_REMOTE_HTTP_TRANSPORT_H_
#define _AE_REMOTE_HTTP_TRANSPORT_H_

#if defined(ENABLE_REMOTE_SESSION)

typedef struct _AeRemoteHTTPTransport AeRemoteHTTPTransport;

/* Remote session transport mechanism */
struct _AeRemoteHTTPTransport
{
    AeRemoteTransport   parent;
	AeChar              *pPostURL;
	AeChar              *pGetURL;
    AeWebRequest        *pPostRequest;
    AeChar              *pTxPtr;
    AeInt32             *piTxLeft;
    AeWebRequest        *pGetRequest;
    AeChar              *pRxPtr;
    AeInt32             *piRxLeft;
    AeBuffer            *pRxBuffer;
    AeBool              bError;
};

#ifdef __cplusplus
extern "C" {
#endif

AeRemoteHTTPTransport *AeRemoteHTTPTransportNew(AeRemoteSession *pSession);

#ifdef __cplusplus
}
#endif

#endif

#endif

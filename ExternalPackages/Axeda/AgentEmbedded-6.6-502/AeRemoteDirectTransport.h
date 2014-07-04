/* $Id: AeRemoteDirectTransport.h,v 1.1.4.1 2010/08/13 16:01:38 dkhodos Exp $ */

#ifndef _AE_REMOTE_DIRECT_TRANSPORT_H_
#define _AE_REMOTE_DIRECT_TRANSPORT_H_

#if defined(ENABLE_REMOTE_SESSION)

typedef struct _AeRemoteDirectTransport AeRemoteDirectTransport;

/* Remote session transport mechanism */
struct _AeRemoteDirectTransport
{
    AeRemoteTransport           parent;
    AeRemoteDirectConnection    *pDC;
};

#ifdef __cplusplus
extern "C" {
#endif

AeRemoteDirectTransport *AeRemoteDirectTransportNew(AeRemoteSession *pSession);

#ifdef __cplusplus
}
#endif

#endif

#endif

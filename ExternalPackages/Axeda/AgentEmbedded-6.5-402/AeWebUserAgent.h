/* $Id: AeWebUserAgent.h,v 1.9 2008/05/21 18:24:35 dkhodos Exp $ */

#ifndef _AE_WEB_USER_AGENT_H_
#define _AE_WEB_USER_AGENT_H_

struct _AeWebUserAgent
{
    AeList  *pConnections;
    AeList  *pTransactions;
};

#ifdef __cplusplus
extern "C" {
#endif

AeWebUserAgent  *AeWebUserAgentNew(void);
void            AeWebUserAgentDestroy(AeWebUserAgent *pUA);
AeError         AeWebUserAgentSyncExecute(AeWebUserAgent *pUA, AeWebRequest **ppRequests, AeInt iCount);
AeError         AeWebUserAgentAsyncExecute(AeWebUserAgent *pUA, AeWebRequest **ppRequests, AeInt iCount);
void            AeWebUserAgentCancel(AeWebUserAgent *pUA, AeWebRequest *pRequest);
AeError         AeWebUserAgentDoEvents(AeWebUserAgent *pUA, AeTimeValue *pTimeOut);
AeBool          AeWebUserAgentEventsPending(AeWebUserAgent *pUA);
AeWebConnection *AeWebUserAgentGetIdleConnection(AeWebUserAgent *pUA, AeWebTask *pTask, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort, AeTimeValue *pTimeOut);
AeWebConnection *AeWebUserAgentGetNewConnection(AeWebUserAgent *pUA, AeWebTask *pTask, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort, AeTimeValue *pTimeOut);
AeWebConnection *AeWebUserAgentGetNewDirectConnection(AeWebUserAgent *pUA, AeWebTask *pTask, AeChar *pHost, AeUInt16 iPort, AeTimeValue *pTimeOut);

#ifdef __cplusplus
}
#endif

#endif

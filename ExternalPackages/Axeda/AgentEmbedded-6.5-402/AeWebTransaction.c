/* $Id: AeWebTransaction.c,v 1.11.2.6 2010/08/13 16:01:39 dkhodos Exp $ */

#include "Ae.h"

static AeError  AeWebTransactionMakeRequestHead(AeWebTransaction *pTran);
static AeError  AeWebTransactionParseResponseHead(AeWebTransaction *pTran, AeBool *pbEOH);
static AeError  AeWebTransactionProcessResponseHead(AeWebTransaction *pTran, AeWebTransactionResult *piResult);
static void     AeWebTransactionUnbindConnection(AeWebTransaction *pTran);
static void     AeWebTransactionClearResponseInfo(AeWebTransaction *pTran);
static AeError  AeWebTransactionDecodeResponseEntity(AeWebTransaction *pTran, AeBool *pbEOB, AeBool *pbGoAhead);
static AeBool   AeWebTransactionMaySendEntity(AeWebTransaction *pTran);
static void     AeWebTransactionEncodeRequestEntity(AeWebTransaction *pTran);
static void     AeWebTransactionWriteRequestLine(AeWebTransaction *pTran);
static void     AeWebTransactionNTLMReset(AeWebTransaction *pTran);
static AeWebProxyInfo *AeWebTransactionGetProxyInfo(AeWebTransaction *pTran);
static AeInt32 AeWebTransactionGetBufferSize(AeWebTransaction *pTran);

static AeWebTaskVTable g_aeWebTransactionVTable =
{
    (void (*)(AeWebTask *))             AeWebTransactionDestroy,
    (AeBool (*)(AeWebTask *))           AeWebTransactionIsDone,
    (AeChar* (*)(AeWebTask *))          AeWebTransactionGetHost,
    (AeError (*)(AeWebTask *, AeBool))  AeWebTransactionOnConnected,
    (AeError (*)(AeWebTask *))          AeWebTransactionOnDisconnected,
    (AeError (*)(AeWebTask *, AeInt32)) AeWebTransactionOnRx,
    (AeError (*)(AeWebTask *, AeInt32)) AeWebTransactionOnTx,
    (void (*)(AeWebTask *, AeError))    AeWebTransactionOnIOError,
    (AeBool (*)(AeWebTask *))           AeWebTransactionOnConnectionReadable,
    (AeBool (*)(AeWebTask *))           AeWebTransactionOnConnectionWritable
};

/******************************************************************************/
AeWebTransaction *AeWebTransactionNew(AeWebUserAgent *pUserAgent, AeWebRequest *pRequest)
{
    AeWebTransaction *pTran;

    pTran = (AeWebTransaction *) AeAlloc(sizeof(AeWebTransaction));
    if (!pTran)
        return NULL;
    memset(pTran, 0, sizeof(AeWebTransaction));

    pTran->pVTable = &g_aeWebTransactionVTable;
    
    pTran->pRequestData = AeBufferNew(NULL, 0, AE_BUFFER_CHUNK);
    if (!pTran->pRequestData)
        return NULL;

    pTran->pResponseData = AeBufferNew(NULL, 0, AE_BUFFER_CHUNK);
    if (!pTran->pResponseData)
        return NULL;

    pTran->pRequestHeaders = AeWebHeadersNew();
    if (!pTran->pRequestHeaders)
        return NULL;

    pTran->pResponseHeaders = AeWebHeadersNew();
    if (!pTran->pResponseHeaders)
        return NULL;

    pTran->pUserAgent = pUserAgent;
    pTran->pRequest = pRequest;
    pTran->pConnection = NULL;
    pTran->iState = AeWebTransactionDormant;
    pTran->pResponseVersion = NULL;
    pTran->iResponseStatus = 0;
    pTran->iContentLength = -1;

    pTran->iResponseEntitySize = 0;
    pTran->iResponseChunkSize = 0;
    pTran->bResponsePersistent = AeFalse;
    pTran->bResponseChunked = AeFalse;
    pTran->bSkipBody = AeFalse;
    pTran->bRestart = AeFalse;
    pTran->bReusedConnection = AeFalse;

    pTran->pNTLMParams = NULL;

    return pTran;
}

/******************************************************************************/
void AeWebTransactionDestroy(AeWebTransaction *pTran)
{
    if (pTran->pRequestData)
        AeBufferDestroy(pTran->pRequestData);
    if (pTran->pResponseData)
        AeBufferDestroy(pTran->pResponseData);
    if (pTran->pRequestHeaders)
        AeWebHeadersDestroy(pTran->pRequestHeaders);
    if (pTran->pResponseHeaders)
        AeWebHeadersDestroy(pTran->pResponseHeaders);
    if (pTran->pResponseVersion)
        AeFree(pTran->pResponseVersion);
    AeWebTransactionNTLMReset(pTran);

    AeFree(pTran);
}

/******************************************************************************/
AeError AeWebTransactionInitiate(AeWebTransaction *pTran)
{
    AeError rc;

    /* query idle connection from user agent */
    if (!pTran->pConnection)
    {
		AeWebProxyInfo *pProxyInfo = AeWebTransactionGetProxyInfo(pTran);
		if(pProxyInfo)
				pTran->pConnection = AeWebUserAgentGetIdleConnection(pTran->pUserAgent,
					(AeWebTask *) pTran, pProxyInfo->pHost, pProxyInfo->iPort,
					pTran->pRequest->pHost, pTran->pRequest->iPort, &pTran->pRequest->timeOut);
        else
            pTran->pConnection = AeWebUserAgentGetIdleConnection(pTran->pUserAgent,
                (AeWebTask *) pTran, pTran->pRequest->pHost, pTran->pRequest->iPort,
                pTran->pRequest->pHost, pTran->pRequest->iPort, &pTran->pRequest->timeOut);

        if (!pTran->pConnection)
            return AeEMemory;
    }

    pTran->iState = AeWebTransactionInitiated;

    /* try to connect or reuse connection */
    if (AeWebConnectionIsConnectedV(pTran->pConnection))
        rc = AeWebTransactionOnConnected(pTran, AeTrue);
    else
        rc = AeWebConnectionConnectV(pTran->pConnection);

    return rc;
}

/******************************************************************************/
AeBool AeWebTransactionIsDone(AeWebTransaction *pTran)
{
    return (pTran->iState == AeWebTransactionCompleted);
}

/******************************************************************************/
AeChar *AeWebTransactionGetHost(AeWebTransaction *pTran)
{
    return pTran->pRequest->pHost;
}

/******************************************************************************/
AeBool AeWebTransactionIsEqual(AeWebTransaction *pTran, AeWebRequest *pRequest)
{
    return (pTran->pRequest == pRequest);
}

/******************************************************************************/
AeError AeWebTransactionOnConnected(AeWebTransaction *pTran, AeBool bReused)
{
    AeChar *pBuffer;
    AeInt32 iBufferSize;
    AeError rc;

    rc = AeEOK;
    if (pTran->iState == AeWebTransactionInitiated)
    {
        pTran->bReusedConnection = bReused;

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
        if (!AeWebTransactionGetProxyInfo(pTran) && pTran->pRequest->bSecure && !pTran->pConnection->bSSLEnabled)
            rc = AeWebConnectionEnableSSL(pTran->pConnection);
#endif

        if (rc == AeEOK)
        {
            pTran->iState = AeWebTransactionConnected;
            AeBufferReset(pTran->pRequestData);
            rc = AeWebTransactionMakeRequestHead(pTran);
            if (rc == AeEOK)
            {
                AeBufferGetReadArea(pTran->pRequestData, &pBuffer, &iBufferSize);
                rc = AeWebConnectionSend(pTran->pConnection, pBuffer, iBufferSize, AeTrue);
            }
        }
    }
    else
        rc = AeEInternal;

    return rc;
}

/******************************************************************************/
AeError AeWebTransactionOnDisconnected(AeWebTransaction *pTran)
{
    AeError rc;

    rc = AeEOK;
    if (pTran->iState == AeWebTransactionHeadReceived && pTran->iContentLength <= 0)
    {
        pTran->iState = AeWebTransactionCompleted;
        if (pTran->pRequest->pOnCompleted)
            (*pTran->pRequest->pOnCompleted)(pTran->pRequest);
    }
    else if (pTran->bReusedConnection &&
             (pTran->iState == AeWebTransactionConnected || pTran->iState == AeWebTransactionHeadSent) &&
             !pTran->bRecoverFromAsynchClose)
    {
        pTran->bRecoverFromAsynchClose = AeTrue;
        pTran->pConnection = NULL;
        AeWebTransactionNTLMReset(pTran);
        rc = AeWebTransactionInitiate(pTran);
    }
    else
        rc = AeENetConnLost;

    return rc;
}

/******************************************************************************/
AeError AeWebTransactionOnRx(AeWebTransaction *pTran, AeInt32 iOctets)
{
    AeChar *pBuffer;
    AeInt32 iBufferSize;
    AeBool bGoAhead;
    AeError rc;

    rc = AeEOK;
    AeBufferSeekWritePos(pTran->pResponseData, iOctets, AeTrue);

    if (pTran->iState == AeWebTransactionHeadSent)
    {
        while (1)
        {
            AeBool bEOH;

            rc = AeWebTransactionParseResponseHead(pTran, &bEOH);
            if (rc == AeEOK && bEOH)
            {
                AeWebTransactionResult iResult;

                pTran->iState = AeWebTransactionHeadReceived;

                rc = AeWebTransactionProcessResponseHead(pTran, &iResult);
                if (rc == AeEOK)
                {
                    if (iResult == AeWebTransactionHead)
                    {
                        pTran->iState = AeWebTransactionCompleted;
                        pTran->pRequest->iResponseStatus = pTran->iResponseStatus;

                        bGoAhead = AeTrue;
                        if (pTran->pRequest->pOnResponse)
                            bGoAhead = (*pTran->pRequest->pOnResponse)(pTran->pRequest,
                                pTran->iResponseStatus);
                        if (bGoAhead)
                        {
                            AeWebTransactionUnbindConnection(pTran);
                            if (pTran->pRequest->pOnCompleted)
                                (*pTran->pRequest->pOnCompleted)(pTran->pRequest);
                        }
                        else
                        {
                            AeWebConnectionDisconnectV(pTran->pConnection);
                            pTran->pConnection = NULL;
                        }
                    }
                    else if (iResult == AeWebTransactionEntity && !pTran->bRestart)
                    {
                        pTran->pRequest->iResponseStatus = pTran->iResponseStatus;

                        bGoAhead = AeTrue;
                        if (pTran->pRequest->pOnResponse)
                            bGoAhead = (*pTran->pRequest->pOnResponse)(pTran->pRequest,
                                pTran->iResponseStatus);
                        if (!bGoAhead)
                        {
                            pTran->iState = AeWebTransactionCompleted;

                            AeWebConnectionDisconnectV(pTran->pConnection);
                            pTran->pConnection = NULL;
                        }
                    }
                    else if (iResult == AeWebTransactionAgain)
                    {
                        AeWebTransactionUnbindConnection(pTran);
                        rc = AeWebTransactionInitiate(pTran);
                    }
                    else if (iResult == AeWebTransactionContinue)
                    {
                        pTran->iState = AeWebTransactionHeadSent;
                        AeWebTransactionClearResponseInfo(pTran);
                        continue;
                    }
                    else if (iResult == AeWebTransactionTunnel)
                    {
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
                        rc = AeWebConnectionEnableSSL(pTran->pConnection);
#endif
                        if (rc == AeEOK)
                        {
                            AeWebTransactionNTLMReset(pTran);
                            rc = AeWebTransactionInitiate(pTran);
                        }
                    }
                }
            }
            break;
        }
    }
    else if (pTran->iState != AeWebTransactionHeadReceived)
        rc = AeEInternal;

    if (rc == AeEOK)
    {
        if (pTran->iState == AeWebTransactionHeadReceived)
        {
            AeBool bEOB;

            rc = AeWebTransactionDecodeResponseEntity(pTran, &bEOB, &bGoAhead);
            if ((rc == AeEOK && bEOB) || !bGoAhead)
            {
                pTran->iState = AeWebTransactionCompleted;
                if (bGoAhead)
                {
                    AeWebTransactionUnbindConnection(pTran);
                    if (pTran->bRestart)
                        AeWebTransactionInitiate(pTran);
                    else
                    {
                        if (pTran->pRequest->pOnCompleted)
                            (*pTran->pRequest->pOnCompleted)(pTran->pRequest);
                    }
                }
                else
                {
                    AeWebConnectionDisconnectV(pTran->pConnection);
                    pTran->pConnection = NULL;
                }
            }
        }

        if (rc == AeEOK &&
            (pTran->iState == AeWebTransactionHeadSent ||
             pTran->iState == AeWebTransactionHeadReceived))
        {
            /* expand response buffer if exhausted */
            AeBufferGetWriteArea(pTran->pResponseData, &pBuffer, &iBufferSize);
            if (iBufferSize == 0)
            {
                AeBufferExpand(pTran->pResponseData, AeWebTransactionGetBufferSize(pTran) - 1);
                AeBufferGetWriteArea(pTran->pResponseData, &pBuffer, &iBufferSize);
            }

            rc = AeWebConnectionReceive(pTran->pConnection, pBuffer, iBufferSize, AeFalse);
        }
    }

    return rc;
}

/******************************************************************************/
AeError AeWebTransactionOnTx(AeWebTransaction *pTran, AeInt32 iOctets)
{
    AeChar *pBuffer;
    AeInt32 iBufferSize;
    AeError rc;

    rc = AeEOK;
    AeBufferSeekReadPos(pTran->pRequestData, iOctets, AeTrue);

    if (pTran->iState == AeWebTransactionConnected)
    {
        pTran->iState = AeWebTransactionHeadSent;

        AeWebTransactionClearResponseInfo(pTran);

        AeBufferReset(pTran->pResponseData);
        AeBufferExpand(pTran->pResponseData, AeWebTransactionGetBufferSize(pTran) - 1);
        AeBufferGetWriteArea(pTran->pResponseData, &pBuffer, &iBufferSize);
        rc = AeWebConnectionReceiveV(pTran->pConnection, pBuffer, iBufferSize, AeFalse);

        if (pTran->pRequest->pEntityData && AeWebTransactionMaySendEntity(pTran) &&
            rc == AeEOK)
        {
            AeWebTransactionEncodeRequestEntity(pTran);
            AeBufferGetReadArea(pTran->pRequestData, &pBuffer, &iBufferSize);
            rc = AeWebConnectionSendV(pTran->pConnection, pBuffer, iBufferSize, AeTrue);
        }
    }
    else if (pTran->iState == AeWebTransactionHeadSent)
        rc = AeEOK;
    else
        rc = AeEInternal;

    return rc;
}

/******************************************************************************/
void AeWebTransactionOnIOError(AeWebTransaction *pTran, AeError iError)
{
    AeError rc;

    if (pTran->bReusedConnection &&
        (pTran->iState == AeWebTransactionConnected || pTran->iState == AeWebTransactionHeadSent) &&
        (iError == AeENetConnReset || iError == AeENetConnAborted || iError == AeENetConnLost) &&
        !pTran->bRecoverFromAsynchClose)
    {
        pTran->bRecoverFromAsynchClose = AeTrue;
        pTran->pConnection = NULL;
        AeWebTransactionNTLMReset(pTran);
        rc = AeWebTransactionInitiate(pTran);
        if (rc == AeEOK)
            return;

        iError = rc;
    }

    if (pTran->pConnection)
    {
        AeWebConnectionDisconnectV(pTran->pConnection);
        pTran->pConnection = NULL;
    }

    if (pTran->iState != AeWebTransactionCompleted)
    {
        pTran->pRequest->iError = iError;
        if (pTran->pRequest->pOnError)
            (*pTran->pRequest->pOnError)(pTran->pRequest, iError);
    }

    pTran->iState = AeWebTransactionCompleted;
}

/******************************************************************************/
AeBool AeWebTransactionOnConnectionReadable(AeWebTransaction *pTran)
{
    return AeTrue;
}

/******************************************************************************/
AeBool AeWebTransactionOnConnectionWritable(AeWebTransaction *pTran)
{
    return AeTrue;
}

/******************************************************************************/
static AeError AeWebTransactionMakeRequestHead(AeWebTransaction *pTran)
{
    AeWebAuthScheme iAuthScheme;
    AePointer pAuthParams;
    AeWebHeaders *pUserHeaders;

    AeWebHeadersRemoveAll(pTran->pRequestHeaders);

    /* set Host: header */
    if (!strcmp(pTran->pRequest->pVersion, HTTP_VERSION_11))
        AeWebHeadersSet(pTran->pRequestHeaders, HTTP_HEADER_HOST,
            pTran->pRequest->pHost);

    /* retrieve auth info for origin host */
    if (AeWebAuthInfoGet(pTran->pRequest->pHost, pTran->pRequest->iPort,
        pTran->pRequest->pAbsPath, &iAuthScheme, &pAuthParams) &&
        iAuthScheme != AeWebAuthNone)
    {
        /* make authorization header */
        AeWebAuthMakeRequestHeaders(pTran->pRequestHeaders, AeFalse, iAuthScheme, pAuthParams,
            pTran->pRequest, pTran->pRequest->pMethod, pTran->pRequest->pUser,
            pTran->pRequest->pPassword);
    }

    /* retrieve auth info for proxy */
    if (AeWebTransactionGetProxyInfo(pTran) &&
			AeWebAuthInfoGet(AeWebTransactionGetProxyInfo(pTran)->pHost, 
				AeWebTransactionGetProxyInfo(pTran)->iPort, NULL,
				&iAuthScheme, &pAuthParams) &&
        iAuthScheme != AeWebAuthNone)
    {
        AeChar *pMethod;

        if (iAuthScheme == AeWebAuthNTLM)
        {
            if (!pTran->pNTLMParams)
            {
                // NTLM server authenticates connections, not HTTP requests, hence there is
                // no need to authenticate if [pre-authenticated] connection is reused.
                if (pTran->bReusedConnection)
                    iAuthScheme = AeWebAuthNone;
                else
                {
                    pTran->pNTLMParams = (AeWebAuthParamsNTLM *) AeAlloc(sizeof(AeWebAuthParamsNTLM));
                    if (!pTran->pNTLMParams)
                        return AeEMemory;
                    memset(pTran->pNTLMParams, 0, sizeof(AeWebAuthParamsNTLM));
                }
            }
            pAuthParams = pTran->pNTLMParams;
        }

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
        if (AeWebTransactionGetProxyInfo(pTran) && pTran->pRequest->bSecure && !pTran->pConnection->bSSLEnabled)
            pMethod = HTTP_METHOD_CONNECT;
        else
#endif
            pMethod = pTran->pRequest->pMethod;

        /* make proxy authorization header */
        AeWebAuthMakeRequestHeaders(pTran->pRequestHeaders, AeTrue, iAuthScheme, pAuthParams,
            pTran->pRequest, pMethod, AeWebTransactionGetProxyInfo(pTran)->pUser,
            AeWebTransactionGetProxyInfo(pTran)->pPassword);
    }

    /* set content headers */
    if (pTran->pRequest->pEntityData)
    {
        if (AeWebTransactionMaySendEntity(pTran))
        {
            AeChar pLength[16];

            if (pTran->pRequest->pContentType)
                AeWebHeadersSet(pTran->pRequestHeaders, HTTP_HEADER_CONTENTTYPE,
                    pTran->pRequest->pContentType);
            else
                AeWebHeadersSet(pTran->pRequestHeaders, HTTP_HEADER_CONTENTTYPE,
                    MIME_DEFAULT);

            snprintf(pLength, sizeof(pLength), "%ld", pTran->pRequest->iEntitySize);
            AeWebHeadersSet(pTran->pRequestHeaders, HTTP_HEADER_CONTENTLENGTH, pLength);
        }
        else
            AeWebHeadersSet(pTran->pRequestHeaders, HTTP_HEADER_CONTENTLENGTH, "0");
    }

    /* set Connection: header */
    if (pTran->pRequest->bPersistent)
    {
        if (!pTran->pRequest->bStrict)
            AeWebHeadersSet(pTran->pRequestHeaders, HTTP_HEADER_CONNECTION,
                "Keep-Alive");
    }
    else if (!strcmp(pTran->pRequest->pVersion, HTTP_VERSION_11))
    {
        if (!pTran->pNTLMParams || (pTran->pNTLMParams && pTran->pNTLMParams->pNonce))
            AeWebHeadersSet(pTran->pRequestHeaders, HTTP_HEADER_CONNECTION, "close");
    }

    if (pTran->pNTLMParams && !pTran->pNTLMParams->pNonce)
        AeWebHeadersSet(pTran->pRequestHeaders, HTTP_HEADER_PROXY_CONNECTION, "Keep-Alive");
    
    /* format request line */
    AeWebTransactionWriteRequestLine(pTran);

    /* format request headers */
    AeWebHeadersWrite(pTran->pRequestHeaders, pTran->pRequestData);

    /* format user-provided request headers */
    pUserHeaders = (AeWebHeaders *) pTran->pRequest->pRequestHeaders;
    if (pUserHeaders)
        AeWebHeadersWrite(pUserHeaders, pTran->pRequestData);

    AeBufferWrite(pTran->pRequestData, CRLF, strlen(CRLF));

    return AeEOK;
}

/******************************************************************************/
static void AeWebTransactionWriteRequestLine(AeWebTransaction *pTran)
{
    AeChar *pMethod, *pScheme, *pHost, *pAbsPath, *pProto, pPort[16], *pTmp;
    AeInt iLength;

    pProto = HTTP;
    
    if (!AeWebTransactionGetProxyInfo(pTran)
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
        || (AeWebTransactionGetProxyInfo(pTran) && pTran->pRequest->bSecure && pTran->pConnection->bSSLEnabled)
#endif
        )
    {
        pMethod = pTran->pRequest->pMethod;
        pScheme = "";
        pHost = "";
        *pPort = '\0';
        pAbsPath = pTran->pRequest->pAbsPath;
    }
    else if (AeWebTransactionGetProxyInfo(pTran)
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
        && !pTran->pRequest->bSecure
#endif
        )
    {
        pMethod = pTran->pRequest->pMethod;
        pScheme = "http://";
        pHost = pTran->pRequest->pHost;
        snprintf(pPort, sizeof(pPort), ":%d", pTran->pRequest->iPort);
        pAbsPath = pTran->pRequest->pAbsPath;
    }
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    else
    {
        pMethod = HTTP_METHOD_CONNECT;
        pScheme = "";
        pHost = pTran->pRequest->pHost;
        snprintf(pPort, sizeof(pPort), ":%d", pTran->pRequest->iPort);
        pAbsPath = "";
    }
#endif

    iLength = strlen(pMethod) + 1 +
        strlen(pScheme) +
        strlen(pHost) +
        strlen(pPort) +
        strlen(pAbsPath) + 1 +
        strlen(pProto) + 1 +
        strlen(pTran->pRequest->pVersion) + 1;
    pTmp = (AeChar *) AeAlloc(iLength);
    if (!pTmp)
        return;
    
    snprintf(pTmp, iLength, "%s %s%s%s%s %s/%s",
        pMethod,
        pScheme,
        pHost,
        pPort,
        pAbsPath,
        pProto,
        pTran->pRequest->pVersion);

    AeBufferWriteLn(pTran->pRequestData, pTmp, strlen(pTmp));

    AeFree(pTmp);
}

/******************************************************************************/
static void AeWebTransactionEncodeRequestEntity(AeWebTransaction *pTran)
{
    AeBufferWrite(pTran->pRequestData, pTran->pRequest->pEntityData,
        pTran->pRequest->iEntitySize);
}

/******************************************************************************/
static AeError AeWebTransactionParseResponseHead(AeWebTransaction *pTran, AeBool *pbEOH)
{
    AeChar *pLine, *pZLine;
    AeInt32 iLength;
    AeError rc;

    rc = AeEOK;
    *pbEOH = AeFalse;

    do
    {
        iLength = AeBufferReadLn(pTran->pResponseData, &pLine, -1);
        if (!pLine)
            break;

        if (iLength == 0)
        {
            if (pTran->iResponseStatus == 0)
                rc = AeEWebBadResponse;
            else
                *pbEOH = AeTrue;
            break;
        }

        pZLine = (AeChar *) AeAlloc(iLength + 1);
        if (!pZLine)
            return AeEMemory;

        memcpy(pZLine, pLine, iLength);
        pZLine[iLength] = '\0';

        if (pTran->iResponseStatus == 0)
        {
            AeChar pProto[sizeof(HTTP) + 1], pVersion[4];
            AeInt iFields;

            iFields = sscanf(pZLine, "%4s/%3s %3d", pProto, pVersion,
                &pTran->iResponseStatus);
            if (iFields == 3)
            {
                if (strcasecmp(pProto, HTTP))
                    rc = AeEWebBadResponse;

                AeSetString(&pTran->pResponseVersion, pVersion, -1);
            }
            else
                rc = AeEWebBadResponse;
        }
        else
        {
            AeChar *pValue;
            
            pValue = strchr(pZLine, ':');
            if (pValue)
            {
                *pValue = '\0';
                pValue += 1;
                pValue += strspn(pValue, " \t");
            }

            if (pValue)
            {
                AeWebHeadersSet(pTran->pResponseHeaders, pZLine, pValue);
                AeWebRequestSetResponseHeader(pTran->pRequest, pZLine, pValue);
            }
            else
                rc = AeEWebBadResponse;
        }

        AeFree(pZLine);
    }
    while (rc == AeEOK);

    return rc;
}

/******************************************************************************/
static AeError AeWebTransactionProcessResponseHead(AeWebTransaction *pTran, AeWebTransactionResult *piResult)
{
    AeWebAuthScheme iAuthScheme;
    AePointer pAuthParams;
    AeChar *pValue;
    AeBool bAuthInfo;
    AeError rc;

    rc = AeEOK;
    pAuthParams = NULL;

    /* Connection (Proxy-Connection) */
    pValue = AeWebHeadersGet(pTran->pResponseHeaders, HTTP_HEADER_CONNECTION);
    if (!pValue)
        pValue = AeWebHeadersGet(pTran->pResponseHeaders, HTTP_HEADER_PROXY_CONNECTION);
    if (pValue && !strcasecmp(pValue, "close"))
        pTran->bResponsePersistent = AeFalse;
    else if (pValue && !strcasecmp(pValue, "Keep-Alive"))
        pTran->bResponsePersistent = AeTrue;
    else
    {
        if (!strcmp(pTran->pResponseVersion, HTTP_VERSION_11))
            pTran->bResponsePersistent = AeTrue;
        else
            pTran->bResponsePersistent = AeFalse;
    }

    /* Transfer-Encoding */
    pValue = AeWebHeadersGet(pTran->pResponseHeaders, HTTP_HEADER_TRANSFERENCODING);
    if (pValue && !strcasecmp(pValue, "chunked"))
        pTran->bResponseChunked = AeTrue;
    else
        pTran->bResponseChunked = AeFalse;

    /* Content-Length */
    pValue = AeWebHeadersGet(pTran->pResponseHeaders, HTTP_HEADER_CONTENTLENGTH);
    if (pValue)
        pTran->iContentLength = strtol(pValue, NULL, 10);
    else
        pTran->iContentLength = -1;

    *piResult = AeWebTransactionHead;

    /* process response status */
    switch (pTran->iResponseStatus)
    {
        case HTTP_STATUS_OK:
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
            if (AeWebTransactionGetProxyInfo(pTran) && pTran->pRequest->bSecure &&
                !pTran->pConnection->bSSLEnabled)
                *piResult = AeWebTransactionTunnel;
            else
#endif
            {
                if (pTran->iContentLength != 0)
                    *piResult = AeWebTransactionEntity;
            }
            break;
        case HTTP_STATUS_CONTINUE:
            *piResult = AeWebTransactionContinue;
            break;
        case HTTP_STATUS_UNAUTHORIZED:
        case HTTP_STATUS_PROXYAUTHREQ:
            /* check if auth info exists for destination */
            if (pTran->iResponseStatus == HTTP_STATUS_UNAUTHORIZED)
                bAuthInfo = AeWebAuthInfoGet(pTran->pRequest->pHost, pTran->pRequest->iPort,
                    pTran->pRequest->pAbsPath, &iAuthScheme, &pAuthParams);
            else
                bAuthInfo = (AeWebTransactionGetProxyInfo(pTran) &&
                    AeWebAuthInfoGet(AeWebTransactionGetProxyInfo(pTran)->pHost, 
						AeWebTransactionGetProxyInfo(pTran)->iPort, NULL,
                        &iAuthScheme, &pAuthParams));

            /* fail we have already tried to authenticate at the destination */
            if (bAuthInfo && iAuthScheme != AeWebAuthNone)
            {
                if (iAuthScheme == AeWebAuthNTLM && pTran->pNTLMParams &&
                    !pTran->pNTLMParams->pNonce)
                    pAuthParams = pTran->pNTLMParams;
                else
                {
                    rc = AeEWebAuthFailed;
                    break;
                }
            }

            /* read x-Authenticate header */
            if (pTran->iResponseStatus == HTTP_STATUS_UNAUTHORIZED)
                pValue = AeWebHeadersGet(pTran->pResponseHeaders,
                    HTTP_HEADER_WWWAUTHENTICATE);
            else
                pValue = AeWebHeadersGet(pTran->pResponseHeaders,
                    HTTP_HEADER_PROXYAUTHENTICATE);
            if (!pValue)
            {
                rc = AeEWebBadResponse;
                break;
            }

            /* parse authentication challenge */
            if (AeWebAuthParseChallenge(pValue, &iAuthScheme, &pAuthParams))
            {
                if (iAuthScheme == AeWebAuthNTLM)
                {
                    pTran->pNTLMParams = pAuthParams;
                    pAuthParams = NULL;
                }

                /* remember auth info: will be used for future requests */
                if (pTran->iResponseStatus == HTTP_STATUS_UNAUTHORIZED)
                    AeWebAuthInfoSet(pTran->pRequest->pHost, pTran->pRequest->iPort,
                        pTran->pRequest->pAbsPath, iAuthScheme, pAuthParams);
                else
                {
                    if (AeWebTransactionGetProxyInfo(pTran))
						AeWebAuthInfoSet(AeWebTransactionGetProxyInfo(pTran)->pHost, 
							AeWebTransactionGetProxyInfo(pTran)->iPort,
                            NULL, iAuthScheme, pAuthParams);
                    else
                        rc = AeEWebAuthFailed;
                }

                *piResult = AeWebTransactionAgain;
            }
            else
                rc = AeEWebAuthUnsupported;
            break;
        default:
            break;
    }

    // if response bears a body, read it out of socket in any case
    if (pTran->iContentLength != -1 || pTran->bResponseChunked)
    {
        if (*piResult == AeWebTransactionAgain)
            pTran->bRestart = AeTrue;
        if (*piResult != AeWebTransactionEntity)
            pTran->bSkipBody = AeTrue;
        *piResult = AeWebTransactionEntity;
    }

    return rc;
}

/******************************************************************************/
static AeError AeWebTransactionDecodeResponseEntity(AeWebTransaction *pTran, AeBool *pbEOB, AeBool *pbGoAhead)
{
    AeChar *pBuffer;
    AeInt32 iBufferSize;
    AeError rc;

    rc = AeEOK;
    *pbEOB = AeFalse;
    *pbGoAhead = AeTrue;

    if (!pTran->bResponseChunked)
    {
        iBufferSize = AeBufferRead(pTran->pResponseData, &pBuffer, -1);
        if (iBufferSize > 0)
        {
            if (!pTran->bSkipBody)
            {
                if (pTran->pRequest->pOnEntity)
                    *pbGoAhead = pTran->pRequest->pOnEntity(pTran->pRequest,
                        pTran->iResponseEntitySize, pBuffer, iBufferSize);
            }

            pTran->iResponseEntitySize += iBufferSize;
        }

        if (pTran->iContentLength != -1 &&
            pTran->iResponseEntitySize >= pTran->iContentLength)
            *pbEOB = AeTrue;
    }
    else
    {
        while (1)
        {
            /* read trailing CRLF */
            if (pTran->iResponseChunkSize < 0)
            {
                iBufferSize = AeBufferReadLn(pTran->pResponseData, &pBuffer, -1);
                if (!pBuffer)
                    break;

                // finish when the last empty line in the trailer is received
                if (pTran->iResponseChunkSize == -2)
                {
                    if (iBufferSize == 0)
                        *pbEOB = AeTrue;

                    break;
                }

                pTran->iResponseChunkSize = 0;
            }

            /* read chunk size */
            if (pTran->iResponseChunkSize == 0)
            {
                AeChar *pEndPtr;

                iBufferSize = AeBufferReadLn(pTran->pResponseData, &pBuffer, -1);
                if (!pBuffer)
                    break;

                pTran->iResponseChunkSize = strtol(pBuffer, &pEndPtr, 16);
                if (pEndPtr == pBuffer)
                    return AeEWebBadResponse;

                // indicate that the only remaining part is the trailer
                if (pTran->iResponseChunkSize == 0)
                    pTran->iResponseChunkSize = -2;
            }

            /* read chunk */
            if (pTran->iResponseChunkSize > 0)
            {
                iBufferSize = AeBufferRead(pTran->pResponseData, &pBuffer,
                    pTran->iResponseChunkSize);
                if (iBufferSize == 0)
                    break;

                if (!pTran->bSkipBody)
                {
                    *pbGoAhead = AeTrue;
                    if (pTran->pRequest->pOnEntity)
                        *pbGoAhead = (*pTran->pRequest->pOnEntity)(pTran->pRequest,
                                                                   pTran->iResponseEntitySize,
                                                                   pBuffer, iBufferSize);
                    if (!(*pbGoAhead))
                        break;
                }

                pTran->iResponseEntitySize += iBufferSize;

                pTran->iResponseChunkSize -= iBufferSize;
                
                /* indicate that the only remaining part is trailing CRLF */
                if (pTran->iResponseChunkSize == 0)
                    pTran->iResponseChunkSize = -1;
            }
        }
    }

    return rc;
}

/******************************************************************************/
static void AeWebTransactionClearResponseInfo(AeWebTransaction *pTran)
{
    AeSetString(&pTran->pResponseVersion, NULL, -1);
    pTran->iResponseStatus = 0;
    pTran->bResponsePersistent = AeFalse;
    pTran->bResponseChunked = AeFalse;
    pTran->iContentLength = -1;
    pTran->iResponseEntitySize = 0;
    pTran->iResponseChunkSize = 0;
    pTran->bSkipBody = AeFalse;
    pTran->bRestart = AeFalse;

    AeWebHeadersRemoveAll(pTran->pResponseHeaders);

    if (pTran->pRequest->pResponseHeaders)
        AeWebHeadersRemoveAll((AeWebHeaders *) pTran->pRequest->pResponseHeaders);
}

/******************************************************************************/
static void AeWebTransactionUnbindConnection(AeWebTransaction *pTran)
{
    if (!pTran->bResponsePersistent ||
        ((!pTran->pRequest->bPersistent ||
         (pTran->pRequest->bStrict && !strcmp(pTran->pRequest->pVersion, HTTP_VERSION_10))) &&
         !(pTran->pNTLMParams && pTran->pNTLMParams->pNonce)))
    {
        AeWebConnectionDisconnectV(pTran->pConnection);
        pTran->pConnection = NULL;
    }
}

/******************************************************************************/
static AeBool AeWebTransactionMaySendEntity(AeWebTransaction *pTran)
{
    if ((pTran->pNTLMParams && !pTran->pNTLMParams->pNonce)
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
        || (AeWebTransactionGetProxyInfo(pTran) && pTran->pRequest->bSecure && !pTran->pConnection->bSSLEnabled)
#endif
        )
        return AeFalse;
    else
        return AeTrue;
}

/******************************************************************************/
static void AeWebTransactionNTLMReset(AeWebTransaction *pTran)
{
    if (pTran->pNTLMParams)
    {
        if (pTran->pNTLMParams->pNonce)
            AeFree(pTran->pNTLMParams->pNonce);
        if (pTran->pNTLMParams->pTargetName)
            AeFree(pTran->pNTLMParams->pTargetName);
        AeFree(pTran->pNTLMParams);

        pTran->pNTLMParams = NULL;
    }
}

/******************************************************************************/
static AeWebProxyInfo *AeWebTransactionGetProxyInfo(AeWebTransaction *pTran)
{
	/* Either use per-request HTTPProxy or the global HTTPProxy settings. SOCKS needs global */
    if (pTran->pRequest->pProxyInfo)
	{
		if(pTran->pRequest->pProxyInfo->iProto == AeWebProxyProtoHTTP)
			return pTran->pRequest->pProxyInfo;
		if(pTran->pRequest->pProxyInfo->iProto == AeWebProxyProtoNone)
			return NULL;
	}

    return g_pWebProxyHTTP;
}

/******************************************************************************/
static AeInt32 AeWebTransactionGetBufferSize(AeWebTransaction *pTran)
{
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    if (pTran->pConnection && pTran->pConnection->bSSLEnabled)
        return AE_SSL_BUFFER_SIZE;
#endif

    return AE_BUFFER_CHUNK;
}

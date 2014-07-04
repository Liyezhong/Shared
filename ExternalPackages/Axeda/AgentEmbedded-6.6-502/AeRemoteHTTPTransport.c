/* $Id: AeRemoteHTTPTransport.c,v 1.1.4.1 2010/08/13 16:01:38 dkhodos Exp $ */

#include "Ae.h"

#if defined(ENABLE_REMOTE_SESSION)

static void AeRemoteHTTPTransportDestroy(AeRemoteHTTPTransport *pTransport);
static void AeRemoteHTTPTransportConnect(AeRemoteHTTPTransport *pTransport);
static void AeRemoteHTTPTransportSend(AeRemoteHTTPTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
static void AeRemoteHTTPTransportPost(AeRemoteHTTPTransport *pTransport);
static void AeRemoteHTTPTransportReceive(AeRemoteHTTPTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
static void AeRemoteHTTPTransportProcessRx(AeRemoteHTTPTransport *pTransport);
static void AeRemoteHTTPTransportFlushRxBuffer(AeRemoteHTTPTransport *pTransport);
static void AeRemoteHTTPTransportGet(AeRemoteHTTPTransport *pTransport);
static AeBool AeRemoteHTTPTransportIsConnecting(AeRemoteHTTPTransport *pTransport);
static AeBool AeRemoteHTTPTransportIsConnected(AeRemoteHTTPTransport *pTransport);
static AeBool AeRemoteHTTPTransportIsSending(AeRemoteHTTPTransport *pTransport);
static AeBool AeRemoteHTTPTransportIsError(AeRemoteHTTPTransport *pTransport);

static void AeRemoteHTTPTransportSetServlet(AeChar **pServlet, AeChar *pURL, AeBool bSecure, AeChar *pServer, AeUInt16 iPort);
static void AeRemoteHTTPTransportProcessError(AeRemoteHTTPTransport *pTransport, AeWebRequest *pRequest);

static void AeRemoteHTTPTransportOnError(AeWebRequest *pRequest, AeError iError);
static AeBool AeRemoteHTTPTransportOnEntity(AeWebRequest *pRequest, AeInt32 iDataOffset, AeChar *pData, AeInt32 iSize);
static void AeRemoteHTTPTransportOnCompleted(AeWebRequest *pRequest);

/**************************************************************************/
AeRemoteTransportVTable g_aeRemoteHTTPTransportVTable =
{
    (void (*)(AeRemoteTransport *)) AeRemoteHTTPTransportDestroy,
    (void (*)(AeRemoteTransport *)) AeRemoteHTTPTransportConnect,
    (void (*)(AeRemoteTransport *, AeChar *, AeInt32, AeInt32 *)) AeRemoteHTTPTransportSend,
    (void (*)(AeRemoteTransport *, AeChar *, AeInt32, AeInt32 *)) AeRemoteHTTPTransportReceive,
    (AeBool (*)(AeRemoteTransport *)) AeRemoteHTTPTransportIsConnecting,
    (AeBool (*)(AeRemoteTransport *)) AeRemoteHTTPTransportIsConnected,
    (AeBool (*)(AeRemoteTransport *)) AeRemoteHTTPTransportIsSending,
    (AeBool (*)(AeRemoteTransport *)) AeRemoteHTTPTransportIsError
};

/**************************************************************************/
AeRemoteHTTPTransport *AeRemoteHTTPTransportNew(AeRemoteSession *pSession)
{
    AeRemoteSessionConfig *pConfig = pSession->pConfig;
    AeRemoteHTTPTransport *pTransport;
    AeUInt16 iPort;

    pTransport = (AeRemoteHTTPTransport *) AeAlloc(sizeof(AeRemoteHTTPTransport));
    if (!pTransport)
        return NULL;
    memset(pTransport, 0, sizeof(AeRemoteHTTPTransport));

    AeRemoteTransportInitialize((AeRemoteTransport *) &pTransport->parent, "HTTP", pSession);
    pTransport->parent.pVTable = &g_aeRemoteHTTPTransportVTable;

    pTransport->pRxBuffer = AeBufferNew(NULL, 0, AE_BUFFER_CHUNK);
    if (!pTransport->pRxBuffer)
    {
        AeRemoteHTTPTransportDestroy(pTransport);
        return NULL;
    }

    iPort = pConfig->bSecure ? pConfig->iSSLPort : pConfig->iPlainPort;
    AeRemoteHTTPTransportSetServlet(&pTransport->pPostURL, pConfig->pPostURL, pConfig->bSecure,
        pConfig->pServer, iPort);
    AeRemoteHTTPTransportSetServlet(&pTransport->pGetURL, pConfig->pGetURL, pConfig->bSecure,
        pConfig->pServer, iPort);

    return pTransport;
}

/**************************************************************************/
void AeRemoteHTTPTransportDestroy(AeRemoteHTTPTransport *pTransport)
{
    if (pTransport->pGetRequest)
        AeWebRequestSetUserData(pTransport->pGetRequest, NULL);
    if (pTransport->pPostRequest)
        AeWebRequestSetUserData(pTransport->pPostRequest, NULL);

    if (pTransport->pPostURL)
        AeFree(pTransport->pPostURL);
    if (pTransport->pGetURL)
        AeFree(pTransport->pGetURL);
    if (pTransport->pRxBuffer)
        AeBufferDestroy(pTransport->pRxBuffer);

    AeFree(pTransport);
}

/**************************************************************************/
static void AeRemoteHTTPTransportConnect(AeRemoteHTTPTransport *pTransport)
{
}

/**************************************************************************/
static void AeRemoteHTTPTransportSend(AeRemoteHTTPTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft)
{
    pTransport->pTxPtr = pData;
    pTransport->piTxLeft = piBytesLeft;
    *pTransport->piTxLeft = iSize;

    AeRemoteHTTPTransportPost(pTransport);
}

/**************************************************************************/
static void AeRemoteHTTPTransportPost(AeRemoteHTTPTransport *pTransport)
{
    pTransport->pPostRequest = AeWebRequestNew();
    if (!pTransport->pPostRequest)
    {
        pTransport->bError = AeTrue;
        return;
    }

    AeWebRequestSetMethod(pTransport->pPostRequest, HTTP_METHOD_POST);
    AeWebRequestSetURL(pTransport->pPostRequest, pTransport->pPostURL);
    AeWebRequestSetPersistent(pTransport->pPostRequest, AeTrue);
    AeWebRequestSetEntityData(pTransport->pPostRequest, pTransport->pTxPtr);
    AeWebRequestSetEntitySize(pTransport->pPostRequest, *pTransport->piTxLeft);
    AeWebRequestSetUserData(pTransport->pPostRequest, pTransport);

    AeWebRequestSetOnError(pTransport->pPostRequest, AeRemoteHTTPTransportOnError);
    AeWebRequestSetOnEntity(pTransport->pPostRequest, NULL);
    AeWebRequestSetOnCompleted(pTransport->pPostRequest, AeRemoteHTTPTransportOnCompleted);

    if (AeWebUserAgentAsyncExecute(g_pWebUA, &pTransport->pPostRequest, 1) != AeEOK)
    {
        AeWebRequestDestroy(pTransport->pPostRequest);
        pTransport->pPostRequest = NULL;
        pTransport->bError = AeTrue;
    }
}

/**************************************************************************/
static void AeRemoteHTTPTransportReceive(AeRemoteHTTPTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft)
{
    pTransport->pRxPtr = pData;
    pTransport->piRxLeft = piBytesLeft;
    *pTransport->piRxLeft = iSize;

    AeRemoteHTTPTransportProcessRx(pTransport);
}

/**************************************************************************/
static void AeRemoteHTTPTransportProcessRx(AeRemoteHTTPTransport *pTransport)
{
    if (!pTransport->piRxLeft)
        return;

    AeRemoteHTTPTransportFlushRxBuffer(pTransport);

    if (*pTransport->piRxLeft > 0)
    {
        if (pTransport->pGetRequest == NULL)
            AeRemoteHTTPTransportGet(pTransport);
    }
    else
    {
        pTransport->pRxPtr = NULL;
        pTransport->piRxLeft = NULL;
    }
}

/**************************************************************************/
static void AeRemoteHTTPTransportFlushRxBuffer(AeRemoteHTTPTransport *pTransport)
{
    AeChar *pBufferedData;
    AeInt32 iBufferedSize;

    AeBufferGetReadArea(pTransport->pRxBuffer, &pBufferedData, &iBufferedSize);
    if (iBufferedSize > 0 && *pTransport->piRxLeft > 0)
    {
        AeInt32 iRxSize = AE_MIN(*pTransport->piRxLeft, iBufferedSize);
        memcpy(pTransport->pRxPtr, pBufferedData, iRxSize);
        pTransport->pRxPtr += iRxSize;
        *pTransport->piRxLeft -= iRxSize;
        AeBufferSeekReadPos(pTransport->pRxBuffer, iRxSize, AeTrue);
    }
}

/**************************************************************************/
static void AeRemoteHTTPTransportGet(AeRemoteHTTPTransport *pTransport)
{
    pTransport->pGetRequest = AeWebRequestNew();
    if (!pTransport->pGetRequest)
    {
        pTransport->bError = AeTrue;
        return;
    }

    AeWebRequestSetMethod(pTransport->pGetRequest, HTTP_METHOD_GET);
    AeWebRequestSetURL(pTransport->pGetRequest, pTransport->pGetURL);
    AeWebRequestSetPersistent(pTransport->pGetRequest, AeTrue);
    AeWebRequestSetUserData(pTransport->pGetRequest, pTransport);

    AeWebRequestSetOnError(pTransport->pGetRequest, AeRemoteHTTPTransportOnError);
    AeWebRequestSetOnEntity(pTransport->pGetRequest, AeRemoteHTTPTransportOnEntity);
    AeWebRequestSetOnCompleted(pTransport->pGetRequest, AeRemoteHTTPTransportOnCompleted);

    if (AeWebUserAgentAsyncExecute(g_pWebUA, &pTransport->pGetRequest, 1) != AeEOK)
    {
        AeWebRequestDestroy(pTransport->pGetRequest);
        pTransport->pGetRequest = NULL;
        pTransport->bError = AeTrue;
    }
}

/**************************************************************************/
static AeBool AeRemoteHTTPTransportIsConnecting(AeRemoteHTTPTransport *pTransport)
{
    return AeFalse;
}

/**************************************************************************/
static AeBool AeRemoteHTTPTransportIsConnected(AeRemoteHTTPTransport *pTransport)
{
    return AeTrue;
}

/**************************************************************************/
static AeBool AeRemoteHTTPTransportIsSending(AeRemoteHTTPTransport *pTransport)
{
    if (pTransport->piTxLeft && *pTransport->piTxLeft > 0)
        return AeTrue;

    return AeFalse;
}

/**************************************************************************/
static AeBool AeRemoteHTTPTransportIsError(AeRemoteHTTPTransport *pTransport)
{
    return pTransport->bError;
}

/**************************************************************************/
static void AeRemoteHTTPTransportSetServlet(AeChar **pServlet, AeChar *pURL, AeBool bSecure, AeChar *pServer, AeUInt16 iPort)
{
    AeURL *pTmpURL;

    pTmpURL = AeURLNew();
    if (!pTmpURL)
        return;

    /* start with the specified URL */
    if (!AeURLSet(pTmpURL, pURL))
    {
        AeURLDestroy(pTmpURL);
        return;
    }

    /* set HTTP/HTTPS flag */
    AeURLSetSecure(pTmpURL, bSecure);
    
    /* use server hostname when specified */
    if (pServer)
        AeURLSetHost(pTmpURL, pServer);

    /* set port */
    AeURLSetPort(pTmpURL, iPort);

    AeSetString(pServlet, AeURLGet(pTmpURL), -1);

    AeURLDestroy(pTmpURL);
}

/**************************************************************************/
static void AeRemoteHTTPTransportProcessError(AeRemoteHTTPTransport *pTransport, AeWebRequest *pRequest)
{
    if (pRequest == pTransport->pPostRequest)
    {
        pTransport->pPostRequest = NULL;
        pTransport->pTxPtr = NULL;
        pTransport->piTxLeft = NULL;
    }

    if (pRequest == pTransport->pGetRequest)
    {
        pTransport->pGetRequest = NULL;
        pTransport->pRxPtr = NULL;
        pTransport->piRxLeft = NULL;
    }

    pTransport->bError = AeTrue;
}

/**************************************************************************/
static void AeRemoteHTTPTransportOnError(AeWebRequest *pRequest, AeError iError)
{
    AeRemoteHTTPTransport *pTransport = (AeRemoteHTTPTransport *) pRequest->pUserData;
    if (pTransport)
    {
        pTransport->bError = AeTrue;

        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_REMOTE_SESSION,
                      "Remote session (id=%s): HTTP %s error: %s",
                      pTransport->parent.pSession->pId, pRequest->pMethod,
                      AE_ERROR_STRING(iError));

        AeRemoteHTTPTransportProcessError(pTransport, pRequest);
    }

    AeWebRequestDestroy(pRequest);
}

/**************************************************************************/
static AeBool AeRemoteHTTPTransportOnEntity(AeWebRequest *pRequest, AeInt32 iDataOffset, AeChar *pData, AeInt32 iSize)
{
    AeRemoteHTTPTransport *pTransport = (AeRemoteHTTPTransport *) pRequest->pUserData;
    if (pTransport)
    {
        AeBufferWrite(pTransport->pRxBuffer, pData, iSize);
        AeRemoteHTTPTransportProcessRx(pTransport);
    }

    return AeTrue;
}

/**************************************************************************/
static void AeRemoteHTTPTransportOnCompleted(AeWebRequest *pRequest)
{
    AeRemoteHTTPTransport *pTransport = (AeRemoteHTTPTransport *) pRequest->pUserData;
    if (pTransport)
    {
        if (pRequest->iResponseStatus != HTTP_STATUS_OK)
        {
            if (g_drmSettings.iLogLevel >= AeLogError)
                AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_REMOTE_SESSION,
                          "Remote session (id=%s): HTTP %s status: %d",
                          pTransport->parent.pSession->pId, pRequest->pMethod,
                          pRequest->iResponseStatus);

            AeRemoteHTTPTransportProcessError(pTransport, pRequest);
        }
        else
        {
            if (pRequest == pTransport->pPostRequest)
            {
                pTransport->pPostRequest = NULL;
                pTransport->pTxPtr = NULL;
                *pTransport->piTxLeft = 0;
                pTransport->piTxLeft = NULL;
            }
            else
            {
                pTransport->pGetRequest = NULL;
                AeRemoteHTTPTransportProcessRx(pTransport);
            }
        }
    }

    AeWebRequestDestroy(pRequest);
}

#endif

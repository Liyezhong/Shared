/* $Id: AeWebConnection.c,v 1.12.2.1 2010/08/13 16:01:39 dkhodos Exp $ */

#include "Ae.h"

static AeError  AeWebConnectionUpdateRxStatus(AeWebConnection *pConn, AeInt32 iOctets);
static AeError  AeWebConnectionUpdateTxStatus(AeWebConnection *pConn, AeInt32 iOctets);
static AeError  AeWebConnectionOnConnected(AeWebConnection *pConn, AeTimeValue *pTimeValue);
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
static AeError  AeWebConnectionUpdateSSLStatus(AeWebConnection *pConn);
static AeError  AeWebConnectionHandleSSLError(AeWebConnection *pConn, AeInt32 iRet);
static AeInt    SSLVerifyCallback(AeInt iPreverifyOK, X509_STORE_CTX *pContext);
#endif

static AeWebConnectionVTable g_abWebConnectionVTable =
{
    AeWebConnectionDestroy,
    AeWebConnectionInitialize,
    AeWebConnectionConnect,
    AeWebConnectionDisconnect,
    AeWebConnectionSend,
    AeWebConnectionReceive,
    AeWebConnectionIsEqual,
    AeWebConnectionIsConnected,
    AeWebConnectionOnReadable,
    AeWebConnectionOnWritable
};

/******************************************************************************/
AeWebConnection *AeWebConnectionNew(void)
{
    AeWebConnection *pConn;

    pConn = (AeWebConnection *) AeAlloc(sizeof(AeWebConnection));
    if (!pConn)
        return NULL;
    memset(pConn, 0, sizeof(AeWebConnection));

    AeWebConnectionConstruct(pConn);

    return pConn;
}

/******************************************************************************/
void AeWebConnectionConstruct(AeWebConnection *pConn)
{
    pConn->pVTable = &g_abWebConnectionVTable;
    pConn->pTask = NULL;
    pConn->iState = AeWebConnectionClosed;
    pConn->pHost = NULL;
    pConn->netAddress.iAddress = 0UL;
    pConn->netAddress.iPort = 0;
    pConn->timeOut.iSec = AE_HTTP_TIMEOUT;
    pConn->timeOut.iMicroSec = 0;
    pConn->pOriginHost = NULL;
    pConn->iOriginPort = 0;
    pConn->pRxPtr = NULL;
    pConn->iRxTotal = 0;
    pConn->iRxLeft = 0;
    pConn->bRxWaitAll = AeFalse;
    pConn->pTxPtr = NULL;
    pConn->iTxTotal = 0;
    pConn->iTxLeft = 0;
    pConn->bTxWaitAll = AeFalse;
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    pConn->bSSLEnabled = AeFalse;
    pConn->bSSLNegotiated = AeFalse;
    pConn->pSSL = NULL;
#endif
    pConn->lastActivityTime.iSec = 0;
    pConn->lastActivityTime.iMicroSec = 0;
    pConn->bSocketValid = AeFalse;
}

/******************************************************************************/
void AeWebConnectionDestroy(AeWebConnection *pConn)
{
    AeWebConnectionFree(pConn);

    AeFree(pConn);
}

/******************************************************************************/
void AeWebConnectionFree(AeWebConnection *pConn)
{
    AeWebConnectionDisconnectV(pConn);

    if (pConn->pHost)
        AeFree(pConn->pHost);
    if (pConn->pOriginHost)
        AeFree(pConn->pOriginHost);

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    if (pConn->pSSL)
        SSL_free(pConn->pSSL);
#endif
}

/******************************************************************************/
void AeWebConnectionInitialize(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort, AeTimeValue *pTimeOut)
{
    AeSetString(&pConn->pHost, pHost, -1);
    pConn->netAddress.iPort = AeNetHToNS(iPort);
    AeSetString(&pConn->pOriginHost, pOriginHost, -1);
    pConn->iOriginPort = iOriginPort;
    pConn->timeOut = *pTimeOut;
}

/******************************************************************************/
AeError AeWebConnectionConnect(AeWebConnection *pConn)
{
    AeError rc;

    /* no need to resolve host string if it is already an IP address */
    if (strspn(pConn->pHost, ".0123456789") == strlen(pConn->pHost))
        pConn->netAddress.iAddress = AeNetInetAddr(pConn->pHost);
    else
    {
        if (!AeWebGetCachedIPAddress(pConn->pHost, &pConn->netAddress.iAddress))
        {
            /* resolve origin hostname */
            rc = AeNetResolve(pConn->pHost, &pConn->netAddress.iAddress);
            if (rc != AeEOK)
                return AeENetUnknownHost;

            /* remember resolved address */
            AeWebSetCachedIPAddress(pConn->pHost, pConn->netAddress.iAddress);
        }
    }

    /* create socket */
    rc = AeNetGetSocket(&pConn->sock, AeTrue);
    if (rc != AeEOK)
        return rc;

    pConn->bSocketValid = AeTrue;

    /* set non-blocking mode on socket */
    rc = AeNetSetBlocking(&pConn->sock, AeFalse);
    if (rc != AeEOK)
        return rc;

    /* set send buffer size */
    /* DISABLED: it is unclear why we needed to modify the send buffer size.
       since small send buffer may degrade SSL performance, don't do it.
    rc = AeNetSetSendBufferSize(&pConn->sock, AE_BUFFER_CHUNK);
    if (rc != AeEOK)
        return rc;
    */

    /* disable Nagle's algorithm */
    rc = AeNetSetNoDelay(&pConn->sock, AeTrue);
    if (rc != AeEOK)
        return rc;

    /* try to connect */
    rc = AeNetConnect(&pConn->sock, &pConn->netAddress);
    if (rc != AeEOK && rc != AeENetWouldBlock)
        return rc;

    AeGetElapsedTime(&pConn->lastActivityTime);

    if (rc == AeEOK)
        return AeWebConnectionOnConnected(pConn, &pConn->lastActivityTime);

    pConn->iState = AeWebConnectionConnecting;

    return AeEOK;
}

/******************************************************************************/
AeError AeWebConnectionDisconnect(AeWebConnection *pConn)
{
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    if (pConn->bSSLEnabled)
    {
        if (SSL_is_init_finished(pConn->pSSL))
        {
            /* remember session used in this connection */
            AeWebSetSSLSession(pConn->pOriginHost, pConn->iOriginPort,
                SSL_get1_session(pConn->pSSL));

            /* send "close notify" alert */
            SSL_shutdown(pConn->pSSL);
        }

        SSL_free(pConn->pSSL);
        pConn->pSSL = NULL;

        pConn->bSSLEnabled = AeFalse;
        pConn->bSSLNegotiated = AeFalse;
    }
#endif

    pConn->iState = AeWebConnectionClosed;
    pConn->pRxPtr = NULL;
    pConn->iRxTotal = 0;
    pConn->iRxLeft = 0;
    pConn->bRxWaitAll = AeFalse;
    pConn->pTxPtr = NULL;
    pConn->iTxTotal = 0;
    pConn->iTxLeft = 0;
    pConn->bTxWaitAll = AeFalse;

    if (pConn->bSocketValid == AeTrue)
    {
        AeNetDisconnect(&pConn->sock);
        pConn->bSocketValid = AeFalse;
    }
    
    return AeEOK;
}

/******************************************************************************/
AeError AeWebConnectionSend(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll)
{
    if (pConn->iState != AeWebConnectionEstablished)
        return AeEInternal;

    pConn->pTxPtr = pData;
    pConn->iTxTotal = iSize;
    pConn->iTxLeft = iSize;
	pConn->bTxWaitAll = bWaitAll;

    return AeEOK;
}

/******************************************************************************/
AeError AeWebConnectionReceive(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll)
{
    if (pConn->iState != AeWebConnectionEstablished)
        return AeEInternal;

    pConn->pRxPtr = pData;
    pConn->iRxTotal = iSize;
    pConn->iRxLeft = iSize;
	pConn->bRxWaitAll = bWaitAll;

    return AeEOK;
}

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
/******************************************************************************/
AeError AeWebConnectionEnableSSL(AeWebConnection *pConn)
{
    SSL_SESSION *pSession;

    if (pConn->bSSLEnabled)
        return AeEOK;

    if (!g_webSSLInfo.pContext)
        return AeESSLGeneral;

    /* create SSL connection structure */
    pConn->pSSL = SSL_new(g_webSSLInfo.pContext);
    if (!pConn->pSSL)
        return AeESSLGeneral;

    SSL_set_ex_data(pConn->pSSL, 0, pConn->pTask);

    /* indicate that this side of SSL connection performs active open */
    SSL_set_connect_state(pConn->pSSL);

    /* install certificate verification/validation callback */
    if (g_webSSLInfo.bServerAuth)
        SSL_set_verify(pConn->pSSL, SSL_VERIFY_PEER, SSLVerifyCallback);
    else
        SSL_set_verify(pConn->pSSL, SSL_VERIFY_NONE, NULL);

    SSL_set_fd(pConn->pSSL, (int) AeSocketGetFD(&pConn->sock));

    /* try to reuse pre-existing SSL session */
    if (AeWebGetSSLSession(pConn->pOriginHost, pConn->iOriginPort, &pSession))
        SSL_set_session(pConn->pSSL, pSession);

    pConn->bSSLEnabled = AeTrue;
    pConn->bSSLNegotiated = AeFalse;

    return AeEOK;
}
#endif

/******************************************************************************/
AeError AeWebConnectionOnReadable(AeWebConnection *pConn, AeTimeValue *pTimeValue)
{
    AeInt32 iOctets;
    AeError rc;

    pConn->lastActivityTime = *pTimeValue;

    if (pConn->iState == AeWebConnectionEstablished)
    {
        if (!AeWebTaskOnConnectionReadableV(pConn->pTask))
            return AeEOK;
    }

    /* read data from a source */
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    if (pConn->bSSLEnabled)
        return AeWebConnectionUpdateSSLStatus(pConn);
#endif

    rc = AeNetReceive(&pConn->sock, pConn->pRxPtr, pConn->iRxLeft, &iOctets);
    if (rc == AeEOK)
    {
        /* process connection closure */
        if (iOctets == 0)
        {
            pConn->iState = AeWebConnectionClosed;

            if (pConn->pTask)
                rc = AeWebTaskOnDisconnectedV(pConn->pTask);
        }

        /* process received data */
        else
            rc = AeWebConnectionUpdateRxStatus(pConn, iOctets);
    }

    /* process receive error */
    else
    {
        if (rc == AeENetWouldBlock)
            rc = AeEOK;
        else
            pConn->iState = AeWebConnectionClosed;
    }

    return rc;
}

/******************************************************************************/
AeError AeWebConnectionOnWritable(AeWebConnection *pConn, AeTimeValue *pTimeValue)
{
    AeInt32 iOctets;
    AeError rc;

    /* process connection completion status if in progress */
    if (pConn->iState == AeWebConnectionConnecting)
    {
        /* read socket error: success indicates that connection is established */
        rc = AeNetGetPendingError(&pConn->sock);
        if (rc == AeEOK)
            rc = AeWebConnectionOnConnected(pConn, pTimeValue);
        else
            pConn->iState = AeWebConnectionClosed;

        return rc;
    }

    pConn->lastActivityTime = *pTimeValue;

    if (pConn->iState == AeWebConnectionEstablished)
    {
        if (!AeWebTaskOnConnectionWritableV(pConn->pTask))
            return AeEOK;
    }

    /* send data from buffer */
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    if (pConn->bSSLEnabled)
        return AeWebConnectionUpdateSSLStatus(pConn);
#endif

    rc = AeNetSend(&pConn->sock, pConn->pTxPtr, pConn->iTxLeft, &iOctets);
    if (rc == AeEOK)
        rc = AeWebConnectionUpdateTxStatus(pConn, iOctets);
    else
    {
        if (rc == AeENetWouldBlock)
            rc = AeEOK;
        else
            pConn->iState = AeWebConnectionClosed;
    }

    return rc;
}

/******************************************************************************/
AeBool AeWebConnectionIsEqual(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort)
{
    return (!strcmp(pConn->pHost, pHost) &&
        pConn->netAddress.iPort == AeNetHToNS(iPort) &&
        !strcmp(pConn->pOriginHost, pOriginHost) &&
        pConn->iOriginPort == iOriginPort);
}

/******************************************************************************/
AeBool AeWebConnectionIsConnecting(AeWebConnection *pConn)
{
    return pConn->iState == AeWebConnectionConnecting;
}

/******************************************************************************/
AeBool AeWebConnectionIsConnected(AeWebConnection *pConn)
{
    return (pConn->iState == AeWebConnectionEstablished);
}

/******************************************************************************/
AeBool AeWebConnectionIsClosed(AeWebConnection *pConn)
{
    return pConn->iState == AeWebConnectionClosed;
}

/******************************************************************************/
AeBool AeWebConnectionIsReceiving(AeWebConnection *pConn)
{
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    if (pConn->bSSLEnabled)
    {
        if (SSL_want_read(pConn->pSSL))
            return AeTrue;
        if (SSL_in_before(pConn->pSSL) == 0 &&
            SSL_is_init_finished(pConn->pSSL) == 0)
            return AeFalse;
    }
#endif

    return pConn->iRxLeft != 0;
}

/******************************************************************************/
AeBool AeWebConnectionIsSending(AeWebConnection *pConn)
{
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    if (pConn->bSSLEnabled)
    {
        if (SSL_want_write(pConn->pSSL))
            return AeTrue;
        if (SSL_in_before(pConn->pSSL) == 0 &&
            SSL_is_init_finished(pConn->pSSL) == 0)
            return AeFalse;
    }
#endif

    return pConn->iTxLeft != 0;
}

/******************************************************************************/
AeBool AeWebConnectionIsRxPending(AeWebConnection *pConn)
{
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    if (pConn->bSSLEnabled)
    {
        if (SSL_is_init_finished(pConn->pSSL) != 0 &&
            SSL_pending(pConn->pSSL) > 0)
            return AeTrue;
    }
#endif

    return AeFalse;
}

/******************************************************************************/
static AeError AeWebConnectionUpdateRxStatus(AeWebConnection *pConn, AeInt32 iOctets)
{
    AeError rc;

    rc = AeEOK;
    pConn->iRxLeft -= iOctets;
    if (pConn->iRxLeft < 0)
        pConn->iRxLeft = 0;
    if (pConn->iRxLeft == 0 || !pConn->bRxWaitAll)
    {
        pConn->iRxLeft = 0;
        if (pConn->pTask && AeWebConnectionIsConnectedV(pConn))
        {
            if (pConn->bRxWaitAll)
	    	    rc = AeWebTaskOnRxV(pConn->pTask, pConn->iRxTotal);
            else
                rc = AeWebTaskOnRxV(pConn->pTask, iOctets);
        }
    }
    else
        pConn->pRxPtr += iOctets;

    return rc;
}

/******************************************************************************/
static AeError AeWebConnectionUpdateTxStatus(AeWebConnection *pConn, AeInt32 iOctets)
{
    AeError rc;

    rc = AeEOK;
    pConn->iTxLeft -= iOctets;
    if (pConn->iTxLeft < 0)
        pConn->iTxLeft = 0;
    if (pConn->iTxLeft == 0 || !pConn->bTxWaitAll)
    {
        pConn->iTxLeft = 0;
        if (pConn->pTask && AeWebConnectionIsConnectedV(pConn))
        {
            if (pConn->bTxWaitAll)
	    	    rc = AeWebTaskOnTxV(pConn->pTask, pConn->iTxTotal);
            else
                rc = AeWebTaskOnTxV(pConn->pTask, iOctets);
        }
    }
    else
        pConn->pTxPtr += iOctets;

    return rc;
}

/******************************************************************************/
static AeError  AeWebConnectionOnConnected(AeWebConnection *pConn, AeTimeValue *pTimeValue)
{
    pConn->iState = AeWebConnectionEstablished;

    pConn->lastActivityTime = *pTimeValue;

    if (pConn->pTask && AeWebConnectionIsConnectedV(pConn))
        return AeWebTaskOnConnectedV(pConn->pTask, AeFalse);

    return AeEOK;
}

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
/******************************************************************************/
static AeError AeWebConnectionUpdateSSLStatus(AeWebConnection *pConn)
{
    AeError rc;
    AeInt32 iOctets;

    rc = AeEOK;
    iOctets = -1;
    if (pConn->iRxLeft && rc == AeEOK && pConn->iState == AeWebConnectionEstablished)
    {
        iOctets = SSL_read(pConn->pSSL, pConn->pRxPtr, pConn->iRxLeft);
        if (iOctets > 0)
            rc = AeWebConnectionUpdateRxStatus(pConn, iOctets);
        else
            rc = AeWebConnectionHandleSSLError(pConn, iOctets);
    }

    if (pConn->iTxLeft && rc == AeEOK && pConn->iState == AeWebConnectionEstablished)
    {
        iOctets = SSL_write(pConn->pSSL, pConn->pTxPtr, pConn->iTxLeft);
        if (iOctets > 0)
            rc = AeWebConnectionUpdateTxStatus(pConn, iOctets);
        else
            rc = AeWebConnectionHandleSSLError(pConn, iOctets);
    }

    if (rc != AeEOK || pConn->iState != AeWebConnectionEstablished)
        return rc;

    if (SSL_is_init_finished(pConn->pSSL) && !pConn->bSSLNegotiated)
    {
        int bits = 0;

        SSL_get_cipher_bits(pConn->pSSL, &bits);

        if ((g_webSSLInfo.iCryptoLevel == AeWebCryptoMedium && bits < 128) ||
            (g_webSSLInfo.iCryptoLevel == AeWebCryptoHigh && bits < 168))
            return AeESSLWeakerCipher;

        pConn->bSSLNegotiated = AeTrue;
    }

    if (SSL_state(pConn->pSSL) == SSL_ST_RENEGOTIATE)
        pConn->bSSLNegotiated = AeFalse;

    return AeEOK;
}

/******************************************************************************/
static AeError AeWebConnectionHandleSSLError(AeWebConnection *pConn, AeInt32 iRet)
{
    AeInt32 iSSLError, iCryptoError;

    iSSLError = SSL_get_error(pConn->pSSL, iRet);
    switch (iSSLError)
    {
        case SSL_ERROR_NONE:
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
            return AeEOK;

        case SSL_ERROR_ZERO_RETURN:
            pConn->iState = AeWebConnectionClosed;
            if (pConn->pTask)
                return AeWebTaskOnDisconnectedV(pConn->pTask);
            return AeEOK;
    
        case SSL_ERROR_SYSCALL:
            pConn->iState = AeWebConnectionClosed;
            if (iRet == 0)
            {
                if (pConn->pTask)
                    return AeWebTaskOnDisconnectedV(pConn->pTask);
                return AeEOK;
            }
            else
                return AeNetGetLastError();

        default:
            pConn->iState = AeWebConnectionClosed;
            break;
    }
    
    if (g_webSSLInfo.bServerAuth)
    {
        AeInt32 iX509Error = SSL_get_verify_result(pConn->pSSL);
        if (iX509Error != X509_V_OK)
        { 
            if (iX509Error == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT ||
                iX509Error == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY)
                return AeESSLCertIssuerUnknown;

            return AeESSLCertInvalid;
        }
    }

    iCryptoError = ERR_get_error();
    if (ERR_GET_LIB(iCryptoError) == ERR_LIB_SSL &&
        ERR_GET_FUNC(iCryptoError) == SSL_F_SSL3_GET_SERVER_CERTIFICATE &&
        ERR_GET_REASON(iCryptoError) == SSL_R_CERTIFICATE_VERIFY_FAILED)
        return AeESSLCertVerifyFailed;
    else if (ERR_GET_LIB(iCryptoError) == ERR_LIB_SSL &&
        (ERR_GET_FUNC(iCryptoError) == SSL_F_GET_SERVER_HELLO ||
         ERR_GET_FUNC(iCryptoError) == SSL_F_SSL23_GET_SERVER_HELLO ||
         ERR_GET_FUNC(iCryptoError) == SSL_F_SSL3_GET_SERVER_HELLO))
        return AeESSLHandshakeFailed;

    return AeESSLGeneral;
}

/******************************************************************************/
static AeInt SSLVerifyCallback(AeInt iPreverifyOK, X509_STORE_CTX *pContext)
{
    X509_NAME *subject;
    SSL *ssl;
    AeWebTask *task;
    AeChar subject_cn[256];

    if (!iPreverifyOK)
        return iPreverifyOK;

    if (X509_STORE_CTX_get_error_depth(pContext) != 0)
        return 1;

    ssl = (SSL *) X509_STORE_CTX_get_ex_data(pContext, SSL_get_ex_data_X509_STORE_CTX_idx());
    if (!ssl)
        return 0;

    task = (AeWebTask *) SSL_get_ex_data(ssl, 0);
    if (!task)
        return 0;

    subject = X509_get_subject_name(pContext->current_cert);
    if (!subject)
        return 0;

    memset(subject_cn, 0, sizeof(subject_cn));
    X509_NAME_get_text_by_NID(subject, OBJ_txt2nid("CN"), subject_cn, sizeof(subject_cn));
    if (strcmp(subject_cn, AeWebTaskGetHostV(task)))
        return 0;

    return 1;
}
#endif

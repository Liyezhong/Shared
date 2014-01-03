/* $Id: AeWebCommon.c,v 1.5 2008/05/21 18:24:35 dkhodos Exp $ */

#include "Ae.h"

static AeInt    AeWebHostNameEntryCompare(AePointer pData1, AePointer pData2);
static void     AeWebHostNameEntryRelease(AePointer pObject);
static AeInt    AeWebHeaderCompare(AePointer pData1, AePointer pData2);
static void     AeWebHeaderRelease(AePointer pObject);

/******************************************************************************/
AeError AeWebInitialize(void)
{
    g_webSettings.iLogLevel = AeLogNone;

    g_pWebProxySOCKS = NULL;
    g_pWebProxyHTTP = NULL;

    g_pHostNameCache = NULL;
    AeMutexInitialize(&g_hostNameCacheLock);

    g_pWebAuthCache = NULL;
    AeMutexInitialize(&g_webAuthCacheLock);

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    {
        AeError rc;

        rc = AeWebSSLInitialize();
        if (rc != AeEOK)
            return rc;

        g_pWebSSLSessionCache = NULL;
        AeMutexInitialize(&g_webSSLSessionCacheLock);
    }
#endif

    return AeEOK;
}

/******************************************************************************/
void AeWebProxySet(AeWebProxyInfo **ppProxy, AeWebProxyProtocol iProto, AeChar *pHost, AeUInt16 iPort, AeChar *pUser, AeChar *pPassword)
{
    if (!(*ppProxy))
    {
        *ppProxy = (AeWebProxyInfo *) AeAlloc(sizeof(AeWebProxyInfo));
        if (!(*ppProxy))
            return;
    }
    memset(*ppProxy, 0, sizeof(AeWebProxyInfo));

    (*ppProxy)->iProto = iProto;
    AeSetString(&(*ppProxy)->pHost, pHost, -1);
    (*ppProxy)->iPort = iPort;
    AeSetString(&(*ppProxy)->pUser, pUser, -1);
    AeSetString(&(*ppProxy)->pPassword, pPassword, -1);
}

/******************************************************************************/
void AeWebProxyDestroy(AeWebProxyInfo *pProxy)
{
    if (pProxy->pHost)
        AeFree(pProxy->pHost);
    if (pProxy->pUser)
        AeFree(pProxy->pUser);
    if (pProxy->pPassword)
        AeFree(pProxy->pPassword);

    AeFree(pProxy);
}

/******************************************************************************/
AeBool AeWebGetCachedIPAddress(AeChar *pHostName, AeUInt32 *piIPAddress)
{
    AeListItem *pItem;
    AeTimeValue currentTime, deltaTime;
    AeWebHostEntry keyHost;
    AeBool bFound;

    if (!g_pHostNameCache)
        return AeFalse;

    bFound = AeFalse;

    /* acquire current time */
    AeGetElapsedTime(&currentTime);

    AeMutexLock(&g_hostNameCacheLock);

    /* search hostname in cache */
    keyHost.pHostName = pHostName;
    pItem = AeListFind(g_pHostNameCache, &keyHost);
    if (pItem)
    {
        AeWebHostEntry *pEntry;
        
        pEntry = AeListData(pItem, AeWebHostEntry);

        /* remove stale entry */
        AE_TIME_VALUE_SUBTRACT(deltaTime, currentTime, pEntry->timeStamp);
        if (deltaTime.iSec > AE_HOSTNAME_TTL)
        {
            AeFree(pEntry);
            AeListRemoveItem(g_pHostNameCache, pItem);
        }
        else
        {
            *piIPAddress = pEntry->iIPAddress;
            bFound = AeTrue;
        }
    }

    AeMutexUnlock(&g_hostNameCacheLock);

    return bFound;
}

/******************************************************************************/
void AeWebSetCachedIPAddress(AeChar *pHostName, AeUInt32 iIPAddress)
{
    AeListItem *pItem;
    AeWebHostEntry *pNewEntry;

    AeMutexLock(&g_hostNameCacheLock);

    /* create cache if it does not exist */
    if (!g_pHostNameCache)
    {
        g_pHostNameCache = AeListNew((AeCompareFunc) AeWebHostNameEntryCompare,
            (AeReleaseFunc) AeWebHostNameEntryRelease);
        if (!g_pHostNameCache)
        {
            AeMutexUnlock(&g_hostNameCacheLock);
            return;
        }
    }

    /* prepare entry for search/insertion */
    pNewEntry = (AeWebHostEntry *) AeAlloc(sizeof(AeWebHostEntry));
    if (!pNewEntry)
    {
        AeMutexUnlock(&g_hostNameCacheLock);
        return;
    }
    memset(pNewEntry, 0, sizeof(AeWebHostEntry));
    AeSetString(&pNewEntry->pHostName, pHostName, -1);

    /* search hostname in cache */
    pItem = AeListFind(g_pHostNameCache, pNewEntry);
    if (pItem)
    {
        /* update IP address */
        AeListData(pItem, AeWebHostEntry)->iIPAddress = iIPAddress;

        /* destroy new entry */
        AeWebHostNameEntryRelease(pNewEntry);
    }
    else
    {
        /* insert new entry */
        pNewEntry->iIPAddress = iIPAddress;
        AeGetElapsedTime(&pNewEntry->timeStamp);
        AeListAppend(g_pHostNameCache, pNewEntry);
    }

    AeMutexUnlock(&g_hostNameCacheLock);
}

/******************************************************************************/
static int AeWebHostNameEntryCompare(AePointer pData1, AePointer pData2)
{
    return strcmp(((AeWebHostEntry *) pData1)->pHostName,
        ((AeWebHostEntry *) pData2)->pHostName);
}

/******************************************************************************/
static void AeWebHostNameEntryRelease(AePointer pObject)
{
    if (((AeWebHostEntry *) pObject)->pHostName)
        AeFree(((AeWebHostEntry *) pObject)->pHostName);

    AeFree(pObject);
}

/******************************************************************************/
AeWebHeaders *AeWebHeadersNew(void)
{
    AeWebHeaders *pHeaders;
    
    pHeaders = AeListNew((AeCompareFunc) AeWebHeaderCompare,
        (AeReleaseFunc) AeWebHeaderRelease);
    if (!pHeaders)
        return NULL;

    return pHeaders;
}

/******************************************************************************/
void AeWebHeadersDestroy(AeWebHeaders *pHeaders)
{
    AeListDestroy(pHeaders);
}

/******************************************************************************/
void AeWebHeadersSet(AeWebHeaders *pHeaders, AeChar *pName, AeChar *pValue)
{
    AeWebHeader *pHeader;

    pHeader = (AeWebHeader *) AeAlloc(sizeof(AeWebHeader));
    if (!pHeader)
        return;
    memset(pHeader, 0, sizeof(AeWebHeader));
    AeSetString(&pHeader->pName, pName, -1);
    AeSetString(&pHeader->pValue, pValue, -1);

    AeListAppend(pHeaders, pHeader);
}

/******************************************************************************/
AeChar *AeWebHeadersGet(AeWebHeaders *pHeaders, AeChar *pName)
{
    AeWebHeader keyHeader;
    AeListItem *pItem;

    keyHeader.pName = pName;
    pItem = AeListFind(pHeaders, &keyHeader);
    if (pItem)
        return AeListData(pItem, AeWebHeader)->pValue;
    else
        return NULL;
}

/******************************************************************************/
void AeWebHeadersRemove(AeWebHeaders *pHeaders, AeChar *pName)
{
//    AeWebHeader keyHeader;
    AeListItem *pItem;

//    keyHeader.pName = pName;
    pItem = AeListFind(pHeaders, pName);
    if (pItem)
    {
        AeWebHeaderRelease(AeListData(pItem, AeWebHeader));
        AeListRemoveItem(pHeaders, pItem);
    }
}

/******************************************************************************/
void AeWebHeadersRemoveAll(AeWebHeaders *pHeaders)
{
    AeListFree(pHeaders);
}

/******************************************************************************/
void AeWebHeadersWrite(AeWebHeaders *pHeaders, AeBuffer *pBuffer)
{
    AeWebHeader *pHeader;
    AeListItem *pItem;

    pItem = AeListFirst(pHeaders);
    while (pItem)
    {
        pHeader = AeListData(pItem, AeWebHeader);

        AeBufferWrite(pBuffer, pHeader->pName, strlen(pHeader->pName));
        AeBufferWrite(pBuffer, ": ", 2);
        AeBufferWriteLn(pBuffer, pHeader->pValue, strlen(pHeader->pValue));

        pItem = AeListNext(pItem);
    }
}

/******************************************************************************/
static AeInt AeWebHeaderCompare(AePointer pData1, AePointer pData2)
{
    return strcasecmp(((AeWebHeader *) pData1)->pName,
        ((AeWebHeader *) pData2)->pName);
}

/******************************************************************************/
static void AeWebHeaderRelease(AePointer pObject)
{
    if (((AeWebHeader *) pObject)->pName)
        AeFree(((AeWebHeader *) pObject)->pName);
    if (((AeWebHeader *) pObject)->pValue)
        AeFree(((AeWebHeader *) pObject)->pValue);

    AeFree(pObject);
}

/******************************************************************************/
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)

static void     AeWebSSLLockingCallback(AeInt mode, AeInt type, const AeChar *file, AeInt line);
static AeInt    AeWebSSLSessionCompare(AePointer pData1, AePointer pData2);
static void     AeWebSSLSessionRelease(AePointer pObject);

static AeMutex  *g_pWebSSLLocks = NULL;
static AeInt    g_iWebSSLLockCount = 0;

/******************************************************************************/
AeError AeWebSSLInitialize(void)
{
    AeInt i;

    g_webSSLInfo.iCryptoLevel = AeWebCryptoNone;
    g_webSSLInfo.bServerAuth = AeFalse;
	g_webSSLInfo.pContext = NULL;

    SSL_library_init();

    /* allocate mutexes for OpenSSL locks */
    g_iWebSSLLockCount = CRYPTO_num_locks();
    g_pWebSSLLocks = (AeMutex *) AeAlloc(sizeof(AeMutex) * g_iWebSSLLockCount);
    if (!g_pWebSSLLocks)
        return AeEMemory;

    /* initialize the mutexes */
    for (i = 0; i < g_iWebSSLLockCount; i++)
        AeMutexInitialize(&(g_pWebSSLLocks[i]));

    CRYPTO_set_locking_callback(AeWebSSLLockingCallback);

    return AeEOK;
}

/******************************************************************************/
void AeWebSSLDestroy(void)
{
    AeInt i;

    if (g_webSSLInfo.pContext)
    {
        SSL_CTX_free(g_webSSLInfo.pContext);
        ERR_remove_state(0);
    }

    EVP_cleanup();

    CRYPTO_set_locking_callback(NULL);

    /* destroy OpenSSL mutexes */
    for (i = 0; i < g_iWebSSLLockCount; i++)
        AeMutexDestroy(&(g_pWebSSLLocks[i]));

    AeFree(g_pWebSSLLocks);
}

/******************************************************************************/
AeBool AeWebGetSSLSession(AeChar *pHost, AeUInt16 iPort, SSL_SESSION **ppSession)
{
    AeWebSSLSession keySession;
    AeListItem *pItem;

    if (!g_pWebSSLSessionCache)
        return AeFalse;

    AeMutexLock(&g_webSSLSessionCacheLock);

    /* search SSL session in cache */
    keySession.pHost = pHost;
    keySession.iPort = iPort;
    pItem = AeListFind(g_pWebSSLSessionCache, &keySession);

    AeMutexUnlock(&g_webSSLSessionCacheLock);

    if (pItem)
    {
        *ppSession = AeListData(pItem, AeWebSSLSession)->pSession;
        return AeTrue;
    }

    *ppSession = NULL;
    return AeFalse;
}

/******************************************************************************/
void AeWebSetSSLSession(AeChar *pHost, AeUInt16 iPort, SSL_SESSION *pSession)
{
    AeWebSSLSession *pNewSession;
    AeListItem *pItem;

    AeMutexLock(&g_webSSLSessionCacheLock);

    /* create cache if it does not exist */
    if (!g_pWebSSLSessionCache)
    {
        g_pWebSSLSessionCache = AeListNew((AeCompareFunc) AeWebSSLSessionCompare,
            (AeReleaseFunc) AeWebSSLSessionRelease);
        if (!g_pWebSSLSessionCache)
        {
            AeMutexUnlock(&g_webSSLSessionCacheLock);
            return;
        }
    }

    /* prepare session for search/insertion */
    pNewSession = (AeWebSSLSession *) AeAlloc(sizeof(AeWebSSLSession));
    if (!pNewSession)
    {
        AeMutexUnlock(&g_webSSLSessionCacheLock);
        return;
    }
    memset(pNewSession, 0, sizeof(AeWebSSLSession));
    AeSetString(&pNewSession->pHost, pHost, -1);
    pNewSession->iPort = iPort;

    /* search session in cache */
    pItem = AeListFind(g_pWebSSLSessionCache, pNewSession);
    if (pItem)
    {
        /* update session */
        SSL_SESSION_free(AeListData(pItem, AeWebSSLSession)->pSession);
        AeListData(pItem, AeWebSSLSession)->pSession = pSession;

        /* destroy new entry */
        AeWebSSLSessionRelease(pNewSession);
    }
    else
    {
        /* insert new entry */
        pNewSession->pSession = pSession;
        AeListAppend(g_pWebSSLSessionCache, pNewSession);
    }

    AeMutexUnlock(&g_webSSLSessionCacheLock);
}

/******************************************************************************/
static void AeWebSSLLockingCallback(AeInt mode, AeInt type, const AeChar *file, AeInt line)
{
    if (mode & CRYPTO_LOCK)
        AeMutexLock(&(g_pWebSSLLocks[type]));
    else
        AeMutexUnlock(&(g_pWebSSLLocks[type]));
}

/******************************************************************************/
static int AeWebSSLSessionCompare(AePointer pData1, AePointer pData2)
{
    return !(!strcmp(((AeWebSSLSession *) pData1)->pHost, ((AeWebSSLSession *) pData2)->pHost) &&
        ((AeWebSSLSession *) pData1)->iPort == ((AeWebSSLSession *) pData2)->iPort);
}

/******************************************************************************/
static void AeWebSSLSessionRelease(AePointer pObject)
{
    if (((AeWebSSLSession *) pObject)->pHost)
        AeFree(((AeWebSSLSession *) pObject)->pHost);

    if (((AeWebSSLSession *) pObject)->pSession)
        SSL_SESSION_free(((AeWebSSLSession *) pObject)->pSession);

    AeFree(pObject);
}
#endif

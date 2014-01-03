/* $Id: AeWebRequest.c,v 1.4.2.1 2009/06/16 20:52:27 hfeng Exp $ */

#include "Ae.h"

/******************************************************************************/
AeWebRequest *AeWebRequestNew(void)
{
    AeWebRequest *pRequest;

    pRequest = (AeWebRequest *) AeAlloc(sizeof(AeWebRequest));
    if (!pRequest)
        return NULL;
    memset(pRequest, 0, sizeof(AeWebRequest));

    pRequest->pURL = NULL;
    AeSetString(&pRequest->pVersion, HTTP_VERSION_11, -1);
    AeSetString(&pRequest->pMethod, HTTP_METHOD_GET, -1);
    pRequest->pHost = NULL;
    pRequest->iPort = HTTP_DEFAULT_PORT;
    AeSetString(&pRequest->pAbsPath, "/", -1);
    pRequest->pUser = NULL;
    pRequest->pPassword = NULL;
    pRequest->pContentType = NULL;
    pRequest->pEntityData = NULL;
    pRequest->iEntitySize = 0;
    pRequest->bPersistent = AeFalse;
    pRequest->bStrict = AeFalse;
    pRequest->bSecure = AeFalse;
    pRequest->timeOut.iSec = AE_HTTP_TIMEOUT;
    pRequest->timeOut.iMicroSec = 0;

    pRequest->pRequestHeaders = NULL;
    pRequest->pResponseHeaders = NULL;
    pRequest->iResponseStatus = 0;
    pRequest->iError = AeEOK;
    pRequest->pUserData = NULL;
	pRequest->pProxyInfo = NULL;

    pRequest->pOnError = NULL;
    pRequest->pOnResponse = NULL;
    pRequest->pOnEntity = NULL;
    pRequest->pOnCompleted = NULL;

    return pRequest;
}

/******************************************************************************/
void AeWebRequestDestroy(AeWebRequest *pRequest)
{
    if (pRequest->pURL)
        AeURLDestroy(pRequest->pURL);
    if (pRequest->pVersion)
        AeFree(pRequest->pVersion);
    if (pRequest->pMethod)
        AeFree(pRequest->pMethod);
    if (pRequest->pHost)
        AeFree(pRequest->pHost);
    if (pRequest->pAbsPath)
        AeFree(pRequest->pAbsPath);
    if (pRequest->pUser)
        AeFree(pRequest->pUser);
    if (pRequest->pPassword)
        AeFree(pRequest->pPassword);
    if (pRequest->pContentType)
        AeFree(pRequest->pContentType);

    if (pRequest->pRequestHeaders)
        AeWebHeadersDestroy((AeWebHeaders *) pRequest->pRequestHeaders);
    if (pRequest->pResponseHeaders)
        AeWebHeadersDestroy((AeWebHeaders *) pRequest->pResponseHeaders);

    AeFree(pRequest);
}

/******************************************************************************/
AeBool AeWebRequestSetURL(AeWebRequest *pRequest, AeChar *pString)
{
    AeURL *pURL;

    if (!pRequest->pURL)
    {
        pRequest->pURL = AeURLNew();
        if (!pRequest->pURL)
            return AeFalse;
    }
    pURL = (AeURL *) pRequest->pURL;

    if (!AeURLSet(pURL, pString))
        return AeFalse;

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    AeWebRequestSetSecure(pRequest, pURL->bSecure);
#endif
    AeWebRequestSetHost(pRequest, pURL->pHost);
    AeWebRequestSetPort(pRequest, pURL->iPort);
    AeWebRequestSetAbsPath(pRequest, pURL->pAbsPath);
    AeWebRequestSetUser(pRequest, pURL->pUser);
    AeWebRequestSetPassword(pRequest, pURL->pPassword);

    return AeTrue;
}

/******************************************************************************/
AeChar *AeWebRequestGetURL(AeWebRequest *pRequest)
{
    AeURL *pURL;

    if (!pRequest->pURL)
    {
        pRequest->pURL = AeURLNew();
        if (!pRequest->pURL)
            return NULL;
    }
    pURL = (AeURL *) pRequest->pURL;

#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
    AeURLSetSecure(pURL, pRequest->bSecure);
#endif
    AeURLSetHost(pURL, pRequest->pHost);
    AeURLSetPort(pURL, pRequest->iPort);
    AeURLSetAbsPath(pURL, pRequest->pAbsPath);
    AeURLSetUser(pURL, pRequest->pUser);
    AeURLSetPassword(pURL, pRequest->pPassword);

    return AeURLGet(pURL);
}

/******************************************************************************/
void AeWebRequestSetVersion(AeWebRequest *pRequest, AeChar *pVersion)
{
    AeSetString(&pRequest->pVersion, pVersion, -1);
}

/******************************************************************************/
void AeWebRequestSetMethod(AeWebRequest *pRequest, AeChar *pMethod)
{
    AeSetString(&pRequest->pMethod, pMethod, -1);
}

/******************************************************************************/
void AeWebRequestSetHost(AeWebRequest *pRequest, AeChar *pHost)
{
    AeSetString(&pRequest->pHost, pHost, -1);
}

/******************************************************************************/
void AeWebRequestSetAbsPath(AeWebRequest *pRequest, AeChar *pAbsPath)
{
    if (!pAbsPath || pAbsPath[0] == '\0')
        AeSetString(&pRequest->pAbsPath, "/", 1);
    else
        AeSetString(&pRequest->pAbsPath, pAbsPath, -1);
}

/******************************************************************************/
void AeWebRequestSetUser(AeWebRequest *pRequest, AeChar *pUser)
{
    AeSetString(&pRequest->pUser, pUser, -1);
}

/******************************************************************************/
void AeWebRequestSetPassword(AeWebRequest *pRequest, AeChar *pPassword)
{
    AeSetString(&pRequest->pPassword, pPassword, -1);
}

/******************************************************************************/
void AeWebRequestSetContentType(AeWebRequest *pRequest, AeChar *pContentType)
{
    AeSetString(&pRequest->pContentType, pContentType, -1);
}

/******************************************************************************/
void AeWebRequestSetRequestHeader(AeWebRequest *pRequest, AeChar *pName, AeChar *pValue)
{
    if (!pRequest->pRequestHeaders)
    {
        pRequest->pRequestHeaders = AeWebHeadersNew();
        if (!pRequest->pRequestHeaders)
            return;
    }

    AeWebHeadersSet((AeWebHeaders *) pRequest->pRequestHeaders, pName, pValue);
}

/******************************************************************************/
void AeWebRequestSetResponseHeader(AeWebRequest *pRequest, AeChar *pName, AeChar *pValue)
{
    if (!pRequest->pResponseHeaders)
    {
        pRequest->pResponseHeaders = AeWebHeadersNew();
        if (!pRequest->pResponseHeaders)
            return;
    }

    AeWebHeadersSet((AeWebHeaders *) pRequest->pResponseHeaders, pName, pValue);
}

/******************************************************************************/
AeChar *AeWebRequestGetResponseHeader(AeWebRequest *pRequest, AeChar *pName)
{
    AeListItem *pItem;

    if (!pRequest->pResponseHeaders)
        return NULL;

    pItem = AeListFirst((AeWebHeaders *) pRequest->pResponseHeaders);
    while (pItem)
    {
        pItem = AeListNext(pItem);
        if (!strcasecmp(AeListData(pItem, AeWebHeader)->pName, pName))
            return AeListData(pItem, AeWebHeader)->pValue;
    }

    return NULL;
}

/******************************************************************************/
AePointer AeWebRequestGetFirstResponseHeader(AeWebRequest *pRequest, AeChar **ppName, AeChar **ppValue)
{
    AeListItem *pItem;

    if (!pRequest->pResponseHeaders)
        return NULL;

    pItem = AeListFirst((AeWebHeaders *) pRequest->pResponseHeaders);
    if (pItem)
    {
        *ppName = AeListData(pItem, AeWebHeader)->pName;
        *ppValue = AeListData(pItem, AeWebHeader)->pValue;
    }

    return pItem;
}

/******************************************************************************/
AePointer AeWebRequestGetNextResponseHeader(AeWebRequest *pRequest, AePointer pPosition, AeChar **ppName, AeChar **ppValue)
{
    AeListItem *pItem;

    pItem = (AeListItem *) pPosition;
    if (pItem)
    {
        pItem = AeListNext(pItem);
        if (pItem)
        {
            *ppName = AeListData(pItem, AeWebHeader)->pName;
            *ppValue = AeListData(pItem, AeWebHeader)->pValue;
        }
    }

    return pItem;
}

/******************************************************************************/
void AeWebRequestClearStatus(AeWebRequest *pRequest)
{
    pRequest->iResponseStatus = 0;
    pRequest->iError = AeEOK;

    if (pRequest->pResponseHeaders)
        AeWebHeadersDestroy((AeWebHeaders *) pRequest->pResponseHeaders);
    pRequest->pResponseHeaders = NULL;
}

/* $Id: AeWebAuth.c,v 1.9 2008/05/21 18:24:34 dkhodos Exp $ */

#include "Ae.h"

static AeInt    AeWebAuthInfoCompare(AePointer pData1, AePointer pData2);
static void     AeWebAuthInfoRelease(AePointer pObject);
static void     AeWebAuthMakeCredentialsBasic(AeBuffer *pCred, AeChar *pUser, AeChar *pPassword);
static void     AeWebAuthGetNextParam(AeChar **ppPosition, AeChar **ppName, AeChar **ppValue);
static void     AeWebAuthMakeCredentialsDigest(AeBuffer *pCred, AeChar *pUser, AeChar *pPassword, AeWebAuthParamsDigest *pParams, AeWebRequest *pRequest, AeChar *pMethod);
static void     AeWebAuthMakeCredentialsNTLM(AeBuffer *pCred, AeChar *pUser, AeChar *pPassword, AeWebAuthParamsNTLM *pParams);
static AeBool   AeWebAuthParseChallengeDigest(AeChar *pChallenge, AeWebAuthParamsDigest *pParams);
static AeBool   AeWebAuthParseChallengeNTLM(AeChar *pChallenge, AeWebAuthParamsNTLM *pParams);
static void     AeWebAuthNTLMGetLMResponse(AeChar *pPassword, AeUInt8 *pNonce, AeUInt8 *pResponse);
static void     AeWebAuthNTLMGetNTLMResponse(AeChar *pPassword, AeUInt8 *pNonce, AeUInt8 *pResponse);
static void     AeWebAuthNTLMSetupDESKey(AeUInt8 *pKey56, des_key_schedule KS);
static void     AeWebAuthNTLMCalculateResponse(AeUInt8 *pKeys, AeUInt8 *pPlainText, AeUInt8 *pResults);
static void     AeWebAuthNTLMParseUserString(AeChar *pUserString, AeChar **ppDomain, AeChar **ppUserName);
static void     AeWebAuthNTLMUppercaseString(AeChar *pString);
static AeInt    AeWebAuthNTLMUnicodeString(AeChar *pSrc, AeChar **ppDst);
static void     AeWebAuthNTLMUnqualifyHostname(AeChar *pHostname);
static void     AeWebAuthNTLMReadBuffer(AeChar *pMsg, AeUInt32 iMsgSize, AeUInt32 iHeaderOffset, AeChar **ppData, AeUInt16 *piSize);
static void     AeWebAuthNTLMWriteBuffer(AeArray *pMsg, AeUInt32 iHeaderSize, AeUInt32 iHeaderOffset, AeChar *pData, AeUInt16 iSize);
static void     AeWebAuthNTLMWriteStringBuffer(AeArray *pMsg, AeUInt32 iHeaderSize, AeUInt32 iHeaderOffset, AeChar *pStr, AeUInt32 iFlags);

/******************************************************************************/
AeBool AeWebAuthInfoGet(AeChar *pHost, AeUInt16 iPort, AeChar *pAbsPath, AeWebAuthScheme *piScheme, AePointer *ppParams)
{
    AeListItem *pItem;
    AeWebAuthInfo keyAuth;

    if (!g_pWebAuthCache)
        return AeFalse;

    AeMutexLock(&g_webAuthCacheLock);

    /* search auth info in the cache */
    keyAuth.pHost = pHost;
    keyAuth.iPort = iPort;
    keyAuth.pAbsPath = pAbsPath;
    pItem = AeListFind(g_pWebAuthCache, &keyAuth);
    if (pItem)
    {
        *piScheme = AeListData(pItem, AeWebAuthInfo)->iScheme;
        *ppParams = AeListData(pItem, AeWebAuthInfo)->pParams;
    }

    AeMutexUnlock(&g_webAuthCacheLock);

    return (pItem != NULL);
}

/******************************************************************************/
void AeWebAuthInfoSet(AeChar *pHost, AeUInt16 iPort, AeChar *pAbsPath, AeWebAuthScheme iScheme, AePointer pParams)
{
    AeListItem *pItem;
    AeWebAuthInfo *pNewAuth;

    AeMutexLock(&g_webAuthCacheLock);

    /* create cache if it does not exist */
    if (!g_pWebAuthCache)
    {
        g_pWebAuthCache = AeListNew((AeCompareFunc) AeWebAuthInfoCompare,
            (AeReleaseFunc) AeWebAuthInfoRelease);
        if (!g_pWebAuthCache)
        {
            AeMutexUnlock(&g_webAuthCacheLock);
            return;
        }
    }

    /* prepare entry for search/insertion */
    pNewAuth = (AeWebAuthInfo *) AeAlloc(sizeof(AeWebAuthInfo));
    if (!pNewAuth)
    {
        AeMutexUnlock(&g_webAuthCacheLock);
        return;
    }
    memset(pNewAuth, 0, sizeof(AeWebAuthInfo));
    AeSetString(&pNewAuth->pHost, pHost, -1);
    pNewAuth->iPort = iPort;
    AeSetString(&pNewAuth->pAbsPath, pAbsPath, -1);
    pNewAuth->iScheme = iScheme;
    pNewAuth->pParams = pParams;

    /* search auth info in the cache */
    pItem = AeListFind(g_pWebAuthCache, pNewAuth);
    if (pItem)
    {
        /* destroy existing entry */
        AeWebAuthInfoRelease(AeListData(pItem, AeWebAuthInfo));
        AeListRemoveItem(g_pWebAuthCache, pItem);
    }

    /* insert new entry */
    AeListAppend(g_pWebAuthCache, pNewAuth);

    AeMutexUnlock(&g_webAuthCacheLock);
}

/******************************************************************************/
void AeWebAuthMakeRequestHeaders(AeWebHeaders *pHeaders, AeBool bProxy, AeWebAuthScheme iScheme, AePointer pParams, AeWebRequest *pRequest, AeChar *pMethod, AeChar *pUser, AeChar *pPassword)
{
    AeBuffer *pCredentials;
    AeChar *pBuffer;
    AeInt32 iBufferSize;

    if (!pUser || !pPassword)
        return;

    pCredentials = AeBufferNew(NULL, 0, 256);
    if (!pCredentials)
        return;

    switch (iScheme)
    {
        case AeWebAuthBasic:
            AeWebAuthMakeCredentialsBasic(pCredentials, pUser, pPassword);
            break;
        case AeWebAuthDigest:
            AeWebAuthMakeCredentialsDigest(pCredentials, pUser, pPassword,
                (AeWebAuthParamsDigest *) pParams, pRequest, pMethod);
            break;
        case AeWebAuthNTLM:
            AeWebAuthMakeCredentialsNTLM(pCredentials, pUser, pPassword,
                (AeWebAuthParamsNTLM *) pParams);
            break;
        case AeWebAuthNone:
        default:
            AeBufferDestroy(pCredentials);
            return;
    }

    AeBufferGetReadArea(pCredentials, &pBuffer, &iBufferSize);
    if (bProxy)
        AeWebHeadersSet(pHeaders, HTTP_HEADER_PROXYAUTHORIZATION, pBuffer);
    else
        AeWebHeadersSet(pHeaders, HTTP_HEADER_AUTHORIZATION, pBuffer);

    AeBufferDestroy(pCredentials);
}

/******************************************************************************/
AeBool AeWebAuthParseChallenge(AeChar *pChallenge, AeWebAuthScheme *piScheme, AePointer *ppParams)
{
    AeBool rc;

    rc = AeTrue;

    /* Basic authentication scheme */
    if (!strncasecmp(pChallenge, HTTP_AUTH_SCHEME_BASIC,
        strlen(HTTP_AUTH_SCHEME_BASIC)))
    {
        *piScheme = AeWebAuthBasic;
        *ppParams = NULL;
    }
    /* Digest authentication scheme */
    else if (!strncasecmp(pChallenge, HTTP_AUTH_SCHEME_DIGEST,
        strlen(HTTP_AUTH_SCHEME_DIGEST)))
    {
        *ppParams = (AeWebAuthParamsDigest *) AeAlloc(sizeof(AeWebAuthParamsDigest));
        if (!(*ppParams))
            return AeFalse;
        memset(*ppParams, 0, sizeof(AeWebAuthParamsDigest));

        rc = AeWebAuthParseChallengeDigest(pChallenge + strlen(HTTP_AUTH_SCHEME_DIGEST),
            (AeWebAuthParamsDigest *) *ppParams);

        *piScheme = AeWebAuthDigest;
    }
    /* NTLM authentication scheme */
    else if (!strncasecmp(pChallenge, HTTP_AUTH_SCHEME_NTLM,
        strlen(HTTP_AUTH_SCHEME_NTLM)) ||
        !strncasecmp(pChallenge, HTTP_AUTH_SCHEME_NEGOTIATE,
            strlen(HTTP_AUTH_SCHEME_NEGOTIATE)))
    {
        if (!(*ppParams))
        {
            *ppParams = (AeWebAuthParamsNTLM *) AeAlloc(sizeof(AeWebAuthParamsNTLM));
            if (!(*ppParams))
                return AeFalse;
            memset(*ppParams, 0, sizeof(AeWebAuthParamsNTLM));
        }
        else
            rc = AeWebAuthParseChallengeNTLM(pChallenge + strlen(HTTP_AUTH_SCHEME_NTLM),
                (AeWebAuthParamsNTLM *) *ppParams);

        *piScheme = AeWebAuthNTLM;
    }
    else
        rc = AeFalse;

    return rc;
}

/******************************************************************************/
static void AeWebAuthMakeCredentialsBasic(AeBuffer *pCred, AeChar *pUser, AeChar *pPassword)
{
    AeChar *pCredPlain, *pCredBase64;
    AeInt iLength;

    iLength = strlen(pUser) + strlen(pPassword) + 2;
    pCredPlain = (AeChar *) AeAlloc(iLength);
    if (!pCredPlain)
        return;
    snprintf(pCredPlain, iLength, "%s:%s", pUser, pPassword);

    pCredBase64 = (AeChar *) AeAlloc((strlen(pCredPlain) + 2) / 3 * 4 + 1);
    if (!pCredBase64)
        return;
    AeBase64Encode(pCredPlain, strlen(pCredPlain), pCredBase64);

    AeBufferWrite(pCred, HTTP_AUTH_SCHEME_BASIC, strlen(HTTP_AUTH_SCHEME_BASIC));
    AeBufferWrite(pCred, " ", 1);
    AeBufferWrite(pCred, pCredBase64, strlen(pCredBase64));

    AeFree(pCredBase64);
    AeFree(pCredPlain);
}

/******************************************************************************/
static void AeWebAuthMakeCredentialsDigest(AeBuffer *pCred, AeChar *pUser, AeChar *pPassword, AeWebAuthParamsDigest *pParams, AeWebRequest *pRequest, AeChar *pMethod)
{
    MD5_CTX ctx;
    AeUInt8 pA1[16], pA2[16], pDigest[16];
    AeChar pHex[33], pURI[256], *pTmp;

    if (!strcasecmp(pMethod, HTTP_METHOD_CONNECT))
        snprintf(pURI, sizeof(pURI), "%s:%d", pRequest->pHost, pRequest->iPort);
    else
        snprintf(pURI, sizeof(pURI), "%s", pRequest->pAbsPath);

    AeBufferWrite(pCred, HTTP_AUTH_SCHEME_DIGEST, strlen(HTTP_AUTH_SCHEME_DIGEST));
    AeBufferWrite(pCred, " ", 1);

    pTmp = "username=\"";
    AeBufferWrite(pCred, pTmp, strlen(pTmp));
    AeBufferWrite(pCred, pUser, strlen(pUser));
    AeBufferWrite(pCred, "\"", 1);

    pTmp = ", realm=\"";
    AeBufferWrite(pCred, pTmp, strlen(pTmp));
    AeBufferWrite(pCred, pParams->pRealm, strlen(pParams->pRealm));
    AeBufferWrite(pCred, "\"", 1);

    pTmp = ", nonce=\"";
    AeBufferWrite(pCred, pTmp, strlen(pTmp));
    AeBufferWrite(pCred, pParams->pNonce, strlen(pParams->pNonce));
    AeBufferWrite(pCred, "\"", 1);

    pTmp = ", pURI=\"";
    AeBufferWrite(pCred, pTmp, strlen(pTmp));
    AeBufferWrite(pCred, pURI, strlen(pURI));
    AeBufferWrite(pCred, "\"", 1);

    MD5_Init(&ctx);
    MD5_Update(&ctx, (AeUInt8 *) pUser, strlen(pUser));
    MD5_Update(&ctx, (AeUInt8 *) ":", 1);
    MD5_Update(&ctx, (AeUInt8 *) pParams->pRealm, strlen(pParams->pRealm));
    MD5_Update(&ctx, (AeUInt8 *) ":", 1);
    MD5_Update(&ctx, (AeUInt8 *) pPassword, strlen(pPassword));
    MD5_Final(pA1, &ctx);

    MD5_Init(&ctx);
    MD5_Update(&ctx, (AeUInt8 *) pMethod, strlen(pMethod));
    MD5_Update(&ctx, (AeUInt8 *) ":", 1);
    MD5_Update(&ctx, (AeUInt8 *) pURI, strlen(pURI));
    MD5_Final(pA2, &ctx);

    MD5_Init(&ctx);
    AeBinToHex(pA1, 16, pHex);
    MD5_Update(&ctx, (AeUInt8 *) pHex, strlen(pHex));
    MD5_Update(&ctx, (AeUInt8 *) ":", 1);
    MD5_Update(&ctx, (AeUInt8 *) pParams->pNonce, strlen(pParams->pNonce));
    MD5_Update(&ctx, (AeUInt8 *) ":", 1);
    AeBinToHex(pA2, 16, pHex);
    MD5_Update(&ctx, (AeUInt8 *) pHex, strlen(pHex));
    MD5_Final(pDigest, &ctx);

    AeBinToHex(pDigest, 16, pHex);

    pTmp = ", response=\"";
    AeBufferWrite(pCred, pTmp, strlen(pTmp));
    AeBufferWrite(pCred, pHex, strlen(pHex));
    AeBufferWrite(pCred, "\"", 1);

    if (pParams->pOpaque)
    {
        pTmp = ", opaque=\"";
        AeBufferWrite(pCred, pTmp, strlen(pTmp));
        AeBufferWrite(pCred, pParams->pOpaque, strlen(pParams->pOpaque));
        AeBufferWrite(pCred, "\"", 1);
    }
}

/******************************************************************************/
static void AeWebAuthMakeCredentialsNTLM(AeBuffer *pCred, AeChar *pUser, AeChar *pPassword, AeWebAuthParamsNTLM *pParams)
{
    AeArray *pMessage;
    AeChar *pDomain, *pUserName, pWorkstation[65], *pMessageBase64;
    AeUInt32 iNumber;

    /* parse user string. the expected format is [domain\]username. */
    AeWebAuthNTLMParseUserString(pUser, &pDomain, &pUserName);
    if (!pUserName)
        return;
    if (pDomain)
        AeWebAuthNTLMUppercaseString(pDomain);

    /* query workstation name from the OS */
    if (AeNetHostName(pWorkstation, sizeof(pWorkstation) - 1) == AeEOK)
    {
        AeWebAuthNTLMUnqualifyHostname(pWorkstation);
        AeWebAuthNTLMUppercaseString(pWorkstation);
    }
    else
        pWorkstation[0] = '\0';

    /* create byte array for the message */
    pMessage = AeArrayNew(1);
    if (!pMessage)
        return;
    
    /* NTLMSSP signature is common for all message types */
    AeArraySet(pMessage, 0, HTTP_NTLM_SIGN, sizeof(HTTP_NTLM_SIGN));

    if (!pParams->pNonce)
    {
        /* write message type (1) */
        iNumber = AeNetHToLEL(0x00000001);
        AeArraySet(pMessage, 8, &iNumber, 4);

        /* calculate flags */
        iNumber = HTTP_NTLM_FLAG_NEG_UNICODE | HTTP_NTLM_FLAG_NEG_OEM |
            HTTP_NTLM_FLAG_NEG_NTLM;
        if (pDomain)
            iNumber |= HTTP_NTLM_FLAG_NEG_DOMAIN;
        else
            iNumber |= HTTP_NTLM_FLAG_REQ_TARGET;
        if (pWorkstation[0] != '\0')
            iNumber |= HTTP_NTLM_FLAG_NEG_WORKSTATION;
        
        /* write flags */
        iNumber = AeNetHToLEL(iNumber);
        AeArraySet(pMessage, 12, &iNumber, 4);

        /* write domain */
        AeWebAuthNTLMWriteBuffer(pMessage, HTTP_NTLM_HEADER_SIZE_TYPE1, 16, pDomain,
                                 (AeUInt16) (pDomain ? strlen(pDomain) : 0));

        /* write workstation */
        AeWebAuthNTLMWriteBuffer(pMessage, HTTP_NTLM_HEADER_SIZE_TYPE1, 24, pWorkstation,
                                 (AeUInt16) strlen(pWorkstation));
    }
    else
    {
        AeUInt8 pResponse[24];

        /* write message type (3) */
        iNumber = AeNetHToLEL(0x00000003);
        AeArraySet(pMessage, 8, &iNumber, 4);

        /* write flags (just return the flags set by the server) */
        iNumber = AeNetHToLEL(pParams->iFlags);
        AeArraySet(pMessage, 60, &iNumber, 4);

        /* write target name (either use the domain from the user string,
           or use the target name provided by the server in the type 2 message) */
        if (pDomain)
            AeWebAuthNTLMWriteStringBuffer(pMessage, HTTP_NTLM_HEADER_SIZE_TYPE3, 28, pDomain,
                                           pParams->iFlags);
        else
            AeWebAuthNTLMWriteBuffer(pMessage, HTTP_NTLM_HEADER_SIZE_TYPE3, 28, pParams->pTargetName,
                                     pParams->iTargetNameSize);

        /* write username */
        AeWebAuthNTLMWriteStringBuffer(pMessage, HTTP_NTLM_HEADER_SIZE_TYPE3, 36, pUserName,
                                       pParams->iFlags);

        /* write workstation */
        AeWebAuthNTLMWriteStringBuffer(pMessage, HTTP_NTLM_HEADER_SIZE_TYPE3, 44, pWorkstation,
                                       pParams->iFlags);

        /* write LM response */
        AeWebAuthNTLMGetLMResponse(pPassword, pParams->pNonce, pResponse);
        AeWebAuthNTLMWriteBuffer(pMessage, HTTP_NTLM_HEADER_SIZE_TYPE3, 12, (AeChar *) pResponse, 24);

        /* write NTLM response */
        AeWebAuthNTLMGetNTLMResponse(pPassword, pParams->pNonce, pResponse);
        AeWebAuthNTLMWriteBuffer(pMessage, HTTP_NTLM_HEADER_SIZE_TYPE3, 20, (AeChar *) pResponse, 24);

        /* write [empty] session key */
        AeWebAuthNTLMWriteBuffer(pMessage, HTTP_NTLM_HEADER_SIZE_TYPE3, 52, NULL, 0);
    }

    pMessageBase64 = (AeChar *) AeAlloc((AeArrayCount(pMessage) + 2) / 3 * 4 + 1);
    if (pMessageBase64)
    {
        AeInt32 iMessageBase64Len;
        iMessageBase64Len = AeBase64Encode((AeChar *) AeArrayElementPtr(pMessage, 0),
                                           AeArrayCount(pMessage), pMessageBase64);

        AeBufferWrite(pCred, HTTP_AUTH_SCHEME_NTLM, strlen(HTTP_AUTH_SCHEME_NTLM));
        AeBufferWrite(pCred, " ", 1);
        AeBufferWrite(pCred, pMessageBase64, iMessageBase64Len);

        AeFree(pMessageBase64);
    }

    AeArrayDestroy(pMessage);
    if (pDomain)
        AeFree(pDomain);
    if (pUserName)
        AeFree(pUserName);
}

/******************************************************************************/
static AeBool AeWebAuthParseChallengeDigest(AeChar *pChallenge, AeWebAuthParamsDigest *pParams)
{
    AeChar *pChallengeCopy, *pName, *pValue, *pPos;

    pChallengeCopy = (AeChar *) AeAlloc(strlen(pChallenge) + 1);
    if (!pChallengeCopy)
        return AeFalse;
    strcpy(pChallengeCopy, pChallenge);

    pPos = pChallengeCopy;
    while (1)
    {
        AeWebAuthGetNextParam(&pPos, &pName, &pValue);
        if (!pName || !pValue)
            break;

        if (!strcasecmp(pName, "realm"))
            AeSetString(&pParams->pRealm, pValue, -1);
        else if (!strcasecmp(pName, "nonce"))
            AeSetString(&pParams->pNonce, pValue, -1);
        else if (!strcasecmp(pName, "opaque"))
            AeSetString(&pParams->pOpaque, pValue, -1);
    }

    AeFree(pChallengeCopy);

    if (!pParams->pRealm || !pParams->pNonce)
        return AeFalse;

    return AeTrue;
}

/******************************************************************************/
static AeBool AeWebAuthParseChallengeNTLM(AeChar *pChallenge, AeWebAuthParamsNTLM *pParams)
{
    AeChar *pParamPlain, *pParamBase64;
    AeInt iParamPlainLen, iParamBase64Len;
    AeBool bOK;

    if (!(*pChallenge))
        return AeFalse;

    pParamBase64 = pChallenge + strspn(pChallenge, "\t ");
    iParamBase64Len = strlen(pParamBase64);
    pParamPlain = AeAlloc(iParamBase64Len * 3 / 4);
    if (!pParamPlain)
        return AeFalse;

    iParamPlainLen = AeBase64Decode(pParamBase64, strlen(pParamBase64), pParamPlain);

    bOK = AeTrue;
    if (iParamPlainLen >= 32 &&
        memcmp(pParamPlain, HTTP_NTLM_SIGN, strlen(HTTP_NTLM_SIGN)) == 0)
    {
        if (!pParams->pNonce)
        {
            pParams->pNonce = (AeUInt8 *) AeAlloc(8);
            if (!pParams->pNonce)
                bOK = AeFalse;
        }

        if (bOK)
        {
            /* read target name */
            AeWebAuthNTLMReadBuffer(pParamPlain, iParamPlainLen, 12,
                                    &pParams->pTargetName, &pParams->iTargetNameSize);

            /* read flags */
            memcpy(&pParams->iFlags, pParamPlain + 20, 4);
            pParams->iFlags = AeNetLEToHL(pParams->iFlags);
            
            /* read nonce */
            memcpy(pParams->pNonce, pParamPlain + 24, 8);
        }
    }
    else
        bOK = AeFalse;

    AeFree(pParamPlain);

    return bOK;
}

/******************************************************************************/
static void AeWebAuthNTLMGetLMResponse(AeChar *pPassword, AeUInt8 *pNonce, AeUInt8 *pResponse)
{
    static AeUInt8 pLMResponseMagic[] =
    {
        0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25
    };

    AeUInt8 lm_pw[14], lm_hpw[21];
    des_key_schedule ks;
    AeInt len, i;

    len = strlen(pPassword);
    if (len <= 14)
    {
        for (i = 0; i < 14; i++)
            if (i < len)
                lm_pw[i] = toupper(pPassword[i]);
            else
                lm_pw[i] = '\0';

        AeWebAuthNTLMSetupDESKey(lm_pw, ks);
        des_ecb_encrypt(&pLMResponseMagic, (des_cblock *) lm_hpw, ks, 1);

        AeWebAuthNTLMSetupDESKey(&lm_pw[7], ks);
        des_ecb_encrypt(&pLMResponseMagic, (des_cblock *) (lm_hpw + 8), ks, 1);

        memset(lm_hpw + 16, 0, 5);
    }
    else
    {
        /* use null LM hash when the password is longer than 14 bytes */
        memset(lm_hpw, 0, 21);
    }

    AeWebAuthNTLMCalculateResponse(lm_hpw, pNonce, pResponse);
}

/******************************************************************************/
static void AeWebAuthNTLMGetNTLMResponse(AeChar *pPassword, AeUInt8 *pNonce, AeUInt8 *pResponse)
{
    AeUInt8 *nt_pw, nt_hpw[21];
    AeChar *pUCPassword;
    MD4_CTX ctx;
    AeInt len;

    len = AeWebAuthNTLMUnicodeString(pPassword, &pUCPassword);
    if (!pUCPassword)
        return;
    nt_pw = (AeUInt8 *) pUCPassword;

    MD4_Init(&ctx);
    MD4_Update(&ctx, nt_pw, len);
    MD4_Final(nt_hpw, &ctx);

    AeFree(pUCPassword);

    memset(nt_hpw + 16, 0, 5);

    AeWebAuthNTLMCalculateResponse(nt_hpw, pNonce, pResponse);
}


/******************************************************************************/
/*
 * turns a 56 bit key into the 64 bit, odd parity key and sets the key.
 * The key schedule ks is also set.
 */
static void AeWebAuthNTLMSetupDESKey(AeUInt8 *pKey56, des_key_schedule KS)
{
    des_cblock key;

    key[0] = pKey56[0];
    key[1] = ((pKey56[0] << 7) & 0xFF) | (pKey56[1] >> 1);
    key[2] = ((pKey56[1] << 6) & 0xFF) | (pKey56[2] >> 2);
    key[3] = ((pKey56[2] << 5) & 0xFF) | (pKey56[3] >> 3);
    key[4] = ((pKey56[3] << 4) & 0xFF) | (pKey56[4] >> 4);
    key[5] = ((pKey56[4] << 3) & 0xFF) | (pKey56[5] >> 5);
    key[6] = ((pKey56[5] << 2) & 0xFF) | (pKey56[6] >> 6);
    key[7] =  (pKey56[6] << 1) & 0xFF;

    des_set_odd_parity(&key);
    des_set_key(&key, KS);
}

/******************************************************************************/
/*
 * takes a 21 byte array and treats it as 3 56-bit DES keys. The
 * 8 byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
static void AeWebAuthNTLMCalculateResponse(AeUInt8 *pKeys, AeUInt8 *pPlainText, AeUInt8 *pResults)
{
    des_key_schedule ks;

    AeWebAuthNTLMSetupDESKey(pKeys, ks);
    des_ecb_encrypt((des_cblock*) pPlainText, (des_cblock*) pResults, ks, 1);

    AeWebAuthNTLMSetupDESKey(pKeys + 7, ks);
    des_ecb_encrypt((des_cblock*) pPlainText, (des_cblock*) (pResults + 8), ks, 1);

    AeWebAuthNTLMSetupDESKey(pKeys + 14, ks);
    des_ecb_encrypt((des_cblock*) pPlainText, (des_cblock*) (pResults + 16), ks, 1);
}

/******************************************************************************/
static void AeWebAuthNTLMParseUserString(AeChar *pUserString, AeChar **ppDomain, AeChar **ppUserName)
{
    AeChar *pDelimPtr;

    *ppDomain = NULL;
    *ppUserName = NULL;

    pDelimPtr = strchr(pUserString, '\\');
    if (pDelimPtr)
    {
        AeSetString(ppDomain, pUserString, (AeInt) (pDelimPtr - pUserString));
        AeSetString(ppUserName, pDelimPtr + 1, -1);
    }
    else
        AeSetString(ppUserName, pUserString, -1);
}

/******************************************************************************/
static void AeWebAuthNTLMUppercaseString(AeChar *pString)
{
    int iLength, i;

    iLength = strlen(pString);
    for (i = 0; i < iLength; i++)
        pString[i] = toupper(pString[i]);
}

/******************************************************************************/
static AeInt AeWebAuthNTLMUnicodeString(AeChar *pSrc, AeChar **ppDst)
{
    int iLength, i, j;

    iLength = strlen(pSrc);
    *ppDst = AeAlloc(iLength * 2);
    if (!(*ppDst))
        return -1;
    
    for (i = 0, j = 0; i < iLength; i += 1, j += 2)
    {
        (*ppDst)[j] = pSrc[i];
        (*ppDst)[j + 1] = '\0';
    }

    return (iLength * 2);
}

/******************************************************************************/
static void AeWebAuthNTLMUnqualifyHostname(AeChar *pHostname)
{
    char *pFirstDot;

    pFirstDot = strchr(pHostname, '.');
    if (pFirstDot)
        *pFirstDot = '\0';
}

/******************************************************************************/
static AeInt AeWebAuthInfoCompare(AePointer pData1, AePointer pData2)
{
    return !(!strcmp(((AeWebAuthInfo *) pData1)->pHost, ((AeWebAuthInfo *) pData2)->pHost) &&
        ((AeWebAuthInfo *) pData1)->iPort == ((AeWebAuthInfo *) pData2)->iPort &&
        (!((AeWebAuthInfo *) pData1)->pAbsPath || !((AeWebAuthInfo *) pData2)->pAbsPath ||
         !strcmp(((AeWebAuthInfo *) pData1)->pAbsPath, ((AeWebAuthInfo *) pData2)->pAbsPath)));
}

/******************************************************************************/
static void AeWebAuthInfoRelease(AePointer pObject)
{
    if (((AeWebAuthInfo *) pObject)->pHost)
        AeFree(((AeWebAuthInfo *) pObject)->pHost);
    if (((AeWebAuthInfo *) pObject)->pAbsPath)
        AeFree(((AeWebAuthInfo *) pObject)->pAbsPath);
    if (((AeWebAuthInfo *) pObject)->pParams &&
        ((AeWebAuthInfo *) pObject)->iScheme == AeWebAuthDigest)
    {
        AeWebAuthParamsDigest *pDigest;

        pDigest = (AeWebAuthParamsDigest *) ((AeWebAuthInfo *) pObject)->pParams;
        if (pDigest->pRealm)
            AeFree(pDigest->pRealm);
        if (pDigest->pNonce)
            AeFree(pDigest->pNonce);
        if (pDigest->pOpaque)
            AeFree(pDigest->pOpaque);
    }

    AeFree(pObject);
}

/******************************************************************************/
static void AeWebAuthGetNextParam(AeChar **ppPosition, AeChar **ppName, AeChar **ppValue)
{
    AeBool bGo, bInQuotes, bInValue;

    bGo = AeTrue;
    bInQuotes = AeFalse;
    bInValue = AeFalse;
    *ppName = NULL;
    *ppValue = NULL;

    while (bGo && **ppPosition)
    {
        switch (**ppPosition)
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                if (!bInQuotes)
                    **ppPosition = '\0';
                break;
            case ',':
                if (!bInQuotes)
                {
                    **ppPosition = '\0';
                    bGo = AeFalse;
                }
                break;
            case '=':
                if (!bInQuotes)
                {
                    **ppPosition = '\0';
                    bInValue = AeTrue;
                }
                break;
            case '"':
                **ppPosition = '\0';
                bInQuotes = !bInQuotes;
                break;
            default:
                if (!(*ppName))
                    *ppName = *ppPosition;
                if (bInValue && !(*ppValue))
                    *ppValue = *ppPosition;
        }

        *ppPosition += 1;
    }
}

/******************************************************************************/
static void AeWebAuthNTLMReadBuffer(AeChar *pMsg, AeUInt32 iMsgSize, AeUInt32 iHeaderOffset, AeChar **ppData, AeUInt16 *piSize)
{
    AeUInt32 iBufferOffset;

    *ppData = NULL;
    *piSize = 0;

    /* check for buffer overrun */
    if (iHeaderOffset + 8 > iMsgSize)
        return;

    /* length of the buffer content in bytes */
    memcpy(piSize, pMsg + iHeaderOffset, 2);
    *piSize = AeNetLEToHS(*piSize);

    /* offset to the start of the buffer in bytes (from the beginning of the message) */
    memcpy(&iBufferOffset, pMsg + iHeaderOffset + 4, 4);
    iBufferOffset = AeNetLEToHL(iBufferOffset);

    /* check for buffer overrun */
    if (iBufferOffset + *piSize > iMsgSize)
        return;

    if (*piSize)
    {
        /* allocate buffer */
        *ppData = AeAlloc(*piSize);
        if (!(*ppData))
            return;

        /* copy buffer content */
        memcpy(*ppData, pMsg + iBufferOffset, *piSize);
    }
}

/******************************************************************************/
static void AeWebAuthNTLMWriteBuffer(AeArray *pMsg, AeUInt32 iHeaderSize, AeUInt32 iHeaderOffset, AeChar *pData, AeUInt16 iSize)
{
    AeUInt16 iBufferSize;
    AeUInt32 iBufferOffset;

    /* check for buffer overrun */
    if (iHeaderOffset + 8 > iHeaderSize)
        return;

    iBufferSize = AeNetHToLES(iSize);

    if (pData && iSize > 0)
    {
        if ((AeUInt32) AeArrayCount(pMsg) <= iHeaderSize)
            iBufferOffset = AeNetHToLEL(iHeaderSize);
        else
            iBufferOffset = AeNetHToLEL(AeArrayCount(pMsg));
    }
    else
        iBufferOffset = 0;

    /* length of the buffer content in bytes */
    AeArraySet(pMsg, iHeaderOffset, &iBufferSize, 2);
    
    /* allocated space for the buffer in bytes (same as the length) */
    AeArraySet(pMsg, iHeaderOffset + 2, &iBufferSize, 2);

    /* offset to the start of the buffer in bytes (from the beginning of the message) */
    AeArraySet(pMsg, iHeaderOffset + 4, &iBufferOffset, 4);

    /* write the buffer if non-empty */
    if (pData && iSize > 0)
        AeArraySet(pMsg, AeNetLEToHL(iBufferOffset), pData, iSize);
}

/******************************************************************************/
static void AeWebAuthNTLMWriteStringBuffer(AeArray *pMsg, AeUInt32 iHeaderSize, AeUInt32 iHeaderOffset, AeChar *pStr, AeUInt32 iFlags)
{
    if (!pStr || pStr[0] == '\0')
    {
        AeWebAuthNTLMWriteBuffer(pMsg, iHeaderSize, iHeaderOffset, NULL, 0);
        return;
    }

    if (iFlags & HTTP_NTLM_FLAG_NEG_UNICODE)
    {
        AeChar *pUCStr;
        AeInt iUCStrLen;
        
        iUCStrLen = AeWebAuthNTLMUnicodeString(pStr, &pUCStr);
        if (!pUCStr)
            return;

        AeWebAuthNTLMWriteBuffer(pMsg, iHeaderSize, iHeaderOffset, pUCStr,
                                 (AeUInt16) iUCStrLen);

        AeFree(pUCStr);
    }
    else
        AeWebAuthNTLMWriteBuffer(pMsg, iHeaderSize, iHeaderOffset, pStr,
                                 (AeUInt16) strlen(pStr));
}

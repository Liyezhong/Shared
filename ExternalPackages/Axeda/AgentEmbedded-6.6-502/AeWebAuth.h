/* $Id: AeWebAuth.h,v 1.4 2008/05/21 18:24:35 dkhodos Exp $ */

#ifndef _AE_WEB_AUTH_H_
#define _AE_WEB_AUTH_H_

#define HTTP_HEADER_AUTHORIZATION       "Authorization"
#define HTTP_HEADER_PROXYAUTHORIZATION  "Proxy-Authorization"

#define HTTP_AUTH_SCHEME_BASIC          "Basic"
#define HTTP_AUTH_SCHEME_DIGEST         "Digest"
#define HTTP_AUTH_SCHEME_NTLM           "NTLM"
#define HTTP_AUTH_SCHEME_NEGOTIATE      "Negotiate"

#define HTTP_NTLM_SIGN                  "NTLMSSP"

#define HTTP_NTLM_HEADER_SIZE_TYPE1     32
#define HTTP_NTLM_HEADER_SIZE_TYPE3     64

#define HTTP_NTLM_FLAG_NEG_UNICODE      0x00000001
#define HTTP_NTLM_FLAG_NEG_OEM          0x00000002
#define HTTP_NTLM_FLAG_REQ_TARGET       0x00000004
#define HTTP_NTLM_FLAG_NEG_NTLM         0x00000200
#define HTTP_NTLM_FLAG_NEG_DOMAIN       0x00001000
#define HTTP_NTLM_FLAG_NEG_WORKSTATION  0x00002000

typedef enum _AeWebAuthScheme AeWebAuthScheme;
typedef struct _AeWebAuthInfo AeWebAuthInfo;
typedef struct _AeWebAuthParamsDigest AeWebAuthParamsDigest;
typedef struct _AeWebAuthParamsNTLM AeWebAuthParamsNTLM;

enum _AeWebAuthScheme
{
    AeWebAuthNone,
    AeWebAuthBasic,
    AeWebAuthDigest,
    AeWebAuthNTLM
};

struct _AeWebAuthInfo
{
    AeChar          *pHost;
    AeUInt16        iPort;
    AeChar          *pAbsPath;
    AeWebAuthScheme iScheme;
    AePointer       pParams;
};

struct _AeWebAuthParamsDigest
{
    AeChar  *pRealm;
    AeChar  *pNonce;
    AeChar  *pOpaque;
};

struct _AeWebAuthParamsNTLM
{
    AeUInt8 *pNonce;
    AeUInt32 iFlags;
    AeChar *pTargetName;
    AeUInt16 iTargetNameSize;
};

#ifdef __cplusplus
extern "C" {
#endif

AeBool  AeWebAuthInfoGet(AeChar *pHost, AeUInt16 iPort, AeChar *pAbsPath, AeWebAuthScheme *piScheme, AePointer *ppParams);
void    AeWebAuthInfoSet(AeChar *pHost, AeUInt16 iPort, AeChar *pAbsPath, AeWebAuthScheme iScheme, AePointer pParams);
void    AeWebAuthMakeRequestHeaders(AeWebHeaders *pHeaders, AeBool bProxy, AeWebAuthScheme iScheme, AePointer pParams, AeWebRequest *pRequest, AeChar *pMethod, AeChar *pUser, AeChar *pPassword);
AeBool  AeWebAuthParseChallenge(AeChar *pChallenge, AeWebAuthScheme *piScheme, AePointer *ppParams);

#ifdef __cplusplus
}
#endif

#endif

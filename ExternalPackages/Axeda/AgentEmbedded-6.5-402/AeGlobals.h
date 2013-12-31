/* $Id: AeGlobals.h,v 1.11.2.3 2010/08/13 16:01:38 dkhodos Exp $ */

#ifndef _AE_GLOBALS_H_
#define _AE_GLOBALS_H_

#ifdef _AE_DEFINE_GLOBALS_
    #define AE_GLOBAL_DECL
    #define AE_GLOBAL_INITIALIZER = 0
    #define AE_GLOBAL_ERROR_STRINGS = \
{\
    "Success",\
    "Internal error",\
    "Invalid argument",\
    "Insufficient memory",\
    "Object already exists",\
    "General network error",\
    "Connection timed out",\
    "Operation would block",\
    "Unknown host",\
    "Connection with server lost",\
    "Connection refused",\
    "Connection reset by peer",\
    "Connection aborted",\
    "Not connected",\
    "Bad HTTP response",\
    "HTTP authentication failed",\
    "HTTP authentication method not supported",\
    "General SSL error",\
    "Weaker SSL cipher negotiated",\
    "Server certificate issuer unknown",\
    "Server certificate invalid",\
    "Server certificate validation failed",\
    "SSL handshake failed",\
    "SOCKS: wrong protocol version",\
    "SOCKS: authentication failed",\
    "SOCKS: general SOCKS server failure",\
    "SOCKS: connection not allowed by ruleset",\
    "SOCKS: network unreachable",\
    "SOCKS: host unreachable",\
    "SOCKS: connection refused",\
    "SOCKS: TTL expired",\
    "SOCKS: command not supported",\
    "SOCKS: address type not supported",\
    "Network unreachable",\
    "Host unreachable",\
    "Bad URL"\
}
    #define AE_GLOBAL_ERRORS_COUNT = sizeof(g_pErrorStrings) / sizeof(AeChar *)
    #define AE_GLOBAL_LOG_CATEGORY_STRINGS = \
{\
    "NTWK",\
    "SRVR",\
    "DATQ",\
    "REMT",\
    "FXFR",\
    "UPLD",\
    "DNLD",\
    "RSTR",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    ",\
    "    "\
}
#else
    #define AE_GLOBAL_DECL extern
    #define AE_GLOBAL_INITIALIZER
    #define AE_GLOBAL_ERROR_STRINGS
    #define AE_GLOBAL_ERRORS_COUNT
    #define AE_GLOBAL_LOG_CATEGORY_STRINGS
#endif

#define AE_ERROR_STRING(x) ((x) >= 0 && (x) < g_iErrorsNumber ? g_pErrorStrings[(x)] : NULL)

AE_GLOBAL_DECL AeChar           *g_pErrorStrings[] AE_GLOBAL_ERROR_STRINGS;
AE_GLOBAL_DECL AeInt            g_iErrorsNumber AE_GLOBAL_ERRORS_COUNT;
AE_GLOBAL_DECL void             (*g_pLogFunc)(AeLogRecordType iType, AeChar *pFormat, ...) AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL void             (*g_pLogExFunc)(AeLogRecordType iType, AeUInt32 iCategory, AeChar *pFormat, ...) AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeChar           *g_pLogCategoryStrings[] AE_GLOBAL_LOG_CATEGORY_STRINGS;
AE_GLOBAL_DECL AeWebSettings    g_webSettings;
AE_GLOBAL_DECL AeWebProxyInfo   *g_pWebProxySOCKS AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeWebProxyInfo   *g_pWebProxyHTTP AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeList           *g_pHostNameCache AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeMutex          g_hostNameCacheLock;
AE_GLOBAL_DECL AeList           *g_pWebAuthCache AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeMutex          g_webAuthCacheLock;
#if defined(HAVE_OPENSSL) && defined(ENABLE_SSL)
AE_GLOBAL_DECL AeWebSSLInfo     g_webSSLInfo;
AE_GLOBAL_DECL AeList           *g_pWebSSLSessionCache AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeMutex          g_webSSLSessionCacheLock;
#endif

AE_GLOBAL_DECL AeDRMSettings    g_drmSettings;
AE_GLOBAL_DECL AeDRMCallbacks   g_drmCallbacks;
AE_GLOBAL_DECL AePtrArray       *g_pDRMDevices AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AePtrArray       *g_pDRMServerConfigs AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeList           *g_pDRMServerLocations AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeDRMQueue       *g_pQueue AE_GLOBAL_INITIALIZER;

#if defined(ENABLE_REMOTE_SESSION)
AE_GLOBAL_DECL AeList           *g_pRemoteInterfaces AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeList			*g_pRemoteSessions AE_GLOBAL_INITIALIZER;
#endif /* defined(ENABLE_REMOTE_SESSION) */

#if defined(ENABLE_FILE_TRANSFER)
#ifdef _AE_DEFINE_GLOBALS_
    #define AE_GLOBAL_COMPRESSION_STRINGS = \
{\
    "none",\
    "tar-gzip"\
}
#else
    #define AE_GLOBAL_COMPRESSION_STRINGS
#endif
AE_GLOBAL_DECL AeList			*g_pFileTransferPackages AE_GLOBAL_INITIALIZER;
AE_GLOBAL_DECL AeChar           *g_pCompressionStrings[] AE_GLOBAL_COMPRESSION_STRINGS;
#endif /* defined(ENABLE_REMOTE_SESSION) */


#endif

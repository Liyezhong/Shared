/* $Id: Ae.h,v 1.15.2.3 2010/08/13 16:01:38 dkhodos Exp $ */

#ifndef _AE_H_
#define _AE_H_

#ifdef __cplusplus
#define AE_EXTERN extern "C"
#else
#define AE_EXTERN extern
#endif

#include "AeOSLocal.h"

#if defined(HAVE_OPENSSL)
#define OPENSSL_DES_LIBDES_COMPATIBILITY
#if defined(ENABLE_SSL)
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#ifndef AXIS_LINUX
#include <openssl/md4.h>
#endif

#include <openssl/md5.h>
#include <openssl/des.h>
#endif

#include "xmlparse.h"

#include "AeCompat.h"
#include "AeTypes.h"
#include "AeError.h"
#include "AeOS.h"
#include "AeContainers.h"
#include "AeUtil.h"
#include "AeWebRequest.h"
#include "AeWebCommon.h"
#include "AeWebConnection.h"
#include "AeWebConnectionSOCKS.h"
#include "AeWebAuth.h"
#include "AeWebTransaction.h"
#include "AeWebUserAgent.h"
#include "AeXML.h"
#include "AeDRMSOAP.h"
#include "AeDRMEMessage.h"
#include "AeDRMQueue.h"
#include "AeDRM.h"
#include "AeGlobals.h"
#include "AeVersion.h"

#if defined(ENABLE_REMOTE_SESSION)
#include "AeRemoteSession.h"
#include "AeRemoteChannel.h"
#include "AeRemoteHTTPTransport.h"
#include "AeRemoteDirectConnection.h"
#include "AeRemoteDirectTransport.h"
#include "AeRemoteDesktopProbe.h"
#endif

#if defined(ENABLE_FILE_TRANSFER)
#include "zlib.h"
#include "AeGzip.h"
#include "AeTar.h"
#include "AeNullFileFilter.h"
#include "AeFileTransfer.h"
#include "AeFileDownload.h"
#include "AeFileUpload.h"
#include "AeAgentRestart.h"
#endif

#endif

/* $Id: AeFileDownload.h,v 1.7 2008/05/21 18:24:34 dkhodos Exp $ */

#ifndef _AE_FILE_DOWNLOAD_H_
#define _AE_FILE_DOWNLOAD_H_

#define AE_FILE_DOWNLOAD_URL_FORMAT "%s?mn=%s&sn=%s&ow=%s&session_id=%s" \
                                    "&position=%" AE_SIZE_FORMAT_MODIFIER "d" \
                                    "&size=%" AE_INT32_FORMAT_MODIFIER "d"

typedef enum _AeFileDownloadMode AeFileDownloadMode;
typedef struct _AeFileDownload AeFileDownload;
typedef struct _AeFileDownloadSpec AeFileDownloadSpec;

enum _AeFileDownloadMode
{
    AeFileDownloadModeOverwrite,
    AeFileDownloadModeUpgrade
};

struct _AeFileDownloadSpec
{
    AeChar   *pName;
};

/* all delays are in milliseconds */
struct _AeFileDownload
{
    AeFileInstruction parent;
    AeChar    *pURL;
    AeChar    *pSessionId;
    AeChar    *pId;
#ifndef ENABLE_LARGEFILE64
    AeInt32   iDownloadSize;
#else
    AeInt64   iDownloadSize;
#endif
    AeUInt8   pCheckSum[MD5_DIGEST_LENGTH];
    AeInt32   iMaxRetryCount;
    AeInt32   iRetryMinDelay;
    AeInt32   iRetryMaxDelay;
    AeInt32   iChunkSize;
    AeInt32   iChunkDelay;
    AeFileDownloadMode iMode;
    AeFileCompression iCompression;
    AeChar    *pDestPath;
    AeList    *pFileList;

    AePointer pFileFilter;
    AeFileFilterHandler *pFileFilterHandler;
    MD5_CTX   md5Global;
    MD5_CTX   md5Request;
    AeWebRequest *pRequest;
    AeInt32   iRetryCount;
#ifndef ENABLE_LARGEFILE64
    AeInt32   iDownloadPos;
#else
    AeInt64   iDownloadPos;
#endif
    AeFileStat *pCurFileStat;
    AeFileHandle iCurFileHandle;
    AeBool    bUserCallback;
    AePointer pUserData;
};

#ifdef __cplusplus
extern "C" {
#endif

AeFileDownload *AeFileDownloadNew(void);

#ifdef __cplusplus
}
#endif

/* _AE_FILE_DOWNLOAD_H_ */
#endif

/* $Id: AeFileUpload.h,v 1.15 2008/05/21 18:24:34 dkhodos Exp $ */

#ifndef _AE_FILE_UPLOAD_H_
#define _AE_FILE_UPLOAD_H_

#define AE_FILE_UPLOAD_URL_FORMAT  "%s?mn=%s&sn=%s&ow=%s&session_id=%s&checksum=%s" \
                                   "&position=%" AE_LONGINT_FORMAT_MODIFIER "d" \
                                   "&total=%" AE_LONGINT_FORMAT_MODIFIER "d" \
                                   "&compression=%s"

typedef struct _AeFileUpload AeFileUpload;
typedef struct _AeFileUploadSource AeFileUploadSource;

struct _AeFileUpload
{
    AeFileInstruction parent;
    AeChar    *pURL;
    AeChar    *pSessionId;
    AeChar    *pId;
    AeInt32   iMaxRetryCount;
    AeInt32   iRetryMinDelay;
    AeInt32   iRetryMaxDelay;
    AeInt32   iChunkSize;
    AeInt32   iChunkDelay;
    AeFileCompression iCompression;
    AeBool    bNoOverallChecksum;
    AeBool    bMultiFile;
    AeFileUploadSpec **ppUploads;
    AeFileUploadExecuteParam param;

    AePointer  pFileFilter;
    AeFileFilterHandler *pFileFilterHandler;
    MD5_CTX    md5Global;
    AeWebRequest *pRequest;
    AeInt32    iRetryCount;
#ifndef ENABLE_LARGEFILE64
    AeInt32    iStartPos;
    AeInt32    iUploadPos;
    AeInt32    iCurFilePos;
#else
    AeInt64    iStartPos;
    AeInt64    iUploadPos;
    AeInt64    iCurFilePos;
#endif
    AeFileStat curFileStat;
    AeChar     *pEncodedData;
    AeInt32    iEncodedSize;
    AeBool     bUserCallback;
    AePointer  pUserData;
    AeFileUploadSource *pSource;
    AeBool     bDeferred;
    AeTimeValue lastProcessed;
    AeTimeValue retryWait;
};

struct _AeFileUploadSource
{
    AeFileUploadSpec **ppUploads;
    AeInt32          iUploadIdx;
    AeFileStat       curFileStat;
    AeFileHandle     iCurFileHandle;
    AeChar           pBuffer[AE_BUFFER_CHUNK];
};

#ifdef __cplusplus
extern "C" {
#endif

AeFileUpload *AeFileUploadNew(void);

#ifdef __cplusplus
}
#endif

/* _AE_FILE_UPLOAD_H_ */
#endif

/* $Id: AeNullFileFilter.h,v 1.2 2008/05/21 18:24:34 dkhodos Exp $ */

#ifndef _AE_NULL_FILE_FILTER_H_
#define _AE_NULL_FILE_FILTER_H_

typedef enum _AeNullFileFilterError AeNullFileFilterError;
typedef enum _AeNullFileFilterState AeNullFileFilterState;
typedef struct _AeNullFileFilter AeNullFileFilter;

enum _AeNullFileFilterError
{
    AeNullFileFilterEOK,
    AeNullFileFilterEMemory,
    AeNullFileFilterEDirection,
    AeNullFileFilterENoFile,
    AeNullFileFilterEMultipleFiles
};

enum _AeNullFileFilterState
{
    AeNullFileFilterStateHead,
    AeNullFileFilterStateData,
    AeNullFileFilterStateEnd
};

struct _AeNullFileFilter
{
    AeBool                  bEncode;
    AeNullFileFilterError   iError;
    AeNullFileFilterState   iState;
    AeFileStat              *pFile;
#ifndef ENABLE_LARGEFILE64
    AeInt32                 iWriteCount;
#else
    AeInt64                 iWriteCount;
#endif
    AeChar                  *pChunk;
    AeInt32                 iChunkSize;
    AeInt32                 iReadPos;
};

#ifdef __cplusplus
extern "C" {
#endif

AeNullFileFilter *AeNullFileFilterNew(AeBool bEncode);
AeBool  AeNullFileFilterInitialize(AeNullFileFilter *pFilter, AeFileStat *pFile);
void    AeNullFileFilterReset(AeNullFileFilter *pFilter);
void    AeNullFileFilterDestroy(AeNullFileFilter *pFilter);
AeBool  AeNullFileFilterIsEndOfFile(AeNullFileFilter *pFilter);
AeBool  AeNullFileFilterIsEndOfStream(AeNullFileFilter *pFilter);

AeInt32 AeNullFileFilterWriteRaw(AeNullFileFilter *pFilter, AeChar *pData, AeInt32 iSize);
AeBool  AeNullFileFilterReadFile(AeNullFileFilter *pFilter, AeFileStat **ppFile);
AeInt32 AeNullFileFilterReadFileData(AeNullFileFilter *pFilter, AeChar **ppData);

AeInt32 AeNullFileFilterReadRaw(AeNullFileFilter *pFilter, AeChar *pData, AeInt32 iSize);
AeBool  AeNullFileFilterWriteFile(AeNullFileFilter *pFilter, AeFileStat *pFile);
AeInt32 AeNullFileFilterWriteFileData(AeNullFileFilter *pFilter, AeChar *pData, AeInt32 iSize);

AeNullFileFilterError AeNullFileFilterGetError(AeNullFileFilter *pFilter);
AeChar  *AeNullFileFilterGetErrorString(AeNullFileFilter *pFilter, AeNullFileFilterError iError);

AE_EXTERN AeFileFilterHandler g_nullFileFilterHandler;

#ifdef __cplusplus
}
#endif

#endif

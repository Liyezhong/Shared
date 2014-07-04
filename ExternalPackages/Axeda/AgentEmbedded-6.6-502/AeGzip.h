/* $Id: AeGzip.h,v 1.6 2008/05/21 18:24:34 dkhodos Exp $ */

#ifndef _AE_GZIP_H_
#define _AE_GZIP_H_

#define GZIP_MAGIC_1      0x1f
#define GZIP_MAGIC_2      0x8b

#define GZIP_HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define GZIP_EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define GZIP_ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define GZIP_COMMENT      0x10 /* bit 4 set: file comment present */

#define GZIP_OS_CODE      0x03 /* assume Unix */

#ifdef m68k
#define GZIP_WBITS        12
#define GZIP_MEM_LEVEL    5
#else
#define GZIP_WBITS        MAX_WBITS
#define GZIP_MEM_LEVEL    MAX_MEM_LEVEL
#endif

#define GZIP_DEFAULT_COMPRESSION (-1)
#define GZIP_NO_COMPRESSION      0

typedef enum _AeGzipError AeGzipError;
typedef enum _AeGzipState AeGzipState;
typedef struct _AeGzipFilter AeGzipFilter;

enum _AeGzipError
{
    AeGzipEOK,
    AeGzipEMemory,
    AeGzipEBadMagic,
    AeGzipEBadMethod,
    AeGzipEBadBlock,
    AeGzipEBadCRC,
    AeGzipEBadLength
};

enum _AeGzipState
{
    AeGzipStateHeader,
    AeGzipStateBlocks,
    AeGzipStateTail,
    AeGzipStateEnd
};

struct _AeGzipFilter
{
    z_stream    *pZStream;
    AeBool      bCompress;
    AeGzipError iError;
    AeGzipState iState;
    AeBool      bFinish;
    AeUInt32    iCRC;
    AeBuffer    *pBuffer;
};

#ifdef __cplusplus
extern "C" {
#endif

AeGzipFilter *AeGzipFilterNew(AeBool bCompress);
AeBool       AeGzipFilterInitialize(AeGzipFilter *pFilter);
void         AeGzipFilterReset(AeGzipFilter *pFilter);
void         AeGzipFilterDestroy(AeGzipFilter *pFilter);
AeInt32      AeGzipFilterWrite(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize);
AeInt32      AeGzipFilterRead(AeGzipFilter *pFilter, AeChar *pData, AeInt32 iSize);
AeBool       AeGzipFilterIsEnd(AeGzipFilter *pFilter);

void         AeGzipFilterSetLevel(AeGzipFilter *pFilter, AeInt32 iLevel);

AE_EXTERN AeFilterHandler g_gzipFilterHandler;

#ifdef __cplusplus
}
#endif

#endif

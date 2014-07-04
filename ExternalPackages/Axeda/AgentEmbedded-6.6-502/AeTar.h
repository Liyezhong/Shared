/* $Id: AeTar.h,v 1.2 2008/05/21 18:24:34 dkhodos Exp $ */

#ifndef _AE_TAR_H_
#define _AE_TAR_H_

#define TAR_BLOCK_SIZE 512
#define TAR_TMAGIC     "ustar"
#define TAR_TVERSION   "00"
#define TAR_REGTYPE    '0'
#define TAR_AREGTYPE   '\0'
#define TAR_DIRTYPE    '5'

#define AE_TAR_DEFAULT_MODE  "0000777"
#define AE_TAR_DEFAULT_UID   "0000000"
#define AE_TAR_DEFAULT_GID   "0000000"
#define AE_TAR_DEFAULT_UNAME "root"
#define AE_TAR_DEFAULT_GNAME "root"

typedef enum _AeTarError AeTarError;
typedef enum _AeTarState AeTarState;
typedef struct _AeTarHeader AeTarHeader;
typedef struct _AeTar AeTar;

enum _AeTarError
{
    AeTarEOK,
    AeTarEMemory,
    AeTarEDirection,
    AeTarEBadMagic,
    AeTarEBadCRC,
    AeTarEBadFile,
    AeTarEIncomplete,
    AeTarEFilter
};

enum _AeTarState
{
    AeTarStateHeader,
    AeTarStateData,
    AeTarStateTail,
    AeTarStateEnd
};

struct _AeTarHeader
{
    char name[100];     /* NUL-terminated if NUL fits */
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
    char linkname[100]; /* NUL-terminated if NUL fits */
    char magic[6];      /* must be TMAGIC (NUL term.) */
    char version[2];    /* must be TVERSION */
    char uname[32];     /* NUL-terminated */
    char gname[32];     /* NUL-terminated */
    char devmajor[8];
    char devminor[8];
    char prefix[155];   /* NUL-terminated if NUL fits */
};

struct _AeTar
{
    AeFilterHandler *pHandler;
    void            *pFilter;
    AeBool          bCreate;
    AeTarError      iError;
    AeTarState      iState;
    AeFileStat      curFile;
#ifndef ENABLE_LARGEFILE64
    AeInt32         iCurFilePos;
#else
    AeInt64         iCurFilePos;
#endif
    AeBuffer        *pBuffer;
};

#ifdef __cplusplus
extern "C" {
#endif

AeTar   *AeTarNew(AeBool bCreate, AeFilterHandler *pHandler);
AeBool  AeTarInitialize(AeTar *pTar);
void    AeTarReset(AeTar *pTar);
void    AeTarDestroy(AeTar *pTar);
AeBool  AeTarIsEOT(AeTar *pTar);
AeBool  AeTarIsEOF(AeTar *pTar);

AeInt32 AeTarWriteRaw(AeTar *pTar, AeChar *pData, AeInt32 iSize);
AeBool  AeTarReadFile(AeTar *pTar, AeFileStat **ppFile);
AeInt32 AeTarReadFileData(AeTar *pTar, AeChar **ppData);

AeInt32 AeTarReadRaw(AeTar *pTar, AeChar *pData, AeInt32 iSize);
AeBool  AeTarWriteFile(AeTar *pTar, AeFileStat *pFile);
AeInt32 AeTarWriteFileData(AeTar *pTar, AeChar *pData, AeInt32 iSize);

AeInt32 AeTarGetError(AeTar *pTar);
AeChar  *AeTarGetErrorString(AeTar *pTar, AeTarError iError);

AE_EXTERN AeFileFilterHandler g_tarFileFilterHandler;

#ifdef __cplusplus
}
#endif

#endif

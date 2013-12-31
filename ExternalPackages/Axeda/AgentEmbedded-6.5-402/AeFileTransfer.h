/* $Id: AeFileTransfer.h,v 1.14 2008/05/21 18:24:34 dkhodos Exp $ */
#ifndef _AE_FILE_TRANSFER_H_
#define _AE_FILE_TRANSFER_H_

/* Attributes for file transfer */
#define AE_FILETRANSFER_JOB_ID   "jobId"
#define AE_FILETRANSFER_PUB_ID   "id"
#define AE_FILETRANSFER_URL    "url"
#define AE_FILETRANSFER_SESSION_ID  "sessionId"
#define AE_FILETRANSFER_SIZE   "size"
#define AE_FILETRANSFER_CHECKSUM  "checksum"
#define AE_FILETRANSFER_RETRY_COUNT  "retryCount"
#define AE_FILETRANSFER_MIN_DELAY  "retryMinDelay"
#define AE_FILETRANSFER_MAX_DELAY  "retryMaxDelay"
#define AE_FILETRANSFER_CHUNK_SIZE  "chunkSize"
#define AE_FILETRANSFER_CHUNK_DELAY  "chunkDelay"
#define AE_FILETRANSFER_POSITION  "position"
#define AE_FILETRANSFER_PRIORITY  "priority"
#define AE_FILETRANSFER_COMPRESSION  "compression"
#define AE_FILETRANSFER_CLIENT_ID  "clientId"
#define AE_FILETRANSFER_REQUIRE_OVERALL_MD5  "requireOverallMD5"
#define AE_FILETRANSFER_MODE   "mode"
#define AE_FILETRANSFER_PATH   "path"
#define AE_FILETRANSFER_POSITION  "position"
#define AE_FILETRANSFER_NAME   "name"
#define AE_FILETRANSFER_DELETE   "delete"
#define AE_FILETRANSFER_STATUS_CODE  "sc"
#define AE_FILETRANSFER_ERROR_CODE  "ec"
#define AE_FILETRANSFER_INSTRUCTION  "ii"

#define AE_FILETRANSFER_SOAPBODY  "SOAP-ENV:Body"
#define AE_FILETRANSFER_DOWNLOAD  "FileTransfer.Download"
#define AE_FILETRANSFER_UPLOAD   "FileTransfer.Upload"
#define AE_PACKAGE_RESTART      "Package.Rs"
#define AE_FILETRANSFER_FILE_LIST "FileList"
#define AE_FILETRANSFER_FILE   "File"
#define AE_FILETRANSFER_DESTINATION  "Destination"
#define AE_FILETRANSFER_POST_STATUS  "Ps"

#define HTTP_CONTENT_MD5 "Content-MD5"

#define AE_FILETRANSFER_MAX_URL_LENGTH  1024

#define AXEDA_BACKUP    ".\\AxedaBackup"

typedef enum _AeFileInstructionState AeFileInstructionState;
typedef enum _AeFileTransferStatus AeFileTransferStatus;
typedef enum _AeFileTransferError AeFileTransferError;

/****************************************************************************/
enum _AeFileInstructionState
{
    AeFileInstructionInitialized,
    AeFileInstructionExecuting,
    AeFileInstructionFinished
};

/****************************************************************************/
/* STATUS CODE. These numbers MUST correspond to values on the Enterprise Server */
enum _AeFileTransferStatus
{
    AeQUEUED = 0,
    AeSTARTED = 1,
    AeSUCCESS = 2,
    AeERROR = 3,
    AeCANCELLED = 4,
    AePAUSED = 17,

    // the following statuses are internal, i.e. not reported to the server
    AePREEMPTED = 19,
    AePREEMPTION_PENDING,
    AeCANCELLATION_PENDING,
    AePAUSE_PENDING
};

/* ERROR CODE. These numbers MUST correspond to values on the Enterprise Server */
/* ERROR CODE is posted to DRM if STATUS CODE == AeERROR */
enum _AeFileTransferError
{
    AeFAILED = 0,
    AeVERSION,
    AeBAD_FORMAT,
    AeUNKNOWN_SOAP_METHOD,
    AeUNSUPPORTED_FUNCTION,          /* for example, data component not found. */
    AeDI_NAME_NOT_FOUND,             /* unknown data item name in dependency. */
    AeREGISTRY_NAME_NOT_FOUND,       /* unknown registry entry name */
    AeREGISTRY_FILE_READ_ERROR,      /* Failed in reading or parsing registry file. */
    AeINVALID_DEPENDENCY_EXPRESSION, /* The dependency is invalid.*/
    AeNO_FILES_FOUND,                /* upload: no explicitly defined files found,*/
    AeSOME_FILES_NOT_FOUND,          /* upload: only some of the explicitly defined files were found */
                                     /* and sent to the Server and some NOT found.*/
    AeDOWNLOAD_EXECUTION_FAILURE,    /* Failed to execute 1st file in download.*/
    AeARCHIVE_ERROR,                 /* Archive failed:  creation, extraction, list, file name etc.*/
    AeREAD_ERROR,                    /* failure during file read of archive being uploaded.*/
    AeHTTP_STATUS_ERROR,             /* HTTP status error (see EPackageMgr::CheckHTTPStatus()) */
    AeCHUNK_CHECKSUM_ERROR,          /* Chunk Checksum (MD5) */
    AeCONNECTION_FAILURE,            /* Web Client connection error */
    AeSOCKS_FAILURE,                 /* Web Client SOCKS failure */
    AeHTTP_FAILURE,                  /* Web Client HTTP failure */
    AeSSL_FAILURE,                   /* Web Client SSL failure */
    AeAGENT_SHUTDOWN,                /* The Agent has been stopped. */
    AeFILE_CHECKSUM_ERROR,           /* FILE Checksum (MD5) error during file download. */
    AeUPLOAD_FILE_MISSING_ERROR,     /* A "partial" uploaded file is missing at the upload request.*/
    AeINVALID_DIRECTORY,             /* The requested directory can not be created.*/
    AeRESTART_OF_GATEWAY_DEVICE,     /* Can not restart a Gateway device (only the Gateway itself).*/
    AeCUSTOM_SOAP_FUNCTION_ERROR,    /* General Purpose use by custom SOAP functions that have an error.*/
    AeNOERROR,
    AeMULTIFILE_UNCOMPRESSED         /* Multi-file transfer without compression requested. */
};

/* structs for file transfer */
typedef struct _AeFileTransferPackage   AeFileTransferPackage;
typedef struct _AeFileInstruction       AeFileInstruction;
typedef struct _AeFileInstructionVTable AeFileInstructionVTable;

/* file transfer package to store file transfer parameters */
struct _AeFileTransferPackage
{
    AeInt32   iDeviceId;
    AeInt32   iConfigId;
    AeChar    *pJobId;
    AeChar    *pPublishedId;
    AeInt32   iPriority;
    AeInt32   iInstruction;
    AeList    *pInstructionList;
    AeFileTransferStatus iStatus;
    AeFileTransferError iError;
};

struct _AeFileInstruction
{
    AeFileInstructionVTable *pVTable;
    AeFileTransferPackage   *pPackage;
    AeFileInstructionState iState;
    AeFileTransferError iError;
};

struct _AeFileInstructionVTable
{
    AeBool (*pInitialize)(AeFileInstruction *pInstruction, AeHandle pParameters);
    void   (*pProcess)(AeFileInstruction *pInstruction);
    void   (*pDestroy)(AeFileInstruction *pInstruction);
    void   (*pNotify)(AeFileInstruction *pInstruction, AeFileTransferEvent iEvent);
    AeBool (*pIsIdle)(AeFileInstruction *pInstruction);
    AeBool (*pIsPendingRequest)(AeFileInstruction *pInstruction);
};

#define AeFileInstructionInitialize(pInstr, pParams) \
    (*(pInstr)->pVTable->pInitialize)(pInstr, pParams)
#define AeFileInstructionProcess(pInstr) \
    (*(pInstr)->pVTable->pProcess)(pInstr)
#define AeFileInstructionDestroy(pInstr) \
    (*(pInstr)->pVTable->pDestroy)(pInstr)
#define AeFileInstructionNotify(pInstr, iEvent) \
    (*(pInstr)->pVTable->pNotify)(pInstr, iEvent)
#define AeFileInstructionIsIdle(pInstr) \
    (*(pInstr)->pVTable->pIsIdle)(pInstr)
#define AeFileInstructionIsPendingRequest(pInstr) \
    (*(pInstr)->pVTable->pIsPendingRequest)(pInstr)

#ifdef __cplusplus
extern "C" {
#endif

/* entry point for file transfer module */
AeBool AeFileTransferStart(AeInt32 iDeviceId, AeInt32 iConfigId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus);
AeBool AeFileTransferStop(AeInt32 iDeviceId, AeHandle pMethod, AeBool bPause, AeDRMSOAPCommandStatus *pStatus);
void   AeFileTransferProcess(void);
void   AeFileTransferPackageDestroy(AeFileTransferPackage *pPackage);
AeInt  AeFileTransferPackageCompare(AePointer pData1, AePointer pData2);
void   AeFileTransferPackageSetPriority(AeFileTransferPackage *pPackage, AeInt32 iPriority);
AeBool AeFileTransferIsIdle(void);

AeFileTransferError AeFileTransferTranslateError(AeError iError);

#ifdef __cplusplus
}
#endif

/* _AE_FILE_TRANSFER_H_ */
#endif

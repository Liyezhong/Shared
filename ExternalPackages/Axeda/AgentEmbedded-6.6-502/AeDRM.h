/* $Id: AeDRM.h,v 1.11.2.1 2009/08/19 21:27:59 dkhodos Exp $ */

#ifndef _AE_DRM_H_
#define _AE_DRM_H_

#define AE_DRM_MASTER_DEVICE_ID     0
#define AE_DRM_INVALID_DEVICE_ID    -1

#define AE_DRM_PRIMARY_CONFIG_ID    0
#define AE_DRM_BACKUP_CONFIG_ID     1

#define AE_DRM_SOAP_ATTR_MN     "mn"
#define AE_DRM_SOAP_ATTR_SN     "sn"
#define AE_DRM_SOAP_ATTR_CMDID  "cmdId"
#define AE_DRM_SOAP_ATTR_USERID "userId"

#define AE_DRM_SOAP_STATUS_SUCCESS                     0
#define AE_DRM_SOAP_STATUS_NOT_REGISTERED              1
#define AE_DRM_SOAP_STATUS_ENTER_MAINTENANCE_MODE      2
#define AE_DRM_SOAP_STATUS_EXIT_MAINTENANCE_MODE       1000
#define AE_DRM_SOAP_STATUS_STAY_IN_MAINTENANCE_MODE    1001

#define AE_DRM_SOAP_COMMAND_REASON_FAILED              "failed"
#define AE_DRM_SOAP_COMMAND_REASON_INVALID_PARAMS      "invalid parameters"
#define AE_DRM_SOAP_COMMAND_REASON_NOT_IMPLEMENTED     "not implemented"

#define AE_DRM_DEFAULT_MAINTENANCE_PING_RATE    120
#define AE_DRM_DEFAULT_OFFLINE_FAILURE_COUNT    3
#define AE_DRM_DEFAULT_RETRY_PERIOD             30
#define AE_DRM_MAX_MESSAGE_SIZE                 51200
#define AE_DRM_INTERNAL_CYCLE_LENGTH            { 1, 0 }

#define AE_DRM_REQUEST(p) ((AeDRMRequest *) (p)->pUserData)
#define AE_DRM_DEVICE(i) ((AeDRMDevice *) AePtrArrayGet(g_pDRMDevices, (i)))
#define AE_DRM_MASTER_DEVICE(i) ((AeDRMDevice *) AePtrArrayGet(g_pDRMDevices, AE_DRM_MASTER_DEVICE_ID))
#define AE_DRM_CONFIG(i) ((AeDRMServerConfig *) AePtrArrayGet(g_pDRMServerConfigs, (i)))
#define AE_DRM_PRIMARY_CONFIG ((AeDRMServerConfig *) AePtrArrayGet(g_pDRMServerConfigs, AE_DRM_PRIMARY_CONFIG_ID))
#define AE_DRM_BACKUP_CONFIG ((AeDRMServerConfig *) AePtrArrayGet(g_pDRMServerConfigs, AE_DRM_BACKUP_CONFIG_ID))
#define AE_DRM_DEVICE_SERVER(p, i) ((AeDRMDeviceServer *) AePtrArrayGet((p)->pDeviceServers, (i)))
#define AE_DRM_DEVICE_SERVER_LOCATION(p) ((p)->pLocation ? (p)->pLocation : (p)->pParent->pLocation)
#define AE_DRM_DEVICE_SERVER_OWNER(p) ((p)->pOwner ? (p)->pOwner : (p)->pParent->pOwner)

typedef struct _AeDRMSettings AeDRMSettings;
typedef struct _AeDRMCallbacks AeDRMCallbacks;
typedef struct _AeDRMDevice AeDRMDevice;
typedef struct _AeDRMServerConfig AeDRMServerConfig;
typedef struct _AeDRMServerLocation AeDRMServerLocation;
typedef struct _AeDRMDeviceServer AeDRMDeviceServer;
typedef struct _AeDRMRequest AeDRMRequest;
typedef struct _AeDRMSOAPStatus AeDRMSOAPStatus;

struct _AeDRMSettings
{
    AeInt32     iQueueSize;
    AeTimeValue retryPeriod;
    AeBool      bServerTimeStamp;
    AeLogLevel  iLogLevel;
    AeBool      bYieldOnIdle;
};

struct _AeDRMCallbacks
{
    void    (*pOnWebError)(AeInt32 iServerId, AeError iError);
    void    (*pOnDeviceRegistered)(AeInt32 iDeviceId);
    void    (*pOnQueueStatus)(AeDRMQueueStatus iNewStatus);
    void    (*pOnSOAPMethod)(AeInt32 iDeviceId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus);
    void    (*pOnSOAPMethodEx)(AeInt32 iDeviceId, AeInt32 iServerId, AeHandle pMethod, AeDRMSOAPCommandId *pSOAPId, AeDRMSOAPCommandStatus *pStatus);
    void    (*pOnCommandSetTag)(AeInt32 iDeviceId, AeDRMDataItem *pDataItem, AeDRMSOAPCommandStatus *pStatus);
    void    (*pOnCommandSetTime)(AeInt32 iDeviceId, AeTimeValue *pTime, AeInt32 *piTZOffset, AeDRMSOAPCommandStatus *pStatus);
    void    (*pOnCommandRestart)(AeInt32 iDeviceId, AeBool bHard, AeDRMSOAPCommandStatus *pStatus);
    void    (*pOnPingRateUpdate)(AeInt32 iServerId, AeDRMPingRate *pPingRate);
    AeBool  (*pOnFileDownloadBegin)(AeInt32 iDeviceId, AePointer *ppUserData);
    AeBool  (*pOnFileDownloadData)(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, AeInt32 iSize, AePointer pUserData);
    void    (*pOnFileDownloadEnd)(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData);
    AeBool  (*pOnFileUploadBegin)(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, AePointer *ppUserData);
    AeBool  (*pOnFileUploadBeginEx)(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, AeFileUploadExecuteParam *pParam, AePointer *ppUserData);
    AeBool  (*pOnFileUploadData)(AeInt32 iDeviceId, AeFileStat **ppFile, AeChar **ppData, AeInt32 *piSize, AePointer pUserData);
    void    (*pOnFileUploadEnd)(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData);
    void    (*pOnFileTransferEvent)(AeInt32 iDeviceId, AeFileTransferEvent iEvent, AePointer pUserData);
	AeBool  (*pOnRemoteSessionStart)(AeRemoteInterface *pInterface);
	void    (*pOnRemoteSessionEnd)(AeRemoteInterface *pInterface);
};

struct _AeDRMDevice
{
    AeDRMDeviceType iType;
    AeChar          *pModelNumber;
    AeChar          *pSerialNumber;
    AeBool          bEnabled;       /* used by AeDRMSetDeviceStatus() */
    AeBool          bOnline;        /* used by AeDRMSetDeviceOnline() */
};

struct _AeDRMServerConfig
{
    AeInt32                 iId;
    AeDRMServerConfigType   iType;
    AeDRMServerLocation     *pLocation;
    AeChar                  *pOwner;
    AeTimeValue             basePingRate;
    AeTimeValue             effectivePingRate;
    AeTimeValue             pingRateReversionTime;
    AeWebVersion            iWebVersion;
    AeBool                  bWebPersistent;
    AeTimeValue             webTimeOut;
    AePtrArray              *pDeviceServers;
};

struct _AeDRMDeviceServer
{
    AeDRMServerConfig   *pParent;
    AeDRMServerLocation *pLocation;
    AeChar              *pOwner;
    AeBool              bRegistered;
    AeBool              bMaintenanceMode;
    AeTimeValue         pingTime;
    AeTimeValue         maintenancePingTime;
    AeTimeValue         maintenancePingRate;
    AeInt               iRefCount;
    AeTimeValue         nextRetryTime;
    AeBool              bSendMDMessage;
    AeBool              bSendDSMessage;
};

struct _AeDRMServerLocation
{
    AeChar              *pURL;
    AeBool              bOnline;
    AeDRMRequest        *pPendingRequest;
    AeInt               iFailureCount;
    AeTimeValue         nextRetryTime;
};

struct _AeDRMRequest
{
    AeWebRequest        *pWebRequest;
    AeInt32             iConfigId;
    AeDRMServerLocation *pLocation;
    AeDRMEMessage       *pEMessage;
    AeArray             *pRawMessage;
    AeArray             *pRawResponse;
    AeList              *pSOAPStatuses;
    AeBool              bOpen;
    AeBool              bActive;
};

struct _AeDRMSOAPStatus
{
    AeInt32 iStatus;
    AeBool  bGlobal;
    AeInt32 iDeviceId;
    AeChar  *pOwner;
    AeInt32 iInterval;
    AeChar  *pURL;
};

extern AeWebUserAgent   *g_pWebUA;


#ifdef __cplusplus
extern "C" {
#endif

AeError             AeDRMInitialize(void);
void                AeDRMDestroy(void);

AeDRMDevice         *AeDRMDeviceNew(AeDRMDeviceType iType, AeChar *pModelNumber, AeChar *pSerialNumber);
void                AeDRMDeviceDestroy(AeDRMDevice *pDevice);
AeDRMServerConfig   *AeDRMServerConfigNew(AeDRMServerConfigType iType, AeChar *pOwner, AeTimeValue *pPingRate);
void                AeDRMServerConfigDestroy(AeDRMServerConfig *pConfig);
AeError             AeDRMServerConfigInitialize(AeDRMServerConfig *pConfig, AeChar *pURL);
AeDRMServerLocation *AeDRMServerLocationNew(AeChar *pURL);
void                AeDRMServerLocationDestroy(AeDRMServerLocation *pLocation);
AeDRMDeviceServer   *AeDRMDeviceServerNew(AeDRMServerConfig *pConfig);
void                AeDRMDeviceServerDestroy(AeDRMDeviceServer *pServer);
#define             AeDRMDeviceServerAddRef(p) ((p)->iRefCount += 1)
#define             AeDRMDeviceServerRelease(p) ((p)->iRefCount > 0 ? --(p)->iRefCount : 0)
AeDRMRequest        *AeDRMRequestNew(AeDRMEMessageType iType, AeDRMDeviceServer *pServer, AeTimeValue *pTimeStamp);
void                AeDRMRequestDestroy(AeDRMRequest *pRequest);
void                AeDRMRequestComplete(AeDRMRequest *pRequest, AeBool bSuccess);
AeDRMSOAPStatus     *AeDRMSOAPStatusNew(AeInt32 iStatus, AeBool bGlobal, AeInt32 iDeviceId);
void                AeDRMSOAPStatusDestroy(AeDRMSOAPStatus *pStatus);
AeArray             *AeDRMGetFormattedContent(AeXMLContent *pContent);
AeError             _AeDRMPostSOAPCommandStatus(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeDRMSOAPCommandId *pSOAPId, AeDRMSOAPCommandStatus *pStatus);
AeError             _AeDRMPostPingRate(AeInt32 iDeviceId, AeInt32 iServerId, AeDRMQueuePriority iPriority, AeTimeValue *pPingRate);

#ifdef __cplusplus
}
#endif

#endif

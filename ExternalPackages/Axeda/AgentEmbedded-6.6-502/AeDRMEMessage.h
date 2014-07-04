/* $Id: AeDRMEMessage.h,v 1.10.2.2 2009/08/19 21:27:59 dkhodos Exp $ */

#ifndef _AE_DRM_EMESSAGE_H_
#define _AE_DRM_EMESSAGE_H_

#define EMESSAGE_MAJOR_VERSION  AE_VERSION_MAJOR
#define EMESSAGE_MINOR_VERSION  AE_VERSION_MINOR

#define EMESSAGE_TAG    "eMessage"
#define MTMESSAGE_TAG   "MtMessage"

#define EMESSAGE_ELEMENT_DEVICE             "De"
#define EMESSAGE_ELEMENT_REGISTRATION       "Re"
#define EMESSAGE_ELEMENT_GATEWAY            "Gw"
#define EMESSAGE_ELEMENT_PING               "Pi"
#define EMESSAGE_ELEMENT_ONLINE             "Ds"
#define EMESSAGE_ELEMENT_DATA               "Da"
#define EMESSAGE_ELEMENT_DATA_ITEM          "Di"
#define EMESSAGE_ELEMENT_ALARM              "Al"
#define EMESSAGE_ELEMENT_EVENT              "Ev"
#define EMESSAGE_ELEMENT_EMAIL              "Em"
#define EMESSAGE_ELEMENT_PING_RATE          "Pr"
#define EMESSAGE_ELEMENT_SOAP_STATUS        "SOAPStatus"
#define EMESSAGE_ELEMENT_MANAGED_DEVICES    "Md"
#define EMESSAGE_ELEMENT_CONFIG_STRING      "Cs"
#if defined(ENABLE_FILE_TRANSFER)
#define EMESSAGE_ELEMENT_UPLOAD_REQUEST     "Ur"
#define EMESSAGE_ELEMENT_FILE               "Fi"
#define EMESSAGE_ELEMENT_PACKAGE_STATUS     "Ps"
#endif
#if defined(ENABLE_REMOTE_SESSION)
#define EMESSAGE_ELEMENT_DEVICE_APP			"Dapp"
#define EMESSAGE_ELEMENT_DEVICE_APP_ITEM	"Ap"
#define EMESSAGE_ELEMENT_DEVICE_APP_VERSION	"Dapv"
#endif

#define EMESSAGE_ATTR_MODEL_NUMBER  "mn"
#define EMESSAGE_ATTR_SERIAL_NUMBER "sn"
#define EMESSAGE_ATTR_OWNER         "ow"
#define EMESSAGE_ATTR_RATE          "r"
#define EMESSAGE_ATTR_TIMESTAMP     "t"
#define EMESSAGE_ATTR_ONLINE        "ol"
#define EMESSAGE_ATTR_TYPE          "y"
#define EMESSAGE_ATTR_NAME          "n"
#define EMESSAGE_ATTR_SEVERITY      "sv"
#define EMESSAGE_ATTR_DESCRIPTION   "de"
#define EMESSAGE_ATTR_ACTIVE        "at"
#define EMESSAGE_ATTR_ACKNOWLEDGED  "ak"
#define EMESSAGE_ATTR_QUALITY       "q"
#define EMESSAGE_ATTR_MESSAGE       "me"
#define EMESSAGE_ATTR_TO            "to"
#define EMESSAGE_ATTR_FROM          "fr"
#define EMESSAGE_ATTR_SUBJECT       "su"
#define EMESSAGE_ATTR_SERVER        "sr"
#define EMESSAGE_ATTR_CONTENT_TYPE  "cn"
#define EMESSAGE_ATTR_COMMAND_ID    "cmdId"
#define EMESSAGE_ATTR_USER_ID       "userId"
#define EMESSAGE_ATTR_STATUS_CODE   "sc"
#define EMESSAGE_ATTR_STATUS_REASON "xsc"
#if defined(ENABLE_FILE_TRANSFER)
#define EMESSAGE_ATTR_JOB_ID        "jobId"
#define EMESSAGE_ATTR_ERROR_CODE    "ec"
#define EMESSAGE_ATTR_INSTRUCTION   "ii"
#define EMESSAGE_ATTR_DELETE        "d"
#define EMESSAGE_ATTR_CLIENT_ID     "cid"
#define EMESSAGE_ATTR_HINT          "h"
#define EMESSAGE_ATTR_PRIORITY      "p"
#define EMESSAGE_ATTR_COMPRESSION   "ct"
#endif
#if defined(ENABLE_REMOTE_SESSION)
#define EMESSAGE_ATTR_LAUNCH		"lo"
#define EMESSAGE_ATTR_CONNECT		"co"
#endif

typedef enum _AeDRMEMessageType AeDRMEMessageType;
typedef enum _AeDRMContentType AeDRMContentType;
typedef struct _AeDRMDeviceElement AeDRMDeviceElement;
typedef struct _AeDRMEMessage AeDRMEMessage;
#if defined(ENABLE_REMOTE_SESSION)
typedef struct _AeRemoteDesktopProbeResponse AeRemoteDesktopProbeResponse;
#endif


enum _AeDRMEMessageType
{
    EMessageStandard,
    EMessageMaintenancePing
};

enum _AeDRMContentType
{
    AeDRMRegistration,
    AeDRMPing,
    AeDRMMaintenancePing,
    AeDRMData,
    AeDRMManagedDevice,
    AeDRMOnline
};

struct _AeDRMDeviceElement
{
    AeInt32             iDeviceId;
    AeDRMContentType    iType;
    AeChar              *pOwner;
    AeXMLElement        *pElement;
};

struct _AeDRMEMessage
{
    AeXMLDocument   *pDocument;
    AeInt           iMajorVer;
    AeInt           iMinorVer;
    AeInt32         iMaxSize;
    AeInt32         iId;
    AeChar          *pTimeStamp;
    AeList          *pDeviceElements;
    AeInt32         iSize;
};

#if defined(ENABLE_REMOTE_SESSION)
struct _AeRemoteDesktopProbeResponse
{
    AeUInt16 iMajor;
    AeUInt16 iMinor; 
    AeUInt16 iBuild;
    AeUInt16 iRFBCompatFlag;
    AeUInt16 iPlatform;
    AeUInt16 iAppNameLength;
    AeChar pAppName[256];
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

AeDRMDeviceElement  *AeDRMDeviceElementNew(AeInt32 iDeviceId, AeDRMContentType iType, AeChar *pOwner, AeXMLElement *pElement);
void                AeDRMDeviceElementDestroy(AeDRMDeviceElement *pDeviceElement);
AeDRMEMessage       *AeDRMEMessageNew(AeDRMEMessageType iType, AeInt iMajorVer, AeInt iMinorVer, AeInt32 iMaxSize, AeTimeValue *pTimeStamp);
void                AeDRMEMessageDestroy(AeDRMEMessage *pMessage);
AeBool              AeDRMEMessageAddRegistration(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner, AeChar *pMasterOwner, AeTimeValue *pPingRate);
AeBool              AeDRMEMessageAddPing(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner);
AeBool              AeDRMEMessageAddOnline(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner);
AeBool              AeDRMEMessageAddMaintenancePing(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner);
AeBool              AeDRMEMessageAddData(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner, AeChar *pData, AeInt32 iSize);
AeBool              AeDRMEMessageAddManagedDevices(AeDRMEMessage *pMessage, AeChar *pOwner, AeList *pDeviceDescriptors);
AeDRMDeviceElement  *AeDRMEMessageLookupDeviceElement(AeDRMEMessage *pMessage, AeInt32 iDeviceId);
AeXMLElement        *AeDRMEMessageCreateAlarm(AeDRMAlarm *pAlarm);
AeXMLElement        *AeDRMEMessageCreateEvent(AeDRMEvent *pEvent);
AeXMLElement        *AeDRMEMessageCreateDataContainer1(void);
AeXMLElement        *AeDRMEMessageCreateDataContainer2(AeTimeValue *pTimeStamp);
AeXMLElement        *AeDRMEMessageCreateEmail(AeDRMEmail *pEmail);
AeXMLElement        *AeDRMEMessageCreatePingRate(AeTimeValue *pPingRate);
AeXMLElement        *AeDRMEMessageCreateManagedDeviceDescriptor(AeInt32 iDeviceId, AeChar *pOwner);

AeXMLElement        *AeDRMEMessageCreateSOAPCommandStatus(AeChar *pCmdId, AeChar *pUserId, AeDRMSOAPCommandStatus *pStatus);
#if defined(ENABLE_FILE_TRANSFER)
AeXMLElement        *AeDRMEMessageCreatePackageStatus(AeChar *pJobId, AeInt32 iStatusCode, AeInt32 iError, AeInt32 iInstr);
AeXMLElement        *AeDRMEMessageCreateUploadContainer(AeFileUploadRequestParam *pParam);
void                AeDRMEMessageAddUploadItem(AeXMLElement *pContainer, AeChar *pName, AeBool bDelete);
#endif
#if defined(ENABLE_REMOTE_SESSION)
AeXMLElement        *AeDRMEMessageCreateRemoteContainer(void);
void                AeDRMEMessageAddRemoteItem(AeXMLElement *pContainer, AeChar *pName, AeChar *pDescription, AeChar *pRemoteType, AeUInt16 iPort);
AeXMLElement        *AeDRMEMessageCreateRemoteDesktopInfo(AeChar *pName, AeRemoteDesktopProbeResponse *pResponse);
#endif

void                AeDRMEMessageAddDataItem(AeXMLElement *pContainer, AeDRMDataItem *pDataItem, AeBool bTimeStamp);

#ifdef __cplusplus
}
#endif

#endif

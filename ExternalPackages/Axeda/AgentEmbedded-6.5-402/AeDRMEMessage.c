/* $Id: AeDRMEMessage.c,v 1.14.2.2 2009/08/19 21:27:59 dkhodos Exp $ */

#include "Ae.h"

static AeInt32  g_iMessageId = 0;

static AeBool       AeDRMEMessageAddCheckContent(AeDRMEMessage *pMessage, AeXMLElement *pContainer, AeXMLContent *pContent);
static AeDRMDeviceElement *AeDRMEMessageGetDeviceElement(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeDRMContentType iType, AeChar *pOwner, AeBool bEmpty);
static AeXMLElement *AeDRMEMessageCreateDeviceElement(AeInt32 iDeviceId, AeChar *pOwner, AeBool bEmpty);
static AeXMLElement *AeDRMEMessageCreateRegistrationElement(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pMasterOwner, AeTimeValue *pPingRate);

static AeInt        AeDRMDeviceElementCompare(AePointer pData1, AePointer pData2);
static void         AeDRMDeviceElementRelease(AePointer pObject);

static void         AeDRMEMessageGetTimeStamp(AeTimeValue *pTime, AeChar *pOutput, AeInt iMaxLen, AeBool bForce);

/******************************************************************************/
AeDRMDeviceElement *AeDRMDeviceElementNew(AeInt32 iDeviceId, AeDRMContentType iType, AeChar *pOwner, AeXMLElement *pElement)
{
    AeDRMDeviceElement *pDeviceElement;
    
    pDeviceElement = (AeDRMDeviceElement *) AeAlloc(sizeof(AeDRMDeviceElement));
    if (!pDeviceElement)
        return NULL;
    memset(pDeviceElement, 0, sizeof(AeDRMDeviceElement));

    pDeviceElement->iDeviceId = iDeviceId;
    pDeviceElement->iType = iType;
    AeSetString(&pDeviceElement->pOwner, pOwner, -1);
    pDeviceElement->pElement = pElement;

    return pDeviceElement;
}

/******************************************************************************/
void AeDRMDeviceElementDestroy(AeDRMDeviceElement *pDeviceElement)
{
    if (pDeviceElement->pOwner)
        AeFree(pDeviceElement->pOwner);

    AeFree(pDeviceElement);
}

/******************************************************************************/
static AeInt AeDRMDeviceElementCompare(AePointer pData1, AePointer pData2)
{
    return !(((AeDRMDeviceElement *) pData1)->iDeviceId ==
        ((AeDRMDeviceElement *) pData2)->iDeviceId);
}

/******************************************************************************/
static void AeDRMDeviceElementRelease(AePointer pObject)
{
    AeDRMDeviceElementDestroy((AeDRMDeviceElement *) pObject);
}

/******************************************************************************/
AeDRMEMessage *AeDRMEMessageNew(AeDRMEMessageType iType, AeInt iMajorVer, AeInt iMinorVer, AeInt32 iMaxSize, AeTimeValue *pTimeStamp)
{
    AeDRMEMessage *pMessage;
    AeChar pTmp[64];

    pMessage = (AeDRMEMessage *) AeAlloc(sizeof(AeDRMEMessage));
    if (!pMessage)
        return NULL;
    memset(pMessage, 0, sizeof(AeDRMEMessage));

    pMessage->pDocument = AeXMLDocumentNew();
    if (!pMessage->pDocument)
        return NULL;

    pMessage->pDocument->pRoot = AeXMLElementNew();
    if (!pMessage->pDocument->pRoot)
        return NULL;

    pMessage->pDeviceElements = AeListNew((AeCompareFunc) AeDRMDeviceElementCompare,
        (AeReleaseFunc) AeDRMDeviceElementRelease);
    if (!pMessage->pDeviceElements)
        return NULL;

    pMessage->iId = g_iMessageId;
    g_iMessageId += 1;

    pMessage->iMajorVer = iMajorVer;
    pMessage->iMinorVer = iMinorVer;
    pMessage->iMaxSize = iMaxSize;
    pMessage->iSize = 0;

    AeDRMEMessageGetTimeStamp(pTimeStamp, pTmp, sizeof(pTmp), AeFalse);
    AeSetString(&pMessage->pTimeStamp, pTmp, -1);

    if (iType == EMessageStandard)
        AeXMLElementSetName(pMessage->pDocument->pRoot, EMESSAGE_TAG);
    else
        AeXMLElementSetName(pMessage->pDocument->pRoot, MTMESSAGE_TAG);
    AeXMLElementSetEmpty(pMessage->pDocument->pRoot, AeFalse);

    snprintf(pTmp, sizeof(pTmp), "%ld", pMessage->iId);
    AeXMLElementAddAttribute(pMessage->pDocument->pRoot, "id", pTmp);

    if (iType == EMessageStandard)
    {
        AeXMLElementAddAttribute(pMessage->pDocument->pRoot, "rc", "0");

        snprintf(pTmp, sizeof(pTmp), "%d.%d", iMajorVer, iMinorVer);
        AeXMLElementAddAttribute(pMessage->pDocument->pRoot, "v", pTmp);
    }

    pMessage->iSize = AeXMLDocumentGetFormattedSize(pMessage->pDocument);

    return pMessage;
}

/******************************************************************************/
void AeDRMEMessageDestroy(AeDRMEMessage *pMessage)
{
    if (pMessage->pDocument)
        AeXMLDocumentDestroy(pMessage->pDocument);
    if (pMessage->pTimeStamp)
        AeFree(pMessage->pTimeStamp);
    if (pMessage->pDeviceElements)
        AeListDestroy(pMessage->pDeviceElements);

    AeFree(pMessage);
}

/******************************************************************************/
/*
 <Re r="15000" t="2001-05-14T14:12:50-04:00" y="#">
      Where # is 
          - 0 for Gateway Device
          - 1 for a gateway managed device
          - 2 for a auto-discovered device
          - 3 for Connector Device
   <Gw>
      <De mn="gModel" sn="gSerial" ow="gOwner"/>
   </Gw>    
 </Re>
*/
AeBool AeDRMEMessageAddRegistration(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner, AeChar *pMasterOwner, AeTimeValue *pPingRate)
{
    AeDRMDeviceElement *pDeviceElement;
    AeXMLElement *pRegElement;
    
    /* fetch device element */
    pDeviceElement = AeDRMEMessageGetDeviceElement(pMessage, iDeviceId, AeDRMRegistration,
        pOwner, AeFalse);
    
    /* there must be only one registration element per device */
    if (!pDeviceElement || pDeviceElement->pElement->pFirstChild)
        return AeFalse;

    /* create registration element */
    pRegElement = AeDRMEMessageCreateRegistrationElement(pMessage, iDeviceId,
        pMasterOwner, pPingRate);
    if (!pRegElement)
        return AeFalse;

    /* add content to device element */
    return AeDRMEMessageAddCheckContent(pMessage, pDeviceElement->pElement,
        (AeXMLContent *) pRegElement);
}

/******************************************************************************/
/*
 <Ds ol="1"/>
*/
AeBool AeDRMEMessageAddOnline(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner)
{
    AeDRMDeviceElement *pDeviceElement;
    AeXMLElement *pOnlineElement;
    AeDRMDevice *pDevice = AePtrArrayGet(g_pDRMDevices, iDeviceId);

    /* fetch device element */
    pDeviceElement = AeDRMEMessageGetDeviceElement(pMessage, iDeviceId, AeDRMOnline,
        pOwner, AeFalse);
    
    /* must be valid */
    if (!pDeviceElement)
        return AeFalse;

    /* create online element */
    pOnlineElement = AeXMLElementNew();
    if (!pOnlineElement)
        return AeFalse;

    AeXMLElementSetName(pOnlineElement, EMESSAGE_ELEMENT_ONLINE);
    AeXMLElementSetEmpty(pOnlineElement, AeTrue);

    /* online attribute */
    if(pDevice && pDevice->bOnline == AeTrue)
        AeXMLElementAddAttribute(pOnlineElement, EMESSAGE_ATTR_ONLINE, "1");
    else
        AeXMLElementAddAttribute(pOnlineElement, EMESSAGE_ATTR_ONLINE, "0");

    return AeDRMEMessageAddCheckContent(pMessage, pDeviceElement->pElement,
        (AeXMLContent *) pOnlineElement);
}

/******************************************************************************/
/*
 <Pi t="2001-05-14T14:12:50-04:00"/>
*/
AeBool AeDRMEMessageAddPing(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner)
{
    AeDRMDeviceElement *pDeviceElement;
    AeXMLElement *pPingElement;

    /* fetch device element */
    pDeviceElement = AeDRMEMessageGetDeviceElement(pMessage, iDeviceId, AeDRMPing,
        pOwner, AeFalse);
    
    /* there must be only one ping element per device */
    if (!pDeviceElement || pDeviceElement->pElement->pFirstChild)
        return AeFalse;

    /* create ping element */
    pPingElement = AeXMLElementNew();
    if (!pPingElement)
        return AeFalse;
    AeXMLElementSetName(pPingElement, EMESSAGE_ELEMENT_PING);
    AeXMLElementSetEmpty(pPingElement, AeTrue);

    /* timestamp attribute */
    AeXMLElementAddAttribute(pPingElement, EMESSAGE_ATTR_TIMESTAMP,
        pMessage->pTimeStamp);

    return AeDRMEMessageAddCheckContent(pMessage, pDeviceElement->pElement,
        (AeXMLContent *) pPingElement);
}

/******************************************************************************/
/*
 <De mn="Model_1" sn="Serial_1" ow="axeda" />
*/
AeBool AeDRMEMessageAddMaintenancePing(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner)
{
    AeDRMDeviceElement *pDeviceElement;

    /* lookup/create device element */
    pDeviceElement = AeDRMEMessageGetDeviceElement(pMessage, iDeviceId, AeDRMMaintenancePing,
        pOwner, AeTrue);
    if (!pDeviceElement)
        return AeFalse;

    return AeTrue;
}

/******************************************************************************/
/* Md message format: (for some reason, the empty <Cs> element must be present) */
/* 
  <De did="0" mn="GWModel" sn="GWSerial" ow="drm-data_source">
    <Md>
      <De mn="ManagedModelOne" sn="SerialOne" ow="drm-data_source">
	    <Cs/>
	  </De>
      <De mn="ManagedModelTwo" sn="SerialTwo" ow="drm-data_source">
	    <Cs/>
	  </De>
    </Md>
  </De>
*/

AeBool AeDRMEMessageAddManagedDevices(AeDRMEMessage *pMessage, AeChar *pOwner, AeList *pDeviceDescriptors)
{
    AeDRMDeviceElement *pDeviceElement;
    AeXMLElement *pMDElement;
    AeListItem  *pItem;

    /* Fetch gateway device element */
    pDeviceElement = AeDRMEMessageGetDeviceElement(pMessage, AE_DRM_MASTER_DEVICE_ID, AeDRMManagedDevice, pOwner, AeFalse);
    if (!pDeviceElement)
        return AeFalse;

    /* Create Md Message element */
    pMDElement = AeXMLElementNew();
    if (!pMDElement)
        return AeFalse;
    AeXMLElementSetName(pMDElement, EMESSAGE_ELEMENT_MANAGED_DEVICES);
    AeXMLElementSetEmpty(pMDElement, AeFalse);

    /* add device descriptor elements */
    pItem = AeListFirst(pDeviceDescriptors);
    while(pItem)
    {
        AeXMLElement *pDeviceDescriptor;

        pDeviceDescriptor = AeListData(pItem, AeXMLElement);
        if (pDeviceDescriptor)
			AeXMLElementAddContent(pMDElement, (AeXMLContent *) pDeviceDescriptor);

        pItem = AeListNext(pItem);
    }

    return AeDRMEMessageAddCheckContent(pMessage, pDeviceElement->pElement, (AeXMLContent *) pMDElement);
}
/******************************************************************************/
AeBool AeDRMEMessageAddData(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pOwner, AeChar *pData, AeInt32 iSize)
{
    AeDRMDeviceElement *pDeviceElement;
    AeXMLCharData *pContent;

    /* fetch device element */
    pDeviceElement = AeDRMEMessageGetDeviceElement(pMessage, iDeviceId, AeDRMData,
        pOwner, AeFalse);
    if (!pDeviceElement)
        return AeFalse;

    /* do not mix data with other content types */
    if (!pDeviceElement || pDeviceElement->iType != AeDRMData)
        return AeFalse;

    /* create content from data */
    pContent = AeXMLCharDataNew();
    if (!pContent)
        return AeFalse;
    AeXMLCharDataSetData(pContent, pData, iSize);

    /* add content to device element */
    /* note that message size is not checked for the first data element in the given */
    /* device element. otherwise large data elements may never get sent. */
    if (!pDeviceElement->pElement->pFirstChild)
    {
        AeXMLElementAddContent(pDeviceElement->pElement, (AeXMLContent *) pContent);
        pMessage->iSize += AeXMLContentGetFormattedSize((AeXMLContent *) pContent);
        return AeTrue;

    }
    else
        return AeDRMEMessageAddCheckContent(pMessage, pDeviceElement->pElement,
            (AeXMLContent *) pContent);
}

/******************************************************************************/
AeDRMDeviceElement *AeDRMEMessageLookupDeviceElement(AeDRMEMessage *pMessage, AeInt32 iDeviceId)
{
    AeDRMDeviceElement keyDeviceElement;
    AeListItem *pItem;

    keyDeviceElement.iDeviceId = iDeviceId;
    pItem = AeListFind(pMessage->pDeviceElements, &keyDeviceElement);
    if (pItem)
        return AeListData(pItem, AeDRMDeviceElement);

    return NULL;
}

/******************************************************************************/
AeXMLElement *AeDRMEMessageCreateAlarm(AeDRMAlarm *pAlarm)
{
    AeXMLElement *pAlarmElement;
    AeChar pTmp[64];

    /* create alarm element */
    pAlarmElement = AeXMLElementNew();
    if (!pAlarmElement)
        return NULL;
    AeXMLElementSetName(pAlarmElement, EMESSAGE_ELEMENT_ALARM);
    AeXMLElementSetEmpty(pAlarmElement, AeFalse);

    /* timestamp attribute */
    AeDRMEMessageGetTimeStamp(&pAlarm->timeStamp, pTmp, sizeof(pTmp), AeFalse);
    AeXMLElementAddAttribute(pAlarmElement, EMESSAGE_ATTR_TIMESTAMP, pTmp);

    /* name attribute */
    AeXMLElementAddAttribute(pAlarmElement, EMESSAGE_ATTR_NAME, pAlarm->pName);

    /* severity attribute */
    snprintf(pTmp, sizeof(pTmp), "%d", pAlarm->iSeverity);
    AeXMLElementAddAttribute(pAlarmElement, EMESSAGE_ATTR_SEVERITY, pTmp);

    /* description attribute */
    if (pAlarm->pDescription)
        AeXMLElementAddAttribute(pAlarmElement, EMESSAGE_ATTR_DESCRIPTION,
            pAlarm->pDescription);

    /* type attribute */
    if(pAlarm->pCondition)
        AeXMLElementAddAttribute(pAlarmElement, EMESSAGE_ATTR_TYPE, pAlarm->pCondition);

    /* active attribute */
    AeXMLElementAddAttribute(pAlarmElement, EMESSAGE_ATTR_ACTIVE,
        pAlarm->bActive ? "y" : "n");

    /* acknowledged attribute */
    AeXMLElementAddAttribute(pAlarmElement, EMESSAGE_ATTR_ACKNOWLEDGED,
        pAlarm->bAck ? "y" : "n");

    /* add data item if any */
    if (pAlarm->pDataItem)
        AeDRMEMessageAddDataItem(pAlarmElement, pAlarm->pDataItem, AeFalse);

    return pAlarmElement;
}

/******************************************************************************/
AeXMLElement *AeDRMEMessageCreateEvent(AeDRMEvent *pEvent)
{
    AeXMLElement *pEventElement;
    AeChar pTmp1[64];

    /* create event element */
    pEventElement = AeXMLElementNew();
    if (!pEventElement)
        return NULL;
    AeXMLElementSetName(pEventElement, EMESSAGE_ELEMENT_EVENT);
    AeXMLElementSetEmpty(pEventElement, AeTrue);

    /* timestamp attribute */
    AeDRMEMessageGetTimeStamp(&pEvent->timeStamp, pTmp1, sizeof(pTmp1), AeFalse);
    AeXMLElementAddAttribute(pEventElement, EMESSAGE_ATTR_TIMESTAMP, pTmp1);

    /* name attribute */
    AeXMLElementAddAttribute(pEventElement, EMESSAGE_ATTR_NAME, pEvent->pName);

    /* severity attribute */
    snprintf(pTmp1, sizeof(pTmp1), "%d", pEvent->iSeverity);
    AeXMLElementAddAttribute(pEventElement, EMESSAGE_ATTR_SEVERITY, pTmp1);

    /* message attribute */
    AeXMLElementAddAttribute(pEventElement, EMESSAGE_ATTR_MESSAGE, pEvent->pMessage);

    return pEventElement;
}

/**************************************************************************/
AeXMLElement *AeDRMEMessageCreateDataContainer1(void)
{
    AeXMLElement *pDataElement;

    pDataElement = AeXMLElementNew();
    if (!pDataElement)
        return NULL;
    AeXMLElementSetName(pDataElement, EMESSAGE_ELEMENT_DATA);
    AeXMLElementSetEmpty(pDataElement, AeFalse);
    
    return pDataElement;
}

/**************************************************************************/
AeXMLElement *AeDRMEMessageCreateDataContainer2(AeTimeValue *pTimeStamp)
{
    AeXMLElement *pDataElement;
    AeChar pTmp[64];

    pDataElement = AeDRMEMessageCreateDataContainer1();
    if (!pDataElement)
        return NULL;

    /* timestamp attribute */
    AeDRMEMessageGetTimeStamp(pTimeStamp, pTmp, sizeof(pTmp), AeFalse);
    AeXMLElementAddAttribute(pDataElement, EMESSAGE_ATTR_TIMESTAMP, pTmp);

    return pDataElement;
}

/**************************************************************************/
AeXMLElement *AeDRMEMessageCreateEmail(AeDRMEmail *pEmail)
{
    AeXMLElement *pEmailElement;
    AeArray *pTmp;

    /* create email element */
    pEmailElement = AeXMLElementNew();
    if (!pEmailElement)
        return NULL;
    AeXMLElementSetName(pEmailElement, EMESSAGE_ELEMENT_EMAIL);
    AeXMLElementSetEmpty(pEmailElement, AeFalse);

    /* "To:" attribute */
    AeXMLElementAddAttribute(pEmailElement, EMESSAGE_ATTR_TO, pEmail->pRecipients);

    /* "From:" attribute */
    pTmp = AeArrayNew(1);
    if (!pTmp)
        return NULL;
    AeArrayAppend(pTmp, pEmail->pSenderName, strlen(pEmail->pSenderName));
    AeArrayAppend(pTmp, " <", 2);
    AeArrayAppend(pTmp, pEmail->pSenderAddress, strlen(pEmail->pSenderAddress));
    AeArrayAppend(pTmp, ">", 1);
    AeXMLElementAddAttribute(pEmailElement, EMESSAGE_ATTR_FROM, AeArrayGet(pTmp, 0));
    AeArrayDestroy(pTmp);

    /* subject attribute */
    AeXMLElementAddAttribute(pEmailElement, EMESSAGE_ATTR_SUBJECT, pEmail->pSubject);

    /* server attribute */
    AeXMLElementAddAttribute(pEmailElement, EMESSAGE_ATTR_SERVER, pEmail->pServerHost);

    /* Content-Type attribute */
    AeXMLElementAddAttribute(pEmailElement, EMESSAGE_ATTR_CONTENT_TYPE, pEmail->pContentType);

    /* add message body */
    pTmp = AeArrayNew(1);
    if (!pTmp)
        return NULL;
    AeXMLEntitize(pEmail->pBody, pTmp);
    AeXMLElementAddCharData(pEmailElement, AeArrayGet(pTmp, 0), AeArrayCount(pTmp));
    AeArrayDestroy(pTmp);

    return pEmailElement;
}

/**************************************************************************/
AeXMLElement *AeDRMEMessageCreatePingRate(AeTimeValue *pPingRate)
{
    AeXMLElement *pPRElement;
    AeChar pTmp[64];

    /* create email element */
    pPRElement = AeXMLElementNew();
    if (!pPRElement)
        return NULL;
    AeXMLElementSetName(pPRElement, EMESSAGE_ELEMENT_PING_RATE);
    AeXMLElementSetEmpty(pPRElement, AeTrue);

    /* rate attribute */
    snprintf(pTmp, sizeof(pTmp), "%ld", AE_TIME_VALUE_MILLISECONDS(*pPingRate));
    AeXMLElementAddAttribute(pPRElement, EMESSAGE_ATTR_RATE, pTmp);

    return pPRElement;
}

/**************************************************************************/
AeXMLElement *AeDRMEMessageCreateManagedDeviceDescriptor(AeInt32 iDeviceId, AeChar *pOwner)
{
    AeXMLElement *pDeviceElement;

	pDeviceElement = AeDRMEMessageCreateDeviceElement(iDeviceId, pOwner, AeFalse);
	if (pDeviceElement)
	{
        AeXMLElement *pCSElement;

		pCSElement = AeXMLElementNew();
		if (pCSElement)
        {
		    AeXMLElementSetName(pCSElement, EMESSAGE_ELEMENT_CONFIG_STRING);
		    AeXMLElementSetEmpty(pCSElement, AeTrue);

		    AeXMLElementAddContent(pDeviceElement, (AeXMLContent *) pCSElement);
        }
	}

    return pDeviceElement;
}

/**************************************************************************/
AeXMLElement *AeDRMEMessageCreateSOAPCommandStatus(AeChar *pCmdId, AeChar *pUserId, AeDRMSOAPCommandStatus *pStatus)
{
    AeXMLElement *pCSElement;
    AeTimeValue timeStamp;
    AeChar pTmp[64];

    /* create SOAP command status element */
    pCSElement = AeXMLElementNew();
    if (!pCSElement)
        return NULL;
    AeXMLElementSetName(pCSElement, EMESSAGE_ELEMENT_SOAP_STATUS);
    AeXMLElementSetEmpty(pCSElement, AeTrue);

    /* command id attribute */
    AeXMLElementAddAttribute(pCSElement, EMESSAGE_ATTR_COMMAND_ID, pCmdId);

    /* status code attribute */
    snprintf(pTmp, sizeof(pTmp), "%lx", pStatus->iStatusCode);
    AeXMLElementAddAttribute(pCSElement, EMESSAGE_ATTR_STATUS_CODE, pTmp);

    /* status reason attribute */
    if (pStatus->pStatusReason)
        AeXMLElementAddAttribute(pCSElement, EMESSAGE_ATTR_STATUS_REASON,
            pStatus->pStatusReason);

    /* timestamp attribute */
    AeGetCurrentTime(&timeStamp);
    AeDRMEMessageGetTimeStamp(&timeStamp, pTmp, sizeof(pTmp), AeTrue);
    AeXMLElementAddAttribute(pCSElement, EMESSAGE_ATTR_TIMESTAMP, pTmp);

    /* user id attribute */
    AeXMLElementAddAttribute(pCSElement, EMESSAGE_ATTR_USER_ID, pUserId);

    return pCSElement;
}

#if defined(ENABLE_FILE_TRANSFER)
/**************************************************************************/
AeXMLElement *AeDRMEMessageCreatePackageStatus(AeChar *pJobId, AeInt32 iStatusCode, AeInt32 iError, AeInt32 iInstr)
{
    AeXMLElement *pPSElement;
    AeChar pTmp[16];

    /* create SOAP command status element */
    pPSElement = AeXMLElementNew();
    if (!pPSElement)
        return NULL;
    AeXMLElementSetName(pPSElement, EMESSAGE_ELEMENT_PACKAGE_STATUS);
    AeXMLElementSetEmpty(pPSElement, AeTrue);

    /* job id attribute */
    AeXMLElementAddAttribute(pPSElement, EMESSAGE_ATTR_JOB_ID, pJobId);

    /* status code attribute */
    snprintf(pTmp, sizeof(pTmp), "%ld", iStatusCode);
    AeXMLElementAddAttribute(pPSElement, EMESSAGE_ATTR_STATUS_CODE, pTmp);

    /* status reason attribute */
    if (iError != AeNOERROR)
    {
        snprintf(pTmp, sizeof(pTmp), "%ld", iError);
        AeXMLElementAddAttribute(pPSElement, EMESSAGE_ATTR_ERROR_CODE, pTmp);

        snprintf(pTmp, sizeof(pTmp), "%ld", iInstr);
        AeXMLElementAddAttribute(pPSElement, EMESSAGE_ATTR_INSTRUCTION, pTmp);
    }

    return pPSElement;
}

/**************************************************************************/
AeXMLElement *AeDRMEMessageCreateUploadContainer(AeFileUploadRequestParam *pParam)
{
    AeXMLElement *pElement;
    AeChar pTmp[16];

    pElement = AeXMLElementNew();
    if (!pElement)
        return NULL;

    AeXMLElementSetName(pElement, EMESSAGE_ELEMENT_UPLOAD_REQUEST);
    AeXMLElementSetEmpty(pElement, AeFalse);

    if (pParam)
    {
        if (pParam->iMask & AE_FILE_UPLOAD_REQUEST_PARAM_ID)
        {
            AeXMLElementAddAttribute(pElement, EMESSAGE_ATTR_CLIENT_ID, pParam->pId);
        }
        if (pParam->iMask & AE_FILE_UPLOAD_REQUEST_PARAM_HINT)
        {
            AeXMLElementAddAttribute(pElement, EMESSAGE_ATTR_HINT, pParam->pHint);
        }
        if (pParam->iMask & AE_FILE_UPLOAD_REQUEST_PARAM_PRIORITY)
        {
            snprintf(pTmp, sizeof(pTmp), "%d", pParam->iPriority);
            AeXMLElementAddAttribute(pElement, EMESSAGE_ATTR_PRIORITY, pTmp);
        }
        if (pParam->iMask & AE_FILE_UPLOAD_REQUEST_PARAM_COMPRESSION)
        {
            AeXMLElementAddAttribute(pElement, EMESSAGE_ATTR_COMPRESSION,
                                     g_pCompressionStrings[pParam->iCompression]);
        }
    }
    
    return pElement;
}

/**************************************************************************/
void AeDRMEMessageAddUploadItem(AeXMLElement *pContainer, AeChar *pName, AeBool bDelete)
{
    AeXMLElement *pElement;

    pElement = AeXMLElementAddElement(pContainer, EMESSAGE_ELEMENT_FILE, AeTrue);
    if (!pElement)
        return;

    /* name */
    AeXMLElementAddAttribute(pElement, EMESSAGE_ATTR_NAME, pName);
 
    /* delete */
    AeXMLElementAddAttribute(pElement, EMESSAGE_ATTR_DELETE, bDelete ? "y" : "n");
}

#endif /* ENABLE_FILE_TRANSFER */

#if defined(ENABLE_REMOTE_SESSION)
/**************************************************************************/
AeXMLElement *AeDRMEMessageCreateRemoteContainer(void)
{
    AeXMLElement *pPRElement;

    pPRElement = AeXMLElementNew();
    if (!pPRElement)
        return NULL;

    AeXMLElementSetName(pPRElement, EMESSAGE_ELEMENT_DEVICE_APP);
    AeXMLElementSetEmpty(pPRElement, AeFalse);

    return pPRElement;
}

/**************************************************************************/
void AeDRMEMessageAddRemoteItem(AeXMLElement *pContainer, AeChar *pName, AeChar *pDescription, AeChar *pRemoteType, AeUInt16 iPort)
{
    AeXMLElement *pDIElement;
    AeChar Tmp[16];

    pDIElement = AeXMLElementAddElement(pContainer, EMESSAGE_ELEMENT_DEVICE_APP_ITEM, AeTrue);
    if (!pDIElement)
        return;

    /* name */
    AeXMLElementAddAttribute(pDIElement, EMESSAGE_ATTR_NAME, pName);
 
    /* Description */
    AeXMLElementAddAttribute(pDIElement, EMESSAGE_ATTR_DESCRIPTION, pDescription);

    /* type */
    AeXMLElementAddAttribute(pDIElement, EMESSAGE_ATTR_LAUNCH, pRemoteType);

    /* port */
    snprintf(Tmp, sizeof(Tmp), "C%d", iPort);
    AeXMLElementAddAttribute(pDIElement, EMESSAGE_ATTR_CONNECT, Tmp);
}

/**************************************************************************/
AeXMLElement *AeDRMEMessageCreateRemoteDesktopInfo(AeChar *pName, AeRemoteDesktopProbeResponse *pResponse)
{
    AeXMLElement *pElement;
    AeChar pTmp[64];

    pElement = AeXMLElementNew();
    if (!pElement)
        return NULL;

    AeXMLElementSetName(pElement, EMESSAGE_ELEMENT_DEVICE_APP_VERSION);
    AeXMLElementSetEmpty(pElement, AeTrue);

    /* application name */
    AeXMLElementAddAttribute(pElement, "an", pResponse->pAppName);

    /* application version */
    snprintf(pTmp, sizeof(pTmp), "%d.%d.%d",
             pResponse->iMajor, pResponse->iMinor, pResponse->iBuild);
    AeXMLElementAddAttribute(pElement, "v", pTmp);

    /* RFB compatibility flag */
    snprintf(pTmp, sizeof(pTmp), "%d", pResponse->iRFBCompatFlag);
    AeXMLElementAddAttribute(pElement, "rfc", pTmp);

    /* platform */
    snprintf(pTmp, sizeof(pTmp), "%d", pResponse->iPlatform);
    AeXMLElementAddAttribute(pElement, "os", pTmp);

    /* interface name */
    AeXMLElementAddAttribute(pElement, "n", pName);

    return pElement;
}

#endif /* ENABLE_REMOTE_SESSION */

/**************************************************************************/
void AeDRMEMessageAddDataItem(AeXMLElement *pContainer, AeDRMDataItem *pDataItem, AeBool bTimeStamp)
{
    AeXMLElement *pDIElement;
    AeChar pTmp1[256];
    AeArray *pTmp2;

    pDIElement = AeXMLElementAddElement(pContainer, EMESSAGE_ELEMENT_DATA_ITEM, AeFalse);
    if (!pDIElement)
        return;

    /* timestamp attribute */
    if (bTimeStamp)
    {
        AeDRMEMessageGetTimeStamp(&pDataItem->value.timeStamp, pTmp1, sizeof(pTmp1), AeFalse);
        AeXMLElementAddAttribute(pDIElement, EMESSAGE_ATTR_TIMESTAMP, pTmp1);
    }

    /* name attribute */
    AeXMLElementAddAttribute(pDIElement, EMESSAGE_ATTR_NAME, pDataItem->pName);

    /* type attribute */
    switch (pDataItem->value.iType)
    {
        case AeDRMDataAnalog:
            strcpy(pTmp1, "a");
            break;
        case AeDRMDataDigital:
            strcpy(pTmp1, "d");
            break;
        case AeDRMDataString:
            strcpy(pTmp1, "s");
            break;
    }
    AeXMLElementAddAttribute(pDIElement, EMESSAGE_ATTR_TYPE, pTmp1);

    /* quality attribute */
    switch (pDataItem->value.iQuality)
    {
        case AeDRMDataGood:
            strcpy(pTmp1, "g");
            break;
        case AeDRMDataBad:
            strcpy(pTmp1, "b");
            break;
        case AeDRMDataUncertain:
            strcpy(pTmp1, "u");
            break;
    }
    AeXMLElementAddAttribute(pDIElement, EMESSAGE_ATTR_QUALITY, pTmp1);

    /* value string */
    switch (pDataItem->value.iType)
    {
        case AeDRMDataAnalog:
#ifndef __UC_LIBC__
            snprintf(pTmp1, sizeof(pTmp1), "%f", pDataItem->value.data.dAnalog);
#else
            gcvt(pDataItem->value.data.dAnalog, 6, pTmp1);
#endif
            AeTrimRight(pTmp1, '0');
            AeTrimRight(pTmp1, '.');
            AeXMLElementAddCharData(pDIElement, pTmp1, strlen(pTmp1));
            break;
        case AeDRMDataDigital:
            snprintf(pTmp1, sizeof(pTmp1), "%d", pDataItem->value.data.bDigital);
            AeXMLElementAddCharData(pDIElement, pTmp1, strlen(pTmp1));
            break;
        case AeDRMDataString:
            pTmp2 = AeArrayNew(1);
            if (!pTmp2)
                return;
            AeXMLEntitize(pDataItem->value.data.pString, pTmp2);
            AeXMLElementAddCharData(pDIElement, AeArrayGet(pTmp2, 0), AeArrayCount(pTmp2));
            AeArrayDestroy(pTmp2);
            break;
    }
}

/**************************************************************************/
static AeBool AeDRMEMessageAddCheckContent(AeDRMEMessage *pMessage, AeXMLElement *pContainer, AeXMLContent *pContent)
{
    AeInt32 iAddedSize;

    iAddedSize = AeXMLContentGetFormattedSize(pContent);

    if (pMessage->iSize + iAddedSize > pMessage->iMaxSize)
    {
        AeXMLContentDestroy(pContent);
        return AeFalse;
    }

    AeXMLElementAddContent(pContainer, pContent);
    pMessage->iSize += iAddedSize;

    return AeTrue;
}

/**************************************************************************/
static AeDRMDeviceElement *AeDRMEMessageGetDeviceElement(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeDRMContentType iType, AeChar *pOwner, AeBool bEmpty)
{
    AeDRMDeviceElement *pDeviceElement, keyDeviceElement;
    AeXMLElement *pElement;
    AeListItem *pItem;
    AeBool bOK;

    /* search for existing device element */
    keyDeviceElement.iDeviceId = iDeviceId;
    pItem = AeListFind(pMessage->pDeviceElements, &keyDeviceElement);

    pDeviceElement = NULL;

    /* create new device element */
    if (!pItem)
    {
        pElement = AeDRMEMessageCreateDeviceElement(iDeviceId, pOwner, bEmpty);
        if (pElement)
        {
            /* put device element under root element */
            bOK = AeDRMEMessageAddCheckContent(pMessage, pMessage->pDocument->pRoot,
                (AeXMLContent *) pElement);
            if (bOK)
            {
                /* add device element to device-element map */
                pDeviceElement = AeDRMDeviceElementNew(iDeviceId, iType, pOwner, pElement);
                if (pDeviceElement)
                    AeListAppend(pMessage->pDeviceElements, pDeviceElement);
            }
        }
    }
    else
        pDeviceElement = AeListData(pItem, AeDRMDeviceElement);

    return pDeviceElement;
}

/**************************************************************************/
static AeXMLElement *AeDRMEMessageCreateDeviceElement(AeInt32 iDeviceId, AeChar *pOwner, AeBool bEmpty)
{
    AeXMLElement *pElement;
    AeDRMDevice *pDevice;
    
    pDevice = AePtrArrayGet(g_pDRMDevices, iDeviceId);
    if (!pDevice)
        return NULL;

    pElement = AeXMLElementNew();
    if (!pElement)
        return NULL;
    AeXMLElementSetName(pElement, EMESSAGE_ELEMENT_DEVICE);
    AeXMLElementSetEmpty(pElement, bEmpty);

    AeXMLElementAddAttribute(pElement, EMESSAGE_ATTR_MODEL_NUMBER, pDevice->pModelNumber);
    AeXMLElementAddAttribute(pElement, EMESSAGE_ATTR_SERIAL_NUMBER, pDevice->pSerialNumber);
    AeXMLElementAddAttribute(pElement, EMESSAGE_ATTR_OWNER, pOwner);

    return pElement;
}

/**************************************************************************/
static AeXMLElement *AeDRMEMessageCreateRegistrationElement(AeDRMEMessage *pMessage, AeInt32 iDeviceId, AeChar *pMasterOwner, AeTimeValue *pPingRate)
{
    AeXMLElement *pRegElement;
#ifdef __GATEWAY__
    AeXMLElement *pGwElement, *pDeviceElement;
#endif
    AeChar pTmp[64];

    pRegElement = AeXMLElementNew();
    if (!pRegElement)
        return NULL;
    AeXMLElementSetName(pRegElement, EMESSAGE_ELEMENT_REGISTRATION);
#ifndef __GATEWAY__
    AeXMLElementSetEmpty(pRegElement, AeTrue);
#else
    if (iDeviceId == AE_DRM_MASTER_DEVICE_ID)
        AeXMLElementSetEmpty(pRegElement, AeTrue);
    else
        AeXMLElementSetEmpty(pRegElement, AeFalse);
#endif

    /* [ping] rate attribute */
    snprintf(pTmp, sizeof(pTmp), "%ld", AE_TIME_VALUE_MILLISECONDS(*pPingRate));
    AeXMLElementAddAttribute(pRegElement, EMESSAGE_ATTR_RATE, pTmp);

    /* timestamp attribute */
    AeXMLElementAddAttribute(pRegElement, EMESSAGE_ATTR_TIMESTAMP, pMessage->pTimeStamp);

    /* device type attribute */
#ifndef __GATEWAY__
    AeXMLElementAddAttribute(pRegElement, EMESSAGE_ATTR_TYPE, "3");
#else
    if (iDeviceId == AE_DRM_MASTER_DEVICE_ID)
        AeXMLElementAddAttribute(pRegElement, EMESSAGE_ATTR_TYPE, "0");
    else
    {
        AeXMLElementAddAttribute(pRegElement, EMESSAGE_ATTR_TYPE, "1");

        /* add gateway element */
        if (pMasterOwner)
        {
            pGwElement = AeXMLElementAddElement(pRegElement, EMESSAGE_ELEMENT_GATEWAY,
                AeFalse);
            if (pGwElement)
            {
                /* add "parent" gateway device element */
                pDeviceElement = AeDRMEMessageCreateDeviceElement(AE_DRM_MASTER_DEVICE_ID,
                    pMasterOwner, AeTrue);
                if (pDeviceElement)
                    AeXMLElementAddContent(pGwElement, (AeXMLContent *) pDeviceElement);
            }
        }
    }
#endif

    return pRegElement;
}

/**************************************************************************/
static void AeDRMEMessageGetTimeStamp(AeTimeValue *pTime, AeChar *pOutput, AeInt iMaxLen, AeBool bForce)
{
    AeTimeValue validTime;

    /* reset time to 00:00.00 Jan-01-1970 UTC, when negative */
    validTime = *pTime;
    if (validTime.iSec < 0)
    {
        validTime.iSec = 0;
        validTime.iMicroSec = 0;
    }

    if (!g_drmSettings.bServerTimeStamp || bForce)
        AeGetISO8601Time(&validTime, pOutput, iMaxLen);
    else
        snprintf(pOutput, iMaxLen, "systime");
}

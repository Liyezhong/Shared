/* $Id: AeFileTransfer.c,v 1.15 2008/05/21 18:24:34 dkhodos Exp $ */

#include "Ae.h"

#if defined(ENABLE_FILE_TRANSFER)

static AeFileTransferPackage *AeFileTransferPackageNew(void);
static AeListItem *AeFileTransferPackageSelect(void);
static AeBool AeFileTransferAddInstruction(AeFileTransferPackage *pPackage, AeHandle pParameter);
static void AeFileTransferPostStatus(AeFileTransferPackage *pPackage, AeFileTransferStatus iStatus, AeFileTransferError iError);
static void AeFileInstructionRelease(AePointer pObject);

/**************************************************************************/
AeFileTransferPackage *AeFileTransferPackageNew(void)
{
    AeFileTransferPackage *pPackage = (AeFileTransferPackage *) AeAlloc(sizeof(AeFileTransferPackage));
    if (!pPackage)
        return NULL;
    memset(pPackage, 0, sizeof(AeFileTransferPackage));

    pPackage->pInstructionList = AeListNew(NULL, (AeReleaseFunc) AeFileInstructionRelease);
    if (!pPackage->pInstructionList)
    {
        AeFileTransferPackageDestroy(pPackage);
        return NULL;
    }

    pPackage->iPriority = 0;
    pPackage->iInstruction = 0;
    pPackage->iStatus = AeQUEUED;
    pPackage->iError = AeNOERROR;

    return pPackage;
}

/**************************************************************************/
void AeFileTransferPackageDestroy(AeFileTransferPackage *pPackage)
{
    if (pPackage->pJobId)
        AeFree(pPackage->pJobId);
    if (pPackage->pPublishedId)
        AeFree(pPackage->pPublishedId);

    if (pPackage->pInstructionList)
        AeListDestroy(pPackage->pInstructionList);

    AeFree(pPackage);
}

/******************************************************************************/
AeInt AeFileTransferPackageCompare(AePointer pData1, AePointer pData2)
{
    AeFileTransferPackage *pPackage1;
    AeFileTransferPackage *pPackage2;

    pPackage1 = (AeFileTransferPackage *) pData1;
    pPackage2 = (AeFileTransferPackage *) pData2;
    
    if (pPackage1->iPriority < pPackage2->iPriority)
        return -1;
    if (pPackage1->iPriority > pPackage2->iPriority)
        return 1;
    return 0;
}

/**************************************************************************/
/* so in case of multiple instructions in the package the highest priority
   is assigned to the package. */
void AeFileTransferPackageSetPriority(AeFileTransferPackage *pPackage, AeInt32 iPriority)
{
    if (iPriority > pPackage->iPriority)
        pPackage->iPriority = iPriority;
}

/**************************************************************************/
/* request from DRM to start file transfer */
AeBool AeFileTransferStart(AeInt32 iDeviceId, AeInt32 iConfigId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus)
{
    AeFileTransferPackage *pPackage;
    AeChar* pTmp;
    AeHandle pParameter;

    pStatus->iStatusCode = 0;
    pStatus->pStatusReason = NULL;

    pPackage = AeFileTransferPackageNew();
    if(!pPackage)
        return AeFalse;

    pPackage->iDeviceId = iDeviceId;
    pPackage->iConfigId = iConfigId;

    pTmp = AeDRMSOAPGetAttributeValueByName(pMethod, AE_FILETRANSFER_JOB_ID);
    AeSetString(&pPackage->pJobId, pTmp, -1);

    pTmp = AeDRMSOAPGetAttributeValueByName(pMethod, AE_FILETRANSFER_PUB_ID);
    AeSetString(&pPackage->pPublishedId, pTmp, -1);

    /* check required attributes */
    if (pPackage->pJobId == NULL || pPackage->pPublishedId == NULL)
    {
        AeFileTransferPostStatus(pPackage, AeERROR, AeBAD_FORMAT);
        AeFileTransferPackageDestroy(pPackage);
        return AeTrue;
    }

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_TRANSFER,
            "FileXfer package (id=%s): deployed",
            pPackage->pJobId);

    /* parse all the instructions of the package */
    pParameter = AeDRMSOAPGetFirstParameter(pMethod);
    while (pParameter)
    {
        if (!strcasecmp(AeDRMSOAPGetParameterName(pParameter), AE_FILETRANSFER_SOAPBODY))
        {
            AeHandle pChild;

            pChild = AeDRMSOAPGetParameterFirstChild(pParameter);
            if (pChild)
            {
                if (!AeFileTransferAddInstruction(pPackage, pChild))
                {
                    AeFileTransferPackageDestroy(pPackage);
                    return AeTrue;
                }
            }
        }

        pParameter = AeDRMSOAPGetNextParameter(pParameter);
    }

    /* If there are instructions to process */
    if (AeListCount(pPackage->pInstructionList) != 0)
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_TRANSFER,
                "FileXfer package (id=%s): initialized",
                pPackage->pJobId);

        AeListInsertSorted(g_pFileTransferPackages, pPackage, AeTrue);
        AeFileTransferPostStatus(pPackage, AeQUEUED, AeNOERROR);
        pPackage->iInstruction = 1;
    }

    /* Empty package */
    else
    {
        AeFileTransferPostStatus(pPackage, AeERROR, AeBAD_FORMAT);
        AeFileTransferPackageDestroy(pPackage);
    }

    return AeTrue;
}

/**************************************************************************/
AeBool AeFileTransferStop(AeInt32 iDeviceId, AeHandle pMethod, AeBool bPause, AeDRMSOAPCommandStatus *pStatus)
{
    AeFileTransferPackage *pPackage;
    AeFileInstruction *pInstr;
    AeListItem *pPackageItem, *pInstrItem;
    AeChar *pJobId;

    pJobId = AeDRMSOAPGetAttributeValueByName(pMethod, AE_FILETRANSFER_JOB_ID);
    if (!pJobId)
        return AeFalse;

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_TRANSFER,
            "FileXfer package (id=%s): %s requested",
            pJobId, bPause ? "pause" : "cancel");

    pStatus->iStatusCode = 0;
    pStatus->pStatusReason = NULL;

    pPackageItem = AeListFirst(g_pFileTransferPackages);
    while (pPackageItem)
    {
        pPackage = AeListData(pPackageItem, AeFileTransferPackage);
        if (!strcasecmp(pPackage->pJobId, pJobId))
        {
            /* destroy package that was no started right away. otherwise mark
               the package for cancellation. */
            if (pPackage->iStatus != AeSTARTED &&
                pPackage->iStatus != AePREEMPTION_PENDING &&
                pPackage->iStatus != AeCANCELLATION_PENDING &&
                pPackage->iStatus != AePAUSE_PENDING)
            {
                if (g_drmSettings.iLogLevel >= AeLogDebug)
                    AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_TRANSFER,
                        "FileXfer package (id=%s): removed from queue",
                        pPackage->pJobId);

                /* notify the application about pause/cancel */
                pInstrItem = AeListFirst(pPackage->pInstructionList);
                if (pInstrItem)
                {
                    pInstr = AeListData(pInstrItem, AeFileInstruction);
                    AeFileInstructionNotify(pInstr, bPause ? AeFileTransferPaused :
                                            AeFileTransferCancelled);
                }

                AeFileTransferPostStatus(pPackage, bPause ? AePAUSED : AeCANCELLED, AeNOERROR);
                AeListRemoveItem(g_pFileTransferPackages, pPackageItem);
                AeFileTransferPackageDestroy(pPackage);
            }
            else
                pPackage->iStatus = (bPause ? AePAUSE_PENDING : AeCANCELLATION_PENDING);

            return AeTrue;
        }

        pPackageItem = AeListNext(pPackageItem);
    }

    return AeTrue;
}

/**************************************************************************/
/* This is called periodically by the main process loop */
void AeFileTransferProcess(void)
{
    AeFileTransferPackage *pPackage;
    AeFileInstruction *pInstr;
    AeListItem *pPackageItem, *pInstrItem;

    /* select package for processing */
    pPackageItem = AeFileTransferPackageSelect();
    if (!pPackageItem)
        return;
    pPackage = AeListData(pPackageItem, AeFileTransferPackage);

    /* work with the the first instruction */
    pInstrItem = AeListFirst(pPackage->pInstructionList);
    if (pInstrItem)
        pInstr = AeListData(pInstrItem, AeFileInstruction);
    else
    {
        /* this should never happen */
        pPackage->iStatus = AeERROR;
        pPackage->iError = AeFAILED;
        pInstr = NULL;
    }

    if (pPackage->iStatus == AeQUEUED)
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_TRANSFER,
                "FileXfer package (id=%s): started",
                pPackage->pJobId);

        pPackage->iStatus = AeSTARTED;
        AeFileTransferPostStatus(pPackage, AeSTARTED, AeNOERROR);
    }

    if (pPackage->iStatus == AePREEMPTION_PENDING)
    {
        if (!AeFileInstructionIsPendingRequest(pInstr))
        {
            if (g_drmSettings.iLogLevel >= AeLogDebug)
                AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_TRANSFER,
                    "FileXfer package (id=%s): preempted",
                    pPackage->pJobId);

            AeFileInstructionNotify(pInstr, AeFileTransferPreempted);
			pPackage->iStatus = AePREEMPTED;
			AeFileTransferPostStatus(pPackage, AePREEMPTED, AeNOERROR);
            
        }
        return;
    }

    if (pPackage->iStatus == AePREEMPTED)
    {
        if (g_drmSettings.iLogLevel >= AeLogDebug)
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_TRANSFER,
                "FileXfer package (id=%s): reactivated",
                pPackage->pJobId);

        AeFileInstructionNotify(pInstr, AeFileTransferReactivated);
        pPackage->iStatus = AeSTARTED;
		AeFileTransferPostStatus(pPackage, AeSTARTED, AeNOERROR);
    }

    if (pPackage->iStatus == AeCANCELLATION_PENDING)
    {
        if (!AeFileInstructionIsPendingRequest(pInstr))
        {
            if (g_drmSettings.iLogLevel >= AeLogDebug)
                AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_TRANSFER,
                    "FileXfer package (id=%s): cancelled",
                    pPackage->pJobId);

            AeFileInstructionNotify(pInstr, AeFileTransferCancelled);
            pPackage->iStatus = AeCANCELLED;
        }
        else
            return;
    }

    if (pPackage->iStatus == AePAUSE_PENDING)
    {
        if (!AeFileInstructionIsPendingRequest(pInstr))
        {
            if (g_drmSettings.iLogLevel >= AeLogDebug)
                AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_FILE_TRANSFER,
                    "FileXfer package (id=%s): paused",
                    pPackage->pJobId);

            AeFileInstructionNotify(pInstr, AeFileTransferPaused);
            pPackage->iStatus = AePAUSED;
        }
        else
            return;
    }

    if (pPackage->iStatus == AeSTARTED)
    {
        /* process instruction */
        AeFileInstructionProcess(pInstr);

        if (pInstr->iState != AeFileInstructionFinished)
            return;
    
        if (pInstr->iError != AeNOERROR)
        {
            if (g_drmSettings.iLogLevel >= AeLogError)
                AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_TRANSFER,
                    "FileXfer package (id=%s): failed at instruction %d",
                    pPackage->pJobId, pPackage->iInstruction);

            pPackage->iStatus = AeERROR;
            pPackage->iError = pInstr->iError;
        }
        else if (AeListCount(pPackage->pInstructionList) == 1)
        {
            if (g_drmSettings.iLogLevel >= AeLogInfo)
                AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_FILE_TRANSFER,
                    "FileXfer package (id=%s): complete",
                    pPackage->pJobId);

            pPackage->iStatus = AeSUCCESS;
        }
        else
            pPackage->iInstruction += 1;

        /* destroy executed instruction */
        AeListRemoveItem(pPackage->pInstructionList, pInstrItem);
        AeFileInstructionDestroy(pInstr);
    }

    /* remove finished package */
    if (pPackage->iStatus == AeSUCCESS ||
        pPackage->iStatus == AeERROR ||
        pPackage->iStatus == AeCANCELLED ||
        pPackage->iStatus == AePAUSED)
    {
        AeFileTransferPostStatus(pPackage, pPackage->iStatus, pPackage->iError);
        AeListRemoveItem(g_pFileTransferPackages, pPackageItem);
        AeFileTransferPackageDestroy(pPackage);
    }
}

/**************************************************************************/
/* This function examines the first package in the queue to determine
   whether file transfer as whole is idle. While in cases where a package
   is being preempted the first package is not the one that is being
   processed, the approach is still OK, because the package in
   AePREEMPTION_PENDING state is not going to be idle, which will also
   be true for the first package. */
AeBool AeFileTransferIsIdle(void)
{
    AeFileTransferPackage *pPackage;
    AeFileInstruction *pInstr;
    AeListItem *pItem;

    pItem = AeListFirst(g_pFileTransferPackages);
    if (!pItem)
        return AeTrue;

    pPackage = AeListData(pItem, AeFileTransferPackage);

    pItem = AeListFirst(pPackage->pInstructionList);
    if (!pItem)
        return AeTrue;

    pInstr = AeListData(pItem, AeFileInstruction);

    return AeFileInstructionIsIdle(pInstr);
}

/**************************************************************************/
AeFileTransferError AeFileTransferTranslateError(AeError iError)
{
    /* FIXME: handle more errors */
    return AeHTTP_FAILURE;
}

/**************************************************************************/
static AeListItem *AeFileTransferPackageSelect(void)
{
    AeFileTransferPackage *pPackage;
    AeListItem *pItem;

    // try to find currently active package
    pItem = AeListFirst(g_pFileTransferPackages);
    while (pItem)
    {
        pPackage = AeListData(pItem, AeFileTransferPackage);
        if (pPackage->iStatus == AeSTARTED)
        {
            /* active package in the middle of the queue must be preempted */
            if (pItem == AeListFirst(g_pFileTransferPackages))
                return pItem;
            else
                pPackage->iStatus = AePREEMPTION_PENDING;
        }

        if (pPackage->iStatus == AePREEMPTION_PENDING ||
            pPackage->iStatus == AeCANCELLATION_PENDING ||
            pPackage->iStatus == AePAUSE_PENDING)
            return pItem;
        
        pItem = AeListNext(pItem);
    }

    // no active packages found - work with the first one in the queue
    return AeListFirst(g_pFileTransferPackages);
}

/**************************************************************************/
static AeBool AeFileTransferAddInstruction(AeFileTransferPackage *pPackage, AeHandle pParameter)
{
    AeFileInstruction *pInstr;
    pInstr = NULL;

    /* download instruction */
    if (!strcasecmp(AeDRMSOAPGetParameterName(pParameter), AE_FILETRANSFER_DOWNLOAD))
        pInstr = (AeFileInstruction *) AeFileDownloadNew();

    /* upload instruction */
    else if (!strcasecmp(AeDRMSOAPGetParameterName(pParameter), AE_FILETRANSFER_UPLOAD))
        pInstr = (AeFileInstruction *) AeFileUploadNew();

    /* Agent restart */
    else if (!strcasecmp(AeDRMSOAPGetParameterName(pParameter), AE_PACKAGE_RESTART))
        pInstr = (AeFileInstruction *) AeAgentRestartNew();

    else
    {
        if (g_drmSettings.iLogLevel >= AeLogError)
            AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_TRANSFER,
                "FileXfer package (id=%s): unrecognized instruction: %s",
                pPackage->pJobId, AeDRMSOAPGetParameterName(pParameter));
    }

    if (pInstr)
    {
        pInstr->pPackage = pPackage;
        if (AeFileInstructionInitialize(pInstr, pParameter))
            AeListAppend(pPackage->pInstructionList, pInstr);
        else
        {
            if (g_drmSettings.iLogLevel >= AeLogError)
                AE_LOG_EX(AeLogError, AE_LOG_CATEGORY_FILE_TRANSFER,
                    "FileXfer package (id=%s): instruction %d initialization failed with code: %d",
                    pPackage->pJobId, AeListCount(pPackage->pInstructionList) + 1,
                    pInstr->iError);

            AeFileTransferPostStatus(pPackage, AeERROR, pInstr->iError);
            AeFileInstructionDestroy(pInstr);
            return AeFalse;
        }
    }

    return AeTrue;
}

/**************************************************************************/
static void AeFileTransferPostStatus(AeFileTransferPackage *pPackage, AeFileTransferStatus iStatus, AeFileTransferError iError)
{
    AeXMLElement *pElement;

    pElement = AeDRMEMessageCreatePackageStatus(pPackage->pJobId, iStatus, iError, pPackage->iInstruction);
    if (!pElement)
        return ;
    AeDRMQueueAddItem(g_pQueue, AeDRMQueueItemMisc, pPackage->iDeviceId, pPackage->iConfigId,
                      AeDRMGetFormattedContent((AeXMLContent *) pElement), AeDRMQueuePriorityNormal);

    AeXMLContentDestroy((AeXMLContent *) pElement);
}

/**************************************************************************/
static void AeFileInstructionRelease(AePointer pObject)
{
    AeFileInstructionDestroy((AeFileInstruction *) pObject);
}

#endif /* defined(ENABLE_FILE_TRANSFER) */

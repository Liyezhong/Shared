/* $Id: AeDRMQueue.c,v 1.5 2008/05/21 18:24:33 dkhodos Exp $ */

#include "Ae.h"

static AeInt32  AeDRMQueueItemCalculateSize(AeDRMQueueItem *pItem);
static void     AeDRMQueueReportItem(AeDRMQueueItem *pItem);
static void     AeDRMQueueCheckStatus(AeDRMQueue *pQueue, AeDRMQueueStatus iNewStatus);
static void     AeDRMQueueDeleteItem(AeDRMQueue *pQueue, AeDRMQueueItem *pItem);
static void     AeDRMQueueItemRelease(AePointer pObject);

/******************************************************************************/
AeDRMQueueItem *AeDRMQueueItemNew(AeDRMQueueItemType iType, AeInt32 iDeviceId, AeInt32 iConfigId, AeArray *pContent, AeDRMQueuePriority iPriority)
{
    AeDRMQueueItem *pItem;

    pItem = (AeDRMQueueItem *) AeAlloc(sizeof(AeDRMQueueItem));
    if (!pItem)
        return NULL;
    memset(pItem, 0, sizeof(AeDRMQueueItem));

    pItem->iType = iType;
    pItem->iDeviceId = iDeviceId;
    pItem->iConfigId = iConfigId;
    pItem->pContent = pContent;
    pItem->iPriority = iPriority;
    pItem->iMessageId = -1;

    return pItem;
}

/******************************************************************************/
void AeDRMQueueItemDestroy(AeDRMQueueItem *pItem)
{
    if (pItem->pContent)
        AeArrayDestroy(pItem->pContent);

    AeFree(pItem);
}

/******************************************************************************/
static AeInt32 AeDRMQueueItemCalculateSize(AeDRMQueueItem *pItem)
{
    AeInt32 iSize;

    /* calculate overall memory usage for item */
    iSize = sizeof(AeListItem) +
        sizeof(AeDRMQueueItem) +
        sizeof(AeArray) +
        AeArrayCount(pItem->pContent) + 1;

    /* estimate memory waste due to memory manager expenses */
    iSize = (AeInt32) (iSize * AE_MEMMAN_WASTE_RATIO);

    return iSize;
}

/******************************************************************************/
AeDRMQueue *AeDRMQueueNew(void)
{
    AeDRMQueue *pQueue;

    pQueue = (AeDRMQueue *) AeAlloc(sizeof(AeDRMQueue));
    if (!pQueue)
        return NULL;
    memset(pQueue, 0, sizeof(AeDRMQueue));

    pQueue->pItemList = AeListNew(NULL, (AeReleaseFunc) AeDRMQueueItemRelease);
    if (!pQueue->pItemList)
        return NULL;

    AeMutexInitialize(&pQueue->lock);
    pQueue->iDataSize = 0;
    pQueue->iStatus = AeDRMQueueEmpty;

    return pQueue;
}

/******************************************************************************/
void AeDRMQueueDestroy(AeDRMQueue *pQueue)
{
    if (pQueue->pItemList)
        AeListDestroy(pQueue->pItemList);

    AeMutexDestroy(&pQueue->lock);

    AeFree(pQueue);
}

/******************************************************************************/
AeError AeDRMQueueAddItem(AeDRMQueue *pQueue, AeDRMQueueItemType iType, AeInt32 iDeviceId, AeInt32 iConfigId, AeArray *pContent, AeDRMQueuePriority iPriority)
{
    AeDRMQueueItem *pNewItem, *pTailItem, *pItem;
    AeListItem *pListItem;
    AeInt32 iItemSize;
    AeBool bRemoved;

    if (!pContent)
        return AeEInvalidArg;

    pNewItem = AeDRMQueueItemNew(iType, iDeviceId, iConfigId, pContent, iPriority);
    if (!pNewItem)
        return AeEMemory;

    /* output debug message */
    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AeDRMQueueReportItem(pNewItem);

    iItemSize = AeDRMQueueItemCalculateSize(pNewItem);

    /* check that item is not too large */
    if (iItemSize > g_drmSettings.iQueueSize)
    {
        if (g_drmSettings.iLogLevel >= AeLogWarning)
            AE_LOG_EX(AeLogWarning, AE_LOG_CATEGORY_DATA_QUEUE,
                "Data item is too large (discarded)");

        AeDRMQueueItemDestroy(pNewItem);
        return AeEOK;
    }

    /* lock the queue */
    AeMutexLock(&pQueue->lock);

    /* remove least recent and least prioritized items to fit new item */
    bRemoved = AeFalse;
    while (pQueue->iDataSize + iItemSize > g_drmSettings.iQueueSize)
    {
        /* discard item which is less prioritized than items already in the queue */
        pTailItem = AeListData(AeListLast(pQueue->pItemList), AeDRMQueueItem);
        if (pTailItem->iPriority > pNewItem->iPriority)
        {
            if (g_drmSettings.iLogLevel >= AeLogWarning)
                AE_LOG_EX(AeLogWarning, AE_LOG_CATEGORY_DATA_QUEUE,
                    "Discarding new item with lower priority (queue is full)");

            AeMutexUnlock(&pQueue->lock);

            /* report queue status */
            AeDRMQueueCheckStatus(pQueue, AeDRMQueueFull);

            AeDRMQueueItemDestroy(pNewItem);
            return AeEOK;
        }

        /* traverse the queue starting from head */
        pListItem = AeListFirst(pQueue->pItemList);
        pItem = AeListData(pListItem, AeDRMQueueItem);
        while (pListItem)
        {
            pItem = AeListData(pListItem, AeDRMQueueItem);
            
            /* remove the first item with lowest priority */
            if (pItem->iPriority == pTailItem->iPriority)
                break;

            pListItem = AeListNext(pListItem);
        }

        if (g_drmSettings.iLogLevel >= AeLogWarning)
            AE_LOG_EX(AeLogWarning, AE_LOG_CATEGORY_DATA_QUEUE,
                "Discarding undelivered queue item (queue is full)");

        AeListRemoveItem(pQueue->pItemList, pListItem);
        AeDRMQueueDeleteItem(pQueue, pItem);
        bRemoved = AeTrue;
    }

    /* to insert new item traverse the queue starting from tail */
    pListItem = AeListLast(pQueue->pItemList);
    while (pListItem)
    {
        pItem = AeListData(pListItem, AeDRMQueueItem);

        /* insert new item after item with equal or greater priority */
        if (pItem->iPriority >= pNewItem->iPriority)
            break;

        pListItem = AeListPrev(pListItem);
    }
    if (pListItem)
        AeListInsertAfter(pQueue->pItemList, pListItem, pNewItem);
    else
        AeListPrepend(pQueue->pItemList, pNewItem);

    /* update overall queue size */
    pQueue->iDataSize += iItemSize;

    /* unlock the queue */
    AeMutexUnlock(&pQueue->lock);

    /* report queue status */
    AeDRMQueueCheckStatus(pQueue, bRemoved ? AeDRMQueueFull : AeDRMQueueNonEmpty);

    return AeEOK;
}

/******************************************************************************/
AeListItem *AeDRMQueueOpen(AeDRMQueue *pQueue)
{
    AeMutexLock(&pQueue->lock);

    return AeListFirst(pQueue->pItemList);
}

/******************************************************************************/
AeDRMQueueItem *AeDRMQueueFetchNext(AeDRMQueue *pQueue, AeListItem **ppPos)
{
    AeDRMQueueItem *pItem;

    if (ppPos)
    {
        pItem = AeListData(*ppPos, AeDRMQueueItem);
        *ppPos = AeListNext(*ppPos);

        return pItem;
    }

    return NULL;
}

/******************************************************************************/
void AeDRMQueueClose(AeDRMQueue *pQueue)
{
    AeMutexUnlock(&pQueue->lock);
}

/******************************************************************************/
void AeDRMQueueDeleteMessageItems(AeDRMQueue *pQueue, AeInt32 iMessageId, AeInt32 iDeviceId)
{
    AeDRMQueueItem *pItem;
    AeListItem *pListItem, *pPrevListItem;
    AeBool bRemoved;

    /* lock the queue */
    AeMutexLock(&pQueue->lock);

    /* look through the queue for items with matching message id */
    bRemoved = AeFalse;
    pListItem = AeListFirst(pQueue->pItemList);
    while (pListItem)
    {
        pPrevListItem = pListItem;
        pItem = AeListData(pListItem, AeDRMQueueItem);
        pListItem = AeListNext(pListItem);

        /* remove matching item */
        if (pItem->iMessageId == iMessageId &&
            (pItem->iDeviceId == iDeviceId || iDeviceId == AE_DRM_INVALID_DEVICE_ID))
        {
            AeListRemoveItem(pQueue->pItemList, pPrevListItem);
            AeDRMQueueDeleteItem(pQueue, pItem);
            bRemoved = AeTrue;
        }
    }

    /* unlock the queue */
    AeMutexUnlock(&pQueue->lock);

    /* report queue status */
    if (bRemoved)
        AeDRMQueueCheckStatus(pQueue, pQueue->iDataSize == 0 ?
            AeDRMQueueEmpty : AeDRMQueueNonEmpty);
}

/******************************************************************************/
AeInt AeDRMQueueGetLevelPct(AeDRMQueue *pQueue)
{
    if (g_drmSettings.iQueueSize == 0)
        return 100;

    return (AeInt) ((AeDouble) pQueue->iDataSize / g_drmSettings.iQueueSize * 100);
}

/******************************************************************************/
static void AeDRMQueueReportItem(AeDRMQueueItem *pItem)
{
    AeDRMDevice *pDevice;
    
    pDevice = AePtrArrayGet(g_pDRMDevices, pItem->iDeviceId);
    if (!pDevice)
        return;

    switch (pItem->iType)
    {
        case AeDRMQueueItemSnapshot:
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_DATA_QUEUE,
                "Received snapshot: model: %s, serial number: %s",
                pDevice->pModelNumber, pDevice->pSerialNumber);
            break;
        case AeDRMQueueItemAlarm:
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_DATA_QUEUE,
                "Received alarm data: model: %s, serial number: %s",
                pDevice->pModelNumber, pDevice->pSerialNumber);
            break;
        case AeDRMQueueItemEvent:
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_DATA_QUEUE,
                "Received event data: model: %s, serial number: %s",
                pDevice->pModelNumber, pDevice->pSerialNumber);
            break;
        case AeDRMQueueItemEmail:
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_DATA_QUEUE,
                "Received e-mail data: model: %s, serial number: %s",
                pDevice->pModelNumber, pDevice->pSerialNumber);
            break;
        case AeDRMQueueItemSOAPStatus:
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_DATA_QUEUE,
                "Received SOAP command status: model: %s, serial number: %s",
                pDevice->pModelNumber, pDevice->pSerialNumber);
            break;
        case AeDRMQueueItemMisc:
            AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_DATA_QUEUE,
                "Received data: model: %s, serial number: %s",
                pDevice->pModelNumber, pDevice->pSerialNumber);
            break;
    }
}

/******************************************************************************/
static void AeDRMQueueCheckStatus(AeDRMQueue *pQueue, AeDRMQueueStatus iNewStatus)
{
    AeChar *pMessage;

    /* process status changes */
    if (iNewStatus == pQueue->iStatus)
        return;

    /* choose appropriate message */
    if (g_drmSettings.iLogLevel >= AeLogDebug)
    {
        switch (iNewStatus)
        {
            case AeDRMQueueEmpty:
                pMessage = "DRM queue is empty";
                break;
            case AeDRMQueueNonEmpty:
                pMessage = "DRM queue is non-empty";
                break;
            case AeDRMQueueFull:
                pMessage = "DRM queue is full";
                break;
            default:
                return;
        }

        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_DATA_QUEUE, pMessage);
    }

    /* save new status */
    AeMutexLock(&pQueue->lock);
    pQueue->iStatus = iNewStatus;
    AeMutexUnlock(&pQueue->lock);

    /* invoke callback */
    if (g_drmCallbacks.pOnQueueStatus)
        (*g_drmCallbacks.pOnQueueStatus)(iNewStatus);
}

/******************************************************************************/
static void AeDRMQueueDeleteItem(AeDRMQueue *pQueue, AeDRMQueueItem *pItem)
{
    /* adjust total data size */
    pQueue->iDataSize -= AeDRMQueueItemCalculateSize(pItem);
    if (pQueue->iDataSize < 0)
        pQueue->iDataSize = 0;

    /* destroy item */
    AeDRMQueueItemDestroy(pItem);
}

/******************************************************************************/
static void AeDRMQueueItemRelease(AePointer pObject)
{
    AeDRMQueueItemDestroy((AeDRMQueueItem *) pObject);
}

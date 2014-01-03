/* $Id: AeDRMQueue.h,v 1.2 2003/12/12 18:11:14 dkhodos Exp $ */

#ifndef _AE_DRM_QUEUE_H_
#define _AE_DRM_QUEUE_H_

#define AE_DRM_QUEUE_DEFAULT_SIZE   1048576
#define AE_MEMMAN_WASTE_RATIO       1.6

typedef enum _AeDRMQueueItemType AeDRMQueueItemType;
typedef struct _AeDRMQueueItem AeDRMQueueItem;
typedef struct _AeDRMQueue AeDRMQueue;

enum _AeDRMQueueItemType
{
    AeDRMQueueItemSnapshot,
    AeDRMQueueItemAlarm,
    AeDRMQueueItemEvent,
    AeDRMQueueItemEmail,
    AeDRMQueueItemSOAPStatus,
    AeDRMQueueItemMisc
};

struct _AeDRMQueueItem
{
    AeDRMQueueItemType  iType;
    AeInt32             iDeviceId;
    AeInt32             iConfigId;
    AeArray             *pContent;
    AeDRMQueuePriority  iPriority;
    AeInt32             iMessageId;
};

struct _AeDRMQueue
{
    AeList              *pItemList;
    AeMutex             lock;
    AeInt32             iDataSize;
    AeDRMQueueStatus    iStatus;
};

#ifdef __cplusplus
extern "C" {
#endif

AeDRMQueueItem  *AeDRMQueueItemNew(AeDRMQueueItemType iType, AeInt32 iDeviceId, AeInt32 iConfigId, AeArray *pContent, AeDRMQueuePriority iPriority);
void            AeDRMQueueItemDestroy(AeDRMQueueItem *pItem);

AeDRMQueue      *AeDRMQueueNew(void);
void            AeDRMQueueDestroy(AeDRMQueue *pQueue);
AeError         AeDRMQueueAddItem(AeDRMQueue *pQueue, AeDRMQueueItemType iType, AeInt32 iDeviceId, AeInt32 iConfigId, AeArray *pContent, AeDRMQueuePriority iPriority);
AeListItem      *AeDRMQueueOpen(AeDRMQueue *pQueue);
AeDRMQueueItem  *AeDRMQueueFetchNext(AeDRMQueue *pQueue, AeListItem **ppPos);
void            AeDRMQueueClose(AeDRMQueue *pQueue);
void            AeDRMQueueDeleteMessageItems(AeDRMQueue *pQueue, AeInt32 iMessageId, AeInt32 iDeviceId);
AeInt           AeDRMQueueGetLevelPct(AeDRMQueue *pQueue);

#ifdef __cplusplus
}
#endif

#endif

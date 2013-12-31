/* $Id: AeContainers.c,v 1.3.2.1 2009/05/28 18:37:08 hfeng Exp $ */

#include "Ae.h"

static AeBool   AeArrayExpand(AeArray *pArray, AeInt32 iCount);
static AeBool   AePtrArrayExpand(AePtrArray *pArray, AeInt32 iCount);

/******************************************************************************/
AeList *AeListNew(AeCompareFunc pCompareFunc, AeReleaseFunc pReleaseFunc)
{
    AeList *pList;

    pList = (AeList *) AeAlloc(sizeof(AeList));
    if (!pList)
        return NULL;
    memset(pList, 0, sizeof(AeList));

    pList->pCompareFunc = pCompareFunc;
    pList->pReleaseFunc = pReleaseFunc;
    pList->pFirstItem = NULL;
    pList->pLastItem = NULL;
    pList->iCount = 0;

    return pList;
}

/******************************************************************************/
void AeListDestroy(AeList *pList)
{
    AeListFree(pList);

    AeFree(pList);
}

/******************************************************************************/
void AeListFree(AeList *pList)
{
    AeListItem *pItem, *pTmp;

    pItem = pList->pFirstItem;
    while (pItem)
    {
        pTmp = pItem;
        pItem = pItem->pNext;
        if (pList->pReleaseFunc)
            (*pList->pReleaseFunc)(pTmp->pData);
        AeFree(pTmp);
    }

    pList->pFirstItem = NULL;
    pList->pLastItem = NULL;
    pList->iCount = 0;
}

/******************************************************************************/
AeListItem *AeListAppend(AeList *pList, AePointer pData)
{
    AeListItem *pItem;

    pItem = (AeListItem *) AeAlloc(sizeof(AeListItem));
    if (!pItem)
        return NULL;
    memset(pItem, 0, sizeof(AeListItem));

    pItem->pNext = NULL;
    pItem->pPrev = pList->pLastItem;
    pItem->pData = pData;

    if (pList->pLastItem)
        pList->pLastItem->pNext = pItem;
    else
        pList->pFirstItem = pItem;
    pList->pLastItem = pItem;

    pList->iCount += 1;

    return pList->pFirstItem;
}

/******************************************************************************/
AeListItem *AeListPrepend(AeList *pList, AePointer pData)
{
    AeListItem *pItem;

    pItem = (AeListItem *) AeAlloc(sizeof(AeListItem));
    if (!pItem)
        return NULL;
    memset(pItem, 0, sizeof(AeListItem));

    pItem->pNext = pList->pFirstItem;
    pItem->pPrev = NULL;
    pItem->pData = pData;

    if (pList->pFirstItem)
        pList->pFirstItem->pPrev = pItem;
    else
        pList->pLastItem = pItem;
    pList->pFirstItem = pItem;

    pList->iCount += 1;

    return pList->pFirstItem;
}

/******************************************************************************/
AeListItem *AeListInsertAfter(AeList *pList, AeListItem *pItem, AePointer pData)
{
    AeListItem *pNewItem;

    pNewItem = (AeListItem *) AeAlloc(sizeof(AeListItem));
    if (!pNewItem)
        return NULL;
    memset(pNewItem, 0, sizeof(AeListItem));

    pNewItem->pNext = pItem->pNext;
    pNewItem->pPrev = pItem;
    pNewItem->pData = pData;

    if (pItem->pNext)
        pItem->pNext->pPrev = pNewItem;
    else
        pList->pLastItem = pNewItem;

    pItem->pNext = pNewItem;

    pList->iCount += 1;

    return pList->pFirstItem;
}

/******************************************************************************/
AeListItem *AeListInsertBefore(AeList *pList, AeListItem *pItem, AePointer pData)
{
    AeListItem *pNewItem;

    pNewItem = (AeListItem *) AeAlloc(sizeof(AeListItem));
    if (!pNewItem)
        return NULL;
    memset(pNewItem, 0, sizeof(AeListItem));

    pNewItem->pNext = pItem;
    pNewItem->pPrev = pItem->pPrev;
    pNewItem->pData = pData;

    if (pItem->pPrev)
        pItem->pPrev->pNext = pNewItem;
    else
        pList->pFirstItem = pNewItem;

    pItem->pPrev = pNewItem;

    pList->iCount += 1;

    return pList->pFirstItem;
}

/******************************************************************************/
AeListItem *AeListRemove(AeList *pList, AePointer pData)
{
    AeListItem *pItem;

    pItem = AeListFind(pList, pData);
    if (pItem)
        return AeListRemoveItem(pList, pItem);
    else
        return pList->pFirstItem;
}

/******************************************************************************/
AeListItem *AeListRemoveItem(AeList *pList, AeListItem *pItem)
{
    if (pList->iCount == 0)
        return NULL;

    if (pItem->pNext)
        pItem->pNext->pPrev = pItem->pPrev;
    else
        pList->pLastItem = pItem->pPrev;

    if (pItem->pPrev)
        pItem->pPrev->pNext = pItem->pNext;
    else
        pList->pFirstItem = pItem->pNext;

    AeFree(pItem);

    pList->iCount -= 1;

    return pList->pFirstItem;
}

/******************************************************************************/
AeListItem *AeListFind(AeList *pList, AePointer pData)
{
    AeListItem *pItem;

    if (!pList->pCompareFunc)
        return NULL;

    pItem = pList->pFirstItem;
    while (pItem)
    {
        if (!(*pList->pCompareFunc)(pItem->pData, pData))
            return pItem;
        pItem = pItem->pNext;
    }

    return NULL;
}

/******************************************************************************/
AeListItem  *AeListInsertSorted(AeList *pList, AePointer pData, AeBool bAscending)
{
    AeListItem *pItem;

    if (!pList->pCompareFunc)
        return AeListAppend(pList, pData);

    pItem = pList->pFirstItem;
    while (pItem)
    {
        int iCompareResult;

        iCompareResult = (*pList->pCompareFunc)(pItem->pData, pData);
        if ((bAscending == AeTrue && iCompareResult > 0) ||
            (bAscending == AeFalse && iCompareResult < 0))
        {
            AeListInsertBefore(pList, pItem, pData);
            return pList->pFirstItem;
        }

        pItem = pItem->pNext;
    }

    return AeListAppend(pList, pData);
}

/******************************************************************************/
AeInt AePointerCompare(AePointer pData1, AePointer pData2)
{
    return !(pData1 == pData2);
}

/******************************************************************************/
AeArray *AeArrayNew(AeInt32 iElementSize)
{
    AeArray *pArray;

    pArray = (AeArray *) AeAlloc(sizeof(AeArray));
    if (!pArray)
        return NULL;
    memset(pArray, 0, sizeof(AeArray));

    pArray->pData = NULL;
    pArray->iCount = 0;
    pArray->iAllocSize = 0;
    pArray->iElementSize = iElementSize;

    return pArray;
}

/******************************************************************************/
void AeArrayDestroy(AeArray *pArray)
{
    if (pArray->pData)
        AeFree(pArray->pData);

    AeFree(pArray);
}

/******************************************************************************/
void AeArraySet(AeArray *pArray, AeInt32 iIndex, AePointer pData, AeInt32 iCount)
{
    AeInt32 iNewCount;

    if (iIndex >= pArray->iCount)
    {
        if (!AeArrayExpand(pArray, iIndex - pArray->iCount + 1))
            return;

        memset(AeArrayElementPtr(pArray, pArray->iCount), 0,
            AeArrayElementSize(pArray, iIndex - pArray->iCount + 1));
    
        pArray->iCount = iIndex + 1;
    }

    iNewCount = iIndex + iCount;
    if (iNewCount > pArray->iCount)
    {
        if (!AeArrayExpand(pArray, iNewCount - pArray->iCount))
            return;

        pArray->iCount = iNewCount;
    }

    memcpy(AeArrayElementPtr(pArray, iIndex), pData, AeArrayElementSize(pArray, iCount));

    memset(AeArrayElementPtr(pArray, pArray->iCount), 0,
        pArray->iElementSize);
}

/******************************************************************************/
void AeArrayInsert(AeArray *pArray, AeInt32 iIndex, AePointer pData, AeInt32 iCount)
{
    if (iIndex >= pArray->iCount)
    {
        AeArraySet(pArray, iIndex, pData, iCount);
        return;
    }

    if (!AeArrayExpand(pArray, iCount))
        return;

    memmove(AeArrayElementPtr(pArray, iIndex + iCount),
        AeArrayElementPtr(pArray, iIndex),
        AeArrayElementSize(pArray, pArray->iCount - iIndex));

    memcpy(AeArrayElementPtr(pArray, iIndex), pData, AeArrayElementSize(pArray, iCount));

    pArray->iCount += iCount;

    memset(AeArrayElementPtr(pArray, pArray->iCount), 0,
        pArray->iElementSize);
}

/******************************************************************************/
void AeArrayAppend(AeArray *pArray, AePointer pData, AeInt32 iCount)
{
    AeArraySet(pArray, pArray->iCount, pData, iCount);
}

/******************************************************************************/
static AeBool AeArrayExpand(AeArray *pArray, AeInt32 iCount)
{
    AeInt32 iReqSize, iAllocSize;
    AeChar *pData;

    /* calculate required amount of memory */
    iReqSize = (pArray->iCount + iCount + 1) * pArray->iElementSize;
    if (iReqSize <= pArray->iAllocSize)
        return AeTrue;

    /* calculate closest power of 2 */
    iAllocSize = 1;
    while (iAllocSize < iReqSize)
        iAllocSize <<= 1;

    iAllocSize = AE_MAX(iAllocSize, AE_ARRAY_MIN_SIZE);

    /* allocate memory */
    pData = AeAlloc(iAllocSize);
    if (!pData)
        return AeFalse;

    if (pArray->pData)
    {
        memcpy(pData, pArray->pData, pArray->iAllocSize - pArray->iElementSize);
        AeFree(pArray->pData);
    }

    pArray->pData = pData;
    pArray->iAllocSize = iAllocSize;

    return AeTrue;
}

/******************************************************************************/
void AeArrayEmpty(AeArray *pArray)
{
    pArray->iCount = 0;
}

/******************************************************************************/
AePtrArray *AePtrArrayNew(AeCompareFunc pCompareFunc, AeReleaseFunc pReleaseFunc)
{
    AePtrArray *pArray;

    pArray = (AePtrArray *) AeAlloc(sizeof(AePtrArray));
    if (!pArray)
        return NULL;
    memset(pArray, 0, sizeof(AePtrArray));

    pArray->pCompareFunc = pCompareFunc;
    pArray->pReleaseFunc = pReleaseFunc;
    pArray->pData = NULL;
    pArray->iCount = 0;
    pArray->iAllocCount = 0;

    return pArray;
}

/******************************************************************************/
void AePtrArrayDestroy(AePtrArray *pArray)
{
    AePtrArrayFree(pArray);

    AeFree(pArray);
}

/******************************************************************************/
void AePtrArrayFree(AePtrArray *pArray)
{
    int i;

    if (pArray->pReleaseFunc)
    {
        for (i = 0; i < pArray->iCount; i++)
        {
            if (pArray->pData[i])
                (*pArray->pReleaseFunc)(pArray->pData[i]);
        }
    }

    AeFree(pArray->pData);
    pArray->pData = NULL;
    pArray->iCount = 0;
    pArray->iAllocCount = 0;
}

/******************************************************************************/
void AePtrArraySet(AePtrArray *pArray, AeInt32 iIndex, AePointer pData)
{
    if (iIndex >= pArray->iCount)
    {
        if (!AePtrArrayExpand(pArray, iIndex - pArray->iCount + 1))
            return;

        memset(pArray->pData + pArray->iCount, 0,
            (iIndex - pArray->iCount + 1) * sizeof(AePointer));
    
        pArray->iCount = iIndex + 1;
    }

    pArray->pData[iIndex] = pData;
}

/******************************************************************************/
void AePtrArrayInsert(AePtrArray *pArray, AeInt32 iIndex, AePointer pData)
{
    if (iIndex >= pArray->iCount)
    {
        AePtrArraySet(pArray, iIndex, pData);
        return;
    }

    if (!AePtrArrayExpand(pArray, 1))
        return;

    memmove(pArray->pData + iIndex + 1, pArray->pData + iIndex,
        (pArray->iCount - iIndex) * sizeof(AePointer));

    pArray->pData[iIndex] = pData;

    pArray->iCount += 1;
}

/******************************************************************************/
void AePtrArrayAppend(AePtrArray *pArray, AePointer pData)
{
    AePtrArraySet(pArray, pArray->iCount, pData);
}

/******************************************************************************/
AePointer AePtrArrayFind(AePtrArray *pArray, AePointer pData)
{
    AeInt32 i;

    if (!pArray->pCompareFunc)
        return NULL;

    for (i = 0; i < pArray->iCount; i++)
    {
        if (!(*pArray->pCompareFunc)(pArray->pData[i], pData))
            return pArray->pData[i];
    }

    return NULL;
}

/******************************************************************************/
static AeBool AePtrArrayExpand(AePtrArray *pArray, AeInt32 iCount)
{
    AeInt32 iReqCount, iAllocCount;
    AePointer pData;

    iReqCount = pArray->iCount + iCount;
    if (iReqCount <= pArray->iAllocCount)
        return AeTrue;

    /* calculate closest power of 2 */
    iAllocCount = 1;
    while (iAllocCount < iReqCount)
        iAllocCount <<= 1;

    iAllocCount = AE_MAX(iAllocCount, AE_ARRAY_MIN_SIZE);

    /* allocate memory */
    pData = AeAlloc(iAllocCount * sizeof(AePointer));
    if (!pData)
        return AeFalse;

    if (pArray->pData)
    {
        memcpy(pData, pArray->pData, pArray->iAllocCount * sizeof(AePointer));
        AeFree(pArray->pData);
    }

    pArray->pData = pData;
    pArray->iAllocCount = iAllocCount;

    return AeTrue;
}

/******************************************************************************/
AeBuffer *AeBufferNew(AeChar *pContent, AeInt32 iSize, AeInt32 iChunkSize)
{
    AeBuffer *pBuffer;

    pBuffer = (AeBuffer *) AeAlloc(sizeof(AeBuffer));
    if (!pBuffer)
        return NULL;
    memset(pBuffer, 0, sizeof(AeBuffer));

    if (pContent && iSize)
    {
        pBuffer->pData = pContent;
        pBuffer->iSize = iSize;
        pBuffer->bExternalData = AeTrue;
    }
    else
    {
        pBuffer->pData = NULL;
        pBuffer->iSize = 0;
        pBuffer->bExternalData = AeFalse;
    }

    pBuffer->iWritePos = pBuffer->iSize;
    pBuffer->iReadPos = 0;
    pBuffer->iChunkSize = iChunkSize;

    return pBuffer;
}

/******************************************************************************/
void AeBufferDestroy(AeBuffer *pBuffer)
{
    if (pBuffer->pData && !pBuffer->bExternalData)
        AeFree(pBuffer->pData);

    AeFree(pBuffer);
}

/******************************************************************************/
AeInt32 AeBufferWrite(AeBuffer *pBuffer, AeChar *pData, AeInt32 iSize)
{
    if (!AeBufferExpand(pBuffer, iSize))
        return 0;

    memcpy(pBuffer->pData + pBuffer->iWritePos, pData, iSize);
    AeBufferSeekWritePos(pBuffer, iSize, AeTrue);

    /* put the terminator (zero character) at the end */
    *(pBuffer->pData + pBuffer->iWritePos) = 0;

    return iSize;
}

/******************************************************************************/
AeInt32 AeBufferWriteLn(AeBuffer *pBuffer, AeChar *pData, AeInt32 iSize)
{
    AeInt32 iResult;

    iResult = AeBufferWrite(pBuffer, pData, iSize);
    if (iResult)
        iResult += AeBufferWrite(pBuffer, "\r\n", 2);

    return iResult;
}

/******************************************************************************/
AeInt32 AeBufferRead(AeBuffer *pBuffer, AeChar **ppData, AeInt32 iSize)
{
	/* iSize is not used. Need to check if iPending is zero like AeBufferReadLn and set ppData to NULL */
    AeInt32 iPending, iResult;
    
    iPending = pBuffer->iWritePos - pBuffer->iReadPos;

    if (iSize == -1 || iSize > iPending)
        iResult = iPending;
    else
        iResult = iSize;

    *ppData = pBuffer->pData + pBuffer->iReadPos;
    AeBufferSeekReadPos(pBuffer, iResult, AeTrue);

    return iResult;
}

/******************************************************************************/
AeInt32 AeBufferReadLn(AeBuffer *pBuffer, AeChar **ppData, AeInt32 iSize)
{
    AeInt32 iPending, iResult;
    AeChar *pLfPtr;

    *ppData = NULL;
    iResult = 0;
    iPending = pBuffer->iWritePos - pBuffer->iReadPos;

    /* find LF */
    pLfPtr = memchr(pBuffer->pData + pBuffer->iReadPos, 0x0a, iPending);
    if (pLfPtr)
    {
        AeChar *pTmp = pLfPtr;

        if (*(pTmp - 1) == 0x0d)
            pTmp -= 1;

        *ppData = pBuffer->pData + pBuffer->iReadPos;
        iResult = pTmp - *ppData;
        AeBufferSeekReadPos(pBuffer, pLfPtr - *ppData + 1, AeTrue);
   }

    return iResult;
}

/******************************************************************************/
void AeBufferSeekWritePos(AeBuffer *pBuffer, AeInt32 iOffset, AeBool bIsRelative)
{
    if (bIsRelative)
        pBuffer->iWritePos += iOffset;
    else
        pBuffer->iWritePos = iOffset;

    if (pBuffer->iWritePos < 0)
        pBuffer->iWritePos = 0;

    if (pBuffer->iWritePos > pBuffer->iSize)
        pBuffer->iWritePos = pBuffer->iSize;
}

/******************************************************************************/
void AeBufferSeekReadPos(AeBuffer *pBuffer, AeInt32 iOffset, AeBool bIsRelative)
{
    if (bIsRelative)
        pBuffer->iReadPos += iOffset;
    else
        pBuffer->iReadPos = iOffset;

    if (pBuffer->iReadPos < 0)
        pBuffer->iReadPos = 0;

    if (pBuffer->iReadPos > pBuffer->iWritePos)
        pBuffer->iReadPos = pBuffer->iWritePos;

	/* seek read should not affect write position */
    if (pBuffer->iReadPos == pBuffer->iWritePos)
    {
        pBuffer->iReadPos = 0;
        pBuffer->iWritePos = 0;
    }
}

/******************************************************************************/
void AeBufferGetReadArea(AeBuffer *pBuffer, AeChar **ppData, AeInt32 *piSize)
{
    *ppData = pBuffer->pData + pBuffer->iReadPos;
    *piSize = pBuffer->iWritePos - pBuffer->iReadPos;
}

/******************************************************************************/
void AeBufferGetWriteArea(AeBuffer *pBuffer, AeChar **ppData, AeInt32 *piSize)
{
    *ppData = pBuffer->pData + pBuffer->iWritePos;
    *piSize = pBuffer->iSize - pBuffer->iWritePos;
}

/******************************************************************************/
AeBool AeBufferExpand(AeBuffer *pBuffer, AeInt32 iSize)
{
    AeInt32 iReqSize;
    
    iReqSize = pBuffer->iWritePos + iSize + 1;
    if (iReqSize > pBuffer->iSize)
    {
        AeUInt iReqChunks;
        AeChar *pChunk;

        iReqChunks = iReqSize / pBuffer->iChunkSize;
        if (iReqSize % pBuffer->iChunkSize)
            iReqChunks += 1;

        pBuffer->iSize = iReqChunks * pBuffer->iChunkSize;
        pChunk = (AeChar *) AeAlloc(pBuffer->iSize);
        if (!pChunk)
            return AeFalse;

        if (pBuffer->pData)
        {
            memcpy(pChunk, pBuffer->pData, pBuffer->iWritePos);
            if (pBuffer->bExternalData)
                pBuffer->bExternalData = AeFalse;
            else
                AeFree(pBuffer->pData);
        }

        pBuffer->pData = pChunk;
    }

    return AeTrue;
}

/******************************************************************************/
void AeBufferReset(AeBuffer *pBuffer)
{
    pBuffer->iWritePos = 0;
    pBuffer->iReadPos = 0;
}

/* $Id: AeWebUserAgent.c,v 1.15.2.1 2010/08/13 16:01:39 dkhodos Exp $ */

#include "Ae.h"

static void AeWebUserAgentUnbindConnections(AeWebUserAgent *pUA, AeWebTask *pTask);
static void AeWebConnectionRelease(void *pObject);
static void AeWebTransactionRelease(void *pObject);

/******************************************************************************/
AeWebUserAgent *AeWebUserAgentNew(void)
{
    AeWebUserAgent *pUA;

    pUA = (AeWebUserAgent *) AeAlloc(sizeof(AeWebUserAgent));
    if (!pUA)
        return NULL;
    memset(pUA, 0, sizeof(AeWebUserAgent));

    pUA->pConnections = AeListNew((AeCompareFunc) AePointerCompare,
        (AeReleaseFunc) AeWebConnectionRelease);
    if (!pUA->pConnections)
        return NULL;

    pUA->pTransactions = AeListNew((AeCompareFunc) AePointerCompare,
        (AeReleaseFunc) AeWebTransactionRelease);
    if (!pUA->pTransactions)
        return NULL;

    return pUA;
}

/******************************************************************************/
void AeWebUserAgentDestroy(AeWebUserAgent *pUA)
{
    if (pUA->pConnections)
        AeListDestroy(pUA->pConnections);
    if (pUA->pTransactions)
        AeListDestroy(pUA->pTransactions);

    AeFree(pUA);
}

/******************************************************************************/
AeError AeWebUserAgentSyncExecute(AeWebUserAgent *pUA, AeWebRequest **ppRequests, AeInt iCount)
{
    AeError rc;

    rc = AeWebUserAgentAsyncExecute(pUA, ppRequests, iCount);
    if (rc == AeEOK)
        rc = AeWebUserAgentDoEvents(pUA, NULL);

    return rc;
}


/******************************************************************************/
AeError AeWebUserAgentAsyncExecute(AeWebUserAgent *pUA, AeWebRequest **ppRequests, AeInt iCount)
{
    AeWebTransaction *pTran;
    AeError rc;
    AeInt i;

    rc = AeEOK;
    for (i = 0; i < iCount; i++)
    {
        AeWebRequestClearStatus(ppRequests[i]);

        pTran = AeWebTransactionNew(pUA, ppRequests[i]);
        if (!pTran)
            return AeEMemory;

        rc = AeWebTransactionInitiate(pTran);
        if (rc != AeEOK)
        {
            ppRequests[i]->iError = rc;
            if (ppRequests[i]->pOnError)
                (*(ppRequests[i]->pOnError))(ppRequests[i], rc);

            if (pTran->pConnection)
            {
                AeListRemove(pUA->pConnections, pTran->pConnection);
                AeWebConnectionDestroyV(pTran->pConnection);
            }

            AeWebTransactionDestroy(pTran);

            continue;
        }

        AeListAppend(pUA->pTransactions, pTran);
    }

    return AeEOK;
}


/******************************************************************************/
void AeWebUserAgentCancel(AeWebUserAgent *pUA, AeWebRequest *pRequest)
{
    AeWebTransaction *pTran;
    AeListItem *pItem;

    pItem = AeListFirst(pUA->pTransactions);
    while (pItem)
    {
        pTran = AeListData(pItem, AeWebTransaction);
        pItem = pItem->pNext;

        if (AeWebTransactionIsEqual(pTran, pRequest))
        {
            if (pTran->pConnection)
            {
                AeListRemove(pUA->pConnections, pTran->pConnection);
                AeWebConnectionDestroyV(pTran->pConnection);
            }

            AeListRemove(pUA->pTransactions, pTran);
            AeWebTransactionDestroy(pTran);

            return;
        }
    }
}

/******************************************************************************/
AeError AeWebUserAgentDoEvents(AeWebUserAgent *pUA, AeTimeValue *pTimeOut)
{
    AeFDArray readSet, writeSet, exceptSet;
    AeTimeValue tvCurrent, tvDelta, tvTmp, tvInfinity, tvZero;
    AeWebConnection *pConn;
    AeListItem *pItem;
    AeSocketFD iFd, iMaxFd;
    AeInt iSelectRc, iSelectCount, iPendingCount;
    AeError rc;

    rc = AeEOK;
    tvInfinity.iSec = 0x7fffffffL;
    tvInfinity.iMicroSec = 0;
    tvZero.iSec = 0;
    tvZero.iMicroSec = 0;

    if (AeListCount(pUA->pConnections) == 0)
        return AeEOK;

    do
    {
        AeGetElapsedTime(&tvCurrent);

        if (!pTimeOut)
            tvDelta = tvInfinity;
        else
            tvDelta = *pTimeOut;

        AeFDZero(&readSet);
        AeFDZero(&writeSet);
        AeFDZero(&exceptSet);

        pItem = AeListFirst(pUA->pConnections);
        iMaxFd = 0;
        iSelectCount = 0;
        iPendingCount = 0;
        while (pItem)
        {
            pConn = AeListData(pItem, AeWebConnection);
            pItem = AeListNext(pItem);

			/* skip closed connections */
            if (AeWebConnectionIsClosed(pConn))
				continue;

            iFd = AeSocketGetFD(&pConn->sock);

            if (AeWebConnectionIsConnecting(pConn))
            {
                AeFDSet(iFd, &writeSet);
                AeFDSet(iFd, &exceptSet);
            }
            else if (AeWebConnectionIsConnected(pConn))
            {
                if (AeWebConnectionIsReceiving(pConn))
                    AeFDSet(iFd, &readSet);

                if (AeWebConnectionIsSending(pConn))
                    AeFDSet(iFd, &writeSet);
            }
            
            if (AeFDIsSet(iFd, &readSet) || AeFDIsSet(iFd, &writeSet) || AeFDIsSet(iFd, &exceptSet))
            {
                if (iFd > iMaxFd)
                    iMaxFd = iFd;

                if (AeFDIsSet(iFd, &readSet) && AeWebConnectionIsRxPending(pConn))
                    iPendingCount++;
                else
                    iSelectCount++;

                if (!pTimeOut)
                {
                    AE_TIME_VALUE_SUBTRACT(tvTmp, tvCurrent, pConn->lastActivityTime);
                    AE_TIME_VALUE_SUBTRACT(tvTmp, pConn->timeOut, tvTmp);

                    if (AE_TIME_VALUE_LESS_THAN(tvTmp, tvDelta))
                        tvDelta = tvTmp;
                }
            }
        }

        if (iSelectCount > 0)
        {
            if (iPendingCount > 0)
                tvDelta = tvZero;

            iSelectRc = AeSelect(iMaxFd + 1, &readSet, &writeSet, &exceptSet, &tvDelta);
            if (iSelectRc < 0)
                return AeENetGeneral;
        }

        AeGetElapsedTime(&tvCurrent);

        pItem = AeListFirst(pUA->pConnections);
        while (pItem)
        {
            pConn = AeListData(pItem, AeWebConnection);
            pItem = AeListNext(pItem);

			rc = AeEOK;
            iFd = AeSocketGetFD(&pConn->sock);

            if (AeWebConnectionIsConnecting(pConn))
            {
                if (AeFDIsSet(iFd, &writeSet) || AeFDIsSet(iFd, &exceptSet))
                    rc = AeWebConnectionOnWritableV(pConn, &tvCurrent);
            }
            else
            {
                if (!AeWebConnectionIsClosed(pConn) && (AeFDIsSet(iFd, &readSet) || AeWebConnectionIsRxPending(pConn)))
                    rc = AeWebConnectionOnReadableV(pConn, &tvCurrent);

                if (!AeWebConnectionIsClosed(pConn) && AeFDIsSet(iFd, &writeSet) && rc == AeEOK)
                    rc = AeWebConnectionOnWritableV(pConn, &tvCurrent);
            }

            if (!AeWebConnectionIsClosed(pConn))
            {
                AE_TIME_VALUE_SUBTRACT(tvTmp, tvCurrent, pConn->lastActivityTime);
                if (!AE_TIME_VALUE_GREATER_THAN(pConn->timeOut, tvTmp))
                {
                    AeWebConnectionDisconnectV(pConn);
                    rc = AeENetTimeout;
                }
            }

			if (pConn->pTask)
			{
				if (rc != AeEOK)
					AeWebTaskOnIOErrorV(pConn->pTask, rc);

				if (AeWebTaskIsDoneV(pConn->pTask))
				{
					AeListRemove(pUA->pTransactions, pConn->pTask);
					AeWebTaskDestroyV(pConn->pTask);
                    AeWebUserAgentUnbindConnections(pUA, pConn->pTask);
				}
			}

            if (AeWebConnectionIsClosed(pConn))
            {
                AeListRemove(pUA->pConnections, pConn);
                AeWebConnectionDestroyV(pConn);
            }
        }

        if (pTimeOut)
            break;

    } while (AeListCount(pUA->pTransactions) != 0);

    return AeEOK;
}

/******************************************************************************/
AeBool AeWebUserAgentEventsPending(AeWebUserAgent *pUA)
{
    AeWebConnection *pConn;
    AeListItem *pItem;

    pItem = AeListFirst(pUA->pConnections);
    while (pItem)
    {
        pConn = AeListData(pItem, AeWebConnection);
        pItem = AeListNext(pItem);

        if (pConn->pTask && !AeWebTaskIsDoneV(pConn->pTask))
            return AeTrue;
    }

    return AeFalse;
}

/******************************************************************************/
AeWebConnection *AeWebUserAgentGetIdleConnection(AeWebUserAgent *pUA, AeWebTask *pTask, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort, AeTimeValue *pTimeOut)
{
    AeWebConnection *pConn;
    AeListItem *pItem;

    pItem = AeListFirst(pUA->pConnections);
    while (pItem)
    {
        pConn = AeListData(pItem, AeWebConnection);
        pItem = AeListNext(pItem);

        if (AeWebConnectionIsClosed(pConn))
            continue;

        if (pConn->pTask)
            continue;

        if (AeWebConnectionIsEqualV(pConn, pHost, iPort, pOriginHost, iOriginPort))
        {
            pConn->pTask = pTask;
            return pConn;
        }
    }

    return AeWebUserAgentGetNewConnection(pUA, pTask, pHost, iPort, pOriginHost, iOriginPort, pTimeOut);
}

/******************************************************************************/
AeWebConnection *AeWebUserAgentGetNewConnection(AeWebUserAgent *pUA, AeWebTask *pTask, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort, AeTimeValue *pTimeOut)
{
    AeWebConnection *pConn;

    if (g_pWebProxySOCKS && pHost && iPort)
        pConn = (AeWebConnection *) AeWebConnectionSOCKSNew();
    else
        pConn = AeWebConnectionNew();

    if (!pConn)
        return NULL;

    AeWebConnectionInitializeV(pConn,
                               pHost ? pHost : pOriginHost,
                               (AeUInt16) (iPort ? iPort : iOriginPort),
                               pOriginHost, iOriginPort, pTimeOut);

    AeListAppend(pUA->pConnections, pConn);

    pConn->pTask = pTask;

    return pConn;
}

/******************************************************************************/
AeWebConnection *AeWebUserAgentGetNewDirectConnection(AeWebUserAgent *pUA, AeWebTask *pTask, AeChar *pHost, AeUInt16 iPort, AeTimeValue *pTimeOut)
{
    return AeWebUserAgentGetNewConnection(pUA, pTask, NULL, 0, pHost, iPort, pTimeOut);
}

/******************************************************************************/
static void AeWebUserAgentUnbindConnections(AeWebUserAgent *pUA, AeWebTask *pTask)
{
    AeWebConnection *pConn;
    AeListItem *pItem;

    pItem = AeListFirst(pUA->pConnections);
    while (pItem)
    {
        pConn = AeListData(pItem, AeWebConnection);
        pItem = AeListNext(pItem);

        if (pConn->pTask == pTask)
            pConn->pTask = NULL;
    }
}

/******************************************************************************/
static void AeWebConnectionRelease(void *pObject)
{
    AeWebConnectionDestroyV((AeWebConnection *) pObject);
}

/******************************************************************************/
static void AeWebTransactionRelease(void *pObject)
{
    AeWebTransactionDestroy((AeWebTransaction *) pObject);
}

/* $Id: AeWebConnectionSOCKS.c,v 1.5 2008/05/21 18:24:35 dkhodos Exp $ */

#include "Ae.h"

static void     AeWebConnectionSOCKSDestroy(AeWebConnection *pConn);
static void     AeWebConnectionSOCKSInitialize(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort, AeTimeValue *pTimeOut);
static AeError  AeWebConnectionSOCKSConnect(AeWebConnection *pConn);
static AeError  AeWebConnectionSOCKSDisconnect(AeWebConnection *pConn);
static AeError  AeWebConnectionSOCKSSend(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll);
static AeError  AeWebConnectionSOCKSReceive(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll);
static AeBool   AeWebConnectionSOCKSIsEqual(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort);
static AeBool   AeWebConnectionSOCKSIsConnected(AeWebConnection *pConn);
static AeError  AeWebConnectionSOCKSOnReadable(AeWebConnection *pConn, AeTimeValue *pTimeValue);
static AeError  AeWebConnectionSOCKSOnWritable(AeWebConnection *pConn, AeTimeValue *pTimeValue);

static AeError  AeWebConnectionSOCKSMakeRequest(AeWebConnectionSOCKS *pSOCKS, SOCKSQuery *pQuery);
static AeError  AeWebConnectionSOCKSGetError(AeUInt8 iVersion, AeUInt8 iReply);

static SOCKSQuery   *SOCKSQueryNew(AeWebConnectionSOCKS *pParent, SOCKSQueryType iType);
static void         SOCKSQueryDestroy(SOCKSQuery *pQuery);
static AeError      SOCKSQueryBuildRequest(SOCKSQuery *pQuery);
static AeError      SOCKSQueryParseReply(SOCKSQuery *pQuery);
static AeError      SOCKSQuerySendRequest(SOCKSQuery *pQuery);
static AeError      SOCKSQueryReceiveReply(SOCKSQuery *pQuery);

static AeError  SOCKSv5SelectionPrepareRequest(SOCKSQuery *pQuery);
static AeError  SOCKSv5SelectionProcessReply(SOCKSQuery *pQuery);
static AeError  SOCKSv5SelectionMarshalRequest(SOCKSQuery *pQuery);
static AeError  SOCKSv5SelectionDemarshalReply(SOCKSQuery *pQuery);
static AeInt32  SOCKSv5SelectionGetRequestSize(SOCKSQuery *pQuery);
static AeInt32  SOCKSv5SelectionGetReplySize(SOCKSQuery *pQuery);

static AeError  SOCKSCommandPrepareRequest(SOCKSQuery *pQuery);
static AeError  SOCKSCommandProcessReply(SOCKSQuery *pQuery);
static AeError  SOCKSCommandMarshalRequest(SOCKSQuery *pQuery);
static AeError  SOCKSCommandDemarshalReply(SOCKSQuery *pQuery);
static AeInt32  SOCKSCommandGetRequestSize(SOCKSQuery *pQuery);
static AeInt32  SOCKSCommandGetReplySize(SOCKSQuery *pQuery);

static AeError  SOCKSv5AuthPlainPrepareRequest(SOCKSQuery *pQuery);
static AeError  SOCKSv5AuthPlainProcessReply(SOCKSQuery *pQuery);
static AeError  SOCKSv5AuthPlainMarshalRequest(SOCKSQuery *pQuery);
static AeError  SOCKSv5AuthPlainDemarshalReply(SOCKSQuery *pQuery);
static AeInt32  SOCKSv5AuthPlainGetRequestSize(SOCKSQuery *pQuery);
static AeInt32  SOCKSv5AuthPlainGetReplySize(SOCKSQuery *pQuery);

static AeWebConnectionVTable g_abWebConnectionSOCKSVTable =
{
    AeWebConnectionSOCKSDestroy,
    AeWebConnectionSOCKSInitialize,
    AeWebConnectionSOCKSConnect,
    AeWebConnectionSOCKSDisconnect,
    AeWebConnectionSOCKSSend,
    AeWebConnectionSOCKSReceive,
    AeWebConnectionSOCKSIsEqual,
    AeWebConnectionSOCKSIsConnected,
    AeWebConnectionSOCKSOnReadable,
    AeWebConnectionSOCKSOnWritable
};

static SOCKSQueryVTable g_socksv5SelectionVTable =
{
    SOCKSv5SelectionPrepareRequest,
    SOCKSv5SelectionProcessReply,
    SOCKSv5SelectionMarshalRequest,
    SOCKSv5SelectionDemarshalReply,
    SOCKSv5SelectionGetRequestSize,
    SOCKSv5SelectionGetReplySize
};

static SOCKSQueryVTable g_socksCommandVTable =
{
    SOCKSCommandPrepareRequest,
    SOCKSCommandProcessReply,
    SOCKSCommandMarshalRequest,
    SOCKSCommandDemarshalReply,
    SOCKSCommandGetRequestSize,
    SOCKSCommandGetReplySize
};

static SOCKSQueryVTable g_socksv5AuthPlainVTable =
{
    SOCKSv5AuthPlainPrepareRequest,
    SOCKSv5AuthPlainProcessReply,
    SOCKSv5AuthPlainMarshalRequest,
    SOCKSv5AuthPlainDemarshalReply,
    SOCKSv5AuthPlainGetRequestSize,
    SOCKSv5AuthPlainGetReplySize
};

static AeUInt8 g_pSOCKSv5AuthMethods[] =
{
    SOCKSV5_AUTH_NONE,
    SOCKSV5_AUTH_PLAIN
};

static AeInt g_iSOCKSv5AuthMethods = sizeof(g_pSOCKSv5AuthMethods) /
    sizeof(g_pSOCKSv5AuthMethods[0]); 

/******************************************************************************/
AeWebConnectionSOCKS *AeWebConnectionSOCKSNew(void)
{
    AeWebConnectionSOCKS *pSOCKS;

    pSOCKS = (AeWebConnectionSOCKS *) AeAlloc(sizeof(AeWebConnectionSOCKS));
    if (!pSOCKS)
        return NULL;
    memset(pSOCKS, 0, sizeof(AeWebConnectionSOCKS));

    AeWebConnectionConstruct(&pSOCKS->parent);

    pSOCKS->parent.pVTable = &g_abWebConnectionSOCKSVTable;
    pSOCKS->iState = AeWebConnectionSOCKSClosed;
    pSOCKS->iVersion = SOCKSV5_VER;
    pSOCKS->pHost = NULL;
    pSOCKS->netAddress.iAddress = 0UL;
    pSOCKS->netAddress.iPort = 0;
    pSOCKS->pQuery = NULL;

    return pSOCKS;
}

/******************************************************************************/
static void AeWebConnectionSOCKSDestroy(AeWebConnection *pConn)
{
    AeWebConnectionSOCKS *pSOCKS;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    AeWebConnectionFree(&pSOCKS->parent);

    if (pSOCKS->pHost)
        AeFree(pSOCKS->pHost);
    if (pSOCKS->pQuery)
        SOCKSQueryDestroy(pSOCKS->pQuery);

    AeFree(pSOCKS);
}

/******************************************************************************/
static void AeWebConnectionSOCKSInitialize(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort, AeTimeValue *pTimeOut)
{
    AeWebConnectionSOCKS *pSOCKS;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    if (!g_pWebProxySOCKS)
       return;

    AeWebConnectionInitialize(pConn, g_pWebProxySOCKS->pHost, g_pWebProxySOCKS->iPort, pOriginHost, iOriginPort, pTimeOut);

    AeSetString(&pSOCKS->pHost, pHost, -1);
    pSOCKS->netAddress.iPort = AeNetHToNS(iPort);
}

/******************************************************************************/
static AeError AeWebConnectionSOCKSConnect(AeWebConnection *pConn)
{
    AeWebConnectionSOCKS *pSOCKS;
    AeError rc;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    rc = AeWebConnectionConnect(pConn);
    if (rc != AeEOK)
        return rc;

    pSOCKS->iState = AeWebConnectionSOCKSConnecting;

    return AeEOK;
}

/******************************************************************************/
static AeError AeWebConnectionSOCKSDisconnect(AeWebConnection *pConn)
{
    AeWebConnectionSOCKS *pSOCKS;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    AeWebConnectionDisconnect(pConn);

    pSOCKS->iState = AeWebConnectionSOCKSClosed;

    return AeEOK;
}

/******************************************************************************/
static AeError AeWebConnectionSOCKSSend(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll)
{
    AeWebConnectionSOCKS *pSOCKS;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    if (pSOCKS->iState != AeWebConnectionSOCKSEstablished)
        return AeEInternal;

    return AeWebConnectionSend(pConn, pData, iSize, bWaitAll);
}

/******************************************************************************/
static AeError AeWebConnectionSOCKSReceive(AeWebConnection *pConn, AeChar *pData, AeInt32 iSize, AeBool bWaitAll)
{
    AeWebConnectionSOCKS *pSOCKS;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    if (pSOCKS->iState != AeWebConnectionSOCKSEstablished)
        return AeEInternal;

    return AeWebConnectionReceive(pConn, pData, iSize, bWaitAll);
}

/******************************************************************************/
static AeBool AeWebConnectionSOCKSIsEqual(AeWebConnection *pConn, AeChar *pHost, AeUInt16 iPort, AeChar *pOriginHost, AeUInt16 iOriginPort)
{
    AeWebConnectionSOCKS *pSOCKS;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    return (!strcmp(pSOCKS->pHost, pHost) &&
        pSOCKS->netAddress.iPort == AeNetHToNS(iPort) &&
        !strcmp(pConn->pOriginHost, pOriginHost) &&
        pConn->iOriginPort == iOriginPort);
}

/******************************************************************************/
static AeBool AeWebConnectionSOCKSIsConnected(AeWebConnection *pConn)
{
    AeWebConnectionSOCKS *pSOCKS;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    return pSOCKS->iState == AeWebConnectionSOCKSEstablished;
}

/******************************************************************************/
static AeError AeWebConnectionSOCKSOnReadable(AeWebConnection *pConn, AeTimeValue *pTimeValue)
{
    AeWebConnectionSOCKS *pSOCKS;
	SOCKSQuery *pQuery;
    AeError rc;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    pQuery = NULL;

    rc = AeWebConnectionOnReadable(pConn, pTimeValue);
    if (rc != AeEOK)
        return rc;

    if (!AeWebConnectionIsConnected(pConn))
        return AeEOK;

    if (pSOCKS->iState == AeWebConnectionSOCKSHandshaking &&
        !AeWebConnectionIsReceiving(pConn))
    {
        rc = SOCKSQueryParseReply(pSOCKS->pQuery);
        if (rc == AeEOK)
        {
            if (pSOCKS->pQuery->iQueryType == SOCKSv5Selection)
            {
                if (pSOCKS->iVersion == SOCKSV4_VER)
                {
                    rc = AeWebConnectionDisconnectV(pConn);
                    if (rc == AeEOK)
                        rc = AeWebConnectionConnectV(pConn);
                }
                else
                {
                    if (pSOCKS->pQuery->reply.selRep.iMethod == SOCKSV5_AUTH_PLAIN)
                        pQuery = SOCKSQueryNew(pSOCKS, SOCKSv5AuthPlain);
                    else
                        pQuery = SOCKSQueryNew(pSOCKS, SOCKSCommand);
                    if (!pQuery)
                        rc = AeEMemory;
                }
            }
            else if (pSOCKS->pQuery->iQueryType == SOCKSv5AuthPlain)
            {
                pQuery = SOCKSQueryNew(pSOCKS, SOCKSCommand);
                if (!pQuery)
                    rc = AeEMemory;
            }
            else if (pSOCKS->pQuery->iQueryType == SOCKSCommand)
            {
                pSOCKS->iState = AeWebConnectionSOCKSEstablished;

                if (pSOCKS->parent.pTask)
                    AeWebTaskOnConnectedV(pSOCKS->parent.pTask, AeFalse);
            }
        }

        SOCKSQueryDestroy(pSOCKS->pQuery);
        pSOCKS->pQuery = NULL;

        if (pQuery)
            rc = AeWebConnectionSOCKSMakeRequest(pSOCKS, pQuery);
    }

    return rc;
}

/******************************************************************************/
static AeError AeWebConnectionSOCKSOnWritable(AeWebConnection *pConn, AeTimeValue *pTimeValue)
{
    AeWebConnectionSOCKS *pSOCKS;
	SOCKSQuery *pQuery;
    AeError rc;

    pSOCKS = (AeWebConnectionSOCKS *) pConn;

    rc = AeWebConnectionOnWritable(pConn, pTimeValue);
    if (rc != AeEOK)
        return rc;

    if (pSOCKS->iState == AeWebConnectionSOCKSConnecting &&
        AeWebConnectionIsConnected(pConn))
    {
        pSOCKS->iState = AeWebConnectionSOCKSHandshaking;
        
        if (pSOCKS->iVersion == SOCKSV5_VER)
		    pQuery = SOCKSQueryNew(pSOCKS, SOCKSv5Selection);
        else
            pQuery = SOCKSQueryNew(pSOCKS, SOCKSCommand);
        if (pQuery)
            rc = AeWebConnectionSOCKSMakeRequest(pSOCKS, pQuery);
        else
            rc = AeEMemory;
    }
    else if (pSOCKS->iState == AeWebConnectionSOCKSHandshaking &&
        !AeWebConnectionIsSending(pConn))
        rc = SOCKSQueryReceiveReply(pSOCKS->pQuery);

    return AeEOK;
}

/******************************************************************************/
static AeError AeWebConnectionSOCKSMakeRequest(AeWebConnectionSOCKS *pSOCKS, SOCKSQuery *pQuery)
{
    AeError rc;

	rc = SOCKSQueryBuildRequest(pQuery);
    if (rc != AeEOK)
        return rc;

    rc = SOCKSQuerySendRequest(pQuery);
    if (rc != AeEOK)
        return rc;

    pSOCKS->pQuery = pQuery;

    return AeEOK;
}

/******************************************************************************/
static AeError AeWebConnectionSOCKSGetError(AeUInt8 iVersion, AeUInt8 iReply)
{
    /* SOCKSv4 specifies 0 as version number in reply */
    if (iVersion == 0)
    {
        switch (iReply)
        {
            case SOCKSV4_CMD_GRANTED:
                return AeEOK;
            case SOCKSV4_CMD_FAILED:
                return AeESOCKSGeneral;
            case SOCKSV4_CMD_REJ_IDENTD:
            case SOCKSV4_CMD_REJ_BADID:
                return AeESOCKSAuthFailed;
            default:
                return AeESOCKSGeneral;
        }
    }
    else if (iVersion == SOCKSV5_VER)
    {
        switch (iReply)
        {
            case 0:
                return AeEOK;
            case 1:
                return AeESOCKSGeneral;
            case 2:
                return AeESOCKSPerm;
            case 3:
                return AeESOCKSNetUnreach;
            case 4:
                return AeESOCKSHostUnreach;
            case 5:
                return AeESOCKSConnRefused;
            case 6:
                return AeESOCKSTTL;
            case 7:
                return AeESOCKSBadCommand;
            case 8:
                return AeESOCKSBadAddress;
            default:
                return AeESOCKSGeneral;
        }
    }
    else
        return AeESOCKSWrongVersion;
}

/******************************************************************************/
static SOCKSQuery *SOCKSQueryNew(AeWebConnectionSOCKS *pParent, SOCKSQueryType iType)
{
    SOCKSQuery *pQuery;

    pQuery = (SOCKSQuery *) AeAlloc(sizeof(SOCKSQuery));
    if (!pQuery)
        return NULL;
    memset(pQuery, 0, sizeof(SOCKSQuery));

    switch (iType)
    {
        case SOCKSv5Selection:
            pQuery->pVTable = &g_socksv5SelectionVTable;
            break;
        case SOCKSCommand:
            pQuery->pVTable = &g_socksCommandVTable;
            break;
        case SOCKSv5AuthPlain:
            pQuery->pVTable = &g_socksv5AuthPlainVTable;
            break;
    }

    pQuery->pParent = pParent;
    pQuery->iQueryType = iType;
    pQuery->pIOBuffer = NULL;

    return pQuery;
}

/******************************************************************************/
static void SOCKSQueryDestroy(SOCKSQuery *pQuery)
{
    if (pQuery->pIOBuffer)
        AeFree(pQuery->pIOBuffer);

    AeFree(pQuery);
}

/******************************************************************************/
static AeError SOCKSQueryBuildRequest(SOCKSQuery *pQuery)
{
    AeError rc;

    rc = SOCKSQueryPrepareRequestV(pQuery);
    if (rc == AeEOK)
    {
        if (pQuery->pIOBuffer)
            AeFree(pQuery->pIOBuffer);

        pQuery->pIOBuffer = AeAlloc(SOCKSQueryGetRequestSizeV(pQuery));
        if (!pQuery->pIOBuffer)
            return AeEMemory;

        rc = SOCKSQueryMarshalRequestV(pQuery);
    }

    return rc;
}

/******************************************************************************/
static AeError SOCKSQueryParseReply(SOCKSQuery *pQuery)
{
    AeError rc;

    rc = SOCKSQueryDemarshalReplyV(pQuery);
    if (rc == AeEOK)
    {
        AeFree(pQuery->pIOBuffer);
        pQuery->pIOBuffer = NULL;

        rc = SOCKSQueryProcessReplyV(pQuery);
    }

    return rc;
}

/******************************************************************************/
static AeError SOCKSQuerySendRequest(SOCKSQuery *pQuery)
{
    return AeWebConnectionSend(&pQuery->pParent->parent, pQuery->pIOBuffer,
        SOCKSQueryGetRequestSizeV(pQuery), AeTrue);
}

/******************************************************************************/
static AeError SOCKSQueryReceiveReply(SOCKSQuery *pQuery)
{
    AeError rc;

    if (pQuery->pIOBuffer)
        AeFree(pQuery->pIOBuffer);

    pQuery->pIOBuffer = AeAlloc(SOCKSQueryGetReplySizeV(pQuery));
    if (!pQuery->pIOBuffer)
        return AeEMemory;

    rc = AeWebConnectionReceive(&pQuery->pParent->parent, pQuery->pIOBuffer,
        SOCKSQueryGetReplySizeV(pQuery), AeTrue);

    return rc;
}

/******************************************************************************/
static AeError SOCKSv5SelectionPrepareRequest(SOCKSQuery *pQuery)
{
    AeInt i;

    pQuery->request.selReq.iVer = pQuery->pParent->iVersion;
    if (g_pWebProxySOCKS->pUser)
    {
        pQuery->request.selReq.iMethods = g_iSOCKSv5AuthMethods;
        for (i = 0; i < g_iSOCKSv5AuthMethods; i++)
            pQuery->request.selReq.pMethods[i] = g_pSOCKSv5AuthMethods[i];
    }
    else
    {
        pQuery->request.selReq.iMethods = 1;
        pQuery->request.selReq.pMethods[0] = SOCKSV5_AUTH_NONE;
    }

    return AeEOK;
}

/******************************************************************************/
static AeError SOCKSv5SelectionProcessReply(SOCKSQuery *pQuery)
{
    AeInt i;
    AeError rc;

    rc = AeEOK;

    if (pQuery->reply.selRep.iVer == SOCKSV5_VER)
    {
        for (i = 0; i < g_iSOCKSv5AuthMethods; i++)
            if (pQuery->reply.selRep.iMethod == g_pSOCKSv5AuthMethods[i])
                break;
        if (i == g_iSOCKSv5AuthMethods)
            return AeESOCKSGeneral;

        pQuery->pParent->iVersion = SOCKSV5_VER;
    }

    /* we're possibly talking to SOCKSv4 server. it is assumed to return some error */
    /* in reply to method selection request. fallback to v4 if this is the case */
    else if (pQuery->reply.selRep.iVer == 0)
    {
        if (pQuery->reply.selRep.iMethod == SOCKSV4_CMD_FAILED ||
            pQuery->reply.selRep.iMethod == SOCKSV4_CMD_REJ_IDENTD ||
            pQuery->reply.selRep.iMethod == SOCKSV4_CMD_REJ_BADID)
        {
            pQuery->pParent->iVersion = SOCKSV4_VER;
            pQuery->reply.selRep.iMethod = SOCKSV5_AUTH_NONE;
            rc = AeEOK;
        }
        else
            rc = AeESOCKSWrongVersion;
    }
    else
        rc = AeESOCKSWrongVersion;

    return rc;
}

/******************************************************************************/
static AeError SOCKSv5SelectionMarshalRequest(SOCKSQuery *pQuery)
{
    AeChar *ptr;
    AeInt i;

    ptr = pQuery->pIOBuffer;

    memcpy(ptr, &pQuery->request.selReq.iVer, sizeof(pQuery->request.selReq.iVer));
    ptr += sizeof(pQuery->request.selReq.iVer);

    memcpy(ptr, &pQuery->request.selReq.iMethods, sizeof(pQuery->request.selReq.iMethods));
    ptr += sizeof(pQuery->request.selReq.iMethods);

    for (i = 0; i < pQuery->request.selReq.iMethods; i++)
    {
        memcpy(ptr, &pQuery->request.selReq.pMethods[i],
            sizeof(pQuery->request.selReq.pMethods[0]));
        ptr += sizeof(pQuery->request.selReq.pMethods[0]);
    }

    return AeEOK;
}

/******************************************************************************/
static AeError SOCKSv5SelectionDemarshalReply(SOCKSQuery *pQuery)
{
    AeChar *ptr;

    ptr = pQuery->pIOBuffer;

    memcpy(&pQuery->reply.selRep.iVer, ptr, sizeof(pQuery->reply.selRep.iVer));
    ptr += sizeof(pQuery->reply.selRep.iVer);

    memcpy(&pQuery->reply.selRep.iMethod, ptr, sizeof(pQuery->reply.selRep.iMethod));
    ptr += sizeof(pQuery->reply.selRep.iMethod);

    return AeEOK;
}

/******************************************************************************/
static AeInt32 SOCKSv5SelectionGetRequestSize(SOCKSQuery *pQuery)
{
    return (sizeof(pQuery->request.selReq.iVer) +
        sizeof(pQuery->request.selReq.iMethods) +
        pQuery->request.selReq.iMethods);
}

/******************************************************************************/
static AeInt32 SOCKSv5SelectionGetReplySize(SOCKSQuery *pQuery)
{
    return (sizeof(pQuery->reply.selRep.iVer) +
        sizeof(pQuery->reply.selRep.iMethod));
}

/******************************************************************************/
static AeError SOCKSCommandPrepareRequest(SOCKSQuery *pQuery)
{
    AeError rc;

    if (strspn(pQuery->pParent->pHost, ".0123456789") == strlen(pQuery->pParent->pHost))
        pQuery->pParent->netAddress.iAddress = AeNetInetAddr(pQuery->pParent->pHost);
    else
    {
        if (!AeWebGetCachedIPAddress(pQuery->pParent->pHost,
            &pQuery->pParent->netAddress.iAddress))
        {
            /* resolve origin hostname */
            rc = AeNetResolve(pQuery->pParent->pHost,
                &pQuery->pParent->netAddress.iAddress);
            if (rc != AeEOK)
                return AeENetUnknownHost;

            /* remember resolved address */
            AeWebSetCachedIPAddress(pQuery->pParent->pHost,
                pQuery->pParent->netAddress.iAddress);
        }
    }

    pQuery->request.cmdReq.iVer = pQuery->pParent->iVersion;
    pQuery->request.cmdReq.iCmd = SOCKS_CMD_CONNECT;
    pQuery->request.cmdReq.iATyp = SOCKSV5_ATYP_IPV4ADDR;
    pQuery->request.cmdReq.iDstAddr = pQuery->pParent->netAddress.iAddress;
    pQuery->request.cmdReq.iDstPort = pQuery->pParent->netAddress.iPort;

    if (pQuery->request.cmdReq.iVer == SOCKSV4_VER)
    {
        if (g_pWebProxySOCKS->pUser)
            pQuery->request.cmdReq.pUserId = g_pWebProxySOCKS->pUser;
        else
            pQuery->request.cmdReq.pUserId = "";
    }

    return AeEOK;
}

/******************************************************************************/
static AeError SOCKSCommandProcessReply(SOCKSQuery *pQuery)
{
    AeError rc;

    if ((pQuery->pParent->iVersion == SOCKSV5_VER &&
         pQuery->reply.cmdRep.iVer != SOCKSV5_VER) ||
        (pQuery->pParent->iVersion == SOCKSV4_VER &&
         pQuery->reply.cmdRep.iVer != 0))
        rc = AeESOCKSWrongVersion;
    else
        rc = AeWebConnectionSOCKSGetError(pQuery->reply.cmdRep.iVer,
            pQuery->reply.cmdRep.iRep);

    return rc;
}

/******************************************************************************/
static AeError SOCKSCommandMarshalRequest(SOCKSQuery *pQuery)
{
    AeChar *ptr;
    
    ptr = pQuery->pIOBuffer;

    memcpy(ptr, &pQuery->request.cmdReq.iVer, sizeof(pQuery->request.cmdReq.iVer));
    ptr += sizeof(pQuery->request.cmdReq.iVer);

    memcpy(ptr, &pQuery->request.cmdReq.iCmd, sizeof(pQuery->request.cmdReq.iCmd));
    ptr += sizeof(pQuery->request.cmdReq.iCmd);

    if (pQuery->request.cmdReq.iVer == SOCKSV4_VER)
    {
        memcpy(ptr, &pQuery->request.cmdReq.iDstPort,
            sizeof(pQuery->request.cmdReq.iDstPort));
        ptr += sizeof(pQuery->request.cmdReq.iDstPort);

        memcpy(ptr, &pQuery->request.cmdReq.iDstAddr,
            sizeof(pQuery->request.cmdReq.iDstAddr));
        ptr += sizeof(pQuery->request.cmdReq.iDstAddr);

        memcpy(ptr, pQuery->request.cmdReq.pUserId,
            strlen(pQuery->request.cmdReq.pUserId) + 1);
        ptr += strlen(pQuery->request.cmdReq.pUserId) + 1;
    }
    else
    {
        *ptr = '\0';
        ptr += 1;

        memcpy(ptr, &pQuery->request.cmdReq.iATyp,
            sizeof(pQuery->request.cmdReq.iATyp));
        ptr += sizeof(pQuery->request.cmdReq.iATyp);

        memcpy(ptr, &pQuery->request.cmdReq.iDstAddr,
            sizeof(pQuery->request.cmdReq.iDstAddr));
        ptr += sizeof(pQuery->request.cmdReq.iDstAddr);

        memcpy(ptr, &pQuery->request.cmdReq.iDstPort,
            sizeof(pQuery->request.cmdReq.iDstPort));
        ptr += sizeof(pQuery->request.cmdReq.iDstPort);
    }

    return AeEOK;
}

/******************************************************************************/
static AeError SOCKSCommandDemarshalReply(SOCKSQuery *pQuery)
{
    AeChar *ptr;
    
    ptr = pQuery->pIOBuffer;

    memcpy(&pQuery->reply.cmdRep.iVer, ptr, sizeof(pQuery->reply.cmdRep.iVer));
    ptr += sizeof(pQuery->reply.cmdRep.iVer);

    memcpy(&pQuery->reply.cmdRep.iRep, ptr, sizeof(pQuery->reply.cmdRep.iRep));
    ptr += sizeof(pQuery->reply.cmdRep.iRep);

    if (pQuery->request.cmdReq.iVer == SOCKSV4_VER)
    {
        memcpy(&pQuery->reply.cmdRep.iBndPort, ptr, sizeof(pQuery->reply.cmdRep.iBndPort));
        ptr += sizeof(pQuery->reply.cmdRep.iBndPort);

        memcpy(&pQuery->reply.cmdRep.iBndAddr, ptr, sizeof(pQuery->reply.cmdRep.iBndAddr));
        ptr += sizeof(pQuery->reply.cmdRep.iBndAddr);
    }
    else
    {
        ptr += 1;

        memcpy(&pQuery->reply.cmdRep.iATyp, ptr, sizeof(pQuery->reply.cmdRep.iATyp));
        ptr += sizeof(pQuery->reply.cmdRep.iATyp);

        if (pQuery->reply.cmdRep.iATyp != SOCKSV5_ATYP_IPV4ADDR)
            return AeESOCKSBadAddress;

        memcpy(&pQuery->reply.cmdRep.iBndAddr, ptr, sizeof(pQuery->reply.cmdRep.iBndAddr));
        ptr += sizeof(pQuery->reply.cmdRep.iBndAddr);

        memcpy(&pQuery->reply.cmdRep.iBndPort, ptr, sizeof(pQuery->reply.cmdRep.iBndPort));
        ptr += sizeof(pQuery->reply.cmdRep.iBndPort);
    }

    return AeEOK;
}

/******************************************************************************/
static AeInt32 SOCKSCommandGetRequestSize(SOCKSQuery *pQuery)
{
    if (pQuery->request.cmdReq.iVer == SOCKSV4_VER)
    {
        return (sizeof(pQuery->request.cmdReq.iVer) +
            sizeof(pQuery->request.cmdReq.iCmd) +
            sizeof(pQuery->request.cmdReq.iDstPort) +
            sizeof(pQuery->request.cmdReq.iDstAddr) +
            strlen(pQuery->request.cmdReq.pUserId) + 1);
    }
    else
    {
        return (sizeof(pQuery->request.cmdReq.iVer) +
            sizeof(pQuery->request.cmdReq.iCmd) +
            sizeof(pQuery->request.cmdReq.iATyp) +
            sizeof(pQuery->request.cmdReq.iDstAddr) +
            sizeof(pQuery->request.cmdReq.iDstPort) + 1);
    }
}

/******************************************************************************/
static AeInt32 SOCKSCommandGetReplySize(SOCKSQuery *pQuery)
{
    if (pQuery->request.cmdReq.iVer == SOCKSV4_VER)
    {
        return (sizeof(pQuery->reply.cmdRep.iVer) +
            sizeof(pQuery->reply.cmdRep.iRep) +
            sizeof(pQuery->reply.cmdRep.iBndPort) +
            sizeof(pQuery->reply.cmdRep.iBndAddr));
    }
    else
    {
        return (sizeof(pQuery->reply.cmdRep.iVer) +
            sizeof(pQuery->reply.cmdRep.iRep) +
            sizeof(pQuery->reply.cmdRep.iATyp) +
            sizeof(pQuery->reply.cmdRep.iBndAddr) +
            sizeof(pQuery->reply.cmdRep.iBndPort) + 1);
    }
}

/******************************************************************************/
static AeError SOCKSv5AuthPlainPrepareRequest(SOCKSQuery *pQuery)
{
    if (!g_pWebProxySOCKS->pUser || !g_pWebProxySOCKS->pPassword)
        return AeESOCKSAuthFailed;

    pQuery->request.authplainReq.iVer = 1;
    pQuery->request.authplainReq.iULen = strlen(g_pWebProxySOCKS->pUser);
    pQuery->request.authplainReq.pUName = g_pWebProxySOCKS->pUser;
    pQuery->request.authplainReq.iPLen = strlen(g_pWebProxySOCKS->pPassword);
    pQuery->request.authplainReq.pPasswd = g_pWebProxySOCKS->pPassword;

    return AeEOK;
}

/******************************************************************************/
static AeError SOCKSv5AuthPlainProcessReply(SOCKSQuery *pQuery)
{
    AeError rc;

    if (pQuery->reply.authplainRep.iVer != 1)
        rc = AeESOCKSWrongVersion;
    else if (pQuery->reply.authplainRep.iStatus != 0)
        rc = AeESOCKSAuthFailed;
    else
        rc = AeEOK;

    return rc;
}

/******************************************************************************/
static AeError SOCKSv5AuthPlainMarshalRequest(SOCKSQuery *pQuery)
{
    AeChar *ptr;
    
    ptr = pQuery->pIOBuffer;

    memcpy(ptr, &pQuery->request.authplainReq.iVer,
        sizeof(pQuery->request.authplainReq.iVer));
    ptr += sizeof(pQuery->request.authplainReq.iVer);

    memcpy(ptr, &pQuery->request.authplainReq.iULen,
        sizeof(pQuery->request.authplainReq.iULen));
    ptr += sizeof(pQuery->request.authplainReq.iULen);

    memcpy(ptr, pQuery->request.authplainReq.pUName,
        pQuery->request.authplainReq.iULen);
    ptr += strlen(pQuery->request.authplainReq.pUName);

    memcpy(ptr, &pQuery->request.authplainReq.iPLen,
        sizeof(pQuery->request.authplainReq.iPLen));
    ptr += sizeof(pQuery->request.authplainReq.iPLen);

    memcpy(ptr, pQuery->request.authplainReq.pPasswd,
        pQuery->request.authplainReq.iPLen);
    ptr += strlen(pQuery->request.authplainReq.pPasswd);

    return AeEOK;
}

/******************************************************************************/
static AeError SOCKSv5AuthPlainDemarshalReply(SOCKSQuery *pQuery)
{
    AeChar *ptr;
    
    ptr = pQuery->pIOBuffer;

    memcpy(&pQuery->reply.authplainRep.iVer, ptr,
        sizeof(pQuery->reply.authplainRep.iVer));
    ptr += sizeof(pQuery->reply.authplainRep.iVer);

    memcpy(&pQuery->reply.authplainRep.iStatus, ptr,
        sizeof(pQuery->reply.authplainRep.iStatus));
    ptr += sizeof(pQuery->reply.authplainRep.iStatus);

    return AeEOK;
}

/******************************************************************************/
static AeInt32 SOCKSv5AuthPlainGetRequestSize(SOCKSQuery *pQuery)
{
    return (sizeof(pQuery->request.authplainReq.iVer) +
        sizeof(pQuery->request.authplainReq.iULen) +
        pQuery->request.authplainReq.iULen +
        sizeof(pQuery->request.authplainReq.iPLen) +
        pQuery->request.authplainReq.iPLen);
}

/******************************************************************************/
static AeInt32 SOCKSv5AuthPlainGetReplySize(SOCKSQuery *pQuery)
{
    return (sizeof(pQuery->reply.authplainRep.iVer) +
        sizeof(pQuery->reply.authplainRep.iStatus));
}

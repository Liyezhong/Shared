/* $Id: AeWebConnectionSOCKS.h,v 1.2 2003/12/12 18:11:16 dkhodos Exp $ */

#ifndef _AE_WEB_CONNECTION_SOCKS_H_
#define _AE_WEB_CONNECTION_SOCKS_H_

#define SOCKSV4_VER             0x04

#define SOCKSV4_CMD_GRANTED     90
#define SOCKSV4_CMD_FAILED      91
#define SOCKSV4_CMD_REJ_IDENTD  92
#define SOCKSV4_CMD_REJ_BADID   93

#define SOCKSV5_VER             0x05

#define SOCKSV5_AUTH_NONE       0x00
#define SOCKSV5_AUTH_GSS        0x01
#define SOCKSV5_AUTH_PLAIN      0x02

#define SOCKS_CMD_CONNECT       0x01
#define SOCKS_CMD_BIND          0x02
#define SOCKSV5_CMD_UDPASSOC    0x03

#define SOCKSV5_ATYP_IPV4ADDR   0x01
#define SOCKSV5_ATYP_DOMAINNAME 0x03
#define SOCKSV5_ATYP_IPV6ADDR   0x04

#define SOCKS_MAX_PACK_SIZE     0x200

typedef struct _SOCKSv5SelectionRequest SOCKSv5SelectionRequest;
typedef struct _SOCKSv5SelectionReply SOCKSv5SelectionReply;
typedef struct _SOCKSCommandRequest SOCKSCommandRequest;
typedef struct _SOCKSCommandReply SOCKSCommandReply;
typedef struct _SOCKSv5AuthPlainRequest SOCKSv5AuthPlainRequest;
typedef struct _SOCKSv5AuthPlainReply SOCKSv5AuthPlainReply;
typedef enum _SOCKSQueryType SOCKSQueryType;
typedef union _SOCKSRequest SOCKSRequest;
typedef union _SOCKSReply SOCKSReply;
typedef struct _SOCKSQueryVTable SOCKSQueryVTable;
typedef struct _SOCKSQuery SOCKSQuery;
typedef enum _AeWebConnectionSOCKSState AeWebConnectionSOCKSState;
typedef struct _AeWebConnectionSOCKS AeWebConnectionSOCKS;

struct _SOCKSv5SelectionRequest
{
    AeUInt8 iVer;
    AeUInt8 iMethods;
    AeUInt8 pMethods[3];
};

struct _SOCKSv5SelectionReply
{
    AeUInt8 iVer;
    AeUInt8 iMethod;
};

struct _SOCKSCommandRequest
{
    AeUInt8     iVer;
    AeUInt8     iCmd;
    AeUInt8     iATyp;
    AeUInt32    iDstAddr;
    AeUInt16    iDstPort;
    AeChar      *pUserId;
};

struct _SOCKSCommandReply
{
    AeUInt8     iVer;
    AeUInt8     iRep;
    AeUInt8     iATyp;
    AeUInt32    iBndAddr;
    AeUInt16    iBndPort;
};

struct _SOCKSv5AuthPlainRequest
{
    AeUInt8     iVer;
    AeUInt8     iULen;
    AeChar      *pUName;
    AeUInt8     iPLen;
    AeChar      *pPasswd;
};

struct _SOCKSv5AuthPlainReply
{
    AeUInt8     iVer;
    AeUInt8     iStatus;
};

enum _SOCKSQueryType
{
    SOCKSv5Selection = 1,
    SOCKSCommand = 2,
    SOCKSv5AuthPlain = 3
};

union _SOCKSRequest
{
    SOCKSv5SelectionRequest selReq;
    SOCKSCommandRequest     cmdReq;
    SOCKSv5AuthPlainRequest authplainReq;
};

union _SOCKSReply
{
    SOCKSv5SelectionReply   selRep;
    SOCKSCommandReply       cmdRep;
    SOCKSv5AuthPlainReply   authplainRep;
};

struct _SOCKSQueryVTable
{
    AeError (*pPrepareRequest)(SOCKSQuery *pQuery);
    AeError (*pProcessReply)(SOCKSQuery *pQuery);
    AeError (*pMarshalRequest)(SOCKSQuery *pQuery);
    AeError (*pDemarshalReply)(SOCKSQuery *pQuery);
    AeInt32 (*pGetRequestSize)(SOCKSQuery *pQuery);
    AeInt32 (*pGetReplySize)(SOCKSQuery *pQuery);
};

#define SOCKSQueryPrepareRequestV(pQuery) \
    (*(pQuery)->pVTable->pPrepareRequest)(pQuery)
#define SOCKSQueryProcessReplyV(pQuery) \
    (*(pQuery)->pVTable->pProcessReply)(pQuery)
#define SOCKSQueryMarshalRequestV(pQuery) \
    (*(pQuery)->pVTable->pMarshalRequest)(pQuery)
#define SOCKSQueryDemarshalReplyV(pQuery) \
    (*(pQuery)->pVTable->pDemarshalReply)(pQuery)
#define SOCKSQueryGetRequestSizeV(pQuery) \
    (*(pQuery)->pVTable->pGetRequestSize)(pQuery)
#define SOCKSQueryGetReplySizeV(pQuery) \
    (*(pQuery)->pVTable->pGetReplySize)(pQuery)

struct _SOCKSQuery
{
    SOCKSQueryVTable        *pVTable;

    SOCKSQueryType          iQueryType;
    SOCKSRequest            request;
    SOCKSReply              reply;

    AeWebConnectionSOCKS    *pParent;
    AeChar                  *pIOBuffer;
};

enum _AeWebConnectionSOCKSState
{
    AeWebConnectionSOCKSClosed,
    AeWebConnectionSOCKSConnecting,
    AeWebConnectionSOCKSHandshaking,
    AeWebConnectionSOCKSEstablished
};

struct _AeWebConnectionSOCKS
{
    AeWebConnection             parent;

    AeWebConnectionSOCKSState   iState;
    AeUInt8                     iVersion;
    AeChar                      *pHost;
    AeNetAddress                netAddress;

    SOCKSQuery                  *pQuery;
};

#ifdef __cplusplus
extern "C" {
#endif

AeWebConnectionSOCKS    *AeWebConnectionSOCKSNew(void);

#ifdef __cplusplus
}
#endif

#endif

/* $Id: AeRemoteDirectTransport.c,v 1.1.4.1 2010/08/13 16:01:38 dkhodos Exp $ */

#include "Ae.h"

#if defined(ENABLE_REMOTE_SESSION)

static void AeRemoteDirectTransportDestroy(AeRemoteDirectTransport *pTransport);
static void AeRemoteDirectTransportConnect(AeRemoteDirectTransport *pTransport);
static void AeRemoteDirectTransportSend(AeRemoteDirectTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
static void AeRemoteDirectTransportReceive(AeRemoteDirectTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft);
static AeBool AeRemoteDirectTransportIsConnecting(AeRemoteDirectTransport *pTransport);
static AeBool AeRemoteDirectTransportIsConnected(AeRemoteDirectTransport *pTransport);
static AeBool AeRemoteDirectTransportIsSending(AeRemoteDirectTransport *pTransport);
static AeBool AeRemoteDirectTransportIsError(AeRemoteDirectTransport *pTransport);

/**************************************************************************/
AeRemoteTransportVTable g_aeRemoteDirectTransportVTable =
{
    (void (*)(AeRemoteTransport *)) AeRemoteDirectTransportDestroy,
    (void (*)(AeRemoteTransport *)) AeRemoteDirectTransportConnect,
    (void (*)(AeRemoteTransport *, AeChar *, AeInt32, AeInt32 *)) AeRemoteDirectTransportSend,
    (void (*)(AeRemoteTransport *, AeChar *, AeInt32, AeInt32 *)) AeRemoteDirectTransportReceive,
    (AeBool (*)(AeRemoteTransport *)) AeRemoteDirectTransportIsConnecting,
    (AeBool (*)(AeRemoteTransport *)) AeRemoteDirectTransportIsConnected,
    (AeBool (*)(AeRemoteTransport *)) AeRemoteDirectTransportIsSending,
    (AeBool (*)(AeRemoteTransport *)) AeRemoteDirectTransportIsError
};

/**************************************************************************/
AeRemoteDirectTransport *AeRemoteDirectTransportNew(AeRemoteSession *pSession)
{
    AeRemoteDirectTransport *pTransport;

    pTransport = (AeRemoteDirectTransport *) AeAlloc(sizeof(AeRemoteDirectTransport));
    if (!pTransport)
        return NULL;
    memset(pTransport, 0, sizeof(AeRemoteDirectTransport));

    AeRemoteTransportInitialize((AeRemoteTransport *) &pTransport->parent, "Direct", pSession);
    pTransport->parent.pVTable = &g_aeRemoteDirectTransportVTable;

    pTransport->pDC = AeRemoteDirectConnectionNew(pSession);
    if (!pTransport->pDC)
    {
        AeRemoteDirectTransportDestroy(pTransport);
        return NULL;
    }

    return pTransport;
}

/**************************************************************************/
void AeRemoteDirectTransportDestroy(AeRemoteDirectTransport *pTransport)
{
    if (pTransport->pDC)
        AeRemoteDirectConnectionDeactivate(pTransport->pDC);

    AeFree(pTransport);
}

/**************************************************************************/
static void AeRemoteDirectTransportConnect(AeRemoteDirectTransport *pTransport)
{
    AeRemoteDirectConnectionConnect(pTransport->pDC);
}

/**************************************************************************/
static void AeRemoteDirectTransportSend(AeRemoteDirectTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft)
{
    AeRemoteDirectConnectionSend(pTransport->pDC, pData, iSize, piBytesLeft);
}

/**************************************************************************/
static void AeRemoteDirectTransportReceive(AeRemoteDirectTransport *pTransport, AeChar *pData, AeInt32 iSize, AeInt32 *piBytesLeft)
{
    AeRemoteDirectConnectionReceive(pTransport->pDC, pData, iSize, piBytesLeft);
}

/**************************************************************************/
static AeBool AeRemoteDirectTransportIsConnecting(AeRemoteDirectTransport *pTransport)
{
    if (pTransport->pDC->iState == AeRemoteDirectConnectionConnecting ||
        pTransport->pDC->iState == AeRemoteDirectConnectionConnected)
        return AeTrue;

    return AeFalse;
}

/**************************************************************************/
static AeBool AeRemoteDirectTransportIsConnected(AeRemoteDirectTransport *pTransport)
{
    if (pTransport->pDC->iState == AeRemoteDirectConnectionPreambleSent)
        return AeTrue;

    return AeFalse;
}

/**************************************************************************/
static AeBool AeRemoteDirectTransportIsSending(AeRemoteDirectTransport *pTransport)
{
    return AeRemoteDirectConnectionIsSending(pTransport->pDC);
}

/**************************************************************************/
static AeBool AeRemoteDirectTransportIsError(AeRemoteDirectTransport *pTransport)
{
    return pTransport->pDC->bError;
}

#endif

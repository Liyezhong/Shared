/* AeAgentRestart.c */

#include "Ae.h"

#if defined(ENABLE_FILE_TRANSFER)

static AeBool AeAgentRestartInitialize(AeAgentRestart *pAeAgentRestart, AeHandle pParameters);
static void AeAgentRestartProcess(AeAgentRestart *pAeAgentRestart);
static void AeAgentRestartDestroy(AeAgentRestart *pAeAgentRestart);
static void AeAgentRestartNotify(AeAgentRestart *pAeAgentRestart, AeFileTransferEvent iEvent);
static AeBool AeAgentRestartIsIdle(AeAgentRestart *pAeAgentRestart);
static AeBool AeAgentRestartIsPendingRequest(AeAgentRestart *pAeAgentRestart);

static AeFileInstructionVTable g_AgentRestartVTable =
{
    (AeBool (*)(AeFileInstruction *, AeHandle)) AeAgentRestartInitialize,
    (void   (*)(AeFileInstruction *)) AeAgentRestartProcess,
    (void   (*)(AeFileInstruction *)) AeAgentRestartDestroy,
    (void   (*)(AeFileInstruction *, AeFileTransferEvent)) AeAgentRestartNotify,
    (AeBool (*)(AeFileInstruction *)) AeAgentRestartIsIdle,
    (AeBool (*)(AeFileInstruction *)) AeAgentRestartIsPendingRequest
};

/**************************************************************************/
AeAgentRestart *AeAgentRestartNew(void)
{
    AeAgentRestart *pAgentRestart = (AeAgentRestart *) AeAlloc(sizeof(AeAgentRestart));
    if (!pAgentRestart)
        return NULL;
    memset(pAgentRestart, 0, sizeof(AeAgentRestart));

    pAgentRestart->parent.pVTable = &g_AgentRestartVTable;
    pAgentRestart->parent.iState = AeFileInstructionInitialized;
    pAgentRestart->parent.iError = AeNOERROR;

    return pAgentRestart;
}

/**************************************************************************/
void AeAgentRestartDestroy(AeAgentRestart *pAeAgentRestart)
{
    AeFree(pAeAgentRestart);
}

/**************************************************************************/
static void AeAgentRestartNotify(AeAgentRestart *pAeAgentRestart, AeFileTransferEvent iEvent)
{
    if (g_drmCallbacks.pOnFileTransferEvent)
        (*g_drmCallbacks.pOnFileTransferEvent)(pAeAgentRestart->parent.pPackage->iDeviceId,
            iEvent, NULL);
}

/**************************************************************************/
static AeBool AeAgentRestartIsIdle(AeAgentRestart *pAeAgentRestart)
{
    /* This is never defered */
    return AeFalse;
}

/***************************************************************************/
static AeBool AeAgentRestartIsPendingRequest(AeAgentRestart *pAeAgentRestart)
{
    /* This is never pending */
    return AeFalse;
}

/**************************************************************************/
static AeBool AeAgentRestartInitialize(AeAgentRestart *pAeAgentRestart, AeHandle pParameters)
{
    /* hard or soft restart */
    AeHandle pChild = AeDRMSOAPGetParameterFirstChild(pParameters);
    if(pChild)
    {
        AeChar* pTmp = AeDRMSOAPGetParameterName(pChild);
        if(pTmp && strcmp(pTmp, "ha") == 0)
        {
            pTmp = AeDRMSOAPGetParameterValue(pChild);
            if(pTmp)
                pAeAgentRestart->bHard = (strtol(pTmp, NULL, 10) ? AeTrue : AeFalse);
        }
    }

    if (g_drmSettings.iLogLevel >= AeLogDebug)
        AE_LOG_EX(AeLogDebug, AE_LOG_CATEGORY_AGENT_RESTART, "Got Agent restart command");

    return AeTrue;
}

/**************************************************************************/
static void AeAgentRestartProcess(AeAgentRestart *pAeAgentRestart)
{
    AeDRMSOAPCommandStatus status;

    if (pAeAgentRestart->parent.iState == AeFileInstructionInitialized)
    {
        if (g_drmSettings.iLogLevel >= AeLogInfo)
            AE_LOG_EX(AeLogInfo, AE_LOG_CATEGORY_AGENT_RESTART, "Executing Agent restart instruction");

        pAeAgentRestart->parent.iState = AeFileInstructionExecuting;
    }

    /* call user callback (if any) to restart agent */
    if (pAeAgentRestart->parent.iState == AeFileInstructionExecuting)
    {
       if (g_drmCallbacks.pOnCommandRestart)
            (*g_drmCallbacks.pOnCommandRestart)(pAeAgentRestart->parent.pPackage->iDeviceId, pAeAgentRestart->bHard, &status);

       /* we are done with this instruction */
        pAeAgentRestart->parent.iState = AeFileInstructionFinished;
    }
}

#endif /* defined(ENABLE_FILE_TRANSFER) */

#include "DeviceControl/Include/Devices/StateMachine/Drawer.h"
#include "DeviceControl/Include/Devices/StateMachine/DrawerState.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"

namespace DeviceControl
{

/****************************************************************************/
void CDrawerState::ChangeState(CDrawer *p_Context, CDrawerState *p_State)
{
    p_Context->ChangeState(p_State);
}

/****************************************************************************/
CBlockedState CBlockedState::m_Instance;

void CBlockedState::SetBlockState (CDrawer *p_Context, CDigitalOutput *p_LedBlocked,
                                   CDigitalOutput *p_LedFree, BlockState_t BlockState)
{
    if (BlockState == BLOCKING_STATE_FREE) {
        ReturnCode_t Result = p_LedBlocked->SetOutputValue(0);
        if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
            Result = p_LedFree->SetOutputValue(1);
            if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
                ChangeState(p_Context, CUnblockingState::Instance());
            }
            else {

            }
        }
        else {

        }
    }
    else {

    }
}

/****************************************************************************/
CUnblockingState CUnblockingState::m_Instance;

void CUnblockingState::OnReportSetBlockState (CDrawer *p_Context, ReturnCode_t ReturnCode)
{
    if (ReturnCode == DCL_ERR_FCT_CALL_SUCCESS) {
        ChangeState(p_Context, CUnblockedState::Instance());
    }
    else {

    }
}

/****************************************************************************/
CUnblockedState CUnblockedState::m_Instance;

void CUnblockedState::SetBlockState (CDrawer *p_Context, CDigitalOutput *p_LedBlocked,
                                     CDigitalOutput *p_LedFree, BlockState_t BlockState)
{
    if (BlockState == BLOCKING_STATE_BLOCKED) {
        ReturnCode_t Result = p_LedFree->SetOutputValue(0);
        if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
            Result = p_LedBlocked->SetOutputValue(1);
            if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
                ChangeState(p_Context, CBlockingState::Instance());
            }
            else {

            }
        }
        else {

        }
    }
    else {

    }
}

/****************************************************************************/
CBlockingState CBlockingState::m_Instance;

void CBlockingState::OnReportSetBlockState (CDrawer *p_Context, ReturnCode_t ReturnCode)
{
    if (ReturnCode == DCL_ERR_FCT_CALL_SUCCESS) {
        ChangeState(p_Context, CBlockedState::Instance());
    }
    else {

    }
}

} //namespace

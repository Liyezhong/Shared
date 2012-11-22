#include "DeviceControl/Include/Devices/StateMachine/MoveEndToEnd.h"
#include "DeviceControl/Include/Devices/StateMachine/MoveEndToEndState.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"

namespace DeviceControl
{

/****************************************************************************/
void CMoveEndToEndState::ChangeState(CMoveEndToEnd *p_Context, CMoveEndToEndState *p_State)
{
    p_Context->ChangeState(p_State);
}

/****************************************************************************/
CUninitializedState CUninitializedState::m_Instance;

void CUninitializedState::DoReferenceRun(CMoveEndToEnd *p_Context, CStepperMotor *p_StepperMotor)
{
    ReturnCode_t Result = p_StepperMotor->ExecReferenceMovement();
    if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
        ChangeState(p_Context, CInitializingState::Instance());
    }
}

/****************************************************************************/
CInitializingState CInitializingState::m_Instance;

void CInitializingState::OnReportReferenceMovementAckn(CMoveEndToEnd *p_Context, ReturnCode_t ReturnCode)
{
    if (ReturnCode == DCL_ERR_FCT_CALL_SUCCESS) {
        ChangeState(p_Context, CPosition1State::Instance());
    }
    else {
        ChangeState(p_Context, CUninitializedState::Instance());
    }
}

/****************************************************************************/
CPosition1State CPosition1State::m_Instance;

void CPosition1State::MoveToPosition2(CMoveEndToEnd *p_Context, CStepperMotor *p_StepperMotor)
{
    ReturnCode_t Result = p_StepperMotor->DriveToPosition(2000, 0);
    if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
        ChangeState(p_Context, CMoveToPosition2State::Instance());
    }
    else {
        ChangeState(p_Context, CUninitializedState::Instance());
    }
}

/****************************************************************************/
CMoveToPosition2State CMoveToPosition2State::m_Instance;

void CMoveToPosition2State::OnReportMovementAckn(CMoveEndToEnd *p_Context, ReturnCode_t ReturnCode)
{
    if (ReturnCode == DCL_ERR_FCT_CALL_SUCCESS) {
        ChangeState(p_Context, CPosition2State::Instance());
    }
    else {
        ChangeState(p_Context, CUninitializedState::Instance());
    }
}

/****************************************************************************/
CPosition2State CPosition2State::m_Instance;

void CPosition2State::MoveToPosition1(CMoveEndToEnd *p_Context, CStepperMotor *p_StepperMotor)
{
    ReturnCode_t Result = p_StepperMotor->DriveToPosition(0, 0);
    if (Result == DCL_ERR_FCT_CALL_SUCCESS) {
        ChangeState(p_Context, CMoveToPosition1State::Instance());
    }
    else {
        ChangeState(p_Context, CUninitializedState::Instance());
    }
}

/****************************************************************************/
CMoveToPosition1State CMoveToPosition1State::m_Instance;

void CMoveToPosition1State::OnReportMovementAckn(CMoveEndToEnd *p_Context, ReturnCode_t ReturnCode)
{
    if (ReturnCode == DCL_ERR_FCT_CALL_SUCCESS) {
        ChangeState(p_Context, CPosition1State::Instance());
    }
    else {
        ChangeState(p_Context, CUninitializedState::Instance());
    }
}

} //namespace

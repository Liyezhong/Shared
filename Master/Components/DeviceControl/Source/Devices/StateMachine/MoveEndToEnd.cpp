#include "DeviceControl/Include/Devices/StateMachine/MoveEndToEnd.h"
#include "DeviceControl/Include/Devices/StateMachine/MoveEndToEndState.h"
#include <iostream>

namespace DeviceControl
{

/****************************************************************************/
CMoveEndToEnd::CMoveEndToEnd(CStepperMotor *p_StepperMotor) : mp_StepperMotor(p_StepperMotor)
{
    mp_State = CUninitializedState::Instance();
}

void CMoveEndToEnd::DoReferenceRun()
{
    mp_State->DoReferenceRun(this, mp_StepperMotor);
}

void CMoveEndToEnd::MoveToPosition1()
{
    mp_State->MoveToPosition1(this, mp_StepperMotor);
}

void CMoveEndToEnd::MoveToPosition2()
{
    mp_State->MoveToPosition2(this, mp_StepperMotor);
}

void CMoveEndToEnd::OnReportReferenceMovementAckn(quint32 InstanceId, ReturnCode_t HandleInfo, qint32 Position)
{
    mp_State->OnReportReferenceMovementAckn(this, HandleInfo);
}

void CMoveEndToEnd::OnReportMovementAckn(quint32 InstanceId, ReturnCode_t HandleInfo, qint32 Position, qint16 Speed)
{
    mp_State->OnReportMovementAckn(this, HandleInfo);
}

void CMoveEndToEnd::ChangeState(CMoveEndToEndState *p_State)
{
    mp_State = p_State;
}

} //namespace

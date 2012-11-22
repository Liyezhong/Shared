#include "FmStepperMotorInitialize.h"
#include <QFinalState>
#include "SignalTransition.h"

#include "MockFm.h"


namespace DeviceControl
{

typedef CSignalTransition<CFmStepperMotorInitialize> CFmSmInitTransition;

CFmStepperMotorInitialize::CFmStepperMotorInitialize(CMockFm *p_StepperMotor, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent), mp_StepperMotor(p_StepperMotor)
{
    CState *p_Start = new CState("StepperInitStart", this);
    setInitialState(p_Start);
    CState *p_SetState = new CState("StepperInitSetState", this);
    CState *p_RefRun = new CState("StepperInitRefRun", this);
    QFinalState *p_End = new QFinalState(this);

    p_Start->addTransition( new CFmSmInitTransition(
        p_Start, SIGNAL(entered()),
        *this, &CFmStepperMotorInitialize::Trans_Start_SetState,
        p_SetState) );

    p_SetState->addTransition( new CFmSmInitTransition(
        mp_StepperMotor, SIGNAL(ReportSetStateAckn(quint32, ReturnCode_t)),
        *this, &CFmStepperMotorInitialize::Trans_SetState_RefRun,
        p_RefRun) );

    p_RefRun->addTransition( new CFmSmInitTransition(
        mp_StepperMotor, SIGNAL(ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)),
        *this, &CFmStepperMotorInitialize::Trans_RefRun_End,
        p_End) );
}


bool CFmStepperMotorInitialize::Trans_Start_SetState(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    ReturnCode_t ReturnCode = mp_StepperMotor->SetMotorState(true);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    return true;
}

bool CFmStepperMotorInitialize::Trans_SetState_RefRun(QEvent *p_Event)
{
    // ReportSetStateAckn(quint32, ReturnCode_t)
    ReturnCode_t ReturnCode = CFmSmInitTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    ReturnCode = mp_StepperMotor->ExecReferenceMovement();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    return true;
}

bool CFmStepperMotorInitialize::Trans_RefRun_End(QEvent *p_Event)
{
    // ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32)
    ReturnCode_t ReturnCode = CFmSmInitTransition::GetEventValue(p_Event, 1);

    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }

    return true;
}


} //namespace

// vi: set ts=4 sw=4 et:

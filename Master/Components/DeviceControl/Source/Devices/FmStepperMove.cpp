#include <QFinalState>
#include "DeviceControl/Include/Devices/FmStepperMove.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"

namespace DeviceControl
{

typedef CSignalTransition<CFmStepperMove> CFmStepperMoveTransition;

CFmStepperMove::CFmStepperMove(
    CStepperMotor *p_StepperMotor,
    const QString &Name,
    QState *p_Parent) :
        CState(Name, p_Parent),
        mp_StepperMotor(p_StepperMotor),
        m_TargetPosition(0),
        m_MotionProfile(0),
        m_MotionProfileStop(0),
        m_TargetSpeed(0)
{

    /*

         |Idle|

    Move(pos, profile)   ^             ^
    FM: Move             |             |
                   FM Ack: Move        |
         |         Ack: ReportMove     |
         V                             |
         |Moving|                      |
                                       |
      Stop      Resume     Abort;  emit aborted (intermediate state)

         |Stopping|

         |Stopped|


      Disable; emit NeedInitialize


    */
    /*

         |Idle|
          ^
          | Rotate(Speed, profile)   ^
          | FM: Rotate               |
    |Speed!=Zero|                    |
         |                FM Ack: Drive Speed
         |                     Ack: Rotate
         V
     |Rotating|
         |
         |
         V
Move(pos, profile)       ^             ^
    FM: Move             |             |
                   FM Ack: Move        |
         |         Ack: ReportMove     |
         V                             |
      |Moving|                         |
                                       |
      Stop      Resume     Abort;  emit aborted (intermediate state)

         |Stopping|

         |Stopped|
    */

    CState *p_Idle = new CState("StepperIdle", this);
    CState *p_ChangingSpeed = new CState("StepperChangingSpeed", this);
    CState *p_RotationActive = new CState("StepperRotationActive", this);
    CState *p_Moving = new CState("StepperMoving", this);
    CState *p_Stopping = new CState("StepperStopping", this);
    CState *p_Stopped = new CState("StepperStopped", this);
    setInitialState(p_Idle);

    p_Idle->addTransition( new CFmStepperMoveTransition(
        this, SIGNAL(Move(quint32, quint8)),
        *this, &CFmStepperMove::Trans_Idle_Moving,
        p_Moving) );
    p_Moving->addTransition( new CFmStepperMoveTransition(
        mp_StepperMotor, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)),
        *this, &CFmStepperMove::Trans_Moving_Idle,
        p_Idle) );
    p_Moving->addTransition( new CFmStepperMoveTransition(
        this, SIGNAL(Stop()),
        *this, &CFmStepperMove::Trans_Moving_Stopping,
        p_Stopping) );
    p_Stopping->addTransition( new CFmStepperMoveTransition(
        mp_StepperMotor, SIGNAL(ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16)),
        *this, &CFmStepperMove::Trans_Stopping_Stopped,
        p_Stopped) );
    p_Stopped->addTransition( new CFmStepperMoveTransition(
        this, SIGNAL(Resume()),
        *this, &CFmStepperMove::Trans_Stopped_Moving,
        p_Moving) );
    p_Stopped->addTransition( new CFmStepperMoveTransition(
        this, SIGNAL(Abort()),
        *this, &CFmStepperMove::Trans_Stopped_Idle,
        p_Idle) );

    p_Idle->addTransition( new CFmStepperMoveTransition(
        this, SIGNAL(Rotate(quint32, quint8)),
        *this, &CFmStepperMove::Trans_Idle_ChangingSpeed,
        p_ChangingSpeed) );
    p_ChangingSpeed->addTransition( new CFmStepperMoveTransition(
        mp_StepperMotor, SIGNAL(ReportMovementAckn(quint32,ReturnCode_t,qint32,qint16)),
        *this, &CFmStepperMove::Trans_ChangingSpeed_Idle,
        p_Idle) );
    p_ChangingSpeed->addTransition( new CFmStepperMoveTransition(
        mp_StepperMotor, SIGNAL(ReportMovementAckn(quint32,ReturnCode_t,qint32,qint16)),
        *this, &CFmStepperMove::Trans_ChangingSpeed_RotationActive,
        p_RotationActive) );
    p_RotationActive->addTransition( new CFmStepperMoveTransition(
        this, SIGNAL(Rotate(quint32, quint8)),
        *this, &CFmStepperMove::Trans_RotationActive_ChangingSpeed,
        p_ChangingSpeed));
    p_RotationActive->addTransition( new CFmStepperMoveTransition(
        this, SIGNAL(Move(quint32, quint8)),
        *this, &CFmStepperMove::Trans_RotationActive_Moving,
         p_Moving));

}
bool CFmStepperMove::Trans_Idle_Moving(QEvent *p_Event)
{
    // Move(quint32 TargetPosition, quint8 MotionProfileIdx)
    if (!CFmStepperMoveTransition::GetEventValue(p_Event, 0, m_TargetPosition)) {
        emit StepperError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CFmStepperMoveTransition::GetEventValue(p_Event, 1, m_MotionProfile)) {
        emit StepperError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    ReturnCode_t ReturnCode = mp_StepperMotor->DriveToPosition(
        m_TargetPosition, m_MotionProfile
    );

    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit StepperError(ReturnCode);
        return false;
    }

    return true;
}

bool CFmStepperMove::Trans_Moving_Idle(QEvent *p_Event)
{
    // ReportMovementAckn(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position, qint16 Speed)
    ReturnCode_t ReturnCode = CFmStepperMoveTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    else
    {
        emit MoveAck();
    }

    return true;
}

bool CFmStepperMove::Trans_Moving_Stopping(QEvent *p_Event)
{
    // Stop()
    Q_UNUSED(p_Event)
    ReturnCode_t ReturnCode = mp_StepperMotor->DriveSpeed(0, m_MotionProfileStop);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        // TODO: handle movement already finished case 
        emit StepperError(ReturnCode);
        return false;
    }

    return true;
}

bool CFmStepperMove::Trans_Stopping_Stopped(QEvent *p_Event)
{
    // ReportMovementAckn(quint32 InstanceID, ReturnCode_t ReturnCode, qint32 Position, qint16 Speed)
    ReturnCode_t ReturnCode = CFmStepperMoveTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    emit ReportStop(ReturnCode);

    return true;
}

bool CFmStepperMove::Trans_Stopped_Moving(QEvent *p_Event)
{
    // Resume()
    Q_UNUSED(p_Event)
    ReturnCode_t ReturnCode = mp_StepperMotor->DriveToPosition(
        m_TargetPosition, m_MotionProfile
    );
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit StepperError(ReturnCode);
        return false;
    }
    // emit already here, as start of resumption succeeded
    emit ReportResume(ReturnCode);

    return true;
}

bool CFmStepperMove::Trans_Stopped_Idle(QEvent *p_Event)
{
    // Abort
    Q_UNUSED(p_Event)
    emit ReportAbort(DCL_ERR_FCT_CALL_SUCCESS);
    return true;
}

bool CFmStepperMove::Trans_Idle_ChangingSpeed(QEvent *p_Event)
{
    if (!CFmStepperMoveTransition::GetEventValue(p_Event, 0, m_TargetSpeed)) {
        emit StepperError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CFmStepperMoveTransition::GetEventValue(p_Event, 1, m_MotionProfile)) {
        emit StepperError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (m_TargetSpeed != 0) {
        ReturnCode_t ReturnCode = mp_StepperMotor->DriveSpeed(m_TargetSpeed, m_MotionProfile);
        if(DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit StepperError(ReturnCode);
        return false;
        }
    }
    return true;
}

bool CFmStepperMove::Trans_ChangingSpeed_Idle(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CFmStepperMoveTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    CFmStepperMoveTransition::GetEventValue(p_Event, 3, m_TargetSpeed);
    if (m_TargetSpeed != 0) {
        return false;
    }
    emit RotateAck();
    return true;
}

bool CFmStepperMove::Trans_ChangingSpeed_RotationActive(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CFmStepperMoveTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit NeedInitialize(ReturnCode);
        return false;
    }
    CFmStepperMoveTransition::GetEventValue(p_Event, 3, m_TargetSpeed);
    if (m_TargetSpeed == 0) {
        return false;
    }
    emit RotateAck();
    return true;
}

bool CFmStepperMove::Trans_RotationActive_ChangingSpeed(QEvent *p_Event)
{
    if (!CFmStepperMoveTransition::GetEventValue(p_Event, 0, m_TargetSpeed)) {
            emit StepperError(DCL_ERR_INVALID_PARAM);
            return false;
    }
    if (!CFmStepperMoveTransition::GetEventValue(p_Event, 1, m_MotionProfile)) {
            emit StepperError(DCL_ERR_INVALID_PARAM);
            return false;
    }
    mp_StepperMotor->DriveSpeed(m_TargetSpeed, m_MotionProfile);
    return true;
}

bool CFmStepperMove::Trans_RotationActive_Moving(QEvent *p_Event)
{
    if (!CFmStepperMoveTransition::GetEventValue(p_Event, 0, m_TargetPosition)) {
        emit StepperError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CFmStepperMoveTransition::GetEventValue(p_Event, 1, m_MotionProfile)) {
        emit StepperError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    ReturnCode_t ReturnCode = mp_StepperMotor->DriveToPosition(
        m_TargetPosition, m_MotionProfile
    );
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit StepperError(ReturnCode);
        return false;
    }
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

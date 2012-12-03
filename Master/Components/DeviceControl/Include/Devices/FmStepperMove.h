#ifndef DEVICECONTROL_FMSTEPPERMOVE_H
#define DEVICECONTROL_FMSTEPPERMOVE_H

#include "DeviceState.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CFmStepperMove : public CState
{
    Q_OBJECT

public:
    explicit CFmStepperMove(CStepperMotor *p_StepperMotor, const QString &Name, QState *p_Parent = 0);

signals:
    // command request interface to DCP 
    void Stop();
    void Resume();
    void Abort();
    void Disable();
    void Rotate(quint32 TargetSpeed, quint8 MotionProfileIdx);

    // command response interface to DCP
    void ReportStop(ReturnCode_t ReturnCode);
    void ReportResume(ReturnCode_t ReturnCode);
    void ReportAbort(ReturnCode_t ReturnCode);
    void ReportDisable(ReturnCode_t ReturnCode);

    // command request interface
    void Move(quint32 TargetPosition, quint8 MotionProfileIdx);

    // command response interface
    void MoveAck();
    void RotateAck();
//    void SpeedAck(quint32 Speed);

    // to this
    void StepperError(ReturnCode_t ReturnCode);
    // depends on encoder presence
    void NeedInitialize(ReturnCode_t ReturnCode);

private:
    bool Trans_Idle_Moving(QEvent *p_Event);
    bool Trans_Moving_Idle(QEvent *p_Event);
    bool Trans_Moving_Stopping(QEvent *p_Event);
    bool Trans_Stopping_Stopped(QEvent *p_Event);
    bool Trans_Stopped_Moving(QEvent *p_Event);
    bool Trans_Stopped_Idle(QEvent *p_Event);
    bool Trans_Idle_ChangingSpeed(QEvent *p_Event);
    bool Trans_ChangingSpeed_RotationActive(QEvent *p_Event);
    bool Trans_ChangingSpeed_Idle(QEvent *p_Event);
    bool Trans_RotationActive_ChangingSpeed(QEvent *p_Event);
    bool Trans_RotationActive_Moving(QEvent *p_Event);

    CStepperMotor *mp_StepperMotor;

    quint32 m_TargetPosition;
    quint8 m_MotionProfile;
    quint8 m_MotionProfileStop;
    quint16 m_TargetSpeed;
};

} //namespace

#endif // DEVICECONTROL_FMSTEPPERMOVE_H

// vi: set ts=4 sw=4 et:


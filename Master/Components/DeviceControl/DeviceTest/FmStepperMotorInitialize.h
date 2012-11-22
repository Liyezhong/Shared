#ifndef DEVICECONTROL_FMSTEPPERMOTORINITIALIZE_H
#define DEVICECONTROL_FMSTEPPERMOTORINITIALIZE_H

#include "State.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

class CMockFm;

class CFmStepperMotorInitialize : public CState
{
    Q_OBJECT

public:
    explicit CFmStepperMotorInitialize(CMockFm *p_StepperMotor, const QString &Name, QState *p_Parent = 0);

signals:
    void NeedInitialize(ReturnCode_t);

private:
    bool Trans_Start_SetState(QEvent *p_Event);
    bool Trans_SetState_RefRun(QEvent *p_Event);
    bool Trans_RefRun_End(QEvent *p_Event);

    CMockFm *mp_StepperMotor,;
};

} //namespace

#endif // DEVICECONTROL_FMSTEPPERMOTORINITIALIZE_H

// vi: set ts=4 sw=4 et:


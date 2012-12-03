#ifndef DEVICECONTROL_MOVEACK_H
#define DEVICECONTROL_MOVEACK_H

#include "DeviceState.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

class CMoveAck : public CState
{
    Q_OBJECT

public:
    CMoveAck(CStepperMotor &StepperMotor, const QString &Name, QState *p_Parent);

signals:
    void MoveError(ReturnCode_t ReturnCode);

protected:
    bool CheckMovementAckn(QEvent *p_Event);

private:
    CStepperMotor &m_StepperMotor;
};

}

#endif // MOVEACK_H

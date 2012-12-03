#ifndef DEVICECONTROL_MOVEXYZ_H
#define DEVICECONTROL_MOVEXYZ_H

#include "DeviceState.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl
{

#define NO_CHANGE ((quint32)0xFFFFFFFF)

class CMoveXYZ : public CState
{
Q_OBJECT

public:
    CMoveXYZ(CStepperMotor &XAxisMotor,
             CStepperMotor &YAxisMotor,
             CStepperMotor &ZAxisMotor,
             const QString &Name,
             QState *p_Parent = 0);

signals:
    // Request Interface
    void Move(quint32 PositionX, quint8 ProfileX,
              quint32 PositionY, quint8 ProfileY,
              quint32 PositionZ, quint8 ProfileZ);
    void Abort();
    void Disable();

    // Response Interface
    void ReportMove(ReturnCode_t ReturnCode);
    void ReportAbort(ReturnCode_t ReturnCode);
    void ReportDisable(ReturnCode_t ReturnCode);

    // Error Response
    void StepperError(ReturnCode_t ReturnCode);

protected:
    bool Trans_Idle_Moving(QEvent *p_Event);
    bool Trans_Moving_Idle(QEvent *p_Event);

    bool Trans_Moving_Aborting(QEvent *p_Event);
    bool Trans_Aborting_Idle(QEvent *p_Event);

private:
    CStepperMotor &m_XAxisMotor;
    CStepperMotor &m_YAxisMotor;
    CStepperMotor &m_ZAxisMotor;

    quint8 m_MotionProfileX;
    quint8 m_MotionProfileY;
    quint8 m_MotionProfileZ;
};

}

#endif // CMOVEXYZ_H

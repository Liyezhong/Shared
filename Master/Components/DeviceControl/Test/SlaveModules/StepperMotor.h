#ifndef STEPPERMOTOR_SIM_H
#define STEPPERMOTOR_SIM_H

#include <QObject>

#include "FunctionModule.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Stepper motor function module class
 *
 *      This class implements the functionality to configure and control a
 *      slave's 'stepper motor' function module
 */
/****************************************************************************/
class CStepperMotor : public CFunctionModule
{
    Q_OBJECT

public:
    ReturnCode_t SetMotorState(bool Enabled)
    {
        Q_UNUSED(Enabled)
        return GetReturn();
    }

    ReturnCode_t ExecReferenceMovement()
    {
        return GetReturn();
    }

    ReturnCode_t DriveToPosition(quint32 TargetPosition, quint8 MotionProfileIdx)
    {
        Q_UNUSED(TargetPosition)
        Q_UNUSED(MotionProfileIdx)
       return GetReturn();
    }

    ReturnCode_t DriveSpeed(quint16 TargetSpeed, quint8 MotionProfileIdx)
    {
        Q_UNUSED(TargetSpeed)
        Q_UNUSED(MotionProfileIdx)
        return GetReturn();
    }

    void EmitReportSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
    {
        emit ReportSetStateAckn(InstanceID, HdlInfo);
        THREAD_PROCESS();
    }

    void EmitReportReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
    {
       emit ReportReferenceMovementAckn(InstanceID, HdlInfo, Position);
       THREAD_PROCESS();
    }

    void EmitReportMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position, qint16 Speed)
    {
        emit ReportMovementAckn(InstanceID, HdlInfo, Position, Speed);
        THREAD_PROCESS();
    }

    void EmitReportSpeedMovementCmdAckn(quint32 InstanceID, ReturnCode_t HdlInf)
    {
        emit ReportSpeedMovementCmdAckn(InstanceID, HdlInf);
        THREAD_PROCESS();
    }

    void EmitReportSpeed(quint32 InstanceID, ReturnCode_t HdlInf, quint16 Speed)
    {
        emit ReportSpeed(InstanceID, HdlInf, Speed);
        THREAD_PROCESS();
    }

signals:
    void ReportSetStateAckn(quint32, ReturnCode_t);
    void ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32);
    void ReportSpeedMovementCmdAckn(quint32 , ReturnCode_t);
    void ReportSpeed(quint32, ReturnCode_t, quint16);
    void ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16);

private:

};

}

#endif // STEPPERMOTOR_H

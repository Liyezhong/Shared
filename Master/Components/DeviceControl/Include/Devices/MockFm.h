#ifndef DEVICECONTROL_MOCKFM_H
#define DEVICECONTROL_MOCKFM_H

#include <QDateTime>
#include <QObject>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl {

class CMockFm : public QObject
{
Q_OBJECT
public:
    CMockFm(QObject *parent = 0) : QObject(parent), m_nextError(false) {}
    void SetNextError() { m_nextError = true; }
    void Reset() { m_nextError = false; }

    ReturnCode_t SetMotorState(bool Enabled) { Q_UNUSED(Enabled) return GetReturn(); }
    ReturnCode_t ExecReferenceMovement()     { return GetReturn(); }

    ReturnCode_t SetState(bool Enabled, quint8 Antenna) { Q_UNUSED(Enabled) Q_UNUSED(Antenna) return GetReturn(); }
    ReturnCode_t SetLogin(quint32 Password) { Q_UNUSED(Password) return GetReturn(); }
    ReturnCode_t ReqUID() { return GetReturn(); }
    ReturnCode_t ReqUserData(quint8 Address) { Q_UNUSED(Address) return GetReturn(); }

    ReturnCode_t DriveToPosition(quint32 TargetPosition, quint8 MotionProfileIdx) { Q_UNUSED(TargetPosition) Q_UNUSED(MotionProfileIdx) return GetReturn(); }
    ReturnCode_t DriveSpeed(quint16 TargetSpeed, quint8 MotionProfileIdx) { Q_UNUSED(TargetSpeed) Q_UNUSED(MotionProfileIdx) return GetReturn(); }

    ReturnCode_t ReqActInputValue(){    return GetReturn(); }

    ReturnCode_t SetOutputValue(quint16 OutputValue) { Q_UNUSED(OutputValue) return GetReturn(); }

    void EmitReportSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
    {
        emit ReportSetStateAckn(InstanceID, HdlInfo);
    }
    void EmitReportReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position)
    {
       emit ReportReferenceMovementAckn(InstanceID, HdlInfo, Position);
    }

    void EmitReportStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
    {
        emit ReportStateAckn(InstanceID, HdlInfo);
    }
    void EmitReportLoginAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
    {
        emit ReportLoginAckn(InstanceID, HdlInfo);
    }
    void EmitReportUID(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 Uid)
    {
        emit ReportUID(InstanceID, HdlInfo, Uid);
    }
    void EmitReportUserData(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Address, quint32 Data)
    {
        emit ReportUserData(InstanceID, HdlInfo, Address, Data);
    }

    void EmitReportActOutputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
    {
        emit ReportActOutputValue(InstanceID, HdlInfo, OutputValue);
    }

    void EmitReportActInputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
    {
        emit ReportActInputValue(InstanceID, HdlInfo, OutputValue);
    }

    void EmitReportMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position, qint16 Speed)
    {
        emit ReportMovementAckn(InstanceID, HdlInfo, Position, Speed);
    }
    void EmitRotate(quint32 Speed,quint8 MotionProfileIdx)
    {
        emit Rotate(Speed, MotionProfileIdx);
    }
    void EmitReportSpeedMovementCmdAckn(quint32 InstanceID, ReturnCode_t HdlInf)
    {
        emit ReportSpeedMovementCmdAckn(InstanceID, HdlInf);
    }
    void EmitReportSpeed(quint32 InstanceID, ReturnCode_t HdlInf, quint16 Speed)
    {
        emit ReportSpeed(InstanceID, HdlInf,Speed);
    }

Q_SIGNALS:
    void ReportSetStateAckn(quint32, ReturnCode_t);
    void ReportReferenceMovementAckn(quint32, ReturnCode_t, qint32);

    void ReportStateAckn(quint32, ReturnCode_t);
    void ReportLoginAckn(quint32, ReturnCode_t);
    void ReportUID(quint32, ReturnCode_t, quint32);
    void ReportUserData(quint32, ReturnCode_t, quint8, quint32);

    void ReportActOutputValue(quint32, ReturnCode_t, quint16);

    void ReportActInputValue(quint32, ReturnCode_t, quint16);

    void ReportMovementAckn(quint32, ReturnCode_t, qint32, qint16);

    void ReportError(quint32, quint16, quint16, quint16, QDateTime);
    void Rotate(quint32, quint8);
    void ReportSpeedMovementCmdAckn(quint32 , ReturnCode_t);
    void ReportSpeed(quint32, ReturnCode_t, quint16);

private:
    ReturnCode_t GetReturn()
    {
        if (m_nextError) {
            m_nextError = false;
            return DCL_ERR_FCT_CALL_FAILED;
        } else {
            return DCL_ERR_FCT_CALL_SUCCESS;
        }
    }

    bool m_nextError;
};


} //namespace

#endif /* DEVICECONTROL_MOCKFM_H */

// vi: set ts=4 sw=4 et:


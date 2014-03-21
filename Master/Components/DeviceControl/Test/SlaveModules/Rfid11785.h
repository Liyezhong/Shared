#ifndef RFID11785_SIM_H
#define RFID11785_SIM_H

#include <QObject>
#include "FunctionModule.h"

namespace DeviceControl
{

class CRfid11785 : public CFunctionModule
{
    Q_OBJECT

public:
    CRfid11785(){

    }

    ReturnCode_t SetState(bool Enabled, quint8 Antenna) { Q_UNUSED(Enabled) Q_UNUSED(Antenna) return GetReturn(); }

    ReturnCode_t SetLogin(quint32 Password) { Q_UNUSED(Password) return GetReturn(); }

    ReturnCode_t ReqUID() { return GetReturn(); }

    ReturnCode_t ReqUserData(quint8 Address) { Q_UNUSED(Address) return GetReturn(); }

    void EmitReportStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
    {
        emit ReportStateAckn(InstanceID, HdlInfo);
        THREAD_PROCESS();
    }
    void EmitReportLoginAckn(quint32 InstanceID, ReturnCode_t HdlInfo)
    {
        emit ReportLoginAckn(InstanceID, HdlInfo);
        THREAD_PROCESS();
    }
    void EmitReportUID(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 Uid)
    {
        emit ReportUID(InstanceID, HdlInfo, Uid);
        THREAD_PROCESS();
    }
    void EmitReportUserData(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 Address, quint32 Data)
    {
        emit ReportUserData(InstanceID, HdlInfo, Address, Data);
        THREAD_PROCESS();
    }

signals:
    void ReportStateAckn(quint32, ReturnCode_t);
    void ReportLoginAckn(quint32, ReturnCode_t);
    void ReportUID(quint32, ReturnCode_t, quint32);
    void ReportUserData(quint32, ReturnCode_t, quint8, quint32);
};

}

#endif // RFID11785_H

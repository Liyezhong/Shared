#ifndef DIGITALOUTPUT_SIM_H
#define DIGITALOUTPUT_SIM_H

#include <QObject>
#include "FunctionModule.h"

namespace DeviceControl
{

class CDigitalOutput : public CFunctionModule
{
    Q_OBJECT

public:
    CDigitalOutput(){

    }

    ReturnCode_t SetOutputValue(quint16 OutputValue)
    {
        Q_UNUSED(OutputValue)

        return GetReturn();
    }

    void EmitReportActOutputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
    {
        emit ReportOutputValueAckn(InstanceID, HdlInfo, OutputValue);
        THREAD_PROCESS();
    }

signals:
    void ReportOutputValueAckn(quint32, ReturnCode_t, quint16);
};

}

#endif // DIGITALOUTPUT_H

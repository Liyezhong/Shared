#ifndef DIGITALINPUT_SIM_H
#define DIGITALINPUT_SIM_H

#include <QObject>

#include "FunctionModule.h"

namespace DeviceControl
{

class CDigitalInput : public CFunctionModule
{
    Q_OBJECT

public:
    CDigitalInput(){

    }

    ReturnCode_t ReqActInputValue(){    return GetReturn(); }

    void EmitReportActInputValue(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 OutputValue)
    {
        emit ReportActInputValue(InstanceID, HdlInfo, OutputValue);
        THREAD_PROCESS();
    }

signals:
    void ReportActInputValue(quint32, ReturnCode_t, quint16);
};

}

#endif // DIGITALINPUT_H

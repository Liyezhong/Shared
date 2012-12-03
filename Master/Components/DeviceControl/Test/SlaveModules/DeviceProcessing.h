#ifndef DEVICEPROCESSING_SIM_H
#define DEVICEPROCESSING_SIM_H

#include <QMap>
#include <QDebug>

#include "FunctionModule.h"
#include "StepperMotor.h"
#include "TemperatureControl.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "Rfid11785.h"

namespace DeviceControl
{

class DeviceProcessing : public QObject
{
    Q_OBJECT

public:
    DeviceProcessing(QObject *p_Parent = NULL) : QObject(p_Parent)
    {
    }

    ~DeviceProcessing()
    {
    }

    //! Return function module specified by it's instance identifier
    CFunctionModule* GetFunctionModule(quint32 InstanceID) const
    {
        if (m_FunctionModule.contains(InstanceID))
        {
            return m_FunctionModule[InstanceID];
        }
        else
        {
            return NULL;
        }
    }

    void EmitShutdown()
    {
        emit DeviceShutdown();
    }

    void InsertFuncModule(quint32 InstanceId, CFunctionModule *FuncModule)
    {
        m_FunctionModule[InstanceId] = FuncModule;
    }

signals:
    void DeviceShutdown();

private:
    QMap<quint32, CFunctionModule*> m_FunctionModule;
};

}

#endif // DEVICEPROCSIMULATOR_H

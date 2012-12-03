#ifndef FUNCTIONMODULE_SIM_H
#define FUNCTIONMODULE_SIM_H

#include <QObject>

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

#include "utGlobal.h"

namespace DeviceControl
{

class CFunctionModule : public QObject
{
    Q_OBJECT

public:
    CFunctionModule() : m_nextError(false)
    {

    }

    void SetNextError()
    {
        m_nextError = true;
    }

    void Reset()
    {
        m_nextError = false;
    }

    void SetModuleHandle(quint32 ModuleHandle)
    {
        m_ModuleHandle = ModuleHandle;
    }

    quint32 GetModuleHandle()
    {
        return m_ModuleHandle;
    }

protected:
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

    quint32 m_ModuleHandle;
};

}

#endif // FUNCTIONMODULE_H

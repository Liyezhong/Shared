#ifndef TEMPERATURECONTROL_SIM_H
#define TEMPERATURECONTROL_SIM_H

#include "FunctionModule.h"

namespace DeviceControl
{

class CTemperatureControl : public CFunctionModule
{
    Q_OBJECT

public:
    CTemperatureControl(){

    }

    ReturnCode_t SetStatus(TempCtrlStatus_t TempCtrlState, qreal Temperature)
    {
        Q_UNUSED(TempCtrlState)
        Q_UNUSED(Temperature)

        return DCL_ERR_FCT_CALL_SUCCESS;
    }

    //! Request actual temperature
    ReturnCode_t ReqActTemperature(quint8 Index)
    {
        Q_UNUSED(Index);

        return DCL_ERR_FCT_CALL_SUCCESS;
    }

};

}
#endif // TEMPERATURECONTROL_H

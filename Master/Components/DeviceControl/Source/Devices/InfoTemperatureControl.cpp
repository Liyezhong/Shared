/****************************************************************************/
/*! \file   InfoTemperatureControl.cpp
 *
 *  \brief  Implementaion file for class CInfoTemperatureControl
 *
 *  \version  0.1
 *  \date     2012-01-31
 *  \author   M.Scherer
 *
 *  \b Description:
 *
 *   N/A
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include "DataManager/Containers/InstrumentHistory/Include/SubModule.h"
#include "DeviceControl/Include/Devices/InfoTemperatureControl.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CInfoTemperatureControl
typedef CSignalTransition<CInfoTemperatureControl> CInfoTemperatureControlTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoTemperatureControl
 *
 *  \iparam p_TemperatureControl = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoTemperatureControl::CInfoTemperatureControl(CTemperatureControl *p_TemperatureControl,
                                                 DataManager::CSubModule *p_SubModule, QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent), mp_TemperatureControl(p_TemperatureControl), mp_SubModule(p_SubModule)
{
    //lint -esym(1524, CInfoTemperatureControl)
    CState *p_Init = new CState("Init", this);
    CState *p_GetHeaterOperatingTime = new CState("GetHeaterOperatingTime", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoTemperatureControlTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoTemperatureControl::GetHeaterOperatingTime,
        p_GetHeaterOperatingTime));
    p_GetHeaterOperatingTime->addTransition(new CInfoTemperatureControlTransition(
        mp_TemperatureControl, SIGNAL(ReportHeaterOperatingTime(quint32, ReturnCode_t, quint8, quint32)),
        *this, &CInfoTemperatureControl::Finished,
        p_Final));

    mp_SubModule->AddParameterInfo("SoftwareVersion", QString());
    mp_SubModule->AddParameterInfo("OperatingTime", "seconds", QString());
    mp_SubModule->AddParameterInfo("OperatingTimeCycle", QString());
}

/****************************************************************************/
/*!
 *  \brief  Requests the life time data from the function module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoTemperatureControl::GetHeaterOperatingTime(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_TemperatureControl->GetHeaterOperatingTime(0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Receives the life time data from the function module
 *
 *  \iparam p_Event = Parameters of the signal ReportHeaterOperatingTime
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoTemperatureControl::Finished(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    quint32 OperatingTime;
    QString Version = QString().setNum(mp_TemperatureControl->GetBaseModule()->GetModuleSWVersion(mp_TemperatureControl->GetType()));

    ReturnCode = CInfoTemperatureControlTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoTemperatureControlTransition::GetEventValue(p_Event, 3, OperatingTime)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", Version)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("OperatingTime", QString().setNum(OperatingTime))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    quint32 lifeCycle = mp_TemperatureControl->GetLifeCycle();
    if (!mp_SubModule->UpdateParameterInfo("OperatingTimeCycle", QString().setNum(lifeCycle))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

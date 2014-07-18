/****************************************************************************/
/*! \file   InfoPressureControl.cpp
 *
 *  \brief  Implementaion file for class CInfoPressureControl
 *
 *  \version  0.1
 *  \date     2014-07-05
 *  \author   A.Yang
 *
 *  \b Description:
 *
 *   N/A
 *
 *  @b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2014 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#include "DataManager/Containers/InstrumentHistory/Include/SubModule.h"
#include "DeviceControl/Include/Devices/InfoPressureControl.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/PressureControl.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CPressureControl
typedef CSignalTransition<CInfoPressureControl> CInfoPressureControlTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoPressureControl
 *
 *  \iparam p_StepperMotor = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoPressureControl::CInfoPressureControl(CPressureControl *p_PressureControl, DataManager::CSubModule *p_SubModule,
                                     QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent),
    mp_PressureControl(p_PressureControl),
    mp_SubModule(p_SubModule)
{
    CState *p_Init = new CState("Init", this);
    CState *p_RequestPumpLifeTimeData = new CState("RequestPumpLifeTimeData", this);
    //CState *p_RequestValveLifeCycleData = new CState("RequestValveLifeCycleData", this);
    //CState *p_RequestFanLifeTimeData = new CState("RequestFanLifeTimeData", this);
    //CState *p_RequestValve2LifeCycleData = new CState("RequestValve2LifeCycleData", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoPressureControlTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoPressureControl::RequestPumpLifeTimeData,
        p_RequestPumpLifeTimeData));

    p_RequestPumpLifeTimeData->addTransition(new CInfoPressureControlTransition(
        mp_PressureControl, SIGNAL(ReportPumpOperatingTime(quint32, ReturnCode_t, quint8, quint32)),
        *this, &CInfoPressureControl::Finished,
        p_Final));

    /*p_RequestValveLifeCycleData->addTransition(new CInfoPressureControlTransition(
        mp_PressureControl, SIGNAL(ReportRefValveState(quint32, ReturnCode_t, quint8, quint8)),
        *this, &CInfoPressureControl::RequestFanLifeTimeData,
        p_RequestFanLifeTimeData));
    */
    mp_SubModule->AddParameterInfo("SoftwareVersion", QString());
    mp_SubModule->AddParameterInfo("PumpOperationTime", "Hours", QString());
    mp_SubModule->AddParameterInfo("Valve1OperationCycle", QString());
    mp_SubModule->AddParameterInfo("Valve2OperationCycle", QString());
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
bool CInfoPressureControl::RequestPumpLifeTimeData(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_PressureControl->GetPumpOperatingTime(0);
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
 *  \iparam p_Event = Parameters of the signal ReportLifeCycleData
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoPressureControl::Finished(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    quint32 OperationTime;

    QString Version = QString().setNum(mp_PressureControl->GetBaseModule()->GetModuleSWVersion(mp_PressureControl->GetType()));

    ReturnCode = CInfoPressureControlTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoPressureControlTransition::GetEventValue(p_Event, 3, OperationTime)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", Version)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("PumpOperationTime", QString().setNum(OperationTime))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("Valve1OperationCycle", QString().setNum(mp_PressureControl->GetValveLifeCycle(0)))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("Valve2OperationCycle", QString().setNum(mp_PressureControl->GetValveLifeCycle(1)))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
   /* if (!mp_SubModule->UpdateParameterInfo("Distance", QString().setNum(StepCounter * mp_StepperMotor->GetCircumference()))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }*/

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

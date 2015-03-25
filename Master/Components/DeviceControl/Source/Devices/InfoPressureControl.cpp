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
 *  \iparam p_PressureControl = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoPressureControl::CInfoPressureControl(CPressureControl *p_PressureControl, DataManager::CSubModule *p_SubModule,
                                     QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent),
    mp_PressureControl(p_PressureControl),
    mp_SubModule(p_SubModule),
    m_LastExhaustFanOperationTime(0)
{
    CState *p_Init = new CState("Init", this);
    CState *p_RequestPumpLifeTimeData = new CState("RequestPumpLifeTimeData", this);

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

    mp_SubModule->AddParameterInfo("SoftwareVersion", QString());
    mp_SubModule->AddParameterInfo("PumpOperationTime", "seconds", QString());
    mp_SubModule->AddParameterInfo("Valve1OperationCycle", QString());
    mp_SubModule->AddParameterInfo("Valve2OperationCycle", QString());
    mp_SubModule->AddParameterInfo("ActiveCarbonFilterLifeTime", "seconds", QString());
    mp_SubModule->AddParameterInfo("ExhaustFanLifeTime", "seconds", QString());
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
    quint32 pumpOperationTime;

    QString Version = QString().setNum(mp_PressureControl->GetBaseModule()->GetModuleSWVersion(mp_PressureControl->GetType()));

    ReturnCode = CInfoPressureControlTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoPressureControlTransition::GetEventValue(p_Event, 3, pumpOperationTime)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", Version)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    //Add the historyData
    quint32 history_Pump_OperationTime = 0;
    PartLifeCycleRecord* pPartLifeCycleRecord = mp_PressureControl->GetPartLifeCycleRecord();
    if (pPartLifeCycleRecord)
        history_Pump_OperationTime = pPartLifeCycleRecord->m_ParamMap.value("History_Pump_OperationTime").toUInt();

    if (!mp_SubModule->UpdateParameterInfo("PumpOperationTime", QString().setNum(pumpOperationTime + history_Pump_OperationTime))) {
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

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    quint32 diffTime = 0;
    if (m_LastExhaustFanOperationTime > 0)
    {
        diffTime = now - m_LastExhaustFanOperationTime;
        diffTime = diffTime / 1000;
    }
    m_LastExhaustFanOperationTime = now;

    quint32 newVal = 0;
    if (mp_PressureControl->GetActiveCarbonFilterLifeTime() == 1)
    {
        newVal = 0 + diffTime;
    }
    else
    {
        newVal = mp_PressureControl->GetActiveCarbonFilterLifeTime() + diffTime;
    }

    mp_PressureControl->SetActiveCarbonFilterLifeTime(newVal);

    newVal = mp_PressureControl->GetExhaustFanLifeTime() + diffTime;
    mp_PressureControl->SetExhaustFanLifeTime(newVal);

    if (!mp_SubModule->UpdateParameterInfo("ActiveCarbonFilterLifeTime", QString().setNum(mp_PressureControl->GetActiveCarbonFilterLifeTime()))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("ExhaustFanLifeTime", QString().setNum(mp_PressureControl->GetExhaustFanLifeTime()))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    //also update the DeviceLifeCycleRecord.xml
    if (!pPartLifeCycleRecord)
        return true;

    QString strLifeCycleNew = QString().setNum(mp_PressureControl->GetValveLifeCycle(0));
    QMap<QString, QString>::const_iterator iter = pPartLifeCycleRecord->m_ParamMap.find("Valve1_LifeCycle");
    if (iter != pPartLifeCycleRecord->m_ParamMap.end())
        pPartLifeCycleRecord->m_ParamMap["Valve1_LifeCycle"] = strLifeCycleNew;

    QString strLifeCycleNew2 = QString().setNum(mp_PressureControl->GetValveLifeCycle(1));
    QMap<QString, QString>::const_iterator iter2 = pPartLifeCycleRecord->m_ParamMap.find("Valve2_LifeCycle");
    if (iter2 != pPartLifeCycleRecord->m_ParamMap.end())
        pPartLifeCycleRecord->m_ParamMap["Valve2_LifeCycle"] = strLifeCycleNew2;

    QString strLifeTimeCarbonFilter = QString().setNum(mp_PressureControl->GetActiveCarbonFilterLifeTime());
    QMap<QString, QString>::const_iterator iterFilter = pPartLifeCycleRecord->m_ParamMap.find("ActiveCarbonFilter_LifeTime");
    if (iterFilter != pPartLifeCycleRecord->m_ParamMap.end())
        pPartLifeCycleRecord->m_ParamMap["ActiveCarbonFilter_LifeTime"] = strLifeTimeCarbonFilter;

    QString strLifeTimeExhaustFan = QString().setNum(mp_PressureControl->GetExhaustFanLifeTime());
    QMap<QString, QString>::const_iterator iterExhaustFan = pPartLifeCycleRecord->m_ParamMap.find("Exhaust_Fan_LifeTime");
    if (iterExhaustFan != pPartLifeCycleRecord->m_ParamMap.end())
        pPartLifeCycleRecord->m_ParamMap["Exhaust_Fan_LifeTime"] = strLifeTimeExhaustFan;

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

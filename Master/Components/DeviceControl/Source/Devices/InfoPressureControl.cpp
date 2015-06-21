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
#include "DeviceControl/Include/Global/dcl_log.h"

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
    //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:Begin";
    ReturnCode_t ReturnCode = mp_PressureControl->GetPumpOperatingTime(0);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:E1";
        emit ReportError(500070601);
        return false;
    }
    //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:End";
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
    //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoPressureControl:Begin";
    ReturnCode_t ReturnCode;
    quint32 pumpOperationTime;

    QString Version = QString().setNum(mp_PressureControl->GetBaseModule()->GetModuleSWVersion(mp_PressureControl->GetType()));

    ReturnCode = CInfoPressureControlTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoPressureControl,E2";
        emit ReportError(500070602);
        return false;
    }
    if (!CInfoPressureControlTransition::GetEventValue(p_Event, 3, pumpOperationTime)) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoPressureControl,E3";
        emit ReportError(500070603);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", Version)) {
        //FILE_LOG_L(laFCT, llDEBUG) << "lifeCycle:CInfoPressureControl,E4";
        emit ReportError(500070604);
        return false;
    }

    //Add the historyData
    quint32 history_Pump_OperationTime = 0;
    PartLifeCycleRecord* pPartLifeCycleRecord = mp_PressureControl->GetPartLifeCycleRecord();
    if (pPartLifeCycleRecord)
        history_Pump_OperationTime = pPartLifeCycleRecord->m_ParamMap.value("History_Pump_OperationTime").toUInt();

    if (!mp_SubModule->UpdateParameterInfo("PumpOperationTime", QString().setNum(pumpOperationTime + history_Pump_OperationTime))) {
        //FILE_LOG_L(laFCT, llDEBUG) << " lifeCycle:CInfoPressureControl,E5";
        emit ReportError(500070605);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("Valve1OperationCycle", QString().setNum(mp_PressureControl->GetValveLifeCycle(0)))) {
        //FILE_LOG_L(laFCT, llDEBUG) << " lifeCycle:CInfoPressureControl,E6";
        emit ReportError(500070606);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("Valve2OperationCycle", QString().setNum(mp_PressureControl->GetValveLifeCycle(1)))) {
        //FILE_LOG_L(laFCT, llDEBUG) << " lifeCycle:CInfoPressureControl,E7";
        emit ReportError(500070607);
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

    quint32 newVal = mp_PressureControl->GetActiveCarbonFilterLifeTime() + diffTime;
    mp_PressureControl->SetActiveCarbonFilterLifeTime(newVal);

    newVal = mp_PressureControl->GetExhaustFanLifeTime() + diffTime;
    mp_PressureControl->SetExhaustFanLifeTime(newVal);

    if (!mp_SubModule->UpdateParameterInfo("ActiveCarbonFilterLifeTime", QString().setNum(mp_PressureControl->GetActiveCarbonFilterLifeTime()))) {
        //FILE_LOG_L(laFCT, llDEBUG) << " lifeCycle:CInfoPressureControl,E8";
        emit ReportError(500070608);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("ExhaustFanLifeTime", QString().setNum(mp_PressureControl->GetExhaustFanLifeTime()))) {
        //FILE_LOG_L(laFCT, llDEBUG) << " lifeCycle:CInfoPressureControl,E9";
        emit ReportError(500070609);
        return false;
    }

    //also update the DeviceLifeCycleRecord.xml
    if (!pPartLifeCycleRecord)
    {

        return true;
    }

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

    pPartLifeCycleRecord->m_ParamMap["CarbonFilter_FirstRecord_Flag"] = "0";

    QString strLifeTimeExhaustFan = QString().setNum(mp_PressureControl->GetExhaustFanLifeTime());
    QMap<QString, QString>::const_iterator iterExhaustFan = pPartLifeCycleRecord->m_ParamMap.find("Exhaust_Fan_LifeTime");
    if (iterExhaustFan != pPartLifeCycleRecord->m_ParamMap.end())
        pPartLifeCycleRecord->m_ParamMap["Exhaust_Fan_LifeTime"] = strLifeTimeExhaustFan;
    //FILE_LOG_L(laFCT, llDEBUG) << " lifeCycle:CInfoPressureControl,End";
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

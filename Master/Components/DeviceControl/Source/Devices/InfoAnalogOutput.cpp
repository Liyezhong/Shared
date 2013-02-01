/****************************************************************************/
/*! \file   InfoAnalogOutput.cpp
 *
 *  \brief  Implementaion file for class CInfoAnalogOutput
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
#include "DeviceControl/Include/Devices/InfoAnalogOutput.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CInfoAnalogOutput
typedef CSignalTransition<CInfoAnalogOutput> CInfoAnalogOutputTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoAnalogOutput
 *
 *  \iparam p_AnalogOutput = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoAnalogOutput::CInfoAnalogOutput(CAnalogOutput *p_AnalogOutput, DataManager::CSubModule *p_SubModule,
                                     QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent), mp_AnalogOutput(p_AnalogOutput), mp_SubModule(p_SubModule)
{
    CState *p_Init = new CState("Init", this);
    CState *p_ReqLifeTimeData = new CState("ReqLifeTimeData", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoAnalogOutputTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoAnalogOutput::ReqLifeTimeData,
        p_ReqLifeTimeData));
    p_ReqLifeTimeData->addTransition(new CInfoAnalogOutputTransition(
        mp_AnalogOutput, SIGNAL(ReportLifeTimeData(quint32, ReturnCode_t, quint32, quint32)),
        *this, &CInfoAnalogOutput::Finished,
        p_Final));

    mp_SubModule->AddParameterInfo("SoftwareVersion", QString());
    mp_SubModule->AddParameterInfo("OnTime", QString(), "minutes");
    mp_SubModule->AddParameterInfo("OffToOnCycles", QString());
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
bool CInfoAnalogOutput::ReqLifeTimeData(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_AnalogOutput->ReqLifeTimeData();
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
 *  \iparam p_Event = Parameters of the signal ReportLifeTimeData
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoAnalogOutput::Finished(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    quint32 LifeTime;
    quint32 LifeCycles;
    QString Version = QString().setNum(mp_AnalogOutput->GetBaseModule()->GetModuleSWVersion(mp_AnalogOutput->GetType()));

    ReturnCode = CInfoAnalogOutputTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoAnalogOutputTransition::GetEventValue(p_Event, 2, LifeTime)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoAnalogOutputTransition::GetEventValue(p_Event, 3, LifeCycles)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    if (!mp_SubModule->UpdateParameterInfo("SoftwareVersion", Version)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("OnTime", QString().setNum(LifeTime))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!mp_SubModule->UpdateParameterInfo("OffToOnCycles", QString().setNum(LifeCycles))) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

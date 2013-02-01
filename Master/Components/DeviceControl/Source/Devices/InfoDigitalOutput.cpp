/****************************************************************************/
/*! \file   InfoDigitalOutput.cpp
 *
 *  \brief  Implementaion file for class CInfoDigitalOutput
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
#include "DeviceControl/Include/Devices/InfoDigitalOutput.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CInfoDigitalOutput
typedef CSignalTransition<CInfoDigitalOutput> CInfoDigitalOutputTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoDigitalOutput
 *
 *  \iparam p_DigitalOutput = Function module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoDigitalOutput::CInfoDigitalOutput(CDigitalOutput *p_DigitalOutput, DataManager::CSubModule *p_SubModule,
                                       QState *p_Parent) :
    CState(p_SubModule->GetSubModuleName(), p_Parent), mp_DigitalOutput(p_DigitalOutput), mp_SubModule(p_SubModule)
{
    CState *p_Init = new CState("Init", this);
    CState *p_ReqLifeTimeData = new CState("ReqLifeTimeData", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoDigitalOutputTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoDigitalOutput::ReqLifeTimeData,
        p_ReqLifeTimeData));
    p_ReqLifeTimeData->addTransition(new CInfoDigitalOutputTransition(
        mp_DigitalOutput, SIGNAL(ReportLifeTimeData(quint32, ReturnCode_t, quint32, quint32)),
        *this, &CInfoDigitalOutput::Finished,
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
bool CInfoDigitalOutput::ReqLifeTimeData(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_DigitalOutput->ReqLifeTimeData();
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
bool CInfoDigitalOutput::Finished(QEvent *p_Event)
{
    ReturnCode_t ReturnCode;
    quint32 LifeTime;
    quint32 LifeCycles;
    QString Version = QString().setNum(mp_DigitalOutput->GetBaseModule()->GetModuleSWVersion(mp_DigitalOutput->GetType()));

    ReturnCode = CInfoDigitalOutputTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }
    if (!CInfoDigitalOutputTransition::GetEventValue(p_Event, 2, LifeTime)) {
        emit ReportError(DCL_ERR_INVALID_PARAM);
        return false;
    }
    if (!CInfoDigitalOutputTransition::GetEventValue(p_Event, 3, LifeCycles)) {
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

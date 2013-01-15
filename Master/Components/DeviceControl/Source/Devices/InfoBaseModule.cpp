/****************************************************************************/
/*! \file   InfoBaseModule.cpp
 *
 *  \brief  Implementaion file for class CInfoBaseModule
 *
 *  \version  0.1
 *  \date     2012-01-07
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

#include "DeviceControl/Include/Devices/InfoBaseModule.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CInfoBaseModule
typedef CSignalTransition<CInfoBaseModule> CInfoBaseModuleTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CInfoBaseModule
 *
 *  \iparam p_BaseModule = Base module used for communication
 *  \iparam p_SubModule = The data is stored here
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CInfoBaseModule::CInfoBaseModule(CBaseModule *p_BaseModule, DataManager::CSubModule *p_SubModule,
                                       const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent), mp_BaseModule(p_BaseModule), mp_SubModule(p_SubModule)
{
    CState *p_Init = new CState("Init", this);
    CState *p_ReqSerialNumber = new CState("ReqSerialNumber", this);
    //CState *p_ReqEndTestResult = new CState("ReqEndTestResult", this);
    QFinalState *p_ReqEndTestResult = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CInfoBaseModuleTransition(
        p_Init, SIGNAL(entered()),
        *this, &CInfoBaseModule::ReqSerialNumber,
        p_ReqSerialNumber));
    p_ReqSerialNumber->addTransition(new CInfoBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportSerialNumber(quint32, ReturnCode_t, QString)),
        *this, &CInfoBaseModule::ReqEndTestResult,
        p_ReqEndTestResult));
}

/****************************************************************************/
/*!
 *  \brief  Requests the serial number from the base module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqSerialNumber(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_BaseModule->ReqSerialNumber();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Requests the end test result from the base module
 *
 *  \iparam p_Event = Parameters of the signal ReportSerialNumber
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CInfoBaseModule::ReqEndTestResult(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CInfoBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

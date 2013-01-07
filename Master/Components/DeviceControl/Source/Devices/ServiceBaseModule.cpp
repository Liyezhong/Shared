/****************************************************************************/
/*! \file   ServiceBaseModule.cpp
 *
 *  \brief  Implementaion file for class CServiceBaseModule
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

#include "DeviceControl/Include/Devices/ServiceBaseModule.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CServiceBaseModule
typedef CSignalTransition<CServiceBaseModule> CServiceBaseModuleTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CServiceBaseModule
 *
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CServiceBaseModule::CServiceBaseModule(CBaseModule *p_BaseModule, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent), mp_BaseModule(p_BaseModule)
{
    CState *p_Init = new CState("Init", this);
    CState *p_ReqSerialNumber = new CState("ReqSerialNumber", this);
    //CState *p_ReqEndTestResult = new CState("ReqEndTestResult", this);
    QFinalState *p_ReqEndTestResult = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CServiceBaseModuleTransition(
        p_Init, SIGNAL(entered()),
        *this, &CServiceBaseModule::ReqSerialNumber,
        p_ReqSerialNumber));
    p_ReqSerialNumber->addTransition(new CServiceBaseModuleTransition(
        mp_BaseModule, SIGNAL(ReportSerialNumber(quint32, ReturnCode_t, QString)),
        *this, &CServiceBaseModule::ReqEndTestResult,
        p_ReqEndTestResult));
}

bool CServiceBaseModule::ReqSerialNumber(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_BaseModule->ReqSerialNumber();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

bool CServiceBaseModule::ReqEndTestResult(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = CServiceBaseModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

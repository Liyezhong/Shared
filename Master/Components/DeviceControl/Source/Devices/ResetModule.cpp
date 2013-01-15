/****************************************************************************/
/*! \file   ResetModule.cpp
 *
 *  \brief  Implementaion file for class CResetModule
 *
 *  \version  0.1
 *  \date     2012-01-15
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

#include "DeviceControl/Include/Devices/ResetModule.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/Module.h"
#include <QFinalState>

namespace DeviceControl
{

//! Signal transition for CResetModule
typedef CSignalTransition<CResetModule> CResetModuleTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CResetModule
 *
 *  \iparam p_Module = Module whose data is reset
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CResetModule::CResetModule(CModule *p_Module, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent), mp_Module(p_Module)
{
    CState *p_Init = new CState("Init", this);
    CState *p_ReqDataReset = new CState("ReqDataReset", this);
    QFinalState *p_Final = new QFinalState(this);
    setInitialState(p_Init);

    p_Init->addTransition(new CResetModuleTransition(
        p_Init, SIGNAL(entered()),
        *this, &CResetModule::ReqDataReset,
        p_ReqDataReset));
    p_ReqDataReset->addTransition(new CResetModuleTransition(
        mp_Module, SIGNAL(ReportDataResetAckn(quint32, ReturnCode_t)),
        *this, &CResetModule::ReportDataResetAckn,
        p_Final));
}

/****************************************************************************/
/*!
 *  \brief  Sends a data reset request command to the module
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CResetModule::ReqDataReset(QEvent *p_Event)
{
    Q_UNUSED(p_Event)

    ReturnCode_t ReturnCode = mp_Module->ReqDataReset();
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Receives a data reset acknowledgement
 *
 *  \iparam p_Event = Parameters of the signal ReportDataResetAckn
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CResetModule::ReportDataResetAckn(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CResetModuleTransition::GetEventValue(p_Event, 1);
    if (DCL_ERR_FCT_CALL_SUCCESS != ReturnCode) {
        emit ReportError(ReturnCode);
        return false;
    }

    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

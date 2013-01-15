/****************************************************************************/
/*! \file   ServiceInfo.cpp
 *
 *  \brief  Implementaion file for class CServiceInfo
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

#include "DeviceControl/Include/Devices/InfoBaseModule.h"
#include "DeviceControl/Include/Devices/ServiceInfo.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"

namespace DeviceControl
{

//! Signal transition for CServiceInfo
typedef CSignalTransition<CServiceInfo> CServiceInfoTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CServiceInfo
 *
 *  \iparam ModuleMap = Slave modules that are part of the device
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CServiceInfo::CServiceInfo(QMap<QString, CModule *> &ModuleMap, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent)
{
    CState *p_Idle = new CState("Idle", this);
    CState *p_Active = new CState("Active", this);
    CState *p_LastState = NULL;
    setInitialState(p_Idle);

    addTransition(new CServiceInfoTransition(
        this, SIGNAL(GetServiceInfo()),
        *this, &CServiceInfo::ReportActive));

    p_Idle->addTransition(this, SIGNAL(GetServiceInfo()), p_Active);
    p_Active->addTransition(new CServiceInfoTransition(
        this, SIGNAL(OnReportError(ReturnCode_t)),
        *this, &CServiceInfo::ReportError,
        p_Idle));

    QMapIterator<QString, CModule *> Iterator(ModuleMap);
    while (Iterator.hasNext()) {
        CState *p_NewState = NULL;
        Iterator.next();

        CBaseModule *p_BaseModule = dynamic_cast<CBaseModule *>(Iterator.value());
        if(p_BaseModule != NULL) {
            DataManager::CSubModule *p_SubModule = new DataManager::CSubModule();

            p_SubModule->SetSubModuleName(p_BaseModule->GetKey());
            m_ModuleInformation.AddSubModuleInfo(p_SubModule);

            p_NewState = new CInfoBaseModule(p_BaseModule, p_SubModule, p_BaseModule->GetKey(), p_Active);
            connect(p_NewState, SIGNAL(ReportError(ReturnCode_t)), this, SIGNAL(OnReportError(ReturnCode_t)));
        }

        if (p_NewState != NULL) {
            if (p_LastState == NULL) {
                p_Active->setInitialState(p_NewState);
            }
            else {
                p_LastState->addTransition(p_LastState, SIGNAL(finished()), p_NewState);
            }
            p_LastState = p_NewState;
        }
    }

    p_Active->addTransition(new CServiceInfoTransition(
        p_LastState, SIGNAL(finished()),
        *this, &CServiceInfo::ReportSuccess,
        p_Idle));
}

/****************************************************************************/
/*!
 *  \brief  Emitted when the information was fetched successfully
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CServiceInfo::ReportSuccess(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportGetServiceInfo(DCL_ERR_FCT_CALL_SUCCESS, m_ModuleInformation);
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Emitted when an error occurs during the transactions
 *
 *  \iparam p_Event = Parameters of the signal OnReportError
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CServiceInfo::ReportError(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CServiceInfoTransition::GetEventValue(p_Event, 0);
    emit ReportGetServiceInfo(ReturnCode, m_ModuleInformation);
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Emitted when the request is issued while another one is active
 *
 *  \iparam p_Event = Unused
 *
 *  \return Transition should be performed or not.
 */
/****************************************************************************/
bool CServiceInfo::ReportActive(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportGetServiceInfo(DCL_ERR_INVALID_STATE, m_ModuleInformation);
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

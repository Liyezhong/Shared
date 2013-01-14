/****************************************************************************/
/*! \file   ServiceInformation.cpp
 *
 *  \brief  Implementaion file for class CServiceInformation
 *
 *  \version  0.1
 *  \date     2012-01-04
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
#include "DeviceControl/Include/Devices/ServiceInformation.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"

namespace DeviceControl
{

//! Signal transition for CServiceInformation
typedef CSignalTransition<CServiceInformation> CServiceInformationTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CServiceInformation
 *
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CServiceInformation::CServiceInformation(QMap<QString, CModule *> &ModuleMap, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent)
{
    CState *p_Idle = new CState("Idle", this);
    CState *p_Active = new CState("Active", this);
    CState *p_LastState = NULL;
    setInitialState(p_Idle);

    p_Idle->addTransition(this, SIGNAL(GetServiceInformation()), p_Active);
    p_Active->addTransition(new CServiceInformationTransition(
                                this, SIGNAL(OnReportError(ReturnCode_t)),
                                *this, &CServiceInformation::ReportError,
                                p_Idle));

    connect(this, SIGNAL(OnReportError(ReturnCode_t)), this, SIGNAL(ReportGetServiceInformation(ReturnCode_t)));

    QMapIterator<QString, CModule *> Iterator(ModuleMap);
    while (Iterator.hasNext()) {
        CState *p_NewState = NULL;
        Iterator.next();

        CBaseModule *p_BaseModule = dynamic_cast<CBaseModule *>(Iterator.value());
        if(p_BaseModule != NULL) {
            DataManager::CSubModule *p_SubModule = new DataManager::CSubModule();
            m_ModuleInformation.AddSubModuleInfo(p_SubModule);

            p_NewState = new CServiceBaseModule(p_BaseModule, p_SubModule, Iterator.key(), p_Active);
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

    p_Active->addTransition(new CServiceInformationTransition(
        p_LastState, SIGNAL(finished()),
        *this, &CServiceInformation::ReportSuccess,
        p_Idle));
}

bool CServiceInformation::ReportSuccess(QEvent *p_Event)
{
    Q_UNUSED(p_Event)
    emit ReportGetServiceInformation(DCL_ERR_FCT_CALL_SUCCESS, m_ModuleInformation);
    return true;
}

bool CServiceInformation::ReportError(QEvent *p_Event)
{
    ReturnCode_t ReturnCode = CServiceInformationTransition::GetEventValue(p_Event, 0);
    emit ReportGetServiceInformation(ReturnCode, m_ModuleInformation);
    return true;
}

} //namespace

// vi: set ts=4 sw=4 et:

/****************************************************************************/
/*! \file   ServiceState.cpp
 *
 *  \brief  Implementaion file for class CServiceState
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
#include "DeviceControl/Include/Devices/ServiceState.h"
#include "DeviceControl/Include/Devices/ServiceInfo.h"
#include "DeviceControl/Include/Devices/ServiceReset.h"
#include "DeviceControl/Include/Devices/SignalTransition.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"

namespace DeviceControl
{

//! Signal transition for CServiceState
typedef CSignalTransition<CServiceState> CServiceStateTransition;

/****************************************************************************/
/*!
 *  \brief  Constructor of class CServiceState
 *
 *  \iparam ModuleMap = Slave modules that are part of the device
 *  \iparam Name = Name of the state for debugging outputs
 *  \iparam p_Parent = Parent state
 */
/****************************************************************************/
CServiceState::CServiceState(QMap<QString, CModule *> &ModuleMap, const QString &Name, QState *p_Parent) :
    CState(Name, p_Parent)
{
    setChildMode(QState::ParallelStates);

    CServiceInfo *p_Info = new CServiceInfo(ModuleMap, "Info", this);
    CServiceReset *p_Reset = new CServiceReset(ModuleMap, "Reset", this);

    connect(this, SIGNAL(GetServiceInfo()), p_Info, SIGNAL(GetServiceInfo()));
    connect(p_Info, SIGNAL(ReportGetServiceInfo(ReturnCode_t, DataManager::CModule)),
            this, SIGNAL(ReportGetServiceInfo(ReturnCode_t,DataManager::CModule)));

    connect(this, SIGNAL(ResetServiceInfo()), p_Reset, SIGNAL(ResetServiceInfo()));
    connect(p_Reset, SIGNAL(ReportResetServiceInfo(ReturnCode_t)),
            this, SIGNAL(ReportResetServiceInfo(ReturnCode_t)));
}

} //namespace

// vi: set ts=4 sw=4 et:

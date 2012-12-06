/****************************************************************************/
/*! \file FunctionModule.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class FunctionModule
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CFunctionModule
 *
 *  \iparam eObjectType = Object type
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to CAN communication object
 *  \iparam pParentNode = pointer to CANNode, where this module is assigned to
 */
/****************************************************************************/
CFunctionModule::CFunctionModule(CModuleConfig::CANObjectType_t eObjectType,
                                 const CANMessageConfiguration* p_MessageConfiguration,
                                 CANCommunicator* pCANCommunicator, CBaseModule* pParentNode) :
    CModule(eObjectType, p_MessageConfiguration, pCANCommunicator),
    m_mainState(FM_MAIN_STATE_UNDEF),
    m_pParent(pParentNode)
{
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CFunctionModule
 */
/****************************************************************************/
CFunctionModule::~CFunctionModule()
{
    m_pParent = 0;
}

/****************************************************************************/
/*!
 *  \brief  Returns the node ID of the function module's node
 *
 *  \return Node ID
 */
/****************************************************************************/
quint32 CFunctionModule::GetNodeID() const
{
    return m_pParent->GetNodeID();
}

} //namespace

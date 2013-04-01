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
CFunctionModule::CFunctionModule(CModuleConfig::CANObjectType_t eObjectType, const CANMessageConfiguration* p_MessageConfiguration,
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
 *  \brief  Initialize the module's event CAN message IDs
 *
 *      The CAN-IDs are read from the CAN-Message configuration class.
 *      The CAN-ID for event notification are determined here
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CFunctionModule::InitializeEventCANMessages(quint8 ModuleID)
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bIfaceID;

    if(m_pCANObjectConfig == 0)
    {
        return DCL_ERR_NULL_PTR_ACCESS;
    }

    bIfaceID = m_pCANObjectConfig->m_sChannel;

    m_unCanIDEventInfo       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "EventInfo", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDEventWarning    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "EventWarning", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDEventError      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "EventError", bIfaceID, m_pParent->GetNodeID());
    m_unCanIDEventFatalError = mp_MessageConfiguration->GetCANMessageID(ModuleID, "EventFatalError", bIfaceID, m_pParent->GetNodeID());

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Register the receive event CAN-messages to communication layer
 *
 *      Each receiveable CAN-message must be registered to the communication
 *      layer. This enables the communication layer to call the
 *      'HandleCANMessage(..)' function of this instance after receiption of
 *      the message.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CFunctionModule::RegisterEventCANMessages()
{
    ReturnCode_t RetVal;

    RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDEventInfo, this);
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDEventWarning, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDEventError, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDEventFatalError, this);
    }

    return RetVal;
}

} //namespace

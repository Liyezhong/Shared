/****************************************************************************/
/*! \file Uart.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 22.12.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANUART
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

#include "DeviceControl/Include/SlaveModules/OtherModule.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief    Constructor for the CANUART
 *
 *  \param    p_MessageConfiguration = Message configuration
 *  \param    pCANCommunicator = pointer to communication class
 *  \param    pParentNode = pointer to CANNode, where this module is assigned to
 *
 ****************************************************************************/
COtherModule::COtherModule(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
             CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_OTHER, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_LifeTime(0)
{
    m_mainState = FM_MAIN_STATE_BOOTUP;
}

/****************************************************************************/
/*!
 *  \brief    Destructor of CANUART
 *
 ****************************************************************************/
COtherModule::~COtherModule()
{
    /// \todo Auto-generated destructor stub
}

/****************************************************************************/
/*!
 *  \brief    Initialize this function module
 *
 *   The CAN-IDs are read from the CAN-Message configuration class, and the CAN-ID are composed
 *   Receiveable CAN-message are registered to the communication layer
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t COtherModule::Initialize()
{
    ReturnCode_t RetVal;

    RetVal = InitializeCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = RegisterCANMessages();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_mainState = FM_MAIN_STATE_INIT;
            FILE_LOG_L(laINIT, llDEBUG) << " function module initialized: " << GetName().toStdString();
        }
    }

    return RetVal;

}

/****************************************************************************/
/*!
 *  \brief    Initialize the module's CAN message IDs
 *
 *   The CAN-IDs are read from the CAN-Message configuration class.
 *   The CAN-ID is composed by the message key, the function
 *   module's channel and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t COtherModule::InitializeCANMessages()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief    Register the receive CAN-messages to communication layer
 *
 *   Each receiveable CAN-message must be registered to the communication layer.
 *   This enables the communication layer to call the 'HandelCANMessage(..)' function of this
 *   instance after receiption of the message.
 *
 *  \return   DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t COtherModule::RegisterCANMessages()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine
 *
 *   Depending on the active main state, the appropriate task function will be called.
 *
 *  \return   void
 */
/****************************************************************************/
void COtherModule::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);
    if (m_mainState == FM_MAIN_STATE_INIT)
    {
        m_mainState = FM_MAIN_STATE_CONFIRMED;
    }
    else if(m_mainState == FM_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIRMED)
    {
        m_mainState = FM_MAIN_STATE_CONFIG;
    }
    else if(m_mainState == FM_MAIN_STATE_CONFIG)
    {
        m_mainState = FM_MAIN_STATE_IDLE;
        m_TaskID = MODULE_TASKID_FREE;
    }
    m_mainState = FM_MAIN_STATE_IDLE;
}



/****************************************************************************/
/*!
 *  \brief    Handles the function module's state machine in idle main state
 *
 *   Depending on the active task ID, the appropriate task function will be called.
 */
/****************************************************************************/
void COtherModule::HandleIdleState()
{

}

void COtherModule::HandleCanMessage(can_frame* pCANframe)
{
    Q_UNUSED(pCANframe);
}




}//namespace

/******************************************************************/
/*! \file ShutdownService.cpp
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 28.07.2011
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This file contains the shutdown service class implementation
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/******************************************************************/

#include <QTextStream>
#include <QDomNode>
#include <QDomElement>

#include "DeviceControl/Include/DeviceProcessing/ShutdownService.h"
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor for the CConfigurationService
 *
 *
 *  \iparam pDeviceProcessing = Pointer to DeviceProcessing
 *  \iparam pCANCommunicator = Pointer to communication interface
 *  \iparam pObjectTree = Pointer to CAN-object container
 */
/****************************************************************************/
CShutdownService::CShutdownService(DeviceProcessing* pDeviceProcessing,
                                   CANCommunicator* pCANCommunicator,
                                   ObjectTree* pObjectTree) :
    m_pDeviceProcessing(pDeviceProcessing),
    m_pCANCommunicator(pCANCommunicator),
    m_pObjectTree(pObjectTree),
    m_LastErrorHdlInfo(DCL_ERR_FCT_CALL_SUCCESS),
    m_LastErrorGroup(0),
    m_LastErrorCode(0),
    m_LastErrorData(0),
    m_StateTimespan(0)
{
    m_MainState = CS_MAIN_STATE_INIT;
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CShutdownService
 */
/****************************************************************************/
CShutdownService::~CShutdownService()
{
    m_pDeviceProcessing = 0;
    m_pCANCommunicator = 0;
    m_pObjectTree = 0;
}

/****************************************************************************/
/*!
 *  \brief  Handles the tasks of the shutdown service
 */
/****************************************************************************/
void CShutdownService::HandleTasks()
{
    if(m_MainState == CS_MAIN_STATE_INIT)
    {
        m_MainState = CS_MAIN_STATE_REQ_NODE_STATE;
    }
    else if(m_MainState == CS_MAIN_STATE_REQ_NODE_STATE)
    {
        ReturnCode_t RetVal;

        FILE_LOG_L(laDIAG, llINFO) << " CShutdownService, m_MainState == CS_MAIN_STATE_REQ_NODE_STATE";
        RetVal = RequestNodeStateShutdown();
        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDIAG, llERROR) << " CShutdownService, switch to shutdown mode";
            m_MainState = CS_MAIN_STATE_ERROR;
        }
        else
        {
            m_MainState = CS_MAIN_STATE_WAIT_NODE_STATES;
        }
    }
    else if(m_MainState == CS_MAIN_STATE_WAIT_NODE_STATES)
    {
        ReturnCode_t RetVal;

        FILE_LOG_L(laDIAG, llINFO) << " CShutdownService, m_MainState == CS_MAIN_STATE_WAIT_NODE_STATES";

        RetVal = CheckNodeState();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_MainState = CS_MAIN_STATE_FINISHED;
        }
        else if(RetVal == DCL_ERR_TIMEOUT)
        {
            FILE_LOG_L(laDIAG, llERROR) << " CShutdownService, timeout when waiting for nodes to shutdown";
            m_MainState = CS_MAIN_STATE_FINISHED;
        }

        else if(RetVal == DCL_ERR_INVALID_STATE)
        {
            ; //still waiting for finishing the CAN object configuration
        }
        else
        {
            // at least one Node is in error mode
            FILE_LOG_L(laINIT, llINFO) << " Set CDiagnosticService main state to ERROR";
            m_LastErrorHdlInfo = RetVal;
            m_MainState = CS_MAIN_STATE_ERROR;
            //m_ErrSubState = CS_SUBSTATE_ERR_INIT;
        }
    }
    else if(m_MainState == CS_MAIN_STATE_FINISHED)
    {
        ;
    }
}

/****************************************************************************/
/*!
 *  \brief  Set the node state of all CAN-nodes in object tree to NODE_STATE_SERVICE
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS in successfully read, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CShutdownService::RequestNodeStateShutdown()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CBaseModule* pCANNode;

    pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(true);
    while (pCANNode)
    {
        if (pCANNode->GetMainState() == CBaseModule::CN_MAIN_STATE_IDLE) {
            RetVal = pCANNode->SetNodeState(NODE_STATE_SHUTDOWN);
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
            {
                FILE_LOG_L(laDIAG, llERROR) << " CShutdownService, SetNodeState to NODE_STATE_SHUTDOWN failed; " << pCANNode->GetKey().toStdString();
                return RetVal;
            }
        }
        pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(false);
    }

    return RetVal;
}

/******************************************************************/
/*!
 *  \brief  Check the node state
 *
 *      All nodes are checked for their nodestate was set to SHUTDOWN.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS in successfully read, otherwise error code
 */
 /******************************************************************/
ReturnCode_t CShutdownService::CheckNodeState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    static quint8 Counter = 0;
    NodeState_t NodeState;
    CBaseModule* pCANNode;

    pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(true);
    while (pCANNode)
    {
        NodeState = pCANNode->GetNodeState();
        if(pCANNode->GetMainState() == CBaseModule::CN_MAIN_STATE_IDLE && NodeState != NODE_STATE_STANDBY)
        {
            // There is at least one node not in service state, set this node to the exchange variable
            RetVal = DCL_ERR_INVALID_STATE;
            Counter++;
            if(Counter > 200)
            {
                if(pCANNode)
                {
                    FILE_LOG_L(laDIAG, llWARNING) << " CShutdownService: At least " <<  pCANNode->GetKey().toStdString() <<
                                                     " not idle, state: " << (int) pCANNode->GetMainState();
                }
                else
                {
                    FILE_LOG_L(laDIAG, llWARNING) << " CShutdownService: At least 'unknown node'' not idle";
                }
                RetVal = DCL_ERR_TIMEOUT;
            }
            break;
        }
        pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(false);
    }

    return RetVal;
}

} //namespace

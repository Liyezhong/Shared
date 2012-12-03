/******************************************************************/
/*! \file DiagnosticService.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CDiagnosticService
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
/******************************************************************/

#include "DeviceControl/Include/DeviceProcessing/DiagnosticService.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/SlaveModules/DigitalInput.h"
#include "DeviceControl/Include/SlaveModules/DigitalOutput.h"
#include "DeviceControl/Include/SlaveModules/AnalogInput.h"
#include "DeviceControl/Include/SlaveModules/AnalogOutput.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "DeviceControl/Include/SlaveModules/Rfid11785.h"
#include "DeviceControl/Include/SlaveModules/Rfid15693.h"
#include "DeviceControl/Include/SlaveModules/TemperatureControl.h"
#include "DeviceControl/Include/Global/dcl_log.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  Constructor
 *
 *  \iparam pDeviceProcessing = Pointer to the the and only DeviceProcessing instance
 *  \iparam pCANCommunicator = Pointer to the CAN bus communication object
 *  \iparam pObjectTree = Pointer to the object tree, for nodes and function module access
 */
/****************************************************************************/
CDiagnosticService::CDiagnosticService(DeviceProcessing* pDeviceProcessing,
                                       CANCommunicator* pCANCommunicator,
                                       ObjectTree* pObjectTree) :
    m_pDeviceProcessing(pDeviceProcessing), m_pCANCommunicator(pCANCommunicator), m_pObjectTree(pObjectTree),
    m_LastErrorHdlInfo(DCL_ERR_FCT_CALL_SUCCESS), m_LastErrorGroup(0), m_LastErrorCode(0), m_LastErrorData(0)

{
    m_MainState = DIAG_MAIN_STATE_INIT;
}

/****************************************************************************/
/*!
 *  \brief  Destructor
 */
/****************************************************************************/
CDiagnosticService::~CDiagnosticService()
{
    m_pDeviceProcessing = 0;
    m_pCANCommunicator = 0;
    m_pObjectTree = 0;
}

/****************************************************************************/
/*!
 *  \brief  Task handling function
 */
/****************************************************************************/
void CDiagnosticService::HandleTasks()
{
    Q_UNUSED(m_pDeviceProcessing);
    Q_UNUSED(m_pCANCommunicator);
    Q_UNUSED(m_pObjectTree);

    ReturnCode_t RetVal;

    if(m_MainState == DIAG_MAIN_STATE_INIT)
    {
        FILE_LOG_L(laDIAG, llINFO) << " CDiagnosticService, m_MainState == DIAG_MAIN_STATE_INIT";
        RetVal = RequestNodeStateAssembly();
        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDIAG, llERROR) << " CDiagnosticService, switch to error mode";
            m_MainState = DIAG_MAIN_STATE_ERROR;
        }
        else
        {
            m_MainState = DIAG_MAIN_STATE_CONFIG;
        }
    }
    if(m_MainState == DIAG_MAIN_STATE_CONFIG)
    {
        HandleConfigurationTask();
    }
}

/****************************************************************************/
/*!
 *  \brief  Handling the configuration task
 */
/****************************************************************************/
void CDiagnosticService::HandleConfigurationTask()
{
    ReturnCode_t RetVal;

    FILE_LOG_L(laDIAG, llINFO) << " CDiagnosticService, m_MainState == DIAG_MAIN_STATE_CONFIG";

    RetVal = CheckNodeState();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        m_MainState = DIAG_MAIN_STATE_IDLE;
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
        m_MainState = DIAG_MAIN_STATE_ERROR;
        //m_ErrSubState = CS_SUBSTATE_ERR_INIT;
    }
}

/****************************************************************************/
/*!
 *  \brief  Set the node state of all CAN-nodes in object tree to NODE_STATE_ASSEMBLY
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS in successfully read, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CDiagnosticService::RequestNodeStateAssembly()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    CBaseModule* pCANNode;

    pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(true);
    while (pCANNode)
    {

        RetVal = pCANNode->SetNodeState(NODE_STATE_ASSEMBLY);
        if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
        {
            FILE_LOG_L(laDIAG, llERROR) << " CDiagnosticService, SetNodeState to NODE_STATE_ASSEMBLY failed; " << pCANNode->GetKey().toStdString();
            return RetVal;
        }
        pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(false);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Check the node state
 *
 *      All nodes are checked for their nodestate was set to SERVICE
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS in successfully read, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CDiagnosticService::CheckNodeState()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    static quint8 Counter = 0;
    NodeState_t NodeState;
    CBaseModule* pCANNode;

    pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(true);
    while (pCANNode)
    {
        NodeState = pCANNode->GetNodeState();
        if(NodeState != NODE_STATE_ASSEMBLY)
        {
            //there is at least one node not in service state, set this node to the exchange variable
            RetVal = DCL_ERR_INVALID_STATE;
            Counter++;
            if(Counter > 200)
            {
                if(pCANNode)
                {
                    FILE_LOG_L(laDIAG, llWARNING) << "  At least " <<  pCANNode->GetKey().toStdString() << " not idle, state: " << (int) pCANNode->GetMainState();
                }
                else
                {
                    FILE_LOG_L(laDIAG, llWARNING) << "  At least 'unknown node'' not idle";
                }
                Counter = 0;
            }
            break;
        }
        pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(false);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Forwards the error information from a function module to DeviceProcessing class
 *
 *      The function forwards the error information to the DeviceProcessing class
 *      (which finally throws the signal assigned to the errors)
 *
 *  \iparam InstanceID = The instance identifier of the module which brought up the error
 *  \iparam ErrorGroup = Error group ID of the thrown error
 *  \iparam ErrorID = Error ID of the thrown error
 *  \iparam ErrorData = Additional error information
 *  \iparam ErrorTime = Time of error detection
 */
/****************************************************************************/
void CDiagnosticService::ThrowErrorSignal(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime ErrorTime)
{
    m_pDeviceProcessing->ThrowError(InstanceID, ErrorGroup, ErrorID, ErrorData, ErrorTime);
}

} //namespace

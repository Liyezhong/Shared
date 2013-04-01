/******************************************************************/
/*! \file AdjustmentService.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 11.10.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the definition of the CAdjustmentService class
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

#include "DeviceControl/Include/DeviceProcessing/AdjustmentService.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/SlaveModules/Joystick.h"
#include "DeviceControl/Include/SlaveModules/StepperMotor.h"
#include "Global/Include/AdjustedTime.h"

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
CAdjustmentService::CAdjustmentService(DeviceProcessing* pDeviceProcessing,
                                       CANCommunicator* pCANCommunicator,
                                       ObjectTree* pObjectTree):
    m_mainState(ADJUST_MAIN_STATE_INIT)
{
    m_pDeviceProcessing = pDeviceProcessing;
    m_pCANCommunicator = pCANCommunicator;
    m_pObjectTree = pObjectTree;

    m_mainState = ADJUST_MAIN_STATE_INIT;
}

/****************************************************************************/
/*!
 *  \brief  Destructor
 */
/****************************************************************************/
CAdjustmentService::~CAdjustmentService()
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
void CAdjustmentService::HandleTasks()
{
    //HandleCANNodesTask();
    if(m_mainState == ADJUST_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_mainState == ADJUST_MAIN_STATE_INIT)
    {
        HandleInitializationState();
        m_mainState = ADJUST_MAIN_STATE_CONFIG;
    }
    else if(m_mainState == ADJUST_MAIN_STATE_CONFIG)
    {
        HandleConfigurationState();
        m_mainState = ADJUST_MAIN_STATE_IDLE;
    }
}

/****************************************************************************/
/*!
 *  \brief  Handling the initialization state
 */
/****************************************************************************/
void CAdjustmentService::HandleInitializationState()
{
    //get grappler access
    //get joystick access
    //check joystick configuration (calibration, sw-version)
    //check adjustment file (e.g save old)
    //data array initialization
}

/****************************************************************************/
/*!
 *  \brief  Handling the configuration state
 */
/****************************************************************************/
void CAdjustmentService::HandleConfigurationState()
{
    // Grappler reference run
    // Grappler joystick mounting position?
    // adjustment data file preparation
}

/****************************************************************************/
/*!
 *  \brief  Handling the idle state
 */
/****************************************************************************/
void CAdjustmentService::HandleIdleState()
{
}

/****************************************************************************/
/*!
 *  \brief  CAN node task handling function
 */
/****************************************************************************/
void CAdjustmentService::HandleCANNodesTask()
{
    CBaseModule* pCANNode;

    pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(true);
    while (pCANNode)
    {
        pCANNode->HandleTasks();
        pCANNode = m_pDeviceProcessing->GetCANNodeFromObjectTree(false);
    }
}

/****************************************************************************/
/*!
 *  \brief  Throws an error signal
 *
 *  \iparam usNodeID = Node ID
 *  \iparam ulModulInfo = Module ID
 *  \iparam usErrorGroup = Error group
 *  \iparam usErrorID = Error ID
 *  \iparam sErrorData = Error data
 */
/****************************************************************************/
void CAdjustmentService::ThrowErrorSignal(quint16 usNodeID, quint32 ulModulInfo, quint16 usErrorGroup, quint16 usErrorID, qint16 sErrorData)
{
    Q_UNUSED(usNodeID);
    Q_UNUSED(ulModulInfo);

    quint32 ulModulHandle = 0;
    QDateTime errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
    m_pDeviceProcessing->ThrowError(ulModulHandle, usErrorGroup, usErrorID, sErrorData, errorTimeStamp);
}

} //namespace

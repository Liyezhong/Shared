/******************************************************************/
/*! \file BaseDeviceNavigator.cpp
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    16.02.2011
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementation of the CBaseDeviceNavigator class
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
#include "DeviceControl/Include/Devices/BaseDeviceNavigator.h"

namespace DeviceControl
{

CBaseDeviceNavigator::CBaseDeviceNavigator()
{
}

CBaseDeviceNavigator::~CBaseDeviceNavigator()
{
}

/****************************************************************************/
/*!
 *  \brief    Set a device task and ts functionmdule tasks to init-state
 *
 *
 *
 *  \iparam   pDeviceTask = The task to set
 *
 *  \return   void
 */
/****************************************************************************/
void CBaseDeviceNavigator::ResetDeviceTask( DeviceTask* pDeviceTask)
{
    FunctionModuleTask* pFunctionModuleTask;
    QMapIterator<quint8, FunctionModuleTask*> iterFctModTask(pDeviceTask->m_FctModTaskMap);

    // set all tasks to state 'unused'
    while (iterFctModTask.hasNext())
    {
        pFunctionModuleTask = iterFctModTask.next().value();
        pFunctionModuleTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    }
    pDeviceTask->m_TaskState = DeviceTask::DEV_TASK_STATE_INIT;
}

} //namespace

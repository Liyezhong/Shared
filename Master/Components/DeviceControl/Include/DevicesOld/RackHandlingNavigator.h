/****************************************************************************/
/*! \file RackHandlingNavigator.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 15.02.2011
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the definition of the CCoverLineNavigator class.
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
/****************************************************************************/

#ifndef RACKHANDLINGNAVIGATOR_H
#define RACKHANDLINGNAVIGATOR_H

#include <QObject>
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/RackHandlingDevice.h"
#include "DeviceControl/Include/Devices/BaseDeviceNavigator.h"

namespace DeviceControl
{

class CStepperMotor;
class FunctionModuleTask;
class CANStepperMotorTask;


/****************************************************************************/
/*! \class CRackHandlingNavigator
 *
 *  \brief CRackHandlingNavigator implements methods used by CRackHandlingDevice to create and handle
 *         the rack movement actions.
 *         Several Create...() - functions create the function module tasks used by device tasks to
 *         manage their jobs. The process parameters will be set there.
 *
 *         When the process is running, the Handle---()-function will be called whenever a associated function module
 *         slot was called. The slots are implemented at CRackHandlingDevice, and the informationis forwarded
 *         to the Handle..()-function of the corresponding task.
 *
 */
/****************************************************************************/
class CRackHandlingNavigator : public CBaseDeviceNavigator
{
public:
    CRackHandlingNavigator();
    ~CRackHandlingNavigator();

    /*! requested action for grappler positioning */
    typedef enum {
        CL_NAVI_ACTION_GRAB                 = 0x00,
        CL_NAVI_ACTION_DISCARD              = 0x01,
        CL_NAVI_ACTION_READ_RFID            = 0x02,
        CL_NAVI_ACTION_READ_LIQUID_LEVEL    = 0x03
    } NavigatorAction_t;

    ReturnCode_t CreateReferenceRunFctModTasks(CRackHandlingDevice* pRackHdlDev, DeviceTask* pDeviceTask);
    ReturnCode_t CreatePosRequestTasks(CRackHandlingDevice* pRackHdlDev, DeviceTask* pDeviceTask);
    ReturnCode_t CreateTransportFctModTasks(CRackHandlingDevice* pRackHdlDev, DeviceTask* pDeviceTask);

    ReturnCode_t CalculateTransportActionTasks(CRackHandlingDevice* pRackHdlDev,
                                               FunctionModuleTaskMap& taskMap,
                                               RackHdlStation_t m_ReqStation,
                                               StationAction_t  m_ReqStationAktion);

    void HandleRefRunTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask);
    void HandleRackHandlingTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask);
    void HandleTransportActionTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask);

    //void ResetDeviceTask( DeviceTask* pDeviceTask);

private:
    FunctionModuleTask* GetFirstTaskWithState(FunctionModuleTaskMap& TaskMap, FunctionModuleTask::FunctionModuleTaskState_t taskState);
    //CANStepperMotorTask*  GetNewCANStepperMotorTask(CANStepperMotorTask::CANStepperMotorTaskID_t MotorTaskID);

};

} //namespace


#endif // RACKHANDLINGNAVIGATOR_H

/****************************************************************************/
/*! \file CoverLineNavigator.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 19.01.2011
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
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef COVERLINENAVIGATOR_H
#define COVERLINENAVIGATOR_H

#include <QObject>
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/CoverLineDevice.h"
#include "DeviceControl/Include/Devices/BaseDeviceNavigator.h"

namespace DeviceControl
{

class CStepperMotor;
class CRfid11785;
class CAnalogInput;
class FunctionModuleTask;
class CANStepperMotorTask;


/****************************************************************************/
/*! \class CCoverLineNavigator
 *
 *  \brief CCoverLineNavigator implements methods used by CCoverLineDevice to create and handle
 *         the cover slipping process.
 *         Several Create...() - functions create the function module tasks used by device tasks to
 *         manage their jobs. The process parameters will be set there.
 *
 *         When the process is running, the Handle---()-function will be called whenever a associated function module
 *         slot was called. The slots are implemented at CCoverLineDevice, and the informationis forwarded
 *         to the Handle..()-function of the corresponding task.
 *
 */
/****************************************************************************/
class CCoverLineNavigator : public CBaseDeviceNavigator
{

public:
    CCoverLineNavigator();
    ~CCoverLineNavigator();

    /*! requested action for grappler positioning */
    typedef enum {
        CL_NAVI_ACTION_GRAB                 = 0x00,
        CL_NAVI_ACTION_DISCARD              = 0x01,
        CL_NAVI_ACTION_READ_RFID            = 0x02,
        CL_NAVI_ACTION_READ_LIQUID_LEVEL    = 0x03
    } NavigatorAction_t;

    ReturnCode_t CreateReferenceRunFctModTasks(CCoverLineDevice* pCoverLine, DeviceTask* pDeviceTask);
    ReturnCode_t CreateBasisPosFctModTasks(CCoverLineDevice* pCoverLine, DeviceTask* pDeviceTask);
    ReturnCode_t CreateRackPosTasks(CCoverLineDevice* pCoverLine, DeviceTask* pDeviceTask);
    ReturnCode_t CreateSlideShiftOutTasks(CCoverLineDevice* pCoverLine, DeviceTask* pDeviceTask);
    ReturnCode_t CreateSlideShiftBackTasks(CCoverLineDevice* pCoverLine, DeviceTask* pDeviceTask);

    ReturnCode_t CreateCoverSlipGrabTasks(CCoverLineDevice* pCoverLine, DeviceTask* pDeviceTask);
    ReturnCode_t CreateCoverSlipCoverTasks(CCoverLineDevice* pCoverLine, DeviceTask* pDeviceTask);
    ReturnCode_t CreateCoverSlipDiscardTasks(CCoverLineDevice* pCoverLine, DeviceTask* pDeviceTask);

    void HandleCoverLineDeviceTasks(CCoverLineDevice* pCoverLineDev, DeviceTaskMap& DeviceTaskMap);
    void HandleRefRunTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask);
    void HandleBasisPositionTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask);
    void HandleRackPositionTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask);
    void HandleCoverslipGrabTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev);
    void HandleCoverslipDiscardTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev);
    void HandleSlideShiftOutTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev);
    void HandleCoverTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev);
    void HandleSlideShiftBackTaskEvent(FunctionModuleTask* pFunctionModuleTask, DeviceTask* pDeviceTask, CCoverLineDevice* pCoverLineDev);

    /*void ResetDeviceTask( DeviceTask* pDeviceTask);

private:
    //FunctionModuleTask* GetFirstTaskWithState(FunctionModuleTaskMap& TaskMap, FunctionModuleTask::FunctionModuleTaskState_t taskState);

    CANStepperMotorTask*  GetNewCANStepperMotorTask(CANStepperMotorTask::CANStepperMotorTaskID_t MotorTaskID);
    CANDigitalInputTask*  GetNewCANDigitalInputTask(CANDigitalInputTask::CANDigitalInputTaskID_t DigInpTaskID);
    CANDigitalOutputTask* GetNewCANDigitalOutputTask(CANDigitalOutputTask::CANDigitalOutputTaskID_t DigOutpTaskID);
*/
};

} //namespace


#endif // COVERLINENAVIGATOR_H

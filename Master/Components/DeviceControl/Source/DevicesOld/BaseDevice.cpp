/******************************************************************/
/*! \file BaseDevice.cpp
 *
 *  \brief
 *
 *   Version: $ 0.1
 *   Date:    $ 11.01.2011
 *   Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the implementaion of the CBaseDevice class
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

#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{


/****************************************************************************/
/*!
 *  \brief    Constructor of the CBaseDevice class
 *
 *
 *  \param    pDeviceProcessing = pointer to DeviceProcessing
 *  \param    Type = Device type string
 *
 ****************************************************************************/
 CBaseDevice::CBaseDevice(DeviceProcessing* pDeviceProcessing, QString Type) :
    m_pDevProc(pDeviceProcessing),
    m_Type(Type),
    m_stateTimespan(0)
{
    m_MainState = DEVICE_MAIN_STATE_START;
    m_MainStateOld = DEVICE_MAIN_STATE_START;

    m_instanceID = DEVICE_INSTANCE_ID_AGITATION;

    // error handling
    m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    m_lastErrorGroup = 0;
    m_lastErrorCode = 0;
    m_lastErrorData = 0;
    //m_lastErrorTime;

    FILE_LOG_L(laDEV, llINFO) << " CBaseDevice constuctor";
}

 /****************************************************************************/
 /*!
  *  \brief    Destructor of CBaseDevice
  *
  *
  *
  ****************************************************************************/
CBaseDevice::~CBaseDevice()
{
    m_pDevProc = 0;
}

/****************************************************************************/
/*!
 *  \brief   Returns the instance identifier of a function module
 *
 *  \param   Key = Fuction module key, as defined at the hardware specification file
 *
 *  \return  The instance id of the requested module, or 0 if Key doesn't match
 *
 ****************************************************************************/
quint32 CBaseDevice::GetFctModInstanceFromKey(const QString &Key)
{
    quint32 InstanceID = 0;

    if(m_FctModList.contains(Key))
    {
        InstanceID = m_FctModList.value(Key);
    }

    return InstanceID;
}

/****************************************************************************/
/*!
 *  \brief   Set the error parameter of the class, and error time to current time
 *
 *  \iparam   ErrorGroup = Error group
 *  \iparam   ErrorCode = Error code
 *  \iparam   ErrorData = Additional error information
 *
 *
 ****************************************************************************/
void CBaseDevice::SetErrorParameter(quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData)
{
    m_lastErrorTime  = Global::AdjustedTime::Instance().GetCurrentDateTime();
    m_lastErrorGroup = ErrorGroup;
    m_lastErrorCode  = ErrorCode;
    m_lastErrorData  = ErrorData;

}

/****************************************************************************/
/*!
 *  \brief   Return the process setting value of the key as Position_t
 *
 *  \param   Key = Process parameter's key string
 *
 *  \return  Process parameter matching 'Key' as Position_t
 *
 ****************************************************************************/
Position_t CBaseDevice::GetProcSettingPosition(QString Key)
{
    QString ProcSettingKey;

    ProcSettingKey = m_TypeKey + "_" + Key;
    return m_pDevProc->GetProcSettingPosition(ProcSettingKey);
}

/****************************************************************************/
/*!
 *  \brief   Return the process setting value of the key as MotionProfileIdx_t
 *
 *  \param   Key = Process parameter's key string
 *
 *  \return  Process parameter matching 'Key' as MotionProfileIdx_t
 *
 ****************************************************************************/
MotionProfileIdx_t CBaseDevice::GetProcSettingMotionProfile(QString Key)
{
    QString ProcSettingKey;

    ProcSettingKey = m_TypeKey + "_" + Key;
    return m_pDevProc->GetProcSettingMotionProfileIdx(ProcSettingKey);
}

/****************************************************************************/
/*!
 *  \brief    Create a new CANStepperMotorTask instance
 *
 *  \iparam   MotorTaskID = The task identification of the function modul task
 *
 *  \return   the new CANStepperMotorTask instance
 */
/****************************************************************************/
CANStepperMotorTask* CBaseDevice::GetNewCANStepperMotorTask(CANStepperMotorTask::CANStepperMotorTaskID_t MotorTaskID)
{
    CANStepperMotorTask* pCANStepperMotorTask;

    pCANStepperMotorTask = new CANStepperMotorTask();
    pCANStepperMotorTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_MOTOR;
    pCANStepperMotorTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANStepperMotorTask->m_MotorTaskID = MotorTaskID;

    return pCANStepperMotorTask;
}

/****************************************************************************/
/*!
 *  \brief    Create a new CANRFIDTask instance
 *
 *  \iparam   RFIDTaskID = The task identification of the function modul task
 *
 *  \return   the new CANRFIDTask instance
 */
/****************************************************************************/
CANRFIDTask* CBaseDevice::GetNewCANRFIDTask(CANRFIDTask::CANRFIDTaskID_t RFIDTaskID)
{
    CANRFIDTask* pCANRFIDTask;

    pCANRFIDTask = new CANRFIDTask();
    //pCANRFIDTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_RFID;
    if(RFIDTaskID == CANRFIDTask::FCTMOD_RFID_TASK_ID_REQ_DATA)
    {
        pCANRFIDTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_RFID;

    }else if(RFIDTaskID == CANRFIDTask::FCTMOD_RFID_TASK_ID_REQ_TAG)
    {
        pCANRFIDTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_RFID_TAG;
    }
    pCANRFIDTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANRFIDTask->m_RFIDTaskID = RFIDTaskID;
    pCANRFIDTask->m_UID = 0;
    pCANRFIDTask->m_RFIDData = 0;

    return pCANRFIDTask;
}

/****************************************************************************/
/*!
 *  \brief    Create a new CANDigitalInputTask instance
 *
 *
 *
 *  \iparam   DigInpTaskID = The task identification of the function modul task
 *
 *  \return   the new CANDigitalInputTask instance
 */
/****************************************************************************/
CANDigitalInputTask* CBaseDevice::GetNewCANDigitalInputTask(CANDigitalInputTask::CANDigitalInputTaskID_t DigInpTaskID)
{
    CANDigitalInputTask* pCANDigitalInputTask;

    pCANDigitalInputTask = new CANDigitalInputTask();
    pCANDigitalInputTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_DIG_INPUT;
    pCANDigitalInputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANDigitalInputTask->m_DigInpTaskID = DigInpTaskID;

    return pCANDigitalInputTask;

}

/****************************************************************************/
/*!
 *  \brief    Create a new CANTemperatureCtrlTask instance
 *
 *  \iparam   TempCtrlTaskID = The task identification of the function modul task
 *
 *  \return   the new CANTemperatureCtrlTask instance
 */
/****************************************************************************/
CANTemperatureCtrlTask* CBaseDevice::GetNewCANTempCtrlTask(CANTemperatureCtrlTask::CANTempCtrlTaskID_t TempCtrlTaskID)
{
    CANTemperatureCtrlTask* pCANTemperatureCtrlTask;

    pCANTemperatureCtrlTask = new CANTemperatureCtrlTask();
    pCANTemperatureCtrlTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_TEMP_CTRL;
    pCANTemperatureCtrlTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANTemperatureCtrlTask->m_TempCtrlTaskID = TempCtrlTaskID;

    return pCANTemperatureCtrlTask;
}

/****************************************************************************/
/*!
 *  \brief    Create a new CANDigitalOutputTask instance
 *
 *
 *
 *  \iparam   DigOutpTaskID = The task identification of the function modul task
 *
 *  \return   the new CANDigitalOutputTask instance
 */
/****************************************************************************/
CANDigitalOutputTask* CBaseDevice::GetNewCANDigitalOutputTask(CANDigitalOutputTask::CANDigitalOutputTaskID_t DigOutpTaskID)
{
    CANDigitalOutputTask* pCANDigitalOutputTask;

    pCANDigitalOutputTask = new CANDigitalOutputTask();
    pCANDigitalOutputTask->m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_DIG_OUTPUT;
    pCANDigitalOutputTask->m_TaskState = FunctionModuleTask::FCTMOD_STATE_INIT;
    pCANDigitalOutputTask->m_DigOutpTaskID = DigOutpTaskID;

    return pCANDigitalOutputTask;

}

/****************************************************************************/
/*!
 *  \brief   Create a device task instance with the passed parameters
 *
 *  \iparam   TaskState = Task state of the new device task
 *  \iparam   Key = Key of the new device task
 *
 *  \return   The new created device task instance
 *
 ****************************************************************************/
DeviceTask* CBaseDevice::GetNewDeviceTask(DeviceTask::DeviceTaskState_t TaskState, quint16 Key)
{
    DeviceTask* pDeviceTask;

    pDeviceTask = new DeviceTask();
    pDeviceTask->m_TaskState = TaskState;
    pDeviceTask->m_Key = Key;

    return pDeviceTask;
}

} //namespace


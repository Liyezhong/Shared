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
#include <QtDebug>
#include "DataManager/Containers/InstrumentHistory/Include/Module.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"
#include "DeviceControl/Include/Devices/ServiceState.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceLifeCycleRecord.h"
#include <QMetaType>

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
 CBaseDevice::CBaseDevice(DeviceProcessing* pDeviceProcessing,
                          QString& Type,
                          const DeviceModuleList_t &ModuleList,
                          quint32 InstanceID) :
    m_pDevProc(pDeviceProcessing),
    m_Type(Type),
    m_instanceID(InstanceID),
    m_stateTimespan(0),
    m_LastSensorCheckTime(0),
    m_ModuleList(ModuleList),
    m_machine(this),
    m_ModuleLifeCycleRecord(0)
{
    m_MainState = DEVICE_MAIN_STATE_START;
    m_MainStateOld = DEVICE_MAIN_STATE_START;
    qRegisterMetaType<DataManager::CModule>("DataManager::CModule");

    // error handling
    m_lastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    m_lastErrorGroup = 0;
    m_lastErrorCode = 0;
    m_lastErrorData = 0;
    m_BaseModuleVoltage = 0;
    m_BaseModuleCurrent = 0;
    //m_lastErrorTime;

    QMapIterator<QString, quint32> Iterator(m_ModuleList);
    while (Iterator.hasNext()) {
        Iterator.next();
        DeviceControl::CModule *p_Module = m_pDevProc->GetNodeFromID(GetFctModInstanceFromKey(Iterator.key()));

        if (p_Module == NULL) {
            p_Module = m_pDevProc->GetFunctionModule(GetFctModInstanceFromKey(Iterator.key()));
        }
        if (p_Module != NULL) {
            m_ModuleMap[Iterator.key()] = p_Module;
        }
    }

    mp_Service = new CServiceState(m_ModuleMap, this->GetType(), &m_machine);
    m_machine.setInitialState(mp_Service);

    connect(this, SIGNAL(GetServiceInfo()), mp_Service, SIGNAL(GetServiceInfo()));
    connect(mp_Service, SIGNAL(ReportGetServiceInfo(ReturnCode_t, DataManager::CModule)),
                     this, SLOT(OnReportGetServiceInfo(ReturnCode_t, DataManager::CModule)));

    /*connect(this, SIGNAL(ResetServiceInfo()), mp_Service, SIGNAL(ResetServiceInfo()));
    connect(mp_Service, SIGNAL(ReportResetServiceInfo(ReturnCode_t)),
                     this, SIGNAL(ReportResetServiceInfo(ReturnCode_t)));*/

    m_machine.start();
    FILE_LOG_L(laDEV, llINFO) << " CBaseDevice constuctor";
}

void CBaseDevice::OnReportGetServiceInfo(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo)
{
    m_pDevProc->OnReportGetServiceInfo(ReturnCode, ModuleInfo, m_Type);
}

void CBaseDevice::OnGetServiceInfor()
{
    emit GetServiceInfo();
}

void CBaseDevice::OnReportSavedServiceInfor(const QString& deviceType)
{
    if (deviceType == m_Type)
    {
        emit ReportSavedServiceInfor();
    }
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

    if(m_ModuleList.contains(Key))
    {
        InstanceID = m_ModuleList.value(Key);
    }

    return InstanceID;
}

/****************************************************************************/
/*!
 *  \brief   Returns the key of a function module
 *
 *  \param   instanceID = The instance id of the requested module
 *
 *  \return  Fuction module key, as defined at the hardware specification file
 *
 ****************************************************************************/
QString CBaseDevice::GetFctModKeyFromInstance(const quint32 instanceID)
{
    return m_ModuleList.key(instanceID, "NOT FOUND");
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

/****************************************************************************/
/*!
 *  \brief   slot associated with function module error reporting.
 *
 *  This slot is connected to the signal, ReportError
 *
 *  \iparam InstanceID = Instance ID of the function module
 *  \iparam ErrorGroup = Error group as define.
 *  \iparam ErrorCode = Error code.
 *  \iparam ErrorData = Error data.
 *  \iparam ErrorTime = Error happened time.
 *
 */
/****************************************************************************/
void CBaseDevice::OnFunctionModuleError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData, QDateTime ErrorTime)
{
    QString FuncModName = "";
    if(m_pDevProc)
    {
        QList<quint32> list;
        list <<  DEVICE_INSTANCE_ID_ROTARY_VALVE
              << DEVICE_INSTANCE_ID_AIR_LIQUID
              << DEVICE_INSTANCE_ID_OVEN
              << DEVICE_INSTANCE_ID_RETORT
              << DEVICE_INSTANCE_ID_MAIN_CONTROL;

        quint32 id;
        foreach (id, list)
        {
            CBaseDevice* pBaseDevice;
            pBaseDevice = m_pDevProc->GetDevice(id);
            FuncModName = pBaseDevice->GetFctModKeyFromInstance(InstanceID);
            if(FuncModName != "NOT FOUND")
            {break;}
        }
        //LOG()<<"Function module: "<< FuncModName <<" Error, Error Group: "<< ErrorGroup<<" Error Code: "<<ErrorCode<<" Error Data: "<<ErrorData<<" Error Time: "<<ErrorTime;
        FILE_LOG_L(laDEV, llDEBUG)<<"Function module: "<< FuncModName.toStdString()<<" Error, Error Group: "<<  std::hex << ErrorGroup<<" Error Code: "<<std::hex<<ErrorCode<<" Error Data: "<<std::hex<<ErrorData<<" Error Time: "<<ErrorTime.toString().toStdString();
    }
}

/****************************************************************************/
/*!
 *  \brief   Insert a base module to the device's list.
 *
 *  The function will check if the device already contain the base module,
 *  if no, will add to the device's base module list.
 *
 *  \iparam pBase = Pointer of the base module
 *
 *  \return  True if successfully inserted the module, othervise False.
 *
 */
/****************************************************************************/
bool CBaseDevice::InsertBaseModule(CBaseModule* pBase)
{
    if((!pBase) || (m_BaseModuleList.contains(pBase)))
    {
        return false;
    }
    m_BaseModuleList.append(pBase);

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Get the base module of the device with CAN-node ID.
 *
 *  \iparam  CANNodeID = The target module's CAN-node ID.
 *
 *  \return  Pointer of the base module if found, othervise NULL.
 *
 */
/****************************************************************************/
CBaseModule* CBaseDevice::GetCANNodeFromID(quint16 CANNodeID)
{
    CBaseModule* pRet = NULL;
    if(m_BaseModuleList.size() > 0)
    {
        QListIterator<CBaseModule*> iter(m_BaseModuleList);
        while (iter.hasNext())
        {
            CBaseModule* pBaseModule = (CBaseModule*)iter.next();
            if(pBaseModule->GetNodeID() == CANNodeID)
            {
                pRet = pBaseModule;
                break;
            }
        }
    }
    return pRet;
}

/****************************************************************************/
/*!
 *  \brief   Get the voltage of the device's base module.
 *
 *  \iparam  CANNodeID = The target module's CAN-node ID.
 *
 *  \return  0 If failed, otherwise the actual voltage in mV.
 *
 */
/****************************************************************************/
quint16 CBaseDevice::GetBaseModuleVoltage(quint16 CANNodeID)
{
    quint16 retValue = 0;
    if(m_pDevProc)
    {
        CBaseModule* pBaseModule = GetCANNodeFromID(CANNodeID);
        if(pBaseModule)
        {
            (void)connect(pBaseModule, SIGNAL(ReportVoltageState(quint32, ReturnCode_t, PowerState_t, quint16, quint16)), this, SLOT(OnReportVoltageState(quint32, ReturnCode_t, PowerState_t, quint16, quint16)));
            ReturnCode_t retCode = pBaseModule->ReqVoltageState();
            if(DCL_ERR_FCT_CALL_SUCCESS == retCode)
            {
                retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_BASE_GET_VOLTAGE);
                if(retCode == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    retValue = m_BaseModuleVoltage;
                }
            }
        }
    }
    return retValue;
}

/****************************************************************************/
/*!
 *  \brief  Slot associated with get base module's voltage.
 *
 *  This slot is connected to the signal, ReportVoltageState
 *
 *  \iparam InstanceID = Instance ID of the function module
 *  \iparam HdlInfo
 *  \iparam State = The power state
 *  \iparam Value = The voltage value
 *  \iparam Failures = The failures number
 *
 */
/****************************************************************************/
void CBaseDevice::OnReportVoltageState(quint32 InstanceID, ReturnCode_t HdlInfo, PowerState_t State, quint16 Value, quint16 Failures)
{
    (void)State;
    (void)Failures;
    if(m_pDevProc)
    {
        CBaseModule* pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
        (void)disconnect(pBaseModule, SIGNAL(ReportVoltageState(quint32, ReturnCode_t, PowerState_t, quint16, quint16)), this, SLOT(OnReportVoltageState(quint32, ReturnCode_t, PowerState_t, quint16, quint16)));
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_BaseModuleVoltage = Value;
        }
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_BASE_GET_VOLTAGE, HdlInfo);
    }
}

/****************************************************************************/
/*!
 *  \brief   Get the current of the device's base module.
 *
 *  \iparam  CANNodeID = The target module's CAN-node ID.
 *
 *  \return  0 If failed, otherwise the actual current in mA.
 *
 */
/****************************************************************************/
quint16 CBaseDevice::GetBaseModuleCurrent(quint16 CANNodeID)
{
    quint16 retValue = 0;
    if(m_pDevProc)
    {
        CBaseModule* pBaseModule = GetCANNodeFromID(CANNodeID);
        if(pBaseModule)
        {
            (void)connect(pBaseModule, SIGNAL(ReportCurrentState(quint32, ReturnCode_t, PowerState_t, quint16, quint16)), this, SLOT(OnReportCurrentState(quint32, ReturnCode_t, PowerState_t, quint16, quint16)));
            ReturnCode_t retCode = pBaseModule->ReqCurrentState();
            if(DCL_ERR_FCT_CALL_SUCCESS == retCode)
            {
                retCode = m_pDevProc->BlockingForSyncCall(SYNC_CMD_BASE_GET_CURRENT);
                if(retCode == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    retValue = m_BaseModuleCurrent;
                }
            }
        }
    }
    return retValue;
}
ReportError_t CBaseDevice::GetSlaveModuleError(quint8 errorCode, quint32 instanceID)
{
    ReportError_t reportError;
    memset(&reportError, 0, sizeof(reportError));
#if 0
    switch (instanceID)
     {
     case CANObjectKeyLUT::FCTMOD_RV_TEMPCONTROL:
         LogDebug("In RV temperature control");
         break;
     case CANObjectKeyLUT::FCTMOD_AL_LEVELSENSORTEMPCTRL:
         LogDebug("In LA LevelSensor temperature control");
         break;
     case CANObjectKeyLUT::FCTMOD_AL_TUBE1TEMPCTRL:
         LogDebug("In LA tube1 temperature control");
         break;
     case CANObjectKeyLUT::FCTMOD_AL_TUBE2TEMPCTRL:
         LogDebug("In LA tube2 tempertuare control");
         break;
     case CANObjectKeyLUT::FCTMOD_OVEN_BOTTOMTEMPCTRL:
         LogDebug("In Oven Bottom tempertuare control");
         break;
     case CANObjectKeyLUT::FCTMOD_OVEN_TOPTEMPCTRL:
         LogDebug("In Oven top tempertuare control");
         break;
     case CANObjectKeyLUT::FCTMOD_RETORT_BOTTOMTEMPCTRL:
         LogDebug("In Retort Bottom tempertuare control");
         break;
     case CANObjectKeyLUT::FCTMOD_RETORT_SIDETEMPCTRL:
         LogDebug("In Retort Side tempertuare control");
         break;
     default:
         break;
     }
#endif
    if (m_ReportErrorList.empty())
    {
        //LogDebug("Temperature module errorlist is empty");
        return reportError;
    }
    else
    {
       //LogDebug(QString("Temperature module errorlist is NOT empty, the size is: %1").arg(m_ReportErrorList.size()));
    }

#if 0
    for (int i=0; i<m_ReportErrorList.size(); ++i)
    {
        LogDebug(QString("The Error InstanceID is: %1, error code is: %2").arg(m_ReportErrorList[i].instanceID).arg(m_ReportErrorList[i].errorCode));
    }
#endif
    FILE_LOG_L(laFCT, llERROR) << " Error instanceID is: 0x"<<std::hex<<m_ReportErrorList[0].instanceID;

    QVector<ReportError_t>::const_iterator iter = m_ReportErrorList.end();
    iter--;
    bool found = false;
    for (; iter>=m_ReportErrorList.begin(); --iter)
    {
        if (instanceID == iter->instanceID && errorCode == iter->errorCode)
        {
            reportError = (*iter);
            found = true;
            break;
        }
    }

    // Clean up the vector if the element was found
    if (true == found)
    {
        QVector<ReportError_t> empVector(0);
        m_ReportErrorList.swap(empVector);
    }

    return reportError;
}

/****************************************************************************/
/*!
 *  \brief  Slot associated with get base module's current.
 *
 *  This slot is connected to the signal, ReportCurrentState
 *
 *  \iparam InstanceID = Instance ID of the function module
 *  \iparam HdlInfo
 *  \iparam State = The Power state
 *  \iparam Value = The current value
 *  \iparam Failures = Failures numbers
 *
 */
/****************************************************************************/
void CBaseDevice::OnReportCurrentState(quint32 InstanceID, ReturnCode_t HdlInfo, PowerState_t State, quint16 Value, quint16 Failures)
{
    (void)State;
    (void)Failures;
    if(m_pDevProc)
    {
        CBaseModule* pBaseModule = m_pDevProc->GetBaseModule(InstanceID);
        (void)disconnect(pBaseModule, SIGNAL(ReportCurrentState(quint32, ReturnCode_t, PowerState_t, quint16, quint16)), this, SLOT(OnReportCurrentState(quint32, ReturnCode_t, PowerState_t, quint16, quint16)));
        if(HdlInfo == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_BaseModuleCurrent = Value;
        }
        m_pDevProc->ResumeFromSyncCall(SYNC_CMD_BASE_GET_CURRENT, HdlInfo);
    }
}

void CBaseDevice::SetModuleLifeCycleRecord(ModuleLifeCycleRecord* pModuleLifeCycleRecord)
{
    m_ModuleLifeCycleRecord = pModuleLifeCycleRecord;
}

} //namespace


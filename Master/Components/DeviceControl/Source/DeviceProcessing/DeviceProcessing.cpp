/******************************************************************/
/*! \file       DeviceProcessing.cpp
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *  This file contains the device processing class implementation
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

#include "DeviceControl/Include/CanCommunication/CANThreads.h"
#include "DeviceControl/Include/CanCommunication/CANInterface.h"
#include "DeviceControl/Include/CanCommunication/CANCommunicator.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/DeviceProcessing/ConfigurationService.h"
#include "DeviceControl/Include/DeviceProcessing/DiagnosticService.h"
#include "DeviceControl/Include/DeviceProcessing/AdjustmentService.h"
#include "DeviceControl/Include/DeviceProcessing/ShutdownService.h"

#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"
#include "DeviceControl/Include/Devices/OvenDevice.h"

#include "DeviceControl/Include/SlaveModules/BaseModule.h"

#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/SystemPaths.h"
#include "Global/Include/AdjustedTime.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

#include <QMetaType>
#include <cerrno>

namespace DeviceControl
{

const QString CANObjectKeyLUT::m_RVMotorKey = "RV_motor";
const QString CANObjectKeyLUT::m_RVTempCtrlKey = "RV_temp_ctrl";
const QString CANObjectKeyLUT::m_ALPressureCtrlKey = "AL_pressure_ctrl";        //!< Air-liquid pressure control
const QString CANObjectKeyLUT::m_ALLevelSensorTempCtrlKey = "AL_level_sensor_temp_ctrl"; //!< Air-liquid level sensor temp control
const QString CANObjectKeyLUT::m_ALTube1TempCtrlKey = "AL_tube1_temp_ctrl";       //!< Air-liquid tube1 temp control
const QString CANObjectKeyLUT::m_ALTube2TempCtrlKey = "AL_tube2_temp_ctrl";       //!< Air-liquid tube2 temp control
//const QString CANObjectKeyLUT::m_ALFanDOKey= "AL_fan_digital_output";               //!< Air-liquid fan digital output
const QString CANObjectKeyLUT::m_OvenTopTempCtrlKey = "OVEN_top_temp_ctrl";       //!< Oven top temp control
const QString CANObjectKeyLUT::m_OvenBottomTempCtrlKey = "OVEN_bottom_temp_ctrl";    //!< Oven bottom temp control
const QString CANObjectKeyLUT::m_OvenLidDIKey = "OVEN_lid_digital_input";            //!< Oven lid digital input
const QString CANObjectKeyLUT::m_RetortBottomTempCtrlKey = "RETORT_bottom_temp_ctrl";  //!< Retort bottom temp control
const QString CANObjectKeyLUT::m_RetortSideTempCtrlKey= "RETORT_side_temp_ctrl";    //!< Retort side temp control
const QString CANObjectKeyLUT::m_RetortLockDOKey= "RETORT_lock_digital_output";          //!< Retort lock digital output
const QString CANObjectKeyLUT::m_RetortLockDIKey= "RETORT_lock_digital_input";           //!< Retort lock digital input
const QString CANObjectKeyLUT::m_PerRemoteAlarmCtrlDOKey= "PER_remote_alarm_ctrl_digital_output";  //!< Miscellaneous remote alarm ctrl digital output
const QString CANObjectKeyLUT::m_PerLocalAlarmCtrlDOKey= "PER_local_alarm_ctrl_digital_output";  //!< Miscellaneous remote alarm ctrl digital output
//const QString CANObjectKeyLUT::m_PerRemoteAlarmSetDOKey= "PER_remote_alarm_set_digital_output";   //!< Miscellaneous remote alarm set digital output
//const QString CANObjectKeyLUT::m_PerRemoteAlarmClearDOKey= "PER_remote_alarm_clear_digital_output";  //!< Miscellaneous remote alarm clear digital output
const QString CANObjectKeyLUT::m_PerMainRelayDOKey= "PER_main_relay_digital_output";     //!< Miscellaneous heater relay digital output
const QString CANObjectKeyLUT::m_PerLocalAlarmDIKey= "PER_local_alarm_digital_input";     //!< Miscellaneous local alarm digital input
const QString CANObjectKeyLUT::m_PerRemoteAlarmDIKey= "PER_remote_alarm_digital_input";     //!< Miscellaneous remote alarm digital input

const QString CANObjectKeyLUT::m_OtherDeviceEBoxKey = "OtherDevice_EBox";              //!< EBox
const QString CANObjectKeyLUT::m_OtherDeviceVentilationFanKey = "OtherDevice_Ventilation_Fan";    //!< VentilationFan
const QString CANObjectKeyLUT::m_OtherDeviceTouchScreenKey = "OtherDevice_Touch_Screen";       //!< TouchScreen
const QString CANObjectKeyLUT::m_OtherDevicePressureSensor = "OtherDevice_Pressure_Sensor";       //!< TouchScreen

//QString DeviceProcessing::m_HWConfigFileName = "hw_specification_sepia.xml";
QString DeviceProcessing::m_HWConfigFileName = "hw_specification.xml";

QString DeviceProcessing::m_SerialNo = "";

/********************************************************************************/
/*!
 *  \brief  Constructor of the class
 *
 *      Because the class is derived from QThread, the constructor needs the
 *      classes parent object.
 *
 *  \iparam p_Parent = The parent object of this class.
 */
/********************************************************************************/
DeviceProcessing::DeviceProcessing(QObject *p_Parent) : QObject(p_Parent),m_canCommunicator(this),
    m_pTaskConfig(0), m_pTaskNormalOperation(0), m_pTaskShutdown(0), m_pTaskDestroy(0),
    m_pTaskDiagnostic(0), m_pTaskAdjustment(0), m_pTaskFirmwareUpdate(0)
{
    ReturnCode_t retCode;

    m_MainState        = DP_MAIN_STATE_INIT;
    m_SubStateNormalOp = DP_SUB_STATE_NORMAL_OP_START;
    m_SubStateDiag     = DP_SUB_STATE_DIAG_START;
    m_SubStateConfig   = DP_SUB_STATE_CONFIG_START;
    m_SubStateShutDown = DP_SUB_STATE_SHUTDOWN_START;
    m_SubStateAdjust   = DP_SUB_STATE_ADJUST_START;

    m_pCANMessageConfiguration = 0;

    m_pConfigurationService = 0;
    m_pDiagnosticService = 0;
    m_pAdjustmentService = 0;
    m_pShutdownService = 0;

    m_pFunctionModuleTaskManager = new CFunctionModuleTaskManager();

    m_ulCanIDMasterHeartbeat = 0;
    m_ulCANNodeIDMaster = 0;

    m_LastErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    m_LastErrorGroup = 0;
    m_LastErrorCode = 0;
    m_LastErrorData = 0;

    m_MainState = DP_MAIN_STATE_INTERNAL_CONFIG;
#ifdef HAL_CV_TEST

    // Make empty when initialization
    QVector< DCLEventLoop*> emptyVector;
    m_EventLoopsForSyncCall.swap(emptyVector);
#endif
    //Heartbeat timer initialization
    if (ftime(&m_tbTimerHeartbeatTime) != 0) {
        FILE_LOG_L(laINIT, llERROR) << " ftime failed.";
        return;
    }

    FILE_LOG_L(laINIT, llINFO) << "  starting thread DeviceProcessing";

    retCode = SetPriority();
    if (retCode != DCL_ERR_FCT_CALL_SUCCESS) {
        return;
    }

    retCode = InitTasks();
    if(retCode != DCL_ERR_FCT_CALL_SUCCESS) {
        return;
    }

    //Read the hardware settings
    retCode = ReadHWSettings();
    if (retCode != DCL_ERR_FCT_CALL_SUCCESS) {
        return;
    }

    //Setup communication interface, usually CAN, for test purpose TCP
    retCode = InitCommunication();
    if (retCode != DCL_ERR_FCT_CALL_SUCCESS) {
        FILE_LOG_L(laDEVPROC, llERROR) << " DeviceProcessing: error: " << retCode << "!"; //lint !e641
        return;
    }
    CONNECTSIGNALSLOT(this, SigStopCommandExec(quint8), this, OnStopCommandExec(quint8));

} //lint !e1566

/****************************************************************************/
/*!
 *  \brief  Destructor of DeviceProcessing
 */
/****************************************************************************/
DeviceProcessing::~DeviceProcessing()
{
    try
    {
        while (!m_lstTasks.isEmpty()) {
            delete m_lstTasks.takeFirst();
        }
        while (!m_ObjectTree.isEmpty())
        {
            delete m_ObjectTree.takeFirst();
        }

        while (!m_DeviceList.isEmpty()) {
            delete m_DeviceList.takeFirst();
        }
    }
    catch (...) //(const std::bad_alloc &)
    {
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  Forwards the error information from a device to IDeviceProcessing
 *
 *      The function forwards the error information to the IDeviceProcessing interface class
 *      (which finally throws the signal assigned to the errors)
 *
 *  \iparam InstanceID = The instance identifier of the device class which brought up the error
 *  \iparam ErrorGroup = Error group ID of the thrown error
 *  \iparam ErrorID = Error ID of the thrown error
 *  \iparam ErrorData = Additional error information
 *  \iparam ErrorTime = Time of error detection
 */
/****************************************************************************/
void DeviceProcessing::OnError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorID, quint16 ErrorData, QDateTime ErrorTime)
{
    ThrowError(InstanceID, ErrorGroup, ErrorID, ErrorData, ErrorTime);
}

/****************************************************************************/
/*!
 *  \brief  Forwards the error information from a function module to IDeviceProcessing
 *
 *      The function forwards the error information to the IDeviceProcessing interface class
 *      (which finally throws the signal assigned to the errors)
 *
 *  \iparam InstanceID = The instance identifier of the module which brought up the error
 *  \iparam ErrorGroup = Error group ID of the thrown error
 *  \iparam ErrorID    = Error ID of the thrown error
 *  \iparam ErrorData  = Additional error information
 *  \iparam ErrorTime  = Time of error detection
 */
/****************************************************************************/
void DeviceProcessing::ThrowError(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorID,
                                  quint16 ErrorData, QDateTime ErrorTime)
{
    FILE_LOG_L(laDEVPROC, llERROR) << "  DeviceProcessing::ThrowError (" << std::hex << InstanceID <<
                                      ", " << ErrorGroup << ", " << ErrorID << ", " << ErrorData <<  ")";
    emit ReportError(InstanceID, ErrorGroup, ErrorID, ErrorData, ErrorTime);
}

/****************************************************************************/
/*!
 *  \brief  Forwards the error information from a function module to IDeviceProcessing
 *
 *      The function forwards the error information to the IDeviceProcessing interface class
 *      (which finally throws the signal assigned to the errors)
 *
 *  \iparam InstanceID = The instance identifier of the module which brought up the error
 *  \iparam ErrorGroup = Error group ID of the thrown error
 *  \iparam ErrorID    = Error ID of the thrown error
 *  \iparam ErrorData  = Additional error information
 *  \iparam ErrorTime  = Time of error detection
 *  \iparam ErrorInfo  = Error information string
 */
/****************************************************************************/
void DeviceProcessing::ThrowErrorWithInfo(quint32 InstanceID, quint16 ErrorGroup, quint16 ErrorID,
                                          quint16 ErrorData, QDateTime ErrorTime, QString ErrorInfo)
{
    Q_UNUSED(ErrorInfo);

    FILE_LOG_L(laDEVPROC, llERROR) << "  DeviceProcessing::ThrowError (" << std::hex << InstanceID <<
                                      ", " << ErrorGroup << ", " << ErrorID << ", " << ErrorData <<  ")";
    //emit ReportError(DEVICE_INSTANCE_ID_DEVPROC, ErrorGroup, ErrorID, ErrorData, ErrorTime);
    emit ReportError(InstanceID, ErrorGroup, ErrorID, ErrorData, ErrorTime);
}

/****************************************************************************/
/*!
 *  \brief  Read the message configuration
 *
 *      Read the general configuration of the CAN bus messages and initialize
 *      the messages used by this class
 *
 *  \return Returns 0 if successfully, otherwise -1
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::ReadConfiguration()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    m_pCANMessageConfiguration = new CANMessageConfiguration();
    if(m_pCANMessageConfiguration)
    {
        retCode = m_pCANMessageConfiguration->ReadCANMessageConfigurationFile();
        if(retCode == DCL_ERR_FCT_CALL_SUCCESS)
        {
            retCode = InitCANMessages();
#if 0 //merged on 2014.2.17
            if(retCode == DCL_ERR_FCT_CALL_SUCCESS)
            {
                retCode = ReadProcessSettings();
            }
            else
            {
                m_LastErrorGroup = EVENT_GRP_DCL_CONFIGURATION;
            }
#endif //merged on 2014.2.17
        }
        else
        {
            m_pCANMessageConfiguration->GetLastError(m_LastErrorCode, m_LastErrorString);
            m_LastErrorGroup = EVENT_GRP_DCL_CONFIGURATION;
        }
    }
    else
    {
        retCode = DCL_ERR_FCT_CALL_FAILED;
    }

    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Returns a pointer to the message configuration class
 *
 *  \return Pointer to CANMessageConfiguration member
 */
/****************************************************************************/
CANMessageConfiguration* DeviceProcessing::GetMessageConfig()
{
    return m_pCANMessageConfiguration;
}

/****************************************************************************/
/*!
 *  \brief  Initialize the can messages used by this class
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::InitCANMessages()
{
    ReturnCode_t retCode = DCL_ERR_NOT_INITIALIZED;

    if(m_pCANMessageConfiguration)
    {
        retCode = DCL_ERR_FCT_CALL_SUCCESS;

        m_ulCanIDMasterHeartbeat = m_pCANMessageConfiguration->GetCANMessageID(MODULE_ID_BASEMODULE, "HeartbeatMaster", 0, m_ulCANNodeIDMaster);
        FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing::InitCANMessages (" << std::hex << m_ulCanIDMasterHeartbeat << ")";
        if(m_ulCanIDMasterHeartbeat & GET_CAN_MSG_RR_FLAG)
        {
            retCode = DCL_ERR_CANMSG_INVALID;
            SetErrorParameter(EVENT_GRP_DCL_CONFIGURATION, ERROR_DCL_CONFIG_CAN_MESSAGE_ASSIGN, 0);
        }
    }

    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Initialize the can messages used by this class
 *
 *      The function reads the 'parameter_master' section from the hardware
 *      configuration file. Because of the serial number must be provided
 *      directly after instantiating the DeviceProcessing class, these values
 *      are read here and not at the HardwareConfiguration class.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise DCL_ERR_INVALID_STATE
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::ReadHWSettings()
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_SUCCESS;

    int errorLine;
    int errorColumn;

    QString errorStr;
    QString strCANInterface;
    QString fileName = "hw_specification.xml";

    QDomDocument domDocument;

    FILE_LOG_L(laINIT, llDEBUG2) << "*******************************************";
    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing: ReadHWSettings";

    //temp. code, remove when configuration file is set from outside
    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing: m_ConfigFileName: " << GetHWConfigFile().toStdString();
    if(!GetHWConfigFile().isEmpty())
    {
        fileName = Global::SystemPaths::Instance().GetSettingsPath() + "/" + GetHWConfigFile();
    }
    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing: use config file: " << fileName.toStdString();

    fileName = Global::SystemPaths::Instance().GetSettingsPath() + "/" + GetHWConfigFile();
    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing: use config file: " << fileName.toStdString();

    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        m_LastErrorString = QObject::tr("Cannot read file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString());
        FILE_LOG_L(laINIT, llERROR) << m_LastErrorString.toStdString();
        m_LastErrorCode = ERROR_DCL_CONFIG_HW_CFG_OPEN_FAILED;

        return DCL_ERR_FCT_CALL_FAILED;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,
                                &errorColumn))
    {
        m_LastErrorString = QObject::tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr);
        FILE_LOG_L(laINIT, llERROR) << m_LastErrorString.toStdString();
        m_LastErrorCode = ERROR_DCL_CONFIG_HW_CFG_PARSE_ERROR;

        return DCL_ERR_FCT_CALL_FAILED;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "hwconfig")
    {
        m_LastErrorString = QObject::tr("The file is not a hardware config file.. Tag 'hwconfig' missed");
        FILE_LOG_L(laINIT, llERROR) << m_LastErrorString.toStdString();
        m_LastErrorCode = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR;

        return DCL_ERR_FCT_CALL_FAILED;
    }
    else if (root.hasAttribute("version")
               && root.attribute("version") != "1.0")
    {
        m_LastErrorString = QObject::tr("The hardware config files' version is not valid.");
        FILE_LOG_L(laINIT, llERROR) << m_LastErrorString.toStdString();
        m_LastErrorCode = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR;

        return DCL_ERR_FCT_CALL_FAILED;
    }

    QDomElement child = root.firstChildElement("parameter_master");
    if(child.isNull())
    {
        m_LastErrorCode = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_MST;
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        QDomElement childCAN;
        QDomElement childTcp;
        QDomElement childSerialNb;

        childCAN= child.firstChildElement("can_interface");
        if(childCAN.isNull())
        {
            m_CanInterface = "";
        }
        else
        {
            m_CanInterface = childCAN.attribute("interface");
        }

        childTcp= child.firstChildElement("tcp_interface");
        if(childTcp.isNull())
        {
            m_TcpInterface = "0";
        }
        else
        {
            m_TcpInterface = childTcp.attribute("usage");
        }

        FILE_LOG_L(laINIT, llINFO) << " CAN-interface set to :" << m_CanInterface.toStdString();
        FILE_LOG_L(laINIT, llINFO) << " TCP-interface set to :" << m_TcpInterface.toStdString();

        //read the serial number
        childSerialNb=  child.firstChildElement("serial_number");
        if(childSerialNb.isNull())
        {
            SetErrorParameter(EVENT_GRP_DCL_DEVCTRL, ERROR_DCL_DEVCTRL_SERIALNO_FAILED, 0);
            FILE_LOG_L(laINIT, llERROR) << " Serial number incorrect.";
            retCode = DCL_ERR_FCT_CALL_FAILED;
        }
        else
        {
            m_SerialNo = childSerialNb.attribute("serialno");
        }
    }

    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Returns the serial number as stored in the config file
 *
 *  \oparam SerialNo = reference to a QString object, will be filled with the serial number
 *
 *  \return TRUE if the serial number can be filled, otherwise FALSE
 */
/****************************************************************************/
bool DeviceProcessing::GetSerialNumber(QString& SerialNo)
{
    int errorLine;
    int errorColumn;

    QString errorStr;
    QString fileName;

    QDomDocument domDocument;

    FILE_LOG_L(laINIT, llDEBUG2) << "*******************************************";
    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing: GetSerialNumber";

    //temp. code, remove when configuration file is set from outside
    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing: m_ConfigFileName: " << GetHWConfigFile().toStdString();
    FILE_LOG_L(laINIT, llINFO) << " GetSettingsPath: " << Global::SystemPaths::Instance().GetSettingsPath() .toStdString();
    if(!GetHWConfigFile().isEmpty())
    {
        fileName = Global::SystemPaths::Instance().GetSettingsPath() + "/" + m_HWConfigFileName;
    }
    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing: use config file: " << fileName.toStdString();

    QFile file(fileName);

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QString ErrorString = QObject::tr("Cannot read file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString());
        FILE_LOG_L(laINIT, llERROR) << ErrorString.toStdString();

        return false;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,
                                &errorColumn))
    {
        QString ErrorString = QObject::tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr);
        FILE_LOG_L(laINIT, llERROR) << ErrorString.toStdString();

        return false;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "hwconfig")
    {
        QString ErrorString = QObject::tr("The file is not a himalaya hardware config file. Tag 'hwconfig' missed");
        FILE_LOG_L(laINIT, llERROR) << ErrorString.toStdString();

        return false;
    }
    else if (root.hasAttribute("version")
               && root.attribute("version") != "1.0")
    {
        QString ErrorString = QObject::tr("The himalaya hardware config files' version is not valid.");
        FILE_LOG_L(laINIT, llERROR) << ErrorString.toStdString();

        return false;
    }

    QDomElement child = root.firstChildElement("parameter_master");
    if(child.isNull())
    {
        return false;
    }
    else
    {
        QDomElement childSerialNb;

        //read the serial number
        childSerialNb=  child.firstChildElement("serial_number");
        if(childSerialNb.isNull())
        {
            FILE_LOG_L(laINIT, llERROR) << " Serial number incorrect.";
            return false;
        }
        else
        {
            SerialNo = childSerialNb.attribute("serialno");
        }
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Initialize the task list
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if done, otherwise the error code
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::InitTasks()
{
    ReturnCode_t retCode;

    //and the tasks
    m_pTaskConfig =          new DeviceProcTask(DeviceProcTask::TASK_ID_DP_CONFIG, DeviceProcTask::TASK_PRIO_VERYHIGH);
    m_pTaskNormalOperation = new DeviceProcTask(DeviceProcTask::TASK_ID_DP_NORMAL_OPERATION, DeviceProcTask::TASK_PRIO_LOW);
    m_pTaskShutdown =        new DeviceProcTask(DeviceProcTask::TASK_ID_DP_SHUTDOWN, DeviceProcTask::TASK_PRIO_HIGH);
    m_pTaskDestroy =         new DeviceProcTask(DeviceProcTask::TASK_ID_DP_DESTROY, DeviceProcTask::TASK_PRIO_HIGH);
    m_pTaskDiagnostic =      new DeviceProcTask(DeviceProcTask::TASK_ID_DP_DIAGNOSTIC, DeviceProcTask::TASK_PRIO_MIDDLE);
    m_pTaskAdjustment =      new DeviceProcTask(DeviceProcTask::TASK_ID_DP_ADJUST, DeviceProcTask::TASK_PRIO_MIDDLE);
    m_pTaskFirmwareUpdate =  new DeviceProcTask(DeviceProcTask::TASK_ID_DP_FW_UPDATE, DeviceProcTask::TASK_PRIO_VERYHIGH);

    m_lstTasks.append(m_pTaskConfig);
    m_lstTasks.append(m_pTaskNormalOperation);
    m_lstTasks.append(m_pTaskShutdown);
    m_lstTasks.append(m_pTaskDestroy);
    m_lstTasks.append(m_pTaskDiagnostic);
    m_lstTasks.append(m_pTaskAdjustment);
    m_lstTasks.append(m_pTaskFirmwareUpdate);

    retCode = ActivateTask(DeviceProcTask::TASK_ID_DP_NORMAL_OPERATION, 0, 0);

    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Activate a task
 *
 *      The task, with taksID matches, will be set to state STARTUP.
 *      The task will become active as soon as the actual active task has
 *      finished
 *
 *  \iparam TaskID = task ID of the task to activate
 *  \iparam TaskParameter1 = additional parameter for the task
 *  \iparam TaskParameter2 = additional parameter for the task
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the task was found,
 *          otherwise DCL_ERR_FCT_CALL_NOT_FOUND
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::ActivateTask(DeviceProcTask::TaskID_t TaskID, quint16 TaskParameter1, quint16 TaskParameter2)
{
    ReturnCode_t retCode = DCL_ERR_FCT_CALL_NOT_FOUND;

    Q_UNUSED(TaskParameter1);
    Q_UNUSED(TaskParameter2);

    QListIterator<DeviceProcTask*> iterTaskList(m_lstTasks);
    DeviceProcTask* pTask;

    iterTaskList.toFront();
    while(iterTaskList.hasNext())
    {
        pTask = iterTaskList.next();
        if(pTask->m_taskID == TaskID)
        {
            pTask->m_state = DeviceProcTask::TASK_STATE_STARTUP;
            FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing: ActivateTask:" << (int) pTask->m_taskID;
            retCode = DCL_ERR_FCT_CALL_SUCCESS;
            break;
        }
    }

    return retCode;
}

/****************************************************************************/
/*!
 * \brief  Return the active task from the task list
 *
 * \iparam pActTask = The task which is set to active state
 *
 * \return Pointer to active ObjectTask*
 */
/****************************************************************************/
DeviceProcTask* DeviceProcessing::GetActiveTask(DeviceProcTask* pActTask)
{
    static DeviceProcTask::TaskID_t lastTaskID = DeviceProcTask::TASK_ID_DP_UNDEF;
    DeviceProcTask* pHighestPriorityTask = 0;
    DeviceProcTask* pTask = 0;

    if(pActTask)
    {
        if(pActTask->m_state == DeviceProcTask::TASK_STATE_FINSHED)
        {
            FILE_LOG_L(laDEVPROC, llINFO) << "  Task " << (int)pActTask->m_taskID << " will be set to 'sleep'";
            pActTask->m_state = DeviceProcTask::TASK_STATE_SLEEP;
            pActTask = 0;
        }
        else if(pActTask->m_state == DeviceProcTask::TASK_STATE_PAUSE)
        {
            FILE_LOG_L(laDEVPROC, llINFO) << "  Task " << (int)pActTask->m_taskID << " will be paused";
            pActTask = 0;
        }
    }

    // the task list keeps all tasks available for execution, and the normal operation mode task,
    // Usually a task is inactive, its m_state == TASK_STATE_SLEEP
    // To start a task, its m_state must be set to TASK_STATE_STARTUP, this is done in 'ActivateTask function
    // At this function, we search for a task which m_state was set to  TASK_STATE_STARTUP recently.
    // It its priority is higher than the acitve task's one, the active task will be set to TASK_STATE_ACTIVE_BRK
    // -> this signals the task to finish its operation.
    QListIterator<DeviceProcTask*> iterTaskList(m_lstTasks);

    iterTaskList.toFront();
    pHighestPriorityTask = pActTask;
    while(iterTaskList.hasNext())
    {
        pTask = iterTaskList.next();
        if(pTask->m_state == DeviceProcTask::TASK_STATE_STARTUP)
        {
            if(pHighestPriorityTask == 0)
            {
                pHighestPriorityTask = pTask;
            }
            else if(pTask->m_priority > pHighestPriorityTask->m_priority)
            {
                pHighestPriorityTask = pTask;
            }
        }
    }

    if((pHighestPriorityTask == 0) || (pActTask == pHighestPriorityTask))
    {
        ; // do nothing, the task with the highest priority is already active
    }
    else if(pActTask == 0 || (pActTask->m_state == DeviceProcTask::TASK_STATE_ACTIVE_BRK))
    {
        // we haven't got an active task, activate the one with the highest priority
        pActTask = pHighestPriorityTask;
        FILE_LOG_L(laDEVPROC, llINFO) << "    DeviceProcessing, new active task: " << (int)pActTask->m_taskID ;
        pActTask->m_state = DeviceProcTask::TASK_STATE_ACTIVE;
    }
    else
    {
        //we have a pending task with higher priority, the active task should be finished
        if(pActTask->m_state == DeviceProcTask::TASK_STATE_ACTIVE)
        {
            FILE_LOG_L(laDEVPROC, llINFO) << "  break task " << (int)pActTask->m_taskID ;
            pActTask->m_state = DeviceProcTask::TASK_STATE_ACTIVE_BRK;
        }
    }

    if((pActTask != 0) && (pActTask->m_taskID != lastTaskID))
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing, new task: " << (int)pActTask->m_taskID << " state:  " << (int)pActTask->m_state << " pr:  " << (int)pActTask->m_priority;
        lastTaskID = pActTask->m_taskID;
    }

    return pActTask;
}

/****************************************************************************/
/*!
 *  \brief  Finishes a task
 *
 *      The function marks the task as finished and does some logging. The
 *      task will be deleted in function GetActiveTask
 *
 *  \iparam pActiveTask = The task to finish
 */
/****************************************************************************/
void DeviceProcessing::TaskFinished(DeviceProcTask* pActiveTask)
{
    pActiveTask->m_state = DeviceProcTask::TASK_STATE_FINSHED;
    FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing: Task " << (int)pActiveTask->m_taskID << " TASK_STATE_FINSHED";

    if((m_pTaskNormalOperation) &&
       (m_pTaskNormalOperation->m_state == DeviceProcTask::TASK_STATE_SLEEP))
    {
        m_pTaskNormalOperation->m_state = DeviceProcTask::TASK_STATE_STARTUP;
        FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing: Task " << (int)pActiveTask->m_taskID << " TASK_STATE_STARTUP";
    }
}


/****************************************************************************/
/*!
 *  \brief  Set the priority of the thread context
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if priority was set,
 *          otherwise DCL_ERR_FCT_CALL_FAILED
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::SetPriority()
{
    sched_param p;
    int policy;

    policy = sched_getscheduler(0);
    if(policy == -1)
    {
        FILE_LOG_L(laDEVPROC, llERROR) << "  cannot set priority: " << (int) errno;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    p.sched_priority = sched_get_priority_max(SCHED_RR);

    FILE_LOG_L(laDEVPROC, llERROR) << "  set priority: " << (int) p.sched_priority ;
    if((policy = sched_setscheduler(0, SCHED_RR, &p)) == -1)
    {
        /// \todo finally remove this!!
        bool RunningAtHost = true;

        if(errno == EINVAL)
            FILE_LOG_L(laDEVPROC, llERROR) << "  cannot set priority (EINVAL): " << (int) errno;
        else if(errno == EPERM)
            FILE_LOG_L(laDEVPROC, llERROR) << "  cannot set priority (EPERM): " << (int) errno;
        else if(errno == ESRCH)
            FILE_LOG_L(laDEVPROC, llERROR) << "  cannot set priority (ESRCH): " << (int) errno;
        else
            FILE_LOG_L(laDEVPROC, llERROR) << "  cannot set priority, I don't know why ";

        if(RunningAtHost)
        {
            //At host its not possible to set high priority, because we are running not as root
        }
        else
        {
            //at the target system, running as root, setting priority shall work!
            return DCL_ERR_FCT_CALL_FAILED;
        }
    }
    else
    {
        FILE_LOG_L(laDEVPROC, llINFO) << " set priority from : " << (int) policy << " to " << (int) p.sched_priority;
    }

    policy = sched_getscheduler(0);

    FILE_LOG_L(laDEVPROC, llINFO) << " priority : " << (int) policy;

    return DCL_ERR_FCT_CALL_SUCCESS;
}

/****************************************************************************/
/*!
 *  \brief  Initialize the communication via CAN or TCP/IP
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if priority was set,
 *          otherwise DCL_ERR_FCT_CALL_FAILED
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::InitCommunication()
{
    //CONNECTSIGNALSLOT(&m_canCommunicator, ReportError(quint32, quint16, quint16, quint16, QDateTime),
    //                    this->GetParent(), OnError(quint32, quint16, quint16, quint16, QDateTime));
    CONNECTSIGNALSLOT(&m_canCommunicator, ReportError(quint32, quint16, quint16, quint16, QDateTime),
                        this, OnError(quint32, quint16, quint16, quint16, QDateTime));

    FILE_LOG_L(laDEVPROC, llINFO) << "  start CAN communication";
    return m_canCommunicator.StartComm(m_CanInterface.toStdString().c_str());
}

/****************************************************************************/
/*!
 *  \brief The thread's main loop
 *
 *      The starting point for the thread. The function implements the
 *      thread's main loop.
 */
/****************************************************************************/
void DeviceProcessing::HandleTasks()
{ 
/*
    struct timeval tv_end;
    struct timeval tv_start;
    static quint32 RunThruCounter = 0;
    bool PerformanceCheck = false;
*/

    static quint16 stTest = 0;
    static quint32 RunThruCounter = 0;
    Q_UNUSED(RunThruCounter);

    static DeviceProcTask* pActiveTask = NULL;
    static DeviceProcTask::TaskID_t stTaskID = DeviceProcTask::TASK_ID_DP_UNDEF;

/*
    //Performance check initialisation
    if (PerformanceCheck)
    {
        if(gettimeofday(&tv_start, 0) != 0)
        {
            PerformanceCheck = false;
            FILE_LOG_L(laDEVPROC, llERROR) << " Perfomance check failed!";
        }
    }
*/
    pActiveTask = GetActiveTask(pActiveTask);
    if((pActiveTask != 0) && (stTaskID != pActiveTask->m_taskID))
    {
        FILE_LOG_L(laDEVPROC, llINFO) << " DeviceProcessing: new TaskID: " << (int) pActiveTask->m_taskID;
        stTaskID = pActiveTask->m_taskID;
        FILE_LOG_L(laDEVPROC, llINFO) << " DeviceProcessing: new TaskID: " << (int) pActiveTask->m_taskID;
    }

    switch (m_MainState)
    {
        case DP_MAIN_STATE_INTERNAL_CONFIG:
        {
            ReturnCode_t retCodeCfg = DCL_ERR_FCT_CALL_SUCCESS;

            FILE_LOG_L(laDEVPROC, llINFO) << " handle DeviceProcessing: DP_MAIN_STATE_INTERNAL_CONFIG";

            RegisterMetaTypes();

            retCodeCfg = ReadConfiguration();
            if(retCodeCfg == DCL_ERR_FCT_CALL_SUCCESS)
            {
                FILE_LOG_L(laDEVPROC, llINFO) << "DeviceProcessing: DP_MAIN_STATE_INTERNAL_CONFIG finished -> change to DP_MAIN_STATE_WAIT_FOR_CONFIG";

                //its important to set the state variables before routing the event.
                m_MainState = DP_MAIN_STATE_WAIT_FOR_CONFIG;
                emit ReportInitializationFinished(retCodeCfg);
            }
            else
            {
                // it doesn't make sense to work without can message configuration
                m_MainState = DP_MAIN_STATE_ERROR;
                emit ReportInitializationFinished(retCodeCfg);
            }
            break;
        }
        case DP_MAIN_STATE_WAIT_FOR_CONFIG:
        case DP_MAIN_STATE_CONFIG:
            if(pActiveTask)
            {
                if(pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_CONFIG)
                {
                    HandleTaskConfig(pActiveTask);
                }
                else if(pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_NORMAL_OPERATION)
                {
                    pActiveTask->m_state = DeviceProcTask::TASK_STATE_FINSHED;
                }
                else if(pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_DESTROY)
                {
                    FILE_LOG_L(laDEVPROC, llINFO) << "  while DP_MAIN_STATE_CONFIG, destroy";
                }
                else
                {
                    FILE_LOG_L(laDEVPROC, llINFO) << " DeviceProcessing: config state, but no config Task: " << (int) pActiveTask->m_taskID; //pfui
                }
            }
            break;
        case DP_MAIN_STATE_WAIT_FOR_ADJUST:
        case DP_MAIN_STATE_ADJUST:
            if(pActiveTask)
            {
                if(pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_ADJUST)
                {
                    HandleTaskAdjustment(pActiveTask);
                }
                else
                {
                    ; //pfui
                }
            }
            break;
        case DP_MAIN_STATE_IDLE:
            if(pActiveTask != 0)
            {
                if(pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_NORMAL_OPERATION)
                {
                    stTest++;
                    if(stTest > 500)
                    {
                        FILE_LOG_L(laDEVPROC, llINFO) << "   HandleTaskNormalOperation";
                        stTest = 0;
                    }
                    HandleTaskNormalOperation(pActiveTask);
                }
                else if((pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_SHUTDOWN) ||
                        (pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_DESTROY))
                {
                    HandleTaskShutDown(pActiveTask);
                    if(m_SubStateShutDown == DP_SUB_STATE_SHUTDOWN_FINISHED)
                    {
                        FILE_LOG_L(laDEVPROC, llINFO) << "   HandleTaskShutDown finished, next exit";
                        emit ReportDestroyFinished();
                    }
                }
                else if(pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_ADJUST)
                {
                    HandleTaskAdjustment(pActiveTask);
                }
                else if(pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_DIAGNOSTIC)
                {
                    HandleTaskDiagnostic(pActiveTask);
                }
                else if(pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_FW_UPDATE)
                {
                }
                else
                {
                    /// \todo handle error state
                    FILE_LOG_L(laDEVPROC, llERROR) << " Invalid task ID";
                }
            }
            break;
        case DP_MAIN_STATE_ERROR:
            if(pActiveTask != 0)
            {
                if((pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_SHUTDOWN) ||
                        (pActiveTask->m_taskID == DeviceProcTask::TASK_ID_DP_DESTROY))
                {
                    HandleTaskShutDown(pActiveTask);
                    if(m_SubStateShutDown == DP_SUB_STATE_SHUTDOWN_FINISHED)
                    {
                        FILE_LOG_L(laDEVPROC, llINFO) << "   HandleTaskShutDown finished, next exit";
                        emit ReportDestroyFinished();
                    }
                }
            }
            break;
        default:
            break;
    }
/*
    if(PerformanceCheck)
    {
        if(gettimeofday(&tv_end, 0) != 0)
        {
            PerformanceCheck = false;
            FILE_LOG_L(laDEVPROC, llINFO) << "Performance check failed";
        }
        else
        {
            long usec;
            QString result = QString(" %1.%2 - %3.%4").arg(tv_start.tv_sec).arg(tv_start.tv_usec).arg(tv_end.tv_sec).arg(tv_end.tv_usec);

            RunThruCounter++;
            usec = (tv_end.tv_sec - tv_start.tv_sec)*1000000;
            usec += (tv_end.tv_usec - tv_start.tv_usec);
            if(usec > 999)
            {
                FILE_LOG_L(laDEVPROC, llINFO) << result.toStdString() << ", " << std::dec << usec << ", idx:" << RunThruCounter;
            }
        }
    }

    if (m_MainState != DP_MAIN_STATE_ERROR) {
        CheckMasterHeartbeat();
    }
    */
}

/****************************************************************************/
/*!
 *  \brief  The task function for the configuration task
 *
 *  \iparam pActiveTask = The active task, used to inform about task changes (break - pause - ..)
 */
/****************************************************************************/
void DeviceProcessing::HandleTaskConfig(DeviceProcTask* pActiveTask)
{
    static quint16 Counter = 0;

    if(Counter++ == 1000)
    {
        FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing:  HandleTaskConfig : " << (int) m_SubStateConfig;
        Counter = 0;
    }

    switch(m_SubStateConfig)
    {
        case (DP_SUB_STATE_CONFIG_INIT):
            m_SubStateConfig = DP_SUB_STATE_CONFIG_REGISTER;
            break;
        case (DP_SUB_STATE_CONFIG_REGISTER):
            m_SubStateConfig = DP_SUB_STATE_CONFIG_START;
            break;
        case (DP_SUB_STATE_CONFIG_START):
            if(!m_pConfigurationService)
            {
                //create configuration service
                m_pConfigurationService = new CConfigurationService(this, &m_canCommunicator);
            }
            m_SubStateConfig = DP_SUB_STATE_CONFIG_CONFIG;
            break;
        case (DP_SUB_STATE_CONFIG_CONFIG):
            if(m_pConfigurationService)
            {
                //handle configuration service tasks
                m_pConfigurationService->HandleTasks();
                //handle CAN nodes tasks
                HandleCANNodesTask();
                if(m_pConfigurationService->GetState() == CConfigurationService::CS_MAIN_STATE_IDLE)
                {
                    m_SubStateConfig = DP_SUB_STATE_CONFIG_INIT_DEVICES;
                    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing:  m_SubStateConfig = DP_SUB_STATE_CONFIG_INIT_DEVICES";
                }
                else if(m_pConfigurationService->GetState() == CConfigurationService::CS_MAIN_STATE_ERROR)
                {
                    m_SubStateConfig = DP_SUB_STATE_CONFIG_ERROR;
                }
            }
            else
            {
                FILE_LOG_L(laINIT, llERROR) << " DeviceProcessing ERROR:  m_SubStateConfig = DP_SUB_STATE_CONFIG_CONFIG, not initialized";
            }
            break;
        case (DP_SUB_STATE_CONFIG_INIT_DEVICES):
            FILE_LOG_L(laINIT, llINFO) << "   DeviceProcessing:  DP_SUB_STATE_CONFIG_INIT_DEVICES";
            HandleCANNodesTask();
            FILE_LOG_L(laINIT, llDEBUG1) << "   DeviceProcessing:  DP_SUB_STATE_CONFIG_INIT_DEVICES 1";
            FILE_LOG_L(laINIT, llDEBUG1) << "   DeviceProcessing:  DP_SUB_STATE_CONFIG_INIT_DEVICES 2";
            m_SubStateConfig = DP_SUB_STATE_CONFIG_CFG_DEVICES;
            break;
        case DP_SUB_STATE_CONFIG_CFG_DEVICES:
            {
                QListIterator<CBaseDevice*> iter(m_DeviceList);
                CBaseDevice* pDevice;
                bool NonIdleFound = false;

                HandleCANNodesTask();
                HandleDevicesTask();

                while (iter.hasNext())
                {
                    pDevice = iter.next();

                    if(pDevice->GetMainState() == CBaseDevice::DEVICE_MAIN_STATE_ERROR)
                    {
                        FILE_LOG_L(laINIT, llERROR) << "   DeviceProcessing: " << pDevice->GetType().toStdString() << " DEVICE_MAIN_STATE_ERROR";
                        m_SubStateConfig = DP_SUB_STATE_CONFIG_ERROR;
                        break;
                    }
                    else if(pDevice->GetMainState() != CBaseDevice::DEVICE_MAIN_STATE_IDLE)
                    {
                        FILE_LOG_L(laINIT, llWARNING) << "   DeviceProcessing: " << pDevice->GetType().toStdString() << " not idle";
                        NonIdleFound = true;
                        break;
                    }
                }
                if(NonIdleFound == false)
                {
                    m_SubStateConfig = DP_SUB_STATE_CONFIG_FINISHED;
                }
            }
            break;
        case (DP_SUB_STATE_CONFIG_FINISHED):
            FILE_LOG_L(laDEVPROC, llINFO) << "DeviceProcessing: DP_MAIN_STATE_CONFIG finished";
            if(m_pConfigurationService != NULL)
            {
                m_MainState = DP_MAIN_STATE_IDLE; //merged on 2014.2.17
                if(m_pConfigurationService->ConfigurationComplete() == true) {
                    emit ReportConfigurationFinished(DCL_ERR_FCT_CALL_SUCCESS);
                }
                else
                {
                    emit ReportConfigurationFinished(DCL_ERR_TIMEOUT);
                }
            }
            m_SubStateConfig = DP_SUB_STATE_CONFIG_FINISHED;
            TaskFinished(pActiveTask);
            //m_MainState = DP_MAIN_STATE_WAIT_FOR_ADJUST;
            //m_MainState = DP_MAIN_STATE_IDLE; //merged on 2014.2.17
            break;
        case (DP_SUB_STATE_CONFIG_ERROR):
            FILE_LOG_L(laDEVPROC, llINFO) << "DeviceProcessing: DP_MAIN_STATE_CONFIG finished with error";
            emit ReportConfigurationFinished(DCL_ERR_FCT_CALL_FAILED);
            m_SubStateConfig = DP_SUB_STATE_CONFIG_FINISHED;
            TaskFinished(pActiveTask);
            m_MainState = DP_MAIN_STATE_ERROR;
            break;
        case (DP_SUB_STATE_CONFIG_UNDEF):
            m_SubStateConfig = DP_SUB_STATE_CONFIG_ERROR;
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief  The task function for the normal operation task
 *
 *  \iparam pActiveTask = The active task, used to inform about task changes (break - pause - ..)
 */
/****************************************************************************/
void DeviceProcessing::HandleTaskNormalOperation(DeviceProcTask* pActiveTask)
{
    if(pActiveTask->m_state == DeviceProcTask::TASK_STATE_ACTIVE_BRK)
    {
        QDateTime errorTimeStamp;

        pActiveTask->m_state = DeviceProcTask::TASK_STATE_PAUSE;
        FILE_LOG_L(laDEVPROC, llINFO) << "  pause task 'normal operation'";
        errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
        //ThrowError(DEVICE_INSTANCE_ID_UNDEFINED, EVENT_GRP_DCL_DEVCTRL, EVENT_DCL_DEVCTRL_BREAK_NORMAL_OP, 0, errorTimeStamp);

        return;
    }

    if(m_SubStateNormalOp == DP_SUB_STATE_NORMAL_OP_RUN)
    {
        HandleCANNodesTask();
        HandleDevicesTask();
    }
    else if(m_SubStateNormalOp == DP_SUB_STATE_NORMAL_OP_START)
    {
        m_SubStateNormalOp = DP_SUB_STATE_NORMAL_OP_CONFIG;
        FILE_LOG_L(laDEVPROC, llINFO) << "  HandleTaskNormalOperation : State OP_CONFIG";
    }
    else if(m_SubStateNormalOp == DP_SUB_STATE_NORMAL_OP_CONFIG)
    {
        HandleCANNodesTask();
        HandleDevicesTask();
        m_SubStateNormalOp = DP_SUB_STATE_NORMAL_OP_RUN;
        emit ReportStartNormalOperationMode(DCL_ERR_FCT_CALL_SUCCESS);
    }
}

/****************************************************************************/
/*!
 *  \brief  The task function for the diagnostic task
 *
 *  \iparam  pActiveTask = The active task, used to inform about task changes (break - pause - ..)
 */
/****************************************************************************/
void DeviceProcessing::HandleTaskDiagnostic(DeviceProcTask* pActiveTask)
{
    if(pActiveTask->m_state == DeviceProcTask::TASK_STATE_ACTIVE_BRK)
    {
        pActiveTask->m_state = DeviceProcTask::TASK_STATE_FINSHED;
        FILE_LOG_L(laDEVPROC, llINFO) << "  finish diag task";

        emit ReportDiagnosticServiceClosed(4714);
        m_SubStateDiag = DP_SUB_STATE_DIAG_FINISHED;
        return;
    }

    if(m_SubStateDiag == DP_SUB_STATE_DIAG_START)
    {
        if(!m_pDiagnosticService)
        {
            m_pDiagnosticService = new CDiagnosticService(this, &m_canCommunicator, &m_ObjectTree);
        }
        m_SubStateDiag = DP_SUB_STATE_DIAG_INIT;
    }
    else if(m_SubStateDiag == DP_SUB_STATE_DIAG_INIT)
    {
        m_SubStateDiag = DP_SUB_STATE_DIAG_CONFIG;
        // internal initialisation
    }
    else if(m_SubStateDiag == DP_SUB_STATE_DIAG_CONFIG)
    {
        QDateTime errorTimeStamp;
        m_SubStateDiag = DP_SUB_STATE_DIAG_IDLE;
        errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
        //ThrowError(DEVICE_INSTANCE_ID_UNDEFINED, EVENT_GRP_DCL_DEVCTRL, EVENT_DCL_DEVCTRL_START_DIAG, 0, errorTimeStamp);
    }
    else if(m_SubStateDiag == DP_SUB_STATE_DIAG_IDLE)
    {
        if(m_pDiagnosticService)
        {
            m_pDiagnosticService->HandleTasks();
        }
    }

    HandleCANNodesTask();
    HandleDevicesTask();
}

/****************************************************************************/
/*!
 *  \brief  The task function for the adjustment task
 *
 *  \iparam pActiveTask = The active task, used to inform about task changes (break - pause - ..)
 */
/****************************************************************************/
void DeviceProcessing::HandleTaskAdjustment(DeviceProcTask* pActiveTask)
{
    static quint8 sTest = 0;

    Q_UNUSED(pActiveTask);

    if(sTest==0)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing::HandleTaskAdjustment called.";
        sTest = 1;
    }

    if(m_pAdjustmentService == 0)
    {
        m_pAdjustmentService = new CAdjustmentService(this, &m_canCommunicator, &m_ObjectTree);
    }

    HandleCANNodesTask();
    HandleDevicesTask();
    m_pAdjustmentService->HandleTasks();
}

/****************************************************************************/
/*!
 *  \brief  The task function for the shutdown task
 *
 *  \iparam pActiveTask = The active task, used to inform about task changes (break - pause - ..)
 */
/****************************************************************************/
void DeviceProcessing::HandleTaskShutDown(DeviceProcTask* pActiveTask)
{
    Q_UNUSED(pActiveTask);

    if(m_SubStateShutDown == DP_SUB_STATE_SHUTDOWN_START)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing::HandleTaskShutDown DP_SUB_STATE_SHUTDOWN_START.";
        if(!m_pShutdownService)
        {
            //create configuration service
            m_pShutdownService = new CShutdownService(this, &m_canCommunicator, &m_ObjectTree);
        }
        m_SubStateShutDown = DP_SUB_STATE_SHUTDOWN_NODES;
    }
    else if(m_SubStateShutDown == DP_SUB_STATE_SHUTDOWN_NODES)
    {
        if(m_pShutdownService)
        {
            //handle shutdown service tasks
            m_pShutdownService->HandleTasks();
            HandleCANNodesTask();

            if(m_pShutdownService->GetState() == CShutdownService::CS_MAIN_STATE_FINISHED)
            {
                m_SubStateShutDown = DP_SUB_STATE_SHUTDOWN_CLOSE_COMM;
                FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing:  m_SubStateConfig = DP_SUB_STATE_SHUTDOWN_CLOSE_COMM";
            }
            else if(m_pConfigurationService->GetState() == CConfigurationService::CS_MAIN_STATE_ERROR)
            {
                m_SubStateShutDown = DP_SUB_STATE_SHUTDOWN_CLOSE_COMM;
            }
        }
    }
    else if(m_SubStateShutDown == DP_SUB_STATE_SHUTDOWN_CLOSE_COMM)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing::HandleTaskShutDown DP_SUB_STATE_SHUTDOWN_CLOSE_COMM.";
        m_canCommunicator.StopComm();
        m_SubStateShutDown = DP_SUB_STATE_SHUTDOWN_CLOSE_OBJECTS;
    }
    else if(m_SubStateShutDown == DP_SUB_STATE_SHUTDOWN_CLOSE_OBJECTS)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing::HandleTaskShutDown start DP_SUB_STATE_SHUTDOWN_CLOSE_OBJECTS";
        delete m_pCANMessageConfiguration;

        // delete the services
        delete m_pConfigurationService;
        delete m_pDiagnosticService;
        delete m_pAdjustmentService;
        delete m_pShutdownService;
        delete m_pFunctionModuleTaskManager;

        // delete CANNodes
        while(!m_ObjectTree.isEmpty())
        {
            CBaseModule *p_BaseModule = m_ObjectTree.takeLast();
            p_BaseModule->DeleteAllFunctionModules();
            delete p_BaseModule;
        }

        m_SubStateShutDown = DP_SUB_STATE_SHUTDOWN_FINISHED;
    }
}

/****************************************************************************/
/**
 *  \brief Registers data types
 *
 *      Registers the types declared in this layer and used by signal
 *      slot mechanism.
 */
/****************************************************************************/
void DeviceProcessing::RegisterMetaTypes()
{
    qRegisterMetaType<TestResult_t>("TestResult_t");
    qRegisterMetaType<ReturnCode_t>("ReturnCode_t");
    qRegisterMetaType<NodeState_t>("NodeState_t");
    qRegisterMetaType<TempCtrlStatus_t>("TempCtrlStatus_t");
    qRegisterMetaType<TempCtrlOperatingMode_t>("TempCtrlOperatingMode_t");
    qRegisterMetaType<TempCtrlMainsVoltage_t>("TempCtrlMainsVoltage_t");
}

/****************************************************************************/
/*!
 *  \brief  Calls the HandleTasks function of all nodes in system
 */
/****************************************************************************/
void DeviceProcessing::HandleCANNodesTask()
{
    QListIterator<CBaseModule *> iter(m_ObjectTree);
    CBaseModule* pCANNode;
    while (iter.hasNext())
    {
        pCANNode = iter.next();
        pCANNode->HandleTasks();
    }
}

/****************************************************************************/
/*!
 *  \brief  Calls the HandleTasks function of all devices
 */
/****************************************************************************/
void DeviceProcessing::HandleDevicesTask()
{
    static quint16 stTest = 2505;

    stTest++;
    if(stTest > 2500)
    {
        FILE_LOG_L(laDEVPROC, llINFO) << " DeviceProcessing::handleDevicesTask";
        stTest = 0;
    }

    QListIterator<CBaseDevice*> iter(m_DeviceList);
    CBaseDevice* pDevice;

    while (iter.hasNext())
    {
        pDevice = iter.next();
        pDevice->HandleTasks();
    }
}

/****************************************************************************/
/*!
 *  \brief  Checks if the heartbeat can message must be send
 *
 *      This function checks if a new heartbeat can message must be send. The
 *      heartbeat interval is defined, the delay is measured with a timeb
 *      struct.
 */
/****************************************************************************/
void DeviceProcessing::CheckMasterHeartbeat()
{
    struct timeb tp;
    int nMilliSeconds, nSeconds;
    static unsigned int nLfdNr = 0;

    if(ftime(&tp) == 0)
    {
        nSeconds = tp.time - m_tbTimerHeartbeatTime.time;

        if(tp.millitm < m_tbTimerHeartbeatTime.millitm)
        {
            nMilliSeconds = tp.millitm + (1000-m_tbTimerHeartbeatTime.millitm);
            nSeconds--;
        }
        else
            nMilliSeconds = tp.millitm - m_tbTimerHeartbeatTime.millitm;

        nMilliSeconds = ((int)(nSeconds*1000)) + nMilliSeconds;

        if(nMilliSeconds > 1000)
        {
            ReturnCode_t retval;
            struct can_frame can_frameHeartbeat;

            can_frameHeartbeat.can_id = m_ulCanIDMasterHeartbeat;
            can_frameHeartbeat.data[0] = (__u8) nLfdNr++;
            can_frameHeartbeat.data[1] = 0x02;
            can_frameHeartbeat.data[2] = 0x03;
            can_frameHeartbeat.data[3] = 0x04;
            can_frameHeartbeat.data[4] = 0x05;
            can_frameHeartbeat.data[5] = 0x06;
            can_frameHeartbeat.data[6] = 0x07;
            can_frameHeartbeat.data[7] = 0x08;

            can_frameHeartbeat.can_dlc = 8;
            retval = m_canCommunicator.SendCOB(can_frameHeartbeat);

            if(ftime(&m_tbTimerHeartbeatTime) || retval != DCL_ERR_FCT_CALL_SUCCESS)
            {
                QDateTime errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
                ThrowError(DEVICE_INSTANCE_ID_DEVPROC, EVENT_GRP_DCL_DEVCTRL, ERROR_DCL_DEVCTRL_HEARTBEAT_ERROR, 0, errorTimeStamp);
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Read the process setting file
 *
 *      This function reads the process setting from file and stores the
 *      setting for later access.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS in successfully read,
 *          otherwise error code
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::ReadProcessSettings()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    QDomDocument domDocument;
    QDomElement root;
    QString ProcessSettingsFileName = QString("ProcessSettings.xml");
    QString strErrorInfo;
    QString errorStr;
    int errorLine;
    int errorColumn;

    ProcessSettingsFileName = Global::SystemPaths::Instance().GetSettingsPath() + "/" + ProcessSettingsFileName;
    FILE_LOG_L(laINIT, llINFO) << " DeviceProcessing: process settings file: " << ProcessSettingsFileName.toStdString();

    QFile file(ProcessSettingsFileName);

    FILE_LOG_L(laINIT, llDEBUG2) << "*******************************************";

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        strErrorInfo = QObject::tr("Cannot read file %1:\n%2.")
                               .arg(ProcessSettingsFileName)
                               .arg(file.errorString());
            FILE_LOG_L(laINIT, llERROR) << strErrorInfo.toStdString();
            //m_usErrorID = ERROR_DCL_CONFIG_PROCSETTING_OPEN_FAILED;

            return DCL_ERR_FCT_CALL_FAILED;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,
                                &errorColumn))
    {
        strErrorInfo = QObject::tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr);
           FILE_LOG_L(laINIT, llERROR) << strErrorInfo.toStdString();
           /*m_usErrorID = ERROR_DCL_CONFIG_PROCSETTING_PARSE_ERROR;*/

           return DCL_ERR_FCT_CALL_FAILED;
    }

    root = domDocument.documentElement();
    if (root.tagName() != "process_settings")
    {
        strErrorInfo = QObject::tr("The file is not a hardware config file. Tag 'hwconfig' missed");
        FILE_LOG_L(laINIT, llERROR) << strErrorInfo.toStdString();
        //m_usErrorID = ERROR_DCL_CONFIG_PROCSETTING_FORMAT_ERROR;

        return DCL_ERR_FCT_CALL_FAILED;
    }
    else if (root.hasAttribute("version")  /// \todo adapt version
               && root.attribute("version") != "1.0")
    {
        strErrorInfo = QObject::tr("The process settings file version is not valid.");
        FILE_LOG_L(laINIT, llERROR) << strErrorInfo.toStdString();
        //m_usErrorID = ERROR_DCL_CONFIG_PROCSETTING_VERSION_ERROR;

        return DCL_ERR_FCT_CALL_FAILED;
    }

    //-------------------------------------------
    //   slaves
    //-------------------------------------------
    QDomElement childGroups, childGroup;
    QDomNode childParameter;
    QDomNamedNodeMap attrmapParameters;
    QDomNode nodeParamKey, nodeParamValue;
    QString strKey, strGroupKey, strParamKey, strParamValue;

    childGroups = root.firstChildElement("groups");
    if(childGroups.isNull())
    {
        //m_usErrorID = ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV;
        return DCL_ERR_FCT_CALL_FAILED;
    }

    childGroup = childGroups.firstChildElement("group");
    while (!childGroup.isNull())
    {
        childParameter = childGroup.firstChildElement("parameter");
        strGroupKey = childGroup.attribute("key");

        while (!childParameter.isNull())
        {
            //read the attributes from the entry
            attrmapParameters = childParameter.attributes();
            nodeParamKey = attrmapParameters.namedItem("key");
            nodeParamValue = attrmapParameters.namedItem("value");

            strParamKey = nodeParamKey.nodeValue();
            strParamValue = nodeParamValue.nodeValue();

            // string together group and parameter key to compose the hash key
            strKey = strGroupKey + "_" + strParamKey;
            //add to hash container
            m_ProcessSettingsTable[strKey] = strParamValue;

            // read next parameter, if any
            childParameter = childParameter.nextSiblingElement("parameter");
        }
        //read next group, if any
        childGroup = childGroup.nextSiblingElement("group");
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Return the specified process parameter
 *
 *      This function returns the process parameter matches to the key as an
 *      quint8.
 *
 *  \iparam Key = process parameter key
 *
 *  \return The specified process parameter
 */
/****************************************************************************/
quint8 DeviceProcessing::GetProcSettingUInt8(QString Key)
{
    QString Value;
    quint8 ProcSettingValue;
    bool Ok;

    Value = m_ProcessSettingsTable[Key];
    ProcSettingValue = (quint8) Value.toInt(&Ok, 10);

    return ProcSettingValue;
}

/****************************************************************************/
/*!
 *  \brief  Return the specified process parameter
 *
 *      This function returns the process parameter matches to the key as an
 *      quint16.
 *
 *  \iparam Key = process parameter key
 *
 *  \return The specified process parameter
 */
/****************************************************************************/
quint16 DeviceProcessing::GetProcSettingUInt16(QString Key)
{
    QString Value;
    quint16 ProcSettingValue;
    bool Ok;

    Value = m_ProcessSettingsTable[Key];
    ProcSettingValue = (quint8) Value.toInt(&Ok, 10);

    return ProcSettingValue;
}

/****************************************************************************/
/*!
 *  \brief  Return the specified process parameter
 *
 *      This function returns the process parameter matches to the key as an
 *      quint32.
 *
 *  \iparam Key = process parameter key
 *
 *  \return The specified process parameter
 */
/****************************************************************************/
quint32 DeviceProcessing::GetProcSettingUInt32(QString Key)
{
    QString Value;
    quint32 ProcSettingValue;
    bool Ok;

    Value = m_ProcessSettingsTable[Key];
    ProcSettingValue = (quint32) Value.toInt(&Ok, 10);

    (void) Ok;

    return ProcSettingValue;
}

/****************************************************************************/
/*!
 *  \brief  Return the specified process parameter
 *
 *      This function returns the process parameter matches to the key as an
 *      QString.
 *
 *  \iparam Key = process parameter key
 *
 *  \return The specified process parameter
 */
/****************************************************************************/
QString DeviceProcessing::GetProcSettingString(QString Key)
{
    return m_ProcessSettingsTable[Key];
}

/****************************************************************************/
/*!
 *  \brief  Return the specified process parameter
 *
 *      This function returns the process parameter matches to the key as a
 *      Position_t.
 *
 *  \iparam Key = process parameter key
 *
 *  \return The specified process parameter
 */
/****************************************************************************/
Position_t DeviceProcessing::GetProcSettingPosition(QString Key)
{
    QString Value;
    Position_t ProcSettingPosition;
    bool Ok;

    Value = m_ProcessSettingsTable[Key];
    ProcSettingPosition = (Position_t) Value.toInt(&Ok, 10);

    (void) Ok;

    return ProcSettingPosition;
}

/****************************************************************************/
/*!
 *  \brief  Return the specified process parameter
 *
 *      This function returns the process parameter matches to the key as a
 *      MotionProfileIndex_t type.
 *
 *  \iparam Key = process parameter key
 *
 *  \return The specified process parameter
 */
/****************************************************************************/
MotionProfileIdx_t DeviceProcessing::GetProcSettingMotionProfileIdx(QString Key)
{
    QString Value;
    MotionProfileIdx_t ProcSettingMotionProfileIdx;
    bool Ok;

    Value = m_ProcessSettingsTable[Key];
    ProcSettingMotionProfileIdx = (MotionProfileIdx_t) Value.toInt(&Ok, 10);

    (void) Ok;

    return ProcSettingMotionProfileIdx;
}

/****************************************************************************/
/*!
 *  \brief  Handles the processing of a new receipt can message
 *
 *      The function handles the can message receive, it will be called whenever a can message
 *      registered by this class was received
 *
 *  \iparam pCANframe = Pointer to the can message received by communication layer
 */
/****************************************************************************/
void DeviceProcessing::HandleCanMessage(can_frame* pCANframe)
{
    FILE_LOG_L(laCOMM, llDEBUG1) << " CAN-ID 0x" << std::hex << pCANframe->can_id << " received.";
}

/****************************************************************************/
/*!
 *  \brief  Restarts the hardware configuration process
 */
/****************************************************************************/
void DeviceProcessing::Initialize()
{
    m_MainState = DP_MAIN_STATE_WAIT_FOR_CONFIG;
    m_SubStateConfig = DP_SUB_STATE_CONFIG_INIT;
    if (m_pConfigurationService != NULL) {
        m_pConfigurationService->Restart();
    }
}

/****************************************************************************/
/*!
 *  \brief  Returns the device matching an identifier
 *
 *      This function returns a device matches the instance identifier from
 *      the internal list
 *
 *  \iparam InstanceID = Instance identifier of the requested device
 *
 *  \return CBaseDevice* matches the parameter
 */
/****************************************************************************/
CBaseDevice* DeviceProcessing::GetDevice(quint32 InstanceID)
{
    QListIterator<CBaseDevice*> iter(m_DeviceList);
    CBaseDevice* pDevice = NULL;

    while (iter.hasNext())
    {
        pDevice = iter.next();

        if(pDevice->GetInstanceID() == InstanceID)
        {
            break;
        }
    }

    return pDevice;
}

/****************************************************************************/
/*!
 *  \brief  Returns a base module from the object tree
 *
 *      The functions returns pointer the base modules from the object tree
 *      in the order they are stored at the list.
 *
 *  \iparam First = The first base module should be returned
 *
 *  \return Next base module in object tree
 */
/****************************************************************************/
CBaseModule* DeviceProcessing::GetCANNodeFromObjectTree(bool First)
{
    CBaseModule* pCANNode = 0;

    if(First == true)
    {
        m_IterObjectTree = m_ObjectTree.begin();
    }
    else
    {
        m_IterObjectTree.operator++();
    }

    if(m_IterObjectTree != m_ObjectTree.end())
    {
        pCANNode = m_IterObjectTree.operator *();
    }

    return pCANNode;
}

/****************************************************************************/
/*!
 *  \brief  Insert a base module into the object tree
 *
 *  \iparam pCANNode = The base module to insert.
 */
/****************************************************************************/
void DeviceProcessing::InsertCANNodeToObjectTree(CBaseModule* pCANNode)
{
     m_ObjectTree.insert(m_ObjectTree.size(), pCANNode);
}

/****************************************************************************/
/*!
 *  \brief  Logs the object tree to the internal logging mechanism
 *
 *  \iparam Parameter = reserved
 */
/****************************************************************************/
void DeviceProcessing::LogObjectTree(int Parameter)
{
    QListIterator<CBaseModule*> iter(m_ObjectTree);

    CBaseModule* p_BaseModule;
    CFunctionModule* pFctModuleBase;
    QString strKey;

    Q_UNUSED(Parameter);

    FILE_LOG_L(laINIT, llDEBUG1) << "Log object tree";

    while (iter.hasNext())
    {
        p_BaseModule = iter.next();
        FILE_LOG_L(laINIT, llDEBUG1) << " " << p_BaseModule->GetKey().toStdString() << ": " <<
                                        (int) p_BaseModule->GetType() << " " << p_BaseModule->GetName().toStdString();

        pFctModuleBase = p_BaseModule->GetFunctionModuleFromList(0);
        while (pFctModuleBase)
        {
            FILE_LOG_L(laINIT, llDEBUG1) << "  -" << pFctModuleBase->GetKey().toStdString() << ": "
                                         << pFctModuleBase->GetName().toStdString() << " " << (int) pFctModuleBase->GetType();
            pFctModuleBase = p_BaseModule->GetFunctionModuleFromList(pFctModuleBase);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Logs the object tree to the internal logging mechanism
 *
 *  \iparam NodeID = The ID of the requested node
 *
 *  \return CANNode* to the requested node, if found, otherwise 0.
 */
/****************************************************************************/
CBaseModule* DeviceProcessing::GetNodeFromID(quint32 NodeID) const
{
    QListIterator<CBaseModule *> iter(m_ObjectTree);
    CBaseModule* p_BaseModule;

    while (iter.hasNext())
    {
        p_BaseModule = iter.next();
        if(((quint16)(p_BaseModule->GetNodeID()) == NodeID))
        {
            return p_BaseModule;
        }
    }

    return NULL;
}

/****************************************************************************/
/*!
 *  \brief  Gets the CAN node assigned to a specific node key
 *
 *      A pointer to the CBaseModule instance with the matching key
 *      will be returned. If no CBaseModule was found, the function
 *      returns  a null pointer.
 *
 *  \iparam NodeKey = The key of the requested node
 *
 *  \return The CANNode* matches the key
 */
/****************************************************************************/
CBaseModule* DeviceProcessing::GetNodeFromKey(QString NodeKey)
{
    CBaseModule* pCANNode = 0;

    for(int nIndex=0; nIndex < m_ObjectTree.size(); nIndex++)
    {
        if(m_ObjectTree.at(nIndex)->GetKey() == NodeKey)
        {
            pCANNode = m_ObjectTree.at(nIndex);
            break;
        }
    }

    return pCANNode;
}

/****************************************************************************/
/*!
 *  \brief  Returns the function module with the specified instance ID
 *
 *  \iparam InstanceID = InstanceID of the requested fucntion module
 *
 *  \return The CANFunctionModuleBase* matches the key
 */
/****************************************************************************/
CFunctionModule* DeviceProcessing::GetFunctionModule(quint32 InstanceID) const
{
    CBaseModule* pCANNode = 0;
    CFunctionModule* pfctModule = 0;
    quint16 canNodeID;
    quint8 channel;

    //canNodeID = ((quint16)(0x0fff & ((quint16)InstanceID)));
    //channel = ((quint8)((0x000ff000 & InstanceID) >> 12));
    canNodeID = ((quint16)(0xff & (InstanceID)));
    channel = ((quint8)((0x000ff0000 & InstanceID) >> 16));

    pCANNode = GetNodeFromID(canNodeID);
    if(pCANNode)
    {
        pfctModule = pCANNode->GetFunctionModuleFromChannel(channel);
    }

    return pfctModule;
}

/****************************************************************************/
/*!
 *  \brief  Returns the base module with the specified instance ID
 *
 *  \iparam InstanceID = InstanceID of the fucntion module which belong to the
 *                       base module
 *
 *  \return The CBaseModule* matches the key
 */
/****************************************************************************/
CBaseModule* DeviceProcessing::GetBaseModule(quint32 InstanceID) const
{
    CBaseModule* pCANNode = 0;
    quint16 canNodeID;

    //canNodeID = ((quint16)(0x0fff & ((quint16)InstanceID)));
    canNodeID = ((quint16)(0xff & (InstanceID)));
    pCANNode = GetNodeFromID(canNodeID);

    return pCANNode;
}

/****************************************************************************/
/*!
 *  \brief  Returns if the function module has been detetcted and configured via CAN
 *
 *  \iparam InstanceID = InstanceID of the requested fucntion module
 *
 *  \return True if the function module exist, else not.
 */
/****************************************************************************/
bool DeviceProcessing::CheckFunctionModuleExistence(quint32 InstanceID)
{
    CBaseModule* pCANNode;
    CFunctionModule* pFctModuleBase;
    pCANNode = this->GetCANNodeFromObjectTree(true);
    quint16 canNodeID = ((quint16)(0xff & (InstanceID)));
    quint8 channel = ((quint8)((0x000ff0000 & InstanceID) >> 16));

    while(pCANNode)
    {
        CBaseModule::CANNodeMainState_t nodeState = pCANNode->GetMainState();
        if (nodeState != CBaseModule::CN_MAIN_STATE_IDLE)
        {
            pCANNode = this->GetCANNodeFromObjectTree(false);
            continue;
        }
        if(pCANNode->GetNodeID() == canNodeID)
        {
            pFctModuleBase = pCANNode->GetFunctionModuleFromList(0);
            while (pFctModuleBase)
            {
                if(pFctModuleBase->GetChannelNo() == channel)
                {
                    return true;
                }
                pFctModuleBase = pCANNode->GetFunctionModuleFromList(pFctModuleBase);
            }
        }
        pCANNode = this->GetCANNodeFromObjectTree(false);
    }
    return false;
}
/****************************************************************************/
/*!
 *  \brief  Set the specified error parameters and error time to current time
 *
 *  \iparam ErrorGroup = Error group ID of the thrown error
 *  \iparam ErrorCode = Error ID of the thrown error
 *  \iparam ErrorData = Additional error information
 */
/****************************************************************************/
void DeviceProcessing::SetErrorParameter(quint16 ErrorGroup, quint16 ErrorCode, quint16 ErrorData)
{
    m_LastErrorTime  = Global::AdjustedTime::Instance().GetCurrentDateTime();
    m_LastErrorGroup = ErrorGroup;
    m_LastErrorCode  = ErrorCode;
    m_LastErrorData  = ErrorData;
}

void DeviceProcessing::OnReportGetServiceInfo(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo, const QString& deviceType)
{
    emit ReportGetServiceInfo(ReturnCode, ModuleInfo, deviceType);
}

void DeviceProcessing::WriteDeviceLifeCycle()
{
    if (m_pConfigurationService)
    {
        m_pConfigurationService->WriteDeviceLifeCycle();
    }
}

#ifdef HAL_CV_TEST

DCLEventLoop* DeviceProcessing::CreateSyncCall(SyncCmdType_t CmdType)
{
    DCLEventLoop* event = new DCLEventLoop();
    event->SetCmdType(CmdType);
    m_EventLoopsForSyncCall.push_back(event);
    return event;
}

ReturnCode_t DeviceProcessing::BlockingForSyncCall(DCLEventLoop* event)
{
    ReturnCode_t ret=DCL_ERR_FCT_CALL_SUCCESS;

    if (!event->GetResumed())
    {
        ret = (ReturnCode_t)(event->exec());
    }
    for (QVector<DCLEventLoop*>::iterator iter = m_EventLoopsForSyncCall.begin(); iter!= m_EventLoopsForSyncCall.end(); ++iter)
    {
        if (*iter == event)
        {
            delete event;
            event = NULL;
            m_EventLoopsForSyncCall.erase(iter++);
            break;
        }
    }
    return ret;
}



/****************************************************************************/
/*!
 *  \brief  Block caller's current thread with specified type
 *
 *  \iparam CmdType = Command type to block the thread
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::BlockingForSyncCall(SyncCmdType_t CmdType)
{
    DCLEventLoop* event = new DCLEventLoop();
    event->SetCmdType(CmdType);
    m_EventLoopsForSyncCall.push_back(event);
    ReturnCode_t ret = (ReturnCode_t)(event->exec());
    for (QVector<DCLEventLoop*>::iterator iter = m_EventLoopsForSyncCall.begin(); iter!= m_EventLoopsForSyncCall.end(); ++iter)
    {
        if (*iter == event)
        {
            delete event;
            event = NULL;
            m_EventLoopsForSyncCall.erase(iter++);
            break;
        }
    }
    return ret;
}

/****************************************************************************/
/*!
 *  \brief  Resume blocked thread with specified type
 *
 *  \iparam CmdType = Command type to unblock the thread
 */
/****************************************************************************/
void DeviceProcessing::ResumeFromSyncCall(SyncCmdType_t CmdType, ReturnCode_t Value)
{
    for (int i =0; i<m_EventLoopsForSyncCall.size(); ++i)
    {
        if (m_EventLoopsForSyncCall[i] == NULL)
        {
            continue;
        }
        if (CmdType == m_EventLoopsForSyncCall[i]->GetCmdType())
        {
            m_EventLoopsForSyncCall[i]->SetResumed();
            m_EventLoopsForSyncCall[i]->exit(Value);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Block caller's current thread with specified type and timeout
 *
 *  \iparam CmdType = Command type to block the thread
 *  \iparam Timeout = Timeout for the command
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::BlockingForSyncCall(SyncCmdType_t CmdType, ulong Timeout)
{
    DCLEventLoop* event = new DCLEventLoop();
    event->SetCmdType(CmdType);
    QTimer::singleShot(Timeout, event, SLOT(TimeOut()));
    m_EventLoopsForSyncCall.push_back(event);
    ReturnCode_t ret = (ReturnCode_t)(event->exec());

    for (QVector<DCLEventLoop*>::iterator iter = m_EventLoopsForSyncCall.begin(); iter!= m_EventLoopsForSyncCall.end(); ++iter)
    {
        if (*iter == event)
        {
            delete event;
            event = NULL;
            m_EventLoopsForSyncCall.erase(iter++);
            break;
        }
    }
    return ret;
}

void DeviceProcessing::OnStopCommandExec(quint8 CmdType)
{
    if (0 == CmdType)
    {
        CmdType = SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR;
    }
    else if (1 == CmdType)
    {
        CmdType = SYNC_CMD_AL_PROCEDURE_DRAINING;
    }

    for (int i =0; i<m_EventLoopsForSyncCall.size(); ++i)
    {
        if (CmdType == m_EventLoopsForSyncCall[i]->GetCmdType())
        {
            m_EventLoopsForSyncCall[i]->exit(DCL_ERR_UNEXPECTED_BREAK);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Timer callback for BlockingForSyncCall
 */
/****************************************************************************/
void DeviceProcessing::BlockingTimerCallback()
{
#if 0
    for(quint32 i = 0; i< SYNC_CMD_TOTAL_NUM; i++)
    {
        if(m_EventLoopsForSyncCall[i].timerActive)
        {
            //qint64 Now = QDateTime::currentMSecsSinceEpoch();
            //if(Now >= m_EventLoopsForSyncCall[i].endTime)
            {
                m_EventLoopsForSyncCall[i].timerActive = false;
                m_EventLoopsForSyncCall[i].eventloop.exit(DCL_ERR_TIMER_TIMEOUT);
                //LOG()<<Now<<"timer end"<<i<<"llll:"<<(Now - m_EventLoopsForSyncCall[i].endTime);
            }
        }
    }
#endif
}
#else

/****************************************************************************/
/*!
 *  \brief  Block caller's current thread with specified type and timeout
 *
 *  \iparam CmdType = Command type to block the thread
 *  \iparam Timeout = Timeout for the command
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 */
/****************************************************************************/
ReturnCode_t DeviceProcessing::BlockingForSyncCall(SyncCmdType_t CmdType, ulong Timeout)
{
    ReturnCode_t retValue;
    // LOG() << "Device Processing: WaitCondition: Wait Before: CMD"<< CmdType<<" ThreadID: "<< QThread::currentThreadId()<<" Time: "<<QDateTime::currentDateTime().toMSecsSinceEpoch();
    if(CmdType < SYNC_CMD_TOTAL_NUM) //lint !e641
    {
        m_Mutex[CmdType].lock();
        if(m_WaitConditionForSyncCall[CmdType].wait(&m_Mutex[CmdType], Timeout))
        {
            retValue = m_SyncCallResult[CmdType];
        }
        else
        {
            retValue = DCL_ERR_TIMER_TIMEOUT;
        }
        m_Mutex[CmdType].unlock();
    }
    else
    {
        retValue = DCL_ERR_INVALID_PARAM;
    }
    // LOG() << "Device Processing: WaitCondition: Wait After: CMD"<< CmdType<<" ThreadID: "<< QThread::currentThreadId()<<" Time: "<<QDateTime::currentDateTime().toMSecsSinceEpoch();
    return retValue;
}

/****************************************************************************/
/*!
 *  \brief  Resume blocked thread with specified type
 *
 *  \iparam CmdType = Command type to unblock the thread
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS or error return code
 */
/****************************************************************************/

ReturnCode_t DeviceProcessing::BlockingForSyncCall(SyncCmdType_t CmdType)
{
    ReturnCode_t retValue;
   // LOG() << "Device Processing: WaitCondition: Wait Before: CMD"<< CmdType<<" ThreadID: "<< QThread::currentThreadId()<<" Time: "<<QDateTime::currentDateTime().toMSecsSinceEpoch();
    if(CmdType < SYNC_CMD_TOTAL_NUM) //lint !e641
    {
        m_Mutex[CmdType].lock();
        (void)m_WaitConditionForSyncCall[CmdType].wait(&m_Mutex[CmdType]);
        retValue = m_SyncCallResult[CmdType];
        m_Mutex[CmdType].unlock();
    }
    else
    {
         retValue = DCL_ERR_INVALID_PARAM;
    }
   // LOG() << "Device Processing: WaitCondition: Wait After: CMD"<< CmdType<<" ThreadID: "<< QThread::currentThreadId()<<" Time: "<<QDateTime::currentDateTime().toMSecsSinceEpoch();
    return retValue;
}

/****************************************************************************/
/*!
 *  \brief  Resume blocked thread with specified type
 *
 *  \iparam CmdType = Command type to unblock the thread
 *  \iparam RetCode = Return value for the blocking caller
 */
/****************************************************************************/
void DeviceProcessing::ResumeFromSyncCall(SyncCmdType_t CmdType,  ReturnCode_t RetCode)
{
    if(CmdType < SYNC_CMD_TOTAL_NUM)  //lint !e641
    {
        m_Mutex[CmdType].lock();
        m_WaitConditionForSyncCall[CmdType].wakeAll();
        m_SyncCallResult[CmdType] = RetCode;
        m_Mutex[CmdType].unlock();
    }
}
#endif
} //namespace

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

#include "DeviceControl/Include/Devices/DeviceBase.h"

#include "DeviceControl/Include/SlaveModules/BaseModule.h"

#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"
#include "Global/Include/EventObject.h"
#include "Global/Include/Exception.h"
#include "Global/Include/SystemPaths.h"
#include "Global/Include/Utils.h"

#include "DataManager/Containers/Adjustment/Include/Adjustment.h"
#include "DataManager/Containers/Adjustment/Include/AdjustmentVerifier.h"

#include <QMetaType>
#include <QDebug>
#include <cerrno>

namespace DeviceControl
{

const QString CANObjectKeyLUT::m_DevAgitationKey = "Agitation";
const QString CANObjectKeyLUT::m_DevExhaustKey = "Exhaust";
const QString CANObjectKeyLUT::m_DevXyzTransportationKey = "XyzTransportation";
const QString CANObjectKeyLUT::m_DevHeatedCuvettesKey = "HeatedCuvettes";
const QString CANObjectKeyLUT::m_DevDrawerKey = "Drawer";
const QString CANObjectKeyLUT::m_DevOvenKey = "Oven";
const QString CANObjectKeyLUT::m_DevRackTransferKey = "RackTransfer";
const QString CANObjectKeyLUT::m_DevWaterKey = "Water";
const QString CANObjectKeyLUT::m_DevRackHandlingKey = "RackHandling";
const QString CANObjectKeyLUT::m_DevCoverLineKey = "CoverLine";
const QString CANObjectKeyLUT::m_DevSlideIdKey = "SlideId";
const QString CANObjectKeyLUT::m_DevHoodKey = "Hood";
const QString CANObjectKeyLUT::m_DevLightKey= "Light";

const QString CANObjectKeyLUT::m_BaseAgitationKey = "base_agitation";
const QString CANObjectKeyLUT::m_BaseExhaustKey = "base_exhaust";
const QString CANObjectKeyLUT::m_BaseXAxisKey = "base_x_axis";
const QString CANObjectKeyLUT::m_BaseYAxisKey = "base_y_axis";
const QString CANObjectKeyLUT::m_BaseZAxisKey = "base_z_axis";
const QString CANObjectKeyLUT::m_BaseHeatedCuvettesKey = "base_heated_cuvettes";
const QString CANObjectKeyLUT::m_BaseDrawerKey = "base_drawer";
const QString CANObjectKeyLUT::m_BaseOvenKey = "base_oven";
const QString CANObjectKeyLUT::m_BaseRackTransferKey = "base_rack_transfer";
const QString CANObjectKeyLUT::m_BaseWaterKey = "base_water";
const QString CANObjectKeyLUT::m_BaseSlideIdKey = "base_slide_id";
const QString CANObjectKeyLUT::m_BaseHoodKey = "base_hood";
const QString CANObjectKeyLUT::m_BaseLightKey = "base_light";

const QString CANObjectKeyLUT::m_MotorAgitationKey = "motor_agitation";
const QString CANObjectKeyLUT::m_FlowSensorKey = "flow_sensor";
const QString CANObjectKeyLUT::m_ControlFan1Key = "control_fan1";
const QString CANObjectKeyLUT::m_ControlFan2Key = "control_fan2";
const QString CANObjectKeyLUT::m_CurrentFan1Key = "current_fan1";
const QString CANObjectKeyLUT::m_CurrentFan2Key = "current_fan2";
const QString CANObjectKeyLUT::m_MotorXAxisKey = "motor_x_axis";
const QString CANObjectKeyLUT::m_MotorYAxisKey = "motor_y_axis";
const QString CANObjectKeyLUT::m_MotorZAxisKey = "motor_z_axis";
const QString CANObjectKeyLUT::m_GrapplerRFIDKey = "grap_rfid_reader";
const QString CANObjectKeyLUT::m_GrapplerLiquidLevelKey = "grap_liquid_level";
const QString CANObjectKeyLUT::m_TempCtrlVessel1Key = "temp_ctrl1";
const QString CANObjectKeyLUT::m_TempCtrlVessel2Key = "temp_ctrl2";
const QString CANObjectKeyLUT::m_TempCtrlVessel3Key = "temp_ctrl3";
const QString CANObjectKeyLUT::m_TempCtrlVessel4Key = "temp_ctrl4";
const QString CANObjectKeyLUT::m_MotorDrawerKey = "motor_drawer";
const QString CANObjectKeyLUT::m_DrawerRFIDKey = "rfid_drawer";
const QString CANObjectKeyLUT::m_DrawerButtonKey = "diginp_button_drawer";
const QString CANObjectKeyLUT::m_DrawerLEDFreeKey = "digout_led_free_drawer";
const QString CANObjectKeyLUT::m_DrawerLEDBlockedKey = "digout_led_blocked_drawer";
const QString CANObjectKeyLUT::m_OvenCoverMotorKey = "motor_oven_cover";
const QString CANObjectKeyLUT::m_OvenTempCtrlKey = "temp_ctrl_oven";
const QString CANObjectKeyLUT::m_RackTransferMotorKey = "motor_transfer";
const QString CANObjectKeyLUT::m_WaterValve1Key = "water_valve1";
const QString CANObjectKeyLUT::m_WaterValve2Key = "water_valve2";
const QString CANObjectKeyLUT::m_WaterValve3Key = "water_valve3";
const QString CANObjectKeyLUT::m_WaterValve4Key = "water_valve4";
const QString CANObjectKeyLUT::m_WaterValve5Key = "water_valve5";
const QString CANObjectKeyLUT::m_WaterValve6Key = "water_valve6";
const QString CANObjectKeyLUT::m_WaterLiquidLevelKey = "collecting_tray_limit";
const QString CANObjectKeyLUT::m_SlideIdPhotoDetectorKey = "photo_detector";
const QString CANObjectKeyLUT::m_SlideIdTransmitControlKey = "transmit_control";
const QString CANObjectKeyLUT::m_SlideIdTransmitCurrentKey = "transmit_current";
const QString CANObjectKeyLUT::m_SlideIdReceiveCurrentKey = "receive_current";
const QString CANObjectKeyLUT::m_HoodSensorKey = "reed_active";
const QString CANObjectKeyLUT::m_LightControlKey = "light_control";

const QString CANObjectKeyLUT::m_MotorCoverLineZAxisKey = "motor_cl_elevator";
const QString CANObjectKeyLUT::m_MotorCoverLineSlideKey = "motor_cl_slide_shift";
const QString CANObjectKeyLUT::m_MotorCoverLineNeedleKey = "motor_cl_needle";
const QString CANObjectKeyLUT::m_MotorCoverLineClampKey = "motor_cl_slide_clamp";
const QString CANObjectKeyLUT::m_MotorCoverSlipYKey = "motor_cl_coverslip_y_axis";
const QString CANObjectKeyLUT::m_MotorCoverSlipZKey = "motor_cl_coverslip_z_axis";
const QString CANObjectKeyLUT::m_MotorEDMPump = "motor_cl_edm_pump";
const QString CANObjectKeyLUT::m_DigInpCoverLineSlideDetect = "diginp_cl_slide_detect";
const QString CANObjectKeyLUT::m_DigInpEDMLevel = "diginp_cl_edm_level";
const QString CANObjectKeyLUT::m_DigOutpVacuumPump = "digout_cl_vakuum_pump";
const QString CANObjectKeyLUT::m_DigOutpVacuumValve1 = "digout_cl_vacuum_valve_1";
const QString CANObjectKeyLUT::m_DigOutpVacuumValve2 = "digout_cl_vacuum_valve_2";
const QString CANObjectKeyLUT::m_AnaInpPressureSensor = "anainp_cl_vacuum";
const QString CANObjectKeyLUT::m_AnaInpHallSensor1 = "anainp_cl_hall";
const QString CANObjectKeyLUT::m_MotorRackHdlXAxisKey = "motor_rackhdl_x_axis";
const QString CANObjectKeyLUT::m_MotorRackHdlYAxisKey = "motor_rackhdl_y_axis";
const QString CANObjectKeyLUT::m_MotorRackHdlZAxisKey = "motor_rackhdl_z_axis";
const QString CANObjectKeyLUT::m_MotorRackHdlGrabberKey = "motor_rackhdl_grab";

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
DeviceProcessing::DeviceProcessing(QObject *p_Parent) : QObject(p_Parent),
    m_pTaskConfig(0), m_pTaskNormalOperation(0), m_pTaskShutdown(0), m_pTaskDestroy(0),
    m_pTaskDiagnostic(0), m_pTaskAdjustment(0), m_pTaskFirmwareUpdate(0)
{
    qDebug() << "Device Processing" << thread();

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

    m_ulCanIDMasterHeartbeat = 0;
    m_ulCANNodeIDMaster = 0;

    m_MainState = DP_MAIN_STATE_INTERNAL_CONFIG;

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
        FILE_LOG_L(laDEVPROC, llERROR) << " DeviceProcessing: error: " << (qint32)retCode << "!";
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  Destructor of DeviceProcessing
 */
/****************************************************************************/
DeviceProcessing::~DeviceProcessing()
{
    try
    {
        CDeviceBase *p_Device;

        while (!m_lstTasks.isEmpty()) {
            delete m_lstTasks.takeFirst();
        }
        foreach (p_Device, m_DeviceList) {
            delete p_Device;
        }
    }
    catch (...) //(const std::bad_alloc &)
    {
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  Forwards device control layer events
 *
 *      The function forwards the error information to the IDeviceProcessing
 *      interface class. (which finally throws the signal assigned to the
 *      errors)
 *
 *  \iparam InstanceID = The instance identifier of the module which brought up the error
 *  \iparam ErrorGroup = Error group ID of the thrown error
 *  \iparam ErrorID    = Error ID of the thrown error
 *  \iparam ErrorData  = Additional error information
 *  \iparam ErrorTime  = Time of error detection
 */
/****************************************************************************/
void DeviceProcessing::ThrowEvent(quint32 EventCode, quint16 EventData)
{
    QDateTime EventTime = Global::AdjustedTime::Instance().GetCurrentDateTime();

    FILE_LOG_L(laDEVPROC, llERROR) << "  DeviceProcessing::ThrowEvent (" << EventCode << ", " << EventData << ", "
                                   << EventTime.toString().constData() << ")";
    Global::EventObject::Instance().RaiseEvent(EventCode, Global::FmtArgs() << EventData);
    emit ReportEvent(EventCode, EventData, EventTime);
}

/****************************************************************************/
/*!
 *  \brief  Forwards device control layer events
 *
 *      The function forwards the error information to the IDeviceProcessing
 *      interface class. (which finally throws the signal assigned to the
 *      errors)
 *
 *  \iparam InstanceID = The instance identifier of the module which brought up the error
 *  \iparam ErrorGroup = Error group ID of the thrown error
 *  \iparam ErrorID    = Error ID of the thrown error
 *  \iparam ErrorData  = Additional error information
 *  \iparam ErrorTime  = Time of error detection
 *  \iparam ErrorInfo  = Error information string
 */
/****************************************************************************/
void DeviceProcessing::ThrowEventWithInfo(quint32 EventCode, quint16 EventData, QString EventInfo)
{
    QDateTime EventTime = Global::AdjustedTime::Instance().GetCurrentDateTime();

    FILE_LOG_L(laDEVPROC, llERROR) << "  DeviceProcessing::ThrowEventWithInfo (" << EventCode << ", " << EventData
                                   << ", " << EventTime.toString().constData() << ", " << EventInfo.constData() << ")";
    Global::EventObject::Instance().RaiseEvent(EventCode, Global::FmtArgs() << EventData << EventInfo);
    emit ReportEventWithInfo(EventCode, EventData, EventTime, EventInfo);
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
            if(retCode == DCL_ERR_FCT_CALL_SUCCESS)
            {
                retCode = ReadProcessSettings();
            }
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
        }
    }

    return retCode;
}

/****************************************************************************/
/*!
 *  \brief  Initialize the can messages used by this class
 *
 *      The function reads the 'parameter_master' section from the hardware
 *      configuration file. In an earlier version the serial number had to be
 *      provided directly after instantiating the DeviceProcessing class.
 *      That is why these values are read here and not at the
 *      HardwareConfiguration class.
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
        QString String = QObject::tr("Cannot read file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString());
        FILE_LOG_L(laINIT, llERROR) << String.toStdString();

        return DCL_ERR_FCT_CALL_FAILED;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,
                                &errorColumn))
    {
        QString String = QObject::tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr);
        FILE_LOG_L(laINIT, llERROR) << String.toStdString();

        return DCL_ERR_FCT_CALL_FAILED;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "hwconfig")
    {
        QString String = QObject::tr("The file is not a hardware config file.. Tag 'hwconfig' missed");
        FILE_LOG_L(laINIT, llERROR) << String.toStdString();

        return DCL_ERR_FCT_CALL_FAILED;
    }
    else if (root.hasAttribute("version")
               && root.attribute("version") != "1.0")
    {
        QString String = QObject::tr("The hardware config files' version is not valid.");
        FILE_LOG_L(laINIT, llERROR) << String.toStdString();

        return DCL_ERR_FCT_CALL_FAILED;
    }

    QDomElement child = root.firstChildElement("parameter_master");
    if(child.isNull())
    {
        return DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        QDomElement childCAN;
        QDomElement childTcp;

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
    }

    return retCode;
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
 *  \brief  This slot receives TCP/IP messages from the simulator
 *
 *      This function is needed, when the Device Control Layer communicates
 *      with the Colopia simulator over TCP/IP sockets. The function recives
 *      the message in the format of a CAN datagram and appends it to the
 *      receive queue.
 *
 *  \iparam ID = CAN identifier
 *  \iparam data0 = Byte 0
 *  \iparam data1 = Byte 1
 *  \iparam data2 = Byte 2
 *  \iparam data3 = Byte 3
 *  \iparam data4 = Byte 4
 *  \iparam data5 = Byte 5
 *  \iparam data6 = Byte 6
 *  \iparam data7 = Byte 7
 *  \iparam dlc = data length count
 */
/****************************************************************************/
void DeviceProcessing::ReceiveCANMessage(quint32 ID, quint8 data0, quint8 data1, quint8 data2, quint8 data3,
                                         quint8 data4, quint8 data5, quint8 data6, quint8 data7, quint8 dlc)
{
    //FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing::ReceiveCANMessage: " << std::hex << ID << ", " << data0 << ", " << data1 << ", " << data2;
    struct can_frame frame;

    frame.can_id = ID;
    frame.data[0] = data0;
    frame.data[1] = data1;
    frame.data[2] = data2;
    frame.data[3] = data3;
    frame.data[4] = data4;
    frame.data[5] = data5;
    frame.data[6] = data6;
    frame.data[7] = data7;
    frame.can_dlc = dlc;

    m_canCommunicator.AppendToReceiveQueue( frame);
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
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    qint16 sResult;
    Can2TcpClient* pClient;

    if(m_TcpInterface == "1")
    {
        FILE_LOG_L(laDEVPROC, llINFO) << "  activate tcp communication";
        m_canCommunicator.SetTcpCommunication(true);
    }

    FILE_LOG_L(laDEVPROC, llINFO) << "  start CAN communication";
    sResult = m_canCommunicator.StartComm(m_CanInterface.toStdString().c_str());
    if(sResult < 0)
    {
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }
    else
    {
        if(m_TcpInterface == "1")
        {
            pClient = m_canCommunicator.GetTcpClient();

            if(!connect((CANReceiveThread*) pClient, SIGNAL(newCANMessage(quint32, quint8, quint8, quint8, quint8, quint8, quint8, quint8, quint8, quint8)),
                    this, SLOT(ReceiveCANMessage(quint32, quint8, quint8, quint8, quint8, quint8, quint8, quint8, quint8, quint8))))
            {
                FILE_LOG_L(laDEVPROC, llERROR) << "  cannot connect 'newCANMessage' signal ";
                RetVal = DCL_ERR_FCT_CALL_FAILED;
            }
        }
    }

    return RetVal;
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
    struct timeval tv_end;
    struct timeval tv_start;

    static quint16 stTest = 0;
    static quint32 RunThruCounter = 0;
    static bool PerformanceCheck = false;
    static DeviceProcTask* pActiveTask = NULL;
    static DeviceProcTask::TaskID_t stTaskID = DeviceProcTask::TASK_ID_DP_UNDEF;

    //Performance check initialisation
    if (PerformanceCheck)
    {
        if(gettimeofday(&tv_start, 0) != 0)
        {
            PerformanceCheck = false;
            FILE_LOG_L(laDEVPROC, llERROR) << " Perfomance check failed!";
        }
    }

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

    qint16 error;
    qint16 errorAddInfo;

    error = m_canCommunicator.GetCommunicationError(errorAddInfo);
    if(error < 0)
    {
        FILE_LOG_L(laDEVPROC, llERROR) << "  Error: DeviceProcessing: DispatchPendingInMessage: " << ", " << error;
        ThrowEvent(EVENT_DEVICECONTROL_ERROR_CANBUS_WRITE, error);
    }

    m_canCommunicator.DispatchPendingInMessage();

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
                m_pConfigurationService = new CConfigurationService(*this, m_canCommunicator);
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
                bool NonIdleFound = false;

                HandleCANNodesTask();

//                foreach (CDeviceBase* p_Device, m_DeviceList)
//                {
//                    if(p_Device->GetMainState() == CDeviceBase::DEVICE_MAIN_STATE_ERROR)
//                    {
//                        FILE_LOG_L(laINIT, llERROR) << "   DeviceProcessing: DEVICE_MAIN_STATE_ERROR";
//                        m_SubStateConfig = DP_SUB_STATE_CONFIG_ERROR;
//                        break;
//                    }
//                    else if(p_Device->GetMainState() != CDeviceBase::DEVICE_MAIN_STATE_IDLE)
//                    {
//                        FILE_LOG_L(laINIT, llWARNING) << "   DeviceProcessing: not idle";
//                        NonIdleFound = true;
//                        break;
//                    }
//                }
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
                if(m_pConfigurationService->ConfigurationComplete() == true) {
                    emit ReportConfigurationFinished(DCL_ERR_FCT_CALL_SUCCESS);
                }
                else {
                    emit ReportConfigurationFinished(DCL_ERR_TIMEOUT);
                }
            }
            m_SubStateConfig = DP_SUB_STATE_CONFIG_FINISHED;
            TaskFinished(pActiveTask);
            //m_MainState = DP_MAIN_STATE_WAIT_FOR_ADJUST;
            m_MainState = DP_MAIN_STATE_IDLE;
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
        pActiveTask->m_state = DeviceProcTask::TASK_STATE_PAUSE;
        FILE_LOG_L(laDEVPROC, llINFO) << "  pause task 'normal operation'";
        ThrowEvent(EVENT_DEVICECONTROL_INFO_BREAK_NORMAL_OP, 0);

        return;
    }

    if(m_SubStateNormalOp == DP_SUB_STATE_NORMAL_OP_RUN)
    {
        HandleCANNodesTask();
    }
    else if(m_SubStateNormalOp == DP_SUB_STATE_NORMAL_OP_START)
    {
        m_SubStateNormalOp = DP_SUB_STATE_NORMAL_OP_CONFIG;
        FILE_LOG_L(laDEVPROC, llINFO) << "  HandleTaskNormalOperation : State OP_CONFIG";
    }
    else if(m_SubStateNormalOp == DP_SUB_STATE_NORMAL_OP_CONFIG)
    {
        HandleCANNodesTask();
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

        emit ReportDiagnosticServiceClosed(DCL_ERR_FCT_CALL_SUCCESS);
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
        m_SubStateDiag = DP_SUB_STATE_DIAG_IDLE;
        ThrowEvent(EVENT_DEVICECONTROL_ERROR_START_DIAG, 0);
    }
    else if(m_SubStateDiag == DP_SUB_STATE_DIAG_IDLE)
    {
        if(m_pDiagnosticService)
        {
            m_pDiagnosticService->HandleTasks();
        }
    }

    HandleCANNodesTask();
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
        ReturnCode_t RetVal;

        FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing::HandleTaskShutDown DP_SUB_STATE_SHUTDOWN_CLOSE_COMM.";
        RetVal = m_canCommunicator.StopComm();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_SubStateShutDown = DP_SUB_STATE_SHUTDOWN_CLOSE_OBJECTS;
            FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing::HandleTaskShutDown DP_SUB_STATE_SHUTDOWN_CLOSE_COMM finished";
        }
        else
        {
            m_SubStateShutDown = DP_SUB_STATE_SHUTDOWN_CLOSE_OBJECTS;
            FILE_LOG_L(laDEVPROC, llINFO) << "  DeviceProcessing::HandleTaskShutDown DP_SUB_STATE_SHUTDOWN_CLOSE_COMM failed:" << (int) RetVal;
        }
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
 *  \brief Registers data Templates
 *
 *      Registers the Templates declared in this layer and used by signal
 *      slot mechanism.
 */
/****************************************************************************/
void DeviceProcessing::RegisterMetaTypes()
{
    qRegisterMetaType<ReturnCode_t>("ReturnCode_t");
    qRegisterMetaType<NodeState_t>("NodeState_t");
    qRegisterMetaType<EmergencyStopReason_t>("EmergencyStopReason_t");
    qRegisterMetaType<PowerState_t>("PowerState_t");
    qRegisterMetaType<TestResult_t>("TestResult_t");
    qRegisterMetaType<BlockState_t>("BlockState_t");
    qRegisterMetaType<HeatedVesselID_t>("HeatedVesselID_t");
    qRegisterMetaType<LoaderPosition_t>("LoaderPosition_t");
    qRegisterMetaType<LoaderRFIDChannel_t>("LoaderRFIDChannel_t");
    qRegisterMetaType<AgitationSpeed_t>("AgitationSpeed_t");
    qRegisterMetaType<WaterValveID_t>("WaterValveID_t");
    qRegisterMetaType<OvenCoverPosition_t>("OvenCoverPosition_t");
    qRegisterMetaType<TempCtrlStatus_t>("TempCtrlStatus_t");
    qRegisterMetaType<TempCtrlOperatingMode_t>("TempCtrlOperatingMode_t");
    qRegisterMetaType<TempCtrlMainsVoltage_t>("TempCtrlMainsVoltage_t");
    qRegisterMetaType<RackAdapterPosition_t>("RackAdapterPosition_t");
    qRegisterMetaType<RackAdapterPosition_t>("AgitationPosition_t");
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
                ThrowEvent(EVENT_DEVICECONTROL_ERROR_HEARTBEAT, 0);
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
 *  \return Device assigned to the instance identifier
 */
/****************************************************************************/
CDeviceBase* DeviceProcessing::GetDevice(DevInstanceID_t InstanceID)
{
    return m_DeviceList[InstanceID];
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
CBaseModule* DeviceProcessing::GetNodeFromID(quint16 NodeID) const
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
 *  \brief  Returns the function module with the specified instance ID
 *
 *  \iparam InstanceID = InstanceID of the requested funtion module
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

    canNodeID = ((quint16)(0x0fff & ((quint16)InstanceID)));
    channel = ((quint8)((0x000ff000 & InstanceID) >> 12));

    pCANNode = GetNodeFromID(canNodeID);
    if(pCANNode)
    {
        pfctModule = pCANNode->GetFunctionModuleFromChannel(channel);
    }

    return pfctModule;
}

/****************************************************************************/
/*!
 *  \brief  Insert a device into the internal list
 *
 *  \iparam InstanceId = Instance identifier of the device
 *  \iparam pBaseDevice = Pointer to device
 */
/****************************************************************************/
void DeviceProcessing::AddDevice(DevInstanceID_t InstanceId, CDeviceBase* pBaseDevice)
{
    m_DeviceList[InstanceId] = pBaseDevice;
}

} //namespace

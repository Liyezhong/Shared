/****************************************************************************/
/*! \file TemperatureControl.cpp
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 05.01.2013
 *   $Author:  $ Albert Zhang
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CPressureControl
 *
*  \b Company:
 *
 *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2012 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifdef PRE_ALFA_TEST
#include "DeviceControl/Include/SlaveModules/PressureControl.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"
#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
#include "DeviceControl/Include/Global/dcl_log.h"
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

#define CANPRESSURE_READ_REQ_TIMEOUT    500 //!< timeout req. actual pressure
#define CANPRESSURE_WRITE_REQ_TIMEOUT   500 //!< timeout set pressure

#define PRESSURE_CTRL_ACTIVE_BITPOS  (0)   //!< bit position state
#define PRESSURE_CTRL_TUNINGBITPOS   (1)    //!< bit position tuning
#define PRESSURE_CTRL_PHASE_BITPOS   (2)    //!< bit position phase
#define PRESSURE_CTRL_VOLTAGE_BITPOS (3)  //!< bit position phase

/****************************************************************************/
/*!
 *  \brief  Constructor for the CPressureControl
 *
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to communication class
 *  \iparam pParentNode = pointer to CANNode, where this module is assigned to
 */
/****************************************************************************/
CPressureControl::CPressureControl(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator, CBaseModule* pParentNode) :
    CFunctionModule(CModuleConfig::CAN_OBJ_TYPE_PRESSURE_CTL, p_MessageConfiguration, pCANCommunicator, pParentNode),
    m_unCanIDError(0), m_unCanIDErrorReq(0),
    m_unCanIDPressureSet(0), m_unCanIDFanWatchdogSet(0), m_unCanIDCurrentWatchdogSet(0),
    m_unCanIDPIDParamSet(0), m_unCanIDPumpTimeSet(0),
    m_unCanIDPressureReq(0), m_unCanIDPressure(0),
    m_unCanIDPIDParamReq(0), m_unCanIDPIDParam(0),
    m_unCanIDPumpTimeReq(0), m_unCanIDPumpTime(0),
    m_unCanIDServiceSensorReq(0), m_unCanIDServiceSensor(0),
    m_unCanIDServiceFanReq(0), m_unCanIDServiceFan(0),
    m_unCanIDHardwareReq(0), m_unCanIDHardware(0),m_unCanIDValveSet(0),
    m_unCanIDCalibration(0),m_unCanIDPWMParamSet(0), m_aktionTimespan(0)
{
    // main state
    m_mainState = FM_MAIN_STATE_BOOTUP;
    // configuration state
    m_subStateConfig = FM_PRESSURE_SUB_STATE_CONFIG_INIT;

    //module command  array initialisation
    for(quint8 idx = 0; idx < MAX_PRESSURE_MODULE_CMD_IDX; idx++)
    {
        m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CPressureControl
 */
/****************************************************************************/
CPressureControl::~CPressureControl()
{
    /// \todo Auto-generated destructor stub
}

/****************************************************************************/
/*!
 *  \brief  Initialize this function module
 *
 *  The CAN-IDs are read from the CAN-Message configuration class, and the CAN-ID are composed
 *  Receiveable CAN-message are registered to the communication layer
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::Initialize()
{
    ReturnCode_t RetVal;

    RetVal = InitializeCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = RegisterCANMessages();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_mainState = FM_MAIN_STATE_INIT;
            FILE_LOG_L(laINIT, llDEBUG) << " function module initialized: " << GetName().toStdString();
        }
    }
    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Initialize the module's CAN message IDs
 *
 *  The CAN-IDs are read from the CAN-Message configuration class.
 *  The CAN-ID is composed by the message key, the function
 *  module's channel and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t  CPressureControl::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 bChannel;
    const quint8 ModuleID = MODULE_ID_PRESSURE;

    bChannel = m_pCANObjectConfig->m_sChannel;

    RetVal = InitializeEventCANMessages(ModuleID);

    m_unCanIDPressureSet        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPressureSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDFanWatchdogSet     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlFanWatchdogSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDCurrentWatchdogSet = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlCurrentWatchdogSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDPIDParamSet        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPIDParamSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDPumpTimeSet        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPumpTimeSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDPressureReq        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPressureReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDPressure           = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPressure", bChannel, m_pParent->GetNodeID());
    m_unCanIDPIDParamReq        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPIDParamReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDPIDParam           = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPIDParam", bChannel, m_pParent->GetNodeID());
    m_unCanIDPumpTimeReq        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPumpTimeReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDPumpTime           = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPumpTime", bChannel, m_pParent->GetNodeID());
    m_unCanIDServiceSensorReq   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlServiceSensorReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDServiceSensor      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlServiceSensor", bChannel, m_pParent->GetNodeID());
    m_unCanIDServiceFanReq      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlServiceFanReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDServiceFan         = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlServiceFan", bChannel, m_pParent->GetNodeID());
    m_unCanIDHardwareReq        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlHardwareReq", bChannel, m_pParent->GetNodeID());
    m_unCanIDHardware           = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlHardware", bChannel, m_pParent->GetNodeID());
    m_unCanIDNotiAutoTune       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlNotiAutoTune", bChannel, m_pParent->GetNodeID());
    m_unCanIDNotiInRange        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlNotiInRange", bChannel, m_pParent->GetNodeID());
    m_unCanIDNotiOutOfRange     = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlNotiOutOfRange", bChannel, m_pParent->GetNodeID());
    m_unCanIDValveSet           = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlValveSet", bChannel, m_pParent->GetNodeID());
    m_unCanIDCalibration        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlCalibration", bChannel, m_pParent->GetNodeID());
    m_unCanIDPWMParamSet        = mp_MessageConfiguration->GetCANMessageID(ModuleID, "PressureCtrlPWMParamSet", bChannel, m_pParent->GetNodeID());

    //FILE_LOG_L(laINIT, llDEBUG) << " CAN-messages for fct-module:" << GetName().toStdString() << ",node id:" << std::hex << m_pParent->GetNodeID();
    FILE_LOG_L(laINIT, llDEBUG) << "   EventInfo          : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventWarning       : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventError         : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "   EventFatalError    : 0x" << std::hex << m_unCanIDEventFatalError;
    FILE_LOG_L(laINIT, llDEBUG) << "   PressureSet        : 0x" << std::hex << m_unCanIDPressureSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   FanWatchdogSet     : 0x" << std::hex << m_unCanIDFanWatchdogSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   CurrentWatchdogSet : 0x" << std::hex << m_unCanIDCurrentWatchdogSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   PIDParamSet        : 0x" << std::hex << m_unCanIDPIDParamSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   PumpTimeSet        : 0x" << std::hex << m_unCanIDPumpTimeSet;
    FILE_LOG_L(laINIT, llDEBUG) << "   PressureReq        : 0x" << std::hex << m_unCanIDPressureReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   Pressure           : 0x" << std::hex << m_unCanIDPressure;
    FILE_LOG_L(laINIT, llDEBUG) << "   PIDParamReq        : 0x" << std::hex << m_unCanIDPIDParamReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   PIDParam           : 0x" << std::hex << m_unCanIDPIDParam;
    FILE_LOG_L(laINIT, llDEBUG) << "   PumpTimeReq        : 0x" << std::hex << m_unCanIDPumpTimeReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   PumpTime           : 0x" << std::hex << m_unCanIDPumpTime;
    FILE_LOG_L(laINIT, llDEBUG) << "   ServiceSensorReq   : 0x" << std::hex << m_unCanIDServiceSensorReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   ServiceSensor      : 0x" << std::hex << m_unCanIDServiceSensor;
    FILE_LOG_L(laINIT, llDEBUG) << "   ServiceFanReq      : 0x" << std::hex << m_unCanIDServiceFanReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   ServiceFan         : 0x" << std::hex << m_unCanIDServiceFan;
    FILE_LOG_L(laINIT, llDEBUG) << "   HardwareReq        : 0x" << std::hex << m_unCanIDHardwareReq;
    FILE_LOG_L(laINIT, llDEBUG) << "   Hardware           : 0x" << std::hex << m_unCanIDHardware;
    FILE_LOG_L(laINIT, llDEBUG) << "   NotiAutoTune       : 0x" << std::hex << m_unCanIDNotiAutoTune;
    FILE_LOG_L(laINIT, llDEBUG) << "   NotiInRange        : 0x" << std::hex << m_unCanIDNotiInRange;
    FILE_LOG_L(laINIT, llDEBUG) << "   NotiOutOfRange     : 0x" << std::hex << m_unCanIDNotiOutOfRange;
    //FILE_LOG_L(laINIT, llDEBUG) << "   ValveSet        : 0x" << std::hex << m_unCanIDValveSet;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Register the receive CAN-messages to communication layer
 *
 *  Each receiveable CAN-message must be registered to the communication layer.
 *  This enables the communication layer to call the 'HandelCANMessage(..)' function of this
 *  instance after receiption of the message.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::RegisterCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDError, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDPressure, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDPIDParam, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDPumpTime, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDServiceSensor, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDServiceFan, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDHardware, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDNotiInRange, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDNotiOutOfRange, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDValveSet, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDCalibration, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDPWMParamSet, this);
    }


    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Handles the function module's state machine
 *
 *  Depending on the active main state, the appropriate task function will be called.
 */
/****************************************************************************/
void CPressureControl::HandleTasks()
{
    QMutexLocker Locker(&m_Mutex);

    if(m_mainState == FM_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(FM_MAIN_STATE_CONFIRMED == m_mainState)
    {
        //fall through
        m_subStateConfig = FM_PRESSURE_SUB_STATE_CONFIG_START;
        m_mainState = FM_MAIN_STATE_CONFIG;
    }
    else if(FM_MAIN_STATE_CONFIG == m_mainState)
    {
        if(FM_PRESSURE_SUB_STATE_CONFIG_START == m_subStateConfig)
        {
            m_subStateConfig = FM_PRESSURE_SUB_STATE_CONFIG_SEND;
            StartTimeDelay();
        }
        else if(FM_PRESSURE_SUB_STATE_CONFIG_SEND == m_subStateConfig)
        {
            ReturnCode_t RetVal;
            CANFctModulePressureCtrl* pCANObjConfPressure = (CANFctModulePressureCtrl*) m_pCANObjectConfig;

            RetVal = SendCANMsgFanWatchdogSet();
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS) {
                FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                m_mainState = FM_MAIN_STATE_ERROR;
                return;
            }
            RetVal = SendCANMsgCurrentWatchdogSet();
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS) {
                FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                m_mainState = FM_MAIN_STATE_ERROR;
                return;
            }
            for(qint32 i = 0; i < pCANObjConfPressure->listPidControllers.size(); i++) {
                RetVal = SendCANMsgPidParametersSet(i);
                if(RetVal != DCL_ERR_FCT_CALL_SUCCESS) {
                    FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                    m_mainState = FM_MAIN_STATE_ERROR;
                    return;
                }
            }
            {
                RetVal = SendCANMsgSetPWMParam(pCANObjConfPressure->pwmController.sMaxActuatingValue ,\
                                               pCANObjConfPressure->pwmController.sMinActuatingValue, \
                                               pCANObjConfPressure->pwmController.sMaxPwmDuty, \
                                               pCANObjConfPressure->pwmController.sMinPwmDuty);
                if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
                {
                    FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": config failed, SendCOB returns" << (int) RetVal;
                    m_mainState = FM_MAIN_STATE_ERROR;
                    return;
                }
            }

            m_subStateConfig = FM_PRESSURE_SUB_STATE_CONFIG_FINISHED;
            m_TaskID    = MODULE_TASKID_FREE;
            m_mainState = FM_MAIN_STATE_IDLE;
            FILE_LOG_L(laCONFIG, llDEBUG) << " Module " << GetName().toStdString() << ": change to FM_MAIN_STATE_IDLE";
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the function module's state machine in idle main state
 *
 *      Depending on the active task ID, the appropriate task function will
 *      be called.
 */
/****************************************************************************/
void CPressureControl::HandleIdleState()
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        HandleCommandRequestTask();
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the task to execute the module commands
 *
 *  The function is called from HandleIdleState() if m_taskID == FM_PRESSURE_TASKID_COMMAND_HDL.
 *  The function loops thru the m_ModuleCommand array and controls the commands stored there
 *
 *   - Module command with state MODULE_CMD_STATE_REQ will be forwarded to the pressure ctrl. function module
 *     on slave side by sending the corresponding CAN-messge
 *
 *   - Module command with state MODULE_CMD_STATE_REQ_SEND will be checked for timeout
 *
 *  Usually, a motor command will be confirmed and closed by receiving the expected CAN-message (e.g. act. pressure)
 *  This is done at the HandleCANMessage... function
 */
/****************************************************************************/
void CPressureControl::HandleCommandRequestTask()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_NOT_FOUND;
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_PRESSURE_MODULE_CMD_IDX; idx++)
    {
        if(m_ModuleCommand[idx].State == MODULE_CMD_STATE_REQ)
        {
            // forward the motor command to the motor function module on slave side by sending
            // the corresponding CAN-message
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_SET_PRESSURE)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl set reference pressure";

                //send the reference pressure to the slave
                RetVal = SendCANMsgSetPressure(m_ModuleCommand[idx].flag , m_ModuleCommand[idx].Pressure,\
                ((CANFctModulePressureCtrl*) m_pCANObjectConfig)->bPressureTolerance,\
                ((CANFctModulePressureCtrl*) m_pCANObjectConfig)->sSamplingPeriod, \
                0);
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
                //because there is no slave acknowledge, we send our own acknowldege
                emit ReportRefPressure(GetModuleHandle(), RetVal, m_ModuleCommand[idx].Pressure);
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_ACTPRESSURE)
            {
                //send the act pressure request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDPressure CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl send pressure request";
                RetVal = SendCANMsgServiceSensorRequest(0);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].Timeout = CAN_PRESSURECTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportActPressure(GetModuleHandle(), RetVal, m_ModuleCommand[idx].Index, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_SET_OPMODE)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl set operation mode";

                //send the pressure ctrl operating mode to the slave
               // RetVal = SendCANMsgSetPressure(m_ModuleCommand[idx].Pressure, m_ModuleCommand[idx].PressureCtrlOpMode, m_ModuleCommand[idx].PressureCtrlState);
                RetVal = SendCANMsgSetPressure(m_ModuleCommand[idx].flag , m_ModuleCommand[idx].Pressure,\
                ((CANFctModulePressureCtrl*) m_pCANObjectConfig)->bPressureTolerance,\
                ((CANFctModulePressureCtrl*) m_pCANObjectConfig)->sSamplingPeriod, \
                0);
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
                //because there is no slave acknowledge, we send our own acknowledge
                emit ReportSetOperatingModeAckn(GetModuleHandle(), RetVal, m_ModuleCommand[idx].PressureCtrlOpMode);
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_ACTOPMODE)
            {
                //send the act operation mode request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDPressure CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl send pressure request";
                RetVal = SendCANMsgPressureRequest();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].Timeout = CAN_PRESSURECTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportActOperatingMode(GetModuleHandle(), RetVal, PRESSURECTRL_OPMODE_UNDEF);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_SET_STATUS)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl set status";

                //send the pressure ctrl status to the slave
               // RetVal = SendCANMsgSetPressure(m_ModuleCommand[idx].Pressure, m_ModuleCommand[idx].PressureCtrlOpMode, m_ModuleCommand[idx].PressureCtrlState);
                RetVal = SendCANMsgSetPressure(m_ModuleCommand[idx].flag , m_ModuleCommand[idx].Pressure,\
                ((CANFctModulePressureCtrl*) m_pCANObjectConfig)->bPressureTolerance,\
                ((CANFctModulePressureCtrl*) m_pCANObjectConfig)->sSamplingPeriod, \
                0);
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
                //because there is no slave acknowledge, we send our own acknowldege
                emit ReportSetStatusAckn(GetModuleHandle(), RetVal, m_ModuleCommand[idx].PressureCtrlState);
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_ACTSTATUS)
            {
                //send the act pressure ctrl. status request to the slave, this command will be acknowledged by the receiption
                // of the m_unCanIDPressure CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl send pressure request";
                RetVal = SendCANMsgPressureRequest();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].Timeout = CAN_PRESSURECTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportActStatus(GetModuleHandle(), RetVal, PRESSURECTRL_STATUS_UNDEF, PRESSURECTRL_VOLTAGE_UNDEF);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_RESET_OPTIME)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl reset pump operating time";

                //send the reset command to the slave
                RetVal = SendCANMsgPumpTimeSet(m_ModuleCommand[idx].Index);

                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
                //because there is no slave acknowledge, we send our own acknowldege
                emit ReportResetPumpOperatingTime(GetModuleHandle(), RetVal, m_ModuleCommand[idx].Index);
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_OPTIME)
            {
                //send the act pressure ctrl. get operating time request to the slave, this command
                //will be acknowledged by the receiption of the m_unCanIDPumpTime CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl send get operating time";
                RetVal = SendCANMsgPumpTimeReq(m_ModuleCommand[idx].Index);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].Timeout = CAN_PRESSURECTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportPumpOperatingTime(GetModuleHandle(), RetVal, m_ModuleCommand[idx].State, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_FANSPEED)
            {
                //send the act pressure ctrl. get fan speed request to the slave, this command
                //will be acknowledged by the receiption of the m_unCanIDFanSpeed CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl send pressure get fan speed";
                RetVal = SendCANMsgServiceFanReq(m_ModuleCommand[idx].Index);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].Timeout = CAN_PRESSURECTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportFanSpeed(GetModuleHandle(), RetVal, m_ModuleCommand[idx].State, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_HARDWARE)
            {
                //send the act pressure ctrl. get fan speed request to the slave, this command
                //will be acknowledged by the receiption of the m_unCanIDFanSpeed CAN-message
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl send presure. get fan speed";
                RetVal = SendCANMsgHardwareReq();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ_SEND;
                    m_ModuleCommand[idx].Timeout = CAN_PRESSURECTRL_TIMEOUT_STATUS_SET_REQ;
                }
                else
                {
                    emit ReportHardwareStatus(GetModuleHandle(), RetVal, 0, 0, 0, 0, 0);
                }
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_SET_VALVE)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl set status";

                //send the pressure ctrl status to the slave

                RetVal = SendCANMsgSetValve(m_ModuleCommand[idx].ValveIndex, m_ModuleCommand[idx].ValveState);
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;

                //because there is no slave acknowledge, we send our own acknowldege
                emit ReportRefValveState(GetModuleHandle(), RetVal, m_ModuleCommand[idx].ValveIndex, m_ModuleCommand[idx].ValveState);
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_CALIBRATION)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl Calibration";

                //send the pressure ctrl status to the slave

                RetVal = SendCANMsgCalibration(m_ModuleCommand[idx].EnableCalibration);
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;

                //because there is no slave acknowledge, we send our own acknowldege
                //emit ReportRefCalibration(GetModuleHandle(), RetVal, m_ModuleCommand[idx].EnableCalibration);
            }
            else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_SET_PWM)
            {
                FILE_LOG_L(laFCT, llDEBUG1) << " CANPressureControl Set PWM Params";

                //send the PWM parameters to the slave

                RetVal = SendCANMsgSetPWMParam(m_ModuleCommand[idx].MaxActuatingValue, \
                                               m_ModuleCommand[idx].MinActuatingValue, \
                                               m_ModuleCommand[idx].MaxPwmDuty, \
                                               m_ModuleCommand[idx].MinPwmDuty);

                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
            }
            //---------------------------
            //check for success
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                //trigger timeout supervision
                m_ModuleCommand[idx].ReqSendTime.Trigger();
            }
            else
            {
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
            }
        }
        else if(m_ModuleCommand[idx].State == MODULE_CMD_STATE_REQ_SEND)
        {
            // check avtive motor commands for timeout
            ActiveCommandFound = true;
            if(m_ModuleCommand[idx].ReqSendTime.Elapsed() > m_ModuleCommand[idx].Timeout)
            {
                m_lastErrorHdlInfo = DCL_ERR_TIMEOUT;
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
                if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_ACTPRESSURE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANPressureControl '" << GetKey().toStdString() <<
                                                  "': Act pressurel request timeout error.";
                    emit ReportActPressure(GetModuleHandle(), m_lastErrorHdlInfo, m_ModuleCommand[idx].Index, 0);
                }
                else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_ACTOPMODE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANPressureControl '" << GetKey().toStdString() <<
                                                  "': Act op. mode request timeout error.";
                    emit ReportActOperatingMode(GetModuleHandle(), m_lastErrorHdlInfo, PRESSURECTRL_OPMODE_UNDEF);
                }
                else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_ACTSTATUS)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANPressureControl '" << GetKey().toStdString() <<
                                                  "': Act status request timeout error.";
                    emit ReportActStatus(GetModuleHandle(), m_lastErrorHdlInfo, PRESSURECTRL_STATUS_UNDEF, PRESSURECTRL_VOLTAGE_UNDEF);
                }
                else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_OPTIME)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANPressureControl '" << GetKey().toStdString() <<
                                                  "': Operating time request timeout error.";
                    emit ReportPumpOperatingTime(GetModuleHandle(), m_lastErrorHdlInfo, m_ModuleCommand[idx].Index, 0);
                }
                else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_FANSPEED)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANPressureControl '" << GetKey().toStdString() <<
                                                  "': Fan speed request timeout error.";
                    emit ReportFanSpeed(GetModuleHandle(), m_lastErrorHdlInfo, m_ModuleCommand[idx].Index, 0);
                }
                else if(m_ModuleCommand[idx].Type == FM_PRESSURE_CMD_TYPE_REQ_HARDWARE)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANPressureControl '" << GetKey().toStdString() <<
                                                  "': Hardware information request timeout error.";
                    emit ReportHardwareStatus(GetModuleHandle(), m_lastErrorHdlInfo, 0, 0, 0, 0, 0);
                }
            }
        }
    }

    if(ActiveCommandFound == false)
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a CAN message
 *
 *  The function is called from communication layer if a CAN message, which
 *  was registered to this class instance, was received.
 *  The message will be forwarded to the specialized function.
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 *
 *  \return void
 */
/****************************************************************************/
void CPressureControl::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    FILE_LOG_L(laFCT, llDEBUG1) << "   CPressureControl::HandleCanMessage 0x" << std::hex << pCANframe->can_id;

    if((pCANframe->can_id == m_unCanIDEventInfo) ||
            (pCANframe->can_id == m_unCanIDEventWarning) ||
            (pCANframe->can_id == m_unCanIDEventError) ||
            (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        HandleCANMsgError(pCANframe);
        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportError(GetModuleHandle(), m_lastErrorGroup, m_lastErrorCode, m_lastErrorData, m_lastErrorTime);
        }
    }
    else if(pCANframe->can_id == m_unCanIDPressure)
    {
        HandleCANMsgPressure(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDServiceSensor)
    {
        HandleCANMsgServiceSensor(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDPumpTime)
    {
        HandleCANMsgPumpTime(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDServiceFan)
    {
        HandleCANMsgServiceFan(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDHardware)
    {
        HandleCANMsgHardware(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDNotiInRange)
    {
        HandleCANMsgNotiRange(pCANframe, true);
    }
    else if(pCANframe->can_id == m_unCanIDNotiOutOfRange)
    {
        HandleCANMsgNotiRange(pCANframe, false);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'ServiceSensor' response CAN message
 *
 *      The message contains information to a sensor's pressure.
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CPressureControl::HandleCANMsgServiceSensor(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_PRESSURE_CMD_TYPE_REQ_ACTPRESSURE);
    }

    if(pCANframe->can_dlc == 3)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        qreal ActPressure = (qreal)GetCANMsgDataS16(pCANframe, 1) / 100;

        FILE_LOG_L(laFCT, llDEBUG) << " CANPressureControl Pressure received: " << ActPressure;
        emit ReportActPressure(GetModuleHandle(), hdlInfo, pCANframe->data[0], ActPressure);
    }
    else
    {
        emit ReportActPressure(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief   Handles the 'Pressure' response CAN message
 *
 *      The message contains information to
 *       - pressure control status and operation mode
 *       - desired pressure
 *       - tolerance
 *       - sampling rate
 *       - auto tuning duration
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CPressureControl::HandleCANMsgPressure(can_frame* pCANframe)
{
    ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    PressureCtrlStatus_t PressureCtrlStatus = PRESSURECTRL_STATUS_OFF;
    PressureCtrlOperatingMode_t PressureCtrlOpMode = PRESSURECTRL_OPMODE_HOLD;
    PressureCtrlMainsVoltage_t PressureCtrlVoltage = PRESSURECTRL_VOLTAGE_220V;

    if(pCANframe->can_dlc == 7)
    {
        //determine the pressure control status
        if(pCANframe->data[0] & (1 << PRESSURE_CTRL_ACTIVE_BITPOS))
        {
            PressureCtrlStatus = PRESSURECTRL_STATUS_ON;
        }

        //determine the pressure control operating mode
        if(pCANframe->data[0] & (1 << PRESSURE_CTRL_PHASE_BITPOS))
        {
            PressureCtrlOpMode = PRESSURECTRL_OPMODE_HOLD;
        }

        //determine the pressure control mains voltage
        if(pCANframe->data[0] & (1 << PRESSURE_CTRL_VOLTAGE_BITPOS))
        {
            PressureCtrlVoltage = PRESSURECTRL_VOLTAGE_110V;
        }

        FILE_LOG_L(laFCT, llDEBUG) << "   CPressureControl::HandleCANMsgPressure: "
                                   << PressureCtrlStatus << ", " << PressureCtrlOpMode << ", " << PressureCtrlVoltage;
    }
    else
    {
        hdlInfo = DCL_ERR_CANMSG_INVALID;
    }

    //forward the status information, if still requested
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_PRESSURE_CMD_TYPE_REQ_ACTSTATUS);
        emit ReportActStatus(GetModuleHandle(), hdlInfo, PressureCtrlStatus, PressureCtrlVoltage);
    }
    //forward the operating mode, if still requested
    else if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_PRESSURE_CMD_TYPE_REQ_ACTOPMODE);
        emit ReportActOperatingMode(GetModuleHandle(), hdlInfo, PressureCtrlOpMode);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'PumpTime' response CAN message
 *
 *      The message contains information to a operating time counter of a
 *      pump.
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CPressureControl::HandleCANMsgPumpTime(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_PRESSURE_CMD_TYPE_REQ_OPTIME);
    }

    if(pCANframe->can_dlc == 5)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        quint32 OperatingTime;

        OperatingTime = GetCANMsgDataU32(pCANframe, 1);

        FILE_LOG_L(laFCT, llDEBUG) << " CANPressureControl operating time received: " << OperatingTime;
        emit ReportPumpOperatingTime(GetModuleHandle(), hdlInfo, pCANframe->data[0], OperatingTime);
    }
    else
    {
        emit ReportPumpOperatingTime(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'ServiceFan' response CAN message
 *
 *      The message contains information to a ventilation fan's speed.
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CPressureControl::HandleCANMsgServiceFan(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_PRESSURE_CMD_TYPE_REQ_FANSPEED);
    }

    if(pCANframe->can_dlc == 3)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        quint16 FanSpeed;

        FanSpeed = GetCANMsgDataU16(pCANframe, 1);

        FILE_LOG_L(laFCT, llDEBUG) << " CANPressureControl fan speed received: " << FanSpeed;
        emit ReportFanSpeed(GetModuleHandle(), hdlInfo, pCANframe->data[0], FanSpeed);
    }
    else
    {
        emit ReportFanSpeed(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'ServiceFan' response CAN message
 *
 *      The message contains information to a boards's hardware setup.
 *
 *  \iparam pCANframe = The received CAN message
 */
/****************************************************************************/
void CPressureControl::HandleCANMsgHardware(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        ResetModuleCommand(FM_PRESSURE_CMD_TYPE_REQ_HARDWARE);
    }

    if(pCANframe->can_dlc == 6)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        quint16 Current;

        Current = GetCANMsgDataU16(pCANframe, 4);

        FILE_LOG_L(laFCT, llDEBUG) << " CANPressureControl hardware information received: " << Current;
        emit ReportHardwareStatus(GetModuleHandle(), hdlInfo, pCANframe->data[0], pCANframe->data[1],
                                  pCANframe->data[2], pCANframe->data[3], Current);
    }
    else
    {
        emit ReportHardwareStatus(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0, 0, 0, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the 'NotiInRange' and 'NotiOutOfRange' response message
 *
 *      The message is received when the measured pressure leaves or
 *      enters a predefined range.
 *
 *  \iparam pCANframe = The received CAN message
 *  \iparam InRange = Pressure in range (true) or out of range (false)
 */
/****************************************************************************/
void CPressureControl::HandleCANMsgNotiRange(can_frame* pCANframe, bool InRange)
{
    if(pCANframe->can_dlc == 2)
    {
        ReturnCode_t hdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        qreal ActPressure;

        ActPressure = (qreal)GetCANMsgDataU16(pCANframe, 1) / 100;

        FILE_LOG_L(laFCT, llDEBUG) << " CANPressureControl pressure range notification received: " << ActPressure;
        emit ReportPressureRange(GetModuleHandle(), hdlInfo, InRange, ActPressure);
    }
    else
    {
        emit ReportPressureRange(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, InRange, 0);
    }
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set the fan parameters
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgFanWatchdogSet()
{
    CANFctModulePressureCtrl* pCANObjConfPressure;
    pCANObjConfPressure = (CANFctModulePressureCtrl*) m_pCANObjectConfig;
    can_frame canmsg;
    ReturnCode_t RetVal;

    //the following parameters must be send to the pressure control:
    if(pCANObjConfPressure)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDFanWatchdogSet;
        canmsg.can_id = m_unCanIDFanWatchdogSet;
        SetCANMsgDataU16(&canmsg, pCANObjConfPressure->sFanSpeed, 0);
        SetCANMsgDataU16(&canmsg, pCANObjConfPressure->sFanThreshold, 2);
        canmsg.can_dlc = 4;

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }
    else
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": configuration not available";
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set the pump current parameters
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgCurrentWatchdogSet()
{
    CANFctModulePressureCtrl* pCANObjConfPressure;
    pCANObjConfPressure = (CANFctModulePressureCtrl*) m_pCANObjectConfig;
    can_frame canmsg;
    ReturnCode_t RetVal;

    //the following parameters must be send to the pressure control:
    if(pCANObjConfPressure)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDCurrentWatchdogSet;
        canmsg.can_id = m_unCanIDCurrentWatchdogSet;
        SetCANMsgDataU16(&canmsg, pCANObjConfPressure->sCurrentGain, 0);
        SetCANMsgDataU16(&canmsg, pCANObjConfPressure->sPumpCurrent, 2);
        SetCANMsgDataU16(&canmsg, pCANObjConfPressure->sPumpThreshold, 4);
        canmsg.can_dlc = 6;

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }
    else
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": configuration not available";
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set the pump current parameters
 *
 *  \iparam Index = Index of the PID controller
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgPidParametersSet(quint8 Index)
{
    CANFctModulePressureCtrl* pCANObjConfPressure;
    pCANObjConfPressure = (CANFctModulePressureCtrl*) m_pCANObjectConfig;
    can_frame canmsg;
    ReturnCode_t RetVal;

    //the following parameters must be send to the pressure control:
    if(pCANObjConfPressure)
    {
        FILE_LOG_L(laCONFIG, llDEBUG) << GetName().toStdString() << ": send configuration: 0x" << std::hex << m_unCanIDPIDParamSet;
        canmsg.can_id = m_unCanIDPIDParamSet;
        canmsg.data[0] = Index;
        canmsg.data[1] = pCANObjConfPressure->listPidControllers[Index].sMaxPressure;
        canmsg.data[2] = pCANObjConfPressure->listPidControllers[Index].sMinPressure;
        SetCANMsgDataU16(&canmsg, pCANObjConfPressure->listPidControllers[Index].sControllerGain, 3);
        SetCANMsgDataU16(&canmsg, pCANObjConfPressure->listPidControllers[Index].sResetTime, 5);
        canmsg.data[7] = pCANObjConfPressure->listPidControllers[Index].sDerivativeTime;
        //SetCANMsgDataU16(&canmsg, pCANObjConfPressure->listPidControllers[Index].sDerivativeTime, 6);
        canmsg.can_dlc = 8;

        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }
    else
    {
        FILE_LOG_L(laCONFIG, llERROR) << " Module " << GetName().toStdString() << ": configuration not available";
        m_lastErrorHdlInfo = DCL_ERR_NULL_PTR_ACCESS;
        RetVal = DCL_ERR_FCT_CALL_FAILED;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set a desired pressure
 *
 *  \iparam flag = 1: Pressure 9: Vaccum
 *  \iparam Pressure = Reference Pressure in 1 kpa steps
 *  \iparam Tolerance = Pressure tolerance in 1 kpa
 *  \iparam SamplingTime = sampleing time
 *  \iparam DurationTime = duration time
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgSetPressure(quint8 flag, qint8 Pressure, quint8 Tolerance, quint16 SamplingTime, quint16 DurationTime)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDPressureSet;

    canmsg.data[0] = flag;
    canmsg.data[1] = Pressure;
    canmsg.data[2] = Tolerance;

    SetCANMsgDataU16(&canmsg, SamplingTime , 3);
    SetCANMsgDataU16(&canmsg, DurationTime , 5);

    canmsg.can_dlc = 7;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   SendCANMsgSetPressure: CanID: 0x" << std::hex << m_unCanIDPressureSet;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set valves' state
 *
 *  \iparam ValveIndex = index of the valves, start from 0
 *  \iparam falgs = 0: close  1: open
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgSetValve(quint8 ValveIndex, quint8 flags)
{

    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDValveSet;
    canmsg.data[0] = ValveIndex;
    canmsg.data[1] = flags;

    FILE_LOG_L(laFCT, llDEBUG) << "   SendCANMsgSetValve: Data0: 0x" << std::hex << (quint8) canmsg.data[0]
                               << ", Data1: 0x" << std::hex << (quint8) canmsg.data[1];
    canmsg.can_dlc = 2;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   SendCANMsgSetPressure: CanID: 0x" << std::hex << m_unCanIDValveSet;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set PWM parameters.
 *
 *  \iparam MaxActuatingValue = Maximum actuaing value
 *  \iparam MinActuatingValue = Minumum actuaing value
 *  \iparam MaxPwmDuty = Maximum PWM duty
 *  \iparam MinPwmDuty = Minumum PWM duty
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgSetPWMParam(quint16 MaxActuatingValue, quint16 MinActuatingValue, quint8 MaxPwmDuty, quint8 MinPwmDuty)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDPWMParamSet;
    SetCANMsgDataU16(&canmsg, MaxActuatingValue, 0);
    SetCANMsgDataU16(&canmsg, MinActuatingValue, 2);
    canmsg.data[4] = MaxPwmDuty;
    canmsg.data[5] = MinPwmDuty;

    FILE_LOG_L(laFCT, llDEBUG) << "   SendCANMsgPWMParamSet: Data0: 0x" << std::hex << (quint8) canmsg.data[0];
    canmsg.can_dlc = 6;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   SendCANMsgPWMParamSet: CanID: 0x" << std::hex << m_unCanIDPWMParamSet;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to enable/disable calibration function.
 *
 *  \iparam Enabl= true: Enable  false: Disable
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgCalibration(bool Enable)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDCalibration;
    canmsg.data[0] = (quint8)Enable;
    FILE_LOG_L(laFCT, llDEBUG) << "   SendCANMsgCalibration: Data0: 0x" << std::hex << (quint8) canmsg.data[0];
    canmsg.can_dlc = 1;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   SendCANMsgSetPressure: CanID: 0x" << std::hex << m_unCanIDCalibration;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the 'Pressure' CAN-Message
 *
 *      The 'Pressure' CAN-Message contains the reference pressure and
 *      the operation mode.
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgPressureRequest()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDPressureReq;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CPressureControl::SendCANMsgPressureRequest canID: 0x"
                               << std::hex << m_unCanIDPressureReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the 'PumpTimeSet' CAN-Message
 *
 *      The 'ServiceSensor' CAN-Message contains the pressure measured by
 *      one of the sensors of the Slave module.
 *
 *  \iparam Index = Number of the sensor
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgServiceSensorRequest(quint8 Index)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDServiceSensorReq;
    canmsg.data[0] = Index;
    canmsg.can_dlc = 1;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CPressureControl::SendCANMsgServiceSensorRequest canID: 0x"
                               << std::hex << m_unCanIDServiceSensorReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to reset the operating time of a pump
 *
 *  \iparam Index = Number of the pump
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgPumpTimeSet(quint8 Index)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDPumpTimeSet;
    canmsg.data[0] = Index;
    canmsg.can_dlc = 1;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CPressureControl::SendCANMsgPumpTimeSet canID: 0x"
                               << std::hex << m_unCanIDPumpTimeSet;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to get the operating time of a pump
 *
 *  \iparam Index = Number of the pump
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgPumpTimeReq(quint8 Index)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDPumpTimeReq;
    canmsg.data[0] = Index;
    canmsg.can_dlc = 1;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CPressureControl::SendCANMsgPumpTimeReq canID: 0x"
                               << std::hex << m_unCanIDPumpTimeReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to get speed of a ventilation fan
 *
 *  \iparam Index = Number of the fan
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgServiceFanReq(quint8 Index)
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDServiceFanReq;
    canmsg.data[0] = Index;
    canmsg.can_dlc = 1;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CPressureControl::SendCANMsgServiceFanReq canID: 0x"
                               << std::hex << m_unCanIDServiceFanReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to get hardware information
 *
 *  \return The return value is set from SendCOB(can_frame)
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SendCANMsgHardwareReq()
{
    ReturnCode_t retval = DCL_ERR_FCT_CALL_SUCCESS;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDHardwareReq;
    canmsg.can_dlc = 0;
    retval = m_pCANCommunicator->SendCOB(canmsg);

    FILE_LOG_L(laFCT, llDEBUG) << "   CPressureControl::m_unCanIDHardwareReq canID: 0x"
                               << std::hex << m_unCanIDHardwareReq;

    return retval;
}

/****************************************************************************/
/*!
 *  \brief  Set the target pressure
 *
 *  \iparam flag = 1: Pressure, 9: Vaccum
 *  \iparam Pressure = Reference pressure
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SetPressure(quint8 flag, qreal Pressure)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_SET_PRESSURE, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].flag = flag;
        m_ModuleCommand[CmdIndex].Pressure = Pressure;
        FILE_LOG_L(laDEV, llINFO) << " CPressureControl, Pressure: " << Pressure;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CPressureControl, Invalid state: " << m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set the valves' state
 *
 *  \iparam ValveIndex: Index of the valves, start from 0
 *  \iparam ValveState = 0: close, 1: open
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SetValve(quint8 ValveIndex, quint8 ValveState)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_SET_VALVE, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].ValveIndex = ValveIndex;
        m_ModuleCommand[CmdIndex].ValveState = ValveState;
        FILE_LOG_L(laDEV, llINFO) << " CPressureControl, Valve Index: " << ValveIndex;
        FILE_LOG_L(laDEV, llINFO) << " CPressureControl, Valve State: " << ValveState;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CPressureControl, Invalid state: " << m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Enable/Disable calibration function
 *
 *  \iparam Enable: true: Enable, false: Disbale
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SetCalibration(bool Enable)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_CALIBRATION, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].EnableCalibration = Enable;
        FILE_LOG_L(laDEV, llINFO) << " CPressureControl, Enable Calibration: " << Enable;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CPressureControl, Invalid state: " << m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set PWM parameters.
 *
 *  \iparam MaxActuatingValue = Maximum actuaing value
 *  \iparam MinActuatingValue = Minumum actuaing value
 *  \iparam MaxPwmDuty = Maximum PWM duty
 *  \iparam MinPwmDuty = Minumum PWM duty
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SetPWMParams(quint16 maxActuatingValue, quint16 minActuatingValue, quint8 maxPwmDuty, quint8 minPwmDuty)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_SET_PWM, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].MaxActuatingValue = maxActuatingValue;
        m_ModuleCommand[CmdIndex].MinActuatingValue = minActuatingValue;
        m_ModuleCommand[CmdIndex].MaxPwmDuty = maxPwmDuty;
        m_ModuleCommand[CmdIndex].MinPwmDuty = minPwmDuty;

        FILE_LOG_L(laDEV, llINFO) << " CPressureControl, Set PWM Params: " << maxActuatingValue << minActuatingValue << maxPwmDuty << minPwmDuty;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CPressureControl, Invalid state: " << m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the actual pressure
 *
 *  \iparam Index = Pressure sensor index
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::ReqActPressure(quint8 Index)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_REQ_ACTPRESSURE, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].Index = Index;
        FILE_LOG_L(laDEV, llINFO) << " CANPressureControl, Index: " << Index;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANPressureControl invalid state: " << m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set the pressure control's operation mode
 *
 *  \iparam PressureCtrlOpMode = Operation mode to set
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SetOperatingMode(PressureCtrlOperatingMode_t PressureCtrlOpMode)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8  CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_SET_OPMODE, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].PressureCtrlOpMode = PressureCtrlOpMode;
        FILE_LOG_L(laDEV, llINFO) << " CANPressureControl, PressureCtrlOpMode: " << (int) PressureCtrlOpMode;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANPressureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the operating mode
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::ReqOperatingMode()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_REQ_ACTOPMODE))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANPressureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANPressureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set the pressure control's status
 *
 *  \iparam PressureCtrlState = Status mode to set
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::SetStatus(PressureCtrlStatus_t PressureCtrlState)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_SET_STATUS, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].PressureCtrlState = PressureCtrlState;
        FILE_LOG_L(laDEV, llINFO) << " CANPressureControl, PressureCtrlState: " << (int) PressureCtrlState;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANPressureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the pressure control status
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::ReqStatus()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_REQ_ACTSTATUS))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANPressureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANPressureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Resets the operating time of a pump
 *
 *  \iparam  Index = Index of the pump element
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::ResetPumpOperatingTime(quint8 Index)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_RESET_OPTIME, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].Index = Index;
        FILE_LOG_L(laDEV, llDEBUG) << " CANPressureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANPressureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Gets the operating time of a pump
 *
 *  \iparam  Index = Index of the pump
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::GetPumpOperatingTime(quint8 Index)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_REQ_OPTIME, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].Index = Index;
        FILE_LOG_L(laDEV, llDEBUG) << " CANPressureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANPressureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Gets the speed of a ventilation fan
 *
 *  \iparam  Index = Index of the ventilation fan
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::GetFanSpeed(quint8 Index)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;
    quint8 CmdIndex;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_REQ_FANSPEED, &CmdIndex))
    {
        m_ModuleCommand[CmdIndex].Index = Index;
        FILE_LOG_L(laDEV, llDEBUG) << " CANPressureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANPressureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Gets information of the hardware connected to the module
 *
 *      This method will return the following information from the
 *      pressure control module on the Slave: number of pressure
 *      sensors, number of ventilation fans, number of pumps, number of PID
 *      controllers in the control loop, current through the pump
 *      in milliamperes.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request was accepted
 *          otherwise an error code
 */
/****************************************************************************/
ReturnCode_t CPressureControl::GetHardwareStatus()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(SetModuleTask(FM_PRESSURE_CMD_TYPE_REQ_HARDWARE))
    {
        FILE_LOG_L(laDEV, llDEBUG) << " CANPressureControl";
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANPressureControl invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Helper function, sets a free module command to the given command type
 *
 *  \iparam  CommandType = command type to set
 *  \iparam  pCmdIndex = pointer to index within the command array the command is set to (optional parameter, default 0)
 *
 *  \return  true, if the command type can be placed, otherwise false
 */
/****************************************************************************/
bool CPressureControl::SetModuleTask(CANPressureCtrlCmdType_t CommandType, quint8* pCmdIndex)
{
    bool CommandAdded = false;

    if((m_TaskID == MODULE_TASKID_FREE) || (m_TaskID == MODULE_TASKID_COMMAND_HDL))
    {
        for(quint8 idx = 0; idx < MAX_PRESSURE_MODULE_CMD_IDX; idx++)
        {
            if(m_ModuleCommand[idx].State == MODULE_CMD_STATE_FREE)
            {
                m_ModuleCommand[idx].State = MODULE_CMD_STATE_REQ;
                m_ModuleCommand[idx].Type = CommandType;

                m_TaskID = MODULE_TASKID_COMMAND_HDL;
                CommandAdded  = true;
                if(pCmdIndex)
                {
                    *pCmdIndex = idx;
                }

                FILE_LOG_L(laFCT, llINFO) << " CANPressureControl:  task " << (int) idx << " request.";
                break;
            }
        }
    }

    return CommandAdded;
}

/****************************************************************************/
/*!
 *  \brief  Set the ModuleCommands with the specified command type to 'FREE'
 *
 *  \iparam ModuleCommandType = ModuleCommands having this command type will be set to free
 */
/****************************************************************************/
void CPressureControl::ResetModuleCommand(CANPressureCtrlCmdType_t ModuleCommandType)
{
    bool ActiveCommandFound = false;

    for(quint8 idx = 0; idx < MAX_PRESSURE_MODULE_CMD_IDX; idx++)
    {
        if((m_ModuleCommand[idx].Type == ModuleCommandType) &&
                (m_ModuleCommand[idx].State == MODULE_CMD_STATE_REQ_SEND))
        {
            m_ModuleCommand[idx].State = MODULE_CMD_STATE_FREE;
        }

        if(m_ModuleCommand[idx].State != MODULE_CMD_STATE_FREE)
        {
            ActiveCommandFound = true;
        }
    }

    if(ActiveCommandFound == false)
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}
} //namespace
#endif

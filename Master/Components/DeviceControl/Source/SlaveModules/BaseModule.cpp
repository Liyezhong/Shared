/****************************************************************************/
/*! \file BaseModule.cpp
 *
 *  \brief Implementation file for class CBaseModule.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *
 *  \b Description:
 *
 *       This module contains the implementation of the class CANNode
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

#include "DeviceControl/Include/Configuration/CANMessageConfiguration.h"
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
#include "Global/Include/AdjustedTime.h"

namespace DeviceControl
{

#define NODE_CFG_LAST_FCT_VALUE 0xFF    //!< Code for last function module in list

#define CANNODE_DELAY_CONFIG_REQUEST 500        //!< timeout configuration request
#define CANNODE_MIN_HARDWAREID_REC_DELAY 900    //!< time delay between two 'HardwareID' reception

#define CANNODE_CANMSG_DCL_HARDWAREID 7     //!< Data code length of 'HardwareID' CAN-message
#define CANNODE_CANMSG_DCL_CONFIGURATION  6 //!< Data code length of 'Configuration' CAN-message

QMap<quint32, std::string> CBaseModule::m_eventString;  // static string list with info strings for CAN events

/****************************************************************************/
/*!
 *  \brief  Constructor for the CBaseModule
 *
 *  \iparam p_MessageConfiguration = Message configuration
 *  \iparam pCANCommunicator = pointer to communication class
 *  \iparam nCANNodeType = Type of the node (hardware type ID)
 *  \iparam nCANNodeIndex = Node index (within nodes with same node type)
 */
/****************************************************************************/
CBaseModule::CBaseModule(const CANMessageConfiguration *p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                         int nCANNodeType, int nCANNodeIndex)
: CModule(CModuleConfig::CAN_OBJ_TYPE_NODE, p_MessageConfiguration, pCANCommunicator),
    m_SubStateInit(CN_SUB_STATE_INIT_UNDEF),
    m_SubStateConfig(CN_SUB_STATE_CONFIG_UNDEF),
    m_SubStateInitFct(CN_SUB_STATE_INIT_FCT_UNDEF),
    m_unCanIDHardwareID(0), m_unCanIDAcknHardwareID(0), m_unCanIDReqConfig(0), m_unCanIDConfig(0),
    m_unCanIDSetNodeState(0), m_unCanIDReqNodeState(0), m_unCanIDNodeState(0),
    m_unCanIDRealTime(0), m_unCanIDSysClock(0),
    m_unCanIDHearbeatCfg(0), m_unCanIDHeartbeatSlave(0), m_unCanIDHeartbeatMaster(0),
    m_unCanIDEmgcyStop(0), m_unCanIDAbortedByEmgcyStop(0), m_unCanIDMasterPower(0), m_unCanIDReset(0), m_unCanIDAcknReset(0),
    m_unCanIDStatistics(0), m_unCanIDCfgStatistics(0),
    m_unCanIDReqMemoryFormat(0), m_unCanIDAcknMemoryFormat(0), m_unCanIDReqSerialNumber(0), m_unCanIDSerialNumber(0),
    m_unCanIDReqEndTestResult(0), m_unCanIDEndTestResult(0),
    m_unCanIDReqHWInfo(0), m_unCanIDHWInfo(0), m_unCanIDReqSWInfo(0), m_unCanIDSWInfo(0),
    m_unCanIDReqBootLoaderInfo(0), m_unCanIDBootLoaderInfo(0),
    m_unCanIDReqLifeCycleData(0), m_unCanIDLifeCycleData(0),
    m_unCanIDReqLaunchDate(0), m_unCanIDLaunchDate(0),
    m_unCanIDReqBoardName(0), m_unCanIDBoardName(0), m_unCanIDReqBoardOptions(0), m_unCanIDBoardOptions(0),
    m_unCanIDCfgVoltageMonitor(0), m_unCanIDReqVoltageState(0), m_unCanIDVoltageState(0),
    m_unCanIDCfgCurrentMonitor(0), m_unCanIDReqCurrentState(0), m_unCanIDCurrentState(0),
    m_unCanIDReqUniqueNumber(0), m_unCanIDUniqueNumber(0),
    m_NodeState(NODE_STATE_UNDEFINED)
{
    mp_MessageConfiguration = p_MessageConfiguration;
    m_pCANCommunicator = pCANCommunicator;

    //initialization of the state machine
    m_mainState = CN_MAIN_STATE_INIT;

    m_NodeClass =  0;
    m_ProtocolVersion = 0;
    m_HWVersion = 0;
    m_SWVersion = 0;
    m_ChannelCount = 0;

    m_bHeartbeatActive = 0;
    m_bHBErrorState = 0;

    //Heartbeat timer initialization
    if(ftime(&m_tbHeartbeatTimeDelay) != 0)
    {
         FILE_LOG_L(laINIT, llERROR) << " ftime failed.";
    }

    if(ftime(&m_tbHardwareIDMsgDelay) == 0)
    {
        m_tbHardwareIDMsgDelay.time--;
    }

    m_bHardwareIDErrorState = 0;

    // some verifications
    if((nCANNodeType > 127) || (nCANNodeIndex > 15))
    {
        m_ulCANNodeID = 0x00000FFF;
    }
    else
    {
        m_ulCANNodeID = nCANNodeType;
        m_ulCANNodeID |= nCANNodeIndex << 8;
    }

    mp_BootLoader = new CBootLoader(mp_MessageConfiguration, m_ulCANNodeID, m_pCANCommunicator, this);

    QString str = QString("0x%1 / 0x%2")
                 .arg(nCANNodeIndex, 1, 16)
                 .arg(nCANNodeType, 2, 16, QChar ('0'));

    // init event string list for hal errors
    m_eventString[1]            = "E_DEVICE_UNCONFIGURED";
    m_eventString[2]            = "E_DEVICE_HANDLE_INVALID";
    m_eventString[3]            = "E_DEVICE_NOT_EXISTS";
    m_eventString[4]            = "E_DEVICE_NOT_OPEN";

    // init event string list for base modul errors
    m_eventString[256 + 1]      = "E_PARAMETER_OUT_OF_RANGE";
    m_eventString[256 + 2]      = "E_MEMORY_FULL";
    m_eventString[256 + 3]      = "E_TIMEOUT";
    m_eventString[256 + 4]      = "E_FUNCTION_UNSUPPORTED";

    m_eventString[256 + 23]     = "E_UNEXPECTED_PARAMETERS";

    m_eventString[256 + 80]     = "E_UNKNOWN_MESSAGE";
    m_eventString[256 + 83]     = "E_MISSING_PARAMETERS";
    m_eventString[256 + 86]     = "E_COMMAND_REJECTED";

    m_eventString[256 + 101]    = "E_MODULE_NOT_ENABLED";

    FILE_LOG_L(laINIT, llDEBUG) << "CANNode index / type " << str.toStdString() << " created.";
}

/****************************************************************************/
/*!
 *  \brief  Destructor of CBaseModule
 */
/****************************************************************************/
CBaseModule::~CBaseModule()
{
    try
    {
        delete mp_BootLoader;
        mp_MessageConfiguration = 0;
        m_pCANCommunicator = 0;
    }
    catch (...)
    {
        // and exit
        return;
    }
}

/****************************************************************************/
/*!
 *  \brief  Adds a function module to the nodes function module list
 *
 *  \iparam pFctModule = Pointer to the function module's base class
 */
/****************************************************************************/
void CBaseModule::AddFunctionModule(CFunctionModule* pFctModule)
{
    if(pFctModule)
    {
        m_FunctionModuleList.insert(m_FunctionModuleList.size(), pFctModule);
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes all function modules from the node's function module list
 */
/****************************************************************************/
void CBaseModule::DeleteAllFunctionModules()
{
    while(!m_FunctionModuleList.isEmpty())
    {
        delete m_FunctionModuleList.takeLast();
    }
}

/****************************************************************************/
/*!
 *  \brief  Initializes the nodes internal data structures (can message IDs, state machine, timer, etc.).\n
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if successfully, otherwise < 0
 */
/****************************************************************************/
ReturnCode_t CBaseModule::Initialize()
{
    ReturnCode_t RetVal;

    RetVal = InitializeCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = RegisterCANMessages();
        if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
        {
            m_SubStateInit = CN_SUB_STATE_INIT_START;
        }
    }

    StartTimeDelay();

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Initialize the module's CAN message IDs
 *
 *      The CAN-IDs are read from the CAN-Message configuration class.
 *      The CAN-ID is composed by the message key, the function
 *      module's channel and the node id of the CANNode this fct-module is assigned to.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::InitializeCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(mp_MessageConfiguration)
    {
        const quint8 ModuleID = MODULE_ID_BASEMODULE;
        RetVal = InitializeEventCANMessages(ModuleID);

        // CAN-IDs for System Messages
        m_unCanIDHardwareID = mp_MessageConfiguration->GetCANMessageID(ModuleID, "HardwareID", 0, m_ulCANNodeID);
        m_unCanIDAcknHardwareID = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AcknHardwareID", 0, m_ulCANNodeID);
        m_unCanIDReqConfig = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqConfiguration", 0, m_ulCANNodeID);
        m_unCanIDConfig = mp_MessageConfiguration->GetCANMessageID(ModuleID, "Configuration", 0, m_ulCANNodeID);

        m_unCanIDSetNodeState = mp_MessageConfiguration->GetCANMessageID(ModuleID, "SetNodeState", 0, m_ulCANNodeID);
        m_unCanIDReqNodeState = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqNodeState", 0, m_ulCANNodeID);
        m_unCanIDNodeState = mp_MessageConfiguration->GetCANMessageID(ModuleID, "NodeState", 0, m_ulCANNodeID);
        m_unCanIDRealTime = mp_MessageConfiguration->GetCANMessageID(ModuleID, "RealTime", 0, m_ulCANNodeID);
        m_unCanIDSysClock = mp_MessageConfiguration->GetCANMessageID(ModuleID, "SysClock", 0, m_ulCANNodeID);
        m_unCanIDHearbeatCfg = mp_MessageConfiguration->GetCANMessageID(ModuleID, "HeartbeatConfig", 0, m_ulCANNodeID);
        m_unCanIDHeartbeatMaster = mp_MessageConfiguration->GetCANMessageID(ModuleID, "HeartbeatMaster", 0, m_ulCANNodeID);
        m_unCanIDHeartbeatSlave = mp_MessageConfiguration->GetCANMessageID(ModuleID, "HeartbeatSlave", 0, m_ulCANNodeID);
        m_unCanIDEmgcyStop = mp_MessageConfiguration->GetCANMessageID(ModuleID, "EmgcyStop", 0, m_ulCANNodeID);;
        m_unCanIDAbortedByEmgcyStop = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AbortedByEmgcyStop", 0, m_ulCANNodeID);;
        m_unCanIDMasterPower = mp_MessageConfiguration->GetCANMessageID(ModuleID, "MasterPower", 0, m_ulCANNodeID);
        m_unCanIDReset = mp_MessageConfiguration->GetCANMessageID(ModuleID, "Reset", 0, m_ulCANNodeID);
        m_unCanIDAcknReset = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AcknReset", 0, m_ulCANNodeID);

        // CAN-IDs for Service/Diagnostics Messages
        m_unCanIDCfgStatistics =    mp_MessageConfiguration->GetCANMessageID(ModuleID, "CfgStatistics", 0, m_ulCANNodeID);
        m_unCanIDStatistics =       mp_MessageConfiguration->GetCANMessageID(ModuleID, "Statistics", 0, m_ulCANNodeID);
        m_unCanIDReqMemoryFormat =  mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqMemoryFormat", 0, m_ulCANNodeID);
        m_unCanIDAcknMemoryFormat = mp_MessageConfiguration->GetCANMessageID(ModuleID, "AcknMemoryFormat", 0, m_ulCANNodeID);
        m_unCanIDReqSerialNumber =  mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqSerialNumber", 0, m_ulCANNodeID);
        m_unCanIDSerialNumber =     mp_MessageConfiguration->GetCANMessageID(ModuleID, "SerialNumber", 0, m_ulCANNodeID);
        m_unCanIDReqEndTestResult = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqEndTestResult", 0, m_ulCANNodeID);
        m_unCanIDEndTestResult =    mp_MessageConfiguration->GetCANMessageID(ModuleID, "EndTestResult", 0, m_ulCANNodeID);
        m_unCanIDReqHWInfo =        mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqHWInfo", 0, m_ulCANNodeID);
        m_unCanIDHWInfo    =        mp_MessageConfiguration->GetCANMessageID(ModuleID, "HWInfo", 0, m_ulCANNodeID);
        m_unCanIDReqSWInfo =        mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqSWInfo", 0, m_ulCANNodeID);
        m_unCanIDSWInfo    =        mp_MessageConfiguration->GetCANMessageID(ModuleID, "SWInfo", 0, m_ulCANNodeID);

        m_unCanIDReqBootLoaderInfo = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqBootLoaderInfo", 0, m_ulCANNodeID);
        m_unCanIDBootLoaderInfo    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "BootLoaderInfo", 0, m_ulCANNodeID);

        m_unCanIDReqLifeCycleData = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqLifeCycleData", 0, m_ulCANNodeID);
        m_unCanIDLifeCycleData    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "LifeCycleData", 0, m_ulCANNodeID);

        m_unCanIDReqLaunchDate = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqLaunchDate", 0, m_ulCANNodeID);
        m_unCanIDLaunchDate    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "LaunchDate", 0, m_ulCANNodeID);

        m_unCanIDReqBoardName    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqBoardName", 0, m_ulCANNodeID);
        m_unCanIDBoardName       = mp_MessageConfiguration->GetCANMessageID(ModuleID, "BoardName", 0, m_ulCANNodeID);
        m_unCanIDReqBoardOptions = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqBoardOptions", 0, m_ulCANNodeID);
        m_unCanIDBoardOptions    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "BoardOptions", 0, m_ulCANNodeID);

        m_unCanIDCfgVoltageMonitor = mp_MessageConfiguration->GetCANMessageID(ModuleID, "CfgVoltageMonitor", 0, m_ulCANNodeID);
        m_unCanIDReqVoltageState   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqVoltageState", 0, m_ulCANNodeID);
        m_unCanIDVoltageState      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "VoltageState", 0, m_ulCANNodeID);
        m_unCanIDCfgCurrentMonitor = mp_MessageConfiguration->GetCANMessageID(ModuleID, "CfgCurrentMonitor", 0, m_ulCANNodeID);
        m_unCanIDReqCurrentState   = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqCurrentState", 0, m_ulCANNodeID);
        m_unCanIDCurrentState      = mp_MessageConfiguration->GetCANMessageID(ModuleID, "CurrentState", 0, m_ulCANNodeID);

        m_unCanIDReqUniqueNumber = mp_MessageConfiguration->GetCANMessageID(ModuleID, "ReqUniqueNumber", 0, m_ulCANNodeID);
        m_unCanIDUniqueNumber    = mp_MessageConfiguration->GetCANMessageID(ModuleID, "UniqueNumber", 0, m_ulCANNodeID);
    }

    FILE_LOG_L(laINIT, llDEBUG) << " CAN-messages for node id:" << std::hex << m_ulCANNodeID;
    FILE_LOG_L(laINIT, llDEBUG) << "  HardwareID         : 0x" << std::hex << m_unCanIDHardwareID;
    FILE_LOG_L(laINIT, llDEBUG) << "  AcknHardwareID     : 0x" << std::hex << m_unCanIDAcknHardwareID;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqConfiguration   : 0x" << std::hex << m_unCanIDReqConfig;
    FILE_LOG_L(laINIT, llDEBUG) << "  Configuration      : 0x" << std::hex << m_unCanIDConfig;
    FILE_LOG_L(laINIT, llDEBUG) << "  SetNodeState       : 0x" << std::hex << m_unCanIDSetNodeState;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqNodeState       : 0x" << std::hex << m_unCanIDReqNodeState;
    FILE_LOG_L(laINIT, llDEBUG) << "  NodeState          : 0x" << std::hex << m_unCanIDNodeState;
    FILE_LOG_L(laINIT, llDEBUG) << "  RealTime           : 0x" << std::hex << m_unCanIDRealTime;
    FILE_LOG_L(laINIT, llDEBUG) << "  SystemClock        : 0x" << std::hex << m_unCanIDSysClock;
    FILE_LOG_L(laINIT, llDEBUG) << "  Heartbeat config   : 0x" << std::hex << m_unCanIDHearbeatCfg;
    FILE_LOG_L(laINIT, llDEBUG) << "  Heartbeat master   : 0x" << std::hex << m_unCanIDHeartbeatMaster;
    FILE_LOG_L(laINIT, llDEBUG) << "  Heartbeat slave    : 0x" << std::hex << m_unCanIDHeartbeatSlave;
    FILE_LOG_L(laINIT, llDEBUG) << "  EmgcyStop          : 0x" << std::hex << m_unCanIDEmgcyStop;
    FILE_LOG_L(laINIT, llDEBUG) << "  AbortedByEmgcyStop : 0x" << std::hex << m_unCanIDAbortedByEmgcyStop;
    FILE_LOG_L(laINIT, llDEBUG) << "  MasterPower        : 0x" << std::hex << m_unCanIDMasterPower;
    FILE_LOG_L(laINIT, llDEBUG) << "  Reset              : 0x" << std::hex << m_unCanIDReset;
    FILE_LOG_L(laINIT, llDEBUG) << "  AcknReset          : 0x" << std::hex << m_unCanIDAcknReset;

    FILE_LOG_L(laINIT, llDEBUG) << "  EventInfo          : 0x" << std::hex << m_unCanIDEventInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "  EventWarning       : 0x" << std::hex << m_unCanIDEventWarning;
    FILE_LOG_L(laINIT, llDEBUG) << "  EventError         : 0x" << std::hex << m_unCanIDEventError;
    FILE_LOG_L(laINIT, llDEBUG) << "  EventFatalError    : 0x" << std::hex << m_unCanIDEventFatalError;

    FILE_LOG_L(laINIT, llDEBUG) << "  CfgStatistics      : 0x" << std::hex << m_unCanIDCfgStatistics;
    FILE_LOG_L(laINIT, llDEBUG) << "  Statistics         : 0x" << std::hex << m_unCanIDStatistics;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqDataReset       : 0x" << std::hex << m_unCanIDReqDataReset;
    FILE_LOG_L(laINIT, llDEBUG) << "  AcknDataReset      : 0x" << std::hex << m_unCanIDAcknDataReset;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqMemoryFormat    : 0x" << std::hex << m_unCanIDReqMemoryFormat;
    FILE_LOG_L(laINIT, llDEBUG) << "  AcknMemoryFormat   : 0x" << std::hex << m_unCanIDAcknMemoryFormat;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqSerialNumber    : 0x" << std::hex << m_unCanIDReqSerialNumber;
    FILE_LOG_L(laINIT, llDEBUG) << "  SerialNumber       : 0x" << std::hex << m_unCanIDSerialNumber;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqEndTestResult   : 0x" << std::hex << m_unCanIDReqEndTestResult;
    FILE_LOG_L(laINIT, llDEBUG) << "  EndTestResult      : 0x" << std::hex << m_unCanIDEndTestResult;
    FILE_LOG_L(laINIT, llDEBUG) << "  HWInfoReq          : 0x" << std::hex << m_unCanIDReqHWInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "  HWInfo             : 0x" << std::hex << m_unCanIDHWInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "  SWInfoReq          : 0x" << std::hex << m_unCanIDReqSWInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "  SWInfo             : 0x" << std::hex << m_unCanIDSWInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqBootLoaderInfo  : 0x" << std::hex << m_unCanIDReqBootLoaderInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "  BootLoaderInfo     : 0x" << std::hex << m_unCanIDBootLoaderInfo;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqLifeCycleData   : 0x" << std::hex << m_unCanIDReqLifeCycleData;
    FILE_LOG_L(laINIT, llDEBUG) << "  LifeCycleData      : 0x" << std::hex << m_unCanIDLifeCycleData;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqLaunchDate      : 0x" << std::hex << m_unCanIDReqLaunchDate;
    FILE_LOG_L(laINIT, llDEBUG) << "  LaunchDate         : 0x" << std::hex << m_unCanIDLaunchDate;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqBoardName       : 0x" << std::hex << m_unCanIDReqBoardName;
    FILE_LOG_L(laINIT, llDEBUG) << "  BoardName          : 0x" << std::hex << m_unCanIDBoardName;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqBoardOptions    : 0x" << std::hex << m_unCanIDReqBoardOptions;
    FILE_LOG_L(laINIT, llDEBUG) << "  BoardOptions       : 0x" << std::hex << m_unCanIDBoardOptions;
    FILE_LOG_L(laINIT, llDEBUG) << "  CfgVoltageMonitor  : 0x" << std::hex << m_unCanIDCfgVoltageMonitor;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqVoltageState    : 0x" << std::hex << m_unCanIDReqVoltageState;
    FILE_LOG_L(laINIT, llDEBUG) << "  VoltageState       : 0x" << std::hex << m_unCanIDVoltageState;
    FILE_LOG_L(laINIT, llDEBUG) << "  CfgCurrentMonitor  : 0x" << std::hex << m_unCanIDCfgCurrentMonitor;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqCurrentState    : 0x" << std::hex << m_unCanIDReqCurrentState;
    FILE_LOG_L(laINIT, llDEBUG) << "  CurrentState       : 0x" << std::hex << m_unCanIDCurrentState;
    FILE_LOG_L(laINIT, llDEBUG) << "  ReqUniqueNumber    : 0x" << std::hex << m_unCanIDReqUniqueNumber;
    FILE_LOG_L(laINIT, llDEBUG) << "  UniqueNumber       : 0x" << std::hex << m_unCanIDUniqueNumber;

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Register the receive CAN-messages to communication layer
 *
 *      Each receiveable CAN-message must be registered to the communication layer.
 *      This enables the communication layer to call the 'HandelCANMessage(..)' function of this
 *      instance after receiption of the message.
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS or error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::RegisterCANMessages()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    RetVal = RegisterEventCANMessages();
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDHardwareID, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDConfig, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDNodeState, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDHeartbeatSlave, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDAbortedByEmgcyStop, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDAcknReset, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDStatistics, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDAcknMemoryFormat, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDSerialNumber, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDEndTestResult, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDHWInfo, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDSWInfo, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDBootLoaderInfo, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDLifeCycleData, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDLaunchDate, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDBoardName, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDBoardOptions, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDVoltageState, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDCurrentState, this);
    }
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        RetVal = m_pCANCommunicator->RegisterCOB(m_unCanIDUniqueNumber, this);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Task handling funktion
 *
 *      This function must be called cyclically to ensure the classes tasks
 *      execution. This is done from the DeviceControl class. The main state
 *      will be handled here, corresponding to its value. The appropriate
 *      state handling function will be called. Additionally, the HandleTasks
 *      function of the assigned interface class will be called.
 */
/****************************************************************************/
void CBaseModule::HandleTasks()
{
    if (mp_BootLoader->Active())
    {
        m_mainState = CN_MAIN_STATE_UPDATE;
    }
    else if (m_mainState == CN_MAIN_STATE_UPDATE)
    {
        m_mainState = CN_MAIN_STATE_INIT;
    }

    if(m_mainState == CN_MAIN_STATE_INIT)
    {
        HandleTaskInitialization(0);
    }
    else if(m_mainState == CN_MAIN_STATE_CONFIG)
    {
        HandleTaskConfiguration(0);
    }
    else if(m_mainState == CN_MAIN_STATE_FCT_CONFIG)
    {
        HandleTaskFctConfiguration();
    }
    else if(m_mainState == CN_MAIN_STATE_IDLE)
    {
        HandleIdleState();
    }
    else if(m_mainState == CN_MAIN_STATE_INACTIVE)
    {
        HandleIdleState();
    }
    else if(m_mainState >= CN_MAIN_STATE_UNDEF)
    {
        FILE_LOG_L(laFCT, llERROR) << "  CANNode: Invalid CANNode main state.";
        m_mainState = CN_MAIN_STATE_ERROR;
    }

    if (m_mainState == CN_MAIN_STATE_IDLE && m_NodeState == NODE_STATE_NORMAL)
    {
        CheckHeartbeat();
    }
}


/****************************************************************************/
/*!
 *  \brief  Calls the HandleTasks - method of all associated function modules
 */
/****************************************************************************/
void CBaseModule::CallHandleTaskFctModules()
{
    CFunctionModule* pFctModule = 0;
    QListIterator<CFunctionModule*> iterFctMod(m_FunctionModuleList);

    iterFctMod.toFront();
    while (iterFctMod.hasNext())
    {
        pFctModule = iterFctMod.next();
        pFctModule->HandleTasks();
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the main state CN_MAIN_TASK_INIT.
 *
 *      While this state is active, the program waits for the CAN message
 *      HardwareID and, if received, acknowledges it. A timeout will be
 *      supervised as well.
 *
 *      If the HardwareID message was received, the function will be called
 *      from HandleCANMsgHardwareID(..). It will be called from HandleTasks,
 *      if CN_MAIN_TASK_INIT is active. In this case, pCANframe is set to 0,
 *      and the function checks the timeout
 *
 *  \iparam pCANframe = the can frame if the HardwareID msg, or 0, if called from HandleTasks
 */
/****************************************************************************/
void CBaseModule::HandleTaskInitialization(can_frame* pCANframe)
{
    if(pCANframe)
    {
        //function was called from HandleCANMsgHardwareID(..)
        if(pCANframe->can_id == m_unCanIDHardwareID)
        {
            // stop the timeout
            StopTimeDelay();

            // parse the CAN-message's data
            m_NodeClass       = pCANframe->data[0];
            m_ProtocolVersion = pCANframe->data[1];
            m_HWVersion      = (((quint16)pCANframe->data[2]) << 8) | pCANframe->data[3];
            m_SWVersion      = (((quint16)pCANframe->data[4]) << 8) | pCANframe->data[5];
            m_ChannelCount    = pCANframe->data[6];

            //check if the number of function modules registered on slave is equal to them from hardware configuration file
            // base module is included in number of channels
            /// \todo this check has to be done later, we dont now the number of channels now on master side
            if((m_FunctionModuleList.count() + 1) != (int) m_ChannelCount)
            {
                quint16 nAddData;

                m_mainState = CN_MAIN_STATE_ERROR;
                m_lastEventTime = Global::AdjustedTime::Instance().GetCurrentDateTime();

                nAddData = (((m_FunctionModuleList.count() << 8) & 0xFF00) | (m_ChannelCount & 0x00FF));
                emit ReportEvent(EVENT_DEVICECONTROL_ERROR_INIT_CHANNEL_COUNT, nAddData, m_lastEventTime);
                FILE_LOG_L(laINIT, llERROR) << "CANNode " << GetName().toStdString() << ": channel count not correct: "
                                            << (m_FunctionModuleList.count() + 1) << " - " << (int) m_ChannelCount;
            }
            else
            {
                //send the CAN message 'HardwareIDAck' and start timeout
                ReturnCode_t RetVal;
                can_frame canmsg;

                canmsg.can_id = m_unCanIDAcknHardwareID;
                canmsg.can_dlc = 0;
                RetVal = m_pCANCommunicator->SendCOB(canmsg);
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    FILE_LOG_L(laINIT, llDEBUG) << "CANNode " << GetName().toStdString() << ": send initialization acknowledge: 0x"
                                                << std::hex << m_unCanIDAcknHardwareID;

                    //after sending the 'HardwareIDAckn'-message, activate heartbeat supervision
                    SetHeartbeatSupervision(true);

                    // start the timer used to delay the configuration request some milliseconds
                    StartTimeDelay();

                    //switch to the next task
                    m_mainState = CN_MAIN_STATE_CONFIG;
                    m_SubStateConfig = CN_SUB_STATE_CONFIG_START;
                    FILE_LOG_L(laINIT, llDEBUG) << "CANNode " << GetName().toStdString() << ": change to MAIN_STATE_CONFIG";
                }
                else
                {
                    m_mainState = CN_MAIN_STATE_ERROR;
                }
            }
        }
    }
    // This part trys to reboot the slave after a certain tiemout during the init phase
    else
    {
        // Function was called from HandeTasks(), checking the timeout
        quint16 Delay = GetTimeDelay();

        if(Delay > CAN_NODE_TIMEOUT_INIT)
        {
            StopTimeDelay();
            FILE_LOG_L(laFCT, llDEBUG) << "  CANNode " << GetName().toStdString() << " " << m_ulCANNodeID <<
                                          ", Restarting the node after " << Delay << " ms";
            if(SendCANMsgReset() != DCL_ERR_FCT_CALL_SUCCESS) {
                m_mainState = CN_MAIN_STATE_ERROR;
            }
            StartTimeDelay();
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the main state CN_MAIN_STATE_CONFIG.
 *
 *      If this state is active, the program sends the configuration request CAN message to the slave
 *      The slave should answer with one or more 'Configuration' CAN messages, each contains information about one
 *      of the slave's function module.
 *      The function module will be confirmed if the corresponding Configuration-CAN message was received.
 *      If all function modules are confirmed, this state will finish.
 *      A timeout supervises the receipt of all function modules 'Configuration' CAN message
 *
 *      It will be called from HandleTasks, if CN_MAIN_STATE_CONFIG is active. In this case, pCANframe is set to 0,
 *      and the function checks the timeout
 *
 *  \iparam pCANframe = CAN message data from Configuration or NodeState CAN message, or 0, if called from HandleTasks
 */
/****************************************************************************/
void CBaseModule::HandleTaskConfiguration(can_frame* pCANframe)
{
    ReturnCode_t RetVal;

    if(pCANframe)
    {
        FILE_LOG_L(laFCT, llDEBUG) << " CANNode state: " << (int) m_mainState << " CAN-ID: 0x" << std::hex << pCANframe->can_id;
    }
    else
    {
        FILE_LOG_L(laFCT, llDEBUG) << " CANNode state: " << (int) m_mainState;
    }

    if(m_mainState != CN_MAIN_STATE_CONFIG)
    {
        FILE_LOG_L(laFCT, llERROR) << " invalid state: " << (int) m_mainState;
    }

    if(m_SubStateConfig == CN_SUB_STATE_CONFIG_START)
    {
        // this is the first sub state, send configuration request
        if(GetTimeDelay() > CANNODE_DELAY_CONFIG_REQUEST)
        {
            //send CAN message 'ConfigurationReq'
            RetVal = SendConfigurationRequest();
            if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
            {
                m_SubStateConfig = CN_SUB_STATE_CONFIG_REQ;
                StartTimeDelay();
            }
        }
    }
    else if((m_SubStateConfig == CN_SUB_STATE_CONFIG_REQ) || (m_SubStateConfig == CN_SUB_STATE_CONFIG_REC))
    {
        // the second sub state, wait until all function modules are confirmed by a received CAN-Message
        // check timeout
        if(pCANframe == 0)
        {
            // function was called from HandleTask(), check timeout
            if(GetTimeDelay() > CAN_NODE_TIMEOUT_CONFIG_RECEIVE)
            {
                // this is the timeout error handling of the configuration phase
                // try to find out which function module was not confirmed
                // throw the error, set the instance's main state to 'CN_MAIN_TASK_ERROR'
                CFunctionModule* pFctModule = 0;
                QListIterator<CFunctionModule*> iterFctMod(m_FunctionModuleList);
                quint8 bUnconfirmedFctModule = false;

                //loop thru the function modules to figure out the not confirmed ones
                iterFctMod.toFront();
                while (iterFctMod.hasNext())
                {
                    pFctModule = iterFctMod.next();
                    if(pFctModule->GetMainState() != CFunctionModule::FM_MAIN_STATE_CONFIRMED)
                    {
                        m_mainState = CN_MAIN_STATE_ERROR;
                        m_SubStateConfig = CN_SUB_STATE_CONFIG_ERROR;
                        bUnconfirmedFctModule = true;
                    }
                    if(bUnconfirmedFctModule == true)
                        break;
                }

                // throw error
                QDateTime errorTimeStamp;
                quint16 nAdditonalData = 0;

                if(pFctModule)
                {
                    nAdditonalData = ((quint16)pFctModule->GetType() << 8) | pFctModule->GetChannelNo();
                }

                errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
                emit ReportEvent(EVENT_DEVICECONTROL_ERROR_TIMEOUT_CONFIG, nAdditonalData, errorTimeStamp);

                if(pFctModule)
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANNode: Timeout node configuration";
                    FILE_LOG_L(laFCT, llERROR) << "   CANNode '" << GetName().toStdString() << "': found unconfirmed fct: " << pFctModule->GetName().toStdString() <<
                            "  State: " << (int)pFctModule->GetMainState();
                    FILE_LOG_L(laINIT, llERROR) << " CANNode " << GetName().toStdString() << ": at least " << pFctModule->GetName().toStdString() << " not confirmed.";
                }
                else
                {
                    FILE_LOG_L(laFCT, llERROR) << " CANNode: Timeout node configuration, no further information available";
                }
            }
        }
        else
        {
            // this function was called from HandleCANMsgConfig(..), the 'Configuration' CAN-message was received
            if (pCANframe->can_id != m_unCanIDConfig)
            {
                m_mainState = CN_MAIN_STATE_ERROR;
            }
            else
            {
                CFunctionModule* pFctModule = 0;
                QListIterator<CFunctionModule*> iterFctMod(m_FunctionModuleList);
                quint16   sModuleIDMsg;
                //quint8    bModuleInstance;
                quint8    bChannelMsg;
                //quint16   sModuleVersion;
                quint8    bFctModuleFound = false;       // indicates that the function module assigned to the 'Configuration CAN-Message was found
                                                         // if not, an error will be thrown
                quint8    bFctModuleUnconfirmed = false; // indicates a unconfirmed function module was found. If not, the state will be finished (-> all confirmed)

                //Read the can message's data
                //  0-1 - ModulID         // function module type
                //  2   - ModulInstanz    // internal number to separate similar function modules
                //  3   - ChannelID       // channel, the channel is used to address the function module via can messages
                //  4-5 - Modulversion    //

                sModuleIDMsg    = ((quint16) pCANframe->data[0]) << 8 | pCANframe->data[1];
                //bModuleInstance = pCANframe->data[2];
                bChannelMsg     = pCANframe->data[3];
                m_moduleSWVersion[sModuleIDMsg] = ((quint16) pCANframe->data[4]) << 8 | pCANframe->data[5];

                FILE_LOG_L(laCONFIG, llDEBUG1) << " --------------------";
                FILE_LOG_L(laCONFIG, llDEBUG1) << " Node: " << GetName().toStdString();
                FILE_LOG_L(laCONFIG, llDEBUG1) << " Try to figure out the module with ID:" << sModuleIDMsg << " , ch:" << std::hex << (int)bChannelMsg;

                //figure out the function module
                if(bChannelMsg == 0)
                {
                    //this is the base module, simply confirm it
                    bFctModuleFound = true;
                    if(m_FunctionModuleList.count() > 0)
                    {
                        bFctModuleUnconfirmed = true;
                    }
                    else
                    {
                        bFctModuleUnconfirmed = false;
                    }
                    m_SubStateConfig = CN_SUB_STATE_CONFIG_REC;
                }
                else
                {
                    iterFctMod.toFront();
                    while (iterFctMod.hasNext())
                    {
                        //loop through the function module list and confirm the one which's data have been received
                        pFctModule = iterFctMod.next();

                        quint8 bChannel = pFctModule->GetChannelNo();

                        FILE_LOG_L(laCONFIG, llDEBUG1) << " test fct: " << pFctModule->GetName().toStdString() << " Ch: " << std::hex << (int)bChannel << " , Type: " << (int) pFctModule->GetType() << " State: " << (int)pFctModule->GetMainState();
                        if((bChannel == bChannelMsg) && (((quint16)pFctModule->GetType()) == sModuleIDMsg))  //check matching module type and interface id
                        {
                            pFctModule->Confirm();
                            FILE_LOG_L(laCONFIG, llDEBUG1) << "  confirm " << pFctModule->GetName().toStdString() << " channel: " << std::hex << (int) bChannel;
                            bFctModuleFound = true;
                            m_SubStateConfig = CN_SUB_STATE_CONFIG_REC;
                        }
                        // detect if any function module left unconfirmed
                        if(pFctModule->GetMainState() != CFunctionModule::FM_MAIN_STATE_CONFIRMED)
                            bFctModuleUnconfirmed = true;
                    }
                }

                if(bFctModuleFound != true)
                {
                    //we received a configuration message with an invalid channel number
                    // this must be treated as a fatal error and is caused by
                    //  -> firmware error (wrong version, incompatible function modules
                    //  -> master configuration file error
                    QDateTime errorTimeStamp;
                    quint16 nAdditonalData;

                    nAdditonalData = ((quint16)sModuleIDMsg << 8) | bChannelMsg;
                    FILE_LOG_L(laCONFIG, llERROR) << "  Error: " << GetName().toStdString() << " fct not found type:" << sModuleIDMsg << " channel: " << (int) bChannelMsg;

                    errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
                    emit ReportEvent(EVENT_DEVICECONTROL_ERROR_CONFIG_INVALID_MODULE, nAdditonalData, errorTimeStamp);

                    m_mainState = CN_MAIN_STATE_ERROR;
                }
                else if(bFctModuleUnconfirmed == false)
                {
                    //all function modules in the list have been confirmed, next set node state to 'normal'
                    RetVal = SendCANMsgSetNodeState(NODE_STATE_NORMAL);
                    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                    {
                        m_SubStateConfig = CN_SUB_STATE_CONFIG_REAL_TIME;
                    }
                    else
                    {
                        QDateTime errorTimeStamp;
                        quint16 AdditonalData;
                        AdditonalData = (quint16) RetVal;
                        errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
                        emit ReportEvent(EVENT_DEVICECONTROL_ERROR_FCT_CALL_FAILED, AdditonalData, errorTimeStamp);

                        m_mainState = CN_MAIN_STATE_ERROR;
                    }
                }
            }
        }
    }
    else if (m_SubStateConfig == CN_SUB_STATE_CONFIG_REAL_TIME)
    {
        if(pCANframe == 0)
        {
            // function was called from HandleTask(), check timeout
            // the timeout checks the reception of the CAN message 'NodeState'
            if(GetTimeDelay() > CAN_NODE_TIMEOUT_CONFIG_RECEIVE)
            {
            }
        }
        else
        {
            if (pCANframe->can_id == m_unCanIDNodeState) {
                RetVal = SendCANMsgRealTime();
                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    RetVal = SendCANMsgHeartbeatConfig();
                }

                if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
                {
                    m_SubStateConfig = CN_SUB_STATE_CONFIG_FCT_CONFIRMED;
                }
                else
                {
                    QDateTime errorTimeStamp;
                    quint16 AdditonalData;
                    AdditonalData = (quint16) RetVal;
                    errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
                    emit ReportEvent(EVENT_DEVICECONTROL_ERROR_FCT_CALL_FAILED, AdditonalData, errorTimeStamp);

                    m_mainState = CN_MAIN_STATE_ERROR;
                }
            }
            else
            {
                m_mainState = CN_MAIN_STATE_ERROR;
            }
        }
    }
    else if (m_SubStateConfig == CN_SUB_STATE_CONFIG_FCT_CONFIRMED)
    {
        if(pCANframe == 0)
        {
            m_SubStateInitFct = CN_SUB_STATE_INIT_FCT_START;
            m_mainState = CN_MAIN_STATE_FCT_CONFIG;
            FILE_LOG_L(laCONFIG, llINFO) << "CANNode " << GetName().toStdString() << ": all fct modules confirmed.";
        }
        else
        {
            QDateTime errorTimeStamp;
            errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
            emit ReportEvent(EVENT_DEVICECONTROL_ERROR_CONFIG_UNEXP_CANMSG, 0, errorTimeStamp);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the main state CN_MAIN_STATE_FCT_CONFIG.
 *
 *      If this state is active, the function modules of the slaves will be configured.
 *      This is done inside the function module corresponding class
 *      This function calls the HandleTasks() function of the node's function modules to let them do the work.
 *      If a function module's configuration has finished, its state is set to FM_MAIN_STATE_IDLE.
 *      If all function modules state is FM_MAIN_STATE_IDLE, this state (CN_MAIN_STATE_FCT_CONFIG) is finished
 *
 *      A timeout supervises the procedure.
 */
/****************************************************************************/
void CBaseModule::HandleTaskFctConfiguration()
{
    CFunctionModule* pFctModule = 0;
    CFunctionModule* pFctModuleUnconfig = 0;
    QListIterator<CFunctionModule*> iterFctMod(m_FunctionModuleList);
    quint8 bNonIdleFctFound = false;

    if(m_SubStateInitFct == CN_SUB_STATE_INIT_FCT_START)
    {
        m_SubStateInitFct = CN_SUB_STATE_INIT_FCT_RUN;
    }

    //loop thru all function modules to check it's main state
    iterFctMod.toFront();
    while (iterFctMod.hasNext())
    {
        pFctModule = iterFctMod.next();
        if(pFctModule->GetMainState() != CFunctionModule::FM_MAIN_STATE_IDLE)
        {
            //store the unconfigured for logging
            pFctModuleUnconfig = pFctModule;

            pFctModule->HandleTasks();

            bNonIdleFctFound = true;
        }
    }

    if(bNonIdleFctFound == false)
    {
        //all function modules are idle
        m_Mutex.lock();
        m_SubStateInitFct = CN_SUB_STATE_INIT_FCT_FINISH;
        m_mainState = CN_MAIN_STATE_IDLE;
        m_TaskID = MODULE_TASKID_FREE;
        m_Mutex.unlock();

        FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString() << ": all fct modules configured";
        FILE_LOG_L(laCONFIG, llDEBUG) << "            change to CN_MAIN_STATE_IDLE and set operation mode";
    }
    else
    {
        //there is at least one unconfigured function module left, check timeout
        if(GetTimeDelay() > CAN_NODE_TIMEOUT_CONFIG_FCT_MODULES)
        {
            QDateTime errorTimeStamp;
            quint16 nAdditonalData = 0;
            FILE_LOG_L(laFCT, llERROR) << " Error: " << GetName().toStdString() << " Timeout node fct configuration";
            FILE_LOG_L(laINIT, llERROR) << " Error: " << GetName().toStdString() << " Timeout node fct configuration";

            //report all function module's state
            iterFctMod.toFront();
            while (iterFctMod.hasNext())
            {
                pFctModule = iterFctMod.next();
                FILE_LOG_L(laINIT, llERROR) << " FctModul " << pFctModule->GetName().toStdString() << " Mainstate: " << (int) pFctModule->GetMainState();
            }

            //report the unconfigured
            if(pFctModuleUnconfig)
            {
                nAdditonalData = ((quint16)pFctModuleUnconfig->GetType() << 8) | pFctModuleUnconfig->GetChannelNo();
                FILE_LOG_L(laFCT, llERROR) << "   CANNode '" << GetName().toStdString() << "': found unconfigured fct: " << pFctModuleUnconfig->GetName().toStdString() <<
                        "  State: " << (int)pFctModule->GetMainState();
                FILE_LOG_L(laCONFIG, llERROR) << " CANNode " << GetName().toStdString() << ": at least " << pFctModuleUnconfig->GetName().toStdString() << "not configured.";
            }

            //throw error
            errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
            emit ReportEvent(EVENT_DEVICECONTROL_ERROR_TIMEOUT_FCT_CONFIG, nAdditonalData, errorTimeStamp);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the main state CN_MAIN_STATE_IDLE.
 *
 *      This state is active if the class is in normal operation mode.
 *      While this state is active, the taskID will be checked for a
 *      tasks to be executed. If taskID is not CN_TASKID_FREE, the
 *      corresponding task handling function will be called.
 *      Furthermore, the task handling function of the assigned function modules
 *      is called.
 */
/****************************************************************************/
void CBaseModule::HandleIdleState()
{
    CallHandleTaskFctModules();

    m_Mutex.lock();
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        HandleCommandRequestTask();
    }
    m_Mutex.unlock();
}

/****************************************************************************/
/*!
 *  \brief  Handles the Task CN_TASKID_COMMAND_HDL.
 *
 *      This task is active whenever a module command is pending for executiuon.
 */
/****************************************************************************/
void CBaseModule::HandleCommandRequestTask()
{
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_NOT_FOUND;

    QMutableListIterator<ModuleCommand_t *> Iterator(m_ModuleCommand);
    while(Iterator.hasNext())
    {
        ModuleCommand_t *p_ModuleCommand = Iterator.next();
        bool RemoveCommand = false;

        if(p_ModuleCommand->State == MODULE_CMD_STATE_REQ)
        {
            //General:
            // Forward the module command request to the slave side by sending
            // the corresponding CAN-message
            p_ModuleCommand->State = MODULE_CMD_STATE_REQ_SEND;
            p_ModuleCommand->ReqSendTime.Trigger();

            FILE_LOG_L(laFCT, llDEBUG) << " CANNode: forward request of command type " << (int) p_ModuleCommand->Type;
            if(p_ModuleCommand->Type == CN_CMD_SET_NODE_STATE)
            {
                RetVal = SendCANMsgSetNodeState(p_ModuleCommand->NodeState);
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    if (p_ModuleCommand->NodeState == NODE_STATE_BOOTING) {
                        RemoveCommand = true;
                        emit ReportNodeState(GetModuleHandle(), RetVal, p_ModuleCommand->NodeState,
                                             RESET_EMERGENCY_STOP, POWER_UNKNOWN);
                    }
                    else {
                        p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_SETINITOPDATA;
                    }
                }
                else {
                    emit ReportNodeState(GetModuleHandle(), RetVal, NODE_STATE_UNDEFINED,
                                         RESET_EMERGENCY_STOP, POWER_UNKNOWN);
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_NODE_STATE)
            {
                RetVal = SendCANMsgReqNodeState();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportNodeState(GetModuleHandle(), RetVal, NODE_STATE_UNDEFINED,
                                         RESET_EMERGENCY_STOP, POWER_UNKNOWN);
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_CONF_STATISTICS)
            {
                RetVal = SendCANMsgConfStatistics();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    //! \todo Signal missing
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_DATA_RESET)
            {
                RetVal = SendCANMsgReqDataReset();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_MEMORY_OPERATION;
                }
                else {
                    emit ReportDataResetAckn(GetModuleHandle(), RetVal);
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_FORMAT_MEM)
            {
                RetVal = SendCANMsgReqFormatMemory(p_ModuleCommand->PartitionTableSize);
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_MEMORY_OPERATION;
                }
                else {
                    emit ReportFormatMemoryAckn(GetModuleHandle(), RetVal);
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_SERIAL_NB)
            {
                RetVal = SendCANMsgReqSerialNumber();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportSerialNumber(GetModuleHandle(), RetVal, QString());
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_END_TEST_RESULT)
            {
                RetVal = SendCANMsgReqEndTestResult();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportEndTestResult(GetModuleHandle(), RetVal, TEST_OPEN, QDate());
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_HW_INFO)
            {
                RetVal = SendCANMsgReqHWInfo();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportHWInfo(GetModuleHandle(), RetVal, 0, 0, QDate());
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_SW_INFO)
            {
                RetVal = SendCANMsgReqSWInfo();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportSWInfo(GetModuleHandle(), RetVal, 0, QDate());
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_LOADER_INFO)
            {
                RetVal = SendCANMsgReqLoaderInfo();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportLoaderInfo(GetModuleHandle(), RetVal, 0, 0, QDate());
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_LIFE_CYCLE_DATA)
            {
                RetVal = SendCANMsgReqLifeCycleData();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportLifeCycleData(GetModuleHandle(), RetVal, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_LAUNCH_DATE)
            {
                RetVal = SendCANMsgReqLaunchData();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportLaunchDate(GetModuleHandle(), RetVal, false, QDate());
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_BOARD_NAME)
            {
                RetVal = SendCANMsgReqBoardName();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportBoardName(GetModuleHandle(), RetVal, QString());
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_BOARD_OPTIONS)
            {
                RetVal = SendCANMsgReqBoardOptions();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportBoardOptions(GetModuleHandle(), RetVal);
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_VOLTAGE_STATE)
            {
                RetVal = SendCANMsgReqVoltageState();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportVoltageState(GetModuleHandle(), RetVal, POWER_UNKNOWN, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_CURRENT_STATE)
            {
                RetVal = SendCANMsgReqCurrentState();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportCurrentState(GetModuleHandle(), RetVal, POWER_UNKNOWN, 0, 0);
                }
            }
            else if(p_ModuleCommand->Type == CN_CMD_REQ_UNIQUE_NUMBER)
            {
                RetVal = SendCANMsgReqUniqueNumber();
                if (RetVal == DCL_ERR_FCT_CALL_SUCCESS) {
                    p_ModuleCommand->Timeout = CAN_NODE_TIMEOUT_DATA_REQ;
                }
                else {
                    emit ReportUniqueNumber(GetModuleHandle(), RetVal, QByteArray());
                }
            }

            // Check for success
            if(RetVal != DCL_ERR_FCT_CALL_SUCCESS)
            {
                RemoveCommand = true;
            }
        }
        else if(p_ModuleCommand->State == MODULE_CMD_STATE_REQ_SEND)
        {
            // check active device commands for timeout
            if(p_ModuleCommand->ReqSendTime.Elapsed() > p_ModuleCommand->Timeout)
            {
                RemoveCommand = true;

                //timeout detected, forward the error information to logging and emit the command signal with error information.
                if(p_ModuleCommand->Type == CN_CMD_SET_NODE_STATE)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': SetNodeState timeout error.";
                    emit ReportNodeState(GetModuleHandle(), DCL_ERR_TIMEOUT, NODE_STATE_UNDEFINED,
                                         RESET_EMERGENCY_STOP, POWER_UNKNOWN);
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_NODE_STATE)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqNodeState timeout error.";
                    emit ReportNodeState(GetModuleHandle(), DCL_ERR_TIMEOUT, NODE_STATE_UNDEFINED,
                                         RESET_EMERGENCY_STOP, POWER_UNKNOWN);
                }
                else if(p_ModuleCommand->Type == CN_CMD_CONF_STATISTICS)
                {
                    //! \todo Signal missing
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_DATA_RESET)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqDataReset timeout error.";
                    emit ReportDataResetAckn(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_FORMAT_MEM)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqFormatMemory timeout error.";
                    emit ReportFormatMemoryAckn(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_SERIAL_NB)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqSerialNumber timeout error.";
                    emit ReportSerialNumber(GetModuleHandle(), DCL_ERR_TIMEOUT, QString());
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_END_TEST_RESULT)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqTestResult timeout error.";
                    emit ReportEndTestResult(GetModuleHandle(), DCL_ERR_TIMEOUT, TEST_OPEN, QDate());
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_HW_INFO)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqHWInfo timeout error.";
                    emit ReportHWInfo(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0, QDate());
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_SW_INFO)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqSWInfo timeout error.";
                    emit ReportSWInfo(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, QDate());
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_LOADER_INFO)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqLoaderInfo timeout error.";
                    emit ReportLoaderInfo(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0, QDate());
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_LIFE_CYCLE_DATA)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqLifeCycleData timeout error.";
                    emit ReportLifeCycleData(GetModuleHandle(), DCL_ERR_TIMEOUT, 0, 0);
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_LAUNCH_DATE)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqLaunchDate timeout error.";
                    emit ReportLaunchDate(GetModuleHandle(), DCL_ERR_TIMEOUT, false, QDate());
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_BOARD_NAME)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqBoardName timeout error.";
                    emit ReportBoardName(GetModuleHandle(), DCL_ERR_TIMEOUT, QString());
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_BOARD_OPTIONS)
                {
                    emit ReportBoardOptions(GetModuleHandle(), DCL_ERR_TIMEOUT);
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_VOLTAGE_STATE)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqVoltageState timeout error.";
                    emit ReportVoltageState(GetModuleHandle(), DCL_ERR_TIMEOUT, POWER_UNKNOWN, 0, 0);
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_CURRENT_STATE)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqCurrentState timeout error.";
                    emit ReportCurrentState(GetModuleHandle(), DCL_ERR_TIMEOUT, POWER_UNKNOWN, 0, 0);
                }
                else if(p_ModuleCommand->Type == CN_CMD_REQ_UNIQUE_NUMBER)
                {
                    FILE_LOG_L(laFCT, llERROR) << "  CANNode '" << GetKey().toStdString()
                                               << "': ReqUniqueNumber timeout error.";
                    emit ReportUniqueNumber(GetModuleHandle(), DCL_ERR_TIMEOUT, QByteArray());
                }
            }
        }

        if (RemoveCommand == true)
        {
            delete p_ModuleCommand;
            Iterator.remove();
        }
    }

    if(m_ModuleCommand.isEmpty())
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}

/****************************************************************************/
/*!
 *  \brief  Returns the boot loader assigned to the base module
 *
 *  \return Boot loader class
 */
/****************************************************************************/
CBootLoader *CBaseModule::GetBootLoader()
{
    return mp_BootLoader;
}

/****************************************************************************/
/**
 *  \brief  Send the CAN message 'ConfigurationReq'
 *
 *  \return configuration request was send (0) or not (1)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendConfigurationRequest()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString() << ": send config req.: 0x" << std::hex << m_unCanIDReqConfig;

    canmsg.can_id = m_unCanIDReqConfig;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);
    if(RetVal == DCL_ERR_FCT_CALL_SUCCESS)
    {
        // switch to next sub state
        m_SubStateConfig  = CN_SUB_STATE_CONFIG_REQ;
        // start the timer used for configuration timeout
        StartTimeDelay();
    }
    else
    {
        // switch sub state to error
        m_SubStateConfig  = CN_SUB_STATE_CONFIG_ERROR;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Set the slave's real time clock
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgRealTime()
{
    ReturnCode_t result;
    can_frame canmsg;

    FILE_LOG_L(laCONFIG, llDEBUG)  << "CANNode " << GetName().toStdString() << ": send 'RealTime'.: 0x" << std::hex << m_unCanIDRealTime;

    // get date and time from system time
    QDateTime CurrentDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
    QDate CurrentDate = CurrentDateTime.date();
    QTime CurrentTime = CurrentDateTime.time();

    //get the date components
    int Year  = CurrentDate.year();
    int Month = CurrentDate.month();
    int Day   = CurrentDate.day();

    //get the time components
    int Hour   = CurrentTime.hour();
    int Minute = CurrentTime.minute();
    int Second = CurrentTime.second();
    int MSec   = CurrentTime.msec();

    //assemble the CAN message
    canmsg.can_id = m_unCanIDRealTime;
    canmsg.data[0] = (quint8) (Year - 2000); //If this code runs after 12-31-2099, we run into trouble
    canmsg.data[1] = (quint8) Month;
    canmsg.data[2] = (quint8) Day;
    canmsg.data[3] = (quint8) Hour;
    canmsg.data[4] = (quint8) Minute;
    canmsg.data[5] = (quint8) Second;
    canmsg.data[6] = (quint8) ((MSec & 0xff00) >> 8);
    canmsg.data[7] = (quint8) (MSec & 0x00ff);
    canmsg.can_dlc = 8;

    //send the CAN-message
    result = m_pCANCommunicator->SendCOB(canmsg);

    return result;
}

/****************************************************************************/
/*!
 *  \brief  Set the slave's heartbeat configuration (slave and master heartbeat)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgHeartbeatConfig()
{
    ReturnCode_t result;
    can_frame canmsg;

    FILE_LOG_L(laCONFIG, llDEBUG)  << "CANNode " << GetName().toStdString() << ": send 'HeartbeatConfig'.: 0x" << std::hex << m_unCanIDHearbeatCfg;

    quint8  HeartbeatMode = 0x80;
    quint16 HBSlaveIntervall = 1000;
    quint16 HBMasterIntervall = 0;

    canmsg.can_id = m_unCanIDHearbeatCfg;
    canmsg.data[0] = HeartbeatMode;
    canmsg.data[1] = (quint8) ((HBSlaveIntervall & 0xff00) >> 8);
    canmsg.data[2] = (quint8) (HBSlaveIntervall & 0x00ff);
    canmsg.data[3] = (quint8) ((HBMasterIntervall & 0xff00) >> 8);
    canmsg.data[4] = (quint8) (HBMasterIntervall & 0x00ff);
    canmsg.data[5] = 0x00;
    canmsg.data[6] = 0x00;
    canmsg.data[7] = 0x00;
    canmsg.can_dlc = 5;
    result = m_pCANCommunicator->SendCOB(canmsg);

    return result;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to set the  node state
 *
 *      This command will be acknowledged by the receiption of the m_unCanIDNodeState CAN-message
 *
 *  \iparam NodeState = Node state the slave shall set to
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgSetNodeState(NodeState_t NodeState)
{
    ReturnCode_t result;
    can_frame canmsg;

    FILE_LOG_L(laCONFIG, llDEBUG)  << "CANNode " << GetName().toStdString() << ": send 'NodeStateSet'.: 0x" << std::hex << m_unCanIDSetNodeState;

    canmsg.can_id = m_unCanIDSetNodeState;
    canmsg.data[0] = (quint8) NodeState;
    canmsg.can_dlc = 1;
    result = m_pCANCommunicator->SendCOB(canmsg);

    return result;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the actual node state
 *
 *      This command will be acknowledged by the receiption of the m_unCanIDNodeState CAN-message
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqNodeState()
{
    ReturnCode_t result;
    can_frame canmsg;

    FILE_LOG_L(laCONFIG, llDEBUG)  << "CANNode " << GetName().toStdString() << ": send 'ReqNodeState'.: 0x" << std::hex << m_unCanIDReqNodeState;

    canmsg.can_id = m_unCanIDReqNodeState;
    canmsg.can_dlc = 0;
    result = m_pCANCommunicator->SendCOB(canmsg);

    return result;
}


/****************************************************************************/
/*!
 *  \brief  Send the CAN message to
 *
 *  \iparam enter = Enter (true) or leave (false) the emergency stop mode
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgEmgcyStop(bool enter)
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDEmgcyStop;
    canmsg.can_id &= 0x1ff80001;        // make it a broadcast msg by setting node type, node index and channel to zero
    canmsg.data[0] = (enter ? 1 : 0);
    canmsg.can_dlc = 1;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReset()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReset;
    canmsg.can_dlc = 1;
    canmsg.data[0] = 0x55;

    if((RetVal = m_pCANCommunicator->SendCOB(canmsg)) == DCL_ERR_FCT_CALL_SUCCESS) {
        canmsg.data[0] = 0xE7;
        RetVal = m_pCANCommunicator->SendCOB(canmsg);
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgConfStatistics()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDCfgStatistics;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request formating the memory
 *
 *  \iparam PartitionTableSize = Number of permanent storage partitions
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqFormatMemory(quint8 PartitionTableSize)
{
    can_frame CanFrame;

    FILE_LOG_L(laCOMM, llDEBUG)  << "CANNode " << GetName().toStdString() << ": send format memory req.: 0x"
                                 << std::hex << m_unCanIDReqMemoryFormat;

    CanFrame.can_id = m_unCanIDReqMemoryFormat;
    CanFrame.can_dlc = 3;
    SetCANMsgDataU16(&CanFrame, ComputePassword(), 0);
    CanFrame.data[2] = PartitionTableSize;

    return m_pCANCommunicator->SendCOB(CanFrame);
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the serial number
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqSerialNumber()
{
    ReturnCode_t result;
    can_frame canmsg;

    FILE_LOG_L(laCOMM, llDEBUG)  << "CANNode " << GetName().toStdString() <<": send hw serial number req.: 0x"
                                 << std::hex << m_unCanIDReqSerialNumber;

    canmsg.can_id = m_unCanIDReqSerialNumber;
    canmsg.can_dlc = 0;
    result = m_pCANCommunicator->SendCOB(canmsg);

    return result;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the end test result
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqEndTestResult()
{
    ReturnCode_t result;
    can_frame canmsg;

    FILE_LOG_L(laCOMM, llDEBUG)  << "CANNode " << GetName().toStdString() << ": send test result req.: 0x"
                                 << std::hex << m_unCanIDReqEndTestResult;

    canmsg.can_id = m_unCanIDReqEndTestResult;
    canmsg.can_dlc = 0;
    result = m_pCANCommunicator->SendCOB(canmsg);

    return result;
}
/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the hw information
 *
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqHWInfo()
{
    ReturnCode_t result;
    can_frame canmsg;

    FILE_LOG_L(laCOMM, llDEBUG)  << "CANNode " << GetName().toStdString() << ": send hw info req.: 0x"
                                 << std::hex << m_unCanIDReqHWInfo;

    canmsg.can_id = m_unCanIDReqHWInfo;
    canmsg.can_dlc = 0;
    result = m_pCANCommunicator->SendCOB(canmsg);

    return result;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the sw information
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqSWInfo()
{
    ReturnCode_t result;
    can_frame canmsg;

    FILE_LOG_L(laCOMM, llDEBUG)  << "CANNode " << GetName().toStdString() << ": send sw info req.: 0x"
                                 << std::hex << m_unCanIDReqSWInfo;

    canmsg.can_id = m_unCanIDReqSWInfo;
    canmsg.can_dlc = 0;
    result = m_pCANCommunicator->SendCOB(canmsg);

    return result;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the sw information
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqLoaderInfo()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReqBootLoaderInfo;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the sw information
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqLifeCycleData()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReqLifeCycleData;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}
/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the sw information
 *
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqLaunchData()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReqLaunchDate;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}
/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the sw information
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqBoardName()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReqBoardName;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}
/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the sw information
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqBoardOptions()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReqBoardOptions;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to request the sw information
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqVoltageState()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReqVoltageState;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqCurrentState()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReqCurrentState;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Send the CAN message to
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the CAN message was successful placed in transmit queue
 *          otherwise the return code from SendCOB(..)
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SendCANMsgReqUniqueNumber()
{
    ReturnCode_t RetVal;
    can_frame canmsg;

    canmsg.can_id = m_unCanIDReqUniqueNumber;
    canmsg.can_dlc = 0;
    RetVal = m_pCANCommunicator->SendCOB(canmsg);

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of a CAN message
 *
 *      The function is called from communication layer if a CAN message, which
 *      was registered by this class instance, was received.
 *      The CAN message's data will be forwarded to the specialized handle function.
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCanMessage(can_frame* pCANframe)
{
    QMutexLocker Locker(&m_Mutex);

    FILE_LOG_L(laCOMM, llDEBUG3) << "   0x" << (quint32) pCANframe->can_id << " received";
    FILE_LOG_L(laCOMM, llDEBUG3) << "         "  << std::hex << (int)pCANframe->data[0] << " "  << std::hex << (int)pCANframe->data[1] <<
                                           " "  << std::hex << (int)pCANframe->data[2] << " "  << std::hex << (int)pCANframe->data[3];

    if(m_ulCANNodeID != ((pCANframe->can_id & 0x00000F00) | ((pCANframe->can_id & 0x000000FE) >> 1)))
    {
        FILE_LOG_L(laFCT, llDEBUG1) << " CANNode " << GetName().toStdString() << ", NodeID: " << m_ulCANNodeID;
    }

    if(7 == ((pCANframe->can_id >> 26) & 0x7))  // check if the can id belongs to command class CMD_CLASS_ASSEMBLY
    {
        mp_BootLoader->HandleCanMessage(pCANframe);
    }
    else if((pCANframe->can_id == m_unCanIDEventInfo) ||
       (pCANframe->can_id == m_unCanIDEventWarning) ||
       (pCANframe->can_id == m_unCanIDEventError) ||
       (pCANframe->can_id == m_unCanIDEventFatalError))
    {
        quint32 EventCode = HandleCANMsgEvent(pCANframe);
        if ((pCANframe->can_id == m_unCanIDEventError) || (pCANframe->can_id == m_unCanIDEventFatalError)) {
            emit ReportEvent(EventCode, m_lastEventData, m_lastEventTime);
        }
    }
    else if(pCANframe->can_id == m_unCanIDHeartbeatSlave)
    {
        HandleCANMsgHeartbeatSlave(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDHardwareID)
    {
        HandleCANMsgHardwareID(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDConfig)
    {
        HandleCANMsgConfig(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDNodeState)
    {
        HandleCANMsgNodeState(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDAbortedByEmgcyStop)
    {
        HandleCANMsgAbortedByEmgcyStop(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDStatistics)
    {
        HandleCANMsgStatistics(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDAcknDataReset)
    {
        ResetModuleCommand(CN_CMD_REQ_DATA_RESET);
        HandleCANMsgAcknDataReset(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDAcknMemoryFormat)
    {
        HandleCANMsgAcknMemoryFormat(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDSerialNumber)
    {
        HandleCANMsgSerialNumber(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDEndTestResult)
    {
        HandleCANMsgEndTestResult(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDHWInfo)
    {
        HandleCANMsgHardwareInformation(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDSWInfo)
    {
        HandleCANMsgSoftwareInformation(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDBootLoaderInfo)
    {
        HandleCANMsgBootLoaderInfo(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDLifeCycleData)
    {
        HandleCANMsgLifeCycleData(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDLaunchDate)
    {
        HandleCANMsgLaunchData(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDBoardName)
    {
        HandleCANMsgBoardName(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDBoardOptions)
    {
        HandleCANMsgBoardOptions(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDVoltageState)
    {
        HandleCANMsgVoltageState(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDCurrentState)
    {
        HandleCANMsgCurrentState(pCANframe);
    }
    else if(pCANframe->can_id == m_unCanIDUniqueNumber)
    {
        HandleCANMsgUniqueNumber(pCANframe);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of Slave heartbeat messages
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgHeartbeatSlave(can_frame* pCANframe)
{
    Q_UNUSED(pCANframe);

    if(ftime(&m_tbHeartbeatTimeDelay) == 0)
    {
        if(m_bHBErrorState == 2)
        {
            m_bHBErrorState = 1;
            m_mainState = CN_MAIN_STATE_IDLE;
        }
    }
}

/******************************************************************/
/**
 *  \brief  The function handles the receipt of the CAN message 'HardwareID'
 *
 *  \iparam pCANframe = Received CAN message
 */
/******************************************************************/
void CBaseModule::HandleCANMsgHardwareID(can_frame* pCANframe)
{
    qint16 nMilliSeconds = 0, nSeconds = 0;
    struct timeb tbHardwareIDRec;

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString() << ": HardwareID received, Channels: " << (int) pCANframe->data[6];

    if(pCANframe->can_dlc == CANNODE_CANMSG_DCL_HARDWAREID)
    {
        // Initialization
        if(ftime(&tbHardwareIDRec) == 0)
        {
            CalcTimeDiff(tbHardwareIDRec, m_tbHardwareIDMsgDelay, nSeconds, nMilliSeconds);
            nMilliSeconds += (1000 * nSeconds);
        }

        if((CANNODE_MIN_HARDWAREID_REC_DELAY > nMilliSeconds) && (m_bHardwareIDErrorState == 0))
        {
            // Hardware ID CAN-message was received within the delay,
            // -> Are there two ore more nodes with same nodeId in the system?
            QDateTime errorTimeStamp;
            quint16 nAdditonalData = 0;

            FILE_LOG_L(laINIT, llERROR) << "Error: " << GetName().toStdString() << " HardwareID received within delay (" << nMilliSeconds << "). 2 or more?";

            errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
            emit ReportEvent(EVENT_DEVICECONTROL_ERROR_HWID_REC_INVALID_STATE, nAdditonalData, errorTimeStamp);

            m_bHardwareIDErrorState = 1;
        }

        // We await this message while CN_MAIN_TASK_INIT is active
        if(m_mainState == CN_MAIN_STATE_INIT)
        {
            HandleTaskInitialization(pCANframe);
        }
        else
        {
            CalcTimeDiff(tbHardwareIDRec, m_tbHardwareIDMsgDelay, nSeconds, nMilliSeconds);
            nMilliSeconds += (1000 * nSeconds);
            if(CANNODE_MIN_HARDWAREID_REC_DELAY > nMilliSeconds)
            {
                // Its a race condition
            }
            else
            {
                // Pfui, da hat er ne HardwareID empfangen, reboot des Slaves?
                QDateTime errorTimeStamp;

                FILE_LOG_L(laINIT, llERROR) << "Error: " << GetName().toStdString() << " HardwareID received. Reboot?" ;

                errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
                emit ReportEvent(EVENT_DEVICECONTROL_ERROR_HWID_REC_INVALID_STATE, 0, errorTimeStamp);

                m_bHardwareIDErrorState = 0;

                // Reset the state machine
                m_mainState = CN_MAIN_STATE_INIT;
                StopTimeDelay();
                StartTimeDelay();
            }
        }

        if(ftime(&m_tbHardwareIDMsgDelay) != 0)
        {
            // Force error
            m_bHardwareIDErrorState = 2;
        }
    }
    else
    {
        QDateTime errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
        emit ReportEvent(EVENT_DEVICECONTROL_ERROR_CANMSG_INVALID_DLC, ((quint16)(pCANframe->can_id >> 13)), errorTimeStamp);
    }
}

/******************************************************************/
/**
 *  \brief  Handles handles the receipt of the CAN message 'Configuration'
 *
 *  \iparam pCANframe = Received CAN message
 */
/******************************************************************/
void CBaseModule::HandleCANMsgConfig(can_frame* pCANframe)
{
    FILE_LOG_L(laINIT, llDEBUG) << "Error: " << GetName().toStdString() << " message 'Configuration' received, main state:" << (int) m_mainState;

    if(pCANframe->can_dlc == CANNODE_CANMSG_DCL_CONFIGURATION)
    {
        //we await this message while CN_MAIN_TASK_CONFIG is active
        if(m_mainState == CN_MAIN_STATE_CONFIG)
        {
            HandleTaskConfiguration(pCANframe);
        }
        else
        {
            /// \todo
            //pfui, da hat er ne Configuration-message empfangen, evtl. wegen reboot des Slaves?
            QDateTime errorTimeStamp;

            errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
            emit ReportEvent(EVENT_DEVICECONTROL_ERROR_HWID_REC_INVALID_STATE, (int) m_mainState, errorTimeStamp);

            m_bHardwareIDErrorState = 2;
        }
    }
    else
    {
        QDateTime errorTimeStamp = Global::AdjustedTime::Instance().GetCurrentDateTime();
        emit ReportEvent(EVENT_DEVICECONTROL_ERROR_CANMSG_INVALID_DLC, ((quint16)(pCANframe->can_id >> 13)), errorTimeStamp);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of CAN message 'NodeState'
 *
 *  \iparam pCANframe = Received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgNodeState(can_frame* pCANframe)
{
    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": NodeState received:" << (int) pCANframe->data[0];

    if(pCANframe->can_dlc == 4)
    {
        // store the node state
        m_NodeState = (NodeState_t) pCANframe->data[0];
        // Emergency stop state (0 - no emergency stop / 1 - emergency stop)
        EmergencyStopReason_t EmergencyStopState = (EmergencyStopReason_t) pCANframe->data[1];
        // Power supply status (0 - OK, 1 - not good, but acceptable, 2 - bad, 9 -unknown)
        PowerState_t PowerSupplyState = (PowerState_t) pCANframe->data[2];

        //we await this message while CN_MAIN_TASK_CONFIG is active
        if(m_mainState == CN_MAIN_STATE_CONFIG)
        {
            HandleTaskConfiguration(pCANframe);
        }
        else
        {
            if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
            {
                ResetModuleCommand(CN_CMD_SET_NODE_STATE);
                ResetModuleCommand(CN_CMD_REQ_NODE_STATE);
            }
            emit ReportNodeState(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, m_NodeState,
                                 EmergencyStopState, PowerSupplyState);
        }
    }
    else
    {
        if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
        {
            ResetModuleCommand(CN_CMD_SET_NODE_STATE);
            ResetModuleCommand(CN_CMD_REQ_NODE_STATE);
        }
        emit ReportNodeState(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, NODE_STATE_UNDEFINED,
                             RESET_EMERGENCY_STOP, POWER_UNKNOWN);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of aborted by emergency stop messages
 *
 *  \iparam pCANframe = Received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgAbortedByEmgcyStop(can_frame* pCANframe)
{
    Q_UNUSED(pCANframe);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of CAN message 'Statistics'
 *
 *  \iparam pCANframe = Received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgStatistics(can_frame* pCANframe)
{
    Q_UNUSED(pCANframe);
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of memory format acknowledgements
 *
 *  \iparam pCANframe = Received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgAcknMemoryFormat(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_FORMAT_MEM);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString() <<
                                     ": Acknowledge format memory received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 1)
    {
        ReturnCode_t HdlInfo = DCL_ERR_FCT_CALL_SUCCESS;

        if(pCANframe->data[0] == 0)
        {
            //the command was not successfully executed
            HdlInfo = DCL_ERR_FCT_CALL_FAILED;
        }

        emit ReportFormatMemoryAckn(GetModuleHandle(), HdlInfo);
    }
    else
    {
        emit ReportFormatMemoryAckn(GetModuleHandle(), DCL_ERR_CANMSG_INVALID);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handle the reception of serial number messages
 *
 *  \iparam pCANframe = Received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgSerialNumber(can_frame* pCANframe)
{
    ReturnCode_t HdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    bool Finished = false;
    qint32 Size = 7;

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Acknowledge serial number received:" << (int) pCANframe->data[0];

    if(pCANframe->can_dlc == 8)
    {
        if (7 * pCANframe->data[0] != m_SerialNumber.size())
        {
            HdlInfo = DCL_ERR_INTERNAL_ERR;
        }

        if (pCANframe->data[7] == 0)
        {
            Finished = true;
            Size = -1;
        }

        m_SerialNumber.append(QString::fromAscii((const char *)&pCANframe->data[1], Size));
    }
    else
    {
        HdlInfo = DCL_ERR_CANMSG_INVALID;
    }

    if (HdlInfo != DCL_ERR_FCT_CALL_SUCCESS || Finished == true)
    {
        if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
        {
            // reset the module command
            ResetModuleCommand(CN_CMD_REQ_SERIAL_NB);
        }
        emit ReportSerialNumber(GetModuleHandle(), HdlInfo, m_SerialNumber);
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'End test result'
 *
 *      This CAN message contains the following information:
 *          - End-test result (OK, FAILED, NOT TESTED)
 *          - Date of end-test (Year, month, day)
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgEndTestResult(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_END_TEST_RESULT);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": End test result received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 4)
    {
        TestResult_t EndtestResult = (TestResult_t) pCANframe->data[0];
        QDate Date(2000 + pCANframe->data[1], pCANframe->data[2], pCANframe->data[3]);

        emit ReportEndTestResult(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, EndtestResult, Date);
    }
    else
    {
        emit ReportEndTestResult(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, TEST_OPEN, QDate());
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Hardware information'
 *
 *      This CAN message contains the following information:
 *          - Hardware version number (major, minor)
 *          - Date of production (Year, month, day)
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgHardwareInformation(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_HW_INFO);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Hardware info received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 5)
    {
        quint8 MajorVersion = pCANframe->data[0];
        quint8 MinorVersion = pCANframe->data[1];
        QDate Date(2000 + pCANframe->data[2], pCANframe->data[3], pCANframe->data[4]);

        emit ReportHWInfo(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, MajorVersion, MinorVersion, Date);
    }
    else
    {
        emit ReportHWInfo(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0, QDate());
    }
}

/****************************************************************************/
/*!
 *  \brief  Handles the reception of the CAN message 'Software information'
 *
 *      This CAN message contains the following information:
 *          - Software version number (major, minor)
 *          - Date of production (Year, month, day)
 *
 *  \iparam pCANframe = struct contains the data of the receipt CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgSoftwareInformation(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_SW_INFO);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Software info received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 5)
    {
        quint16 SWVersion = GetCANMsgDataU16(pCANframe, 0);
        QDate Date(2000 + pCANframe->data[2], pCANframe->data[3], pCANframe->data[4]);

        emit ReportSWInfo(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, SWVersion, Date);
    }
    else
    {
        emit ReportSWInfo(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, QDate());
    }
}

/****************************************************************************/
/**
 *  \brief  Handles the receiption of the can message 'BootLoaderInfo'
 *
 *  \iparam pCANframe = the received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgBootLoaderInfo(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_LOADER_INFO);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Boot loader info received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 5)
    {
        quint8 VersionMajor = pCANframe->data[0];
        quint8 VersionMinor = pCANframe->data[1];
        QDate Date(2000 + pCANframe->data[2], pCANframe->data[3], pCANframe->data[4]);

        emit ReportLoaderInfo(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, VersionMajor, VersionMinor, Date);
    }
    else
    {
        emit ReportLoaderInfo(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0, QDate());
    }
}

/****************************************************************************/
/**
 *  \brief  Handles the receiption of the can message 'LifeCycleData'
 *
 *  \iparam pCANframe = the received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgLifeCycleData(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_LIFE_CYCLE_DATA);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Lifecycle data received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 6)
    {
        quint32 OperationTime = GetCANMsgDataU32(pCANframe, 0);
        quint16 StartupCycles = GetCANMsgDataU16(pCANframe, 4);

        emit ReportLifeCycleData(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, OperationTime, StartupCycles);
    }
    else
    {
        emit ReportLifeCycleData(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, 0, 0);
    }
}

/****************************************************************************/
/**
 *  \brief  Handles the receiption of the can message 'LaunchData'
 *
 *  \iparam pCANframe = the received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgLaunchData(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_LAUNCH_DATE);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Launch data received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 4)
    {
        bool Launched = pCANframe->data[0];
        QDate Date(2000 + pCANframe->data[1], pCANframe->data[2], pCANframe->data[3]);

        emit ReportLaunchDate(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, Launched, Date);
    }
    else
    {
        emit ReportLaunchDate(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, false, QDate());
    }
}

/****************************************************************************/
/**
 *  \brief  Handles the receiption of the can message 'BoardName'
 *
 *  \iparam pCANframe = the received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgBoardName(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_BOARD_NAME);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Board name received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 8)
    {
        QString BoardName = QString::fromAscii((const char *)pCANframe->data);

        emit ReportBoardName(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, BoardName);
    }
    else
    {
        emit ReportBoardName(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, QString());
    }
}

/****************************************************************************/
/**
 *  \brief  Handles the receiption of the can message 'BoardOptions'
 *
 *  \iparam pCANframe = the received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgBoardOptions(can_frame* pCANframe)
{
    Q_UNUSED(pCANframe);
}

/****************************************************************************/
/**
 *  \brief  Handles the receiption of the can message 'VoltageState'
 *
 *  \iparam pCANframe = the received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgVoltageState(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_VOLTAGE_STATE);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Voltage state received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 5)
    {
        PowerState_t State = (PowerState_t)pCANframe->data[0];
        quint16 Value = GetCANMsgDataU16(pCANframe, 1);
        quint16 Failures = GetCANMsgDataU16(pCANframe, 3);

        emit ReportVoltageState(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, State, Value, Failures);
    }
    else
    {
        emit ReportVoltageState(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, POWER_UNKNOWN, 0, 0);
    }
}

/****************************************************************************/
/**
 *  \brief  Handles the receiption of the can message 'CurrentState'
 *
 *  \iparam pCANframe the received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgCurrentState(can_frame* pCANframe)
{
    if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
    {
        // reset the module command
        ResetModuleCommand(CN_CMD_REQ_CURRENT_STATE);
    }

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Current state received:" << (int) pCANframe->data[0];
    if(pCANframe->can_dlc == 5)
    {
        PowerState_t State = (PowerState_t)pCANframe->data[0];
        quint16 Value = GetCANMsgDataU16(pCANframe, 1);
        quint16 Failures = GetCANMsgDataU16(pCANframe, 3);

        emit ReportCurrentState(GetModuleHandle(), DCL_ERR_FCT_CALL_SUCCESS, State, Value, Failures);
    }
    else
    {
        emit ReportCurrentState(GetModuleHandle(), DCL_ERR_CANMSG_INVALID, POWER_UNKNOWN, 0, 0);
    }
}

/****************************************************************************/
/**
 *  \brief  Handles the receiption of the can message 'UniqueNumber'
 *
 *  \iparam pCANframe = the received CAN message
 */
/****************************************************************************/
void CBaseModule::HandleCANMsgUniqueNumber(can_frame* pCANframe)
{
    ReturnCode_t HdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
    bool Finished = false;

    FILE_LOG_L(laCONFIG, llDEBUG) << "CANNode " << GetName().toStdString()
                                  << ": Acknowledge unique number received:" << (int) pCANframe->data[0];

    if(pCANframe->can_dlc > 2)
    {
        qint32 Size = pCANframe->data[0] - 8 * m_UniqueNumber.size();
        Size = (Size > 48) ? 48 : Size;

        if (6 * pCANframe->data[1] != m_UniqueNumber.size())
        {
            HdlInfo = DCL_ERR_INTERNAL_ERR;
        }

        m_UniqueNumber.append((const char *)&pCANframe->data[2], Size / 8);

        if (pCANframe->data[0] == 8 * m_UniqueNumber.size())
        {
            Finished = true;
        }
    }
    else
    {
        HdlInfo = DCL_ERR_CANMSG_INVALID;
    }

    if (HdlInfo != DCL_ERR_FCT_CALL_SUCCESS || Finished == true)
    {
        if(m_TaskID == MODULE_TASKID_COMMAND_HDL)
        {
            // reset the module command
            ResetModuleCommand(CN_CMD_REQ_UNIQUE_NUMBER);
        }
        emit ReportUniqueNumber(GetModuleHandle(), HdlInfo, m_UniqueNumber);
    }
}

/****************************************************************************/
/*!
 *  \brief  Set the node state
 *
 *  \iparam NodeState = node state to set
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::SetNodeState(NodeState_t NodeState)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_SET_NODE_STATE);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->NodeState = NodeState;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief   Request the actual node state
 *
 *  \return  DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqNodeState()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_NODE_STATE);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Force an emergency stop
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::EnterEmcyStop()
{
    return SendCANMsgEmgcyStop(true);
}

/****************************************************************************/
/*!
 *  \brief  Release an emergency stop
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ExitEmcyStop()
{
    return SendCANMsgEmgcyStop(false);
}

/****************************************************************************/
/*!
 *  \brief  Force an node reset
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqReset()
{
    return SendCANMsgReset();
}

/****************************************************************************/
/*!
 *  \brief  Configure the statistics calculation
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ConfigureStatistics()
{
    return DCL_ERR_FCT_NOT_IMPLEMENTED;
}

/****************************************************************************/
/*!
 *  \brief  Request a data reset
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqDataReset()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_DATA_RESET);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the formatting of the memory
 *
 *  \iparam PartitionTableSize = Number of permanent storage partitions
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqFormatMemory(quint8 PartitionTableSize)
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_FORMAT_MEM);
    if(p_ModuleCommand != NULL)
    {
        p_ModuleCommand->PartitionTableSize = PartitionTableSize;
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the serial number
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqSerialNumber()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_SERIAL_NB);
    if(p_ModuleCommand != NULL)
    {
        m_SerialNumber.clear();
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the end test result
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqEndTestResult()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    if(m_NodeState == NODE_STATE_NORMAL)
    {
        ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_END_TEST_RESULT);
        if(p_ModuleCommand == NULL)
        {
            RetVal = DCL_ERR_INVALID_STATE;
            FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
        }
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid node state: " << (int) m_NodeState;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the hardware information
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqHWInfo()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_HW_INFO);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the software information
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqSWInfo()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_SW_INFO);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the boot loader information
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqLoaderInfo()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_LOADER_INFO);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the live cycle data
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqLifeCycleData()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_LIFE_CYCLE_DATA);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the launch date
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqLaunchDate()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_LAUNCH_DATE);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the board name
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqBoardName()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_BOARD_NAME);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the board options
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqBoardOptions()
{
    return DCL_ERR_FCT_NOT_IMPLEMENTED;
}

/****************************************************************************/
/*!
 *  \brief  Configure the voltage monitor
 *
 *  \iparam Enable = Enables or disables monitoring
 *  \iparam Filter = Number of coefficients for the running average filter
 *  \iparam SamplingPeriod = Time between two input samples in milliseconds
 *  \iparam GoodThreshold = The state is good when it is above this limit
 *  \iparam FailThreshold = The state is failed when it is below this limit
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ConfigureVoltageMonitor(bool Enable, quint8 Filter, quint8 SamplingPeriod,
                                                  quint16 GoodThreshold, quint16 FailThreshold)
{
    can_frame CanFrame;

    CanFrame.can_id = m_unCanIDCfgVoltageMonitor;
    CanFrame.can_dlc = 7;

    CanFrame.data[0] = 0;
    if (Enable) {
        CanFrame.data[0] = 0x80;
    }
    CanFrame.data[1] = Filter;
    CanFrame.data[2] = SamplingPeriod;
    SetCANMsgDataU16(&CanFrame, GoodThreshold, 3);
    SetCANMsgDataU16(&CanFrame, FailThreshold, 5);

    return m_pCANCommunicator->SendCOB(CanFrame);
}

/****************************************************************************/
/*!
 *  \brief  Request the state of the voltage monitor
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqVoltageState()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_VOLTAGE_STATE);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Configure the current monitor
 *
 *  \iparam Enable = Enables or disables monitoring
 *  \iparam Filter = Number of coefficients for the running average filter
 *  \iparam SamplingPeriod = Time between two input samples in milliseconds
 *  \iparam GoodThreshold = The state is good when it is below this limit
 *  \iparam FailThreshold = The state is failed when it is above this limit
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ConfigureCurrentMonitor(bool Enable, quint8 Filter, quint8 SamplingPeriod,
                                                  quint16 GoodThreshold, quint16 FailThreshold)
{
    can_frame CanFrame;

    CanFrame.can_id = m_unCanIDCfgCurrentMonitor;
    CanFrame.can_dlc = 7;

    CanFrame.data[0] = 0;
    if (Enable) {
        CanFrame.data[0] = 0x80;
    }
    CanFrame.data[1] = Filter;
    CanFrame.data[2] = SamplingPeriod;
    SetCANMsgDataU16(&CanFrame, GoodThreshold, 3);
    SetCANMsgDataU16(&CanFrame, FailThreshold, 5);

    return m_pCANCommunicator->SendCOB(CanFrame);
}

/****************************************************************************/
/*!
 *  \brief  Request the state of the current monitor
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqCurrentState()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_CURRENT_STATE);
    if(p_ModuleCommand == NULL)
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/*!
 *  \brief  Request the unique board number
 *
 *  \return DCL_ERR_FCT_CALL_SUCCESS if the request can be forwarded, otherwise error code
 */
/****************************************************************************/
ReturnCode_t CBaseModule::ReqUniqueNumber()
{
    QMutexLocker Locker(&m_Mutex);
    ReturnCode_t RetVal = DCL_ERR_FCT_CALL_SUCCESS;

    ModuleCommand_t *p_ModuleCommand = SetModuleTask(CN_CMD_REQ_UNIQUE_NUMBER);
    if(p_ModuleCommand != NULL)
    {
        m_UniqueNumber.clear();
    }
    else
    {
        RetVal = DCL_ERR_INVALID_STATE;
        FILE_LOG_L(laFCT, llERROR) << " CANNode '" << GetKey().toStdString() << "' invalid state: " << (int) m_TaskID;
    }

    return RetVal;
}

/****************************************************************************/
/**
 *  \brief  Set the master heartbeat supervision state
 *
 *  \iparam HBSupervisionState = Enables or disables heartbeat supervision
 */
/****************************************************************************/
void CBaseModule::SetHeartbeatSupervision(bool HBSupervisionState)
{
    FILE_LOG_L(laFCT, llDEBUG1) << GetName().toStdString() << ": activated Heartbeat supervision.";
    if(HBSupervisionState)
    {
        m_bHeartbeatActive = 1;
        if(ftime(&m_tbHeartbeatTimeDelay) == 0)
        {
            m_bHBErrorState = 1;
        }
    }
    else
    {
        m_bHeartbeatActive = 0;
        m_bHBErrorState = 0;
    }
}

/****************************************************************************/
/*!
 *  \brief  Heartbeat handing
 *
 *      Checks the cyclically reception of the slave's heartbeat messages. An
 *      error will be thrown if a heartbeat failure was detected.
 */
/****************************************************************************/
void CBaseModule::CheckHeartbeat()
{
    struct timeb tpNow;
    qint16 nMilliSeconds = 999, nSeconds = 9; //force error if ftime failes

    if(m_bHeartbeatActive)
    {
        if(ftime(&tpNow) == 0)
        {
            CalcTimeDiff(tpNow, m_tbHeartbeatTimeDelay, nSeconds, nMilliSeconds);

            nSeconds = tpNow.time - m_tbHeartbeatTimeDelay.time;
            if(tpNow.millitm < m_tbHeartbeatTimeDelay.millitm)
            {
                nMilliSeconds = tpNow.millitm + (1000-m_tbHeartbeatTimeDelay.millitm);
                nSeconds--;
            }
            else
            {
                nMilliSeconds = tpNow.millitm - m_tbHeartbeatTimeDelay.millitm;
            }

            nMilliSeconds = ((int)(nSeconds*1000)) + nMilliSeconds;
        }

        if(nMilliSeconds > CAN_NODE_TIMEOUT_HEARTBEAT_FAILURE && (m_bHBErrorState == 1))
        {
            FILE_LOG_L(laINIT, llERROR) << "Error: " << GetName().toStdString() << " Heartbeat failed." ;

            emit ReportEvent(EVENT_DEVICECONTROL_ERROR_HEARTBEAT_TIMEOUT, 0,
                             Global::AdjustedTime::Instance().GetCurrentDateTime());

            m_bHBErrorState = 2;
            m_mainState = CN_MAIN_STATE_INACTIVE;
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Calculate the time span between two timeb's
 *
 *  \iparam tpTimeEnd = End time
 *  \iparam tbTimeStart = Start time
 *  \iparam nSeconds      = Time difference, second
 *  \iparam nMilliSeconds = Time difference, ms part
 */
/****************************************************************************/
void CBaseModule::CalcTimeDiff(struct timeb tpTimeEnd, struct timeb tbTimeStart, qint16& nSeconds, qint16& nMilliSeconds)
{

    nSeconds = tpTimeEnd.time - tbTimeStart.time;

    if(tpTimeEnd.millitm < tbTimeStart.millitm)
    {
        nMilliSeconds = tpTimeEnd.millitm + (1000-tbTimeStart.millitm);
        nSeconds--;
    }
    else
        nMilliSeconds = tpTimeEnd.millitm - tbTimeStart.millitm;

    nMilliSeconds = ((int)(nSeconds*1000)) + nMilliSeconds;
}

/****************************************************************************/
/*!
 *  \brief  Return the function module matches the key
 *
 *  \iparam FctModulKey = Function module key string
 *
 *  \return Pointer to function module which maches the key
 */
/****************************************************************************/
CFunctionModule* CBaseModule::GetFunctionModuleFromKey(QString FctModulKey)
{
    CFunctionModule* pFctModule = 0;
    CFunctionModule* pFctModuleRet = 0;
    QListIterator<CFunctionModule*> iterFct(m_FunctionModuleList);

    while (iterFct.hasNext())
    {
        pFctModule = iterFct.next();
        if(pFctModule->GetKey() == FctModulKey)
        {
            pFctModuleRet = pFctModule;
            break;
        }
    }
    return pFctModuleRet;
}

/****************************************************************************/
/*!
 *  \brief  Return the function module matches the channel
 *
 *  \iparam Channel = Function module channel number
 *
 *  \return Pointer to function module which maches the channel
 */
/****************************************************************************/
CFunctionModule* CBaseModule::GetFunctionModuleFromChannel(quint8 Channel)
{
    CFunctionModule* pFctModule = 0;
    CFunctionModule* pFctModuleRet = 0;
    QListIterator<CFunctionModule*> iterFct(m_FunctionModuleList);
    while (iterFct.hasNext())
    {
        pFctModule = iterFct.next();
        if(pFctModule->GetChannelNo() == Channel)
        {
            pFctModuleRet = pFctModule;
            break;
        }
    }
    return pFctModuleRet;
}

/****************************************************************************/
/*!
 *  \brief  Return the function module from linked list
 *
 *  \iparam xpFctModule = Previous functionmodule in list, or 0
 *
 *  \return Pointer to function module which is next to xpFctModule, or the first one
 */
/****************************************************************************/
CFunctionModule* CBaseModule::GetFunctionModuleFromList(CFunctionModule* xpFctModule)
{
    /// \todo check if the list is sorted!!
    bool fReturnNext = false;
    CFunctionModule* pFctModule = 0;
    CFunctionModule* pFctModuleRet = 0;
    QListIterator<CFunctionModule*> iterFct(m_FunctionModuleList);

    while (iterFct.hasNext())
    {
        pFctModule = iterFct.next();
        if(xpFctModule == 0)
        {
            // return the first from list
            pFctModuleRet = pFctModule;
            break;
        }
        if(fReturnNext == true)
        {
            pFctModuleRet = pFctModule;
            break;
        }

        if(pFctModule == xpFctModule)
        {
            // this is the given one, return the next one
            fReturnNext = true;
        }
    }

    return pFctModuleRet;

}

/****************************************************************************/
/*!
 *  \brief  Adds a new command to the transmit queue
 *
 *  \iparam CommandType = Command type to set
 *
 *  \return Module command, if the command type can be placed, otherwise NULL
 */
/****************************************************************************/
CBaseModule::ModuleCommand_t *CBaseModule::SetModuleTask(CANNodeModuleCmdType_t CommandType)
{
    if((m_TaskID == MODULE_TASKID_FREE) || (m_TaskID == MODULE_TASKID_COMMAND_HDL)) {
        for(qint32 i = 0; i < m_ModuleCommand.size(); i++) {
            if (m_ModuleCommand[i]->Type == CommandType) {
                return NULL;
            }
        }

        ModuleCommand_t *p_ModuleCommand = new ModuleCommand_t;
        p_ModuleCommand->Type = CommandType;
        p_ModuleCommand->State = MODULE_CMD_STATE_REQ;
        m_ModuleCommand.append(p_ModuleCommand);

        m_TaskID = MODULE_TASKID_COMMAND_HDL;

        return p_ModuleCommand;
    }

    return NULL;
}

/****************************************************************************/
/*!
 *  \brief  Removes an existing command from the transmit queue
 *
 *  \iparam CommandType = Command of that type will be set to free
 */
/****************************************************************************/
void CBaseModule::ResetModuleCommand(CANNodeModuleCmdType_t CommandType)
{
    for(qint32 i = 0; i < m_ModuleCommand.size(); i++) {
        if (m_ModuleCommand[i]->Type == CommandType && m_ModuleCommand[i]->State == MODULE_CMD_STATE_REQ_SEND) {
            delete m_ModuleCommand.takeAt(i);
            break;
        }
    }

    if(m_ModuleCommand.isEmpty())
    {
        m_TaskID = MODULE_TASKID_FREE;
    }
}

QTextStream& operator<< (QTextStream& s, const NodeState_t &NodeState)
{
    s << "node state (" << int(NodeState) << ") = ";
    switch (NodeState)
    {
    case NODE_STATE_UNDEFINED:
        s << "Undefined state";
        break;
    case NODE_STATE_BOOTING:
        s << "Node is in boot loader";
        break;
    case NODE_STATE_STARTUP:
        s << "Firmware initialization";
        break;
    case NODE_STATE_IDENTIFY:
        s << "Node identification";
        break;
    case NODE_STATE_CONFIGURE:
        s << "Node configuration";
        break;
    case NODE_STATE_NORMAL:
        s << "Normal operation mode";
        break;
    case NODE_STATE_ASSEMBLY:
        s << "Assembly mode";
        break;
    case NODE_STATE_SHUTDOWN:
        s << "Shutdown (going to standby)";
        break;
    case NODE_STATE_STANDBY:
        s << "Standby";
        break;
    default:
        s << "unknown";
        break;
    }
    return s;
}

} //namespace

/****************************************************************************/
/*! \file BaseModule.h
 *
 *  \brief  Definition file for class CBaseModule.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CBaseModule
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

#ifndef DEVICECONTROL_BASEMODULEPRIVATE_H
#define DEVICECONTROL_BASEMODULEPRIVATE_H

#include <QList>
#include <QTextStream>

#include "Global/Include/MonotonicTime.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/SlaveModules/BootLoader.h"
#include "DeviceControl/Include/SlaveModules/FunctionModule.h"
#include "DeviceControl/Include/SlaveModules/Module.h"

namespace DeviceControl
{

class CBaseModule;
class CANCommunicator;
class CANMessageConfiguration;

//! List of all function modules assigned to the node
typedef QList<CFunctionModule*> FctModuleList;

/****************************************************************************/
/*!
 *  \brief  This class represents the firmware basis module.
 *
 *      The class handles the can communication to a slave's basis module.
 *      This class lets appear all the basis module's functionality
 *      The class implements methods to initialize, configure and control a basis module
 *      Additionally the error handling is implemented, the class catches all the nodes error messages
 */
/****************************************************************************/
class CBaseModule : public CModule
{
    Q_OBJECT

public:
    CBaseModule(const CANMessageConfiguration* p_MessageConfiguration, CANCommunicator* pCANCommunicator,
                int nCANNodeType, int nCANNodeIndex);
    ~CBaseModule();

    ReturnCode_t Initialize();  //!< Initialisation

    void HandleTasks(); //!< Task handling function
    void HandleCanMessage(can_frame* pCANframe);    //!< CAN message handing function

    CBootLoader *GetBootLoader();
    void BootLoaderUpdate(CBootLoader::State_t State);

    //! Adds a function module to the internal module list
    void AddFunctionModule(CFunctionModule* pFctModule);

    //! Deletes all function modules from the internal module list
    void DeleteAllFunctionModules();

    //! Main state type definition
    typedef enum {
        CN_MAIN_STATE_INIT       = 0x00, /*!< initialization, wait for CAN msg HardwareID and acknowledge it*/
        CN_MAIN_STATE_CONFIG     = 0x01, /*!< configuration, request node's configuration msgs */
        CN_MAIN_STATE_FCT_CONFIG = 0x02, /*!< function module configuration, send each function modules data */
        CN_MAIN_STATE_IDLE       = 0x03, /*!< idle, nothing special to do */
        CN_MAIN_STATE_INACTIVE   = 0x04, /*!< Inactive */
        CN_MAIN_STATE_ERROR      = 0x05, /*!< error state */
        CN_MAIN_STATE_UPDATE     = 0x06, /*!< wait for firmware update */
        CN_MAIN_STATE_UNDEF      = 0x07  /*!< undefined */
    } CANNodeMainState_t;

    /****************************************************************************/
    /*!
     *  \brief  Return the actual MainState
     *
     *  \return Actual main state
     */
    /****************************************************************************/
    CANNodeMainState_t GetMainState() const { return m_MainState; }

    /****************************************************************************/
    /*!
     *  \brief  Sets the module into a passive state
     *
     *      This method sets a node into a passive state. This way the node can
     *      CBaseModule representing this node can be deactivated, if it
     *      was not found on the CAN bus. The inactive state is left as soon as
     *      matching hardware ID messages a received from the CAN bus.
     */
    /****************************************************************************/
    void SetStateInactive() {  m_MainState = CN_MAIN_STATE_INACTIVE; }

    /****************************************************************************/
    /*!
     *  \brief  Return the Node-ID
     *
     *  \return Node identifier
     */
    /****************************************************************************/
    quint32 GetNodeID() const { return m_ulCANNodeID; }

    /****************************************************************************/
    /*!
     *  \brief  Return the node state
     *
     *  \return Recently received node state
     */
    /****************************************************************************/
    NodeState_t GetNodeState() { return m_NodeState; }

    //! Returns the function module identified by it's key
    CFunctionModule* GetFunctionModuleFromKey(QString fctModulKey);
    //! Returns the function module identified by it's channel
    CFunctionModule* GetFunctionModuleFromChannel(quint8 channel);
    //! Returns the function module identified by it's instance pointer (start with 0)
    CFunctionModule* GetFunctionModuleFromList(CFunctionModule* pFctModule);

    /****************************************************************************/
    /*!
     *  \brief  Returns the CAN protocol version (taken from HardwareID-CAN-message)
     *
     *  \return CAN protocol version
     */
    /****************************************************************************/
    quint16 GetCANVersion() const { return m_ProtocolVersion; }

    /****************************************************************************/
    /*!
     *  \brief  Returns the hardware version (taken from HardwareID-CAN-message)
     *
     *  \return Hardware version
     */
    /****************************************************************************/
    quint16 GetHWVersion() const { return m_HWVersion; }

    /****************************************************************************/
    /*!
     *  \brief  Returns the software version (taken from SoftwareID-CAN-message)
     *
     *  \return Software version
     */
    /****************************************************************************/
    quint16 GetSWVersion() const { return m_SWVersion; }

    quint16 GetModuleSWVersion(CModuleConfig::CANObjectType_t ModuleID) const;

    ReturnCode_t SetNodeState(NodeState_t); //!< Set node state
    ReturnCode_t ReqNodeState();            //!< Request actual node state

    ReturnCode_t EnterEmcyStop();           //!< enter emergency stop state
    ReturnCode_t ExitEmcyStop();            //!< exit emergency stop state
    ReturnCode_t ReqReset();                //!< Request the node's reset

    ReturnCode_t ConfigureStatistics();     //!< Configure statistics
    ReturnCode_t ReqDataReset();            //!< Request data reset
    ReturnCode_t ReqFormatMemory();         //!< Request formatting memory
    ReturnCode_t ReqSerialNumber();         //!< Request serial number
    ReturnCode_t ReqEndTestResult();        //!< Request end test result
    ReturnCode_t ReqHWInfo();               //!< Request hardware information
    ReturnCode_t ReqSWInfo();               //!< Request software information
    ReturnCode_t ReqLoaderInfo();           //!< Request bootloader information
    ReturnCode_t ReqLifeCycleData();        //!< Request live cylce data
    ReturnCode_t ReqLaunchDate();           //!< Request launch date
    ReturnCode_t ReqBoardName();            //!< Request board name
    ReturnCode_t ReqBoardOptions();         //!< Request board options
    ReturnCode_t ConfigureVoltageMonitor(bool Enable, quint8 Filter, quint8 SamplingPeriod,
                                         quint16 GoodThreshold, quint16 FailThreshold);
    ReturnCode_t ReqVoltageState();         //!< Request voltage monitor status
    ReturnCode_t ConfigureCurrentMonitor(bool Enable, quint8 Filter, quint8 SamplingPeriod,
                                         quint16 GoodThreshold, quint16 FailThreshold);
    ReturnCode_t ReqCurrentState();         //!< Request current monitor status
    ReturnCode_t ReqUniqueNumber();         //!< Request unique number
    ReturnCode_t ReqModuleSerialNumber();   //!< Request module serial number
    ReturnCode_t SetModuleSerialNumber(quint64 ModuleSerialNumber) const;

    static QMap<quint32, std::string> m_EventString;    //!< list with info strings for CAN events

signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted when the node state has changed
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam NodeState  = Node state
     *  \iparam EmergencyStopReason = No stop, heartbeat, master
     *  \iparam VoltageState = Good, warning, failed, unknown
     */
    /****************************************************************************/
    void ReportNodeState(quint32 InstanceID, ReturnCode_t HdlInfo, NodeState_t NodeState,
                         EmergencyStopReason_t EmergencyStopReason, PowerState_t VoltageState);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted to report the node's reset
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportReset(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  Report the format memory acknowledge
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportFormatMemoryAckn(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  Report the serial number
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam serialNb = Serial number string
     */
    /****************************************************************************/
    void ReportSerialNumber(quint32 InstanceID, ReturnCode_t HdlInfo, QString serialNb);

    /****************************************************************************/
    /*!
     *  \brief  Report the end test result
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Result = Open, passed, failed
     *  \iparam Date = End test date
     */
    /****************************************************************************/
    void ReportEndTestResult(quint32 InstanceID, ReturnCode_t HdlInfo, TestResult_t Result, QDate Date);

    /****************************************************************************/
    /*!
     *  \brief  Report the hardware information
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam VersionMajor = Major version number
     *  \iparam VersionMinor = Minor version number
     *  \iparam Date = Production date
     */
    /****************************************************************************/
    void ReportHWInfo(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 VersionMajor, quint8 VersionMinor, QDate Date);

    /****************************************************************************/
    /*!
     *  \brief  Report the software information
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Version = Software version number
     *  \iparam Date = Software release date
     */
    /****************************************************************************/
    void ReportSWInfo(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 Version, QDate Date);

    /****************************************************************************/
    /*!
     *  \brief  Report the bootloader information
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam VersionMajor = Boot loader major version number
     *  \iparam VersionMinor = Boot loader minor version number
     *  \iparam Date = Boot loader release date
     */
    /****************************************************************************/
    void ReportLoaderInfo(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 VersionMajor, quint8 VersionMinor, QDate Date);

    /****************************************************************************/
    /*!
     *  \brief  Report life cycle data
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam OperationTime = Total microcontroller uptime in minutes
     *  \iparam StartupCycles = Number of times the microcontroller was started
     */
    /****************************************************************************/
    void ReportLifeCycleData(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 OperationTime, quint16 StartupCycles);

    /****************************************************************************/
    /*!
     *  \brief  Report the first launch date
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam Launched = Launched or not launched
     *  \iparam Date = Launch date
     */
    /****************************************************************************/
    void ReportLaunchDate(quint32 InstanceID, ReturnCode_t HdlInfo, bool Launched, QDate Date);

    /****************************************************************************/
    /*!
     *  \brief  Report the board name
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam BoardName = Board name string
     */
    /****************************************************************************/
    void ReportBoardName(quint32 InstanceID, ReturnCode_t HdlInfo, QString BoardName);

    /****************************************************************************/
    /*!
     *  \brief  Report the board options
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     */
    /****************************************************************************/
    void ReportBoardOptions(quint32 InstanceID, ReturnCode_t HdlInfo);

    /****************************************************************************/
    /*!
     *  \brief  Report the voltage state
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam State = Good, warning, failed, unknown
     *  \iparam Value = Voltage measured in millivolts
     *  \iparam Failures = Number of voltage failures
     */
    /****************************************************************************/
    void ReportVoltageState(quint32 InstanceID, ReturnCode_t HdlInfo, PowerState_t State, quint16 Value, quint16 Failures);

    /****************************************************************************/
    /*!
     *  \brief  Report the current state
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam State = Good, warning, failed, unknow
     *  \iparam Value = Current measured in milliamperes
     *  \iparam Failures = Number of current failures
     */
    /****************************************************************************/
    void ReportCurrentState(quint32 InstanceID, ReturnCode_t HdlInfo, PowerState_t State, quint16 Value, quint16 Failures);

    /****************************************************************************/
    /*!
     *  \brief  Report the unique number
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam UniqueNumber = Byte array of the unique number
     */
    /****************************************************************************/
    void ReportUniqueNumber(quint32 InstanceID, ReturnCode_t HdlInfo, QByteArray UniqueNumber);

    /****************************************************************************/
    /*!
     *  \brief  Report the module serial number
     *
     *  \iparam InstanceID = Instance identifier of this function module instance
     *  \iparam HdlInfo = DCL_ERR_FCT_CALL_SUCCESS, otherwise the error code
     *  \iparam ModuleSerialNumber = Module serial number
     */
    /****************************************************************************/
    void ReportModuleSerialNumber(quint32 InstanceID, ReturnCode_t HdlInfo, quint64 ModuleSerialNumber);

private:
    CBaseModule();   ///< Not implemented.
    CBaseModule(const CBaseModule &); ///< Not implemented.
    const CBaseModule & operator = (const CBaseModule &); ///< Not implemented.

    ReturnCode_t InitializeCANMessages();   //!< CAN message ID initialization
    ReturnCode_t RegisterCANMessages();     //!< Registers the can messages to communication layer

    //void Prepare4Configuration();
    ReturnCode_t SendConfigurationRequest();    //!< Sends the CAN message 'ConfigurationRequest'

    void SetHeartbeatSupervision(bool bHBSupervisionState); //!< (De-)activates the supervision of the slave's heartbeat
    void CheckHeartbeat();                      //!< Supervision of the slave's heartbeat

    void CallHandleTaskFctModules();            //!< call all function modules task function

    void HandleTaskInitialization(can_frame* pCANframe);    //!< Task function while m_eMainState == CN_MAIN_TASK_INIT
    void HandleTaskConfiguration(can_frame* pCANframe);     //!< Task function while m_eMainState == CN_MAIN_TASK_CONFIG
    void HandleTaskFctConfiguration();          //!< Task function while m_eMainState == CN_MAIN_TASK_FCT_CONFIG
    void HandleTaskFctSetOperatingMode();       //!< Task function while m_eMainState == CN_MAIN_TASK_CHANGE_OPMODE

    void HandleIdleState();                     //!< Idle task handling function
    void HandleCommandRequestTask();            //!< Command handling function

    // sending CAN messages
    ReturnCode_t SendCANMsgRealTime();                  //!< sends the can message 'RealTime'
    ReturnCode_t SendCANMsgHeartbeatConfig();           //!< sends the can message 'HeartbeatConfig'

    ReturnCode_t SendCANMsgSetNodeState(NodeState_t);   //!< sends the can message 'SetNodeState'
    ReturnCode_t SendCANMsgReqNodeState();              //!< sends the can message 'ReqNodeState'
    ReturnCode_t SendCANMsgEmgcyStop(bool enter);       //!< sends the can message 'EmergencyStop'
    ReturnCode_t SendCANMsgReset();                     //!< sends the can message 'NodeReset'
    ReturnCode_t SendCANMsgConfStatistics();            //!< sends the can message 'CfgStatistics'
    ReturnCode_t SendCANMsgReqFormatMemory();           //!< sends the can message 'm_unCanIDReqMemoryFormat'
    ReturnCode_t SendCANMsgReqSerialNumber();           //!< sends the can message 'm_unCanIDReqSerialNumber'
    ReturnCode_t SendCANMsgReqEndTestResult();          //!< sends the can message 'm_unCanIDReqEndTestResult'
    ReturnCode_t SendCANMsgReqHWInfo();                 //!< sends the can message 'm_unCanIDReqHWInfo'
    ReturnCode_t SendCANMsgReqSWInfo();                 //!< sends the can message 'm_unCanIDReqSWInfo'
    ReturnCode_t SendCANMsgReqLoaderInfo();             //!< sends the can message 'm_unCanIDReqBootLoaderInfo'
    ReturnCode_t SendCANMsgReqLifeCycleData();          //!< sends the can message 'ReqLifeCycleData'
    ReturnCode_t SendCANMsgReqLaunchData();             //!< sends the can message 'RequestLaunchData'
    ReturnCode_t SendCANMsgReqBoardName();              //!< sends the can message 'RequestBoardName'
    ReturnCode_t SendCANMsgReqBoardOptions();           //!< sends the can message 'RequestBoardOptions'
    ReturnCode_t SendCANMsgConfVoltageMon();            //!< sends the can message 'ConfigVoltageMonitor'
    ReturnCode_t SendCANMsgReqVoltageState();           //!< sends the can message 'RequestVoltageState'
    ReturnCode_t SendCANMsgConfCurrentMon();            //!< sends the can message 'ConfigCurrentMonitor'
    ReturnCode_t SendCANMsgReqCurrentState();           //!< sends the can message 'CurrentState'
    ReturnCode_t SendCANMsgReqUniqueNumber();           //!< sends the can message 'RequestUniqueNumber'
    ReturnCode_t SendCANMsgReqModuleSerialNumber();     //!< sends the can message 'RequestModuleSerialNumber'
    ReturnCode_t SendCANMsgSetModuleSerialNumber();     //!< sends the can message 'SetModuleSerialNumber'

    // evaluation of received CAN messages
    void HandleCANMsgHeartbeatSlave(can_frame* pCANframe);          //!< handles the receipt of can message 'HeartbeatSlave'
    void HandleCANMsgHardwareID(can_frame* pCANframe);              //!< handles the receipt of can message 'HardwareID'
    void HandleCANMsgConfig(can_frame* pCANframe);                  //!< handles the receipt of can message 'Configuration'
    void HandleCANMsgNodeState(can_frame* pCANframe);               //!< handles the receipt of can message 'NodeState'
    void HandleCANMsgAbortedByEmgcyStop(can_frame* pCANframe);      //!< handles the receipt of can message 'AbortByEmgcyStop'
    void HandleCANMsgStatistics(can_frame* pCANframe);              //!< handles the receipt of can message 'Statistics'
    void HandleCANMsgAcknMemoryFormat(can_frame* pCANframe);        //!< handles the receipt of can message 'AcknMemoryFormat'
    void HandleCANMsgSerialNumber(can_frame* pCANframe);            //!< handles the receipt of can message 'SerialNumber'
    void HandleCANMsgEndTestResult(can_frame* pCANframe);           //!< handles the receipt of can message 'EndTestResult'
    void HandleCANMsgHardwareInformation(can_frame* pCANframe);     //!< handles the receipt of can message 'HardwareInformation'
    void HandleCANMsgSoftwareInformation(can_frame* pCANframe);     //!< handles the receipt of can message 'SoftwareInformation'
    void HandleCANMsgBootLoaderInfo(can_frame* pCANframe);          //!< handles the receipt of can message 'BootLoaderInfo'
    void HandleCANMsgLifeCycleData(can_frame* pCANframe);           //!< handles the receipt of can message 'LifeCycleData'
    void HandleCANMsgLaunchData(can_frame* pCANframe);              //!< handles the receipt of can message 'LaunchData'
    void HandleCANMsgBoardName(can_frame* pCANframe);               //!< handles the receipt of can message 'BoardName'
    void HandleCANMsgBoardOptions(can_frame* pCANframe);            //!< handles the receipt of can message 'BoardOptions'
    //void HandleCANMsgConfigVoltageMonitor(can_frame* pCANframe);  //!< handles the receipt of can message 'ConfigVoltageMonitor'
    void HandleCANMsgVoltageState(can_frame* pCANframe);            //!< handles the receipt of can message 'VoltageState'
    //void HandleCANMsgConfigCurrentMonitor(can_frame* pCANframe);  //!< handles the receipt of can message 'ConfigCurrentMonitor'
    void HandleCANMsgCurrentState(can_frame* pCANframe);            //!< handles the receipt of can message 'CurrentState'
    void HandleCANMsgUniqueNumber(can_frame* pCANframe);            //!< handles the receipt of can message 'UniqueNumber'
    void HandleCANMsgModuleSerialNumber(can_frame* pCANframe);      //!< handles the receipt of can message 'ModuleSerialNumber'

    //! Helper functions
    static void CalcTimeDiff(struct timeb tpTimeEnd, struct timeb tbTimeStart, qint16& nSeconds, qint32& nMilliSeconds);
    void ThrowEvent(quint32 EventCode, quint16 EventData);

    /*! Initialisation state type definition */
    typedef enum {
        CN_SUB_STATE_INIT_UNDEF  = 0x00,
        CN_SUB_STATE_INIT_START  = 0x01,
        CN_SUB_STATE_INIT_ACKN   = 0x02,
        CN_SUB_STATE_INIT_FINISH = 0x03
    } CANNodeSubStateInit_t;

    CANNodeSubStateInit_t m_SubStateInit;   //!< Initialisation state

    /*! Configuraton state defines */
    typedef enum {
        CN_SUB_STATE_CONFIG_UNDEF         = 0x00,   //!< undefined state
        CN_SUB_STATE_CONFIG_START         = 0x01,   //!< initial sub state
        CN_SUB_STATE_CONFIG_REQ           = 0x02,   //!< configuration request send
        CN_SUB_STATE_CONFIG_REC           = 0x03,   //!< at least one configuration message received
        CN_SUB_STATE_CONFIG_REAL_TIME     = 0x04,   //!< configure the slave's realtime clock
        CN_SUB_STATE_CONFIG_FCT_CONFIRMED = 0x05,   //!< wait until all function modules are confirmed
        CN_SUB_STATE_CONFIG_FINISH        = 0x06,   //!< configuration has finished
        CN_SUB_STATE_CONFIG_ERROR         = 0x07    //!< error state, configuration aborted
    } CANNodeSubStateConfig_t;

    CANNodeSubStateConfig_t m_SubStateConfig;   //!< Configuraton state

    /*! Function modules initialisation state defines */
    typedef enum {
        CN_SUB_STATE_INIT_FCT_UNDEF  = 0x00,    //!< undefined state
        CN_SUB_STATE_INIT_FCT_START  = 0x01,    //!< start up
        CN_SUB_STATE_INIT_FCT_RUN    = 0x02,    //!< function module confuguration
        CN_SUB_STATE_INIT_FCT_FINISH = 0x03     //!< finished
    } CANNodeSubStateInitFct_t;

    CANNodeSubStateInitFct_t m_SubStateInitFct; //!< function modules initialisation state

    /*! Module command type definitions*/
    typedef enum {
        CN_CMD_SET_NODE_STATE,      //!< set node state
        CN_CMD_REQ_NODE_STATE,      //!< request node state
        //CN_CMD_REQ_EMCY_STOP,       //!< request emergency stop
        //CN_CMD_REQ_RESET,           //!< request reset
        //CN_CMD_CONF_STATISTICS,     //!< configure statistics
        CN_CMD_REQ_DATA_RESET,      //!< request data reset
        CN_CMD_REQ_FORMAT_MEM,      //!< request formatting memory
        CN_CMD_REQ_SERIAL_NB,       //!< request serial number
        CN_CMD_REQ_END_TEST_RESULT, //!< request end test result
        CN_CMD_REQ_HW_INFO,         //!< request hardware infomation
        CN_CMD_REQ_SW_INFO,         //!< request software infomation
        CN_CMD_REQ_LOADER_INFO,     //!< request loader info
        CN_CMD_REQ_LIFE_CYCLE_DATA, //!< request life cycle data
        CN_CMD_REQ_LAUNCH_DATE,     //!< request launch date
        CN_CMD_REQ_BOARD_NAME,      //!< request board name
        CN_CMD_REQ_BOARD_OPTIONS,   //!< request board options
        CN_CMD_CONF_VOLTAGE_MON,    //!< configure voltage monitoring
        CN_CMD_REQ_VOLTAGE_STATE,   //!< change node state
        CN_CMD_CONF_CURRENT_MON,    //!< configure current monitoring
        CN_CMD_REQ_CURRENT_STATE,   //!< request current state
        CN_CMD_REQ_UNIQUE_NUMBER,   //!< request unique number
        CN_CMD_REQ_MODULE_SERIAL    //!< request module serial number
    } CANNodeModuleCmdType_t;

    /*! Module command data, used for internal data transfer*/
    typedef struct {
        CANNodeModuleCmdType_t Type;    //!< command type
        ModuleCmdState_t State;         //!< command state
        //quint16 Data;                   //!< data transfer
        //quint32 DataU32;                //!< data transfer 32 bit
        NodeState_t NodeState;          //!< node state
        Global::MonotonicTime ReqSendTime;  //!< time the command was executed
        qint32 Timeout;                     //!< timeout in ms
    } ModuleCommand_t;

    QList<ModuleCommand_t *> m_ModuleCommand;   //!< Queue of module commands for simultaneous execution

    //! Adds the module command type to the transmit queue
    ModuleCommand_t *SetModuleTask(CANNodeModuleCmdType_t CommandType);
    //! Clears all entrys with the specified module command type to free
    void ResetModuleCommand(CANNodeModuleCmdType_t CommandType);

    FctModuleList m_FunctionModuleList; //!< List of function modules assigned to this node

    quint8  m_NodeClass;        //!< Node class received by the harwdare ID message
    quint8  m_ProtocolVersion;  //!< CAN protocoll version
    quint16 m_HWVersion;        //!< hardware version, received with CAN message HardwareID
    quint16 m_SWVersion;        //!< software version, received with CAN message HardwareID
    quint8  m_ChannelCount;     //!< number of channels, including base module

    QMap<quint16, quint16> m_ModuleSWVersion;   //!< Software version of each module
    CANNodeMainState_t m_MainState;             //!< Main state

    // CAN communication, the following section declares the CAN message ID variables
    quint32 m_ulCANNodeID;                  //!< Node ID, composed by node index and node type
                                            //!< declared as quint32 for easy calculation with can message IDs

    quint32 m_unCanIDHardwareID;            /*!< CAN-ID 'HardwareID' message */
    quint32 m_unCanIDAcknHardwareID;        /*!< CAN-ID 'AcknHardwareID' message */

    quint32 m_unCanIDReqConfig;             /*!< CAN-ID 'RequestConfiguration' message */
    quint32 m_unCanIDConfig;                /*!< CAN-ID 'Configuration' message */

    quint32 m_unCanIDSetNodeState;          /*!< CAN-ID 'SetNodeState' message */
    quint32 m_unCanIDReqNodeState;          /*!< CAN-ID 'ReqNodeState' message */
    quint32 m_unCanIDNodeState;             /*!< CAN-ID 'NodeState' message */
    quint32 m_unCanIDRealTime;              /*!< CAN-ID 'RealTime' message */
    quint32 m_unCanIDSysClock;              /*!< CAN-ID 'SystemClock' message */
    quint32 m_unCanIDHearbeatCfg;           /*!< CAN-ID 'HeartbeatConfig' message */
    quint32 m_unCanIDHeartbeatSlave;        /*!< CAN-ID 'HeartbeatSlave' message */
    quint32 m_unCanIDHeartbeatMaster;       /*!< CAN-ID 'HeartbeatMaster' message */
    quint32 m_unCanIDEmgcyStop;             /*!< CAN-ID 'EmergencyStop' message */
    quint32 m_unCanIDAbortedByEmgcyStop;    /*!< CAN-ID 'AbortByEmgcyStop' message */
    quint32 m_unCanIDMasterPower;           /*!< CAN-ID 'Master Power' message */
    quint32 m_unCanIDReset;                 /*!< CAN-ID 'NodeReset' message */
    quint32 m_unCanIDAcknReset;             /*!< CAN-ID 'AcknNodeReset' message */

    quint32 m_unCanIDStatistics;            /*!< CAN-ID 'Statistics' message */
    quint32 m_unCanIDCfgStatistics;         /*!< CAN-ID 'CfgStatistics' message */

    quint32 m_unCanIDReqMemoryFormat;       //!< CAN-ID 'Request formating memory'
    quint32 m_unCanIDAcknMemoryFormat;      //!< CAN-ID 'AcknMemoryFormat'
    quint32 m_unCanIDReqSerialNumber;       //!< CAN-ID 'RequestSerialNumber'
    quint32 m_unCanIDSerialNumber;          //!< CAN-ID 'SerialNumber'
    quint32 m_unCanIDReqEndTestResult;      //!< CAN-ID 'RequestEndTestResult'
    quint32 m_unCanIDEndTestResult;         //!< CAN-ID 'EndTestResult'
    quint32 m_unCanIDReqHWInfo;             //!< CAN-ID 'RequestHardwareInformation'
    quint32 m_unCanIDHWInfo;                //!< CAN-ID 'HardwareInformation'
    quint32 m_unCanIDReqSWInfo;             //!< CAN-ID 'RequestSoftwareInformation'
    quint32 m_unCanIDSWInfo;                //!< CAN-ID 'SoftwareInformation'
    quint32 m_unCanIDReqBootLoaderInfo;     //!< CAN-ID 'ReqBootLoaderInfo'
    quint32 m_unCanIDBootLoaderInfo;        //!< CAN-ID 'BootLoaderInfo'
    quint32 m_unCanIDReqLifeCycleData;      //!< CAN-ID 'ReqLifeCycleData'
    quint32 m_unCanIDLifeCycleData;         //!< CAN-ID 'LifeCycleData'
    quint32 m_unCanIDReqLaunchDate;         //!< CAN-ID 'RequestLaunchData'
    quint32 m_unCanIDLaunchDate;            //!< CAN-ID 'LaunchData'
    quint32 m_unCanIDReqBoardName;          //!< CAN-ID 'RequestBoardName'
    quint32 m_unCanIDBoardName;             //!< CAN-ID 'BoardName'
    quint32 m_unCanIDReqBoardOptions;       //!< CAN-ID 'RequestBoardOptions'
    quint32 m_unCanIDBoardOptions;          //!< CAN-ID 'BoardOptions'

    quint32 m_unCanIDCfgVoltageMonitor;     //!< CAN-ID 'ConfigVoltageMonitor'
    quint32 m_unCanIDReqVoltageState;       //!< CAN-ID 'RequestVoltageState'
    quint32 m_unCanIDVoltageState;          //!< CAN-ID 'VoltageState'
    quint32 m_unCanIDCfgCurrentMonitor;     //!< CAN-ID 'ConfigCurrentMonitor'
    quint32 m_unCanIDReqCurrentState;       //!< CAN-ID 'RequestCurrentState'
    quint32 m_unCanIDCurrentState;          //!< CAN-ID 'CurrentState'
    quint32 m_unCanIDReqUniqueNumber;       //!< CAN-ID 'RequestUniqueNumber'
    quint32 m_unCanIDUniqueNumber;          //!< CAN-ID 'UniqueNumber'
    quint32 m_unCanIDReqModuleSerial;       //!< CAN-ID 'RequestModuleSerialNumber'
    quint32 m_unCanIDModuleSerial;          //!< CAN-ID 'ModuleSerialNumber'
    quint32 m_unCanIDSetModuleSerial;       //!< CAN-ID 'SetModuleSerialNumber'

    //!< heartbeat control
    struct timeb m_tbHeartbeatTimeDelay;    //!< Heartbeat control timer
    quint8 m_bHeartbeatActive;              //!< Flag for heartbeat activation
    quint8 m_bHBErrorState;                 //!< Heartbeat error state

    // time observation for CAN message hardwareID
    // we have to detect if two identical nodes are connected to CAN bus,
    // this is done by supervise the CAN message's interval
    struct timeb m_tbHardwareIDMsgDelay;    //!< HardwareID message control timer
    quint8 m_bHardwareIDErrorState;         //!< Error state regarding the HardwareID reception

    NodeState_t m_NodeState;    //!< Actual node state
    CBootLoader *mp_BootLoader; //!< Handles boot loader state and messages

    QString m_SerialNumber;     //!< Serial number of the slave
    QByteArray m_UniqueNumber;  //!< Unique number of the slave
};

QTextStream& operator<< (QTextStream& s, const NodeState_t &NodeState); //!< convert NodeState_t to string

} //namespace

#endif /* DEVICECONTROL_BASEMODULEPRIVATE_H */

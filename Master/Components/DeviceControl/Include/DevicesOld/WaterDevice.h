/****************************************************************************/
/*! \file WaterDevice.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    08.07.2010
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CWaterDevice
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

#ifndef WATERDEVICE_H
#define WATERDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"

namespace DeviceControl
{

class DeviceProcessing;
class CDigitalInput;
class CDigitalOutput;

#define MAX_WATER_VALVE_COUNT  6   ///< number of valves
#define WD_MAX_MODULE_CMD_IDX  6   ///< number of module commands which can be handled simultaneously

/****************************************************************************/
/*! \class CWaterDevice
 *
 *  \brief This class provides functions to control the water device
 *
 *         By calling functions of this class, the following functionality can be controlled:
 *          - Switch on/off the vater valves of the water stations
 *          - supervise the water collecting tray sensor
 *
 *         General:
 *         This is the implementation class of the water device functionality. The functions of this class
 *         shall not be called directly from other software layers, because they are not thread-safe.
 *         Use the interface class of the water device for functional requests, the interface class is IWaterDevice.
 *         The public functions of IWaterDevice are thread-safe and can be called from other software layers.
 *
 *         Description:
 *         This class controls the following function modules:
 *               water_valve1 - water valve of cuvette W08
 *               water_valve2 - water valve of cuvette W09
 *               water_valve3 - water valve of cuvette W22
 *               water_valve4 - water valve of cuvette W23
 *               water_valve5 - water valve of cuvette W36
 *               water_valve6 - water valve of cuvette W37
 *               collecting_tray_limit - limit switch detecting the liquid level at the collecting tray
 *         Pointer to those function modules are stored by this class, the pointer initialisation is done
 *          by the HandleInitializationState() function.
 *
 *         To receive the function modules information, it's signals are connected to the classes slots. The connection
 *           is established by the HandleConfigurationState() function.
 *
 *         If initialisation and configuration has been done successfully, the internal state (m_MainState) changes to DEVICE_MAIN_STATE_IDLE.
 *         While m_MainState == DEVICE_MAIN_STATE_IDLE, the class accepts functional requests.
 *
 *         The following functional requests are provided:
 *            SetValveStatus - Set the valve status of a unique valve, or of all valves
 *            ReqValveStatus - Request the actual valve status of a unique valve
 *            ReqLiquidLevel - Request the liquid level
 *
 *         The request execution is done assynchron, the function return code shows if the request was accepted or not.
 *         If the request was accepted and its processing has finished, the class calls the corresponding routing
 *         function of IWaterDevice. This is done in case of an error as well. The routing functions are assigned as shown below:
 *            SetValveStatus -> IWaterDevice::RouteValveStatus
 *            ReqValveStatus -> IWaterDevice::RouteValveStatus
 *            ReqLiquidLevel -> IWaterDevice::RouteLiquidLevel
 *        The parameter 'HdlInfo', whis is part of all routing functions parameter list, is set to DCL_ERR_FCT_CALL_SUCCESS if
 *        the request was processed successfully. If the parameter is set to another value, an error has occured while processing
 *        the request. in this case the functional parameters are invalid.
 *
 *        -------------------------
 *        Processing a request
 *        When calling a request function, the parameters of the request are stored at a free element of the module command array.
 *        If no free module command was found, the request function returns an error.
 *        At least the command type is stored at the free module command, and the state is set to WATERDEV_MODULE_CMD_STATE_REQ.
 *        If the request has additional parameters (e.g. output values, motor speeds,..), those parameters are stored at the mocule command as well.
 *        The variable m_TaskID is set to WATERDEV_TASKID_COMMAND_HDL, this indicates that a pending module command waits for execution.
 *
 *        When HandleIdleState() is called and m_TaskID == WATERDEV_TASKID_COMMAND_HDL, the function HandleCommandRequestTask() is called to
 *        handle the tasks of the pending module commands. A module command whose state is WATERDEV_MODULE_CMD_STATE_REQ, will be procceded by
 *        calling the corresponding function module request.
 *        The following list shows the module command types and the accordant function-module functions called when the command type is pending:
 *            SetValveStatus -> CANDigitalOutput::SetOutputValue(..)
 *            ReqValveStatus -> CANDigitalOutput::ReqOutputValue()
 *            ReqLiquidLevel -> CANDigitalInput::ReqActInputValue()
 *        Additionally to the function call a timeout will be started. The module command state is set to WATERDEV_MODULE_CMD_STATE_REQ_SEND.
 *
 *        When the module command state is WATERDEV_MODULE_CMD_STATE_REQ_SEND, the answer of the function module is expected. The function module
 *        gives the answer by emitting a signal. The list below shows the assignment between the called function and the signal:
 *            CANDigitalOutput::SetOutputValue(...)  -> CANDigitalOutput::ReportOutputValueAckn(...)
 *            CANDigitalOutput::ReqOutputValue(...)  -> CANDigitalOutput::ReportActOutputValue(...)
 *            CANDigitalInput::ReqActInputValue(...) -> CANDigitalInput::ReportActInputValue(...)
 *
 *        Those signals are connected to slots of this class as shown here:
 *            CANDigitalOutput::ReportOutputValueAckn(...) -> CWaterDevice::WaterValveOutputValueAckn(...)
 *            CANDigitalOutput::ReportActOutputValue(...)  -> CWaterDevice::WaterValveActOutputValue(...)
 *            CANDigitalInput::ReportActInputValue(...)    -> CWaterDevice::LiquidLevelSensorStatus(...)
 *
 *        Additionally, the funtion modules error notification signals are connected:
 *            CANDigitalOutput::ReportError(...) -> CWaterDevice::WaterValveError(...)
 *            CANDigitalInput::ReportError(...)  -> CWaterDevice::LiquidLevelSensorError(...)
 *
 *        If a signal was received, and the accordingly slot was called, the signal's information will be forwarded to the IWaterDevice class.
 *        This is done by calling the routing function of the interface class ( refer the list above). Additionally, the state of the module command, which has
 *        triggered the procedure, will be set to WATERDEV_MODULE_CMD_STATE_FREE. This is done at the function ResetModuleCommand(...).
 *        The timeout supervision is done at function HandleCommandRequestTask(), when the time has expired, an error will be set.
 *
 *        In summary the list below shows the control flow if a function was called.
 *
 *         IWaterDevice::HandleCommandRequestTask() -> SetValveStatus(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANDigitalOutput::SetOutputValue(...) ---------- *process slave communication*
 *         IWaterDevice::RouteValveStatus <- WaterValveOutputValueAckn(...) <- CANDigitalOutput::ReportOutputValueAckn(...) <--- *receive slave message*
 *
 *         IWaterDevice::HandleCommandRequestTask() -> ReqOutputValue(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANDigitalOutput::ReqOutputValue(...)
 *         IWaterDevice::RouteValveStatus <- WaterValveActOutputValue(...) <- CANDigitalOutput::ReportActOutputValue(...)
 *
 *         IWaterDevice::HandleCommandRequestTask() -> ReqLiquidLevel(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANDigitalInput::ReqActInputValue(...)
 *         IWaterDevice::RouteLiquidLevel <- LiquidLevelSensorStatus(...) <- CANDigitalInput::ReportActInputValue(...)
 *
 *        -------------------------
 *        Error detection and error handling
 *        The following errors can occur:
 *         - A call to a function-modules function failed (e.g. invalid state of the function module)
 *            -> m_lastErrorHdlInfo is set to the return value of the function module
 *            -> ErrorCode is set to the error, according to the module command type: EVENT_DCL_WATER_DEV_<Type>_FCT_CALL_FAILED. Ths
 *               The function SetErrorParameter(..) will be called and sets the variable m_lastErrorCode to this error code.
 *               At SetErrorParameter(..) the variable m_lastErrorGroup is set to EVENT_GRP_DCL_WATER_DEV,
 *               m_lastErrorData is set to the m_TaskID, and m_lastErrorTime is set to the current time.
 *  Pruefe: Soll wirklich m_TaskID in m_lastErrorData gesetzt werden? besser m_ModuleCommand[idx].Type oder m_lastErrorHdlInfo?
 *         - The timeout expires before receiving the function modules 'answer'
 *             -> The function SetErrorParameter(..) will be called and sets the variable m_lastErrorCode to EVENT_DCL_WATER_DEV_<Type>_TIMEOUT.
 *                At SetErrorParameter(..) the variable m_lastErrorGroup is set to EVENT_GRP_DCL_WATER_DEV,
 *                m_lastErrorData is set to the m_TaskID, and m_lastErrorTime is set to the current time.
 *  Pruefe auch hier: Soll wirklich m_TaskID in m_lastErrorData gesetzt werden? besser m_ModuleCommand[idx].Type oder m_lastErrorHdlInfo?
 *
 *         - A function module sends an error signal, which is received by one of the following slots:
 *              CANDigitalInput   ->  LiquidLevelSensorError(...)
 *              CANDigitalOutput  ->  WaterValveError(...)
 *           The error information will be forwarded to the interface class by calling it's RouteError(..) function. The instanceID will be set to
 *           the device's instance ID (because higher software layer don't now about function modules like motors, digital IO and so on.
 *           If one of this slots is called, the device is set to error mode, which is done by setting the m_TaskID to WATERDEV_TASKID_ERROR
 *         - If a functional slot is called, and the parameter HdlInfo doesn't match DCL_ERR_FCT_CALL_SUCCESS, this value is forwarded to the interface class.
 *           In this case, the functional parameters are invalid (usually set to 0)
 *
 *        -------------------------
 *        Error logging
 *        An error is always logged as closed to it's detection as possible. To log an error means the error information is forwarded by calling the
 *        function DeviceProcessing::ThrowError(..), this ensures that the information is stored to the 'general' error log file.
 *        At this class, the following errors will be forwarded to DeviceProcessing::ThrowError(..):
 *         - Call to a function-modules function failed
 *         - The timeout expires
 */
/****************************************************************************/
class CWaterDevice : public CBaseDevice
{
    Q_OBJECT

public:
    //!< constructor
    CWaterDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    //!< destructor
    virtual ~CWaterDevice();

    //!< general task handling function
    void HandleTasks();

    //!< Set the valve status from a unique valve, or all
    ReturnCode_t SetValveStatus(WaterValveID_t valveID, quint8 valveState);
    //!< Request the actual valve status of a unique valve, or all
    ReturnCode_t ReqValveStatus(WaterValveID_t valveID);
    //!< Request the liquid level
    ReturnCode_t ReqLiquidLevel();

    //qint16 GetLastErrorCode();
    ReturnCode_t ClearErrorState();

private slots:
    //!< Liquid level sensor error notification
    void LiquidLevelSensorError(quint32 instanceID, quint16 errGroup, quint16 errorCode, quint16 errorData, QDateTime errorTimeStamp);
    //!< Liquid level sensor input status notification
    void LiquidLevelSensorStatus(quint32 instanceID, ReturnCode_t hdlInfo, quint16 inputValue);
    //!< Valve output channels error notification
    void WaterValveError(quint32 instanceID, quint16 errGroup, quint16 errorCode, quint16 errorData, QDateTime errorTimeStamp);
    //!< Valve output acknwoledge notification
    void WaterValveOutputValueAckn(quint32 instanceID, ReturnCode_t hdlInfo, quint16 OutputValue);
    //!< Valve output request notification
    void WaterValveActOutputValue(quint32 instanceID, ReturnCode_t hdlInfo, quint16 OutputValue);

private:
    //!< Handle substate initialisation
    ReturnCode_t HandleInitializationState();
    //!< Handle substate configuration
    ReturnCode_t HandleConfigurationState();

    //!< idle state handling task
    void HandleIdleState();
    //!< command handling task
    void HandleCommandRequestTask();
    //!< error handling task
    void HandleErrorState();

    //!< Return the function module's position within the array
    quint8 GetValveIndexFromType(WaterValveID_t valveID);
    //!< Return the Valve id type from function module's array index position
    WaterValveID_t GetValveIDFromIndex(quint8 valveID);

    CDigitalOutput* m_pWaterValve[MAX_WATER_VALVE_COUNT];   //!< 6* water valve
    CDigitalInput* m_pLiquidLevel;     //!< liquid limit switch, collecting tray

    /*! Task definitions */
    typedef enum {WATERDEV_TASKID_INIT            = 0x00,  //!< initialisation state
                  WATERDEV_TASKID_FREE            = 0x01,  //!< idle state
                  WATERDEV_TASKID_ERROR           = 0x02,  //!< error occurred
                  WATERDEV_TASKID_COMMAND_HDL     = 0x03,  //!< at least one command is active and must be handled
    } WaterDevTaskID_t;        
    WaterDevTaskID_t    m_TaskID;  //!< Task identifier

    /*! Device command type definitions */
    typedef enum { WATERDEV_CMD_TYPE_UNDEF    = 0x00,   //!< undefined command type
                   WATERDEV_CMD_REQ_LIQUID_LEVEL,       //!< request liquid level
                   WATERDEV_CMD_SET_VALVE_STATE,        //!< set valve state
                   WATERDEV_CMD_REQ_ACTVALVE_STATE      //!< request actual valve state
    } WaterDevCmdType_t;

    /*! command state definitions */
    typedef enum { WATERDEV_MODULE_CMD_STATE_FREE     = 0x00,   //!< undefined command state
                   WATERDEV_MODULE_CMD_STATE_REQ      = 0x01,   //!< command was reqested, waits for beeing send to slave
                   WATERDEV_MODULE_CMD_STATE_REQ_SEND = 0x02,   //!< command requests was sent to the slave
                   WATERDEV_MODULE_CMD_STATE_FINISHED = 0x03,   //!< command was finished
                   WATERDEV_MODULE_CMD_STATE_ERR      = 0x04    //!< command was finished with error
    } WaterDevCmdState_t;

    /*! device command data, used for internal data transfer */
    typedef struct {
        WaterDevCmdType_t      Type;          //!< command type
        WaterDevCmdState_t     State;         //!< command state
        Global::MonotonicTime  ReqSendTime;   //!< time the command was executed
        qint32                 Timeout;       //!< timeout in ms
        WaterValveID_t         m_ValveID;     //!< valve identification
        quint8                 m_ValveStatus; //!< multible usage for data transfer
        quint8                 m_AcknMask;    //!< bit mask to mark acknowledge reception
    } WaterDevCommand_t;


    WaterDevCommand_t m_ModuleCommand[WD_MAX_MODULE_CMD_IDX];  //!< module command array for simultaneously command execution

    //!< set the module command type to free entry within array
    bool SetModuleTask(WaterDevCmdType_t CommandType, quint8* pCmdIndex = 0);
    //!< clears all entrys with the specified module command type to free
    void ResetModuleCommand(WaterDevCmdType_t);

    /*typedef enum {WATER_DEV_TASKID_INIT             = 0x00,  ///< first state after creation
                  WATER_DEV_TASKID_FREE             = 0x01,  ///< init state, read the assigned function modules
                  WATER_DEV_TASKID_REQ_LIQUID_LEVEL = 0x02,  ///< request the liquid sensor state
                  WATER_DEV_TASKID_SET_VALVE        = 0x03,  ///< Set the valve state
                  WATER_DEV_TASKID_REQ_VALVE_STATUS = 0x04,  ///< request the valve
    } WaterDevTaskID_t;

     typedef enum {WATER_DEV_TASK_STATE_FREE      = 0x00,  ///< task state free, ready for action request
                   WATER_DEV_TASK_STATE_REQ       = 0x01,  ///< a task was requested, next step will be to forward the command via CAN
                   WATER_DEV_TASK_STATE_REQ_SEND  = 0x02,  ///< command was forwarded via CAN, wait for CAN acknowledge, check timeout
                   WATER_DEV_TASK_STATE_ERROR     = 0x03,  ///< error, e.g. timeout while waiting for acknowledge, or error CAN-message received
     } WaterDevTaskState_t;*/

     /*WaterDevTaskID_t       m_taskID;         ///< task requested to the water device
     WaterDevTaskState_t    m_taskState;      ///< task state
     WaterDevErrTaskState_t m_errTaskState;   ///< the error task state*/

     //requested valve status, we set just one valve
     /*WaterValveID_t m_reqValveID;            ///< valve identification
     quint8 m_reqValveStatus;                ///< status of the valve

     WaterValveID_t m_valveID;               ///< valve identification
     quint8 m_actValvesStatusMask;           ///< status of the valve

     quint16 m_liquidLevel;                  ///< status of the liquid level sensor (Bit 0)*/

     /*! Error task state */
     typedef enum {WATER_DEV_ERRTASK_STATE_FREE            = 0x00,  ///< task state free, no error
                   WATER_DEV_ERRTASK_STATE_REPORT_IFACE    = 0x01,  ///< reports the error to the interface class
                   WATER_DEV_ERRTASK_STATE_REPORT_DEVPROC  = 0x02,  ///< reports the error to device processing
                   WATER_DEV_ERRTASK_STATE_IDLE            = 0x03   ///< error idle state, do nothing, block anything
     } WaterDevErrTaskState_t;
     WaterDevErrTaskState_t m_ErrorTaskState;  //!< error task state

};

}  //namespace

#endif /* WATERDEVICE_H */

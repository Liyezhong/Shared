/****************************************************************************/
/*! \file OvenDevice.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    08.07.2010
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class COvenDevice
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

#ifndef OVENDEVICE_H
#define OVENDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"

class WrapperDeviceOven;

namespace DeviceControl
{

class CTemperatureControl;
class CStepperMotor;

#define OVEN_DEV_TASK_REFRUN          0 //!< reference run
#define OVEN_DEV_TASK_COVER_POS_SET   1 //!< set oven cover position
#define OVEN_DEV_TASK_COVER_POS_REQ   2 //!< request oven cover position
#define OVEN_DEV_TASK_TEMP_SET        3 //!< set temperature
#define OVEN_DEV_TASK_TEMP_REQ        4 //!< request temperature
#define OVEN_DEV_TASK_TEMP_STATUS_SET 5 //!< set status temperatur ctrl.
#define OVEN_DEV_TASK_TEMP_STATUS_REQ 6 //!< request status temperatur ctrl.
#define OVEN_DEVICE_TASK_COUNT        7 //!< number of device tasks

/****************************************************************************/
/*!
 *  \brief This class provides functions to control the oven
 *
 *         By calling functions of this class, the following functionality can be controlled:
 *          - Switch on/off the oven device temperature control
 *          - Reference run of the oven cover
 *          - Open or close the oven cover
 *          - Set the oven temperature
 *
 *         General:
 *         This is the implementation class of the oven device functionality. The functions of this class
 *         shall not be called directly from other software layers, because they are not thread-safe.
 *         Use the interface class of the water device for functional requests, the interface class is IWaterDevice.
 *         The public functions of IWaterDevice are thread-safe and can be called from other software layers.
 *
 *         Description:
 *         This class controls the following function modules:
 *               motor_oven_cover - oven cover stepper motor
 *               temp_ctrl_oven   - oven temeprature control
 *
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
 *            ReqReferenceRun        - Execute the reference run of the oven cover motor
 *            ReqCoverMoveToPosition - Position the oven cover
 *            ReqActCoverPosition    - Request actual oven cover position
 *            SetTemperature         - Set oven nominal temperature
 *            ReqActTemperature      - Request actual oven temperature
 *            SetTempCtrlStatus      - Set the status of the oven temperature control (on/off)
 *            ReqTempCtrlStatus      - Request the status of the oven temperature control
 *
 *         The request execution is done assynchron, the function return code shows if the request was accepted or not.
 *         If the request was accepted and its processing has finished, the class calls the corresponding routing
 *         function of IWaterDevice. This is done in case of an error as well. The routing functions are assigned as shown below:
 *            ReqReferenceRun        -> IOvenDevice::RouteReferenceRunAckn
 *            ReqCoverMoveToPosition -> IOvenDevice::RouteOvenCoverPositionAckn
 *            ReqActCoverPosition    -> IOvenDevice::RouteActOvenCoverPosition
 *            SetTemperature         -> IOvenDevice::RouteActTemperature
 *            ReqActTemperature      -> IOvenDevice::RouteActTemperature
 *            SetTempCtrlStatus      -> IOvenDevice::RouteActOvenStatus
 *            ReqTempCtrlStatus      -> IOvenDevice::RouteActOvenStatus
 *
 *        The parameter 'HdlInfo', whis is part of all routing functions parameter list, is set to DCL_ERR_FCT_CALL_SUCCESS if
 *        the request was processed successfully. If the parameter is set to another value, an error has occured while processing
 *        the request. In this case the functional parameters are invalid.
 *
 *        -------------------------
 *        Processing a request
 *        When calling a request function, the parameters of the request are stored at a free element of the module command array.
 *        If no free module command was found, the request function returns an error.
 *        At least the command type is stored at the free module command, and the state is set to WATERDEV_MODULE_CMD_STATE_REQ.
 *        If the request has additional parameters (e.g. output values, motor speeds,..), those parameters are stored at the mocule command as well.
 *        The variable m_TaskID is set to OVENDEV_TASKID_COMMAND_HDL, this indicates that a pending module command waits for execution.
 *
 *        When HandleIdleState() is called and m_TaskID == OVENDEV_TASKID_COMMAND_HDL, the function HandleCommandRequestTask() is called to
 *        handle the tasks of the pending module commands. A module command whose state is OVENDEV_MODULE_CMD_STATE_REQ will be procceded by
 *        calling the corresponding function module request.
 *        The following list shows the module command types and the accordant function-module functions called when the command type is pending:
 *            ReqReferenceRun        -> CANStepperMotor::ExecReferenceMovement
 *            ReqCoverMoveToPosition -> CANStepperMotor::DriveToPosition
 *            ReqActCoverPosition    -> CANStepperMotor::ReqActPosition
 *            SetTemperature         -> CANTemperatureControl::SetTemperature
 *            ReqActTemperature      -> CANTemperatureControl::ReqActTemperature
 *            SetTempCtrlStatus      -> CANTemperatureControl::SetStatus
 *            ReqTempCtrlStatus      -> CANTemperatureControl::ReqStatus
 *        Additionally to the function call a timeout will be started. The module command state is set to OVENDEV_MODULE_CMD_STATE_REQ_SEND.
 *
 *        When the module command state is OVENDEV_MODULE_CMD_STATE_REQ_SEND, the answer of the function module is expected. The function module
 *        gives the answer by emitting a signal. The list below shows the assignment between the called function and the signal:
 *            CANStepperMotor::ExecReferenceMovement(...) -> CANStepperMotor::ReportReferenceMovementAckn(...)
 *            CANStepperMotor::DriveToPosition(...)       -> CANStepperMotor::ReportMovementAckn(...)
 *            CANStepperMotor::ReqActPosition(...)        -> CANStepperMotor::ReportPosition(...)
 *            CANTemperatureControl::SetTemperature(...)     -> TBD
 *            CANNTemperatureControl::ReqActTemperature(...) -> CANStepperMotor::ReportActTemperature(...)
 *            CANTemperatureControl::SetStatus(...)          -> TBD
 *            CANTemperatureControl::ReqStatus(...)          -> CANStepperMotor::ReportActStatus(...)
 *
 *
 *        Those signals are connected to slots of this class as shown here:
 *            CANStepperMotor::ReportReferenceMovementAckn(...) -> COvenDevice::StepperMotorReferenceMovementAckn(...)
 *            CANStepperMotor::ReportMovementAckn(...)          -> COvenDevice::StepperMotorMovementAckn(...)
 *            CANStepperMotor::ReportPosition(...)              -> COvenDevice::StepperMotorActPosition(...)
 *            CANTemperatureControl::ReportActTemperature(...)  -> COvenDevice::TempCtrlActTemperature(...)
 *            CANTemperatureControl::ReportActStatus(...)       -> COvenDevice::TempCtrlActStatus(...)
 *
 *        Additionally, the funtion modules error notification signals are connected to slots:
 *            CANStepperMotor::ReportError(...)       -> COvenDevice::StepperMotorError(...)
 *            CANTemperatureControl::ReportError(...) -> COvenDevice::TempCtrlError(...)
 *
 *        If a signal was received, and the accordingly slot was called, the signal's information will be forwarded to the interface class.
 *        This is done by calling the routing function of the interface class ( refer the list above). Additionally, the state of the module command, which has
 *        triggered the procedure, will be set to OVENDEV_MODULE_CMD_STATE_FREE. This is done at the function ResetModuleCommand(...).
 *        The timeout supervision is done at function HandleCommandRequestTask(), when the time has expired, an error will be set.
 *
 *        In summary the list below shows the control flow if a function was called.
 *
 *         IOvenDevice::HandleCommandRequestTask() -> ReqReferenceRun(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANStepperMotor::ExecReferenceMovement(...) ---------- %process slave communication%
 *         IOvenDevice::RouteReferenceRunAckn <- StepperMotorReferenceMovementAckn(...) <- CANStepperMotor::ReportReferenceMovementAckn(...) <--- %receive slave message%
 *
 *         IOvenDevice::HandleCommandRequestTask() -> ReqCoverMoveToPosition(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANStepperMotor::DriveToPosition(...)
 *         IOvenDevice::RouteOvenCoverPositionAckn <- StepperMotorMovementAckn(...) <- CANStepperMotor::ReportMovementAckn(...)
 *
 *         IOvenDevice::HandleCommandRequestTask() -> ReqActCoverPosition(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANStepperMotor::ReqActPosition(...)
 *         IOvenDevice::RouteActOvenCoverPosition <- StepperMotorActPosition(...) <- CANStepperMotor::ReportPosition(...)
 *
 *         IOvenDevice::HandleCommandRequestTask() -> SetTemperature(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANTemperatureControl::SetTemperature(...)
 *         IOvenDevice::TBD <- TBD(...) <- CANTemperatureControl::TBD(...)
 *
 *         IOvenDevice::HandleCommandRequestTask() -> ReqActTemperature(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANTemperatureControl::ReqActTemperature(...)
 *         IOvenDevice::RouteActTemperature <- TempCtrlActTemperature(...) <- CANTemperatureControl::ReportActTemperature(...)
 *
 *         IOvenDevice::HandleCommandRequestTask() -> SetTempCtrlStatus(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANTemperatureControl::SetStatus(...)
 *         IOvenDevice::TBD <- TBD(...) <- CANTemperatureControl::TBD(...)
 *
 *         IOvenDevice::HandleCommandRequestTask() -> ReqTempCtrlStatus(..)
 *                          HandleIdleState() -> HandleCommandRequestTask() -> CANTemperatureControl::ReqStatus(...)
 *         IOvenDevice::RouteActOvenStatus <- TempCtrlActStatus(...) <- CANTemperatureControl::ReportActStatus(...)
 *
 *        -------------------------
 *        Error detection and error handling
 *        The following errors can occur:
 *         - A call to a function-modules function failed (e.g. invalid state of the function module)
 *            -> m_lastErrorHdlInfo is set to the return value of the function module
 *            -> ErrorCode is set to the error, according to the module command type: EVENT_DCL_OVEN_DEV_<Type>_FCT_CALL_FAILED.
 *               The function SetErrorParameter(..) will be called and sets the variable m_lastErrorCode to this error code.
 *               At SetErrorParameter(..) the variable m_lastErrorGroup is set to EVENT_GRP_DCL_OVEN_DEV,
 *               m_lastErrorData is set to the m_TaskID, and m_lastErrorTime is set to the current time.
 *  Pruefe: Soll wirklich m_TaskID in m_lastErrorData gesetzt werden? besser m_ModuleCommand[idx].Type oder m_lastErrorHdlInfo?
 *         - The timeout expires before receiving the function modules 'answer'
 *             -> The function SetErrorParameter(..) will be called and sets the variable m_lastErrorCode to EVENT_DCL_OVEN_DEV_<Type>_TIMEOUT.
 *                At SetErrorParameter(..) the variable m_lastErrorGroup is set to EVENT_GRP_DCL_OVEN_DEV,
 *                m_lastErrorData is set to the m_TaskID, and m_lastErrorTime is set to the current time.
 *  Pruefe auch hier: Soll wirklich m_TaskID in m_lastErrorData gesetzt werden? besser m_ModuleCommand[idx].Type oder m_lastErrorHdlInfo?
 *
 *         - A function module sends an error signal, which is received by one of the following slots:
 *              CANStepperMotor       ->  StepperMotorError(...)
 *              CANTemperatureControl ->  TempCtrlError(...)
 *           The error information will be forwarded to the interface class by calling it's RouteError(..) function. The instanceID will be set to
 *           the device's instance ID (because higher software layer don't now about function modules like motors, digital IO and so on.
 *           If one of this slots is called, the device is set to error mode, which is done by setting the m_TaskID to OVENDEV_TASKID_ERROR
 *         - If a functional slot is called, and the parameter HdlInfo doesn't match DCL_ERR_FCT_CALL_SUCCESS, this value is forwarded to the interface class.
 *           In this case, the functional parameters are invalid (usually set to 0)
 *
 *        -------------------------
 *        Error logging
 *        An error is always logged as closed to it's detection as possible. To log an error means the error information is forwarded by calling the
 *        function DeviceProcessing::ThrowError(..), this ensures that the information is stored to the 'general' error log file of the master software.
 *        At this class, the following errors will be forwarded to DeviceProcessing::ThrowError(..):
 *         - Call to a function-modules function failed
 *         - The timeout expires
 */
/****************************************************************************/
class COvenDevice : public CBaseDevice
{
    friend class ::WrapperDeviceOven;
    Q_OBJECT

public:
    //! constructor
    COvenDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    //! destructor
    virtual ~COvenDevice();

    //! general task handling function
    void HandleTasks();

    //! Execute the reference run of the oven cover motor
    ReturnCode_t ReqReferenceRun();
    //! Position the oven cover
    ReturnCode_t ReqCoverMoveToPosition(OvenCoverPosition_t CoverPosition);
    //! Request actual oven cover position
    ReturnCode_t ReqActCoverPosition();

    //! Request actual oven temperature
    ReturnCode_t ReqActTemperature();
    //! Set oven nominal temperature
    ReturnCode_t SetTemperature(qint16 Temperatur);
    //! Set the status of the oven temperature control
    ReturnCode_t SetTempCtrlStatus(TempCtrlStatus_t TempCtrlStatus);
    //! Request the status of the oven temperature control
    ReturnCode_t ReqTempCtrlStatus();

    ReturnCode_t ClearErrorState();

private slots:
    //! Oven cover motor error notification
    void StepperMotorError(quint32, quint16, quint16, quint16, QDateTime);
    //! motor set state acknowledge notification
    void StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo);
    //! motor reference run acknowledge notification
    void StepperMotorReferenceMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);
    //! motor movement acknowledge notification
    void StepperMotorMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position, qint16 speed);
    //! motor position notification
    void StepperMotorActPosition(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);

    //! Temperature control error notification
    void TempCtrlError(quint32, quint16, quint16, quint16, QDateTime);
    //! Actual temperature notification
    void TempCtrlActTemperature(quint32, ReturnCode_t, quint8 Index, qreal OvenTemperature);
    //! Set temperature acknowledge notification
    void ReportRefTemperature(quint32, ReturnCode_t, qreal);
    //! Actual temperature control status notification
    void TempCtrlActStatus(quint32, ReturnCode_t, TempCtrlStatus_t, TempCtrlMainsVoltage_t);
    //! Set temperature control status acknowledge notification
    void TempCtrlSetStatusAckn(quint32, ReturnCode_t, TempCtrlStatus_t);
    //! Actual temperature control operating mode notification
    void TempCtrlActOperatingMode(quint32, ReturnCode_t, TempCtrlOperatingMode_t);
    //! Set temperature control operating mode acknowledge notification
    void TempCtrlSetOperatingModeAckn(quint32, ReturnCode_t, TempCtrlOperatingMode_t);

private:
    //! Handle substate initialisation
    ReturnCode_t HandleInitializationState();
    //! Handle substate configuration
    ReturnCode_t HandleConfigurationState();
    //! Handle substate motor configuration
    ReturnCode_t ConfigureDeviceTasks();

    //! idle state handling task
    void HandleIdleState();
    //! command handling task
    void HandleCommandRequestTask();
    void HandleDeviceTaskActions();
    //! error handling task
    void HandleErrorState();

    //! Return the oven cover position type from motor position
    OvenCoverPosition_t GetOvencoverPositionFromMotorPos(qint32 motorPosition);
    //! Return the motor position from oven cover position type
    Position_t GetMotorPosFromCoverPos(OvenCoverPosition_t coverPosition);

    //Function modules
    CTemperatureControl* m_pTempCtrl;    //!< Temperature control
    CStepperMotor* m_pMotorOvenCover;    //!< Oven cover motor

    /*! Task definitions */
    typedef enum {
        OVENDEV_TASKID_INIT        = 0x00,  //! <initialisation state
        OVENDEV_TASKID_FREE        = 0x01,  //!< idle state
        OVENDEV_TASKID_ERROR       = 0x02,  //!< error occurred
        OVENDEV_TASKID_COMMAND_HDL = 0x03   //!< at least one command is active and must be handled
    } OvenDevTaskID_t;
    OvenDevTaskID_t m_TaskID;   //!< Task identifier

    /*! Device command type definitions */
    typedef enum {
        OVENDEV_CMD_TYPE_UNDEF       = 0x00,    //!< undefined command type
        OVENDEV_CMD_COVER_REFRUN     = 0x01,    //!< cover reference run
        OVENDEV_CMD_REQ_COVER_MOVE   = 0x02,    //!< move cover
        OVENDEV_CMD_REQ_COVER_ACTPOS = 0x03,    //!< request actual cover position
        OVENDEV_CMD_SET_TEMP         = 0x04,    //!< set oven temperature
        OVENDEV_CMD_REQ_ACT_TEMP     = 0x05,    //!< request actual oven temperature
        OVENDEV_CMD_SET_STATUS       = 0x06,    //!< set temperature control state
        OVENDEV_CMD_REQ_ACT_STATUS   = 0x07     //!< request actual temperature control state
    } OvenDevCmdType_t;

    /*! command state definitions */
    typedef enum {
        OVENDEV_MODULE_CMD_STATE_FREE     = 0x00,   //!< undefined command state
        OVENDEV_MODULE_CMD_STATE_REQ      = 0x01,   //!< command was reqested, waits for beeing send to slave
        OVENDEV_MODULE_CMD_STATE_REQ_SEND = 0x02,   //!< command requests was sent to the slave
        OVENDEV_MODULE_CMD_STATE_REQ_PROC = 0x03,   //!< command requests are processed by device tasks list
        OVENDEV_MODULE_CMD_STATE_ERR      = 0x04    //!< command was finished with error
    } OvenDevCmdState_t;

    //! activate a device task
    bool ActivateDeviceTask(quint8 DeviceTaskIndex);

    DeviceTask m_DeviceTask[OVEN_DEVICE_TASK_COUNT];  ///< device task control

    /*! error task state definitiosn */
    typedef enum {
        OVEN_DEV_ERRTASK_STATE_FREE           = 0x00,   ///< task state free, no error
        OVEN_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,   ///< reports the error to the interface class
        OVEN_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,   ///< reports the error to device processing
        OVEN_DEV_ERRTASK_STATE_IDLE           = 0x03,   ///< error idle state, do nothing, block anything
        OVEN_DEV_ERRTASK_STATE_RESET          = 0x04    ///< reset error state, change to initial state
    } OvenDevErrTaskState_t;

    OvenDevErrTaskState_t m_ErrorTaskState;  //!< error task state

    // Variables from process settings
    MotionProfileIdx_t m_CoverOpenSpeedMotionProfile;   ///< motor speed motion profile for oven cover open
    MotionProfileIdx_t m_CoverCloseSpeedMotionProfile;  ///< motor speed motion profile for oven cover close
    Position_t m_CoverClosedPosition;  //!< motor position cover closed
    Position_t m_CoverOpenPosition;    //!< motor position cover opened
};

}  //namespace

#endif /* OVENDEVICE_H */

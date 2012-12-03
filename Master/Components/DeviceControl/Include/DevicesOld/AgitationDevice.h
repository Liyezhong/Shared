/****************************************************************************/
/*! \file AgitationDevice.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    08.07.2010
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CAgitationDevice
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

#ifndef AGITATIONDEVICE_H
#define AGITATIONDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

class WrapperDeviceAgitation;

namespace DeviceControl
{

class DeviceProcessing;
class CStepperMotor;

#define AGIT_MAX_MODULE_CMD_IDX      2   ///< number of module commands which can be handled simultaneously
#define AGITATION_DEV_TASK_REFRUN       0   //!< reference run
#define AGITATION_DEV_TASK_SET_SPEED    1   //!< set agitation speed
#define AGITATION_DEV_TASK_REQ_ACTSPEED 2   //!< request agitation speed
#define AGITATION_DEV_TASK_REQ_ACTPOS   3   //!< request agitation position
#define MAX_AGITATION_DEVICE_TASK_ID    4   ///< number of device tasks

/****************************************************************************/
/*! \class CAgitationDevice
 *
 *  \brief This is the class to control the agitation
 *
 *         The class provides functions to setup the agitation speed
 *
 *         For agitation motor control, set the motors 'rev_hstep_count' value to a
 *         valid value.
 *         This parameter defines the number of half steps need for one revision of the mechanical
 *         output side.
 *         Example: Motor with 400 steps/rev, = 800 half steps
 *                  Agitation ratio is 2:1
 *                  -> we need 1600 halfsteps for one revision.
 *                   By sending a speed=0-command, setting the SubCommandId to SM_SUBCMD_MOTION_SPEED_STOP_POS,
 *                   the motor stops at the 'degree'-position taken from SubCommandData.
 *                   The conversation between degrees and half-steps is done by the slave's motor fct-module.
 *                   [here: 1600 half steps = 360 degrees]
 *
 */
/****************************************************************************/
class CAgitationDevice : public CBaseDevice
{
    friend class ::WrapperDeviceAgitation;
    Q_OBJECT

public:
    CAgitationDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    virtual ~CAgitationDevice();

    //! General task handling function
    void HandleTasks();

    //! Reqest reference run
    ReturnCode_t ReqReferenceRun();
    //! Set agitation speed, or stop it
    ReturnCode_t DriveSpeed(AgitationSpeed_t AgitationSpeed);

    //! Request actual agitation speed
    ReturnCode_t ReqActSpeed();
    //! Get actual agitation speed
    AgitationSpeed_t GetActSpeed();
    //! Get actual agitation position
    ReturnCode_t ReqActPosition();

private slots:
    //! Motor set state acknowledge notification
    void StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo);
    //! Reference run ackn. notification
    void StepperMotorReferenceMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position);
    //! Movement ackn. notification
    void StepperMotorMovementAckn(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 position, qint16 Speed);
    //! Position notification
    void StepperMotorActPosition(quint32 InstanceID, ReturnCode_t HdlInfo, qint32 Position);
    //! Motor error notification
    void StepperMotorError(quint32, quint16, quint16, quint16, QDateTime);

private:
    //! Initialisation state handling function
    ReturnCode_t HandleInitializationState();
    //! Configuration state handling function
    ReturnCode_t HandleConfigurationState();
    //! Device task configuration state handling function
    ReturnCode_t HandleDeviceTaskConfigurationState();

    //! Idle state handling task
    void HandleIdleState();
    //! Device tasks handling
    void HandleDeviceTaskActions();
    //! Error handling task
    void HandleErrorState();

    //! Request actual motor position
    AgitationPosition_t GetAgitationPosFromMotorPos(Position_t motorPos);
    //! Request actual motor speed
    AgitationSpeed_t    GetAgitationSpeedFromMotorSpeed(qint16 motorSpeed);

    qint16 GetMotorSpeedFromAgitationSpeed(AgitationSpeed_t Speed);

    CStepperMotor* m_pMotorAgitation;   //!< Pointer to the agitation motor function module

    //! Agitation task definitions
    typedef enum {
        AGITATION_TASKID_INIT            = 0x00,    //!< initialisation state
        AGITATION_TASKID_FREE            = 0x01,    //!< idle state
        AGITATION_TASKID_ERROR           = 0x02,    //!< error occurred
        AGITATION_TASKID_COMMAND_HDL     = 0x03     //!< at least one command is active and must be handled
    } AgitationTaskID_t;
    AgitationTaskID_t m_TaskID; //!< Task identifier

    //! Device command type definitions
    typedef enum {
        AGITATION_CMD_TYPE_UNDEF    = 0x00, //!< undefined command type
        AGITATION_CMD_REFRUN        = 0x01, //!< reference run
        AGITATION_CMD_SET_SPEED     = 0x02, //!< set agitation speed
        AGITATION_CMD_REQ_ACT_SPEED = 0x03, //!< request actual speed
        AGITATION_CMD_REQ_ACT_POS   = 0x04  //!< request actual position
    } AgitationCmdType_t;

    //! Command state definitions
    typedef enum {
        AGITATION_MODULE_CMD_STATE_FREE     = 0x00, //!< undefined command state
        AGITATION_MODULE_CMD_STATE_REQ      = 0x01, //!< command was reqested, waits for beeing send to slave
        AGITATION_MODULE_CMD_STATE_REQ_SEND = 0x02, //!< command requests was sent to the slave
        AGITATION_MODULE_CMD_STATE_REQ_PROC = 0x03, //!< command requests are processed by device tasks list
        AGITATION_MODULE_CMD_STATE_ERR      = 0x04  //!< command was finished with error
    } AgitationCmdState_t;

    //! Clears all entrys with the specified module command type to free
    void ResetModuleCommand(AgitationCmdType_t);

    //! Activate the device task and it's fct-mdoule tasks
    bool ActivateDeviceTask(quint8 DeviceTaskIndex);

    DeviceTask m_DeviceTask[MAX_AGITATION_DEVICE_TASK_ID];  //!< Device task control

    //! Error task state definition
    typedef enum {
        AGITATION_ERRTASK_STATE_FREE           = 0x00,  ///< task state free, no error
        AGITATION_ERRTASK_STATE_REPORT_IFACE   = 0x01,  ///< reports the error to the interface class
        AGITATION_ERRTASK_STATE_REPORT_DEVPROC = 0x02,  ///< reports the error to device processing
        AGITATION_ERRTASK_STATE_IDLE           = 0x03,  ///< error idle state, do nothing, block anything
        AGITATION_ERRTASK_STATE_RESET          = 0x04   ///< reset error state, change to initial state
    } AgitationErrTaskState_t;

    AgitationErrTaskState_t m_ErrorTaskState;   //!< Error task state

    // Variables from process settings
    MotionProfileIdx_t m_AgitationMotionProfile;    //!< Motor speed motion profile for agitation
};

} //namespace

#endif /* AGITATIONDEVICE_H */

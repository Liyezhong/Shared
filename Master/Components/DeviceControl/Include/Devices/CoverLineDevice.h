/****************************************************************************/
/*! \file CoverLineDevice.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    14.01.2011
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declarationof the class CCoverLineDevice
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

#ifndef COVERLINEDEVICE_H
#define COVERLINEDEVICE_H

#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl
{

#define CL_MOTOR_ELEVATOR     0  ///< slide elevator axis
#define CL_MOTOR_SLIDE        1  ///< slide shift axis
#define CL_MOTOR_NEEDLE       2  ///< EDM needle axis
#define CL_MOTOR_CLAMP        3  ///< slide clamp axis
#define CL_MOTOR_CS_Y         4  ///< cover slip handling y axis
#define CL_MOTOR_CS_Z         5  ///< cover slip handling z axis
#define CL_MOTOR_EDM_PUMP     6  ///< EDM pump
#define COVERLINE_MOTOR_MAX   7  ///< number of motors at the device

class DeviceProcessing;
class CStepperMotor;
class CDigitalInput;
class CDigitalOutput;
class CAnalogOutput;
class CCoverLineNavigator;

///< device tasks
#define DEVICE_TASK_REFRUN                  1  ///< 'ReferenceRun'
#define DEVICE_TASK_BASIS_POS               2  ///< move all axis to basis position
#define DEVICE_TASK_RACK_POS                3  ///< rack positioning to enable slide shift-out
#define DEVICE_TASK_SLIDE_SHIFT_OUT         4  ///< slide shift-out with EDM
#define DEVICE_TASK_COVER_SLIP_GRAB         5  ///< coverslip grabbing
#define DEVICE_TASK_SLIDE_SHIFT_BACK        6  ///< shift back slide to elevator
#define DEVICE_TASK_COVER_SLIP_PREP_COV     7  ///< unused
#define DEVICE_TASK_COVER_SLIP_EXEC_COVER   8  ///< execute cover process
#define DEVICE_TASK_COVER_SLIP_DISCARD      9  ///< discard a 'broken' coverslip
#define MAX_DEVICE_TASK_ID                  9  ///< maximum value

///< function module tasks within in device task 'ReferenceRun'
#define REFRUN_FCT_TASK_CLAMP    0         ///< reference run clamp axis
#define REFRUN_FCT_TASK_NEEDLE   1         ///< reference run needle axis
#define REFRUN_FCT_TASK_SLIDE    2         ///< reference run slide shift axis
#define REFRUN_FCT_TASK_ELEVATOR 3         ///< reference run elevator axis
#define REFRUN_FCT_TASK_CS_Z     4         ///< reference run coverslip transport z-axis
#define REFRUN_FCT_TASK_CS_Y     5         ///< reference run coverslip transport y-axis

///< function module tasks within in device task 'Basis position
#define FCT_TASK_ID_BASIS_POS_REQ_MOTOR_CLAMP      0  //!< request clamp motor position
#define FCT_TASK_ID_BASIS_POS_REQ_MOTOR_NEEDLE     1  //!< request needle motor position
#define FCT_TASK_ID_BASIS_POS_REQ_MOTOR_SLIDE      2  //!< request slide shifter motor position
#define FCT_TASK_ID_BASIS_POS_REQ_MOTOR_ELEVATOR   3  //!< request elevator motor position
#define FCT_TASK_ID_BASIS_POS_REQ_MOTOR_Z          4  //!< request z motor position
#define FCT_TASK_ID_BASIS_POS_REQ_MOTOR_Y          5  //!< request y motor position
#define FCT_TASK_ID_BASIS_POS_MOTOR_CLAMP          6  //!< position task for clamp motor
#define FCT_TASK_ID_BASIS_POS_MOTOR_NEEDLE         7  //!< position task for needle motor
#define FCT_TASK_ID_BASIS_POS_MOTOR_SLIDE          8  //!< position task for slide shifter motor
#define FCT_TASK_ID_BASIS_POS_MOTOR_ELEVATOR       9  //!< position task for elevator motor
#define FCT_TASK_ID_BASIS_POS_MOTOR_Z             10  //!< position task for z motor
#define FCT_TASK_ID_BASIS_POS_MOTOR_Y             11  //!< position task for y motor

///< function module tasks within in device task 'rack positioning'
#define FCT_TASK_ID_RACK_POS_MOTOR_ELEVATOR        0  //!< position task for elevator motor
#define FCT_TASK_ID_RACK_POS_DIG_INP_SLIDE_DETECT  1  //!< read digital input 'slide detection'


///< function module tasks within in device task 'Cover slop grab'
#define FCT_TASK_ID_CS_GRAB_MOTOR_CS_Y_GRAB_POS  0  ///< move y axis to coverslip depot
#define FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_DOWN      1  ///< move z axis down to the coverslip depot
#define FCT_TASK_ID_CS_GRAB_VACUUM_PUMP_ON       2  ///< vacuum pump on
#define FCT_TASK_ID_CS_GRAB_VACUUM_VALVE_1_ON    3  ///< vacuum valve 1 on
#define FCT_TASK_ID_CS_GRAB_VACUUM_VALVE_2_ON    4  ///< vacuum valve 2 on
#define FCT_TASK_ID_CS_GRAB_VACUUM_SENSOR_ON     5  ///< check the vacuum sensor for coverslip contact detection
#define FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_STOP      6  ///< stop z-axis
#define FCT_TASK_ID_CS_GRAB_MOTOR_CS_Z_UP        7  ///< move z-axis up
#define FCT_TASK_ID_CS_GRAB_HALL_VALUE           8  ///< check coverslip via hallsensor
#define FCT_TASK_ID_CS_GRAB_MOTOR_CS_Y_COVER_POS 9  ///< move coverslip via y-axis to cover position

///< function module tasks within in device task 'Covering'

///< function module tasks within in device task 'discard coverslip'
#define FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Y_DISCARD_POS 0  ///< move coverslip via y-axis to discard position
#define FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Z_DOWN        1  ///< move z-axis down to discard coverslip
#define FCT_TASK_ID_CS_DISCARD_VACUUM_VALVE_1_OFF     2  ///< switch off vacuum valve 1 to discard coverslip
#define FCT_TASK_ID_CS_DISCARD_VACUUM_VALVE_2_OFF     3  ///< switch off vacuum valve 2 to discard coverslip
#define FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Z_UP          4  ///< move z-axis up (now without coverslip)
#define FCT_TASK_ID_CS_DISCARD_MOTOR_CS_Y_DEPOT_POS   5  ///< move y-axis to coverslip depot position


/****************************************************************************/
/*! \class CoverSlipControl
 *
 *  \brief This class contains simple data used by the cover process
 *
 */
/****************************************************************************/
class CoverSlipControl
{
public:
    CoverSlipControl() { m_SubStateCover = 0;
                         m_ConditionCoverslipOK = false;
                         m_ConditionCoverslipFailed = false;
                         m_ConditionClampOK = false;
                       };

    quint8 m_SubStateCover;             ///< sub state of the cover process
    bool   m_ConditionCoverslipOK;      ///< identifies a coverslip ready for cover (hall sensor OK)
    bool   m_ConditionCoverslipFailed;  ///< identifies a damaged coverslip (hall sensor not OK)
    bool   m_ConditionClampOK;          ///< identifies a clamped slide, ready for cover

};


/****************************************************************************/
/*! \class CCoverLineDevice
 *
 *  \brief This is the class to control the cover line at sepia device
 *
 *         The class provides functions to start, control and interrupt the
 *         cover procedure of a rack
 */
/****************************************************************************/
class CCoverLineDevice : public CBaseDevice
{
    friend class CCoverLineNavigator;
    Q_OBJECT

public:
    CCoverLineDevice(DeviceProcessing* pDeviceProcessing, QString Type, DevInstanceID_t InstanceID);
    virtual ~CCoverLineDevice();

    void HandleTasks();

    ReturnCode_t ReqReferenceRun();
    ReturnCode_t ReqCoverProcedure(RackType_t RackType);

private slots:
     void StepperMotorReferenceMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);
     void StepperMotorMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position, qint16 speed);
     void StepperMotorPosition(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);
     void StepperMotorError(quint32, quint16, quint16, quint16, QDateTime);
     void AnalogInputError(quint32, quint16, quint16, quint16, QDateTime);
     void AnalogInputChanged(quint32, ReturnCode_t, quint16);
     //void AnalogOutputValue(quint32, ReturnCode_t, quint16);
     //void AnalogOutputError(quint32, quint16, quint16, quint16, QDateTime);
     void DigInputError(quint32, quint16, quint16, quint16, QDateTime);
     void DigInputChanged(quint32, ReturnCode_t, quint16);
     void DigOutputError(quint32, quint16, quint16, quint16, QDateTime);
     void DigOutputValue(quint32, ReturnCode_t);


private:
    //!< Initialisation state task handling function
    ReturnCode_t HandleInitializationState();
    //!< Configuration state task handling function
    ReturnCode_t HandleConfigurationState();
    //!< Configuration state handling function for device tasks
    ReturnCode_t HandleDeviceTasksConfigurationState();

    //!< Handles motors configuration state
    void HandleMotorConfigurationState();
    //!< Idle state task handling function
    void HandleIdleState();
    //!< Error state task handling function
    void HandleErrorState();

    //!< Reference run task handling function
    void HandleReferenceRunTask();
    //!< Coverp process task handling function
    void HandleCoverTask();

    //!< Reset all active tasks to idle state
    void ResetTask();

    //!< check tasks pending for beeing started
    void CheckTasksForStartup(bool& bActionStateInProgress, bool& bActionStateWithErrorDetect);

    //!< Forwards the slot information of a function module's task to the device task handler
    void ForwardTaskToDeviceTaskHandler(int DeviceTaskIndex, FunctionModuleTask* pFunctionModuleTask);

    CCoverLineNavigator* m_pCoverLineNavigator;   ///< pointer to the navigator class

    CStepperMotor*  m_pMotorCoverLine[COVERLINE_MOTOR_MAX];  ///< pointer to the cover line motor function modules
    CDigitalInput* m_pDigInpSlideDetection;  ///< pointer to the digital input used for slide detection
    CDigitalInput* m_pDigInpEDMLevel;        ///< pointer to the digital input used for EDM level detection
    CDigitalOutput* m_pDigOutpVacuumPump;    ///< pointer to the digital output used to control the vacuum pump
    CDigitalOutput* m_pDigOutpVacuumValve1;  ///< pointer to the digital output used to control vacuum valve 1
    CDigitalOutput* m_pDigOutpVacuumValve2;  ///< pointer to the digital output used to control vacuum valve 2
    CAnalogInput* m_pAnaInpPressureSensor;   ///< pointer to the analog input connected to the pressure sensor
    CAnalogInput* m_pAnaInpHallSensor1;      ///< pointer to the analog input connected to the hall sensor 1


    /*! Main device tasks definitions */
    typedef enum { CL_TASK_REFRUN,     //!< reference run
                   CL_TASK_BASIS_POS,  //!< basic positioning
                   CL_TASK_RACK_POS    //!< rack positioning
    } DeviceTaskID_t;

    DeviceTaskMap m_DeviceTaskMap;   ///< device task map of this device

    /*! cover line device tasks definitions */
    typedef enum { COVERLINE_DEV_TASKID_INIT           = 0x00,  ///< initialization state, requests are not accepted
                   COVERLINE_DEV_TASKID_FREE           = 0x01,  ///< free, a new request will be accepted
                   COVERLINE_DEV_TASKID_REFRUN         = 0x03,  ///< reference run was requested
                   COVERLINE_DEV_TASKID_COVER          = 0x04,  ///< request cover procedure
                   COVERLINE_DEV_TASKID_REQ_ACTPOS     = 0x05,  ///< request actual agitation position
                   COVERLINE_DEV_TASKID_REQ_ACTSPEED   = 0x06,  ///< request actual agitation speed
    } CoverLineDevTaskID_t;

    /*! task states definitions */
    typedef enum { COVERLINE_DEV_TASK_STATE_FREE                   = 0x00,  ///< task state free, ready for action request
                   COVERLINE_DEV_TASK_STATE_REQ                    = 0x01,  ///< a task was requested, next step will be to forward the command via CAN
                   COVERLINE_DEV_TASK_STATE_ACTPOS_REQ_SEND        = 0x02,  ///< the command to request the actual motor positions was
                                                                            ///< forwarded via CAN, wait for motor positions being received by CAN
                   COVERLINE_DEV_TASK_STATE_ACTPOS_REQ_ACKN        = 0x03,  ///< the motor positions were received,
                                                                            ///< next step: calculate the single steps which split the action
                   COVERLINE_DEV_TASK_STATE_PROGRESS_ACTION_STEPS  = 0x04,  ///< progress the steps
                   COVERLINE_DEV_TASK_STATE_ACTION_STEPS_ACKN      = 0x05,  ///< acknowledge the execution
                   COVERLINE_DEV_TASK_STATE_ERROR                  = 0x06   ///< error, e.g. timeout while waiting for acknowledge, or error CAN-message received
    } CoverLineDevTaskState_t;

    /*! error task state definitions */
    typedef enum { COVERLINE_DEV_ERRTASK_STATE_FREE            = 0x00,  ///< task state free, no error
                   COVERLINE_DEV_ERRTASK_STATE_REPORT_IFACE    = 0x01,  ///< reports the error to the interface class
                   COVERLINE_DEV_ERRTASK_STATE_REPORT_DEVPROC  = 0x02,  ///< reports the error to device processing
                   COVERLINE_DEV_ERRTASK_STATE_IDLE            = 0x03,  ///< error idle state, do nothing, block anything
                   COVERLINE_DEV_ERRTASK_STATE_RESET           = 0x04,  ///< reset error state, change to initial state
    } CoverLineDevErrTaskState_t;

    CoverLineDevTaskID_t       m_taskID;        ///< task requested to the agitation
    CoverLineDevTaskState_t    m_taskState;     ///< task state
    CoverLineDevErrTaskState_t m_errTaskState;  ///< error task state

    CoverSlipControl m_CoverSlipControl;  ///< cover slip process control datas

    RackType_t m_RackType;     ///< rack type, used for correct slide positioning

};

}  //namespace

#endif // COVERLINEDEVICE_H

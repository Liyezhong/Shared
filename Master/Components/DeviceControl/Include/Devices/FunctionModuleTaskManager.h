/****************************************************************************/
/*! \file FunctionModuleTaskManager.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 24.08.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CFunctionModuleTaskManager
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

#ifndef FUNCTIONMODULETASKMANAGER_H
#define FUNCTIONMODULETASKMANAGER_H

#include <QObject>
#include "Global/Include/MonotonicTime.h"
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"

namespace DeviceControl
{

class CGrapplerDevice;
class CStepperMotor;
class CRfid11785;
class CAnalogInput;
class FunctionModuleTask;

class DeviceTask;

/****************************************************************************/
/*! \class FunctionModuleTask
 *
 *  \brief The class implements data members to control a function module task.
 *
 */
/****************************************************************************/
class FunctionModuleTask
{
public:
    FunctionModuleTask() {
        m_pDeviceTask = 0;
        m_TaskIndex = TaskIndexUndef;
        m_StartTrigger = FCTMOD_TASK_START_UNDEF;
        m_TriggerTaskIndex = TaskIndexUndef;
        m_OffsetTime = 0;
        m_TaskID = FunctionModuleTask::FCTMOD_TASK_ID_UNDEF;
        m_TaskState = FunctionModuleTask::FCTMOD_STATE_UNUSED;
        m_pCANObject = 0;
        m_ErrorHdlInfo = DCL_ERR_FCT_CALL_SUCCESS;
        startTime = 0;
        m_Timeout = 0;
    }

    virtual ~FunctionModuleTask() { m_pCANObject = 0; m_pDeviceTask = 0; }

    /*! task identifier */
    typedef enum  {
        FCTMOD_TASK_ID_UNDEF      = 0x00,   //!< undefined
        FCTMOD_TASK_ID_DIG_INPUT  = 0x01,   //!< get digital input value
        FCTMOD_TASK_ID_DIG_OUTPUT = 0x02,   //!< set digital output value
        FCTMOD_TASK_ID_ANA_INPUT  = 0x03,   //!< get digital inpit value
        FCTMOD_TASK_ID_ANA_OUTPUT = 0x04,   //!< set digital inpit value
        FCTMOD_TASK_ID_MOTOR      = 0x05,   //!< execute the motor's reference run
        FCTMOD_TASK_ID_RFID       = 0x06,   //!< read the rack's RFID data
        FCTMOD_TASK_ID_TEMP_CTRL  = 0x07,   //!< temperature control task
        FCTMOD_TASK_ID_RFID_TAG   = 0x08    //!< read the rack's RFID data
    } FunctionModuleTaskID_t ;

    /*! task state */
    typedef enum {
        FCTMOD_STATE_UNUSED        = 0x00,  //!< the task is unused
        FCTMOD_STATE_INIT          = 0x01,  //!< initial state, the task is still waiting to be executed
        FCTMOD_STATE_WAIT_PROGRESS = 0x02,  //!< wait to be executed, only used by manual started tasks
        FCTMOD_STATE_PROGRESS      = 0x03,  //!< execution in progress, task waits for acknowledge
        FCTMOD_STATE_FINISHED      = 0x04,  //!< the execution has finished
        FCTMOD_STATE_ERROR         = 0x05   //!< error state, the task was not executed successfully
    } FunctionModuleTaskState_t;

    /*! task start condition  */
    typedef enum {
        FCTMOD_TASK_START_UNDEF             = 0x00, //!< undefined
        FCTMOD_TASK_START_FIRST             = 0x01, //!< start this task first
        FCTMOD_TASK_START_PREVIOUS_FINISHED = 0x02, //!< start the task after a previous has finished
        FCTMOD_TASK_START_SIMULTANEOUS      = 0x03, //!< start the task simultaneous with another one
        FCTMOD_TASK_START_TIMEOFFSET        = 0x04, //!< start the task with a time offset to another one
        FCTMOD_TASK_START_MANUAL            = 0x05  //!< the start of the task will be triggered by software
    } FunctionModuleTaskStartTrigger_t;

    static const quint8 TaskIndexUndef = 0xff;  ///< undefined marker

    quint8 m_TaskIndex; ///< Task index

    /// The start condition of the task
    FunctionModuleTaskStartTrigger_t m_StartTrigger;

    /// The index of the task which triggers the start condition
    quint8 m_TriggerTaskIndex;
    /// Time offset this task will be started, if the start condition is GRAPPLER_DEV_TASK_START_TIMEOFFSET
    qint16 m_OffsetTime;

    /// Task identifier, identifies the assigned function module type
    FunctionModuleTaskID_t m_TaskID;
    /// Task state
    FunctionModuleTaskState_t m_TaskState;

    DeviceTask*            m_pDeviceTask;   ///< The device task this tasks belongs to
    CModule*        m_pCANObject;    ///< Functions module which handles the task
    ReturnCode_t           m_ErrorHdlInfo;  ///< Error code

    int startTime;  ///< Start time for timeout calculation
    int m_Timeout;  ///< Timout in ms
};

/****************************************************************************/
/*! \class CANStepperMotorTask
 *
 *  \brief The class implements data members to control a stepper motor's function module task.
 *
 */
/****************************************************************************/
class CANStepperMotorTask : public FunctionModuleTask
{
public:
    /****************************************************************************/
    /*!
     *  \brief    Constructor of CANStepperMotorTask
     *
     ****************************************************************************/
    CANStepperMotorTask() {
        m_MotorTaskID = FCTMOD_SM_TASK_ID_UNDEF;
        m_TargetPos = 0;
        m_ActPos = 0;
        m_TargetSpeed = 0;
        m_ActSpeed = 0;
        m_Profile = 0;
        m_SubCommandID = SM_SUBCMD_MOTION_NULL;
        m_SubCommandData = 0;
    }

    /****************************************************************************/
    /*!
     *  \brief    Destructor of CANStepperMotorTask
     *
     ****************************************************************************/
    ~CANStepperMotorTask() {}

    /*! stepper motor tasks definition */
    typedef enum {
        FCTMOD_SM_TASK_ID_UNDEF      = 0,   ///< undefined
        FCTMOD_SM_TASK_ID_STATE      = 1,   ///< Motor state (on/off)
        FCTMOD_SM_TASK_ID_REFRUN     = 2,   ///< Reference run
        FCTMOD_SM_TASK_ID_REQ_POS    = 3,   ///< Request motor position
        FCTMOD_SM_TASK_ID_REQ_SPEED  = 4,   ///< Request motor speed
        FCTMOD_SM_TASK_ID_MOVE_POS   = 5,   ///< Move to position
        FCTMOD_SM_TASK_ID_MOVE_SPEED = 6    ///< Move with speed
    } CANStepperMotorTaskID_t;

    CANStepperMotorTaskID_t m_MotorTaskID;  ///< Task identification 'what shall the task do'

    /// target position
    Position_t m_TargetPos;
    /// actual position
    Position_t m_ActPos;
    /// target speed
    Speed_t m_TargetSpeed;
    /// actual speed
    Speed_t m_ActSpeed;
    /// movement profile index
    MotionProfileIdx_t m_Profile;
    /// sub command identifier
    StepperMotorSubCommandMotion_t m_SubCommandID;
    /// sub command data
    quint16 m_SubCommandData;
};

/****************************************************************************/
/*! \class CANRFIDTask
 *
 *  \brief The class implements data members to control a RFID function module task.
 *
 */
/****************************************************************************/
class CANRFIDTask : public FunctionModuleTask
{
public:
    /****************************************************************************/
    /*!
     *  \brief    Constructor for the CANRFIDTask class
     *
     ****************************************************************************/
    CANRFIDTask() {
        m_UID = 0;
        m_Address = 0;
        m_RFIDData = 0;
        m_RFIDTaskID = FCTMOD_RFID_TASK_ID_UNDEF;
    }

    /****************************************************************************/
    /*!
     *  \brief    Destructor of CANRFIDTask
     *
     ****************************************************************************/
    ~CANRFIDTask() {}

    /*! stepper motor tasks definition */
    typedef enum {
        FCTMOD_RFID_TASK_ID_UNDEF    = 0,   ///< undefined
        FCTMOD_RFID_TASK_ID_REQ_DATA = 1,   ///< Read RFID data
        FCTMOD_RFID_TASK_ID_REQ_TAG  = 2    ///< Read RFID TAG
    } CANRFIDTaskID_t;

    CANRFIDTaskID_t m_RFIDTaskID;   ///< Task identification 'what shall the task do'

    quint32 m_UID;      ///< Unique ID of the RFID tag
    quint8 m_Address;   ///< RFID address
    quint32 m_RFIDData; ///< RFID data
};

/****************************************************************************/
/*!
 *  \brief  The class implements data members to control a temperature
 *          control function module task.
 */
/****************************************************************************/
class CANTemperatureCtrlTask : public FunctionModuleTask
{
public:
    /****************************************************************************/
    /*!
     *  \brief    Constructor for the CANTemperatureCtrlTask
     *
     ****************************************************************************/
    CANTemperatureCtrlTask() {
        m_TempCtrlTaskID = FCTMOD_TEMP_TASK_ID_UNDEF;
        m_Temperatur = 0;
        m_State = TEMPCTRL_STATUS_UNDEF;
        m_OperationMode = TEMPCTRL_OPMODE_UNDEF;
    }

    /****************************************************************************/
    /*!
     *  \brief    Destructor for the CANTemperatureCtrlTask
     *
     ****************************************************************************/
    ~CANTemperatureCtrlTask() {}

    /*! Temperature control tasks definition */
    typedef enum {
        FCTMOD_TEMP_TASK_ID_UNDEF      = 0, ///< undefined
        FCTMOD_TEMP_TASK_ID_SET_TEMP   = 1, ///< Set reference temperature
        FCTMOD_TEMP_TASK_ID_REQ_TEMP   = 2, ///< Request act. temp.
        FCTMOD_TEMP_TASK_ID_SET_STATE  = 3, ///< Set temperature ctrl. state
        FCTMOD_TEMP_TASK_ID_REQ_STATE  = 4, ///< Request temperature ctrl. state
        FCTMOD_TEMP_TASK_ID_SET_OPMODE = 5, ///< Set temperature ctrl. operating mode
        FCTMOD_TEMP_TASK_ID_REQ_OPMODE = 6  ///< Set temperature ctrl. operating mode
    } CANTempCtrlTaskID_t;

    CANTempCtrlTaskID_t m_TempCtrlTaskID;       ///< Task identification 'what shall the task do'
    quint16 m_Temperatur;                       ///< Reference temperature
    TempCtrlStatus_t m_State;                   ///< Temperature ctrl. state
    TempCtrlOperatingMode_t m_OperationMode;    ///< Operation mode
};

/****************************************************************************/
/*!
 *  \brief The class implements data members to control a digital input function module task.
 */
/****************************************************************************/
class CANDigitalInputTask : public FunctionModuleTask
{
public:
    CANDigitalInputTask() {
        m_DigInpTaskID = FCTMOD_DI_TASK_ID_UNDEF;
        m_Value = 0;
    }

    ~CANDigitalInputTask() {}

    /*! digital input tasks definition */
    typedef enum {
        FCTMOD_DI_TASK_ID_UNDEF      = 0,   ///< undefined
        FCTMOD_DI_TASK_ID_REQ_VALUE  = 1,   ///< request input value
        FCTMOD_DI_TASK_ID_WAIT_VALUE = 2    ///< wait for input value notification
    } CANDigitalInputTaskID_t;

    CANDigitalInputTaskID_t m_DigInpTaskID; ///< Task identification 'what shall the task do'

    quint16 m_Value;    ///< input value
};

/****************************************************************************/
/*!
 *  \brief The class implements data members to control a digital output function module task.
 */
/****************************************************************************/
class CANDigitalOutputTask : public FunctionModuleTask
{
public:
    CANDigitalOutputTask() {
        m_DigOutpTaskID = FCTMOD_DO_TASK_ID_UNDEF;
        m_Value = 0;
    }

    ~CANDigitalOutputTask() {}

    /// Task identification 'what shall the task do'
    typedef enum {
        FCTMOD_DO_TASK_ID_UNDEF         = 0,    ///< undefined
        FCTMOD_DO_TASK_ID_SET_VALUE     = 1,    ///< set output value
        FCTMOD_DO_TASK_ID_REQ_ACT_VALUE = 2     ///< request output value
    } CANDigitalOutputTaskID_t;

    CANDigitalOutputTaskID_t m_DigOutpTaskID;   ///< Task identification 'what shall the task do'

    quint16 m_Value;    ///< output value
};

/****************************************************************************/
/*! \class CANAnalogInputTask
 *
 *  \brief The class implements data members to control a analog intput function module task.
 *
 */
/****************************************************************************/
class CANAnalogInputTask : public FunctionModuleTask
{
public:
    CANAnalogInputTask() {
        m_AnaInpTaskID = FCTMOD_AI_TASK_ID_UNDEF;
        m_Value = 0;
    }

    ~CANAnalogInputTask() {}

    /*! analog input tasks definition */
    typedef enum {
        FCTMOD_AI_TASK_ID_UNDEF            = 0, ///< undefined
        FCTMOD_AI_TASK_ID_REQ_ACT_VALUE    = 1, ///< request actual input value
        FCTMOD_AI_TASK_ID_WAIT_VALUE_ABOVE = 2, ///< wait for high limit value notification
        FCTMOD_AI_TASK_ID_WAIT_VALUE_BELOW = 3  ///< wait for high limit value notification
    } CANAnalogInputTaskID_t;

    CANAnalogInputTaskID_t m_AnaInpTaskID;  ///< Task identification 'what shall the task do'

    quint16 m_Value;    ///< input value
};

/*! task map for movement and measurement actions */
typedef QMap<quint8, FunctionModuleTask*> FunctionModuleTaskMap;

/****************************************************************************/
/*!
 *  \brief The class implements a container for several function module tasks
 *
 *      The class provides a function module task list to arrange tasks need
 *      for complex actions together. The tasks are stored in a list of type
 *      FunctionModuleTaskList E.g. all tasks for a grappler's reference run
 *      are arranged into the list.
 */
/****************************************************************************/
class DeviceTask
{
public:
    DeviceTask();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CreateFunctionModuleTask
     *
     *  \return from CreateFunctionModuleTask
     */
    /****************************************************************************/
    FunctionModuleTask* CreateFunctionModuleTask();

    /*! task state type definition */
    typedef enum {
        DEV_TASK_STATE_UNUSED   = 0x00, //!< the task is unused
        DEV_TASK_STATE_INIT     = 0x01, //!< initial state, the task is still waiting to be executed
        DEV_TASK_STATE_START    = 0x02, //!< start state, triggers the beginning of the execution
        DEV_TASK_STATE_PROGRESS = 0x03, //!< execution in progress, task waits for acknowledge
        DEV_TASK_STATE_FINISHED = 0x04, //!< the execution has finished
        DEV_TASK_STATE_FINISHED_WITH_INFO = 0x05,   //!< the execution has finished, but there are further information (e.g. error, warning)
        DEV_TASK_STATE_ERROR    = 0x06  //!< error state, the task was not executed successfully
    } DeviceTaskState_t;

    /*! task info definition */
    typedef enum {
        DEV_TASK_INFO_UNUSED                     = 0x00,    //!< the task is unused
        DEV_TASK_STATE_CS_GRAB_COVERSLIP_BROCKEN = 0x01,    //!< initial state, the task is still waiting to be executed
        DEVICE_TASK_STATE_RACK_POS_NO_SLIDE      = 0x02,    //!< start state, triggers the beginning of the execution
        DEV_TASK_INFO_PROGRESS                   = 0x03,    //!< execution in progress, task waits for acknowledge
        DEV_TASK_INFO_FINISHED                   = 0x04,    //!< the execution has finished
        DEV_TASK_INFO_FINISHED_WITH_INFO         = 0x05,    //!< the execution has finished, but there are further information (e.g. error, warning)
        DEV_TASK_INFO_ERROR                      = 0x06     //!< error state, the task was not executed successfully
    } DeviceTaskInfo_t;

    quint16                m_Key;               ///< The key of the task
    DeviceTaskState_t      m_TaskState;         ///< Task state
    FunctionModuleTaskMap  m_FctModTaskMap;     ///< Map which contions the module tasks

    DeviceTaskInfo_t       m_TaskInfo;          ///< Task inormation

    Global::MonotonicTime  m_timeAction;        ///< Action start time, for timeout detection
    qint16                 m_aktionTimespan;    ///< Delay im ms, for timeout detection
};

/*! map for device tasks */
typedef QMap<quint16, DeviceTask*> DeviceTaskMap;

/****************************************************************************/
/*!
 *  \brief The class handles the access functionality to a task list
 *
 *      The class provides functions for task access, need e.g. by the
 *      grappler device while an action is executed. The tasks are stored in
 *      a list of type FunctionModuleTaskList.
 */
/****************************************************************************/
class CFunctionModuleTaskManager : public QObject
{
    Q_OBJECT

public:
    CFunctionModuleTaskManager();
    ~CFunctionModuleTaskManager();

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetStartupTask
     *
     *  \param taskMap = FunctionModuleTaskMap type parameter
     *  \param bActionStateInProgress =  bool type parameter
     *  \param bActionStateWithErrorDetect =  bool type parameter
     *
     *  \return from GetStartupTask
     */
    /****************************************************************************/
    FunctionModuleTask* GetStartupTask(FunctionModuleTaskMap& taskMap, bool& bActionStateInProgress, bool& bActionStateWithErrorDetect);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetTaskFromIndex
     *
     *  \param taskMap = FunctionModuleTaskMap type parameter
     *  \param taskIndex =  quint8 type parameter
     *
     *  \return from GetTaskFromIndex
     */
    /****************************************************************************/
    FunctionModuleTask* GetTaskFromIndex(FunctionModuleTaskMap& taskMap, quint8 taskIndex);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function GetTaskFromInstanceID
     *
     *  \param taskMap = FunctionModuleTaskMap type parameter
     *  \param instanceID =  quint32 type parameter
     *  \param pLastFunctionModuleTask =  FunctionModuleTask type parameter
     *
     *  \return from GetTaskFromInstanceID
     */
    /****************************************************************************/
    FunctionModuleTask* GetTaskFromInstanceID(FunctionModuleTaskMap& taskMap, quint32 instanceID, FunctionModuleTask* pLastFunctionModuleTask);

    FunctionModuleTask* GetTaskFromInstanceID(DeviceTaskMap& taskList, quint32 instanceID, FunctionModuleTask* pLastFunctionModuleTask, int& DeviceTaskKey);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function StartTask
     *
     *  \param pFunctionModuleTask = FunctionModuleTask type parameter
     *
     *  \return from StartTask
     */
    /****************************************************************************/
    ReturnCode_t StartTask(FunctionModuleTask* pFunctionModuleTask);

    static FunctionModuleTask* GetTaskWithState(FunctionModuleTaskMap& taskMap,
                                                FunctionModuleTask::FunctionModuleTaskID_t taskID,
                                                FunctionModuleTask::FunctionModuleTaskState_t taskState);

    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function CheckTasksTimeOut
     *
     *  \param TaskMap = FunctionModuleTaskMap type parameter
     *  \param pFctModTaskTimeout =  FunctionModuleTask type parameter
     *
     *  \return from CheckTasksTimeOut
     */
    /****************************************************************************/
    ReturnCode_t CheckTasksTimeOut(FunctionModuleTaskMap& TaskMap, FunctionModuleTask* pFctModTaskTimeout);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of function ResetTasks
     *
     *  \param TaskMap = FunctionModuleTaskMap type parameter
     *
     *  \return from ResetTasks
     */
    /****************************************************************************/
    void ResetTasks(FunctionModuleTaskMap& TaskMap);

    Global::MonotonicTime m_Timer;  ///< timer for timeout observation
};

} //namespace

#endif /* FUNCTIONMODULETASKMANAGER_H */

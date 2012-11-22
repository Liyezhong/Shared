/****************************************************************************/
/*! \file RackHandlingDevice.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 14.02.2011
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CRackHandlingDevice
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef RACKHANDLINGDEVICE_H
#define RACKHANDLINGDEVICE_H

#include <QObject>
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"

namespace DeviceControl
{

class CRackHandlingNavigator;
class CStepperMotor;

#define RACKHDL_DEV_TASK_REFRUN            0   ///< reference run
#define RACKHDL_DEV_TASK_POS_REQ           1   ///< all motor position request
#define RACKHDL_DEV_TASK_TRANSPORT         2   ///< transport process
#define MAX_RACKHDL_DEVICE_TASK_ID         2   ///< maximum id

///< function module tasks within in device task 'ReferenceRun'
#define FCT_TASK_ID_REFRUN_FCT_TASK_X_AXIS   0   ///< reference run x-axis
#define FCT_TASK_ID_REFRUN_FCT_TASK_Y_AXIS   1   ///< reference run y-axis
#define FCT_TASK_ID_REFRUN_FCT_TASK_Z_AXIS   2   ///< reference run z-axis
#define FCT_TASK_ID_REFRUN_FCT_TASK_GRABBER  3   ///< reference run grabber axis

#define FCT_TASK_ID_POSREQ_FCT_TASK_X_AXIS  0       //!< TaskID position request x-axis
#define FCT_TASK_ID_POSREQ_FCT_TASK_Y_AXIS  1       //!< TaskID position request y-axis
#define FCT_TASK_ID_POSREQ_FCT_TASK_Z_AXIS  2       //!< TaskID position request z-axis
#define FCT_TASK_ID_POSREQ_FCT_TASK_GRABBER_AXIS 3  //!< TaskID position request grabber-axis

/****************************************************************************/
/*! \class RackHdlStationPos
 *
 *  \brief This class contain the position data used by the rack handling process
 *
 */
/****************************************************************************/
class RackHdlStationPos
{
public:
    QString    Key;        //!< Station key
    Position_t PositionX;  //!< x-motor position in half steps
    Position_t PositionY;  //!< z-motor position in half steps
    Position_t PositionZ;  //!< z-motor position in half steps
    bool       Accessible; //!< flag for 'Station is accessable' state
};

/****************************************************************************/
/*! \class RackHdlStationPosAdjust
 *
 *  \brief This class contain adjustment data used by the rack handling process
 *
 */
/****************************************************************************/
class RackHdlStationPosAdjust
{
public:
    Position_t PositionX;   //!< adjustment offset of x position in half steps
    Position_t PositionY;   //!< adjustment offset of y position in half steps
    Position_t PositionZ;   //!< adjustment offset of z position in half steps
};


/*! task map for station positions */
typedef QMap<RackHdlStation_t, RackHdlStationPos*> RackHdlStationPosMap;
/*! task map for station adjustment positions */
typedef QMap<RackHdlStation_t, RackHdlStationPosAdjust*> RackHdlStationAdjustPosMap;

/****************************************************************************/
/*! \class CRackHandlingDevice
 *
 *  \brief This is the class to control the rack handling
 *
 *         The class provides functions to position the rack handling device the required
 *         actions like
 *          - transport a rack
 */
/****************************************************************************/
class CRackHandlingDevice : public CBaseDevice
{
    friend class CRackHandlingNavigator;
    Q_OBJECT

public:
    CRackHandlingDevice(DeviceProcessing* pDeviceProcessing, QString Type);
    virtual ~CRackHandlingDevice();

    void HandleTasks();

    ReturnCode_t ReqReferenceRun();
    ReturnCode_t ReqTransportAction(RackHdlStation_t RackHdlStation, StationAction_t stationAktion);

private slots:
    void StepperMotorReferenceMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);
    void StepperMotorMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position, qint16 speed);
    void StepperMotorPosition(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);
    void StepperMotorError(quint32, quint16, quint16, quint16, QDateTime);

private:
    ReturnCode_t HandleInitializationState();
    ReturnCode_t HandleConfigurationState();
    ReturnCode_t HandleDeviceTasksConfigurationState();

    void HandleIdleState();
    void HandleErrorState();

    void HandleReferenceRunTask();
    void HandleActionTask();
    void ResetTask();
    void SetTaskStateToError();

    void CheckTasksForStartup(bool& bActionStateInProgress, bool& bActionStateWithErrorDetect);
    void ForwardTaskToDeviceTaskHandler(int DeviceTaskIndex, FunctionModuleTask* pFunctionModuleTask);
    void CalcMovementsFromAction();
    void GetTargetPositions(Position_t posX, Position_t posY, RackHdlStation_t TargetStation);
    //void CorrectMotorPosAccordingAction(qint32 m_nextStationPosX, qint32 m_nextStationPosY, StationAction_t m_actStation);
    void GetMotorPositionsFromStation(RackHdlStation_t Station, Position_t& MotorPosXAxis, Position_t& MotorPosYAxis, Position_t& MotorPosZAxis, Position_t& MotorPosGrabber);
    RackHdlStation_t GetRackHdlStationTypeFromKey(QString Key);

    ReturnCode_t GetStationPositions(RackHdlStation_t StationKey, Position_t& StationPosX, Position_t& StationPosY, Position_t& StationPosZ);

    ReturnCode_t ReadAdjustment();
    ReturnCode_t ReadStationLayout();

    CRackHandlingNavigator* m_pRackHandlingNavigator;   ///< pointer to the navigator class

    /*! Task id definitions */
    typedef enum {
        RACKHDL_DEV_TASKID_INIT       = 0x00,   ///< initialization state, requests are not accepted
        RACKHDL_DEV_TASKID_FREE       = 0x01,   ///< free, a new request will be accepted
        RACKHDL_DEV_TASKID_ERROR      = 0x02,   ///<
        RACKHDL_DEV_TASKID_REFRUN     = 0x03,   ///< reference run was requested
        RACKHDL_DEV_TASKID_ACTION_REQ = 0x04    ///< action was requested
    } RackHandlingDevTaskID_t;

    /*! Task state definitions */
    typedef enum {
        RACKHDL_DEV_TASK_STATE_FREE                  = 0x00,    ///< task state free, ready for action request
        RACKHDL_DEV_TASK_STATE_REQ                   = 0x01,    ///< an action was requested, next step will be to
                                                                // - request the reference runs, if STATION_DEV_TASKID_REFRUN
                                                                // - request actual motor positions, if STATION_DEV_TASKID_ACTION_REQ
        RACKHDL_DEV_TASK_STATE_ACTPOS_REQ_SEND       = 0x02,    ///< the command to request the actual motor positions was
                                                                // forwarded via CAN, wait for motor positions being received by CAN
        RACKHDL_DEV_TASK_STATE_ACTPOS_REQ_ACKN       = 0x03,    ///< the motor positions were received,
                                                                // next step: calculate the single steps which split the action
        RACKHDL_DEV_TASK_STATE_PROGRESS_ACTION_STEPS = 0x04,    ///< progress the steps
        RACKHDL_DEV_TASK_STATE_ACTION_STEPS_ACKN     = 0x05,    ///< acknowledge the execution
        RACKHDL_DEV_TASK_STATE_ERROR                 = 0x06     ///< error state
    } RackHandlingDevTaskState_t;

    /*! error task states */
    typedef enum {
        RACKHDL_DEV_ERRTASK_STATE_FREE           = 0x00,    ///< task state free, no error
        RACKHDL_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,    ///< reports the error to the interface class
        RACKHDL_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,    ///< reports the error to device processing
        RACKHDL_DEV_ERRTASK_STATE_IDLE           = 0x03,    ///< error idle state, do nothing, block anything
        RACKHDL_DEV_ERRTASK_STATE_RESET          = 0x04     ///< reset error state, change to initial state
    } RackHandlingDevErrTaskState_t;

    RackHandlingDevTaskID_t m_taskID;               ///< task requested to the loader
    RackHandlingDevTaskState_t m_taskState;         ///< task state
    RackHandlingDevErrTaskState_t m_errTaskState;   ///< the error task state

    CStepperMotor* m_pMotorXAxis;   ///< pointer to the x-axis motor function module
    CStepperMotor* m_pMotorYAxis;   ///< pointer to the y-axis motor function module
    CStepperMotor* m_pMotorZAxis;   ///< pointer to the z-axis motor function module
    CStepperMotor* m_pMotorGrabber; ///< pointer to the grabber motor function module

    DeviceTaskMap m_DeviceTaskMap;  ///< device task control

    RackHdlStationPosMap m_MapStationPos;               ///< task map for station positions
    RackHdlStationAdjustPosMap m_MapStationAdjustPos;   ///< task map for station adjustment positions

    //requested station action parameters
    RackHdlStation_t m_ReqStation;      ///< target station (where should the action be done)
    StationAction_t m_ReqStationAktion; ///< target action (what action should be done)
};

}  //namespace

#endif // RACKHANDLINGDEVICE_H

/****************************************************************************/
/*! \file GrapplerDevice.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaation of the class CGrapplerDevice
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

#ifndef GRAPPLERDEVICE_H
#define GRAPPLERDEVICE_H

#include <QObject>
#include "DeviceControl/Include/DeviceProcessing/DeviceProcessing.h"
#include "DeviceControl/Include/Devices/BaseDevice.h"
#include "DeviceControl/Include/Devices/FunctionModuleTaskManager.h"

class WrapperDeviceGrappler;

namespace DeviceControl
{

class CANCommunicator;
class CStepperMotor;
class CRfid11785;
class CAnalogInput;
class GrapplerDevTask;
class CGrapplerNavigator;

///< Station type definition
typedef qint8  Station;

/*! Station position data */
typedef struct {
    qint32 positionX;   //!< x position in half steps
    qint32 positionY;   //!< y position in half steps
    bool   accessible;  //!< flag for accessibility
} StationPosition_t;

/*! Station adjustment data */
typedef struct {
    qint32 positionX;   //!< adjustment offset of y position in half steps
    qint32 positionY;   //!< adjustment offset of x position in half steps
} StationPositionAdjust_t;


#define MAX_GRAPPLER_TASK_COUNT 10   //!< maximum tasks number

#define GRAPPLER_DEV_TASK_REFRUN            0   //!< reference run
#define GRAPPLER_DEV_TASK_POS_REQ           1   //!< motor position request
#define GRAPPLER_DEV_TASK_GRAB              2   //!< grap the rack
#define GRAPPLER_DEV_TASK_READ_RFID         3   //!< RFID data addess
#define GRAPPLER_DEV_TASK_READ_LIQUID_LEVEL 4   //!< Read liquid level
/// \todo 4 or 5?
#define MAX_GRAPPLER_DEVICE_TASK_ID         4   //!< Maximum number of grappler tasks

/****************************************************************************/
/*! \class CGrapplerDevice
 *
 *  \brief This is the class to control the grappler
 *
 *         The class provides functions to position the grappler and handle the required
 *         actions like
 *          - grab a rack
 *          - discard a rack
 *          - measure liquid lever
 *          - read RFID tag
 */
/****************************************************************************/
class CGrapplerDevice : public CBaseDevice
{
    friend class ::WrapperDeviceGrappler;
    friend class CGrapplerNavigator;
    Q_OBJECT

public:
    CGrapplerDevice(DeviceProcessing* pDeviceProcessing, QString Type, DevInstanceID_t instanceID);
    virtual ~CGrapplerDevice();

    void GetStationXYPositionFromRowCol(qint16 stationCol, qint16 stationRow, StationAction_t stationAction, Position_t &posX, Position_t &posY);

    //!< Task handling function
    void HandleTasks();

    ReturnCode_t ReqReferenceRun();
    ReturnCode_t ReqStationAction(qint16 nStationColumn, qint16 nStationRow, StationAction_t stationAktion);
    ReturnCode_t ReqStationActionTime(qint16 stationColumn, qint16 stationRow, StationAction_t stationAktion);

    void GetMovementInfo();

private slots:
    void StepperMotorSetStateAckn(quint32 InstanceID, ReturnCode_t HdlInfo);
    void StepperMotorReferenceMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);
    void StepperMotorMovementAckn(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position, qint16 speed);
    void StepperMotorPosition(quint32 instanceID, ReturnCode_t hdlInfo, qint32 position);
    void StepperMotorError(quint32, quint16, quint16, quint16, QDateTime);

    void RFIDDataReceived(quint32 instanceID, ReturnCode_t hdlInfo, quint8 Address, quint32 rfidDataMSB);
    void RFIDError(quint32, quint16, quint16, quint16, QDateTime);

    void AnalogInputChanged(quint32 instanceID, ReturnCode_t hdlInfo, quint16 inputValue);
    void AnalogInputError(quint32, quint16, quint16, quint16, QDateTime);

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

    void CalcMovementsFromAction();
    void GetStationPositions(Position_t posX, Position_t posY, Station m_nextStation);
    void GetZPositionFromAction(Position_t& posZ, StationAction_t action);
    void CorrectMotorPosAccordingAction(qint32 m_nextStationPosX, qint32 m_nextStationPosY, StationAction_t m_actStation);

    CANStepperMotorTask* GetMotorFctModTaskInProcess(quint32 InstanceID,
                                                     CANStepperMotorTask* pMotorTaskPrev,
                                                     CANStepperMotorTask::CANStepperMotorTaskID_t FctTaskID);

    ReturnCode_t ReadAdjustment();
    ReturnCode_t ReadStationLayout();

    CGrapplerNavigator* m_pGrapplerNavigator;   ///< pointer to the navigator class

    /*! task identifiaction definitions */
    typedef enum {
        GRAPPLER_DEV_TASKID_INIT            = 0x00, ///< initialization state, requests are not accepted
        GRAPPLER_DEV_TASKID_FREE            = 0x01, ///< free, a new request will be accepted
        GRAPPLER_DEV_TASKID_ERROR           = 0x02, ///< error state
        GRAPPLER_DEV_TASKID_REFRUN          = 0x03, ///< reference run was requested
        GRAPPLER_DEV_TASKID_ACTION_REQ      = 0x04, ///< action was requested
        GRAPPLER_DEV_TASKID_ACTION_TIME_REQ = 0x05  ///< time calculation of an action was requested
    } GrapplerDevTaskID_t;

    /*! task state definitions */
    typedef enum {
        GRAPPLER_DEV_TASK_STATE_FREE                  = 0x00,  ///< task state free, ready for action request
        GRAPPLER_DEV_TASK_STATE_REQ                   = 0x01,  ///< an action was requested, next step will be to
                                                                // - request the reference runs, if STATION_DEV_TASKID_REFRUN
                                                                // - request actual motor positions, if STATION_DEV_TASKID_ACTION_REQ
        GRAPPLER_DEV_TASK_STATE_ACTPOS_REQ_SEND       = 0x02,  ///< the command to request the actual motor positions was
                                                                // forwarded via CAN, wait for motor positions being received by CAN
        GRAPPLER_DEV_TASK_STATE_ACTPOS_REQ_ACKN       = 0x03,  ///< the motor positions were received,
                                                                // next step: calculate the single steps which split the action
        GRAPPLER_DEV_TASK_STATE_PROGRESS_ACTION_STEPS = 0x04,  ///< progress the steps
        GRAPPLER_DEV_TASK_STATE_ACTION_STEPS_ACKN     = 0x05,  ///< acknowledge the execution
        GRAPPLER_DEV_TASK_STATE_ERROR                 = 0x06   ///< error state
    } GrapplerDevTaskState_t;

    /*! error task states */
    typedef enum {
        GRAPPLER_DEV_ERRTASK_STATE_FREE           = 0x00,   ///< task state free, no error
        GRAPPLER_DEV_ERRTASK_STATE_REPORT_IFACE   = 0x01,   ///< reports the error to the interface class
        GRAPPLER_DEV_ERRTASK_STATE_REPORT_DEVPROC = 0x02,   ///< reports the error to device processing
        GRAPPLER_DEV_ERRTASK_STATE_IDLE           = 0x03,   ///< error idle state, do nothing, block anything
        GRAPPLER_DEV_ERRTASK_STATE_RESET          = 0x04    ///< reset error state, change to initial state
    } GrapplerDevErrTaskState_t;

    GrapplerDevTaskID_t       m_taskID;        ///< task requested to the loader
    GrapplerDevTaskState_t    m_taskState;     ///< task state
    GrapplerDevErrTaskState_t m_errTaskState;  ///< the error task state

    /*! transport state */
    typedef enum {
        TRANSP_STATE_UNKNOWN,   //!< unknown
        TRANSP_STATE_RACK,      //!< rack transport position
        TRANSP_STATE_EMPTY,     //!< movement position without rack
        TRANSP_STATE_ATTACHED,  //!< down and attached to rack (rack touches cuvette)
        TRANSP_STATE_REFPOS     //!< reference position
    } TransportState_t;

    CStepperMotor* m_pMotorXAxis;    ///< pointer to the x-axis motor function module
    CStepperMotor* m_pMotorYAxis;    ///< pointer to the y-axis motor function module
    CStepperMotor* m_pMotorZAxis;    ///< pointer to the z-axis motor function module
    CRfid11785* m_pRFID;             ///< pointer to the RFID reader function module
    CAnalogInput* m_pLiquidLevel;    ///< pointer to the analog input function module connected to 'liquid level sensor'

    DeviceTask m_DeviceTask[MAX_GRAPPLER_DEVICE_TASK_ID];   ///< device task control

    StationPosition_t       m_stationPosition[4][18];   //!< Station position data
    StationPositionAdjust_t m_stationAdjust[4][18];     //!< Station adjustment data

    //position of the recent run-up station
    qint32 m_stationPosX;   ///< Recent run-up station x pos
    qint32 m_stationPosY;   ///< Recent run-up station y pos
    //StationAction_t m_stationAktion;

    //position of the next station
    qint32 m_nextStationRow;       ///< next target station row
    qint32 m_nextStationColumn;    ///< next target station column
    qint32 m_nextStationPosX;      ///< next target station x position (for motor)
    qint32 m_nextStationPosY;      ///< next target station y position (for motor)
    StationAction_t m_ActStationAction;   ///< next target station action

    //actual RFID data
    quint32 m_actRfidDataMSB;   ///< RFID data
    quint32 m_actRfidDataLSB;   ///< RFID data

    //actual liquid level
    quint16 m_actLiquidLevelValue;   ///< Liquid level value

    ///<variables from process settings
    // Z-axis
    Position_t         m_PositionZAxisTransportUnload; ///< z-position when moving x-y loaded (with rack)
    Position_t         m_PositionZAxisTransportLoad;   ///< z-position when moving x-y unloaded
    Position_t         m_PositionZAxisCuvette;         ///< z-position when attaching/detaching a rack in cuvette
    MotionProfileIdx_t m_ProfileZAxisUpUnload;         ///< z-axis motion profile when moving up without rack
    MotionProfileIdx_t m_ProfileZAxisUpLoadSlow;       ///< z-axis motion profile when pulling a rack out of liquid (slow)
    MotionProfileIdx_t m_ProfileZAxisUpLoadFast;       ///< z-axis motion profile when pulling a rack out of th vessel (high)
    quint16            m_CmdDelayZAxisUpLoadFast;      ///< time delay for sending the fast speed command when pulling a rack
    Position_t         m_PositionZAxisUpAcc;           ///< position to change from low to high speed when pulling a rack
    MotionProfileIdx_t m_ProfileZAxisDownUnload;       ///< z_axis_down_unload_speed_motion_profile
    MotionProfileIdx_t m_ProfileZAxisDownLoad;         ///< z-axis motion profile when moving a rack into the vessel

    // X-axis
    MotionProfileIdx_t m_ProfileXAxisUnload; ///< x-axis speed without rack
    MotionProfileIdx_t m_ProfileXAxisLoad;   ///< x-axis speed with rack
    // Y-axis
    MotionProfileIdx_t m_ProfileYAxisUnload; ///< y-axis speed without rack
    MotionProfileIdx_t m_ProfileYAxisLoad;   ///< y-axis speed with rack

    //Park positions
    Position_t m_PositionX1AxisPark;    ///< grapper 1 x-axis park position
    Position_t m_PositionX2AxisPark;    ///< grapper 2 x-axis park position
    Position_t m_PositionY1AxisPark;    ///< grapper 1 y-axis park position
    Position_t m_PositionY2AxisPark;    ///< grapper 2 y-axis park position
    Position_t m_PositionZ1AxisPark;    ///< grapper 1 z-axis park position
    Position_t m_PositionZ2AxisPark;    ///< grapper 2 z-axis park position

    //Position_t m_PositionZAxisLoadTransport;   ///< z-position when moving x-y loaded (with rack)
    //Position_t m_PositionZAxisUnloadTransport;   ///< z-position when moving x-y unloaded

    qint32 m_OffsetYAxisLoad;   ///< y-axis position offset to cuvette centre when grapper is attached to rack
    qint32 m_OffsetYAxisUnload; ///< y-axis position offset to cuvette centre when grapper is detached to rack (moves empty up or down)

    qint32 m_ProfileYAxisGrabLoad;  ///< y-axis speed when grabbing/discarding a rack (small positon offset inside the vessel

    TransportState_t  m_TransportState;      ///< transport state (with or without rack)

#ifdef TIME_CONSUMP
    //Time consumption
    /**
     * Accelerate without rack in x-direction until full speed is reached.
    */
    static quint16 xTimeToAccelerateToEmptyMoveSpeed;

    /**
     * Come down from nominal speed moving without rack in x-direction to halt.
    */
    static quint16 xTimeToComeToHaltFromEmptyMoveSpeed;
    /**
     * time required to travel 1 meter left <--> right.
    */
    static quint16 xTimeFor1000mmAtEmptyMoveSpeed;


    /**
     * Accelerate without rack in y-direction until full speed is reached.
    */
    static quint16 yTimeToAccelerateToEmptyMoveSpeed;
    /**
     * Come down from nominal speed moving without rack in y-direction to halt.
    */
    static quint16 yTimeToComeToHaltFromEmptyMoveSpeed;
    /**
     * time required to travel 1 meter back <--> front.
    */
    static quint16 yTimeFor1000mmAtEmptyMoveSpeed;

    /**
     * Accelerate with rack attached in x-direction until full transport speed is reached.
    */
    static quint16 xTimeToAccelerateToRackTransportSpeed;
    /**
     * Come to halt from rack transport speed in x-direction.
    */
    static quint16 xTimeToComeToHaltFromRackTransportSpeed;
    /**
     * time required to travel 1 meter left <--> right with rack attached.
    */
    static quint16 xTimeFor1000mmAtRackTransportSpeed;

    /**
     * Accelerate with rack attached in y-direction until full transport speed is reached.
    */
    static quint16 yTimeToAccelerateToRackTransportSpeed;
    /**
     * Come to halt from rack transport speed in y-direction.
    */
    static quint16 yTimeToComeToHaltFromRackTransportSpeed;
    /**
     * time required to travel 1 meter back <--> front with rack attached.
    */
    static quint16 yTimeFor1000mmAtRackTransportSpeed;

    /** (Z-axis)
     * Move grappler with attached rack downward from transport height (after a rack transport has ended).
    */
    static quint16 zTimeToLetDownRackBeforeStainingStart;

    /** (Y-axis)
     * Detach grappler from rack (after rack has touched bottom of cuvette). (y-axis)
    */
    static quint16 yTimeToDetachRackFromGrapplerAfterStainingStart;

    /** (Z-axis)
     * Move grappler without rack upwards (after the rack was detached).
    */
    static quint16 zTimeToPullGrapplerUpEmptyAfterDetachFromRack;

    /** (Z-axis)
     * Move grappler without rack downwards (after a combined x/y-axis move has completed).
    */
    static quint16 zTimeToLetGrapplerDownEmptyBeforeAttachToRack;

    /** (Y-axis)
     * Attach grappler to rack (after the z-axis move downwards is completed).
    */
    static quint16 yTimeToAttachRackToGrapplerBeforeStainingEnd;

    /** (Z-axis)
     * Move grappler with attached rack upward to transport height (so that rack transport may begin).
    */
    static quint16 zTimeToLiftUpRackAfterStainingEnd;
#endif
};


}  //namespace

#endif /* GRAPPLERDEVICE_H */

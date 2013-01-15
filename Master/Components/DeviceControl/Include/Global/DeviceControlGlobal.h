/****************************************************************************/
/*! \file DeviceControlGlobal.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    01.08.2010
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the global defines of the device control layer
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
#ifndef DEVICE_CONTROL_GLOBAL_H
#define DEVICE_CONTROL_GLOBAL_H

#include <QObject>
#include <QMetaType>

namespace DeviceControl
{

//****************************************************************************/
// Module constants
//****************************************************************************/

// Base module timeouts
#define CAN_NODE_TIMEOUT_INIT                   3000    //!< Timeout initialisation procedure
#define CAN_NODE_TIMEOUT_CONFIG_RECEIVE         3000    //!< Timeout configuration procedure
#define CAN_NODE_TIMEOUT_CONFIG_FCT_MODULES     3000    //!< Timeout function module configuration procedure
#define CAN_NODE_TIMEOUT_HEARTBEAT_FAILURE      5000    //!< Timeout heartbeat failure
#define CAN_NODE_TIMEOUT_SETINITOPDATA           500    //!< Timeout setting initial operation data
#define CAN_NODE_TIMEOUT_DATA_REQ                500    //!< Timeout for several requests with data transfer
#define CAN_NODE_TIMEOUT_MEMORY_OPERATION       1000    //!< Timeout for memory operation, eg. reset or format partition

// digital input timeouts
#define CAN_DINP_TIMEOUT_READ_REQ                400    //!< Timeout read request
// digital output timeouts
#define CAN_DOUTP_TIMEOUT_READ_REQ               400    //!< Timeout read request
// digital output timeouts
#define CAN_DOUTP_TIMEOUT_LIFECYCLE_REQ         1000    //!< Timeout lifecycle request
// digital output timeouts
#define CAN_DOUTP_TIMEOUT_SET_ACK                400    //!< Timeout read request
// analog input timeouts
#define CAN_AINP_TIMEOUT_READ_REQ                400    //!< Timeout read request
// analog output timeouts
#define CAN_AOUTP_TIMEOUT_READ_REQ               400    //!< Timeout read request
// analog output timeouts
#define CAN_AOUTP_TIMEOUT_SET_ACK                400    //!< Timeout read request
// temperature set timeouts
#define CAN_TEMPCTRL_TIMEOUT_TEMP_SET_ACK        400    //!< Timeout set request
// temperature req timeouts
#define CAN_TEMPCTRL_TIMEOUT_ACTTEMP_REQ         400    //!< Timeout read request
// temperature control status set timeouts (on/off)
#define CAN_TEMPCTRL_TIMEOUT_STATUS_SET_REQ      400    //!< Timeout set request
// temperature control status req timeouts
#define CAN_TEMPCTRL_TIMEOUT_ACTSTATUS_REQ       400    //!< Timeout read request
// operation mode set timeouts
#define CAN_TEMPCTRL_TIMEOUT_OPMODE_SET_REQ      400    //!< Timeout set request
// operation mode req timeouts
#define CAN_TEMPCTRL_TIMEOUT_ACTOPMODE_REQ       400    //!< Timeout read request


// stepper motor timeouts
#define CAN_STEPPERMOTOR_TIMEOUT_SET_STATE      1000    //!< Timeout SetState
#define CAN_STEPPERMOTOR_TIMEOUT_REFMOVE_REQ     400    //!< Timeout reference run request
#define CAN_STEPPERMOTOR_TIMEOUT_POSMOVE_REQ  600000    //!< Timeout positioning  \todo remove
#define CAN_STEPPERMOTOR_TIMEOUT_SPEEDMOVE_REQ  5000    //!< Timeout speed request
#define CAN_STEPPERMOTOR_TIMEOUT_ACTPOS_REQ      400    //!< Timeout actual position request (just communication)
#define CAN_STEPPERMOTOR_TIMEOUT_ACTSPD_REQ      400    //!< Timeout actual speed request (just communication)
#define CAN_STEPPERMOTOR_TIMEOUT_LIFECYCLEDATA_REQ 400  //!< Timeout life cycle date request (just communication, but three messages)
// rfid timeouts
#define CAN_RFID_TIMEOUT_READ_REQ               1000    //!< Timeout read request
#define CAN_RFID_TIMEOUT_WRITE_REQ              1250    //!< Timeout write operation
//inlinometer timeouts
#define CAN_INCL_TIMEOUT_CALIB_ACKN             1000    //!< Timeout calibration
#define CAN_INCL_TIMEOUT_READ_REQ                400    //!< Timeout read request
#define CAN_INCL_TIMEOUT_LIMIT_SET_REQ           400    //!< Timeout set request
//joystick timeouts
#define CAN_JOYSTICK_TIMEOUT_CALIB_ACKN         1000    //!< Timeout calibration
#define CAN_JOYSTICK_TIMEOUT_READ                400    //!< Timeout read request

// UART timeouts
#define CAN_UART_TIMEOUT_READ_REQ               1000    //!< Timeout send data ackn
#define CAN_UART_TIMEOUT_REQ_REC_DATA           1000    //!< Timeout req received data answer

//joystick timeouts
#define DEV_LOADER_TIMEOUT_BLOCKSTATE_SET_ACK    400    //!< Timeout set loader block mode
#define DEV_LOADER_TIMEOUT_ACTBLOCKSTATE_REQ     400    //!< Timeout req loader block mode


//*****************************************************************************/
// Module type definitions
//*****************************************************************************/

/*! general return codes, will be used as return value by methods, and as hdlInfo variable by signals
   these return codes will not appear as an error code in EventEntrys errorID. Anstead they will be transmitted in errorData variable */
typedef enum {
    DCL_ERR_FCT_CALL_SUCCESS      =  0,  //!< function was executed successfully
    DCL_ERR_FCT_CALL_FAILED       =  1,  //!< function was failed, no further information available
    DCL_ERR_FCT_CALL_NOT_FOUND    =  2,  //!< inside the function, a parameter concerning action was not done because anything was not found
    DCL_ERR_FCT_NOT_IMPLEMENTED   =  3,  //!< Temporary return value for bare functions, should be removed at the end
    DCL_ERR_INVALID_STATE         = 10,  //!< a method was called while the internal state machine was not able to process the functionality
    DCL_ERR_NOT_INITIALIZED       = 11,  //!< A method was called before complete initialisation was done
    DCL_ERR_TIMEOUT               = 12,  //!< A timeout appeared (typically a CAN-Bus timeout, while waiting for an answer)
    DCL_ERR_INVALID_PARAM         = 13,  //!< One ore more invalid parameters were passed to the method
    DCL_ERR_INTERNAL_ERR          = 14,  //!< The error reason is internal, further information is available at ...
    DCL_ERR_EXTERNAL_ERROR        = 15,  //!< The error reason is external, it was reported via CAN-bus
    DCL_ERR_EXTERNAL_WARNING      = 16,  //!< The warning reason is external, it was reported via CAN-bus
    DCL_ERR_EXTERNAL_INFO         = 17,  //!< The information reason is external, it was reported via CAN-bus
    DCL_ERR_NULL_PTR_ACCESS       = 18,  //!< A null pointer would be accessed, but there was an instruction to avoid its direct access
    DCL_ERR_CANMSG_INVALID        = 20,  //!< An invalid CAN-message was received (e.g. dlc not correct)
    DCL_ERR_CANBUS_NOT_READY      = 21,  //!< A CAN-message was tried to be send while the CAN bus is in error state
    DCL_ERR_CANBUS_ERROR          = 22,  //!< A CAN-message was tried to be send while an 'unknown' CAN bus error is active
    DCL_ERR_UNDEFINED             = 99   //!< The return code was not set
} ReturnCode_t;

/*! Node state definitions */
typedef enum { 
    NODE_STATE_UNDEFINED = 0,    //!< Undefined state
    NODE_STATE_BOOTING   = 1,    //!< Node is in boot loader
    NODE_STATE_STARTUP   = 2,    //!< Firmware initialization
    NODE_STATE_IDENTIFY  = 3,    //!< Node identification
    NODE_STATE_CONFIGURE = 4,    //!< Node configuration
    NODE_STATE_NORMAL    = 5,    //!< Normal operation mode
    NODE_STATE_ASSEMBLY  = 6,    //!< Assembly mode
    NODE_STATE_SHUTDOWN  = 7,    //!< Shutdown (going to standby)
    NODE_STATE_STANDBY   = 8     //!< Standby
} NodeState_t;

/*! Enumeration to control emergency stops */
typedef enum {
    RESET_EMERGENCY_STOP = 0,   //!< Clear emergency stop state
    STOPPED_BY_HEARTBEAT = 1,   //!< Emergency stop by heartbeat loss
    STOPPED_BY_NOTSTOP   = 2    //!< Emergency stop by master command
} EmergencyStopReason_t;

/*! Power supply states (applies to both: voltage and current) */
typedef enum {
    POWER_GOOD    = 0,  //!< Power is ok
    POWER_WARNING = 1,  //!< Power isn't good, but still acceptable
    POWER_FAILED  = 2,  //!< Power is bad
    POWER_UNKNOWN = 9   //!< Power is unknown (can't be read)
} PowerState_t;

/*! Test result code, e.g. for the end test */
typedef enum {
    TEST_OPEN   = 0,    //!< Test has not been executed yet
    TEST_PASSED = 1,    //!< The test was successful
    TEST_FAILED = 2     //!< The test failed
} TestResult_t;

/*! requested action for station positioning */
typedef enum {
    STATION_ACTION_MOVE_EMTPY_TO        = 0x00,  //!< move x- and y-axis without rack to station
    STATION_ACTION_TRANSP_RACK_TO       = 0x01,  //!< move x- and y-axis with rack to station
    STATION_ACTION_ATTACH_TO_RACK       = 0x02,  //!< move down z-axis and attaches to rack (by moving y-axis)
    STATION_ACTION_DETACH_RACK          = 0x03,  //!< detaches rack (by moving y-axis) and move z-axis up to x/y-move height
    STATION_ACTION_LET_DOWN_RACK        = 0x04,  //!< move down z-axis, rack relieves from grappler (but not detached)
    STATION_ACTION_PULL_UP_RACK         = 0x05,  //!< move up z-axis with rack to transport height
    STATION_ACTION_READ_RFID            = 0x06,  //!< read the rack's RFID tag
    STATION_ACTION_READ_LIQUID_LEVEL    = 0x07,  //!< measure liquid laver at target vessel
    STATION_ACTION_REFERENCE_RUN        = 0x08,  //!< execute reference run
    STATION_ACTION_MOTOR_POSITION_REQ   = 0x09,  //!< internal use only (request motor position)
    STATION_ACTION_ADJUSTMENT           = 0x0a,  //!< internal use only (run adjustment cycle)
    STATION_ACTION_UNDEF                = 0x0b   //!< undefined
} StationAction_t;

/*! requested action for station positioning */
typedef enum {
    RACKHDL_STATION_UNDEF                = 0x00,   //!< undefined
    RACKHDL_STATION_TRANSFER             = 0x01,   //!< rack transfer station
    RACKHDL_STATION_LOADER_1             = 0x02,   //!< loader vessel 1
    RACKHDL_STATION_LOADER_2             = 0x03,   //!< loader vessel 2
    RACKHDL_STATION_ROTATION_INSERT      = 0x04,   //!< insert rack to rack rotation device
    RACKHDL_STATION_ROTATION_REMOVE      = 0x05,   //!< remove rack from rack rotation device
    RACKHDL_STATION_COVER_LINE_1         = 0x06,   //!< cover line 1
    RACKHDL_STATION_COVER_LINE_2         = 0x07,   //!< cover line 1
    RACKHDL_STATION_OVEN                 = 0x08,   //!< oven device
    RACKHDL_STATION_UNLOADER_1           = 0x09,   //!< unloader channel 1
    RACKHDL_STATION_UNLOADER_2           = 0x0a,   //!< unloader channel 2
    RACKHDL_STATION_UNLOADER_3           = 0x0b    //!< unloader channel 3
} RackHdlStation_t;

/*! Block state, used for loader and rack transfer devices */
typedef enum {
    BLOCKING_STATE_UNDEF          = 0x00,   //!< blocking state undefinded
    BLOCKING_STATE_FREE           = 0x01,   //!< blocking state 'free'
    BLOCKING_STATE_BLOCKED        = 0x02    //!< blocking state 'blocked'
} BlockState_t;

/*! Hood state, used for detecting hood open event */
typedef enum {
    HOOD_STATE_UNDEF   = 0x00,
    HOOD_STATE_OPEN    = 0x01,
    HOOD_STATE_CLOSED  = 0x02
} HoodState_t;

/*! drawer position at loader/unloader */
typedef enum {
    LOADER_OPEN          = 0x00,   //!< opened
    LOADER_CLOSED        = 0x01,   //!< closed
    LOADER_UNDEF         = 0x02    //!< undefined
} LoaderPosition_t;

/*! vessel index for RFID operation to drawer at loader/unloader */
typedef enum {
    LOADER_ID_RFID_UNDEF = 0x00,   //!< undefined
    LOADER_ID_RFID_1     = 0x01,   //!< vessel 1
    LOADER_ID_RFID_2     = 0x02,   //!< vessel 2
    LOADER_ID_RFID_3     = 0x03,   //!< vessel 3
    LOADER_ID_RFID_4     = 0x04,   //!< vessel 4
    LOADER_ID_RFID_5     = 0x05    //!< vessel 5
} LoaderRFIDChannel_t;

/*! predefined agitation speeds */
typedef enum {
    AGITATION_SPEED_UNDEF = 0x00,  //!< undefined
    AGITATION_SPEED_STOP  = 0,  //!< stop agitation
    AGITATION_SPEED_05    = 800,  //!< agitation speed 0.5 rev/sec
    AGITATION_SPEED_10    = 1600,  //!< agitation speed 1.0 rev/sec
    AGITATION_SPEED_15    = 2400,  //!< agitation speed 1.5 rev/sec
    AGITATION_SPEED_20    = 3200   //!< agitation speed 2.0 rev/sec
} AgitationSpeed_t;

/*! agitation position */
typedef enum {
    AGITATION_POS_UNDEF        = 0x00,   //!< undefined (anywhere, except basing point)
    AGITATION_POS_BASING_POINT = 0x01    //!< basing point
} AgitationPosition_t;

/*! water valve identification */
typedef enum {
    WATER_VALVE_ID_UNDEF = 0x00,   //!< undefined
    WATER_VALVE_ID_1     = 0x01,   //!< water valve 1
    WATER_VALVE_ID_2     = 0x02,   //!< water valve 2
    WATER_VALVE_ID_3     = 0x03,   //!< water valve 3
    WATER_VALVE_ID_4     = 0x04,   //!< water valve 4
    WATER_VALVE_ID_5     = 0x05,   //!< water valve 5
    WATER_VALVE_ID_6     = 0x06,   //!< water valve 6
    WATER_VALVE_ID_ALL   = 0x07    //!< all water valve (e.g for new state)
} WaterValveID_t;

/*! oven cover position */
typedef enum {
    OVEN_COVER_UNDEF   = 0x00,  //!< undefined
    OVEN_COVER_OPENED  = 0x01,  //!< opened
    OVEN_COVER_CLOSED  = 0x02   //!< closeded
} OvenCoverPosition_t;

/*! temperature control status */
typedef enum {
    TEMPCTRL_STATUS_UNDEF = 0x00,   //!< status undefined
    TEMPCTRL_STATUS_OFF   = 0x01,   //!< temp. ctrl. is off
    TEMPCTRL_STATUS_ON    = 0x02    //!< temp. ctrl. is active (on)
} TempCtrlStatus_t;

/*! temperature control operation */
typedef enum {
    TEMPCTRL_OPMODE_UNDEF = 0x00,   //!< operation mode undefined
    TEMPCTRL_OPMODE_FULL  = 0x01,   //!< operation mode full power
    TEMPCTRL_OPMODE_HOLD  = 0x02    //!< operation mode power safe
} TempCtrlOperatingMode_t;

/*! temperature control mains voltage */
typedef enum {
    TEMPCTRL_VOLTAGE_UNDEF = 0x00,  //!< status undefined
    TEMPCTRL_VOLTAGE_220V  = 0x01,  //!< 220V mains voltage
    TEMPCTRL_VOLTAGE_110V  = 0x02   //!< 110V mains voltage
} TempCtrlMainsVoltage_t;

/*! Heated vessels access identification */
typedef enum {
    HVESSELS_ID_VESSEL_UNDEF = 0x00,  //!< vessel identification undefiened
    HVESSELS_ID_VESSEL_1     = 0x01,  //!< vessel 1
    HVESSELS_ID_VESSEL_2     = 0x02,  //!< vessel 2
    HVESSELS_ID_VESSEL_3     = 0x03,  //!< vessel 3
    HVESSELS_ID_VESSEL_4     = 0x04,  //!< vessel 4
    HVESSELS_ID_VESSEL_ALL   = 0x05   //!< all vessel
} HeatedVesselID_t;

/*! Rack transfer adapter position */
typedef enum {
    RACK_ADAPTER_POS_UNDEF    = 0x00,   //!< undefined
    RACK_ADAPTER_POS_LOAD     = 0x01,   //!< load position (from colorado view)
    RACK_ADAPTER_POS_TRANSFER = 0x02    //!< transfer position (from colorado view)
} RackAdapterPosition_t;

/*! Rack type definition */
typedef enum {
    RACK_TYPE_UNDEF      = 0x00,  //!< undefined
    RACK_TYPE_30_SLIDES  = 0x01,  //!< rack with 30 slide positions
    RACK_TYPE_20_SLIDES  = 0x02,  //!< rack with 20 slide positions
    RACK_TYPE_5_SLIDES   = 0x03   //!< rack with 5 slide positions
} RackType_t;

typedef qint32 Position_t;         //!< stepper motor position, [half steps]
typedef qint16 Speed_t;            //!< stepper motor speed, [half steps/sec]
typedef quint8 MotionProfileIdx_t; //!< stepper motor motion profile index

/*! Device instance ID definitions */
typedef enum {
    DEVICE_INSTANCE_ID_UNDEFINED      = 0x00000000,  //!< undefine. used for initialization
    DEVICE_INSTANCE_ID_DEVPROC        = 0x00008000,  //!< the device processing itself
  //  DEVICE_INSTANCE_ID_GRAPPLER_1     = 0x00008010,  //!< the left grappler
  //  DEVICE_INSTANCE_ID_GRAPPLER_2     = 0x00008011,  //!< the right grappler
    DEVICE_INSTANCE_ID_LOADER         = 0x00008020,  //!< loader
    DEVICE_INSTANCE_ID_UNLOADER       = 0x00008021,  //!< unloader
    DEVICE_INSTANCE_ID_OVEN           = 0x00008030,  //!< oven
    DEVICE_INSTANCE_ID_HVESSELS       = 0x00008040,  //!< heated vessels
    DEVICE_INSTANCE_ID_AGITATION      = 0x00008050,  //!< agitation
    DEVICE_INSTANCE_ID_RACKTRANSFER   = 0x00008060,  //!< rack transfer
    DEVICE_INSTANCE_ID_EXHAUST        = 0x00008070,  //!< exhaust, includes fan control an supervision
    DEVICE_INSTANCE_ID_WATER          = 0x00008080,  //!< water, six valves and the liquid level sensor
    DEVICE_INSTANCE_ID_COVERLINE_1    = 0x000080A0,  //!< cover line unit 1
    DEVICE_INSTANCE_ID_COVERLINE_2    = 0x000080A1,  //!< cover line unit 2
    DEVICE_INSTANCE_ID_RACK_HANDLING  = 0x000080B0,  //!< rack handling
    DEVICE_INSTANCE_ID_HOOD           = 0x000080B1,  //!< device cover (hood)
    DEVICE_INSTANCE_ID_LIGHT          = 0x000080C1,  //!< device Light
    DEVICE_INSTANCE_ID_XYZ            = 0x00008010,  //!< device XYZ
    DEVICE_INSTANCE_ID_XYZ_2          = 0x00008011,  //!< device XYZ_2
    DEVICE_INSTANCE_ID_SLIDE_ID       = 0x00008090   //!< Slide ID
} DevInstanceID_t;


/****************************************************************************/
/*!
 *  \brief  This classes elements containing the can object keys, used for
 *          object identification by string at the colorado project
 */
/****************************************************************************/
class CANObjectKeyLUT
{
public:
    // Devices
    static const QString m_DevAgitationKey;         //!< Agitation device key
    static const QString m_DevExhaustKey;           //!< Exhaust device key
    static const QString m_DevXyzTransportationKey; //!< XYZ transportation device key
    static const QString m_DevHeatedCuvettesKey;    //!< Heated cuvettes device key
    static const QString m_DevDrawerKey;            //!< Drawer device key
    static const QString m_DevOvenKey;              //!< Oven device key
    static const QString m_DevRackTransferKey;      //!< Rack transfer device key
    static const QString m_DevWaterKey;             //!< Water device key
    static const QString m_DevRackHandlingKey;      //!< Rack handling device key
    static const QString m_DevCoverLineKey;         //!< Cover line device key
    static const QString m_DevSlideIdKey;           //!< Slide ID device key
    static const QString m_DevHoodKey;              //!< Hood device key
    static const QString m_DevLightKey;             //!< Hood device key
    // Colorado base modules
    static const QString m_BaseAgitationKey;        //!< Agitation base module key
    static const QString m_BaseExhaustKey;          //!< Exhaust base module key
    static const QString m_BaseXAxisKey;            //!< X-axis base module key
    static const QString m_BaseYAxisKey;            //!< Y-axis base module key
    static const QString m_BaseZAxisKey;            //!< Z-axis base module key
    static const QString m_BaseHeatedCuvettesKey;   //!< Heated cuvettes base module key
    static const QString m_BaseDrawerKey;           //!< Drawer base module key
    static const QString m_BaseOvenKey;             //!< Oven base module key
    static const QString m_BaseRackTransferKey;     //!< Rack transfer base module key
    static const QString m_BaseWaterKey;            //!< Water base module key
    static const QString m_BaseSlideIdKey;          //!< Slide ID base module key
    // Colorado function modules
    static const QString m_MotorAgitationKey;           //!< Motor agitation
    static const QString m_FlowSensorKey;               //!< air flow sensor
    static const QString m_CurrentFan1Key;              //!< current measurement fan 1
    static const QString m_CurrentFan2Key;              //!< current measurement fan 2
    static const QString m_MotorXAxisKey;               //!< Motor X-axis
    static const QString m_MotorYAxisKey;               //!< Motor Y-axis
    static const QString m_MotorZAxisKey;               //!< Motor Z-axis
    static const QString m_GrapplerRFIDKey;             //!< RFID reader at grappler
    static const QString m_GrapplerLiquidLevelKey;      //!< Liquid level sensor at grappler
    static const QString m_TempCtrlVessel1Key;          //!< Temperature control cuvette 1
    static const QString m_TempCtrlVessel2Key;          //!< Temperature control cuvette 2
    static const QString m_TempCtrlVessel3Key;          //!< Temperature control cuvette 3
    static const QString m_TempCtrlVessel4Key;          //!< Temperature control cuvette 4
    static const QString m_InclinometerKey;             //!< Inclinometr
    static const QString m_MotorDrawerKey;              //!< Drawer motor
    static const QString m_DrawerRFIDKey;               //!< Drawer rfid module
    static const QString m_DrawerButtonKey;             //!< Drawer button
    static const QString m_DrawerLEDFreeKey;            //!< Drawer LED 'free' state
    static const QString m_DrawerLEDBlockedKey;         //!< Drawer LED 'blocked' state
    static const QString m_OvenCoverMotorKey;           //!< Motor oven cover
    static const QString m_OvenTempCtrlKey;             //!< temperature control oven
    static const QString m_RackTransferMotorKey;        //!< Motor rack transfer
    static const QString m_WaterValve1Key;              //!< Water valve 1
    static const QString m_WaterValve2Key;              //!< Water valve 2
    static const QString m_WaterValve3Key;              //!< Water valve 3
    static const QString m_WaterValve4Key;              //!< Water valve 4
    static const QString m_WaterValve5Key;              //!< Water valve 5
    static const QString m_WaterValve6Key;              //!< Water valve 6
    static const QString m_WaterLiquidLevelKey;         //!< Liquid level sensor
    static const QString m_SlideIdPhotoDetectorKey;     //!< Slide ID photo detector
    static const QString m_SlideIdTransmitControlKey;   //!< Slide ID transmit control
    static const QString m_SlideIdTransmitCurrentKey;   //!< Slide ID transmit current
    static const QString m_SlideIdReceiveCurrentKey;    //!< Slide ID receive current
    // Sepia function modules
    static const QString m_MotorCoverLineZAxisKey;      //!< Motor Z-axis (elevator)
    static const QString m_MotorCoverLineSlideKey;      //!< Motor slide shifter
    static const QString m_MotorCoverLineNeedleKey;     //!< Motor needle
    static const QString m_MotorCoverLineClampKey;      //!< Motor clamp mechanism
    static const QString m_MotorCoverSlipYKey;          //!< Motor Y-axis slide transport
    static const QString m_MotorCoverSlipZKey;          //!< Motor Z-axis slide transport
    static const QString m_MotorEDMPump;                //!< Motor EDM pump
    static const QString m_DigInpCoverLineSlideDetect;  //!< Sensor slide detection
    static const QString m_DigInpEDMLevel;              //!< Sensor EDM level
    static const QString m_DigOutpVacuumPump;           //!< Vacuum pump slide transport
    static const QString m_DigOutpVacuumValve1;         //!< Vacuum valve slide transport
    static const QString m_DigOutpVacuumValve2;         //!< Vacuum valve slide transport
    static const QString m_AnaInpPressureSensor;        //!< Vacuum sensor slide transport
    static const QString m_AnaInpHallSensor1;           //!< Hall sensor slide transport
    static const QString m_MotorRackHdlXAxisKey;        //!< Motor X-axis rack handling
    static const QString m_MotorRackHdlYAxisKey;        //!< Motor Y-axis rack handling
    static const QString m_MotorRackHdlZAxisKey;        //!< Motor Z-axis rack handling
    static const QString m_MotorRackHdlGrabberKey;      //!< Motor grabber rack handling
    static const QString m_HoodSensorKey;               //!< Hood open detection
};

} //namespace

Q_DECLARE_METATYPE(DeviceControl::ReturnCode_t)
Q_DECLARE_METATYPE(DeviceControl::LoaderRFIDChannel_t)
Q_DECLARE_METATYPE(DeviceControl::TempCtrlStatus_t)
Q_DECLARE_METATYPE(DeviceControl::TempCtrlOperatingMode_t)
Q_DECLARE_METATYPE(DeviceControl::HeatedVesselID_t)

#endif /* DEVICE_CONTROL_GLOBAL_H */

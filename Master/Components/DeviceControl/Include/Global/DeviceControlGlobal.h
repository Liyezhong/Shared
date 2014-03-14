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

/* if TESTVERSION is defined, the executable is compiled with the /DeviceControl test gui */
//#define TESTVERSION


namespace DeviceControl
{
#define UNDEFINED_1_BYTE    (0xFF)
#define UNDEFINED_2_BYTE    (0xFFFF)
#define UNDEFINED_4_BYTE    (0xFFFFFFFF)

//****************************************************************************/
// Module constants
//****************************************************************************/

// basis module timeouts
#define CAN_NODE_TIMEOUT_INIT                   3000  //!< Timeout initialisation procedure
#define CAN_NODE_TIMEOUT_CONFIG_RECEIVE         3000  //!< Timeout configuration procedure
#define CAN_NODE_TIMEOUT_CONFIG_FCT_MODULES     3000  //!< Timeout function module configuration procedure
#define CAN_NODE_TIMEOUT_HEARTBEAT_FAILURE      5000  //!< Timeout heartbeat failure
#define CAN_NODE_TIMEOUT_SETINITOPDATA           500  //!< Timeout setting initial operation data
#define CAN_NODE_TIMEOUT_DATA_REQ                500  //!< Timeout for several requests with data transfer
#define CAN_NODE_TIMEOUT_MEMORY_OPERATION       1000  //!< Timeout for memory operation, eg. reset or format partition

// digital input timeouts
#define CAN_DINP_TIMEOUT_READ_REQ                400  //!< Timeout read request
// digital output timeouts
#define CAN_DOUTP_TIMEOUT_READ_REQ               400  //!< Timeout read request
// digital output timeouts
#define CAN_DOUTP_TIMEOUT_SET_ACK                400  //!< Timeout read request
// analog input timeouts
#define CAN_AINP_TIMEOUT_READ_REQ                400  //!< Timeout read request
// analog output timeouts
#define CAN_AOUTP_TIMEOUT_READ_REQ               400  //!< Timeout read request
// analog output timeouts
#define CAN_AOUTP_TIMEOUT_SET_ACK                400  //!< Timeout read request
// temperature set timeouts
#define CAN_TEMPCTRL_TIMEOUT_TEMP_SET_ACK        400  //!< Timeout set request
// temperature req timeouts
#define CAN_TEMPCTRL_TIMEOUT_ACTTEMP_REQ         400  //!< Timeout read request
// temperature control status set timeouts (on/off)
#define CAN_TEMPCTRL_TIMEOUT_STATUS_SET_REQ      400  //!< Timeout set request
// temperature control status req timeouts
#define CAN_TEMPCTRL_TIMEOUT_ACTSTATUS_REQ       400  //!< Timeout read request
// operation mode set timeouts
#define CAN_TEMPCTRL_TIMEOUT_OPMODE_SET_REQ      400  //!< Timeout set request
// operation mode req timeouts
#define CAN_TEMPCTRL_TIMEOUT_ACTOPMODE_REQ       400  //!< Timeout read request
// temperature req timeouts
#define CAN_PRESSURECTRL_TIMEOUT_TEMP_SET_ACK        400  //!< Timeout set request
// temperature req timeouts
#define CAN_PRESSURECTRL_TIMEOUT_ACTTEMP_REQ         400  //!< Timeout read request
// temperature control status set timeouts (on/off)
#define CAN_PRESSURECTRL_TIMEOUT_STATUS_SET_REQ      400  //!< Timeout set request
// temperature control status req timeouts
#define CAN_PRESSURECTRL_TIMEOUT_ACTSTATUS_REQ       400  //!< Timeout read request
// operation mode set timeouts
#define CAN_PRESSURECTRL_TIMEOUT_OPMODE_SET_REQ      400  //!< Timeout set request
// operation mode req timeouts
#define CAN_PRESSURECTRL_TIMEOUT_ACTOPMODE_REQ       400  //!< Timeout read request


// stepper motor timeouts
#define CAN_STEPPERMOTOR_TIMEOUT_SET_STATE      1000  //!< Timeout SetState
#define CAN_STEPPERMOTOR_TIMEOUT_REFMOVE_REQ     400  //!< Timeout reference run request
#define CAN_STEPPERMOTOR_TIMEOUT_POSMOVE_REQ  600000  //!< Timeout positioning  \todo remove
#define CAN_STEPPERMOTOR_TIMEOUT_SPEEDMOVE_REQ  5000  //!< Timeout speed request
#define CAN_STEPPERMOTOR_TIMEOUT_ACTPOS_REQ      400  //!< Timeout actual position request (just communication)
#define CAN_STEPPERMOTOR_TIMEOUT_ACTSPD_REQ      400  //!< Timeout actual speed request (just communication)
#define CAN_STEPPERMOTOR_TIMEOUT_LIFECYCLEDATA_REQ 400  //!< Timeout life cycle date request (just communication, but three messages)
// rfid timeouts
#define CAN_RFID_TIMEOUT_READ_REQ               1000  //!< Timeout read request
#define CAN_RFID_TIMEOUT_WRITE_REQ              1250  //!< Timeout write operation
//inlinometer timeouts
#define CAN_INCL_TIMEOUT_CALIB_ACKN             1000  //!< Timeout calibration
#define CAN_INCL_TIMEOUT_READ_REQ                400  //!< Timeout read request
#define CAN_INCL_TIMEOUT_LIMIT_SET_REQ           400  //!< Timeout set request
//joystick timeouts
#define CAN_JOYSTICK_TIMEOUT_CALIB_ACKN         1000  //!< Timeout calibration
#define CAN_JOYSTICK_TIMEOUT_READ                400  //!< Timeout read request

// UART timeouts
#define CAN_UART_TIMEOUT_READ_REQ               1000  //!< Timeout send data ackn
#define CAN_UART_TIMEOUT_REQ_REC_DATA           1000  //!< Timeout req received data answer

//joystick timeouts
#define DEV_LOADER_TIMEOUT_BLOCKSTATE_SET_ACK    400  //!< Timeout set loader block mode
#define DEV_LOADER_TIMEOUT_ACTBLOCKSTATE_REQ     400  //!< Timeout req loader block mode


//*****************************************************************************/
// Event code definitions
//*****************************************************************************/
#define EVENT_SOURCE_DEV_ROTARY_VALVE      0x01      //!< Event Source: Rotary valve device
#define EVENT_SOURCE_DEV_AIR_LIQUID        0x02      //!< Event Source: Air-liquid device
#define EVENT_SOURCE_DEV_RETORT            0x03      //!< Event Source: Retort device
#define EVENT_SOURCE_DEV_OVEN              0x04      //!< Event Source: Oven device
#define EVENT_SOURCE_DEV_MAIN_CTRL         0x05      //!< Event Source: Main control device
#define EVENT_SOURCE_DEV_INTERFACE         0x06      //!< Event Source: Device control interface
#define EVENT_SOURCE_FM_TEMP_CTRL          0x10      //!< Event Source: Temperature control function module
#define EVENT_SOURCE_FM_PRESSURE_CTRL      0x11      //!< Event Source: Pressure control function module
#define EVENT_SOURCE_FM_STEPPER_MOTOR      0x12      //!< Event Source: Stepper motor function module
#define EVENT_SOURCE_FM_DIGITAL_INPUT      0x13      //!< Event Source: Digital input function module
#define EVENT_SOURCE_FM_DIGITAL_OUTPUT     0x14      //!< Event Source: Digital output function module
#define EVENT_SOURCE_FM_ANALOG_INPUT       0x15      //!< Event Source: Analog input function module
#define EVENT_SOURCE_FM_ANALOG_OUTPUT      0x16      //!< Event Source: Analog output function module

#define EVENT_FUNC_SET_TEMP_CTRL_STATE       0x01    //!< Event Function: Set temperature control state
#define EVENT_FUNC_SET_TEMP_PID              0x02    //!< Event Function: Set temperature control PID parameter
#define EVENT_FUNC_START_TEMP_CTRL           0x03    //!< Event Function: Start temperature control
#define EVENT_FUNC_GET_TEMP                  0x04    //!< Event Function: Get temperature
#define EVENT_FUNC_GET_TEMP_CTRL_STATE       0x05    //!< Event Function: Get temperature control state
#define EVENT_FUNC_PRESSURE_PROCEDURE        0x0A    //!< Event Function: Pressure procedure
#define EVENT_FUNC_VACCUM_PROCEDURE          0x0B    //!< Event Function: Vaccum procedure
#define EVENT_FUNC_FILLING_PROCEDURE         0x0C    //!< Event Function: Filling procedure
#define EVENT_FUNC_DRAINING_PROCEDURE        0x0D    //!< Event Function: Draining procedure
#define EVENT_FUNC_SET_PRESSURE              0x0E    //!< Event Function: Set pressure
#define EVENT_FUNC_GET_PRESSURE              0x0F    //!< Event Function: Get pressure
#define EVENT_FUNC_SET_PRESSURE_CTRL_STATE   0x10    //!< Event Function: Set pressure control state
#define EVENT_FUNC_RELEASE_PRESSURE          0x11    //!< Event Function: Release pressure
#define EVENT_FUNC_FAN_OPERATION             0x12    //!< Event Function: Fan operation
#define EVENT_FUNC_ALL_STOP                  0x13    //!< Event Function: Stop all operation
#define EVENT_FUNC_SET_PRESSURE_DRIFT        0x14    //!< Event Function: Set pressure drift
#define EVENT_FUNC_GET_PRESSURE_DRIFT        0x15    //!< Event Function: Get pressure drift
#define EVENT_FUNC_BOTTLE_CHECK              0x16    //!< Event Function: Bottle check
#define EVENT_FUNC_LEVEL_SENSOR_STATE        0x17    //!< Event Function: Level sensor new state
#define EVENT_FUNC_MOVE_TO_INIT_POS          0x1A    //!< Event Function: Move to initial position
#define EVENT_FUNC_MOVE_TO_RV_POS            0x1B    //!< Event Function: Move to rotary valve position
#define EVENT_FUNC_GET_RV_POS                0x1C    //!< Event Function: Get rotary valve position
#define EVENT_FUNC_REF_RUN                   0x1D    //!< Event Function: Reference run
#define EVENT_FUNC_SET_LOCK_STATUS           0x20    //!< Event Function: Get lock status
#define EVENT_FUNC_GET_LOCK_STATUS           0x21    //!< Event Function: Set lock status
#define EVENT_FUNC_SET_MAIN_RELAY_STATUS     0x22    //!< Event Function: Set main relay status

#define EVENT_CODE_SUCCESS                   0x00    //!< Event Code: Success
#define EVENT_CODE_FAIL                      0x01    //!< Event Code: Failed
#define EVENT_CODE_GENERAL_ERROR             0x02    //!< Event Code: General error
#define EVENT_CODE_NOT_INITIALIZED           0x03    //!< Event Code: Not initialized
#define EVENT_CODE_INVALID_INPUT             0x04    //!< Event Code: Invalid input
#define EVENT_CODE_TIMEOUT                   0x05    //!< Event Code: Timeout
#define EVENT_CODE_BOTTLE_CHECK_EMPTY        0x06    //!< Event Code: Bottle check result: Empty
#define EVENT_CODE_BOTTLE_CHECK_NOTFULL      0x07    //!< Event Code: Bottle check result: Not-full
#define EVENT_CODE_BOTTLE_CHECK_OK           0x08    //!< Event Code: Bottle check result: OK
#define EVENT_CODE_BOTTLE_CHECK_BLOCKAGE     0x09    //!< Event Code: Bottle check result: Blockage
#define EVENT_CODE_MOTOR_EXCEED_UPPER_LIMIT  0x0A    //!< Event Code: Motor run exceed upper limit
#define EVENT_CODE_MOTOR_EXCEED_LOWER_LIMIT  0x0B    //!< Event Code: Motor run exceed lower limit
#define EVENT_CODE_MOTOR_UNEXPECTED_POS      0x0C    //!< Event Code: Motor run hit un-expected position
#define EVENT_CODE_INTERRUPT                 0x0D    //!< Event Code: Interrupted
#define EVENT_CODE_OVERFLOW                  0x0E    //!< Event Code: Overflow happend
#define EVENT_CODE_DIGITAL_SIGNAL_0          0x10    //!< Event Code: Digital input signal 0
#define EVENT_CODE_DIGITAL_SIGNAL_1          0x11    //!< Event Code: Digital input signal 1
#define EVENT_CODE_TIMEOUT_WARNING           0x12    //!< Event Code: Timeout for Warning
#define EVENT_CODE_TIMEOUT_ERROR             0x13    //!< Event Code: Timeout for Error



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
    DCL_ERR_UNEXPECTED_BREAK      = 23,   //!< Current program was breaked by other program
    DCL_ERR_SNYC_CALL_BUSY        = 24,   //!< Current program was breaked by other program
    DCL_ERR_TIMER_TIMEOUT         = 25,
    DCL_ERR_CANCOMMUTOR_START				= 0x0026,
    DCL_ERR_CANCOMMUTOR_INTERFACE_OPEN		= 0x0027,
    DCL_ERR_CANCOMMUTOR_TCPCLIENT_INIT		= 0x0028,
    DCL_ERR_CANCOMMUTOR_COMM_FAILED			= 0x0029,

    DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_0 = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_FILLING_PROCEDURE) << 8)| EVENT_CODE_DIGITAL_SIGNAL_0), //!< Get level sensor state 0
    DCL_ERR_FM_TEMP_LEVEL_SENSOR_STATE_1 = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_FILLING_PROCEDURE) << 8)| EVENT_CODE_DIGITAL_SIGNAL_1), //!< Get level sensor state 1

    DCL_ERR_DEV_AL_RELEASE_PRESSURE_TIMEOUT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_RELEASE_PRESSURE) << 8)| EVENT_CODE_TIMEOUT),//!< Release pressure timeout
    DCL_ERR_DEV_AL_RELEASE_PRESSURE_FAILED = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_RELEASE_PRESSURE) << 8)| EVENT_CODE_FAIL),//!< Release pressure failed
    DCL_ERR_DEV_AL_SETUP_PRESSURE_FAILED = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_SET_PRESSURE) << 8)| EVENT_CODE_FAIL),//!< Setup pressure failed
    DCL_ERR_DEV_AL_DRAIN_SUCCESS = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_DRAINING_PROCEDURE) << 8)| EVENT_CODE_SUCCESS),//!< Draining succeed
    DCL_ERR_DEV_AL_DRAIN_WARNING_TIMEOUT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_DRAINING_PROCEDURE) << 8)| EVENT_CODE_TIMEOUT_WARNING),//!< Draing timeout
    DCL_ERR_DEV_AL_DRAIN_ERROR_TIMEOUT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_DRAINING_PROCEDURE) << 8)| EVENT_CODE_TIMEOUT_ERROR),//!< Draing timeout
    DCL_ERR_DEV_AL_DRAIN_SETUP_PRESSURE_TIMEOUT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_DRAINING_PROCEDURE) << 8)| EVENT_CODE_TIMEOUT),//!< Draing setup pressure timeout
    DCL_ERR_DEV_AL_DRAIN_GENERAL_ERR = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_DRAINING_PROCEDURE) << 8)| EVENT_CODE_GENERAL_ERROR),//!< Draining general error
    DCL_ERR_DEV_AL_DRAIN_INTERRUPT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_DRAINING_PROCEDURE) << 8)| EVENT_CODE_INTERRUPT),//!< Draining has been interrupted

    DCL_ERR_DEV_AL_FILL_SUCCESS = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_FILLING_PROCEDURE) << 8)| EVENT_CODE_SUCCESS),//!< Filling succeed
    DCL_ERR_DEV_AL_FILL_INTERRUPT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_FILLING_PROCEDURE) << 8)| EVENT_CODE_INTERRUPT),//!< Filling has been interrupted
    DCL_ERR_DEV_AL_FILL_TIMEOUT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_FILLING_PROCEDURE) << 8)| EVENT_CODE_TIMEOUT),//!< Filling timeout
    DCL_ERR_DEV_AL_FILL_WARNING_TIMEOUT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_FILLING_PROCEDURE) << 8)| EVENT_CODE_TIMEOUT_WARNING),//!< Filling warning timeout
    DCL_ERR_DEV_AL_FILL_ERROR_TIMEOUT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_FILLING_PROCEDURE) << 8)| EVENT_CODE_TIMEOUT_ERROR),//!< Filling error timeout
    DCL_ERR_DEV_AL_FILL_OVERFLOW = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_FILLING_PROCEDURE) << 8)| EVENT_CODE_OVERFLOW),//!< Filling overflow

    DCL_ERR_DEV_AL_VACCUM_SUCCESS = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_VACCUM_PROCEDURE) << 8)| EVENT_CODE_SUCCESS),//!< Vaccum succeed
    DCL_ERR_DEV_AL_VACCUM_INTERRUPT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_VACCUM_PROCEDURE) << 8)| EVENT_CODE_INTERRUPT),//!< Vaccum has been interrupted
    DCL_ERR_DEV_AL_VACCUM_TIMEOUT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_VACCUM_PROCEDURE) << 8)| EVENT_CODE_TIMEOUT),//!< Vaccum timeout
    DCL_ERR_DEV_AL_PRESSURE_INTERRUPT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_PRESSURE_PROCEDURE) << 8)| EVENT_CODE_INTERRUPT),//!< Pressure procedure has been interrupted
    DCL_ERR_DEV_AL_PRESSURE_TIMEOUT = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_PRESSURE_PROCEDURE) << 8)| EVENT_CODE_TIMEOUT),//!< Pressure procedure timeout
    DCL_ERR_DEV_AL_PRESSURE_SUCCESS = ((((EVENT_SOURCE_DEV_AIR_LIQUID << 8) | EVENT_FUNC_PRESSURE_PROCEDURE) << 8)| EVENT_CODE_SUCCESS),//!< Pressure procedure succeed

    DCL_ERR_DEV_TEMP_CTRL_STATE_ERR = 43,//!< Temperatuer control state error
    DCL_ERR_DEV_TEMP_CTRL_ALREADY_ON = 44,//!< Temperatuer control is already on
    DCL_ERR_DEV_TEMP_CTRL_SET_TEMP_ERR = 45,//!< Temperatuer control set temperature error
    DCL_ERR_DEV_TEMP_CTRL_SET_STATE_ERR = 46,//!< Temperatuer control set state error

    DCL_ERR_DEV_BOTTLE_CHECK_OK = ((((EVENT_SOURCE_DEV_INTERFACE << 8) | EVENT_FUNC_BOTTLE_CHECK) << 8)| EVENT_CODE_BOTTLE_CHECK_OK),//!< Bottle check succeed
    DCL_ERR_DEV_BOTTLE_CHECK_NOT_FULL = ((((EVENT_SOURCE_DEV_INTERFACE << 8) | EVENT_FUNC_BOTTLE_CHECK) << 8)| EVENT_CODE_BOTTLE_CHECK_NOTFULL),//!< Bottle check result: Not full
    DCL_ERR_DEV_BOTTLE_CHECK_BLOCKAGE =  ((((EVENT_SOURCE_DEV_INTERFACE << 8) | EVENT_FUNC_BOTTLE_CHECK) << 8)| EVENT_CODE_BOTTLE_CHECK_BLOCKAGE),//!< Bottle check result: Blockage
    DCL_ERR_DEV_BOTTLE_CHECK_EMPTY =  ((((EVENT_SOURCE_DEV_INTERFACE << 8) | EVENT_FUNC_BOTTLE_CHECK) << 8)| EVENT_CODE_BOTTLE_CHECK_EMPTY),//!< Bottle check result: Empty
    DCL_ERR_DEV_BOTTLE_CHECK_ERROR =  ((((EVENT_SOURCE_DEV_INTERFACE << 8) | EVENT_FUNC_BOTTLE_CHECK) << 8)| EVENT_CODE_FAIL),//!< Bottle check error
    DCL_ERR_DEV_BOTTLE_CHECK_TIMEOUT =  ((((EVENT_SOURCE_DEV_INTERFACE << 8) | EVENT_FUNC_BOTTLE_CHECK) << 8)| EVENT_CODE_TIMEOUT),//!< Bottle check timeout

    DCL_ERR_DEV_RV_MOVE_TO_INIT_FUNC = (((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_MOVE_TO_INIT_POS) << 8),//!< Move to initial position fucntion
    DCL_ERR_DEV_RV_MOVE_TO_INIT_POS_SUCCESS = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_MOVE_TO_INIT_POS) << 8)| EVENT_CODE_SUCCESS),//!< Move to initial position succeed
    DCL_ERR_DEV_RV_MOVE_TO_INIT_UNEXPECTED_POS = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_MOVE_TO_INIT_POS) << 8)| EVENT_CODE_MOTOR_UNEXPECTED_POS),//!< Move to initial position failed

   // DCL_ERR_DEV_RV_MOVE_OK = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_MOVE_TO_RV_POS) << 8)| EVENT_CODE_SUCCESS),
    DCL_ERR_DEV_RV_REF_MOVE_OK = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_REF_RUN) << 8)| EVENT_CODE_SUCCESS),//!< Motor reference run ok
    DCL_ERR_DEV_RV_REF_MOVE_FAILED = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_REF_RUN) << 8)| EVENT_CODE_FAIL),//!< Motor reference run failed
    DCL_ERR_DEV_RV_MOVE_EXCEED_UPPER_LIMIT = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_REF_RUN) << 8)| EVENT_CODE_MOTOR_EXCEED_UPPER_LIMIT),//!< Motor reference run exceed code disk's upper limit
    DCL_ERR_DEV_RV_MOVE_EXCEED_LOWER_LIMIT = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_REF_RUN) << 8)| EVENT_CODE_MOTOR_EXCEED_LOWER_LIMIT),//!< Motor reference run exceed code disk's lower limit
    DCL_ERR_DEV_RV_INVALID_INPUT = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_REF_RUN) << 8)| EVENT_CODE_INVALID_INPUT),//!< Invalid input for motor reference run
    DCL_ERR_DEV_RV_NOT_INITIALIZED = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_REF_RUN) << 8)| EVENT_CODE_NOT_INITIALIZED),//!< Motor has not been initialized
    DCL_ERR_DEV_RV_UNEXPECTED_POS = ((((EVENT_SOURCE_DEV_ROTARY_VALVE << 8) | EVENT_FUNC_REF_RUN) << 8)| EVENT_CODE_MOTOR_UNEXPECTED_POS),//!< Motor encouter unexpected position

    DCL_ERR_DEV_RV_MOVE_LS_ERROR = 54,//!< Motor reference run read limit switch error
    DCL_ERR_DEV_RV_MOVE_GENERAL_ERROR = 55,//!< Motor reference run general error


    DCL_ERR_UNDEFINED             = 99   //!< The return code was not set
} ReturnCode_t;

/*! Synchronized functions definitions */
typedef enum
{
    SYNC_CMD_RV_REF_RUN = 0,
    SYNC_CMD_RV_SET_MOTOR_STATE = 1,
    SYNC_CMD_RV_SET_MOTOR_CONFIG = 2,
    SYNC_CMD_RV_GET_MOTOR_POSITION = 3,
    SYNC_CMD_RV_GET_LS_CODE = 4,
    SYNC_CMD_RV_SET_TEMP = 5,
    SYNC_CMD_RV_GET_TEMP = 6,
    SYNC_CMD_RV_SET_TEMP_PID = 7,
    SYNC_CMD_RV_GET_TEMP_CTRL_STATE = 8,
    SYNC_CMD_RV_TC_GET_HW_STATUS = 9,
    SYNC_CMD_AL_SET_PRESSURE = 12,
    SYNC_CMD_AL_GET_PRESSURE = 13,
    SYNC_CMD_AL_SET_VALVE = 14,
    SYNC_CMD_AL_PROCEDURE_RELEASE_PRESSURE = 15,
    SYNC_CMD_AL_PROCEDURE_PRESSURE = 16,
    SYNC_CMD_AL_PROCEDURE_DRAINING = 17,
    SYNC_CMD_AL_PROCEDURE_SUCKING_LEVELSENSOR = 18,
    SYNC_CMD_AL_PROCEDURE_VACCUM = 19,
    SYNC_CMD_AL_SET_TEMP = 20,
    SYNC_CMD_AL_GET_TEMP = 21,
    SYNC_CMD_AL_SET_TEMP_PID = 22,
    SYNC_CMD_AL_GET_TEMP_CTRL_STATE = 23,
    //SYNC_CMD_AL_SET_DO_VALVE = 21,
    SYNC_CMD_AL_SET_FAN_STATUS = 24,
    SYNC_CMD_AL_GET_DO_LIFE_TIME = 25,
    SYNC_CMD_AL_TC_GET_HW_STATUS = 26,
    SYNC_CMD_RT_SET_TEMP = 30,
    SYNC_CMD_RT_GET_TEMP = 31,
    SYNC_CMD_RT_SET_TEMP_PID = 32,
    SYNC_CMD_RT_GET_TEMP_CTRL_STATE = 33,
    SYNC_CMD_RT_SET_DO_VALUE = 34,
    SYNC_CMD_RT_GET_DI_VALUE = 35,
    SYNC_CMD_RT_GET_HW_STATUS = 36,
    SYNC_CMD_OVEN_SET_TEMP = 40,
    SYNC_CMD_OVEN_GET_TEMP = 41,
    SYNC_CMD_OVEN_SET_TEMP_PID = 42,
    SYNC_CMD_OVEN_GET_TEMP_CTRL_STATE = 43,
    SYNC_CMD_OVEN_GET_DI_VALUE = 44,
    SYNC_CMD_OVEN_TC_GET_HW_STATUS = 45,
    SYNC_CMD_PER_GET_DI_VALUE = 46,
    SYNC_CMD_PER_SET_DO_VALUE = 47,
    SYNC_CMD_BASE_GET_VOLTAGE = 48,
    SYNC_CMD_BASE_GET_CURRENT = 49,

    SYNC_CMD_TOTAL_NUM = 50
} SyncCmdType_t;

/*! Node state definitions */
typedef enum {
    NODE_STATE_UNDEFINED = 0,    //!< Undefined state
    NODE_STATE_BOOTING   = 1,    //!< Node is in boot loader
    NODE_STATE_STARTUP   = 2,    //!< Firmware initialization
    NODE_STATE_IDENTIFY  = 3,    //!< Node identification
    NODE_STATE_CONFIGURE = 4,    //!< Node configuration
    NODE_STATE_NORMAL    = 5,    //!< Normal operation mode
    NODE_STATE_SERVICE   = 6,    //!< Service mode
    NODE_STATE_SHUTDOWN  = 7,    //!< Shutdown (going to standby)
    NODE_STATE_STANDBY   = 8,    //!< Standby
    NODE_STATE_UPDATE    = 9     //!< Firmware update
} NodeState_t;

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
    //STATION_ACTION_GRAB                 = 0x0b,  //< (will be removed) grab rack at target vessel
    //STATION_ACTION_DISCARD              = 0x0c,  //!< (will be removed) discard rack at target vessel
    //STATION_ACTION_MOVEOVER             = 0x0d,  //!< (will be removed) just move over target vessel
    STATION_ACTION_UNDEF                = 0x0e   //!< undefined
} StationAction_t;


/*! RV tube position */
typedef enum {
    RV_UNDEF   = 0x00,  //!< undefined
    RV_TUBE_1   = 1,  //!< undefined
    RV_TUBE_2   = 3,  //!< undefined
    RV_TUBE_3   = 5,  //!< undefined
    RV_TUBE_4   = 7,  //!< undefined
    RV_TUBE_5   = 9,  //!< undefined
    RV_TUBE_6   = 11,  //!< undefined
    RV_TUBE_7   = 13,  //!< undefined
    RV_TUBE_8   = 15,  //!< undefined
    RV_TUBE_9   = 17,  //!< undefined
    RV_TUBE_10   = 19,  //!< undefined
    RV_TUBE_11   = 21,  //!< undefined
    RV_TUBE_12   = 23,  //!< undefined
    RV_TUBE_13   = 25,  //!< undefined
    RV_TUBE_14   = 27,  //!< undefined
    RV_TUBE_15   = 29,  //!< undefined
    RV_TUBE_16   = 31,  //!< undefined
    RV_SEAL_1   = 2,  //!< undefined
    RV_SEAL_2   = 4,  //!< undefined
    RV_SEAL_3   = 6,  //!< undefined
    RV_SEAL_4   = 8,  //!< undefined
    RV_SEAL_5   = 10,  //!< undefined
    RV_SEAL_6   = 12,  //!< undefined
    RV_SEAL_7   = 14,  //!< undefined
    RV_SEAL_8   = 16,  //!< undefined
    RV_SEAL_9   = 18,  //!< undefined
    RV_SEAL_10   = 20,  //!< undefined
    RV_SEAL_11   = 22,  //!< undefined
    RV_SEAL_12   = 24,  //!< undefined
    RV_SEAL_13   = 26,  //!< undefined
    RV_SEAL_14   = 28,  //!< undefined
    RV_SEAL_15   = 30,  //!< undefined
    RV_SEAL_16   = 32  //!< undefined
} RVPosition_t;

/*! temperature control status */
typedef enum {
    TEMPCTRL_STATUS_UNDEF = 0x00,   //!< status undefined
    TEMPCTRL_STATUS_OFF   = 0x01,   //!< temp. ctrl. is off
    TEMPCTRL_STATUS_ON    = 0x02    //!< temp. ctrl. is active (on)
} TempCtrlStatus_t;

/*! temperature control status */
typedef enum {
    TEMPCTRL_STATE_OFF = 0x00,   //!< status undefined
    TEMPCTRL_STATE_INSIDE_RANGE = 0x01,   //!< status undefined
    TEMPCTRL_STATE_OUTSIDE_RANGE = 0x02,   //!< status undefined
    TEMPCTRL_STATE_ERROR = 0x03   //!< status undefined
} TempCtrlState_t;

/*! temperature control operation */
typedef enum {
    TEMPCTRL_OPMODE_UNDEF = 0x00,   //!< operation mode undefined
    TEMPCTRL_OPMODE_FULL  = 0x01,   //!< operation mode full power
    TEMPCTRL_OPMODE_HOLD  = 0x02    //!< operation mode power safe
} TempCtrlOperatingMode_t;

/*! pressure control status */
typedef enum {
    PRESSURECTRL_STATUS_UNDEF = 0x00,   //!< status undefined
    PRESSURECTRL_STATUS_OFF   = 0x01,   //!< pressure ctrl. is off
    PRESSURECTRL_STATUS_ON    = 0x02    //!< pressure ctrl. is active (on)
} PressureCtrlStatus_t;

/*! pressure control operation mode */
typedef enum {
    PRESSURECTRL_OPMODE_UNDEF  = 0x00,   //!< operation mode undefined
    PRESSURECTRL_OPMODE_FULL   = 0x01,   //!< operation mode full power
    PRESSURECTRL_OPMODE_HOLD   = 0x02,   //!< operation mode power safe
    PRESSURECTRL_OPMODE_VACUUM = 0x08    //!< operation mode vacuuming
} PressureCtrlOperatingMode_t;

/*! pressure control main voltage */
typedef enum {
    PRESSURECTRL_VOLTAGE_UNDEF = 0x00,  //!< status undefined
    PRESSURECTRL_VOLTAGE_220V  = 0x01,  //!< 220V mains voltage
    PRESSURECTRL_VOLTAGE_110V  = 0x02   //!< 110V mains voltage
} PressureCtrlMainsVoltage_t;


/*! temperature control mains voltage */
typedef enum {
    TEMPCTRL_VOLTAGE_UNDEF = 0x00,  //!< status undefined
    TEMPCTRL_VOLTAGE_220V  = 0x01,  //!< 220V mains voltage
    TEMPCTRL_VOLTAGE_110V  = 0x02   //!< 110V mains voltage
} TempCtrlMainsVoltage_t;

/*! temperature control hardware status */
typedef struct {
    quint8 Sensors;           //!< Sensors count
    quint8 Fans;              //!< Fans count
    quint8 Heaters;           //!< Heaters count
    quint8 Pids;              //!< PID parameter set count
    quint16 Current;          //!< Current
    quint8 HeaterSwitchType;  //!< Heater switch type
} TempCtrlHardwareStatus_t;


/*! Sub commands for motor movement requests */
typedef enum {SM_SUBCMD_MOTION_NULL           = 0x00,  //!< no further command information
              SM_SUBCMD_MOTION_ACKN           = 0x01,  //!< request command acknowledge from slave (additionally to action ackn.!)
              SM_SUBCMD_MOTION_SYSCLK_DELAY   = 0x02,  //!< system clock triggered command execution
              SM_SUBCMD_MOTION_POS_DELAY      = 0x03,  //!< start command execution when motor has moved the position offset
              SM_SUBCMD_MOTION_TIME_DELAY     = 0x04,  //!< start command execution with a time delay after receiption
              SM_SUBCMD_MOTION_SPEED_STOP_POS = 0x08   //!< speed '0' command with stop position as additional data
 } StepperMotorSubCommandMotion_t;

typedef qint32 Position_t;         //!< stepper motor position, [half steps]
typedef qint16 Speed_t;            //!< stepper motor speed, [half steps/sec]
typedef quint8 MotionProfileIdx_t; //!< stepper motor motion profile index

/*! Device instance ID definitions */
const quint32 DEVICE_INSTANCE_ID_UNDEFINED      = 0x00000000;  //!< undefine. used for initialization
const quint32 DEVICE_INSTANCE_ID_DEVPROC        = 0x00008000;  //!< the device processing itself
const quint32 DEVICE_INSTANCE_ID_GRAPPLER_1     = 0x00008010;  //!< the left grappler
const quint32 DEVICE_INSTANCE_ID_GRAPPLER_2     = 0x00008011;  //!< the right grappler
const quint32 DEVICE_INSTANCE_ID_LOADER         = 0x00008020;  //!< loader
const quint32 DEVICE_INSTANCE_ID_UNLOADER       = 0x00008021;  //!< unloader
const quint32 DEVICE_INSTANCE_ID_HVESSELS       = 0x00008040;  //!< heated vessels
const quint32 DEVICE_INSTANCE_ID_AGITATION      = 0x00008050;  //!< agitation
const quint32 DEVICE_INSTANCE_ID_RACKTRANSFER   = 0x00008060;  //!< rack transfer
const quint32 DEVICE_INSTANCE_ID_EXHAUST        = 0x00008070;  //!< exhaust, includes fan control an supervision
const quint32 DEVICE_INSTANCE_ID_WATER          = 0x00008080;  //!< water, six valves and the liquid level sensor
const quint32 DEVICE_INSTANCE_ID_INCLINOMETER   = 0x00008090;  //!< inclination
const quint32 DEVICE_INSTANCE_ID_COVERLINE_1    = 0x000080A0;  //!< cover line unit 1
const quint32 DEVICE_INSTANCE_ID_COVERLINE_2    = 0x000080A1;  //!< cover line unit 2
const quint32 DEVICE_INSTANCE_ID_RACK_HANDLING  = 0x000080B0;  //!< rack handling
const quint32 DEVICE_INSTANCE_ID_HOOD           = 0x000080B1;  //!< device cover (hood)
const quint32 DEVICE_INSTANCE_ID_ROTARY_VALVE   = 0x000080C0;   //!< Rotary valve
const quint32 DEVICE_INSTANCE_ID_AIR_LIQUID     = 0x000080C1;   //!< Air liquid system
const quint32 DEVICE_INSTANCE_ID_OVEN           = 0x000080C2;   //!< Oven
const quint32 DEVICE_INSTANCE_ID_RETORT         = 0x000080C3;   //!< Retort
const quint32 DEVICE_INSTANCE_ID_MAIN_CONTROL   = 0x000080C4;   //!< Main Control
const quint32 DEVICE_INSTANCE_ID_CAN_COMMUTOR   = 0x000080C5;   //!< CAN Communicator 

/*! Air-liquid device's temperature control function module*/
typedef enum {
    AL_LEVELSENSOR = 0,
    AL_TUBE1 = 1,
    AL_TUBE2 = 2,
    AL_TEMP_CTRL_NUM = 3
} ALTempCtrlType_t;

/*! Oven device's temperature control function module*/
typedef enum {
    OVEN_TOP = 0,
    OVEN_BOTTOM = 1,
    OVEN_TEMP_CTRL_NUM =2
} OVENTempCtrlType_t;

/*! Retort device's temperature control function module*/
typedef enum {
    RT_BOTTOM = 0,
    RT_SIDE = 1,
    RT_TEMP_CTRL_NUM =2
} RTTempCtrlType_t;

/****************************************************************************/
/*! \class CANObjectKeyLUT
 *
 *  \brief this classes elements containing the can object keys, used for
 *         object identification by string at the colorado project
 *
 */
/****************************************************************************/
class CANObjectKeyLUT
{
public:
    // devices
    static const QString m_DevRotaryValveKey;     //!< rotary valve key
    static const QString m_DevAirLiquidKey;       //!< air-liquid key
    static const QString m_DevOvenKey;            //!< Oven key
    static const QString m_RetortKey;             //!< Retort key
    static const QString m_PeripheryKey;      //!< Periphery device key
    //function modules
    static const QString m_RVMotorKey;               //!< Rotary valve motor
    static const QString m_RVTempCtrlKey;            //!< Rotary valve temperature control
    static const QString m_ALPressureCtrlKey;        //!< Air-liquid pressure control
    static const QString m_ALLevelSensorTempCtrlKey; //!< Air-liquid level sensor temp control
    static const QString m_ALTube1TempCtrlKey;       //!< Air-liquid tube1 temp control
    static const QString m_ALTube2TempCtrlKey;       //!< Air-liquid tube2 temp control
//    static const QString m_ALFanDOKey;               //!< Air-liquid fan digital output
    static const QString m_OvenTopTempCtrlKey;       //!< Oven top temp control
    static const QString m_OvenBottomTempCtrlKey;    //!< Oven bottom temp control
    static const QString m_OvenLidDIKey;            //!< Oven lid digital input
    static const QString m_RetortBottomTempCtrlKey;  //!< Retort bottom temp control
    static const QString m_RetortSideTempCtrlKey;    //!< Retort side temp control
    static const QString m_RetortLockDOKey;          //!< Retort lock digital output
    static const QString m_RetortLockDIKey;          //!< Retort lock digital output
    static const QString m_PerRemoteAlarmCtrlDOKey;  //!< Miscellaneous remote alarm ctrl digital output
    static const QString m_PerLocalAlarmCtrlDOKey;  //!< Miscellaneous remote alarm ctrl digital output
//    static const QString m_PerRemoteAlarmSetDOKey;   //!< Miscellaneous remote alarm set digital output
//    static const QString m_PerRemoteAlarmClearDOKey;  //!< Miscellaneous remote alarm clear digital output
    static const QString m_PerMainRelayDOKey;     //!< Miscellaneous heater relay digital output


    // calling the DeviceProcessing::GetFunctionModule-method
    //!< can object IDs, used for identification by Id
    typedef enum
    {

        FCTMOD_RV_MOTOR               = 0x30003,  //!< Rotary valve motor
        FCTMOD_RV_TEMPCONTROL         = 0x40003,  //!< Rotary valve temperature control

        FCTMOD_AL_PRESSURECTRL        = 0x1000F,  //!< Air-liquid pressure control
        FCTMOD_AL_LEVELSENSORTEMPCTRL = 0x2000F,  //!< Air-liquid level sensor temp control
        FCTMOD_AL_TUBE1TEMPCTRL       = 0x3000F,  //!< Air-liquid tube1 temp control
        FCTMOD_AL_TUBE2TEMPCTRL       = 0x4000F,  //!< Air-liquid tube2 temp control
        FCTMOD_AL_FANDO               = 0x5000F,  //!< Air-liquid fan digital output
        FCTMOD_OVEN_TOPTEMPCTRL       = 0x10005,  //!< Oven top temp control
        FCTMOD_OVEN_BOTTOMTEMPCTRL    = 0x20005,  //!< Oven bottom temp control
        FCTMOD_OVEN_LIDDI             = 0xB000F,  //!< Oven lid digital input
        FCTMOD_RETORT_BOTTOMTEMPCTRL  = 0x30005,  //!< Retort bottom temp control
        FCTMOD_RETORT_SIDETEMPCTRL    = 0x40005,  //!< Retort side temp control
        FCTMOD_RETORT_LOCKDO          = 0x6000F,  //!< Retort lock digital output
        FCTMOD_RETORT_LOCKDI          = 0xA000F,  //!< Retort lock digital input
        FCTMOD_PER_REMOTEALARMCTRLDO  = 0x8000F,  //!< Miscellaneous remote alarm ctrl digital output
        FCTMOD_PER_LOCALALARMCTRLDO   = 0x9000F,  //!< Miscellaneous remote alarm set digital output
        //FCTMOD_PER_REMOTEALARMSETDO   = 0x900F,  //!< Miscellaneous remote alarm set digital output
        //FCTMOD_PER_REMOTEALARMCLEARDO = 0xA00F,  //!< Miscellaneous remote alarm clear digital output
        FCTMOD_PER_MAINRELAYDO        = 0x7000F   //!< Miscellaneous heater relay digital output
    } CANObjectIdentifier_t;
};

}

Q_DECLARE_METATYPE(DeviceControl::ReturnCode_t)

#endif /* DEVICE_CONTROL_GLOBAL_H */

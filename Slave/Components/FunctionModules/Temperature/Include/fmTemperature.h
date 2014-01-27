/****************************************************************************/
/*! \file fmTemperature.h
 *
 *  \brief Functional Module controlling temperature regulation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 29.06.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This module contains the top level source code of a temperature
 *      controller. The code in this file binds different subcomponents, that
 *      are generally needed for temperature regulation. The module controls
 *      various heaters and temperature indicators through the hardware
 *      abstraction layer. It furthermore watches ventilation fans, the
 *      lifetime of heating elements and sets parameters for temperature
 *      regulation.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef FMTEMPERATURE_H
#define FMTEMPERATURE_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

// Temperature module related errors
//! The parameters of the PID controller are not set
#define E_TEMP_PID_NOT_CONFIGURED   BUILD_ERRCODE(MODULE_ID_TEMPERATURE, ERRCLASS_ERROR, 0)
//! The module is heating, operation not permitted
#define E_TEMP_MODULE_ACTIVE        BUILD_ERRCODE(MODULE_ID_TEMPERATURE, ERRCLASS_ERROR, 1)
//! The module is stopped or in standby
#define E_TEMP_MODULE_INACTIVE      BUILD_ERRCODE(MODULE_ID_TEMPERATURE, ERRCLASS_ERROR, 2)
//! Speed of a fan is out of range
#define E_TEMP_FAN_OUT_OF_RANGE     BUILD_ERRCODE(MODULE_ID_TEMPERATURE, ERRCLASS_ERROR, 3)
//! The current through the heating elements is too high or too low
#define E_TEMP_CURRENT_OUT_OF_RANGE BUILD_ERRCODE(MODULE_ID_TEMPERATURE, ERRCLASS_ERROR, 4)
//! Temperature is too high
#define E_TEMP_SENSOR_OUT_OF_RANGE  BUILD_ERRCODE(MODULE_ID_TEMPERATURE, ERRCLASS_ERROR, 5)
//! Temperature sensors are measuring different values
#define E_TEMP_SENSORS_INCONSISTENT BUILD_ERRCODE(MODULE_ID_TEMPERATURE, ERRCLASS_ERROR, 6)
//! Unsupported type of temperature sensor
#define E_TEMP_SENSOR_NOT_SUPPORTED BUILD_ERRCODE(MODULE_ID_TEMPERATURE, ERRCLASS_ERROR, 7)
//! Unsupported type of heater
#define E_TEMP_HEATER_NOT_SUPPORTED BUILD_ERRCODE(MODULE_ID_TEMPERATURE, ERRCLASS_ERROR, 8)


// Temperature module related CAN identifiers
//! Sets temperature related parameters
#define MSG_TEMP_SET_TEMP             BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)
//! Sets the fan speed watchdog parameters
#define MSG_TEMP_SET_FAN_WATCHDOG     BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)
//! Sets the heater current watchdog parameters
#define MSG_TEMP_SET_CURRENT_WATCHDOG BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)
//! Sets the PID gain and time parameters
#define MSG_TEMP_SET_PID_PARAMS       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 1)
//! Resets the operating timers of the heaters
#define MSG_TEMP_SET_HEATER_TIME      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 1)

//! Requests temperature related parameters
#define MSG_TEMP_REQ_TEMP             BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 1)
//! Requests the PID gain and time parameters
#define MSG_TEMP_REQ_PID_PARAMS       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 1)
//! Requests the operating time of the heaters
#define MSG_TEMP_REQ_HEATER_TIME      BUILD_CAN_ID(CMD_CLASS_SERVICE, 12, 1)
//! Requests service related sensor temperature
#define MSG_TEMP_REQ_SERVICE_SENSOR   BUILD_CAN_ID(CMD_CLASS_SERVICE, 13, 1)
//! Requests service related fan speeds
#define MSG_TEMP_REQ_SERVICE_FAN      BUILD_CAN_ID(CMD_CLASS_SERVICE, 14, 1)
//! Requests hardware configuration and the heater current
#define MSG_TEMP_REQ_HARDWARE         BUILD_CAN_ID(CMD_CLASS_SERVICE, 15, 1)

//! Responds temperature related parameters
#define MSG_TEMP_RESP_TEMP            BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 0)
//! Responds the PID gain and time paremeters
#define MSG_TEMP_RESP_PID_PARAMS      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 0)
//! Responds the operating time of the heaters
#define MSG_TEMP_RESP_HEATER_TIME     BUILD_CAN_ID(CMD_CLASS_SERVICE, 12, 0)
//! Responds service related sensor temperature
#define MSG_TEMP_RESP_SERVICE_SENSOR  BUILD_CAN_ID(CMD_CLASS_SERVICE, 13, 0)
//! Responds service related fan speeds
#define MSG_TEMP_RESP_SERVICE_FAN     BUILD_CAN_ID(CMD_CLASS_SERVICE, 14, 0)
//! Responds hardware configuration and the heater current
#define MSG_TEMP_RESP_HARDWARE        BUILD_CAN_ID(CMD_CLASS_SERVICE, 15, 0)

//! Notification that marks the end of the auto tuning process 
#define MSG_TEMP_NOTI_AUTO_TUNE       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 16, 0)
//! Notification sent when the temperature enters the required range
#define MSG_TEMP_NOTI_IN_RANGE        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 17, 0)
//! Notification sent when the temperature leaves the required range
#define MSG_TEMP_NOTI_OUT_OF_RANGE    BUILD_CAN_ID(CMD_CLASS_FUNCTION, 18, 0)
//! Notification sent when preset temperature variation slope reached
#define MSG_TEMP_NOTI_SLOPE_DETECTED  BUILD_CAN_ID(CMD_CLASS_FUNCTION, 19, 0)

//! Sets the switch state of the heaters
#define MSG_TEMP_SET_SWITCH_STATE     BUILD_CAN_ID(CMD_CLASS_FUNCTION, 20, 1)

//! Sets the AC heater current watchdog parameters
#define MSG_TEMP_SET_AC_CURRENT_WATCHDOG \
                                      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 21, 1)

//! Sets the extra AC heater current watchdog parameters
#define MSG_TEMP_SET_AC_CURRENT_WATCHDOG_EXT \
                                      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 22, 1)

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t tempInitializeModule (UInt16 ModuleID, UInt16 NumberOfInstances);
void tempRegisterDebugNames  (UInt16 ModuleID);

//****************************************************************************/

#endif /*FMTEMPERATURE_H*/

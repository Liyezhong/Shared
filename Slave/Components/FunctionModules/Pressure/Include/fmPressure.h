/****************************************************************************/
/*! \file fmPressure.h
 *
 *  \brief Functional Module controlling pressure regulation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *      This module contains the top level source code of a pressure
 *      controller. The code in this file binds different subcomponents, that
 *      are generally needed for pressure regulation. The module controls
 *      various pumps and pressure indicators through the hardware
 *      abstraction layer. It furthermore watches ventilation fans, the
 *      lifetime of heating elements and sets parameters for pressure
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

#ifndef FMPRESSURE_H
#define FMPRESSURE_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

// Pressure module related errors
//! The parameters of the PID controller are not set
#define E_PRESS_PID_NOT_CONFIGURED   BUILD_ERRCODE(MODULE_ID_PRESSURE, ERRCLASS_ERROR, 0)
//! The module is pumping, operation not permitted
#define E_PRESS_MODULE_ACTIVE        BUILD_ERRCODE(MODULE_ID_PRESSURE, ERRCLASS_ERROR, 1)
//! The module is stopped or in standby
#define E_PRESS_MODULE_INACTIVE      BUILD_ERRCODE(MODULE_ID_PRESSURE, ERRCLASS_ERROR, 2)
//! The current through the pump elements is too high or too low
#define E_PRESS_CURRENT_OUT_OF_RANGE BUILD_ERRCODE(MODULE_ID_PRESSURE, ERRCLASS_ERROR, 3)
//! Pressure is too high
#define E_PRESS_SENSOR_OUT_OF_RANGE  BUILD_ERRCODE(MODULE_ID_PRESSURE, ERRCLASS_ERROR, 4)
//! Pressure sensors are measuring different values
#define E_PRESS_SENSORS_INCONSISTENT BUILD_ERRCODE(MODULE_ID_PRESSURE, ERRCLASS_ERROR, 5)
//! Unsupported type of pressure sensor
#define E_PRESS_SENSOR_NOT_SUPPORTED BUILD_ERRCODE(MODULE_ID_PRESSURE, ERRCLASS_ERROR, 6)
//! The parameters of the PWM controller are not set
#define E_PRESS_PWM_NOT_CONFIGURED   BUILD_ERRCODE(MODULE_ID_PRESSURE, ERRCLASS_ERROR, 7)


// Pressure module related CAN identifiers
//! Sets pressure related parameters
#define MSG_PRESS_SET_PRESSURE         BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)
//! Sets the fan speed watchdog parameters (Obsolete)
#define MSG_PRESS_SET_FAN_WATCHDOG     BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)
//! Sets the pump current watchdog parameters
#define MSG_PRESS_SET_CURRENT_WATCHDOG BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)
//! Sets the PID gain and time parameters
#define MSG_PRESS_SET_PID_PARAMS       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 1)
//! Resets the operating timers of the pumps 
#define MSG_PRESS_SET_PUMP_TIME      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 9, 1)

//! Requests pressure related parameters
#define MSG_PRESS_REQ_PRESSURE         BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 1)
//! Requests the PID gain and time parameters
#define MSG_PRESS_REQ_PID_PARAMS       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 1)
//! Requests the operating time of the pumps
#define MSG_PRESS_REQ_PUMP_TIME        BUILD_CAN_ID(CMD_CLASS_SERVICE, 12, 1)
//! Requests service related sensor pressure
#define MSG_PRESS_REQ_SERVICE_SENSOR   BUILD_CAN_ID(CMD_CLASS_SERVICE, 13, 1)
//! Requests service related fan speeds
#define MSG_PRESS_REQ_SERVICE_FAN      BUILD_CAN_ID(CMD_CLASS_SERVICE, 14, 1)
//! Requests hardware configuration and the pump current
#define MSG_PRESS_REQ_HARDWARE         BUILD_CAN_ID(CMD_CLASS_SERVICE, 15, 1)

//! Responds pressure related parameters
#define MSG_PRESS_RESP_PRESSURE        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 10, 0)
//! Responds the PID gain and time paremeters
#define MSG_PRESS_RESP_PID_PARAMS      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 11, 0)
//! Responds the operating time of the pumps
#define MSG_PRESS_RESP_PUMP_TIME       BUILD_CAN_ID(CMD_CLASS_SERVICE, 12, 0)
//! Responds service related sensor pressure
#define MSG_PRESS_RESP_SERVICE_SENSOR  BUILD_CAN_ID(CMD_CLASS_SERVICE, 13, 0)
//! Responds service related fan speeds
#define MSG_PRESS_RESP_SERVICE_FAN     BUILD_CAN_ID(CMD_CLASS_SERVICE, 14, 0)
//! Responds hardware configuration and the pump current
#define MSG_PRESS_RESP_HARDWARE        BUILD_CAN_ID(CMD_CLASS_SERVICE, 15, 0)

//! Notification that marks the end of the auto tuning process 
#define MSG_PRESS_NOTI_AUTO_TUNE       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 16, 0)
//! Notification sent when the pressure enters the required range
#define MSG_PRESS_NOTI_IN_RANGE        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 17, 0)
//! Notification sent when the pressure leaves the required range
#define MSG_PRESS_NOTI_OUT_OF_RANGE    BUILD_CAN_ID(CMD_CLASS_FUNCTION, 18, 0)

//! Set valve status 
#define MSG_PRESS_SET_VALVE            BUILD_CAN_ID(CMD_CLASS_FUNCTION, 19, 1)

//! Set valve status 
#define MSG_PRESS_SET_CALIBRATION      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 20, 1)

//! Set PWM controller parameters
#define MSG_PRESS_SET_PWM_PARAMS       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 21, 1)

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t pressInitializeModule (UInt16 ModuleID, UInt16 NumberOfInstances);
void pressRegisterDebugNames  (UInt16 ModuleID);

//****************************************************************************/

#endif /*FMPRESSURE_H*/

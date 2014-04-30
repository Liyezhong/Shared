/****************************************************************************/
/*! \file DeviceControlEventCodes.h
 *
 *  \brief  Error codes of the device control layer
 *
 *   Version: 0.1
 *   Date:    01.08.2010
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the error and event defines of the device
 *       control layer.
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

#ifndef DEVICECONTROL_DEVICECONTROLEVENTCODES_H
#define DEVICECONTROL_DEVICECONTROLEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

namespace DeviceControl
{

//! Event break normal operation mode
const quint32 EVENT_DEVICECONTROL_INFO_BREAK_NORMAL_OP  = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0002;
//! Could not activate a device processing task
const quint32 EVENT_DEVICECONTROL_ERROR_ACTIVATE_TASK   = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0003;
//! Problems with heartbeat distribution
const quint32 EVENT_DEVICECONTROL_ERROR_HEARTBEAT       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0004;
//! Writing a CAN message failed
const quint32 EVENT_DEVICECONTROL_ERROR_CANBUS_WRITE    = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0005;

//! Hardware configuration, open config file failed
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_OPEN_FAILED      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0101;
//! Hardware configuration, general parsing error
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_PARSE            = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0102;
//! Hardware configuration, format error
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT           = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0103;
//! Hardware configuration, version confict
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_VERSION          = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0104;
//! Hardware configuration, section master
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT_MST       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0105;
//! Hardware configuration, can interface missed
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT_CAN       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0106;
//! Hardware configuration, section slave
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT_SLV       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0107;
//! Hardware configuration, section function module
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT_FCT       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0108;
//! Hardware configuration, section device
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT_DEV       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0109;
//! CAN message configuration, open config file failed
const quint32 EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_OPEN_FAILED = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x010A;
//! CAN message configuration, general parsing error
const quint32 EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_PARSE       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x010B;
//! CAN message configuration, format error
const quint32 EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x010C;
//! CAN message configuration, version confict
const quint32 EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_VERSION     = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x010D;
//! CAN message configuration, section base module
const quint32 EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_BASE = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x010E;
//! CAN message configuration, section function module
const quint32 EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_FCT  = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x010F;
//! CAN message configuration, section message
const quint32 EVENT_DEVICECONTROL_ERROR_CAN_MSG_CFG_FORMAT_MSG  = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0110;

//! Timeout between base module initialization and 'HardwareID' receipt
const quint32 EVENT_DEVICECONTROL_ERROR_TIMEOUT_INIT            = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0201;
//! Timeout between 'ConfigurationReq' and 'Configuration' receipt
const quint32 EVENT_DEVICECONTROL_ERROR_TIMEOUT_CONFIG          = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0202;
//! The number of channels read from the hardware configuration file and the real number of channels differ
const quint32 EVENT_DEVICECONTROL_ERROR_INIT_CHANNEL_COUNT      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0203;
//! The module information read from the configuration file does not match the real information
const quint32 EVENT_DEVICECONTROL_ERROR_CONFIG_INVALID_MODULE   = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0204;
//! The CAN message 'Configuration' was received while it was not expected
const quint32 EVENT_DEVICECONTROL_ERROR_CONFIG_UNEXP_CANMSG     = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0205;
//! Timeout while the software tried to configure the node's function modules
const quint32 EVENT_DEVICECONTROL_ERROR_TIMEOUT_FCT_CONFIG      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0206;
//! Node heartbeat timeout
const quint32 EVENT_DEVICECONTROL_ERROR_HEARTBEAT_TIMEOUT       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0207;
//! The can message HardwareID was received within the minimal time delay
const quint32 EVENT_DEVICECONTROL_ERROR_HARDWAREID_INTERVAL     = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0208;
//! The can message HardwareID was received although the node is already configured
const quint32 EVENT_DEVICECONTROL_ERROR_HWID_REC_INVALID_STATE  = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0209;
//! Invalid state
const quint32 EVENT_DEVICECONTROL_ERROR_INVALID_STATE           = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x020A;
//! Null pointer access
const quint32 EVENT_DEVICECONTROL_ERROR_NULL_PTR_ACCESS         = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x020B;
//! Function call failed
const quint32 EVENT_DEVICECONTROL_ERROR_FCT_CALL_FAILED         = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x020C;
//! A can message with invalid data length code was received
const quint32 EVENT_DEVICECONTROL_ERROR_CANMSG_INVALID_DLC      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x020D;
//! CAN node is inactive
const quint32 EVENT_DEVICECONTROL_ERROR_CANNODE_INACTIVE        = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x020E;

//!@{
//! Generic device error _General
const quint32 EVENT_DEVICECONTROL_ERROR_DEVICE                  = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0300;

const quint32 EVENT_DEVICECONTROL_ERROR_NULL_PTR                = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x01;
const quint32 EVENT_DEVICECONTROL_ERROR_DEVICE_INITIALIZING     = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x02;
const quint32 EVENT_DEVICECONTROL_ERROR_GET_CIRCUMFERENCE       = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x03;
const quint32 EVENT_DEVICECONTROL_ERROR_GET_POSITION            = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x04;
const quint32 EVENT_DEVICECONTROL_ERROR_GET_SPEED_PROFILE       = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x05;
const quint32 EVENT_DEVICECONTROL_ERROR_SET_CIRCUMFERENCE       = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x06;
const quint32 EVENT_DEVICECONTROL_ERROR_SET_POSITION            = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x07;
const quint32 EVENT_DEVICECONTROL_ERROR_SET_SPEED_PROFILE       = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x08;
const quint32 EVENT_DEVICECONTROL_ERROR_RESET_POSITION          = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x09;
const quint32 EVENT_DEVICECONTROL_ERROR_DEVICE_NAME_UNKNOWN     = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x0A;
const quint32 EVENT_DEVICECONTROL_ERROR_DEVICE_CONFIGURING      = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x0B;

//! Device lost connection to one of it's base modules
const quint32 EVENT_DEVICECONTROL_ERROR_DEVICE_CONNECTION_LOST  = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x10;
//! Device detected that one of it's base modules was reseted
const quint32 EVENT_DEVICECONTROL_ERROR_DEVICE_MODULE_RESET     = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x11;

const quint32 EVENT_DEVICECONTROL_ERROR_THREAD_EXIT             = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x20;
const quint32 EVENT_DEVICECONTROL_ERROR_STATEMACHINE            = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x21;

//! Device has a defect stepper motor module
const quint32 EVENT_DEVICECONTROL_ERROR_DEVICE_STEPPER_DEFECT   = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x37;
//! Device has a defect temperature control module
const quint32 EVENT_DEVICECONTROL_ERROR_DEVICE_TEMPCTRL_DEFECT  = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x3B;
//!@}

//!@{
//! Generic device info _General
const quint32 EVENT_DEVICECONTROL_INFO                          = EVENT_DEVICECONTROL_ERROR_DEVICE + 0x80;

const quint32 EVENT_DEVICECONTROL_INFO_DEVICE_CREATED           = EVENT_DEVICECONTROL_INFO + 0x01;
const quint32 EVENT_DEVICECONTROL_INFO_DEVICE_DESTROYED         = EVENT_DEVICECONTROL_INFO + 0x02;
const quint32 EVENT_DEVICECONTROL_INFO_DEVICE_SHUTDOWN          = EVENT_DEVICECONTROL_INFO + 0x03;
const quint32 EVENT_DEVICECONTROL_INFO_THREAD_STARTED           = EVENT_DEVICECONTROL_INFO + 0x04;
const quint32 EVENT_DEVICECONTROL_INFO_THREAD_EXITED            = EVENT_DEVICECONTROL_INFO + 0x05;
const quint32 EVENT_DEVICECONTROL_INFO_STATE_MACHINE_STARTED    = EVENT_DEVICECONTROL_INFO + 0x06;
const quint32 EVENT_DEVICECONTROL_INFO_CONFIGURED               = EVENT_DEVICECONTROL_INFO + 0x07;
const quint32 EVENT_DEVICECONTROL_INFO_INITIALIZED              = EVENT_DEVICECONTROL_INFO + 0x08;
const quint32 EVENT_DEVICECONTROL_INFO_REINITIALIZING           = EVENT_DEVICECONTROL_INFO + 0x09;
//!@}

//!@{
//! Device
//!@{
//! Generic device error _Specific
const quint32 EVENT_DCL_ERROR_AGITATION                         = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0400;
const quint32 EVENT_DCL_ERROR_DRAWER                            = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0500;
const quint32 EVENT_DCL_ERROR_XYZ                               = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0600;
const quint32 EVENT_DCL_ERROR_RACK_TRANSFER                     = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0700;
const quint32 EVENT_DCL_ERROR_OVEN                              = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0800;
const quint32 EVENT_DCL_ERROR_HEADTED_CUVETTES                  = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0900;
const quint32 EVENT_DCL_ERROR_EXHAUST                           = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0A00;
const quint32 EVENT_DCL_ERROR_HOOD                              = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0B00;
const quint32 EVENT_DCL_ERROR_LIGHT                             = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0C00;
const quint32 EVENT_DCL_ERROR_WATER                             = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0D00;
const quint32 EVENT_DCL_ERROR_SLIDE_ID                          = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0E00;
const quint32 EVENT_DCL_ERROR_CONSUMABLES                       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0F00;
const quint32 EVENT_DCL_ERROR_XYZG                              = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x1000;

const quint32 EVENT_DCL_ERROR_READRACKRFID                      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x8100;
const quint32 EVENT_DCL_ERROR_LEVELSENSOR                       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x8200;
const quint32 EVENT_DCL_ERROR_STEPPERMOTOR                      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x8300;
const quint32 EVENT_DCL_ERROR_JOYSTICK                          = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x8400;
//!@}

//!@{
//! Agitation error codes
const quint32 EVENT_DCL_ERROR_AGITATION_ROTATE                  = EVENT_DCL_ERROR_AGITATION + 0x01;
const quint32 EVENT_DCL_ERROR_AGITATION_STOP                    = EVENT_DCL_ERROR_AGITATION + 0x02;
//!@}

//!@{
//! Drawer error codes
const quint32 EVENT_DCL_ERROR_LOADER_OPEN                       = EVENT_DCL_ERROR_DRAWER + 0x01;
const quint32 EVENT_DCL_ERROR_UNLOADER_OPEN                     = EVENT_DCL_ERROR_DRAWER + 0x02;
const quint32 EVENT_DCL_ERROR_LOADER_CLOSE                      = EVENT_DCL_ERROR_DRAWER + 0x03;
const quint32 EVENT_DCL_ERROR_UNLOADER_CLOSE                    = EVENT_DCL_ERROR_DRAWER + 0x04;
const quint32 EVENT_DCL_ERROR_DRAWER_BUTTON                     = EVENT_DCL_ERROR_DRAWER + 0x05;
const quint32 EVENT_DCL_ERROR_EXT_LOADER_PARK                   = EVENT_DCL_ERROR_DRAWER + 0X06;
const quint32 EVENT_DCL_ERROR_EXT_LOADER_CLOSE                  = EVENT_DCL_ERROR_DRAWER + 0X07;
const quint32 EVENT_DCL_ERROR_EXT_LOADER_OPEN                   = EVENT_DCL_ERROR_DRAWER + 0x08;

const quint32 EVENT_DCL_ERROR_PERMANENT_LOADER                  = EVENT_DCL_ERROR_DRAWER + 0x80;
const quint32 EVENT_DCL_ERROR_PERMANENT_UNLOADER                = EVENT_DCL_ERROR_DRAWER + 0x81;
//!@}

//!@{
//! XYZ error codes
const quint32 EVENT_DCL_ERROR_XYZ_MOVE_EMPTY                    = EVENT_DCL_ERROR_XYZ + 0x01;
const quint32 EVENT_DCL_ERROR_XYZ_MOVE_RACK                     = EVENT_DCL_ERROR_XYZ + 0x02;
const quint32 EVENT_DCL_ERROR_XYZ_LET_DOWN_RACK                 = EVENT_DCL_ERROR_XYZ + 0x03;
const quint32 EVENT_DCL_ERROR_XYZ_PULL_UP_RACK                  = EVENT_DCL_ERROR_XYZ + 0x04;
const quint32 EVENT_DCL_ERROR_XYZ_ATTACH_RACK                   = EVENT_DCL_ERROR_XYZ + 0x05;
const quint32 EVENT_DCL_ERROR_XYZ_DETACH_RACK                   = EVENT_DCL_ERROR_XYZ + 0x06;
const quint32 EVENT_DCL_ERROR_XYZ_COUNT_SLIDES                  = EVENT_DCL_ERROR_XYZ + 0x07;
const quint32 EVENT_DCL_ERROR_XYZ_MOVE_TO_SAFE_POSITION         = EVENT_DCL_ERROR_XYZ + 0x08;
const quint32 EVENT_DCL_ERROR_XYZ_READ_LIQUID_LEVEL             = EVENT_DCL_ERROR_XYZ + 0x09;
const quint32 EVENT_DCL_ERROR_XYZ_TEACH_LEVEL_SENSOR            = EVENT_DCL_ERROR_XYZ + 0x0A;
const quint32 EVENT_DCL_ERROR_XYZ_SET_STATION_POSITION          = EVENT_DCL_ERROR_XYZ + 0x0B;

const quint32 EVENT_DCL_ERROR_XYZ_X_AXIS                        = EVENT_DCL_ERROR_XYZ + 0x10;
const quint32 EVENT_DCL_ERROR_XYZ_Y_AXIS                        = EVENT_DCL_ERROR_XYZ + 0x11;
const quint32 EVENT_DCL_ERROR_XYZ_Z_AXIS                        = EVENT_DCL_ERROR_XYZ + 0x12;
//!@}

//!@{
//! XyzG error codes
const quint32 EVENT_DCL_ERROR_XYZG_MOVE_EMPTY                   = EVENT_DCL_ERROR_XYZG + 0x01;
const quint32 EVENT_DCL_ERROR_XYZG_MOVE_RACK                    = EVENT_DCL_ERROR_XYZG + 0x02;
const quint32 EVENT_DCL_ERROR_XYZG_ATTACH_RACK                  = EVENT_DCL_ERROR_XYZG + 0x03;
const quint32 EVENT_DCL_ERROR_XYZG_DETACH_RACK                  = EVENT_DCL_ERROR_XYZG + 0x04;

const quint32 EVENT_DCL_ERROR_XYZG_MOVE_GRABBLER                = EVENT_DCL_ERROR_XYZG + 0x10;
//!@}

//!@{
//! Rack Transfer error codes
//const quint32 EVENT_DCL_ERROR_RACK_XX                         = EVENT_DCL_ERROR_RACK_TRANSFER + 0x01;
//!@}

//!@{
//! Oven error codes
//const quint32 EVENT_DCL_ERROR_OVEN_XX                         = EVENT_DCL_ERROR_OVEN + 0x01;
//!@}

//!@{
//! Heated cuvettes error codes
const quint32 EVENT_DCL_ERROR_HEATEDCUVETTES_RANGE              = EVENT_DCL_ERROR_HEADTED_CUVETTES + 0x01;
const quint32 EVENT_DCL_ERROR_HEATEDCUVETTES_TEMPERATURESET     = EVENT_DCL_ERROR_HEADTED_CUVETTES + 0x02;
const quint32 EVENT_DCL_ERROR_HEATEDCUVETTES_TEMPERATUREGET     = EVENT_DCL_ERROR_HEADTED_CUVETTES + 0x03;
const quint32 EVENT_DCL_ERROR_HEATEDCUVETTES_MODE               = EVENT_DCL_ERROR_HEADTED_CUVETTES + 0x04;
//!@}

//!@{
//! Exhaust error codes
const quint32 EVENT_DCL_ERROR_EXHAUST_AIRFLOW                   = EVENT_DCL_ERROR_EXHAUST + 0x01;
const quint32 EVENT_DCL_ERROR_EXHAUST_FANCURRENT_1              = EVENT_DCL_ERROR_EXHAUST + 0x02;
const quint32 EVENT_DCL_ERROR_EXHAUST_FANCURRENT_2              = EVENT_DCL_ERROR_EXHAUST + 0x03;
//!@}

//!@{
//! Hood error codes
//const quint32 EVENT_DCL_ERROR_HOOD_XX                              = EVENT_DCL_ERROR_HOOD + 0x01;
//!@}

//!@{
//! Light error codes
//const quint32 EVENT_DCL_ERROR_LIGHT_XX                             = EVENT_DCL_ERROR_LIGHT + 0x01;
//!@}

//!@{
//! Water error codes
const quint32 EVENT_DCL_ERROR_WATER_LEVEL                       = EVENT_DCL_ERROR_WATER + 0x01;
const quint32 EVENT_DCL_ERROR_WATER_OUTPUT                      = EVENT_DCL_ERROR_WATER + 0x02;
const quint32 EVENT_DCL_ERROR_WATER_SENSOR_TEACH                = EVENT_DCL_ERROR_WATER + 0x03;
//!@}

//!@{
//! Slide ID error codes
//const quint32 EVENT_DCL_ERROR_SLIDE_ID_XX                          = EVENT_DCL_ERROR_SLIDE_ID + 0x01;
//!@}

//!@{
//! Consumables error codes
const quint32 EVENT_DCL_ERROR_CONSUMABLES_SETLOGIN              = EVENT_DCL_ERROR_CONSUMABLES + 0x01;
const quint32 EVENT_DCL_ERROR_CONSUMABLES_READUID               = EVENT_DCL_ERROR_CONSUMABLES + 0x02;
const quint32 EVENT_DCL_ERROR_CONSUMABLES_READ_DATA             = EVENT_DCL_ERROR_CONSUMABLES + 0x03;
const quint32 EVENT_DCL_ERROR_CONSUMABLES_WRITE_DATA            = EVENT_DCL_ERROR_CONSUMABLES + 0x04;
const quint32 EVENT_DCL_ERROR_CONSUMABLES_LOCK_DATA             = EVENT_DCL_ERROR_CONSUMABLES + 0x05;
const quint32 EVENT_DCL_ERROR_CONSUMABLES_SET_STATE_OFF         = EVENT_DCL_ERROR_CONSUMABLES + 0x06;
//!@}
//!@}

//!@{
//! Function module error codes
//!@{
//! RFID error codes
const quint32 EVENT_DCL_ERROR_READRACKRFID_SETLOGIN             = EVENT_DCL_ERROR_READRACKRFID + 0x01;
const quint32 EVENT_DCL_ERROR_READRACKRFID_READUID              = EVENT_DCL_ERROR_READRACKRFID + 0x02;
const quint32 EVENT_DCL_ERROR_READRACKRFID_DATA                 = EVENT_DCL_ERROR_READRACKRFID + 0x03;
//!@}

//!@{
//! Level sensor error codes
const quint32 EVENT_DCL_ERROR_LEVELSENSOR_MOVE_STATION          = EVENT_DCL_ERROR_LEVELSENSOR + 0x01;
const quint32 EVENT_DCL_ERROR_LEVELSENSOR_READ_LEVEL            = EVENT_DCL_ERROR_LEVELSENSOR + 0x02;
const quint32 EVENT_DCL_ERROR_LEVELSENSOR_MOVE_MAX_STATION      = EVENT_DCL_ERROR_LEVELSENSOR + 0x03;
const quint32 EVENT_DCL_ERROR_LEVELSENSOR_MOVE_MIN_STATION      = EVENT_DCL_ERROR_LEVELSENSOR + 0x04;
const quint32 EVENT_DCL_ERROR_LEVELSENSOR_TEACH_SENSOR          = EVENT_DCL_ERROR_LEVELSENSOR + 0x05;
const quint32 EVENT_DCL_ERROR_LEVELSENSOR_ENABLE_SENSOR         = EVENT_DCL_ERROR_LEVELSENSOR + 0x06;
//!@}

//!@{
//!Stepper motor error codes
const quint32 EVENT_DCL_ERROR_STEPPER_SET_STATE                 = EVENT_DCL_ERROR_STEPPERMOTOR + 0x01;
const quint32 EVENT_DCL_ERROR_STEPPER_REFERENCE_RUN             = EVENT_DCL_ERROR_STEPPERMOTOR + 0x02;
const quint32 EVENT_DCL_ERROR_STEPPER_INIT_POSITION             = EVENT_DCL_ERROR_STEPPERMOTOR + 0x03;
const quint32 EVENT_DCL_ERROR_STEPPER_MOVE                      = EVENT_DCL_ERROR_STEPPERMOTOR + 0x04;
const quint32 EVENT_DCL_ERROR_STEPPER_STOP                      = EVENT_DCL_ERROR_STEPPERMOTOR + 0x05;
const quint32 EVENT_DCL_ERROR_STEPPER_ROTATE                    = EVENT_DCL_ERROR_STEPPERMOTOR + 0x06;
const quint32 EVENT_DCL_ERROR_STEPPER_DISABLE                   = EVENT_DCL_ERROR_STEPPERMOTOR + 0x07;
const quint32 EVENT_DCL_ERROR_STEPPER_SWITCHCODE                = EVENT_DCL_ERROR_STEPPERMOTOR + 0x08;
//!@}

//!@{
//! Adjusment(Joystick) error codes
const quint32 EVENT_DCL_ERROR_JOYSTICK_SET_STATE                = EVENT_DCL_ERROR_JOYSTICK + 0x01;
const quint32 EVENT_DCL_ERROR_JOYSTICK_MOVE_TO_STATION          = EVENT_DCL_ERROR_JOYSTICK + 0x02;
const quint32 EVENT_DCL_ERROR_JOYSTICK_MOVE_DOWN                = EVENT_DCL_ERROR_JOYSTICK + 0x03;
const quint32 EVENT_DCL_ERROR_JOYSTICK_MOVE_RAPID               = EVENT_DCL_ERROR_JOYSTICK + 0x04;
const quint32 EVENT_DCL_ERROR_JOYSTICK_MOVE_BABY_STEP           = EVENT_DCL_ERROR_JOYSTICK + 0x05;
const quint32 EVENT_DCL_ERROR_JOYSTICK_MOVE_IDLE                = EVENT_DCL_ERROR_JOYSTICK + 0x06;
const quint32 EVENT_DCL_ERROR_JOYSTICK_ERROR_HANDLING           = EVENT_DCL_ERROR_JOYSTICK + 0x07;
//!@}
//!@}

} //namespace

#endif /* DEVICECONTROL_DEVICECONTROLEVENTCODES_H */

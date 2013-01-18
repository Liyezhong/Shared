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

//! Event start service
const quint32 EVENT_DEVICECONTROL_ERROR_START_DIAG      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0001;
//! Event break normal operation mode
const quint32 EVENT_DEVICECONTROL_ERROR_BREAK_NORMAL_OP = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0002;
//! Could not activate a device processing task
const quint32 EVENT_DEVICECONTROL_ERROR_ACTIVATE_TASK   = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0003;
//! Problems with heartbeat distribution
const quint32 EVENT_DEVICECONTROL_ERROR_HEARTBEAT       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0004;
//! Writing a CAN message failed
const quint32 EVENT_DEVICECONTROL_ERROR_CANBUS_WRITE    = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0005;
//! Event notification diagnostic finished
const quint32 EVENT_DEVICECONTROL_ERROR_DIAGNOSTIC      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0006;

//! hardware configuration, open config file failed
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_OPEN_FAILED      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0101;
//! hardware configuration, general parsing error
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_PARSE            = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0102;
//! hardware configuration, format error
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT           = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0103;
//! hardware configuration, version confict
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_VERSION          = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0104;
//! hardware configuration, section master
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT_MST       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0105;
//! hardware configuration, can interface missed
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT_CAN       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0106;
//! hardware configuration, section slave
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT_SLV       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0107;
//! hardware configuration, section function module
const quint32 EVENT_DEVICECONTROL_ERROR_HW_CFG_FORMAT_FCT       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0108;
//! hardware configuration, section device
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
//! error reading process settings file
const quint32 EVENT_DEVICECONTROL_ERROR_PROCSETTING_OPEN_FAILED = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0111;
//! parser error while reading process settings file
const quint32 EVENT_DEVICECONTROL_ERROR_PROCSETTING_PARSE       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0112;
//! format error at process settings file
const quint32 EVENT_DEVICECONTROL_ERROR_PROCSETTING_FORMAT      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0113;
//! version error at process settings file
const quint32 EVENT_DEVICECONTROL_ERROR_PROCSETTING_VERSION     = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0114;

//! timeout between CANNode initialization and 'HardwareID' receipt
const quint32 EVENT_DEVICECONTROL_ERROR_TIMEOUT_INIT            = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0201;
//! timeout between 'ConfigurationReq' and 'Configuration' receipt
const quint32 EVENT_DEVICECONTROL_ERROR_TIMEOUT_CONFIG          = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0202;
//! the number if channels read from hardware configuration file and the number of channels
const quint32 EVENT_DEVICECONTROL_ERROR_INIT_CHANNEL_COUNT      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0203;
//! the module information as read from configuration file does not match to the information
const quint32 EVENT_DEVICECONTROL_ERROR_CONFIG_INVALID_MODULE   = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0204;
//! The CAN message 'Configuration' was received while it was not expected
const quint32 EVENT_DEVICECONTROL_ERROR_CONFIG_UNEXP_CANMSG     = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0205;
//! timeout while the software tried to configure the node's function modules
const quint32 EVENT_DEVICECONTROL_ERROR_TIMEOUT_FCT_CONFIG      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0206;
//! node heartbeat timeout
const quint32 EVENT_DEVICECONTROL_ERROR_HEARTBEAT_TIMEOUT       = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0207;
//! the can message HardwareID was received within the minimal time delay
const quint32 EVENT_DEVICECONTROL_ERROR_HARDWAREID_INTERVAL     = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0208;
//! the can message HardwareID was received although the node is already configured
const quint32 EVENT_DEVICECONTROL_ERROR_HWID_REC_INVALID_STATE  = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0209;
//! invalid state
const quint32 EVENT_DEVICECONTROL_ERROR_INVALID_STATE           = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x020A;
//! null pointer access
const quint32 EVENT_DEVICECONTROL_ERROR_NULL_PTR_ACCESS         = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x020B;
//! function call failed
const quint32 EVENT_DEVICECONTROL_ERROR_FCT_CALL_FAILED         = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x020C;
//! a can message with invalid data length code was received ( <> expected)
const quint32 EVENT_DEVICECONTROL_ERROR_CANMSG_INVALID_DLC      = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x020D;

//! Generic device error
const quint32 EVENT_DEVICECONTROL_ERROR_DEVICE  = EVENT_GROUP_PLATFORM_DEVICECONTROL + 0x0301;

} //namespace

#endif /* DEVICECONTROL_DEVICECONTROLEVENTCODES_H */

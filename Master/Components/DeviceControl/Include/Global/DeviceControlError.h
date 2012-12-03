/****************************************************************************/
/*! \file DeviceControlError.h
 *
 *  \brief
 *
 *   Version: 0.1
 *   Date:    01.08.2010
 *   Author:  Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the error and event defines of the device control layer
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
#ifndef DEVICE_CONTROL_ERROR_H
#define DEVICE_CONTROL_ERROR_H

#include <../Include/PlatformEventCodes.h>

/*! General description
 *  -> remark: sometimes instead of 'error' the term 'event' is used.
 * The errors will be separated by error group and error identifier
 *
 * The error group is a 16bit number,
 *  the high byte is named 'error main group id'
 *  the low byte is named 'error sub group id'
 *  The 'error main group id' typically defined by each software module, each module should use a unique abbreviation
 *  We use EVENT_GROUP_PLATFORM as main group id. Definition of EVENT_GROUP_PLATFORM can be found in PlatformEventCodes.h
 *
 *  The 'event sub group id' is defined by each software module according to its submodules
 *    e.g. CAN interface       0x202
 *         Configuration       0x203
 * \warning We start the sub group id's from 0x201 so that event ID's are unique through out the project
 * \warning All the sub group id's shall be separated into groups using the s
 *
 * The event id is a 16bit number, this number can be defined freely within each event group
 *
 */

///********************************************************/
///*! list of 'Application Control' sub group id's  (example) */
///********************************************************/
//const quint32 EVENT_GRP_APP_UNIVERSAL                 (EVENT_MAIN_GRP_APP << 8) | 0x0201  //!< universal errors and events
//const quint32 EVENT_GRP_APP_EVENT_HDL                 (EVENT_MAIN_GRP_APP << 8) | 0x0202  //!< error concerning the 'event definition file' reading process

 /*! \warning All the sub group id's shall be separated into groups.
  *           For eg. we will subgroup all the events under the subgroup "EVENT_GRP_DCL_DEVCTRL"
  *           as follows:
  */
    /*********************************************************
    START_OF_GROUP EVENT_GRP_DCL_DEVCTRL
    *********************************************************/
   /*
    //const quint32 ERROR_DCL_DEVCTRL_START_COMM              =   0x0001;  ///< Event start communication
    //const quint32 ERROR_DCL_DEVCTRL_READCONFIG              =   0x0002;  ///< Event read configuration
    //const quint32 EVENT_DCL_DEVCTRL_START_DIAG              =   0x0003;  ///< Event start service
    //const quint32 EVENT_DCL_DEVCTRL_START_NORMAL_OP         =   0x0004;  ///< Event start normal operation mode
    //const quint32 EVENT_DCL_DEVCTRL_BREAK_NORMAL_OP         =   0x0005;  ///< Event break normal operation mode
    //const quint32 ERROR_DCL_DEVCTRL_ACTIVATE_TASK_FAILED    =   0x0006;  ///< Could not activate a device processing task
    //const quint32 ERROR_DCL_DEVCTRL_HEARTBEAT_ERROR         =   0x0007;  ///< Problems with heartbeat distribution
    */
    /*********************************************************
    END_OF_GROUP EVENT_GRP_DCL_DEVCTRL
    *********************************************************/
  /*!
  *
  * \note We make use of "START_OF_GROUP" and "END_OF_GROUP" to indicate begining and end and of a subgroup. This
  *       is IMPORTANT for the "StringUpdate" tool to recognize and generate an EventID .
  *       "START_OF_GROUP" is followed by a space and the subgroup name. Similar convention is followed for
  *       "END_OF_GROUP".
*/

/********************************************************/
/*! list of Device Control Layer sub group id's */
/********************************************************/
const quint32 EVENT_GRP_DCL_DEVCTRL              =     (EVENT_GROUP_PLATFORM << 8) | 0x0201;   //!< device control
const quint32 EVENT_GRP_DCL_CANINTERFACE         =     (EVENT_GROUP_PLATFORM << 8) | 0x0202;   //!< can interface
const quint32 EVENT_GRP_DCL_CONFIGURATION        =     (EVENT_GROUP_PLATFORM << 8) | 0x0203;   //!< configuration
const quint32 EVENT_GRP_DCL_DEVICEPROC_CONFIG    =     (EVENT_GROUP_PLATFORM << 8) | 0x0204;   //!< device processing - configuration
const quint32 EVENT_GRP_DCL_CANBUS               =     (EVENT_GROUP_PLATFORM << 8) | 0x0205;   //!< can bus
const quint32 EVENT_GRP_DCL_DIAG                 =     (EVENT_GROUP_PLATFORM << 8) | 0x0206;   //!< diagnsostic / service

/*! CAN-Object error groups, node and all function modules
 * those error appear on master side within the device control layer (dcl) */
const quint32 EVENT_GRP_DCL_NODE_DCL                  = (EVENT_GROUP_PLATFORM << 8) | 0x0207;   //!< CANNode
const quint32 EVENT_GRP_DCL_FCTMOD_DIG_INP_DCL        = (EVENT_GROUP_PLATFORM << 8) | 0x0208;   //!< digital input
const quint32 EVENT_GRP_DCL_FCTMOD_DIG_OUTP_DCL       = (EVENT_GROUP_PLATFORM << 8) | 0x0209;   //!< digital output
const quint32 EVENT_GRP_DCL_FCTMOD_ANA_INP_DCL        = (EVENT_GROUP_PLATFORM << 8) | 0x020A;   //!< analog input
const quint32 EVENT_GRP_DCL_FCTMOD_ANA_OUTP_DCL       = (EVENT_GROUP_PLATFORM << 8) | 0x020B;   //!< analog output
const quint32 EVENT_GRP_DCL_FCTMOD_INCL_DCL           = (EVENT_GROUP_PLATFORM << 8) | 0x020C;   //!< inclinometer
const quint32 EVENT_GRP_DCL_FCTMOD_MOTOR_DCL          = (EVENT_GROUP_PLATFORM << 8) | 0x020D;   //!< stepper motor
const quint32 EVENT_GRP_DCL_FCTMOD_RFID11785_DCL      = (EVENT_GROUP_PLATFORM << 8) | 0x020E;   //!< RFID ISO 11785
const quint32 EVENT_GRP_DCL_FCTMOD_RFID15693_DCL      = (EVENT_GROUP_PLATFORM << 8) | 0x020F;   //!< RFID ISO 15693
const quint32 EVENT_GRP_DCL_FCTMOD_TEMP_DCL           = (EVENT_GROUP_PLATFORM << 8) | 0x0210;   //!< temperature control
const quint32 EVENT_GRP_DCL_FCTMOD_JSTCK_DCL          = (EVENT_GROUP_PLATFORM << 8) | 0x0211;   //!< joystick
const quint32 EVENT_GRP_DCL_FCTMOD_UART_DCL           = (EVENT_GROUP_PLATFORM << 8) | 0x0212;   //!< UART

/*! slave object error groups, include the node's and all function module error
 *  those error appear on slave side and will be reported via CAN bus (slv); */
const quint32 EVENT_GRP_DCL_NODE_SLV                  = (EVENT_GROUP_PLATFORM << 8) | 0x0213;  //!< CANNode
const quint32 EVENT_GRP_DCL_FCTMOD_DIG_INP_SLV        = (EVENT_GROUP_PLATFORM << 8) | 0x0214;   //!< digital input
const quint32 EVENT_GRP_DCL_FCTMOD_DIG_OUTP_SLV       = (EVENT_GROUP_PLATFORM << 8) | 0x0215;   //!< digital output
const quint32 EVENT_GRP_DCL_FCTMOD_ANA_INP_SLV        = (EVENT_GROUP_PLATFORM << 8) | 0x0216;   //!< analog input
const quint32 EVENT_GRP_DCL_FCTMOD_ANA_OUTP_SLV       = (EVENT_GROUP_PLATFORM << 8) | 0x0217;   //!< analog output
const quint32 EVENT_GRP_DCL_FCTMOD_INCL_SLV           = (EVENT_GROUP_PLATFORM << 8) | 0x0218;   //!< inclinometer
const quint32 EVENT_GRP_DCL_FCTMOD_MOTOR_SLV          = (EVENT_GROUP_PLATFORM << 8) | 0x0219;   //!< stepper motor
const quint32 EVENT_GRP_DCL_FCTMOD_RFID_SLV           = (EVENT_GROUP_PLATFORM << 8) | 0x021A;   //!< RFID
const quint32 EVENT_GRP_DCL_FCTMOD_TEMP_SLV           = (EVENT_GROUP_PLATFORM << 8) | 0x021B;   //!< temperature control
const quint32 EVENT_GRP_DCL_FCTMOD_JSTCK_SLV          = (EVENT_GROUP_PLATFORM << 8) | 0x021C;   //!< joystick
const quint32 EVENT_GRP_DCL_FCTMOD_UART_SLV           = (EVENT_GROUP_PLATFORM << 8) | 0x021D;   //!< UART
const quint32 EVENT_GRP_DCL_GRAPPLER_DEV              = (EVENT_GROUP_PLATFORM << 8) | 0x021E;   //!< Grappler
const quint32 EVENT_GRP_DCL_AGITATION_DEV             = (EVENT_GROUP_PLATFORM << 8) | 0x021F;   //!< Agitation
const quint32 EVENT_GRP_DCL_RACKTRANS_DEV             = (EVENT_GROUP_PLATFORM << 8) | 0x0220;   //!< Rack transport
const quint32 EVENT_GRP_DCL_OVEN_DEV                  = (EVENT_GROUP_PLATFORM << 8) | 0x0221;   //!< Oven
const quint32 EVENT_GRP_DCL_LOADER_DEV                = (EVENT_GROUP_PLATFORM << 8) | 0x0222;   //!< Loader
const quint32 EVENT_GRP_DCL_HVESSELS_DEV              = (EVENT_GROUP_PLATFORM << 8) | 0x0223;   //!< Heated vessels
const quint32 EVENT_GRP_DCL_WATER_DEV                 = (EVENT_GROUP_PLATFORM << 8) | 0x0224;   //!< Water
const quint32 EVENT_GRP_DCL_EXHAUST_DEV               = (EVENT_GROUP_PLATFORM << 8) | 0x0225;   //!< Exhaust
const quint32 EVENT_GRP_DCL_INCL_DEV                  = (EVENT_GROUP_PLATFORM << 8) | 0x0226;   //!< Inclinometer
const quint32 EVENT_GRP_DCL_COVERLINE_DEV             = (EVENT_GROUP_PLATFORM << 8) | 0x0227;   //!< Coverline
const quint32 EVENT_GRP_DCL_RACKHDL_DEV               = (EVENT_GROUP_PLATFORM << 8) | 0x0228;   //!< Rackhandling sepia
const quint32 EVENT_GRP_DCL_HOOD_DEV                  = (EVENT_GROUP_PLATFORM << 8) | 0x0229;   //!< Hood Open Detection

/*********************************************************
 * list of  'Application Control' events (and errors)
 ********************************************************/

/*********************************************************
 *  Error group EVENT_GRP_APP_UNIVERSAL */
/*#define ERROR_APP_UNIVERSAL_APP_START          0x0001  //Event 'Application start'
#define ERROR_APP_UNIVERSAL_GUI_START          0x0002  //Event 'GUI start'
#define ERROR_APP_UNIVERSAL_DCL_START          0x0003  //Event 'Device Control Layer' start
#define ERROR_APP_UNIVERSAL_DCL_CONFIG_START   0x0004  //Event 'Device Control Layer' configuration start
#define ERROR_APP_UNIVERSAL_DCL_DIAGSRV_START  0x0005  //Event 'Device Control Layer' diagnostic service start
*/

/*********************************************************
 *  Error group EVENT_GRP_APP_EVENT_HDL */
/*#define EVENT_APP_EVENT_HDL_ERROR               0x0001  // general error while reading the event definition file
#define EVENT_APP_EVENT_HDL_LANGUAGE            0x0002  // no language specified before reading the event definition file
#define EVENT_APP_EVENT_HDL_FILE_OPEN_FAILED    0x0003  // event definition file not found
#define EVENT_APP_EVENT_HDL_PARSE_ERROR         0x0004  // error while parsing a xml file
#define EVENT_APP_EVENT_HDL_FORMAT_ERROR        0x0005  // format error in xml file
#define EVENT_APP_EVENT_HDL_VERSION_ERROR       0x0006  // version error in xml file
#define EVENT_APP_EVENT_HDL_FORMAT_ERROR_LANG   0x0007  // specified language not found in error description file
#define EVENT_APP_EVENT_HDL_FORMAT_ERROR_EV     0x0008  // child 'event_definitions' not found
*/

/*********************************************************
 * List of Device Control Layer events (and errors)
 ********************************************************/
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_DEVCTRL
*********************************************************/
const quint32 ERROR_DCL_DEVCTRL_START_COMM              =   0x0001;  ///< Event start communication
const quint32 ERROR_DCL_DEVCTRL_READCONFIG              =   0x0002;  ///< Event read configuration
const quint32 EVENT_DCL_DEVCTRL_START_DIAG              =   0x0003;  ///< Event start service
const quint32 EVENT_DCL_DEVCTRL_START_NORMAL_OP         =   0x0004;  ///< Event start normal operation mode
const quint32 EVENT_DCL_DEVCTRL_BREAK_NORMAL_OP         =   0x0005;  ///< Event break normal operation mode
const quint32 ERROR_DCL_DEVCTRL_ACTIVATE_TASK_FAILED    =   0x0006;  ///< Could not activate a device processing task
const quint32 ERROR_DCL_DEVCTRL_HEARTBEAT_ERROR         =   0x0007;  ///< Problems with heartbeat distribution
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_DEVCTRL
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_CANBUS
*********************************************************/
const quint32 ERROR_DCL_CANBUS_WRITE                   = 0x0001;  //!< writeing a CAN message failed
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_CANBUS
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_CONFIGURATION
*********************************************************/
const quint32 ERROR_DCL_CONFIG_CAN_MESSAGE_ASSIGN            = 0x0001;  //!< CAN message assignment failed
const quint32 ERROR_DCL_CONFIG_HW_CFG_OPEN_FAILED            = 0x0010;  //!< hardware configuration, open config file failed
const quint32 ERROR_DCL_CONFIG_HW_CFG_PARSE_ERROR            = 0x0011;  //!< hardware configuration, general parsing error
const quint32 ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR           = 0x0012;  //!< hardware configuration, format error
const quint32 ERROR_DCL_CONFIG_HW_CFG_VERSION_ERROR          = 0x0013;  //!< hardware configuration, version confict
const quint32 ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_MST       = 0x0014;  //!< hardware configuration, section master
const quint32 ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_CAN       = 0x0015;  //!<  hardware configuration, can interface missed
const quint32 ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV       = 0x0016;  //!< hardware configuration, section slave
const quint32 ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT       = 0x0017;  //!< hardware configuration, section function module
const quint32 ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_DEV       = 0x0018;  //!< hardware configuration, section device

const quint32 ERROR_DCL_CONFIG_CAN_MSG_CFG_OPEN_FAILED       = 0x0020;  //!< CAN message configuration, open config file failed
const quint32 ERROR_DCL_CONFIG_CAN_MSG_CFG_PARSE_ERROR       = 0x0021;  //!< CAN message configuration, general parsing error
const quint32 ERROR_DCL_CONFIG_CAN_MSG_CFG_FORMAT_ERROR      = 0x0022;  //!< CAN message configuration, format error
const quint32 ERROR_DCL_CONFIG_CAN_MSG_CFG_VERSION_ERROR     = 0x0023;  //!< CAN message configuration, version confict
const quint32 ERROR_DCL_CONFIG_CAN_MSG_CFG_FORMAT_ERROR_BASE = 0x0024;  //!< CAN message configuration, section base module
const quint32 ERROR_DCL_CONFIG_CAN_MSG_CFG_FORMAT_ERROR_FCT  = 0x0025;  //!< CAN message configuration, section function module
const quint32 ERROR_DCL_CONFIG_CAN_MSG_CFG_FORMAT_ERROR_MSG  = 0x0026;  //!< CAN message configuration, section message

const quint32 ERROR_DCL_CONFIG_PROCSETTING_OPEN_FAILED       = 0x0030 ;  //!< error reading process settings file
const quint32 ERROR_DCL_CONFIG_PROCSETTING_PARSE_ERROR       = 0x0031 ;  //!< parser error while reading process settings file
const quint32 ERROR_DCL_CONFIG_PROCSETTING_FORMAT_ERROR      = 0x0032 ;  //!< format error at process settings file
const quint32 ERROR_DCL_CONFIG_PROCSETTING_VERSION_ERROR     = 0x0033 ;  //!< version error at process settings file

const quint32 ERROR_DCL_CONFIG_CAN_NODE_IDLE_TIMEOUT         = 0x0040 ;  //!< timeout while waiting for all CAN nodes switching to idle mode
const quint32 ERROR_DCL_CONFIG_CAN_NODE_ERROR                = 0x0041 ;  //!< CAN node switched to error node while waiting for idle mode

const quint32 ERROR_DCL_CONFIG_DEVICE_TYPE_INVALID           = 0x0050 ;  //!< Invalid device type key

const quint32 ERROR_DCL_CONFIG_FINISHED                      = 0x0051  ;  //!< event notification 'coniguration finished'

/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_CONFIGURATION
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_DIAG
*********************************************************/
const quint32 ERROR_DCL_DIAG_FINISHED                   = 0x01  ;  //!< event notification 'diagnostic finished'
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_DIAG
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_NODE_DCL
*********************************************************/
const quint32 ERROR_DCL_NODE_TIMEOUT_INIT               = 0x0001;  //!< timeout between CANNode initialization and 'HardwareID' receipt
                                                          // additional information: none
                                                          // this errors appears while initialization state is active, the CANNode waits for 'HardwareID' which
                                                          // is not received.
                                                          // Typically the can bus is not connected to the slaves, or the 'HardwareID'-msg identifier is not correct

const quint32 ERROR_DCL_NODE_TIMEOUT_CONFIG             = 0x0002;  //!< timeout between 'ConfigurationReq' and 'Configuration' receipt
                                                          //   additional information: Byte1: fct module's type (CAN-message byte0)
                                                          //                           Byte2: fct module's channel
const quint32 ERROR_DCL_NODE_INIT_WRONG_CHANNEL_COUNT   = 0x0003;  //!< the number if channels read from hardware configuration file and the number of channels
                                                          //   announced by CAN message 'HardwareID' doesn't match
                                                          //   additional information: Byte1: channel count from config
                                                          //                           Byte2: channel count from node
const quint32 ERROR_DCL_NODE_CONFIG_INVALID_MODULE      = 0x0004;  //!< the module information as read from configuration file does not match to the information
                                                          //   announced by CAN-message 'Configuration' from the node
                                                          //    additional information: Byte1: channel count from config
                                                          //                            Byte2: channel count from node
const quint32 ERROR_DCL_NODE_CONFIG_UNEXP_CANMSG        = 0x0005;  //!< The CAN message 'Configuration' was received while it was not expected
                                                          //   additional information: Byte1: channel from node (CAN-message byte0)
                                                          //                           Byte2: node main state
const quint32 ERROR_DCL_NODE_TIMEOUT_FCT_CONFIG         = 0x0006;  //!< timeout while the software tried to configure the node's function modules
                                                          //   additional information: Byte1: fct module's type (CAN-message byte0)
                                                          //                           Byte2: fct module's channel
const quint32 ERROR_DCL_NODE_HEARTBEAT_TIMEOUT          = 0x0007;  //!< node heartbeat timeout

const quint32 ERROR_DCL_NODE_HARDWAREID_INTERVALL       = 0x0008;  //!< the can message HardwareID was received within the minimal time delay
                                                          //    there are two nodes with same nodeID connected to can bus
                                                          //    additional information: Byte1: 0
                                                          //                            Byte2: 0

const quint32 ERROR_DCL_NODE_HWID_REC_INVALID_STATE     = 0x0009;  //!< the can message HardwareID was received although the node is already configured
                                                          //   -> unexpected node reboot (watchdog?)
                                                          //   additional information: Byte1: 0
                                                          //                           Byte2: 0
const quint32 ERROR_DCL_NODE_INVALID_STATE              = 0x000A;  //!< invalid state
const quint32 ERROR_DCL_NODE_NULL_PTR_ACCESS            = 0x000B;  //!< null pointer access
const quint32 ERROR_DCL_NODE_FCT_CALL_FAILED            = 0x000C;  //!< function call failed
const quint32 ERROR_DCL_NODE_CANMSG_INVALID_DLC         = 0x000D;  //!< a can message with invalid data length code was received ( <> expected)

const quint32 EVENT_DCL_INODE_SET_NODE_STATE_FCTCALL_FAILED   = 0x0E ;  //!< set node state
const quint32 EVENT_DCL_NODE_SET_NODE_STATE_CANMSG_NOT_SEND   = 0x0F ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_SET_NODE_STATE_CANMSG_TIMEOUT    = 0x1A ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_NODE_STATE_FCTCALL_FAILED   = 0x1B ;  //!< request node state
const quint32 EVENT_DCL_NODE_REQ_NODE_STATE_CANMSG_NOT_SEND   = 0x1C ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_NODE_STATE_CANMSG_TIMEOUT    = 0x1D ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_EMCY_STOP_FCTCALL_FAILED    = 0x1F ;  //!< request partition data reset
const quint32 EVENT_DCL_NODE_REQ_EMCY_STOP_CANMSG_NOT_SEND    = 0x20 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_EMCY_STOP_CANMSG_TIMEOUT     = 0x21 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_RESET_FCTCALL_FAILED        = 0x22 ;  //!< request partition data reset
const quint32 EVENT_DCL_NODE_REQ_RESET_CANMSG_NOT_SEND        = 0x23 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_RESET_CANMSG_TIMEOUT         = 0x24 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_CONV_STATISTICS_FCTCALL_FAILED  = 0x25 ;  //!< request statistic data failed
const quint32 EVENT_DCL_NODE_CONV_STATISTICS_CANMSG_NOT_SEND  = 0x26 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_CONV_STATISTICS_CANMSG_TIMEOUT   = 0x27 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_DATA_RESET_FCTCALL_FAILED   = 0x28 ;  //!< request partition data reset
const quint32 EVENT_DCL_NODE_REQ_DATA_RESET_CANMSG_NOT_SEND   = 0x29 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_DATA_RESET_CANMSG_TIMEOUT    = 0x2A ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_FORMAT_MEM_FCTCALL_FAILED   = 0x2B ;  //!< request formatting the memmory
const quint32 EVENT_DCL_NODE_REQ_FORMAT_MEM_CANMSG_NOT_SEND   = 0x2C ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_FORMAT_MEM_CANMSG_TIMEOUT    = 0x2D ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_SER_NB_FCTCALL_FAILED       = 0x2E ;  //!< request serial hw number
const quint32 EVENT_DCL_NODE_REQ_SER_NB_CANMSG_NOT_SEND       = 0x2F ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_SER_NB_CANMSG_TIMEOUT        = 0x30 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_END_TEST_RES_FCTCALL_FAILED = 0x31 ;  //!< function call 'request test result' failed (from ICANNode to CANNode)
const quint32 EVENT_DCL_NODE_REQ_END_TEST_RES_CANMSG_NOT_SEND = 0x32;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_END_TEST_RES_CANMSG_TIMEOUT  = 0x33 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_HW_INFO_FCTCALL_FAILED      = 0x34 ;  //!< request hw info
const quint32 EVENT_DCL_NODE_REQ_HW_INFO_CANMSG_NOT_SEND      = 0x35 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_HW_INFO_CANMSG_TIMEOUT       = 0x36 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_SW_INFO_FCTCALL_FAILED      = 0x37 ;  //!< request sw info
const quint32 EVENT_DCL_NODE_REQ_SW_INFO_CANMSG_NOT_SEND      = 0x38 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_SW_INFO_CANMSG_TIMEOUT       = 0x39 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_LOADER_INFO_FCTCALL_FAILED  = 0X40 ;  //!< request sw info
const quint32 EVENT_DCL_NODE_REQ_LOADER_INFO_CANMSG_NOT_SEND  = 0x41 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_LOADER_INFO_CANMSG_TIMEOUT   = 0x42 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_LIFECYC_DATA_FCTCALL_FAILED = 0x43 ;  //!< function call failed
const quint32 EVENT_DCL_NODE_REQ_LIFECYC_DATA_CANMSG_NOT_SEND = 0x44 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_LIFECYC_DATA_CANMSG_TIMEOUT  = 0x45 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_LAUNCH_DATE_FCTCALL_FAILED  = 0x46 ;  //!< function call failed
const quint32 EVENT_DCL_NODE_REQ_LAUNCH_DATE_CANMSG_NOT_SEND  = 0x47 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_LAUNCH_DATE_CANMSG_TIMEOUT   = 0x48 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_BOARD_NAME_FCTCALL_FAILED   = 0x49 ;  //!< function call failed
const quint32 EVENT_DCL_NODE_REQ_BOARD_NAME_CANMSG_NOT_SEND   = 0x4A;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_BOARD_NAME_CANMSG_TIMEOUT    = 0x4B ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_BOARD_OPT_FCTCALL_FAILED    = 0x4C ;  //!< request
const quint32 EVENT_DCL_NODE_REQ_BOARD_OPT_CANMSG_NOT_SEND    = 0x4D ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_BOARD_OPT_CANMSG_TIMEOUT     = 0x4E ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_CONV_VOLTAGE_MON_FCTCALL_FAILED = 0x4F ;  //!< function call failed
const quint32 EVENT_DCL_NODE_CONV_VOLTAGE_MON_CANMSG_NOT_SEND = 0x50 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_CONV_VOLTAGE_MON_CANMSG_TIMEOUT  = 0x51 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_VOLT_STATE_FCTCALL_FAILED   = 0x52 ;  //!< request
const quint32 EVENT_DCL_NODE_REQ_VOLT_STATE_CANMSG_NOT_SEND   = 0x53 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_VOLT_STATE_CANMSG_TIMEOUT    = 0x54;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_CONV_CURRENT_MON_FCTCALL_FAILED = 0x55 ;  //!< function call failed
const quint32 EVENT_DCL_NODE_CONV_CURRENT_MON_CANMSG_NOT_SEND = 0x56 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_CONV_CURRENT_MON_CANMSG_TIMEOUT  = 0x57 ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_CURR_STATE_FCTCALL_FAILED   = 0x58 ;  //!< request
const quint32 EVENT_DCL_NODE_REQ_CURR_STATE_CANMSG_NOT_SEND   = 0x59 ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_CURR_STATE_CANMSG_TIMEOUT    = 0x5A ;  //!< acknowledge timeout
const quint32 EVENT_DCL_INODE_REQ_UNIQUE_NB_FCTCALL_FAILED    = 0x5B ;  //!< request
const quint32 EVENT_DCL_NODE_REQ_UNIQUE_NB_CANMSG_NOT_SEND    = 0x5C ;  //!< CAN message was not send
const quint32 EVENT_DCL_NODE_REQ_UNIQUE_NB_CANMSG_TIMEOUT     = 0x5D ;  //!< acknowledge timeout
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_NODE_DCL
*********************************************************/

//  stepper motor
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_MOTOR_DCL
*********************************************************/
const quint32 EVENT_DCL_FCTMOD_MOTOR_NULL_PTR_ACCESS           = 0x0001;  //!< null pointer access
const quint32 EVENT_DCL_IFCTMOD_MOTOR_NULL_PTR_ACCESS          = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_FCTMOD_MOTOR_INVALID_MAIN_TASK_STATE   = 0x0003;  //!< invalid state
const quint32 ERROR_DCL_FCTMOD_MOTOR_CANMSG_INVALID_DLC        = 0x0004;  //!< a can message with invalid data length code was received ( <> expected)
const quint32 EVENT_DCL_IFCTMOD_MOTOR_SET_STATE_FCTCALL_FAILED = 0x0010;  //!< state set failed
const quint32 EVENT_DCL_FCTMOD_MOTOR_SET_STATE_CANMSG_NOT_SEND = 0x0011;  //!< CAN message 'SetState' couldn't be send
const quint32 EVENT_DCL_FCTMOD_MOTOR_SET_STATE_CANMSG_TIMEOUT  = 0x0012;  //!< timeout while waiting for state acknowledge
const quint32 EVENT_DCL_FCTMOD_MOTOR_SET_STATE_INVALID_STATE   = 0x0013;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_MOTOR_SET_STATE_INVALID_STATE  = 0x0014;  //!< invalid state (interface class)
const quint32 EVENT_DCL_IFCTMOD_MOTOR_REFRUN_FCTCALL_FAILED    = 0x0020;  //!< request reference run failed
const quint32 EVENT_DCL_FCTMOD_MOTOR_REFRUN_CANMSG_NOT_SEND    = 0x0021;  //!< CAN message 'reference run' couln't be send
const quint32 EVENT_DCL_FCTMOD_MOTOR_REFRUN_CANMSG_TIMEOUT     = 0x0022;  //!< timeout while waiting for reference run acknowledge
const quint32 EVENT_DCL_FCTMOD_MOTOR_REFRUN_INVALID_STATE      = 0x0023;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_MOTOR_REFRUN_INVALID_STATE     = 0x0024;  //!< invalid state (interface class)
const quint32 EVENT_DCL_IFCTMOD_MOTOR_POSREQ_FCTCALL_FAILED    = 0x0030;  //!< request positioning failed
const quint32 EVENT_DCL_FCTMOD_MOTOR_POSREQ_CANMSG_NOT_SEND    = 0x0031;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_MOTOR_POSREQ_CANMSG_TIMEOUT     = 0x0032;  //!< acknowledge timeout
const quint32 EVENT_DCL_FCTMOD_MOTOR_POSREQ_INVALID_STATE      = 0x0033;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_MOTOR_POSREQ_INVALID_STATE     = 0x0034;  //!< invalid state (interface class)
const quint32 EVENT_DCL_FCTMOD_MOTOR_ACTPOSREQ_FCTCALL_FAILED  = 0x0040;  //!< request actual position failed
const quint32 EVENT_DCL_FCTMOD_MOTOR_ACTPOSREQ_CANMSG_NOT_SEND = 0x0041;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_MOTOR_ACTPOSREQ_CANMSG_TIMEOUT  = 0x0042;  //!< acknowledge timeout
const quint32 EVENT_DCL_FCTMOD_MOTOR_ACTPOSREQ_INVALID_STATE   = 0x0043;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_MOTOR_ACTPOSREQ_INVALID_STATE  = 0x0044;  //!< invalid state (interface class)
const quint32 EVENT_DCL_FCTMOD_MOTOR_ACTSPDREQ_FCTCALL_FAILED  = 0x0050;  //!< request actual speed failed
const quint32 EVENT_DCL_FCTMOD_MOTOR_ACTSPDREQ_CANMSG_NOT_SEND = 0x0051;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_MOTOR_ACTSPDREQ_CANMSG_TIMEOUT  = 0x0052;  //!< acknowledge timeout
const quint32 EVENT_DCL_FCTMOD_MOTOR_ACTSPDREQ_INVALID_STATE   = 0x0053;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_MOTOR_ACTSPDREQ_INVALID_STATE  = 0x0054;  //!< invalid state (interface class)
const quint32 EVENT_DCL_FCTMOD_MOTOR_SPDREQ_FCTCALL_FAILED     = 0x0060;  //!< request move speed failed
const quint32 EVENT_DCL_FCTMOD_MOTOR_SPDREQ_CANMSG_NOT_SEND    = 0x0061;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_MOTOR_SPDREQ_CANMSG_TIMEOUT     = 0x0062;  //!< acknowledge timeout
const quint32 EVENT_DCL_FCTMOD_MOTOR_SPDREQ_INVALID_STATE      = 0x0063;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_MOTOR_SPDREQ_INVALID_STATE     = 0x0064;  //!< invalid state (interface class)
const quint32 EVENT_DCL_FCTMOD_MOTOR_LIFECYCLEDATA_CANMSG_NOT_SEND = 0x0071;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_MOTOR_LIFECYCLEDATA_CANMSG_TIMEOUT  = 0x0072;  //!< acknowledge timeout
const quint32 EVENT_DCL_IFCTMOD_MOTOR_LIFECYCLEDATA_INVALID_STATE  = 0x0074;  //!< invalid state (interface class)
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_MOTOR_DCL
*********************************************************/


//  temperature control
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_TEMP_DCL
*********************************************************/
const quint32 EVENT_DCL_FCTMOD_TEMP_NULL_PTR_ACCESS              = 0x0001;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_NULL_PTR_ACCESS             = 0x0002;  //!< invalid state (interface class)
const quint32 ERROR_DCL_FCTMOD_TEMP_INVALID_MAIN_TASK_STATE      = 0x0003;  //!< invalid main state
const quint32 ERROR_DCL_FCTMOD_TEMP_CANMSG_INVALID_DLC           = 0x0004 ;  //!< invalid data length code detected
const quint32 EVENT_DCL_IFCTMOD_TEMP_SET_TEMP_FCTCALL_FAILED     = 0x0010;  //!< set temperature failed
const quint32 EVENT_DCL_FCTMOD_TEMP_SET_TEMP_CANMSG_NOT_SEND     = 0x0011;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_TEMP_SET_TEMP_INVALID_STATE       = 0x0012;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_SET_TEMP_INVALID_STATE      = 0x0013;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_REQ_ACTTEMP_FCTCALL_FAILED  = 0x0020;  //!< request actual temperature failed
const quint32 EVENT_DCL_FCTMOD_TEMP_REQ_ACTTEMP_CANMSG_NOT_SEND  = 0x0021;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_TEMP_REQ_ACTTEMP_CANMSG_TIMEOUT   = 0x0022;  //!< timeout of the acknowledge
const quint32 EVENT_DCL_FCTMOD_TEMP_REQ_ACTTEMP_INVALID_STATE    = 0x0023;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_REQ_ACTTEMP_INVALID_STATE   = 0x0024;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_SET_OPMODE_FCTCALL_FAILED   = 0x0030;  //!< set operation mode failed
const quint32 EVENT_DCL_FCTMOD_TEMP_SET_OPMODE_CANMSG_NOT_SEND   = 0x0031;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_TEMP_SET_OPMODE_INVALID_STATE     = 0x0033;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_SET_OPMODE_INVALID_STATE    = 0x0034;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_REQ_OPMODE_FCTCALL_FAILED   = 0x0040;  //!< request temperature failed
const quint32 EVENT_DCL_FCTMOD_TEMP_REQ_OPMODE_CANMSG_NOT_SEND   = 0x0041;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_TEMP_REQ_OPMODE_CANMSG_TIMEOUT    = 0x0042;  //!< timeout of the acknowledge
const quint32 EVENT_DCL_FCTMOD_TEMP_REQ_OPMODE_INVALID_STATE     = 0x0043;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_REQ_OPMODE_INVALID_STATE    = 0x0044;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_SET_STATUS_FCTCALL_FAILED   = 0x0050;  //!< set status of temperature ctrl. failed
const quint32 EVENT_DCL_FCTMOD_TEMP_SET_STATUS_CANMSG_NOT_SEND   = 0x0051;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_TEMP_SET_STATUS_INVALID_STATE     = 0x0052;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_SET_STATUS_INVALID_STATE    = 0x0053;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_REQ_STATUS_FCTCALL_FAILED   = 0x0060;  //!< request status of temperature ctrl. failed
const quint32 EVENT_DCL_FCTMOD_TEMP_REQ_STATUS_CANMSG_NOT_SEND   = 0x0061;  //!< CAN message was not send
const quint32 EVENT_DCL_FCTMOD_TEMP_REQ_STATUS_CANMSG_TIMEOUT    = 0x0062;  //!< timeout of the acknowledge
const quint32 EVENT_DCL_FCTMOD_TEMP_REQ_STATUS_INVALID_STATE     = 0x0063;  //!< invalid state
const quint32 EVENT_DCL_IFCTMOD_TEMP_REQ_STATUS_INVALID_STATE    = 0x0064;  //!< invalid state
const quint32 EVENT_DCL_FCTMOD_TEMP_DCL_INVALID_IDLE_STATE       = 0x0065;  //!< invalid state at idle mode
const quint32 EVENT_DCL_FCTMOD_TEMP_DCL_TIMEOUT_CANMSG           = 0x0066 ;  //!< acknowledge timeout
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_TEMP_DCL
*********************************************************/


//  analog input
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_ANA_INP_DCL
*********************************************************/
const quint32 ERROR_DCL_FCTMOD_ANA_INP_CANMSG_ANA_INP_DLC          = 0x0001 ;  //!< invalid data length code detected
const quint32 ERROR_DCL_IFCTMOD_ANA_INP_REQ_ACTINP_FCTCALL_FAILED  = 0x0010 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_ANA_INP_REQ_ACTINP_FCTCALL_FAILED   = 0x0011 ;  //!< function call from CANFctModule failed
const quint32 ERROR_DCL_FCTMOD_ANA_INP_REQ_ACTINP_CANMSG_NOT_SEND  = 0x0012 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_ANA_INP_REQ_ACTINP_CANMSG_TIMEOUT   = 0x0013 ;  //!< timeout while waiting for acknowledge
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_ANA_INP_DCL
*********************************************************/


//  analog output
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_ANA_OUTP_DCL
*********************************************************/
const quint32 ERROR_DCL_FCTMOD_ANA_OUTP_CANMSG_ANA_OUTP_DLC         = 0x0001 ;  //!< invalid data length code detected
const quint32 ERROR_DCL_IFCTMOD_ANA_OUTP_SET_OUTP_FCTCALL_FAILED    = 0x0010 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_ANA_OUTP_SET_OUTP_CANMSG_NOT_SEND    = 0x0011 ;  //!< function call from CANFctModule failed
const quint32 ERROR_DCL_FCTMOD_ANA_OUTP_SET_OUTP_CANMSG_INVALID     = 0x0012 ;  //!< CAN-message could not be sent

const quint32 ERROR_DCL_IFCTMOD_ANA_OUTP_REQ_OUTP_FCTCALL_FAILED    = 0x0020 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_ANA_OUTP_REQ_OUTP_CANMSG_NOT_SEND    = 0x0021 ;  //!< function call from CANFctModule failed
const quint32 ERROR_DCL_FCTMOD_ANA_OUTP_REQ_OUTP_CANMSG_INVALID     = 0x0022 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_ANA_OUTP_REQ_OUTP_TIMEOUT            = 0x0023 ;  //!< timeout while waiting for acknowledge

const quint32 ERROR_DCL_IFCTMOD_ANA_OUTP_REQ_LIVE_FCTCALL_FAILED    = 0x0024 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_ANA_OUTP_REQ_LIVE_CANMSG_NOT_SEND    = 0x0025 ;  //!< function call from CANFctModule failed
const quint32 ERROR_DCL_FCTMOD_ANA_OUTP_REQ_LIVE_CANMSG_INVALID     = 0x0026 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_ANA_OUTP_REQ_LIVE_TIMEOUT            = 0x0027 ;  //!< timeout while waiting for acknowledge
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_ANA_OUTP_DCL
*********************************************************/


//  digital input
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_DIG_INP_DCL
*********************************************************/
const quint32 ERROR_DCL_FCTMOD_DIG_INP_CANMSG_INVALID_DLC          = 0x0001 ;  //!< invalid data length code detected
const quint32 ERROR_DCL_IFCTMOD_DIG_INP_REQ_ACTINP_FCTCALL_FAILED  = 0x0010 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_DIG_INP_REQ_ACTINP_CANMSG_NOT_SEND  = 0x0011 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_DIG_INP_REQ_ACTINP_CANMSG_TIMEOUT   = 0x0012 ;  //!< timeout while waiting for acknowledge
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_DIG_INP_DCL
*********************************************************/


//  digital output
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_DIG_OUTP_DCL
*********************************************************/
const quint32 ERROR_DCL_FCTMOD_DIG_OUTP_CANMSG_INVALID_DLC         = 0x0001 ;  //!< invalid data length code detected
const quint32 ERROR_DCL_IFCTMOD_DIG_OUTP_SET_OUTP_FCTCALL_FAILED   = 0x0010 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_DIG_OUTP_SET_OUTP_CANMSG_NOT_SEND   = 0x0011 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_DIG_OUTP_SET_OUTP_CANMSG_TIMEOUT    = 0x0012 ;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_DIG_OUTP_REQ_OUTP_FCTCALL_FAILED   = 0x0020 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_DIG_OUTP_REQ_OUTP_CANMSG_NOT_SEND   = 0x0021 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_DIG_OUTP_REQ_OUTP_CANMSG_TIMEOUT    = 0x0022 ;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_DIG_OUTP_REQ_LIFE_FCTCALL_FAILED   = 0x0030 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_DIG_OUTP_REQ_LIFE_CANMSG_NOT_SEND   = 0x0031 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_DIG_OUTP_REQ_LIFE_CANMSG_TIMEOUT    = 0x0032 ;  //!< timeout while waiting for acknowledge
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_DIG_OUTP_DCL
*********************************************************/


// RFID ISO/IEC 11785
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_RFID11785_DCL
*********************************************************/
const quint32 ERROR_DCL_FCTMOD_RFID11785_CANMSG_INVALID_DLC           = 0x0001;  //!< invalid data length code detected
const quint32 ERROR_DCL_IFCTMOD_RFID11785_SET_CONFIG_FCTCALL_FAILED   = 0x0010;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID11785_SET_CONFIG_CANMSG_NOT_SEND   = 0x0011;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID11785_SET_CONFIG_CANMSG_TIMEOUT    = 0x0012;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID11785_LOGIN_FCTCALL_FAILED        = 0x0020;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID11785_LOGIN_CANMSG_NOT_SEND        = 0x0021;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID11785_LOGIN_CANMSG_TIMEOUT         = 0x0022;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID11785_READ_DATA_FCTCALL_FAILED    = 0x0030;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID11785_READ_DATA_CANMSG_NOT_SEND    = 0x0031;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID11785_READ_DATA_CANMSG_TIMEOUT     = 0x0032;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID11785_READ_UID_FCTCALL_FAILED     = 0x0040;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID11785_READ_UID_CANMSG_NOT_SEND     = 0x0041;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID11785_READ_UID_CANMSG_TIMEOUT      = 0x0042;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID11785_WRITE_DATA_FCTCALL_FAILED   = 0x0050;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID11785_WRITE_DATA_CANMSG_NOT_SEND   = 0x0051;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID11785_WRITE_DATA_CANMSG_TIMEOUT    = 0x0052;  //!< timeout while waiting for acknowledge
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_RFID11785_DCL
*********************************************************/


// RFID ISO/IEC 15693
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_RFID15693_DCL
*********************************************************/
const quint32 ERROR_DCL_FCTMOD_RFID15693_CANMSG_INVALID_DLC           = 0x0001;  //!< invalid data length code detected
const quint32 ERROR_DCL_IFCTMOD_RFID15693_SET_CONFIG_FCTCALL_FAILED   = 0x0010;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID15693_SET_CONFIG_CANMSG_NOT_SEND   = 0x0011;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID15693_SET_CONFIG_CANMSG_TIMEOUT    = 0x0012;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID15693_ACQUIRE_UID_FCTCALL_FAILED  = 0x0020;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID15693_ACQUIRE_UID_CANMSG_NOT_SEND  = 0x0021;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID15693_ACQUIRE_UID_CANMSG_TIMEOUT   = 0x0022;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID15693_SET_UID_FCTCALL_FAILED      = 0x0030;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID15693_SET_UID_CANMSG_NOT_SEND      = 0x0031;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID15693_SET_UID_CANMSG_TIMEOUT       = 0x0032;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID15693_REQUEST_UID_FCTCALL_FAILED  = 0x0040;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID15693_REQUEST_UID_CANMSG_NOT_SEND  = 0x0041;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID15693_REQUEST_UID_CANMSG_TIMEOUT   = 0x0042;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID15693_READ_BLOCK_FCTCALL_FAILED   = 0x0050;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID15693_READ_BLOCK_CANMSG_NOT_SEND   = 0x0051;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID15693_READ_BLOCK_CANMSG_TIMEOUT    = 0x0052;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID15693_WRITE_BLOCK_FCTCALL_FAILED  = 0x0060;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID15693_WRITE_BLOCK_CANMSG_NOT_SEND  = 0x0061;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID15693_WRITE_BLOCK_CANMSG_TIMEOUT   = 0x0062;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_RFID15693_LOCK_BLOCK_FCTCALL_FAILED   = 0x0070;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_RFID15693_LOCK_BLOCK_CANMSG_NOT_SEND   = 0x0071;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_RFID15693_LOCK_BLOCK_CANMSG_TIMEOUT    = 0x0072;  //!< timeout while waiting for acknowledge
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_RFID15693_DCL
*********************************************************/


// joystick
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_JSTCK_DCL
*********************************************************/
const quint32 ERROR_DCL_FCTMOD_JSTCK_CANMSG_INVALID_DLC               = 0x0001 ;  //!< invalid data length code detected
const quint32 ERROR_DCL_IFCTMOD_JSTCK_REQ_INPUT_FCTCALL_FAILED        = 0x0010 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_JSTCK_REQ_INPUT_CANMSG_NOT_SEND        = 0x0011 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_JSTCK_REQ_INPUT_CANMSG_TIMEOUT         = 0x0012 ;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_JSTCK_SET_CONTACT_LMT_FCTCALL_FAILED  = 0x0020 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_JSTCK_SET_CONTACT_LMT_CANMSG_NOT_SEND  = 0x0021 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_JSTCK_SET_CONTACT_LMT_CANMSG_TIMEOUT   = 0x0022 ;  //!< timeout while waiting for acknowledge
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_JSTCK_DCL
*********************************************************/


// UART
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_FCTMOD_UART_DCL
*********************************************************/
const quint32 ERROR_DCL_FCTMOD_UART_CANMSG_INVALID_DLC             = 0x0001 ;  //!< invalid data length code detected
const quint32 ERROR_DCL_IFCTMOD_UART_SEND_DATA_FCTCALL_FAILED      = 0x0010 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_UART_SEND_DATA_CANMSG_NOT_SEND      = 0x0011 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_UART_SEND_DATA_ACKN_CANMSG_TIMEOUT  = 0x0012 ;  //!< timeout while waiting for acknowledge
const quint32 ERROR_DCL_IFCTMOD_UART_REQ_REC_DATA_FCTCALL_FAILED   = 0x0020 ;  //!< function call from ICANFctModule failed
const quint32 ERROR_DCL_FCTMOD_UART_REQ_REC_DATA_CANMSG_NOT_SEND   = 0x0021 ;  //!< CAN-message could not be sent
const quint32 ERROR_DCL_FCTMOD_UART_REQ_REC_DATA_CANMSG_TIMEOUT    = 0x0022 ;  //!< timeout while waiting for acknowledge
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_FCTMOD_UART_DCL
*********************************************************/


//#############################################################
//      Devices
//#############################################################


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_AGITATION_DEV
*********************************************************/
const quint32 ERROR_DCL_IAGITATION_DEV_INVALID_STATE             = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IAGITATION_DEV_NULL_PTR_ACCESS           = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_AGITATION_DEV_INVALID_TASKID             = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_AGITATION_DEV_INVALID_STATE              = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_AGITATION_DEV_NULL_PTR_ACCESS            = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_AGITATION_DEV_INIT_FCT_ALLOC_FAILED      = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_AGITATION_DEV_CONFIG_CONNECT_FAILED      = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_AGITATION_DEV_FCTMODULES_ERROR           = 0x0015;  //!< function module error state reported
const quint32 ERROR_DCL_IAGITATION_DEV_REFRUN_FCTCALL_FAILED     = 0x0016;  //!< function call failed when calling reference run request (interface class)
const quint32 ERROR_DCL_AGITATION_DEV_REFRUN_FCTCALL_FAILED      = 0x0017;  //!< function call failed when calling reference run request
const quint32 ERROR_DCL_AGITATION_DEV_REFRUN_TIMEOUT             = 0x0018;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IAGITATION_DEV_SET_SPEED_FCTCALL_FAILED  = 0x0019;  //!< function call failed when setting speed (interface class)
const quint32 ERROR_DCL_AGITATION_DEV_SET_SPEED_FCTCALL_FAILED   = 0x0020;  //!< function call failed when setting speed
const quint32 ERROR_DCL_AGITATION_DEV_SET_SPEED_TIMEOUT          = 0x0021;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IAGITATION_DEV_REQ_ACTPOS_FCTCALL_FAILED = 0x0022;  //!< function call failed when requesting actual position (interface class)
const quint32 ERROR_DCL_AGITATION_DEV_REQ_ACTPOS_FCTCALL_FAILED  = 0x0023;  //!< function call failed when requesting actual position
const quint32 ERROR_DCL_AGITATION_DEV_REQ_ACTPOS_TIMEOUT         = 0x0024;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IAGITATION_DEV_REQ_ACTSPD_FCTCALL_FAILED = 0x0025;  //!< function call failed when requesting actual speed (interface class)
const quint32 ERROR_DCL_AGITATION_DEV_REQ_ACTSPD_FCTCALL_FAILED  = 0x0026;  //!< function call failed when requesting actual speed
const quint32 ERROR_DCL_AGITATION_DEV_REQ_ACTSPD_TIMEOUT         = 0x0027;  //!< timeout acknowledge signal
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_AGITATION_DEV
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_EXHAUST_DEV
*********************************************************/
const quint32 ERROR_DCL_IEXHAUST_DEV_INVALID_STATE                     = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IEXHAUST_DEV_NULL_PTR_ACCESS                   = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_EXHAUST_DEV_INVALID_TASKID                     = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_EXHAUST_DEV_INVALID_STATE                      = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_EXHAUST_DEV_NULL_PTR_ACCESS                    = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_EXHAUST_DEV_INIT_FCT_ALLOC_FAILED              = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_EXHAUST_DEV_CONFIG_CONNECT_FAILED              = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_EXHAUST_DEV_FCTMODULES_ERROR                   = 0x0015;  //!< function module error state reported
const quint32 ERROR_DCL_IEXHAUST_DEV_REQ_SENSOR_STATUS_FCTCALL_FAILED  = 0x0016;  //!< function call failed when requesting actual flow sensor status (interface class)
const quint32 ERROR_DCL_EXHAUST_DEV_REQ_SENSOR_STATUS_FCTCALL_FAILED   = 0x0017;  //!< function call failed when requesting actual flow sensor status
const quint32 ERROR_DCL_EXHAUST_DEV_REQ_SENSOR_STATUS_TIMEOUT          = 0x0018;  //!< timeout acknowledge signal
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_EXHAUST_DEV
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_INCL_DEV
*********************************************************/
const quint32 ERROR_DCL_IINCL_DEV_INVALID_STATE                = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IINCL_DEV_NULL_PTR_ACCESS              = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_INCL_DEV_INVALID_TASKID                = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_INCL_DEV_INVALID_STATE                 = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_INCL_DEV_NULL_PTR_ACCESS               = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_INCL_DEV_INIT_FCT_ALLOC_FAILED         = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_INCL_DEV_CONFIG_CONNECT_FAILED         = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_INCL_DEV_FCTMODULES_ERROR              = 0x0015;  //!< function module error state reported
const quint32 ERROR_DCL_IINCL_DEV_REQ_ACTINCL_FCTCALL_FAILED   = 0x0016;  //!< function call failed when calling actual inclination request (interface class)
const quint32 ERROR_DCL_INCL_DEV_REQ_ACTINCL_FCTCALL_FAILED    = 0x0018;  //!< function call failed when calling actual inclination request
const quint32 ERROR_DCL_INCL_DEV_REQ_ACTINCL_TIMEOUT           = 0x0019;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IINCL_DEV_SET_INCLLIMIT_FCTCALL_FAILED = 0x0020;  //!< function call failed when setting inclination limit (interface class)
const quint32 ERROR_DCL_INCL_DEV_SET_INCLLIMIT_FCTCALL_FAILED  = 0x0021;  //!< function call failed when setting inclination limit (interface class)
const quint32 ERROR_DCL_IINCL_DEV_SET_REFPOS_FCTCALL_FAILED    = 0x0023;  //!< function call failed when setting reference position (interface class)
const quint32 ERROR_DCL_INCL_DEV_SET_REFPOS_FCTCALL_FAILED     = 0x0024;  //!< function call failed when setting reference position
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_INCL_DEV
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_LOADER_DEV
*********************************************************/
const quint32 ERROR_DCL_ILOADER_DEV_INVALID_STATE                  = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_ILOADER_DEV_NULL_PTR_ACCESS                = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_LOADER_DEV_INVALID_TASKID                  = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_LOADER_DEV_INVALID_STATE                   = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_LOADER_DEV_NULL_PTR_ACCESS                 = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_LOADER_DEV_INIT_FCT_ALLOC_FAILED           = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED           = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_LOADER_DEV_FCTMODULES_ERROR                = 0x0015;  //!< function module error state reported
const quint32 ERROR_DCL_LOADER_DEV_UNKNOWN_ERROR                   = 0x0016;  //!< error, but no error information available
const quint32 ERROR_DCL_ILOADER_DEV_REQ_REFRUN_FCTCALL_FAILED      = 0x0017;  //!< function call failed when calling reference run request (interface class)
const quint32 ERROR_DCL_LOADER_DEV_REQ_REFRUN_FCTCALL_FAILED       = 0x0018;  //!< function call failed when calling reference run request
const quint32 ERROR_DCL_LOADER_DEV_REQ_REFRUN_TIMEOUT              = 0x0019;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_ILOADER_DEV_SET_DRAWER_POS_FCTCALL_FAILED  = 0x0020;  //!< function call failed when calling movement request (interface class)
const quint32 ERROR_DCL_LOADER_DEV_SET_DRAWER_POS_FCTCALL_FAILED   = 0x0021;  //!< function call failed when calling movement request
const quint32 ERROR_DCL_LOADER_DEV_SET_DRAWER_POS_TIMEOUT          = 0x0022;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_ILOADER_DEV_REQ_DRAWER_POS_FCTCALL_FAILED  = 0x0023;  //!< function call failed when calling actual position request (interface class)
const quint32 ERROR_DCL_LOADER_DEV_REQ_DRAWER_POS_FCTCALL_FAILED   = 0x0024;  //!< function call failed when calling actual position request
const quint32 ERROR_DCL_LOADER_DEV_REQ_DRAWER_POS_TIMEOUT          = 0x0025;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_ILOADER_DEV_REQ_RFID_DATA_FCTCALL_FAILED   = 0x0026;  //!< function call failed when calling rfid data request (interface class)
const quint32 ERROR_DCL_LOADER_DEV_REQ_RFID_DATA_FCTCALL_FAILED    = 0x0027;  //!< function call failed when calling rfid data request
const quint32 ERROR_DCL_LOADER_DEV_REQ_RFID_DATA_TIMEOUT           = 0x0028;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_ILOADER_DEV_SET_BLK_STATE_FCTCALL_FAILED   = 0x0029;  //!< function call failed when setting the blocking state (interface class)
const quint32 ERROR_DCL_LOADER_DEV_SET_BLK_STATE_FCTCALL_FAILED    = 0x0030;  //!< function call failed when setting the blocking state
const quint32 ERROR_DCL_LOADER_DEV_OPEN                            = 0x0040;
const quint32 ERROR_DCL_LOADER_DEV_CLOSED                          = 0x0041;
const quint32 ERROR_DCL_LOADER_DEV_LOCKED                         = 0x0042;
const quint32 ERROR_DCL_LOADER_DEV_UNLOCKED                       = 0x0043;
const quint32 ERROR_DCL_UNLOADER_DEV_OPEN                          = 0x0044;
const quint32 ERROR_DCL_UNLOADER_DEV_CLOSED                        = 0x0045;
const quint32 ERROR_DCL_UNLOADER_DEV_LOCKED                       = 0x0046;
const quint32 ERROR_DCL_UNLOADER_DEV_UNLOCKED                     = 0x0047;
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_LOADER_DEV
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_RACKTRANS_DEV
*********************************************************/
const quint32 ERROR_DCL_IRACKTFR_DEV_INVALID_STATE                  = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IRACKTFR_DEV_NULL_PTR_ACCESS                = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_RACKTFR_DEV_INVALID_TASKID                  = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_RACKTFR_DEV_INVALID_STATE                   = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_RACKTFR_DEV_NULL_PTR_ACCESS                 = 0x0012;  //!< null pointer access
const quint32 EVENT_DCL_RACKTFR_DEV_INIT_FCT_ALLOC_FAILED           = 0x0013;  //!< function module allocation failed
const quint32 EVENT_DCL_RACKTFR_DEV_CONFIG_CONNECT_FAILED           = 0x0014;  //!< signal slot connect failed
const quint32 EVENT_DCL_RACKTFR_DEV_FCTMODULES_ERROR                = 0x0015;  //!< function module error state reported
const quint32 ERROR_DCL_IRACKTFR_DEV_REQ_REFRUN_FCTCALL_FAILED      = 0x0016;  //!< function call failed when calling reference run request (interface class)
const quint32 ERROR_DCL_RACKTFR_DEV_REQ_REFRUN_FCTCALL_FAILED       = 0x0017;  //!< function call failed when calling reference run request
const quint32 ERROR_DCL_RACKTFR_DEV_REQ_REFRUN_TIMEOUT              = 0x0018;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IRACKTFR_DEV_SET_ADAPTER_POS_FCTCALL_FAILED = 0x0019;  //!< function call failed when calling movement request (interface class)
const quint32 ERROR_DCL_RACKTFR_DEV_SET_ADAPTER_POS_FCTCALL_FAILED  = 0x0020;  //!< function call failed when calling movement request
const quint32 ERROR_DCL_RACKTFR_DEV_SET_ADAPTER_POS_TIMEOUT         = 0x0021;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IRACKTFR_DEV_REQ_ACTPOS_FCTCALL_FAILED      = 0x0022;  //!< function call failed when calling actual position request (interface class)
const quint32 ERROR_DCL_RACKTFR_DEV_REQ_ACTPOS_FCTCALL_FAILED       = 0x0023;  //!< function call failed when calling actual position request
const quint32 ERROR_DCL_RACKTFR_DEV_REQ_ACTPOS_TIMEOUT              = 0x0024;  //!< timeout acknowledge signal
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_RACKTRANS_DEV
*********************************************************/


//          grappler device'
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_GRAPPLER_DEV
*********************************************************/
const quint32 ERROR_DCL_IGRAPPLER_DEV_INVALID_STATE              = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IGRAPPLER_DEV_NULL_PTR_ACCESS            = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_GRAPPLER_DEV_INVALID_TASKID              = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_GRAPPLER_DEV_INVALID_STATE               = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_GRAPPLER_DEV_NULL_PTR_ACCESS             = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_GRAPPLER_DEV_INIT_FCT_ALLOC_FAILED       = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED       = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_GRAPPLER_DEV_FCTMODULES_ERROR            = 0x0015;  //!< function module error state reported
const quint32 EVENT_DCL_IGRAPPLER_DEV_REQREFRUN_FCT_CALL_FAILED  = 0x0016;  //!< function call failed when calling reference run request (interface class)
const quint32 EVENT_DCL_GRAPPLER_DEV_REQREFRUN_FCT_CALL_FAILED   = 0x0017;  //!< function call failed when calling reference run request
const quint32 EVENT_DCL_IGRAPPLER_DEV_REQ_ACTION_FCT_CALL_FAILED = 0x0018;  //!< error while requesting positioning (interface class)
const quint32 EVENT_DCL_GRAPPLER_DEV_REQACTPOS_FCT_CALL_FAILED   = 0x0019;  //!< error while requesting actual motor positions
const quint32 EVENT_DCL_GRAPPLER_DEV_CALCSTEPS_FCT_CALL_FAILED   = 0x0020;  //!< error while calculating grappler steps
const quint32 EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_READ_FAILED      = 0x0021;  //!< the file could not be read
const quint32 EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_PARSE_ERROR      = 0x0022;  //!< xml parser error
const quint32 EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_DOCTYPE_ERROR    = 0x0023;  //!< illegal unexpected
const quint32 EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_VERSION_ERROR    = 0x0024;  //!< version unexpected
const quint32 EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_FORMAT_ERROR     = 0x0025;  //!< format not correct
const quint32 EVENT_DCL_GRAPPLER_DEV_LAYOUT_READ_FAILED          = 0x0026;  //!< the file could not be read
const quint32 EVENT_DCL_GRAPPLER_DEV_LAYOUT_PARSE_ERROR          = 0x0027;  //!< xml parser error
const quint32 EVENT_DCL_GRAPPLER_DEV_LAYOUT_DOCTYPE_ERROR        = 0x0028;  //!< illegal unexpected
const quint32 EVENT_DCL_GRAPPLER_DEV_LAYOUT_VERSION_ERROR        = 0x0029;  //!< version unexpected
const quint32 EVENT_DCL_GRAPPLER_DEV_LAYOUT_FORMAT_ERROR         = 0x0030;  //!< format not correct
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_GRAPPLER_DEV
*********************************************************/


//       'Heated vessels'
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_HVESSELS_DEV
*********************************************************/
const quint32 ERROR_DCL_IHVESSELS_DEV_INVALID_STATE               = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IHVESSELS_DEV_NULL_PTR_ACCESS             = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_HVESSELS_DEV_INVALID_TASKID               = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_HVESSELS_DEV_INVALID_STATE                = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_HVESSELS_DEV_NULL_PTR_ACCESS              = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_HVESSELS_DEV_INIT_FCT_ALLOC_FAILED        = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_HVESSELS_DEV_CONFIG_CONNECT_FAILED        = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_HVESSELS_DEV_FCTMODULES_ERROR             = 0x0015;  //!< function module error state reported
const quint32 EVENT_DCL_IHVESSELS_DEV_SET_STATUS_FCT_CALL_FAILED  = 0x0016;  //!< function call failed when requesting actual status of temperature control (interface class)
const quint32 EVENT_DCL_HVESSELS_DEV_SET_STATUS_FCT_CALL_FAILED   = 0x0017;  //!< function call failed when requesting actual status of temperature control (interface class)
const quint32 EVENT_DCL_IHVESSELS_DEV_SET_TEMP_FCT_CALL_FAILED    = 0x0018;  //!< function call failed when setting temperature (interface class)
const quint32 EVENT_DCL_HVESSELS_DEV_SET_TEMP_FCT_CALL_FAILED     = 0x0019;  //!< function call failed when setting temperature
const quint32 EVENT_DCL_IHVESSELS_DEV_REQ_STATUS_FCT_CALL_FAILED  = 0x0020;  //!< function call failed when requesting actual status of temperature control (interface class)
const quint32 EVENT_DCL_HVESSELS_DEV_REQ_STATUS_FCT_CALL_FAILED   = 0x0021;  //!< function call failed when requesting actual status of temperature control
const quint32 EVENT_DCL_HVESSELS_DEV_REQ_STATUS_TIMEOUT           = 0x0022;  //!< timeout acknowledge signal
const quint32 EVENT_DCL_IHVESSELS_DEV_REQ_ACTTEMP_FCT_CALL_FAILED = 0x0023;  //!< function call failed when calling actual temperature request (interface class)
const quint32 EVENT_DCL_HVESSELS_DEV_REQ_ACTTEMP_FCT_CALL_FAILED  = 0x0024;  //!< function call failed when calling actual temperature request
const quint32 EVENT_DCL_HVESSELS_DEV_REQ_ACTTEMP_TIMEOUT          = 0x0025;  //!< timeout acknowledge signal
const quint32 EVENT_DCL_IHVESSELS_DEV_SET_OPMODE_FCT_CALL_FAILED  = 0x0026;  //!< function call failed when set operation mode of temp. control (interface class)
const quint32 EVENT_DCL_HVESSELS_DEV_SET_OPMODE_FCT_CALL_FAILED   = 0x0027;  //!< function call failed when set operation mode of temp. control
const quint32 EVENT_DCL_IHVESSELS_DEV_REQ_OPMODE_FCT_CALL_FAILED  = 0x0028;  //!< function call failed when requesting actual operation mode of temp. control (interface class)
const quint32 EVENT_DCL_HVESSELS_DEV_REQ_OPMODE_FCT_CALL_FAILED   = 0x0029;  //!< function call failed when requesting actual operation mode of temp. control
const quint32 EVENT_DCL_HVESSELS_DEV_REQ_OPMODE_TIMEOUT           = 0x0030;  //!< timeout acknowledge signal
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_HVESSELS_DEV
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_WATER_DEV
*********************************************************/
const quint32 ERROR_DCL_IWATER_DEV_INVALID_STATE                    = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IWATER_DEV_NULL_PTR_ACCESS                  = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_WATER_DEV_INVALID_TASKID                    = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_WATER_DEV_INVALID_STATE                     = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_WATER_DEV_NULL_PTR_ACCESS                   = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_WATER_DEV_INIT_FCT_ALLOC_FAILED             = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_WATER_DEV_CONFIG_CONNECT_FAILED             = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_WATER_DEV_FCTMODULES_ERROR                  = 0x0015;  //!< function module error state reported
const quint32 EVENT_DCL_IWATER_DEV_REQ_LIQUID_LEVEL_FCT_CALL_FAILED = 0x0016;  //!< request liqid level function call failed (interface class)
const quint32 EVENT_DCL_WATER_DEV_REQ_LIQUID_LEVEL_FCT_CALL_FAILED  = 0x0017;  //!< request liqid level function call failed
const quint32 EVENT_DCL_WATER_DEV_REQ_LIQUID_LEVEL_TIMEOUT          = 0x0018;  //!< request liqid level timeout
const quint32 EVENT_DCL_IWATER_DEV_SET_VALVE_FCT_CALL_FAILED        = 0x0019;  //!< set valve state function call failed (interface class)
const quint32 EVENT_DCL_WATER_DEV_SET_VALVE_FCT_CALL_FAILED         = 0x0020;  //!< set valve state function call failed
const quint32 EVENT_DCL_WATER_DEV_SET_VALVE_STATE_TIMEOUT           = 0x0031;  //!< set valve state timeout
const quint32 EVENT_DCL_IWATER_DEV_REQ_VALVE_STATE_FCT_CALL_FAILED  = 0x0032;  //!< request valve state function call failed (interface class)
const quint32 EVENT_DCL_WATER_DEV_REQ_VALVE_STATE_FCT_CALL_FAILED   = 0x0034;  //!< request valve state function call failed
const quint32 EVENT_DCL_WATER_DEV_REQ_VALVE_STATE_TIMEOUT           = 0x0035;  //!< request valve state timeout
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_WATER_DEV
*********************************************************/


/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_OVEN_DEV
*********************************************************/
const quint32 ERROR_DCL_IOVEN_DEV_INVALID_STATE                    = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IOVEN_DEV_NULL_PTR_ACCESS                  = 0x0002;  //!< Null pointer access
const quint32 ERROR_DCL_OVEN_DEV_INVALID_TASKID                    = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_OVEN_DEV_INVALID_STATE                     = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_OVEN_DEV_NULL_PTR_ACCESS                   = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_OVEN_DEV_INIT_FCT_ALLOC_FAILED             = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED             = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_OVEN_DEV_FCTMODULES_ERROR                  = 0x0015;  //!< function module error state reported
const quint32 ERROR_DCL_IOVEN_DEV_COVER_REFRUN_FCTCALL_FAILED      = 0x0016;  //!< function call failed when calling reference run request (interface class)
const quint32 ERROR_DCL_OVEN_DEV_COVER_REFRUN_FCTCALL_FAILED       = 0x0017;  //!< function call failed when calling reference run request
const quint32 ERROR_DCL_OVEN_DEV_COVER_REFRUN_TIMEOUT              = 0x0018;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IOVEN_DEV_REQ_COVER_MOVE_FCTCALL_FAILED    = 0x0019;  //!< function call failed when calling cover movement (interface class)
const quint32 ERROR_DCL_OVEN_DEV_REQ_COVER_MOVE_FCTCALL_FAILED     = 0x0020;  //!< function call failed when calling cover movement
const quint32 ERROR_DCL_OVEN_DEV_REQ_COVER_MOVE_TIMEOUT            = 0x0021;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IOVEN_DEV_REQ_COVER_ACTPOS_FCTCALL_FAILED  = 0x0022;  //!< function call failed when calling actual position request (interface class)
const quint32 ERROR_DCL_OVEN_DEV_REQ_COVER_ACTPOS_FCTCALL_FAILED   = 0x0023;  //!< function call failed when calling actual position request
const quint32 ERROR_DCL_OVEN_DEV_REQ_COVER_ACTPOS_TIMEOUT          = 0x0024;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IOVEN_DEV_REQ_ACTTEMP_FCTCALL_FAILED       = 0x0025;  //!< function call failed when calling actual temperature request (interface class)
const quint32 ERROR_DCL_OVEN_DEV_REQ_ACTTEMP_FCTCALL_FAILED        = 0x0026;  //!< function call failed when calling actual temperature request
const quint32 ERROR_DCL_OVEN_DEV_REQ_ACTTEMP_TIMEOUT               = 0x0027;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IOVEN_DEV_SET_TEMP_FCTCALL_FAILED          = 0x0028;  //!< function call failed when setting temperature (interface class)
const quint32 ERROR_DCL_OVEN_DEV_SET_TEMP_FCTCALL_FAILED           = 0x0029;  //!< function call failed when setting temperature
const quint32 ERROR_DCL_OVEN_DEV_SET_TEMP_TIMEOUT                  = 0x0030;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IOVEN_DEV_SET_STATUS_FCTCALL_FAILED        = 0x0031;  //!< function call failed when setting status of temperature control (interface class)
const quint32 ERROR_DCL_OVEN_DEV_SET_STATUS_FCTCALL_FAILED         = 0x0032;  //!< function call failed when setting status of temperature control
const quint32 ERROR_DCL_OVEN_DEV_SET_STATUS_TIMEOUT                = 0x0033;  //!< timeout acknowledge signal
const quint32 ERROR_DCL_IOVEN_DEV_REQ_STATUS_FCTCALL_FAILED        = 0x0034;  //!< function call failed when requesting actual status of temperature control (interface class)
const quint32 ERROR_DCL_OVEN_DEV_REQ_STATUS_FCTCALL_FAILED         = 0x0035;  //!< function call failed when requesting actual status of temperature control
const quint32 ERROR_DCL_OVEN_DEV_REQ_STATUS_TIMEOUT                = 0x0036;  //!< timeout acknowledge signal
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_OVEN_DEV
*********************************************************/


//          cover line device'
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_COVERLINE_DEV
*********************************************************/
const quint32 ERROR_DCL_ICOVERLINE_DEV_INVALID_STATE              = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_ICOVERLINE_DEV_NULL_PTR_ACCESS            = 0x0002;  //!< null pointer access (interface class)
const quint32 ERROR_DCL_COVERLINE_DEV_INVALID_TASKID              = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_COVERLINE_DEV_INVALID_STATE               = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_COVERLINE_DEV_NULL_PTR_ACCESS             = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED       = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED       = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_COVERLINE_DEV_FCTMODULES_ERROR            = 0x0015;  //!< function module error state reported
const quint32 EVENT_DCL_ICOVERLINE_DEV_REQREFRUN_FCT_CALL_FAILED  = 0x0016;  //!< function call failed when calling reference run request  (interface class)
const quint32 EVENT_DCL_COVERLINE_DEV_REQREFRUN_FCT_CALL_FAILED   = 0x0017;  //!< function call failed when calling reference run request
const quint32 EVENT_DCL_ICOVERLINE_DEV_REQCOVER_FCT_CALL_FAILED   = 0x0018;  //!< function call failed when calling cover process  (interface class)
const quint32 EVENT_DCL_COVERLINE_DEV_REQCOVER_FCT_CALL_FAILED    = 0x0019;  //!< function call failed when calling cover process
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_COVERLINE_DEV
*********************************************************/


//          RackHandling device
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_RACKHDL_DEV
*********************************************************/
const quint32 ERROR_DCL_IRACKHDL_DEV_INVALID_STATE                    = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IRACKHDL_DEV_NULL_PTR_ACCESS                  = 0x0002;  //!< null pointer access (interface class)
const quint32 EVENT_DCL_RACKHDL_DEV_ADJUSTMENT_FORMAT_ERROR           = 0x0003;  //!< format error at ajustment file (xml)
const quint32 ERROR_DCL_RACKHDL_DEV_INVALID_TASKID                    = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_RACKHDL_DEV_INVALID_STATE                     = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_RACKHDL_DEV_NULL_PTR_ACCESS                   = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_RACKHDL_DEV_INIT_FCT_ALLOC_FAILED             = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED             = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_RACKHDL_DEV_FCTMODULES_ERROR                  = 0x0015;  //!< function module error state reported
const quint32 EVENT_DCL_IRACKHDL_DEV_REQREFRUN_FCT_CALL_FAILED        = 0x0016;  //!< function call failed when calling reference run request (interface class)
const quint32 EVENT_DCL_RACKHDL_DEV_REQREFRUN_FCT_CALL_FAILED         = 0x0017;  //!< function call failed when calling reference run request
const quint32 EVENT_DCL_IRACKHDL_DEV_REQACTION_FCT_CALL_FAILED        = 0x0018;  //!< function call failed when calling action request (interface class)
const quint32 EVENT_DCL_RACKHDL_DEV_REQACTPOS_FCT_CALL_FAILED         = 0x0019;  //!< function call failed when calling position request
const quint32 EVENT_DCL_RACKHDL_DEV_CALC_TRANSP_FCT_CALL_FAILED       = 0x0020;  //!< function call failed when calling rack transport request
const quint32 EVENT_DCL_RACKHDL_DEV_TRANSP_TASK_START_FCT_CALL_FAILED = 0x0021;  //!< function call failed when calling rack transport request within the process
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_RACKHDL_DEV
*********************************************************/


//          Hood open detection
/*********************************************************
START_OF_GROUP EVENT_GRP_DCL_HOOD_DEV
*********************************************************/
const quint32 ERROR_DCL_IHOOD_DEV_INVALID_STATE                    = 0x0001;  //!< invalid state (interface class)
const quint32 ERROR_DCL_IHOOD_DEV_NULL_PTR_ACCESS                  = 0x0002;  //!< null pointer access (interface class)
const quint32 EVENT_DCL_HOOD_DEV_ADJUSTMENT_FORMAT_ERROR           = 0x0003;  //!< format error at ajustment file (xml)
const quint32 ERROR_DCL_HOOD_DEV_INVALID_TASKID                    = 0x0010;  //!< invalid task id
const quint32 ERROR_DCL_HOOD_DEV_INVALID_STATE                     = 0x0011;  //!< invalid state
const quint32 ERROR_DCL_HOOD_DEV_NULL_PTR_ACCESS                   = 0x0012;  //!< null pointer access
const quint32 ERROR_DCL_HOOD_DEV_INIT_FCT_ALLOC_FAILED             = 0x0013;  //!< function module allocation failed
const quint32 ERROR_DCL_HOOD_DEV_CONFIG_CONNECT_FAILED             = 0x0014;  //!< signal slot connect failed
const quint32 ERROR_DCL_HOOD_DEV_FCTMODULES_ERROR                  = 0x0015;  //!< function module error state reported
const quint32 ERROR_DCL_HOOD_DEV_UNKNOWN_ERROR                     = 0x0016;  //!< error, but no error information available
const quint32 ERROR_DCL_IHOOD_DEV_REQ_STATUS_FCTCALL_FAILED        = 0x0017;  //!< function call failed when calling get status (interface class)
const quint32 ERROR_DCL_HOOD_DEV_REQ_STATUS_FCTCALL_FAILED         = 0x0018;  //!< function call failed when calling get status
const quint32 ERROR_DCL_HOOD_DEV_REQ_STATUS_TIMEOUT                = 0x0019;  //!< timeout acknowledge signal
const quint32 EVENT_DCL_HOOD_DEV_OPEN                              = 0x0201;  //!< hood has been opened
const quint32 EVENT_DCL_HOOD_DEV_CLOSED                            = 0x0202;  //!< hood has been closed
/*********************************************************
END_OF_GROUP EVENT_GRP_DCL_HOOD_DEV
*********************************************************/


#endif /* DEVICE_CONTROL_ERROR_H */

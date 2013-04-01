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

/* General description
 *  -> remark: sometimes instead of 'error' the term 'event' is used.
 * The errors will be separated by error group and error identifier
 *
 * The error group is a 16bit number,
 *  the high byte is named 'error main group id'
 *  the low byte is named 'error sub group id'
 *  The 'error main group id' typically defined by each software module, each module should use a unique abbreviation
 *    e.g. Application Control 0x01
 *         GUI                 0x02
 *         DeviceControlLayer  0x03
 *         Scheduler           0x04
 *
 *  The 'event sub group id' is defined by each software module according to its submodules
 *    e.g. CAN interface       0x01
 *         Configuration       0x02
 *         Adjustment          0x03
 *
 * The event id is a 16bit number, this number can be defined freely within each event group
 *
 */

/*********************************************************/
/*! list of error main group id's (example)  */
/*********************************************************/
#define EVENT_MAIN_GRP_APP                      0x01  //!< Application Control
#define EVENT_MAIN_GRP_GUI                      0x02  //!< GUI
#define EVENT_MAIN_GRP_DCL                      0x03  //!< Device control layer
#define EVENT_MAIN_GRP_SCD                      0x04  //!< Scheduler
#define EVENT_MAIN_GRP_NODE                     0x10  //!< Node
#define EVENT_MAIN_GRP_FUNCMOD                  0x20  //!< Function module


/********************************************************/
/*! list of 'Application Control' sub group id's  (example) */
/********************************************************/
#define EVENT_GRP_APP_UNIVERSAL                 (EVENT_MAIN_GRP_APP << 8) | 0x0001  //!< universal errors and events
#define EVENT_GRP_APP_EVENT_HDL                 (EVENT_MAIN_GRP_APP << 8) | 0x0002  //!< error concerning the 'event definition file' reading process


/********************************************************/
/*! list of Device Control Layer sub group id's */
/********************************************************/
#define EVENT_GRP_DCL_DEVCTRL                   ((EVENT_MAIN_GRP_DCL << 8) | 0x0001)   //!< device control
#define EVENT_GRP_DCL_CANINTERFACE              ((EVENT_MAIN_GRP_DCL << 8) | 0x0002)   //!< can interface
#define EVENT_GRP_DCL_CONFIGURATION             ((EVENT_MAIN_GRP_DCL << 8) | 0x0003)   //!< configuration
#define EVENT_GRP_DCL_DEVICEPROC_CONFIG         ((EVENT_MAIN_GRP_DCL << 8) | 0x0004)   //!< device processing - configuration
#define EVENT_GRP_DCL_CANBUS                    ((EVENT_MAIN_GRP_DCL << 8) | 0x0005)   //!< can bus
#define EVENT_GRP_DCL_DIAG                      ((EVENT_MAIN_GRP_DCL << 8) | 0x0006)   //!< diagnsostic / service

/*! CAN-Object error groups, node and all function modules
 * those error appear on master side within the device control layer (dcl) */
#define EVENT_GRP_DCL_NODE_DCL                  ((EVENT_MAIN_GRP_DCL << 8) | 0x0010)   //!< CANNode
#define EVENT_GRP_DCL_FCTMOD_DIG_INP_DCL        ((EVENT_MAIN_GRP_DCL << 8) | 0x0011)   //!< digital input
#define EVENT_GRP_DCL_FCTMOD_DIG_OUTP_DCL       ((EVENT_MAIN_GRP_DCL << 8) | 0x0012)   //!< digital output
#define EVENT_GRP_DCL_FCTMOD_ANA_INP_DCL        ((EVENT_MAIN_GRP_DCL << 8) | 0x0013)   //!< analog input
#define EVENT_GRP_DCL_FCTMOD_ANA_OUTP_DCL       ((EVENT_MAIN_GRP_DCL << 8) | 0x0014)   //!< analog output
#define EVENT_GRP_DCL_FCTMOD_INCL_DCL           ((EVENT_MAIN_GRP_DCL << 8) | 0x0015)   //!< inclinometer
#define EVENT_GRP_DCL_FCTMOD_MOTOR_DCL          ((EVENT_MAIN_GRP_DCL << 8) | 0x0016)   //!< stepper motor
#define EVENT_GRP_DCL_FCTMOD_RFID11785_DCL      ((EVENT_MAIN_GRP_DCL << 8) | 0x0017)   //!< RFID ISO 11785
#define EVENT_GRP_DCL_FCTMOD_RFID15693_DCL      ((EVENT_MAIN_GRP_DCL << 8) | 0x0018)   //!< RFID ISO 15693
#define EVENT_GRP_DCL_FCTMOD_TEMP_DCL           ((EVENT_MAIN_GRP_DCL << 8) | 0x0019)   //!< temperature control
#define EVENT_GRP_DCL_FCTMOD_JSTCK_DCL          ((EVENT_MAIN_GRP_DCL << 8) | 0x001A)   //!< joystick
#define EVENT_GRP_DCL_FCTMOD_UART_DCL           ((EVENT_MAIN_GRP_DCL << 8) | 0x001B)   //!< UART

/*! slave object error groups, include the node's and all function module error
 *  those error appear on slave side and will be reported via CAN bus (slv) */
#define EVENT_GRP_DCL_NODE_SLV                  ((EVENT_MAIN_GRP_DCL << 8) | 0x0020)   //!< CANNode
#define EVENT_GRP_DCL_FCTMOD_DIG_INP_SLV        ((EVENT_MAIN_GRP_DCL << 8) | 0x0021)   //!< digital input
#define EVENT_GRP_DCL_FCTMOD_DIG_OUTP_SLV       ((EVENT_MAIN_GRP_DCL << 8) | 0x0022)   //!< digital output
#define EVENT_GRP_DCL_FCTMOD_ANA_INP_SLV        ((EVENT_MAIN_GRP_DCL << 8) | 0x0023)   //!< analog input
#define EVENT_GRP_DCL_FCTMOD_ANA_OUTP_SLV       ((EVENT_MAIN_GRP_DCL << 8) | 0x0024)   //!< analog output
#define EVENT_GRP_DCL_FCTMOD_INCL_SLV           ((EVENT_MAIN_GRP_DCL << 8) | 0x0025)   //!< inclinometer
#define EVENT_GRP_DCL_FCTMOD_MOTOR_SLV          ((EVENT_MAIN_GRP_DCL << 8) | 0x0026)   //!< stepper motor
#define EVENT_GRP_DCL_FCTMOD_RFID_SLV           ((EVENT_MAIN_GRP_DCL << 8) | 0x0027)   //!< RFID
#define EVENT_GRP_DCL_FCTMOD_TEMP_SLV           ((EVENT_MAIN_GRP_DCL << 8) | 0x0028)   //!< temperature control
#define EVENT_GRP_DCL_FCTMOD_JSTCK_SLV          ((EVENT_MAIN_GRP_DCL << 8) | 0x0029)   //!< joystick
#define EVENT_GRP_DCL_FCTMOD_UART_SLV           ((EVENT_MAIN_GRP_DCL << 8) | 0x001A)   //!< UART

#define EVENT_GRP_DCL_GRAPPLER_DEV              ((EVENT_MAIN_GRP_DCL << 8) | 0x0030)   //!< Grappler
#define EVENT_GRP_DCL_AGITATION_DEV             ((EVENT_MAIN_GRP_DCL << 8) | 0x0031)   //!< Agitation
#define EVENT_GRP_DCL_RACKTRANS_DEV             ((EVENT_MAIN_GRP_DCL << 8) | 0x0032)   //!< Rack transport
//#define EVENT_GRP_DCL_OVEN_DEV                  ((EVENT_MAIN_GRP_DCL << 8) | 0x0033)   //!< Oven
#define EVENT_GRP_DCL_LOADER_DEV                ((EVENT_MAIN_GRP_DCL << 8) | 0x0034)   //!< Loader
#define EVENT_GRP_DCL_HVESSELS_DEV              ((EVENT_MAIN_GRP_DCL << 8) | 0x0035)   //!< Heated vessels
#define EVENT_GRP_DCL_WATER_DEV                 ((EVENT_MAIN_GRP_DCL << 8) | 0x0036)   //!< Water
#define EVENT_GRP_DCL_EXHAUST_DEV               ((EVENT_MAIN_GRP_DCL << 8) | 0x0037)   //!< Exhaust
#define EVENT_GRP_DCL_INCL_DEV                  ((EVENT_MAIN_GRP_DCL << 8) | 0x0038)   //!< Inclinometer
#define EVENT_GRP_DCL_COVERLINE_DEV             ((EVENT_MAIN_GRP_DCL << 8) | 0x0039)   //!< Coverline
#define EVENT_GRP_DCL_RACKHDL_DEV               ((EVENT_MAIN_GRP_DCL << 8) | 0x003A)   //!< Rackhandling sepia
#define EVENT_GRP_DCL_HOOD_DEV                  ((EVENT_MAIN_GRP_DCL << 8) | 0x003B)   //!< Hood Open Detection
#define EVENT_GRP_DCL_RV_DEV                    ((EVENT_MAIN_GRP_DCL << 8) | 0x003C)   //!< Rotary Valve

#define EVENT_GRP_DCL_AL_DEV                    ((EVENT_MAIN_GRP_DCL << 8) | 0x003D)   //!< Air liquid
#define EVENT_GRP_DCL_RT_DEV                    ((EVENT_MAIN_GRP_DCL << 8) | 0x003E)   //!< Retort
#define EVENT_GRP_DCL_OVEN_DEV                  ((EVENT_MAIN_GRP_DCL << 8) | 0x003F)   //!< Oven
#define EVENT_GRP_DCL_PER_DEV                   ((EVENT_MAIN_GRP_DCL << 8) | 0x0040)   //!< Periphery


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
 *  Error group EVENT_GRP_DCL_DEVCTRL */
#define ERROR_DCL_DEVCTRL_START_COMM             0x0001  //!< Event start communication
#define ERROR_DCL_DEVCTRL_READCONFIG             0x0002  //!< Event read configuration
#define EVENT_DCL_DEVCTRL_START_DIAG             0x0003  //!< Event start service
#define EVENT_DCL_DEVCTRL_START_NORMAL_OP        0x0004  //!< Event start normal operation mode
#define EVENT_DCL_DEVCTRL_BREAK_NORMAL_OP        0x0005  //!< Event break normal operation mode
#define ERROR_DCL_DEVCTRL_SERIALNO_FAILED        0x0006  //!< serial number not correct
#define ERROR_DCL_DEVCTRL_ACTIVATE_TASK_FAILED   0x0007  //!< Could not activate a device processing task
#define ERROR_DCL_DEVCTRL_HEARTBEAT_ERROR        0x0008  //!< Problems with heartbeat distribution

/*********************************************************
 *  Error group EVENT_GRP_DCL_CANBUS */
#define ERROR_DCL_CANBUS_WRITE                   0x0001  //!< writeing a CAN message failed

/*********************************************************
 *  Error group EVENT_GRP_DCL_CONFIGURATION */
#define ERROR_DCL_CONFIG_CAN_MESSAGE_ASSIGN            0x0001  //!< CAN message assignment failed
#define ERROR_DCL_CONFIG_HW_CFG_OPEN_FAILED            0x0010  //!< hardware configuration, open config file failed
#define ERROR_DCL_CONFIG_HW_CFG_PARSE_ERROR            0x0011  //!< hardware configuration, general parsing error
#define ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR           0x0012  //!< hardware configuration, format error
#define ERROR_DCL_CONFIG_HW_CFG_VERSION_ERROR          0x0013  //!< hardware configuration, version confict
#define ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_MST       0x0014  //!< hardware configuration, section master
#define ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_CAN       0x0015  //!<  hardware configuration, can interface missed
#define ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_SLV       0x0016  //!< hardware configuration, section slave
#define ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_FCT       0x0017  //!< hardware configuration, section function module
#define ERROR_DCL_CONFIG_HW_CFG_FORMAT_ERROR_DEV       0x0018  //!< hardware configuration, section device

#define ERROR_DCL_CONFIG_CAN_MSG_CFG_OPEN_FAILED       0x0020  //!< CAN message configuration, open config file failed
#define ERROR_DCL_CONFIG_CAN_MSG_CFG_PARSE_ERROR       0x0021  //!< CAN message configuration, general parsing error
#define ERROR_DCL_CONFIG_CAN_MSG_CFG_FORMAT_ERROR      0x0022  //!< CAN message configuration, format error
#define ERROR_DCL_CONFIG_CAN_MSG_CFG_VERSION_ERROR     0x0023  //!< CAN message configuration, version confict
#define ERROR_DCL_CONFIG_CAN_MSG_CFG_FORMAT_ERROR_BASE 0x0023  //!< CAN message configuration, section base module
#define ERROR_DCL_CONFIG_CAN_MSG_CFG_FORMAT_ERROR_FCT  0x0024  //!< CAN message configuration, section function module
#define ERROR_DCL_CONFIG_CAN_MSG_CFG_FORMAT_ERROR_MSG  0x0025  //!< CAN message configuration, section message

#define ERROR_DCL_CONFIG_PROCSETTING_OPEN_FAILED       0x0030   //!< error reading process settings file
#define ERROR_DCL_CONFIG_PROCSETTING_PARSE_ERROR       0x0031   //!< parser error while reading process settings file
#define ERROR_DCL_CONFIG_PROCSETTING_FORMAT_ERROR      0x0032   //!< format error at process settings file
#define ERROR_DCL_CONFIG_PROCSETTING_VERSION_ERROR     0x0033   //!< version error at process settings file

#define ERROR_DCL_CONFIG_CAN_NODE_IDLE_TIMEOUT         0x0040   //!< timeout while waiting for all CAN nodes switching to idle mode
#define ERROR_DCL_CONFIG_CAN_NODE_ERROR                0x0041   //!< CAN node switched to error node while waiting for idle mode

#define ERROR_DCL_CONFIG_DEVICE_TYPE_INVALID           0x0050   //!< Invalid device type key

#define ERROR_DCL_CONFIG_FINISHED                      0x0100    //!< event notification 'coniguration finished'


/*********************************************************
 * Error group EVENT_GRP_DCL_DIAG */
#define ERROR_DCL_DIAG_FINISHED                   0x0100    //!< event notification 'diagnostic finished'


/*********************************************************
 * Error group EVENT_GRP_DCL_NODE_DCL */
#define ERROR_DCL_NODE_TIMEOUT_INIT               0x0001  //!< timeout between CANNode initialization and 'HardwareID' receipt
                                                          // additional information: none
                                                          // this errors appears while initialization state is active, the CANNode waits for 'HardwareID' which
                                                          // is not received.
                                                          // Typically the can bus is not connected to the slaves, or the 'HardwareID'-msg identifier is not correct

#define ERROR_DCL_NODE_TIMEOUT_CONFIG             0x0002  //!< timeout between 'ConfigurationReq' and 'Configuration' receipt
                                                          //   additional information: Byte1: fct module's type (CAN-message byte0)
                                                          //                           Byte2: fct module's channel
#define ERROR_DCL_NODE_INIT_WRONG_CHANNEL_COUNT   0x0003  //!< the number if channels read from hardware configuration file and the number of channels
                                                          //   announced by CAN message 'HardwareID' doesn't match
                                                          //   additional information: Byte1: channel count from config
                                                          //                           Byte2: channel count from node
#define ERROR_DCL_NODE_CONFIG_INVALID_MODULE      0x0004  //!< the module information as read from configuration file does not match to the information
                                                          //   announced by CAN-message 'Configuration' from the node
                                                          //    additional information: Byte1: channel count from config
                                                          //                            Byte2: channel count from node
#define ERROR_DCL_NODE_CONFIG_UNEXP_CANMSG        0x0005  //!< The CAN message 'Configuration' was received while it was not expected
                                                          //   additional information: Byte1: channel from node (CAN-message byte0)
                                                          //                           Byte2: node main state
#define ERROR_DCL_NODE_TIMEOUT_FCT_CONFIG         0x0006  //!< timeout while the software tried to configure the node's function modules
                                                          //   additional information: Byte1: fct module's type (CAN-message byte0)
                                                          //                           Byte2: fct module's channel
#define ERROR_DCL_NODE_HEARTBEAT_TIMEOUT          0x0007  //!< node heartbeat timeout

#define ERROR_DCL_NODE_HARDWAREID_INTERVALL       0x0008  //!< the can message HardwareID was received within the minimal time delay
                                                          //    there are two nodes with same nodeID connected to can bus
                                                          //    additional information: Byte1: 0
                                                          //                            Byte2: 0

#define ERROR_DCL_NODE_HWID_REC_INVALID_STATE     0x0009  //!< the can message HardwareID was received although the node is already configured
                                                          //   -> unexpected node reboot (watchdog?)
                                                          //   additional information: Byte1: 0
                                                          //                           Byte2: 0
#define ERROR_DCL_NODE_INVALID_STATE              0x000A  //!< invalid state
#define ERROR_DCL_NODE_NULL_PTR_ACCESS            0x000B  //!< null pointer access
#define ERROR_DCL_NODE_FCT_CALL_FAILED            0x000C  //!< function call failed
#define ERROR_DCL_NODE_CANMSG_INVALID_DLC         0x000D  //!< a can message with invalid data length code was received ( <> expected)

#define EVENT_DCL_INODE_SET_NODE_STATE_FCTCALL_FAILED   0x0100   //!< set node state
#define EVENT_DCL_NODE_SET_NODE_STATE_CANMSG_NOT_SEND   0x0101   //!< CAN message was not send
#define EVENT_DCL_NODE_SET_NODE_STATE_CANMSG_TIMEOUT    0x0102   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_NODE_STATE_FCTCALL_FAILED   0x0110   //!< request node state
#define EVENT_DCL_NODE_REQ_NODE_STATE_CANMSG_NOT_SEND   0x0111   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_NODE_STATE_CANMSG_TIMEOUT    0x0112   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_EMCY_STOP_FCTCALL_FAILED    0x0120   //!< request partition data reset
#define EVENT_DCL_NODE_REQ_EMCY_STOP_CANMSG_NOT_SEND    0x0121   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_EMCY_STOP_CANMSG_TIMEOUT     0x0122   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_RESET_FCTCALL_FAILED        0x0130   //!< request partition data reset
#define EVENT_DCL_NODE_REQ_RESET_CANMSG_NOT_SEND        0x0131   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_RESET_CANMSG_TIMEOUT         0x0132   //!< acknowledge timeout
#define EVENT_DCL_INODE_CONV_STATISTICS_FCTCALL_FAILED  0x0140   //!< request statistic data failed
#define EVENT_DCL_NODE_CONV_STATISTICS_CANMSG_NOT_SEND  0x0141   //!< CAN message was not send
#define EVENT_DCL_NODE_CONV_STATISTICS_CANMSG_TIMEOUT   0x0142   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_DATA_RESET_FCTCALL_FAILED   0x0150   //!< request partition data reset
#define EVENT_DCL_NODE_REQ_DATA_RESET_CANMSG_NOT_SEND   0x0151   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_DATA_RESET_CANMSG_TIMEOUT    0x0152   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_FORMAT_MEM_FCTCALL_FAILED   0x0160   //!< request formatting the memmory
#define EVENT_DCL_NODE_REQ_FORMAT_MEM_CANMSG_NOT_SEND   0x0161   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_FORMAT_MEM_CANMSG_TIMEOUT    0x0162   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_SER_NB_FCTCALL_FAILED       0x0170   //!< request serial hw number
#define EVENT_DCL_NODE_REQ_SER_NB_CANMSG_NOT_SEND       0x0171   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_SER_NB_CANMSG_TIMEOUT        0x0172   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_END_TEST_RES_FCTCALL_FAILED 0x0180   //!< function call 'request test result' failed (from ICANNode to CANNode)
#define EVENT_DCL_NODE_REQ_END_TEST_RES_CANMSG_NOT_SEND 0x0181   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_END_TEST_RES_CANMSG_TIMEOUT  0x0182   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_HW_INFO_FCTCALL_FAILED      0x0190   //!< request hw info
#define EVENT_DCL_NODE_REQ_HW_INFO_CANMSG_NOT_SEND      0x0191   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_HW_INFO_CANMSG_TIMEOUT       0x0192   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_SW_INFO_FCTCALL_FAILED      0x01a0   //!< request sw info
#define EVENT_DCL_NODE_REQ_SW_INFO_CANMSG_NOT_SEND      0x01a1   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_SW_INFO_CANMSG_TIMEOUT       0x01a2   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_LOADER_INFO_FCTCALL_FAILED  0x01b0   //!< request sw info
#define EVENT_DCL_NODE_REQ_LOADER_INFO_CANMSG_NOT_SEND  0x01b1   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_LOADER_INFO_CANMSG_TIMEOUT   0x01b2   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_LIFECYC_DATA_FCTCALL_FAILED 0x01c0   //!< function call failed
#define EVENT_DCL_NODE_REQ_LIFECYC_DATA_CANMSG_NOT_SEND 0x01c1   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_LIFECYC_DATA_CANMSG_TIMEOUT  0x01c2   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_LAUNCH_DATE_FCTCALL_FAILED  0x01d0   //!< function call failed
#define EVENT_DCL_NODE_REQ_LAUNCH_DATE_CANMSG_NOT_SEND  0x01d1   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_LAUNCH_DATE_CANMSG_TIMEOUT   0x01d2   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_BOARD_NAME_FCTCALL_FAILED   0x01e0   //!< function call failed
#define EVENT_DCL_NODE_REQ_BOARD_NAME_CANMSG_NOT_SEND   0x01e1   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_BOARD_NAME_CANMSG_TIMEOUT    0x01e2   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_BOARD_OPT_FCTCALL_FAILED    0x01f0   //!< request
#define EVENT_DCL_NODE_REQ_BOARD_OPT_CANMSG_NOT_SEND    0x01f1   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_BOARD_OPT_CANMSG_TIMEOUT     0x01f2   //!< acknowledge timeout
#define EVENT_DCL_INODE_CONV_VOLTAGE_MON_FCTCALL_FAILED 0x0200   //!< function call failed
#define EVENT_DCL_NODE_CONV_VOLTAGE_MON_CANMSG_NOT_SEND 0x0201   //!< CAN message was not send
#define EVENT_DCL_NODE_CONV_VOLTAGE_MON_CANMSG_TIMEOUT  0x0202   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_VOLT_STATE_FCTCALL_FAILED   0x0210   //!< request
#define EVENT_DCL_NODE_REQ_VOLT_STATE_CANMSG_NOT_SEND   0x0211   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_VOLT_STATE_CANMSG_TIMEOUT    0x0212   //!< acknowledge timeout
#define EVENT_DCL_INODE_CONV_CURRENT_MON_FCTCALL_FAILED 0x0220   //!< function call failed
#define EVENT_DCL_NODE_CONV_CURRENT_MON_CANMSG_NOT_SEND 0x0221   //!< CAN message was not send
#define EVENT_DCL_NODE_CONV_CURRENT_MON_CANMSG_TIMEOUT  0x0222   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_CURR_STATE_FCTCALL_FAILED   0x0230   //!< request
#define EVENT_DCL_NODE_REQ_CURR_STATE_CANMSG_NOT_SEND   0x0231   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_CURR_STATE_CANMSG_TIMEOUT    0x0232   //!< acknowledge timeout
#define EVENT_DCL_INODE_REQ_UNIQUE_NB_FCTCALL_FAILED    0x0240   //!< request
#define EVENT_DCL_NODE_REQ_UNIQUE_NB_CANMSG_NOT_SEND    0x0241   //!< CAN message was not send
#define EVENT_DCL_NODE_REQ_UNIQUE_NB_CANMSG_TIMEOUT     0x0242   //!< acknowledge timeout


// Group EVENT_GRP_DCL_FCTMOD_TEMP_DCL
#define EVENT_DCL_FCTMOD_TEMP_DCL_INVALID_IDLE_STATE  0x0001   //!< invalid state at idle mode
#define EVENT_DCL_FCTMOD_TEMP_DCL_TIMEOUT_CANMSG      0x0002   //!< acknowledge timeout

//##############################################
//  Group EVENT_GRP_DCL_FCTMOD_MOTOR_DCL
//  stepper motor
#define EVENT_DCL_FCTMOD_MOTOR_NULL_PTR_ACCESS           0x0001  //!< null pointer access
#define EVENT_DCL_IFCTMOD_MOTOR_NULL_PTR_ACCESS          0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_FCTMOD_MOTOR_INVALID_MAIN_TASK_STATE   0x0003  //!< invalid state
#define ERROR_DCL_FCTMOD_MOTOR_CANMSG_INVALID_DLC        0x0004  //!< a can message with invalid data length code was received ( <> expected)
#define EVENT_DCL_IFCTMOD_MOTOR_SET_STATE_FCTCALL_FAILED 0x0010  //!< state set failed
#define EVENT_DCL_FCTMOD_MOTOR_SET_STATE_CANMSG_NOT_SEND 0x0011  //!< CAN message 'SetState' couldn't be send
#define EVENT_DCL_FCTMOD_MOTOR_SET_STATE_CANMSG_TIMEOUT  0x0012  //!< timeout while waiting for state acknowledge
#define EVENT_DCL_FCTMOD_MOTOR_SET_STATE_INVALID_STATE   0x0013  //!< invalid state
#define EVENT_DCL_IFCTMOD_MOTOR_SET_STATE_INVALID_STATE  0x0014  //!< invalid state (interface class)
#define EVENT_DCL_IFCTMOD_MOTOR_REFRUN_FCTCALL_FAILED    0x0020  //!< request reference run failed
#define EVENT_DCL_FCTMOD_MOTOR_REFRUN_CANMSG_NOT_SEND    0x0021  //!< CAN message 'reference run' couln't be send
#define EVENT_DCL_FCTMOD_MOTOR_REFRUN_CANMSG_TIMEOUT     0x0022  //!< timeout while waiting for reference run acknowledge
#define EVENT_DCL_FCTMOD_MOTOR_REFRUN_INVALID_STATE      0x0023  //!< invalid state
#define EVENT_DCL_IFCTMOD_MOTOR_REFRUN_INVALID_STATE     0x0024  //!< invalid state (interface class)
#define EVENT_DCL_IFCTMOD_MOTOR_POSREQ_FCTCALL_FAILED    0x0030  //!< request positioning failed
#define EVENT_DCL_FCTMOD_MOTOR_POSREQ_CANMSG_NOT_SEND    0x0031  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_MOTOR_POSREQ_CANMSG_TIMEOUT     0x0032  //!< acknowledge timeout
#define EVENT_DCL_FCTMOD_MOTOR_POSREQ_INVALID_STATE      0x0033  //!< invalid state
#define EVENT_DCL_IFCTMOD_MOTOR_POSREQ_INVALID_STATE     0x0034  //!< invalid state (interface class)
#define EVENT_DCL_FCTMOD_MOTOR_ACTPOSREQ_FCTCALL_FAILED  0x0040  //!< request actual position failed
#define EVENT_DCL_FCTMOD_MOTOR_ACTPOSREQ_CANMSG_NOT_SEND 0x0041  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_MOTOR_ACTPOSREQ_CANMSG_TIMEOUT  0x0042  //!< acknowledge timeout
#define EVENT_DCL_FCTMOD_MOTOR_ACTPOSREQ_INVALID_STATE   0x0043  //!< invalid state
#define EVENT_DCL_IFCTMOD_MOTOR_ACTPOSREQ_INVALID_STATE  0x0044  //!< invalid state (interface class)
#define EVENT_DCL_FCTMOD_MOTOR_ACTSPDREQ_FCTCALL_FAILED  0x0050  //!< request actual speed failed
#define EVENT_DCL_FCTMOD_MOTOR_ACTSPDREQ_CANMSG_NOT_SEND 0x0051  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_MOTOR_ACTSPDREQ_CANMSG_TIMEOUT  0x0052  //!< acknowledge timeout
#define EVENT_DCL_FCTMOD_MOTOR_ACTSPDREQ_INVALID_STATE   0x0053  //!< invalid state
#define EVENT_DCL_IFCTMOD_MOTOR_ACTSPDREQ_INVALID_STATE  0x0054  //!< invalid state (interface class)
#define EVENT_DCL_FCTMOD_MOTOR_SPDREQ_FCTCALL_FAILED     0x0060  //!< request move speed failed
#define EVENT_DCL_FCTMOD_MOTOR_SPDREQ_CANMSG_NOT_SEND    0x0061  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_MOTOR_SPDREQ_CANMSG_TIMEOUT     0x0062  //!< acknowledge timeout
#define EVENT_DCL_FCTMOD_MOTOR_SPDREQ_INVALID_STATE      0x0063  //!< invalid state
#define EVENT_DCL_IFCTMOD_MOTOR_SPDREQ_INVALID_STATE     0x0064  //!< invalid state (interface class)
#define EVENT_DCL_FCTMOD_MOTOR_LIFECYCLEDATA_CANMSG_NOT_SEND 0x0071  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_MOTOR_LIFECYCLEDATA_CANMSG_TIMEOUT  0x0072  //!< acknowledge timeout
#define EVENT_DCL_IFCTMOD_MOTOR_LIFECYCLEDATA_INVALID_STATE  0x0074  //!< invalid state (interface class)

//##############################################
//  Group EVENT_GRP_DCL_FCTMOD_TEMP_DCL
//  temperature control
#define EVENT_DCL_FCTMOD_TEMP_NULL_PTR_ACCESS              0x0001  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_NULL_PTR_ACCESS             0x0002  //!< invalid state (interface class)
#define ERROR_DCL_FCTMOD_TEMP_INVALID_MAIN_TASK_STATE      0x0003  //!< invalid main state
#define ERROR_DCL_FCTMOD_TEMP_CANMSG_INVALID_DLC           0x0004   //!< invalid data length code detected
#define EVENT_DCL_IFCTMOD_TEMP_SET_TEMP_FCTCALL_FAILED     0x0010  //!< set temperature failed
#define EVENT_DCL_FCTMOD_TEMP_SET_TEMP_CANMSG_NOT_SEND     0x0011  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_TEMP_SET_TEMP_INVALID_STATE       0x0012  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_SET_TEMP_INVALID_STATE      0x0013  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_REQ_ACTTEMP_FCTCALL_FAILED  0x0020  //!< request actual temperature failed
#define EVENT_DCL_FCTMOD_TEMP_REQ_ACTTEMP_CANMSG_NOT_SEND  0x0021  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_TEMP_REQ_ACTTEMP_CANMSG_TIMEOUT   0x0022  //!< timeout of the acknowledge
#define EVENT_DCL_FCTMOD_TEMP_REQ_ACTTEMP_INVALID_STATE    0x0023  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_REQ_ACTTEMP_INVALID_STATE   0x0024  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_SET_OPMODE_FCTCALL_FAILED   0x0030  //!< set operation mode failed
#define EVENT_DCL_FCTMOD_TEMP_SET_OPMODE_CANMSG_NOT_SEND   0x0031  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_TEMP_SET_OPMODE_INVALID_STATE     0x0033  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_SET_OPMODE_INVALID_STATE    0x0034  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_REQ_OPMODE_FCTCALL_FAILED   0x0040  //!< request temperature failed
#define EVENT_DCL_FCTMOD_TEMP_REQ_OPMODE_CANMSG_NOT_SEND   0x0041  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_TEMP_REQ_OPMODE_CANMSG_TIMEOUT    0x0042  //!< timeout of the acknowledge
#define EVENT_DCL_FCTMOD_TEMP_REQ_OPMODE_INVALID_STATE     0x0043  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_REQ_OPMODE_INVALID_STATE    0x0044  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_SET_STATUS_FCTCALL_FAILED   0x0050  //!< set status of temperature ctrl. failed
#define EVENT_DCL_FCTMOD_TEMP_SET_STATUS_CANMSG_NOT_SEND   0x0051  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_TEMP_SET_STATUS_INVALID_STATE     0x0052  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_SET_STATUS_INVALID_STATE    0x0053  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_REQ_STATUS_FCTCALL_FAILED   0x0060  //!< request status of temperature ctrl. failed
#define EVENT_DCL_FCTMOD_TEMP_REQ_STATUS_CANMSG_NOT_SEND   0x0061  //!< CAN message was not send
#define EVENT_DCL_FCTMOD_TEMP_REQ_STATUS_CANMSG_TIMEOUT    0x0062  //!< timeout of the acknowledge
#define EVENT_DCL_FCTMOD_TEMP_REQ_STATUS_INVALID_STATE     0x0062  //!< invalid state
#define EVENT_DCL_IFCTMOD_TEMP_REQ_STATUS_INVALID_STATE    0x0063  //!< invalid state


//##############################################
//  Group EVENT_GRP_DCL_FCTMOD_ANA_INP_DCL
//  analog input
#define ERROR_DCL_FCTMOD_ANA_INP_CANMSG_ANA_INP_DLC          0x0001   //!< invalid data length code detected
#define ERROR_DCL_IFCTMOD_ANA_INP_REQ_ACTINP_FCTCALL_FAILED  0x0010   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_ANA_INP_REQ_ACTINP_FCTCALL_FAILED   0x0011   //!< function call from CANFctModule failed
#define ERROR_DCL_FCTMOD_ANA_INP_REQ_ACTINP_CANMSG_NOT_SEND  0x0012   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_ANA_INP_REQ_ACTINP_CANMSG_TIMEOUT   0x0013   //!< timeout while waiting for acknowledge

//##############################################
//  Group EVENT_GRP_DCL_FCTMOD_ANA_OUTP_DCL
//  analog output
#define ERROR_DCL_FCTMOD_ANA_OUTP_CANMSG_ANA_OUTP_DLC         0x0001   //!< invalid data length code detected
#define ERROR_DCL_IFCTMOD_ANA_OUTP_SET_OUTP_FCTCALL_FAILED    0x0010   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_ANA_OUTP_SET_OUTP_CANMSG_NOT_SEND    0x0011   //!< function call from CANFctModule failed
#define ERROR_DCL_FCTMOD_ANA_OUTP_SET_OUTP_CANMSG_INVALID     0x0012   //!< CAN-message could not be sent

#define ERROR_DCL_IFCTMOD_ANA_OUTP_REQ_OUTP_FCTCALL_FAILED    0x0020   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_ANA_OUTP_REQ_OUTP_CANMSG_NOT_SEND    0x0021   //!< function call from CANFctModule failed
#define ERROR_DCL_FCTMOD_ANA_OUTP_REQ_OUTP_CANMSG_INVALID     0x0022   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_ANA_OUTP_REQ_OUTP_TIMEOUT            0x0023   //!< timeout while waiting for acknowledge

#define ERROR_DCL_IFCTMOD_ANA_OUTP_REQ_LIVE_FCTCALL_FAILED    0x0020   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_ANA_OUTP_REQ_LIVE_CANMSG_NOT_SEND    0x0021   //!< function call from CANFctModule failed
#define ERROR_DCL_FCTMOD_ANA_OUTP_REQ_LIVE_CANMSG_INVALID     0x0022   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_ANA_OUTP_REQ_LIVE_TIMEOUT            0x0023   //!< timeout while waiting for acknowledge

//##############################################
//  Group EVENT_GRP_DCL_FCTMOD_DIG_INP_DCL
//  digital input
#define ERROR_DCL_FCTMOD_DIG_INP_CANMSG_INVALID_DLC          0x0001   //!< invalid data length code detected
#define ERROR_DCL_IFCTMOD_DIG_INP_REQ_ACTINP_FCTCALL_FAILED  0x0010   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_DIG_INP_REQ_ACTINP_CANMSG_NOT_SEND  0x0011   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_DIG_INP_REQ_ACTINP_CANMSG_TIMEOUT   0x0012   //!< timeout while waiting for acknowledge


//##############################################
//  Group EVENT_GRP_DCL_FCTMOD_DIG_OUTP_DCL
//  digital output
#define ERROR_DCL_FCTMOD_DIG_OUTP_CANMSG_INVALID_DLC         0x0001   //!< invalid data length code detected
#define ERROR_DCL_IFCTMOD_DIG_OUTP_SET_OUTP_FCTCALL_FAILED   0x0010   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_DIG_OUTP_SET_OUTP_CANMSG_NOT_SEND   0x0011   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_DIG_OUTP_SET_OUTP_CANMSG_TIMEOUT    0x0012   //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_DIG_OUTP_REQ_OUTP_FCTCALL_FAILED   0x0020   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_DIG_OUTP_REQ_OUTP_CANMSG_NOT_SEND   0x0021   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_DIG_OUTP_REQ_OUTP_CANMSG_TIMEOUT    0x0022   //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_DIG_OUTP_REQ_LIFE_FCTCALL_FAILED   0x0030   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_DIG_OUTP_REQ_LIFE_CANMSG_NOT_SEND   0x0031   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_DIG_OUTP_REQ_LIFE_CANMSG_TIMEOUT    0x0032   //!< timeout while waiting for acknowledge


//##############################################
//  Group EVENT_GRP_DCL_FCTMOD_RFID11785_DCL
// RFID ISO/IEC 11785
#define ERROR_DCL_FCTMOD_RFID11785_CANMSG_INVALID_DLC           0x0001  //!< invalid data length code detected
#define ERROR_DCL_IFCTMOD_RFID11785_SET_CONFIG_FCTCALL_FAILED   0x0010  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID11785_SET_CONFIG_CANMSG_NOT_SEND   0x0011  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID11785_SET_CONFIG_CANMSG_TIMEOUT    0x0012  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID11785_LOGIN_FCTCALL_FAILED        0x0020  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID11785_LOGIN_CANMSG_NOT_SEND        0x0021  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID11785_LOGIN_CANMSG_TIMEOUT         0x0022  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID11785_READ_DATA_FCTCALL_FAILED    0x0030  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID11785_READ_DATA_CANMSG_NOT_SEND    0x0031  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID11785_READ_DATA_CANMSG_TIMEOUT     0x0032  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID11785_READ_UID_FCTCALL_FAILED     0x0040  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID11785_READ_UID_CANMSG_NOT_SEND     0x0041  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID11785_READ_UID_CANMSG_TIMEOUT      0x0042  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID11785_WRITE_DATA_FCTCALL_FAILED   0x0050  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID11785_WRITE_DATA_CANMSG_NOT_SEND   0x0051  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID11785_WRITE_DATA_CANMSG_TIMEOUT    0x0052  //!< timeout while waiting for acknowledge

//##############################################
//  Group EVENT_GRP_DCL_FCTMOD_RFID15693_DCL
// RFID ISO/IEC 15693
#define ERROR_DCL_FCTMOD_RFID15693_CANMSG_INVALID_DLC           0x0001  //!< invalid data length code detected
#define ERROR_DCL_IFCTMOD_RFID15693_SET_CONFIG_FCTCALL_FAILED   0x0010  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID15693_SET_CONFIG_CANMSG_NOT_SEND   0x0011  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID15693_SET_CONFIG_CANMSG_TIMEOUT    0x0012  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID15693_ACQUIRE_UID_FCTCALL_FAILED  0x0020  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID15693_ACQUIRE_UID_CANMSG_NOT_SEND  0x0021  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID15693_ACQUIRE_UID_CANMSG_TIMEOUT   0x0022  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID15693_SET_UID_FCTCALL_FAILED      0x0030  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID15693_SET_UID_CANMSG_NOT_SEND      0x0031  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID15693_SET_UID_CANMSG_TIMEOUT       0x0032  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID15693_REQUEST_UID_FCTCALL_FAILED  0x0040  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID15693_REQUEST_UID_CANMSG_NOT_SEND  0x0041  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID15693_REQUEST_UID_CANMSG_TIMEOUT   0x0042  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID15693_READ_BLOCK_FCTCALL_FAILED   0x0050  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID15693_READ_BLOCK_CANMSG_NOT_SEND   0x0051  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID15693_READ_BLOCK_CANMSG_TIMEOUT    0x0052  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID15693_WRITE_BLOCK_FCTCALL_FAILED  0x0060  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID15693_WRITE_BLOCK_CANMSG_NOT_SEND  0x0061  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID15693_WRITE_BLOCK_CANMSG_TIMEOUT   0x0062  //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_RFID15693_LOCK_BLOCK_FCTCALL_FAILED   0x0070  //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_RFID15693_LOCK_BLOCK_CANMSG_NOT_SEND   0x0071  //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_RFID15693_LOCK_BLOCK_CANMSG_TIMEOUT    0x0072  //!< timeout while waiting for acknowledge

//##############################################
//Group EVENT_GRP_DCL_FCTMOD_JSTCK_DCL
// joystick
#define ERROR_DCL_FCTMOD_JSTCK_CANMSG_INVALID_DLC               0x0001   //!< invalid data length code detected
#define ERROR_DCL_IFCTMOD_JSTCK_REQ_INPUT_FCTCALL_FAILED        0x0010   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_JSTCK_REQ_INPUT_CANMSG_NOT_SEND        0x0011   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_JSTCK_REQ_INPUT_CANMSG_TIMEOUT         0x0012   //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_JSTCK_SET_CONTACT_LMT_FCTCALL_FAILED  0x0020   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_JSTCK_SET_CONTACT_LMT_CANMSG_NOT_SEND  0x0021   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_JSTCK_SET_CONTACT_LMT_CANMSG_TIMEOUT   0x0022   //!< timeout while waiting for acknowledge

//##############################################
//  Group EVENT_GRP_DCL_FCTMOD_UART_DCL
// UART
#define ERROR_DCL_FCTMOD_UART_CANMSG_INVALID_DLC             0x0001   //!< invalid data length code detected
#define ERROR_DCL_IFCTMOD_UART_SEND_DATA_FCTCALL_FAILED      0x0010   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_UART_SEND_DATA_CANMSG_NOT_SEND      0x0011   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_UART_SEND_DATA_ACKN_CANMSG_TIMEOUT  0x0012   //!< timeout while waiting for acknowledge
#define ERROR_DCL_IFCTMOD_UART_REQ_REC_DATA_FCTCALL_FAILED   0x0020   //!< function call from ICANFctModule failed
#define ERROR_DCL_FCTMOD_UART_REQ_REC_DATA_CANMSG_NOT_SEND   0x0021   //!< CAN-message could not be sent
#define ERROR_DCL_FCTMOD_UART_REQ_REC_DATA_CANMSG_TIMEOUT    0x0022   //!< timeout while waiting for acknowledge

//#############################################################
//      Devices
//#############################################################

//##############################################
//Group EVENT_GRP_DCL_AGITATION_DEV
#define ERROR_DCL_IAGITATION_DEV_INVALID_STATE             0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IAGITATION_DEV_NULL_PTR_ACCESS           0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_AGITATION_DEV_INVALID_TASKID             0x0010  //!< invalid task id
#define ERROR_DCL_AGITATION_DEV_INVALID_STATE              0x0011  //!< invalid state
#define ERROR_DCL_AGITATION_DEV_NULL_PTR_ACCESS            0x0012  //!< null pointer access
#define ERROR_DCL_AGITATION_DEV_INIT_FCT_ALLOC_FAILED      0x0013  //!< function module allocation failed
#define ERROR_DCL_AGITATION_DEV_CONFIG_CONNECT_FAILED      0x0014  //!< signal slot connect failed
#define ERROR_DCL_AGITATION_DEV_FCTMODULES_ERROR           0x0015  //!< function module error state reported
#define ERROR_DCL_IAGITATION_DEV_REFRUN_FCTCALL_FAILED     0x0110  //!< function call failed when calling reference run request (interface class)
#define ERROR_DCL_AGITATION_DEV_REFRUN_FCTCALL_FAILED      0x0111  //!< function call failed when calling reference run request
#define ERROR_DCL_AGITATION_DEV_REFRUN_TIMEOUT             0x0112  //!< timeout acknowledge signal
#define ERROR_DCL_IAGITATION_DEV_SET_SPEED_FCTCALL_FAILED  0x0120  //!< function call failed when setting speed (interface class)
#define ERROR_DCL_AGITATION_DEV_SET_SPEED_FCTCALL_FAILED   0x0121  //!< function call failed when setting speed
#define ERROR_DCL_AGITATION_DEV_SET_SPEED_TIMEOUT          0x0122  //!< timeout acknowledge signal
#define ERROR_DCL_IAGITATION_DEV_REQ_ACTPOS_FCTCALL_FAILED 0x0130  //!< function call failed when requesting actual position (interface class)
#define ERROR_DCL_AGITATION_DEV_REQ_ACTPOS_FCTCALL_FAILED  0x0131  //!< function call failed when requesting actual position
#define ERROR_DCL_AGITATION_DEV_REQ_ACTPOS_TIMEOUT         0x0132  //!< timeout acknowledge signal
#define ERROR_DCL_IAGITATION_DEV_REQ_ACTSPD_FCTCALL_FAILED 0x0140  //!< function call failed when requesting actual speed (interface class)
#define ERROR_DCL_AGITATION_DEV_REQ_ACTSPD_FCTCALL_FAILED  0x0141  //!< function call failed when requesting actual speed
#define ERROR_DCL_AGITATION_DEV_REQ_ACTSPD_TIMEOUT         0x0142  //!< timeout acknowledge signal

//##############################################
//Group EVENT_GRP_DCL_EXHAUST_DEV
#define ERROR_DCL_IEXHAUST_DEV_INVALID_STATE                     0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IEXHAUST_DEV_NULL_PTR_ACCESS                   0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_EXHAUST_DEV_INVALID_TASKID                     0x0010  //!< invalid task id
#define ERROR_DCL_EXHAUST_DEV_INVALID_STATE                      0x0011  //!< invalid state
#define ERROR_DCL_EXHAUST_DEV_NULL_PTR_ACCESS                    0x0012  //!< null pointer access
#define ERROR_DCL_EXHAUST_DEV_INIT_FCT_ALLOC_FAILED              0x0013  //!< function module allocation failed
#define ERROR_DCL_EXHAUST_DEV_CONFIG_CONNECT_FAILED              0x0014  //!< signal slot connect failed
#define ERROR_DCL_EXHAUST_DEV_FCTMODULES_ERROR                   0x0015  //!< function module error state reported
#define ERROR_DCL_IEXHAUST_DEV_REQ_SENSOR_STATUS_FCTCALL_FAILED  0x0101  //!< function call failed when requesting actual flow sensor status (interface class)
#define ERROR_DCL_EXHAUST_DEV_REQ_SENSOR_STATUS_FCTCALL_FAILED   0x0101  //!< function call failed when requesting actual flow sensor status
#define ERROR_DCL_EXHAUST_DEV_REQ_SENSOR_STATUS_TIMEOUT          0x0102 //!< timeout acknowledge signal

//##############################################
//Group EVENT_GRP_DCL_INCL_DEV
#define ERROR_DCL_IINCL_DEV_INVALID_STATE                0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IINCL_DEV_NULL_PTR_ACCESS              0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_INCL_DEV_INVALID_TASKID                0x0010  //!< invalid task id
#define ERROR_DCL_INCL_DEV_INVALID_STATE                 0x0011  //!< invalid state
#define ERROR_DCL_INCL_DEV_NULL_PTR_ACCESS               0x0012  //!< null pointer access
#define ERROR_DCL_INCL_DEV_INIT_FCT_ALLOC_FAILED         0x0013  //!< function module allocation failed
#define ERROR_DCL_INCL_DEV_CONFIG_CONNECT_FAILED         0x0014  //!< signal slot connect failed
#define ERROR_DCL_INCL_DEV_FCTMODULES_ERROR              0x0015  //!< function module error state reported
#define ERROR_DCL_IINCL_DEV_REQ_ACTINCL_FCTCALL_FAILED   0x0110  //!< function call failed when calling actual inclination request (interface class)
#define ERROR_DCL_INCL_DEV_REQ_ACTINCL_FCTCALL_FAILED    0x0111  //!< function call failed when calling actual inclination request
#define ERROR_DCL_INCL_DEV_REQ_ACTINCL_TIMEOUT           0x0112  //!< timeout acknowledge signal
#define ERROR_DCL_IINCL_DEV_SET_INCLLIMIT_FCTCALL_FAILED 0x0120  //!< function call failed when setting inclination limit (interface class)
#define ERROR_DCL_INCL_DEV_SET_INCLLIMIT_FCTCALL_FAILED  0x0121  //!< function call failed when setting inclination limit (interface class)
#define ERROR_DCL_IINCL_DEV_SET_REFPOS_FCTCALL_FAILED    0x0130  //!< function call failed when setting reference position (interface class)
#define ERROR_DCL_INCL_DEV_SET_REFPOS_FCTCALL_FAILED     0x0131  //!< function call failed when setting reference position


//##############################################
//Group EVENT_GRP_DCL_LOADER_DEV
#define ERROR_DCL_ILOADER_DEV_INVALID_STATE                  0x0001  //!< invalid state (interface class)
#define ERROR_DCL_ILOADER_DEV_NULL_PTR_ACCESS                0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_LOADER_DEV_INVALID_TASKID                  0x0010  //!< invalid task id
#define ERROR_DCL_LOADER_DEV_INVALID_STATE                   0x0011  //!< invalid state
#define ERROR_DCL_LOADER_DEV_NULL_PTR_ACCESS                 0x0012  //!< null pointer access
#define ERROR_DCL_LOADER_DEV_INIT_FCT_ALLOC_FAILED           0x0013  //!< function module allocation failed
#define ERROR_DCL_LOADER_DEV_CONFIG_CONNECT_FAILED           0x0014  //!< signal slot connect failed
#define ERROR_DCL_LOADER_DEV_FCTMODULES_ERROR                0x0015  //!< function module error state reported
#define ERROR_DCL_LOADER_DEV_UNKNOWN_ERROR                   0x0016  //!< error, but no error information available
#define ERROR_DCL_ILOADER_DEV_REQ_REFRUN_FCTCALL_FAILED      0x0110  //!< function call failed when calling reference run request (interface class)
#define ERROR_DCL_LOADER_DEV_REQ_REFRUN_FCTCALL_FAILED       0x0111  //!< function call failed when calling reference run request
#define ERROR_DCL_LOADER_DEV_REQ_REFRUN_TIMEOUT              0x0112  //!< timeout acknowledge signal
#define ERROR_DCL_ILOADER_DEV_SET_DRAWER_POS_FCTCALL_FAILED  0x0120  //!< function call failed when calling movement request (interface class)
#define ERROR_DCL_LOADER_DEV_SET_DRAWER_POS_FCTCALL_FAILED   0x0121  //!< function call failed when calling movement request
#define ERROR_DCL_LOADER_DEV_SET_DRAWER_POS_TIMEOUT          0x0122  //!< timeout acknowledge signal
#define ERROR_DCL_ILOADER_DEV_REQ_DRAWER_POS_FCTCALL_FAILED  0x0130  //!< function call failed when calling actual position request (interface class)
#define ERROR_DCL_LOADER_DEV_REQ_DRAWER_POS_FCTCALL_FAILED   0x0131  //!< function call failed when calling actual position request
#define ERROR_DCL_LOADER_DEV_REQ_DRAWER_POS_TIMEOUT          0x0132  //!< timeout acknowledge signal
#define ERROR_DCL_ILOADER_DEV_REQ_RFID_DATA_FCTCALL_FAILED   0x0140  //!< function call failed when calling rfid data request (interface class)
#define ERROR_DCL_LOADER_DEV_REQ_RFID_DATA_FCTCALL_FAILED    0x0141  //!< function call failed when calling rfid data request
#define ERROR_DCL_LOADER_DEV_REQ_RFID_DATA_TIMEOUT           0x0142  //!< timeout acknowledge signal
#define ERROR_DCL_ILOADER_DEV_SET_BLK_STATE_FCTCALL_FAILED   0x0150  //!< function call failed when setting the blocking state (interface class)
#define ERROR_DCL_LOADER_DEV_SET_BLK_STATE_FCTCALL_FAILED    0x0151  //!< function call failed when setting the blocking state

//##############################################
//Group EVENT_GRP_DCL_RACKTRANS_DEV
#define ERROR_DCL_IRACKTFR_DEV_INVALID_STATE                  0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IRACKTFR_DEV_NULL_PTR_ACCESS                0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_RACKTFR_DEV_INVALID_TASKID                  0x0010  //!< invalid task id
#define ERROR_DCL_RACKTFR_DEV_INVALID_STATE                   0x0011  //!< invalid state
#define ERROR_DCL_RACKTFR_DEV_NULL_PTR_ACCESS                 0x0012  //!< null pointer access
#define EVENT_DCL_RACKTFR_DEV_INIT_FCT_ALLOC_FAILED           0x0013  //!< function module allocation failed
#define EVENT_DCL_RACKTFR_DEV_CONFIG_CONNECT_FAILED           0x0014  //!< signal slot connect failed
#define EVENT_DCL_RACKTFR_DEV_FCTMODULES_ERROR                0x0015  //!< function module error state reported
#define ERROR_DCL_IRACKTFR_DEV_REQ_REFRUN_FCTCALL_FAILED      0x0110  //!< function call failed when calling reference run request (interface class)
#define ERROR_DCL_RACKTFR_DEV_REQ_REFRUN_FCTCALL_FAILED       0x0111  //!< function call failed when calling reference run request
#define ERROR_DCL_RACKTFR_DEV_REQ_REFRUN_TIMEOUT              0x0112  //!< timeout acknowledge signal
#define ERROR_DCL_IRACKTFR_DEV_SET_ADAPTER_POS_FCTCALL_FAILED 0x0120  //!< function call failed when calling movement request (interface class)
#define ERROR_DCL_RACKTFR_DEV_SET_ADAPTER_POS_FCTCALL_FAILED  0x0121  //!< function call failed when calling movement request
#define ERROR_DCL_RACKTFR_DEV_SET_ADAPTER_POS_TIMEOUT         0x0122  //!< timeout acknowledge signal
#define ERROR_DCL_IRACKTFR_DEV_REQ_ACTPOS_FCTCALL_FAILED      0x0130  //!< function call failed when calling actual position request (interface class)
#define ERROR_DCL_RACKTFR_DEV_REQ_ACTPOS_FCTCALL_FAILED       0x0131  //!< function call failed when calling actual position request
#define ERROR_DCL_RACKTFR_DEV_REQ_ACTPOS_TIMEOUT              0x0132  //!< timeout acknowledge signal

//##############################################
//Group GRAPPLER
//  EVENT_GRP_DCL_GRAPPLER_DEV        grappler device'
#define ERROR_DCL_IGRAPPLER_DEV_INVALID_STATE              0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IGRAPPLER_DEV_NULL_PTR_ACCESS            0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_GRAPPLER_DEV_INVALID_TASKID              0x0010  //!< invalid task id
#define ERROR_DCL_GRAPPLER_DEV_INVALID_STATE               0x0011  //!< invalid state
#define ERROR_DCL_GRAPPLER_DEV_NULL_PTR_ACCESS             0x0012  //!< null pointer access
#define ERROR_DCL_GRAPPLER_DEV_INIT_FCT_ALLOC_FAILED       0x0013  //!< function module allocation failed
#define ERROR_DCL_GRAPPLER_DEV_CONFIG_CONNECT_FAILED       0x0014  //!< signal slot connect failed
#define ERROR_DCL_GRAPPLER_DEV_FCTMODULES_ERROR            0x0015  //!< function module error state reported
#define EVENT_DCL_IGRAPPLER_DEV_REQREFRUN_FCT_CALL_FAILED  0x0110  //!< function call failed when calling reference run request (interface class)
#define EVENT_DCL_GRAPPLER_DEV_REQREFRUN_FCT_CALL_FAILED   0x0111  //!< function call failed when calling reference run request
#define EVENT_DCL_IGRAPPLER_DEV_REQ_ACTION_FCT_CALL_FAILED 0x0120  //!< error while requesting positioning (interface class)
#define EVENT_DCL_GRAPPLER_DEV_REQACTPOS_FCT_CALL_FAILED   0x0202  //!< error while requesting actual motor positions
#define EVENT_DCL_GRAPPLER_DEV_CALCSTEPS_FCT_CALL_FAILED   0x0203  //!< error while calculating grappler steps
#define EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_READ_FAILED      0x1001  //!< the file could not be read
#define EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_PARSE_ERROR      0x1002  //!< xml parser error
#define EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_DOCTYPE_ERROR    0x1003  //!< illegal unexpected
#define EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_VERSION_ERROR    0x1004  //!< version unexpected
#define EVENT_DCL_GRAPPLER_DEV_ADJUSTMENT_FORMAT_ERROR     0x1005  //!< format not correct
#define EVENT_DCL_GRAPPLER_DEV_LAYOUT_READ_FAILED          0x1011  //!< the file could not be read
#define EVENT_DCL_GRAPPLER_DEV_LAYOUT_PARSE_ERROR          0x1012  //!< xml parser error
#define EVENT_DCL_GRAPPLER_DEV_LAYOUT_DOCTYPE_ERROR        0x1013  //!< illegal unexpected
#define EVENT_DCL_GRAPPLER_DEV_LAYOUT_VERSION_ERROR        0x1014  //!< version unexpected
#define EVENT_DCL_GRAPPLER_DEV_LAYOUT_FORMAT_ERROR         0x1015  //!< format not correct

//##############################################
//Group  EVENT_GRP_DCL_HVESSELS_DEV
//       'Heated vessels'
#define ERROR_DCL_IHVESSELS_DEV_INVALID_STATE               0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IHVESSELS_DEV_NULL_PTR_ACCESS             0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_HVESSELS_DEV_INVALID_TASKID               0x0010  //!< invalid task id
#define ERROR_DCL_HVESSELS_DEV_INVALID_STATE                0x0011  //!< invalid state
#define ERROR_DCL_HVESSELS_DEV_NULL_PTR_ACCESS              0x0012  //!< null pointer access
#define ERROR_DCL_HVESSELS_DEV_INIT_FCT_ALLOC_FAILED        0x0013  //!< function module allocation failed
#define ERROR_DCL_HVESSELS_DEV_CONFIG_CONNECT_FAILED        0x0014  //!< signal slot connect failed
#define ERROR_DCL_HVESSELS_DEV_FCTMODULES_ERROR             0x0015  //!< function module error state reported
#define EVENT_DCL_IHVESSELS_DEV_SET_STATUS_FCT_CALL_FAILED  0x0110  //!< function call failed when requesting actual status of temperature control (interface class)
#define EVENT_DCL_HVESSELS_DEV_SET_STATUS_FCT_CALL_FAILED   0x0111  //!< function call failed when requesting actual status of temperature control (interface class)
#define EVENT_DCL_IHVESSELS_DEV_SET_TEMP_FCT_CALL_FAILED    0x0120  //!< function call failed when setting temperature (interface class)
#define EVENT_DCL_HVESSELS_DEV_SET_TEMP_FCT_CALL_FAILED     0x0121  //!< function call failed when setting temperature
#define EVENT_DCL_IHVESSELS_DEV_REQ_STATUS_FCT_CALL_FAILED  0x0130  //!< function call failed when requesting actual status of temperature control (interface class)
#define EVENT_DCL_HVESSELS_DEV_REQ_STATUS_FCT_CALL_FAILED   0x0131  //!< function call failed when requesting actual status of temperature control
#define EVENT_DCL_HVESSELS_DEV_REQ_STATUS_TIMEOUT           0x0132  //!< timeout acknowledge signal
#define EVENT_DCL_IHVESSELS_DEV_REQ_ACTTEMP_FCT_CALL_FAILED 0x0140  //!< function call failed when calling actual temperature request (interface class)
#define EVENT_DCL_HVESSELS_DEV_REQ_ACTTEMP_FCT_CALL_FAILED  0x0141  //!< function call failed when calling actual temperature request
#define EVENT_DCL_HVESSELS_DEV_REQ_ACTTEMP_TIMEOUT          0x0142  //!< timeout acknowledge signal
#define EVENT_DCL_IHVESSELS_DEV_SET_OPMODE_FCT_CALL_FAILED  0x0150  //!< function call failed when set operation mode of temp. control (interface class)
#define EVENT_DCL_HVESSELS_DEV_SET_OPMODE_FCT_CALL_FAILED   0x0151  //!< function call failed when set operation mode of temp. control
#define EVENT_DCL_IHVESSELS_DEV_REQ_OPMODE_FCT_CALL_FAILED  0x0160  //!< function call failed when requesting actual operation mode of temp. control (interface class)
#define EVENT_DCL_HVESSELS_DEV_REQ_OPMODE_FCT_CALL_FAILED   0x0161  //!< function call failed when requesting actual operation mode of temp. control
#define EVENT_DCL_HVESSELS_DEV_REQ_OPMODE_TIMEOUT           0x0162  //!< timeout acknowledge signal

//##############################################
//Group EVENT_GRP_DCL_WATER_DEV
#define ERROR_DCL_IWATER_DEV_INVALID_STATE                    0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IWATER_DEV_NULL_PTR_ACCESS                  0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_WATER_DEV_INVALID_TASKID                    0x0010  //!< invalid task id
#define ERROR_DCL_WATER_DEV_INVALID_STATE                     0x0011  //!< invalid state
#define ERROR_DCL_WATER_DEV_NULL_PTR_ACCESS                   0x0012  //!< null pointer access
#define ERROR_DCL_WATER_DEV_INIT_FCT_ALLOC_FAILED             0x0013  //!< function module allocation failed
#define ERROR_DCL_WATER_DEV_CONFIG_CONNECT_FAILED             0x0014  //!< signal slot connect failed
#define ERROR_DCL_WATER_DEV_FCTMODULES_ERROR                  0x0015  //!< function module error state reported
#define EVENT_DCL_IWATER_DEV_REQ_LIQUID_LEVEL_FCT_CALL_FAILED 0x0100  //!< request liqid level function call failed (interface class)
#define EVENT_DCL_WATER_DEV_REQ_LIQUID_LEVEL_FCT_CALL_FAILED  0x0101  //!< request liqid level function call failed
#define EVENT_DCL_WATER_DEV_REQ_LIQUID_LEVEL_TIMEOUT          0x0102  //!< request liqid level timeout
#define EVENT_DCL_IWATER_DEV_SET_VALVE_FCT_CALL_FAILED        0x0110  //!< set valve state function call failed (interface class)
#define EVENT_DCL_WATER_DEV_SET_VALVE_FCT_CALL_FAILED         0x0111  //!< set valve state function call failed
#define EVENT_DCL_WATER_DEV_SET_VALVE_STATE_TIMEOUT           0x0112  //!< set valve state timeout
#define EVENT_DCL_IWATER_DEV_REQ_VALVE_STATE_FCT_CALL_FAILED  0x0120  //!< request valve state function call failed (interface class)
#define EVENT_DCL_WATER_DEV_REQ_VALVE_STATE_FCT_CALL_FAILED   0x0121  //!< request valve state function call failed
#define EVENT_DCL_WATER_DEV_REQ_VALVE_STATE_TIMEOUT           0x0122  //!< request valve state timeout

//##############################################
//Group 'Oven' EVENT_GRP_DCL_OVEN_DEV
#define ERROR_DCL_IOVEN_DEV_INVALID_STATE                    0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IOVEN_DEV_NULL_PTR_ACCESS                  0x0002  //!< Null pointer access
#define ERROR_DCL_OVEN_DEV_INVALID_TASKID                    0x0010  //!< invalid task id
#define ERROR_DCL_OVEN_DEV_INVALID_STATE                     0x0011  //!< invalid state
#define ERROR_DCL_OVEN_DEV_NULL_PTR_ACCESS                   0x0012  //!< null pointer access
#define ERROR_DCL_OVEN_DEV_INIT_FCT_ALLOC_FAILED             0x0013  //!< function module allocation failed
#define ERROR_DCL_OVEN_DEV_CONFIG_CONNECT_FAILED             0x0014  //!< signal slot connect failed
#define ERROR_DCL_OVEN_DEV_FCTMODULES_ERROR                  0x0015  //!< function module error state reported
#define ERROR_DCL_IOVEN_DEV_COVER_REFRUN_FCTCALL_FAILED      0x0110  //!< function call failed when calling reference run request (interface class)
#define ERROR_DCL_OVEN_DEV_COVER_REFRUN_FCTCALL_FAILED       0x0111  //!< function call failed when calling reference run request
#define ERROR_DCL_OVEN_DEV_COVER_REFRUN_TIMEOUT              0x0112  //!< timeout acknowledge signal
#define ERROR_DCL_IOVEN_DEV_REQ_COVER_MOVE_FCTCALL_FAILED    0x0120  //!< function call failed when calling cover movement (interface class)
#define ERROR_DCL_OVEN_DEV_REQ_COVER_MOVE_FCTCALL_FAILED     0x0121  //!< function call failed when calling cover movement
#define ERROR_DCL_OVEN_DEV_REQ_COVER_MOVE_TIMEOUT            0x0122  //!< timeout acknowledge signal
#define ERROR_DCL_IOVEN_DEV_REQ_COVER_ACTPOS_FCTCALL_FAILED  0x0130  //!< function call failed when calling actual position request (interface class)
#define ERROR_DCL_OVEN_DEV_REQ_COVER_ACTPOS_FCTCALL_FAILED   0x0131  //!< function call failed when calling actual position request
#define ERROR_DCL_OVEN_DEV_REQ_COVER_ACTPOS_TIMEOUT          0x0132  //!< timeout acknowledge signal
#define ERROR_DCL_IOVEN_DEV_REQ_ACTTEMP_FCTCALL_FAILED       0x0140  //!< function call failed when calling actual temperature request (interface class)
#define ERROR_DCL_OVEN_DEV_REQ_ACTTEMP_FCTCALL_FAILED        0x0141  //!< function call failed when calling actual temperature request
#define ERROR_DCL_OVEN_DEV_REQ_ACTTEMP_TIMEOUT               0x0142  //!< timeout acknowledge signal
#define ERROR_DCL_IOVEN_DEV_SET_TEMP_FCTCALL_FAILED          0x0150  //!< function call failed when setting temperature (interface class)
#define ERROR_DCL_OVEN_DEV_SET_TEMP_FCTCALL_FAILED           0x0151  //!< function call failed when setting temperature
#define ERROR_DCL_OVEN_DEV_SET_TEMP_TIMEOUT                  0x0152  //!< timeout acknowledge signal
#define ERROR_DCL_IOVEN_DEV_SET_STATUS_FCTCALL_FAILED        0x0160  //!< function call failed when setting status of temperature control (interface class)
#define ERROR_DCL_OVEN_DEV_SET_STATUS_FCTCALL_FAILED         0x0161  //!< function call failed when setting status of temperature control
#define ERROR_DCL_OVEN_DEV_SET_STATUS_TIMEOUT                0x0162  //!< timeout acknowledge signal
#define ERROR_DCL_IOVEN_DEV_REQ_STATUS_FCTCALL_FAILED        0x0170  //!< function call failed when requesting actual status of temperature control (interface class)
#define ERROR_DCL_OVEN_DEV_REQ_STATUS_FCTCALL_FAILED         0x0171  //!< function call failed when requesting actual status of temperature control
#define ERROR_DCL_OVEN_DEV_REQ_STATUS_TIMEOUT                0x0172  //!< timeout acknowledge signal

//##############################################
//Group COVERLINE
//  EVENT_GRP_DCL_COVERLINE_DEV        cover line device'
#define ERROR_DCL_ICOVERLINE_DEV_INVALID_STATE              0x0001  //!< invalid state (interface class)
#define ERROR_DCL_ICOVERLINE_DEV_NULL_PTR_ACCESS            0x0002  //!< null pointer access (interface class)
#define ERROR_DCL_COVERLINE_DEV_INVALID_TASKID              0x0010  //!< invalid task id
#define ERROR_DCL_COVERLINE_DEV_INVALID_STATE               0x0011  //!< invalid state
#define ERROR_DCL_COVERLINE_DEV_NULL_PTR_ACCESS             0x0012  //!< null pointer access
#define ERROR_DCL_COVERLINE_DEV_INIT_FCT_ALLOC_FAILED       0x0013  //!< function module allocation failed
#define ERROR_DCL_COVERLINE_DEV_CONFIG_CONNECT_FAILED       0x0014  //!< signal slot connect failed
#define ERROR_DCL_COVERLINE_DEV_FCTMODULES_ERROR            0x0015  //!< function module error state reported
#define EVENT_DCL_ICOVERLINE_DEV_REQREFRUN_FCT_CALL_FAILED  0x0110  //!< function call failed when calling reference run request  (interface class)
#define EVENT_DCL_COVERLINE_DEV_REQREFRUN_FCT_CALL_FAILED   0x0111  //!< function call failed when calling reference run request
#define EVENT_DCL_ICOVERLINE_DEV_REQCOVER_FCT_CALL_FAILED   0x0120  //!< function call failed when calling cover process  (interface class)
#define EVENT_DCL_COVERLINE_DEV_REQCOVER_FCT_CALL_FAILED    0x0121  //!< function call failed when calling cover process

//##############################################
//Group RackHandling
//  EVENT_GRP_DCL_RACKHDL_DEV        RackHandling device
#define ERROR_DCL_IRACKHDL_DEV_INVALID_STATE                    0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IRACKHDL_DEV_NULL_PTR_ACCESS                  0x0002  //!< null pointer access (interface class)
#define EVENT_DCL_RACKHDL_DEV_ADJUSTMENT_FORMAT_ERROR           0x0003  //!< format error at ajustment file (xml)
#define ERROR_DCL_RACKHDL_DEV_INVALID_TASKID                    0x0010  //!< invalid task id
#define ERROR_DCL_RACKHDL_DEV_INVALID_STATE                     0x0011  //!< invalid state
#define ERROR_DCL_RACKHDL_DEV_NULL_PTR_ACCESS                   0x0012  //!< null pointer access
#define ERROR_DCL_RACKHDL_DEV_INIT_FCT_ALLOC_FAILED             0x0013  //!< function module allocation failed
#define ERROR_DCL_RACKHDL_DEV_CONFIG_CONNECT_FAILED             0x0014  //!< signal slot connect failed
#define ERROR_DCL_RACKHDL_DEV_FCTMODULES_ERROR                  0x0015  //!< function module error state reported
#define EVENT_DCL_IRACKHDL_DEV_REQREFRUN_FCT_CALL_FAILED        0x0110  //!< function call failed when calling reference run request (interface class)
#define EVENT_DCL_RACKHDL_DEV_REQREFRUN_FCT_CALL_FAILED         0x0111  //!< function call failed when calling reference run request
#define EVENT_DCL_IRACKHDL_DEV_REQACTION_FCT_CALL_FAILED        0x0120  //!< function call failed when calling action request (interface class)
#define EVENT_DCL_RACKHDL_DEV_REQACTPOS_FCT_CALL_FAILED         0x0121  //!< function call failed when calling position request
#define EVENT_DCL_RACKHDL_DEV_CALC_TRANSP_FCT_CALL_FAILED       0x0122  //!< function call failed when calling rack transport request
#define EVENT_DCL_RACKHDL_DEV_TRANSP_TASK_START_FCT_CALL_FAILED 0x0123  //!< function call failed when calling rack transport request within the process

//##############################################
//Group Hood Device
//  EVENT_GRP_DCL_HOOD_DEV        Hood open detection
#define ERROR_DCL_IHOOD_DEV_INVALID_STATE                    0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IHOOD_DEV_NULL_PTR_ACCESS                  0x0002  //!< null pointer access (interface class)
#define EVENT_DCL_HOOD_DEV_ADJUSTMENT_FORMAT_ERROR           0x0003  //!< format error at ajustment file (xml)
#define ERROR_DCL_HOOD_DEV_INVALID_TASKID                    0x0010  //!< invalid task id
#define ERROR_DCL_HOOD_DEV_INVALID_STATE                     0x0011  //!< invalid state
#define ERROR_DCL_HOOD_DEV_NULL_PTR_ACCESS                   0x0012  //!< null pointer access
#define ERROR_DCL_HOOD_DEV_INIT_FCT_ALLOC_FAILED             0x0013  //!< function module allocation failed
#define ERROR_DCL_HOOD_DEV_CONFIG_CONNECT_FAILED             0x0014  //!< signal slot connect failed
#define ERROR_DCL_HOOD_DEV_FCTMODULES_ERROR                  0x0015  //!< function module error state reported
#define ERROR_DCL_HOOD_DEV_UNKNOWN_ERROR                     0x0016  //!< error, but no error information available
#define ERROR_DCL_IHOOD_DEV_REQ_STATUS_FCTCALL_FAILED        0x0110  //!< function call failed when calling get status (interface class)
#define ERROR_DCL_HOOD_DEV_REQ_STATUS_FCTCALL_FAILED         0x0111  //!< function call failed when calling get status
#define ERROR_DCL_HOOD_DEV_REQ_STATUS_TIMEOUT                0x0112  //!< timeout acknowledge signal

//##############################################
//Group 'Rotary Valve' EVENT_GRP_DCL_RV_DEV
#define ERROR_DCL_IRV_DEV_INVALID_STATE                    0x0001  //!< invalid state (interface class)
#define ERROR_DCL_IRV_DEV_NULL_PTR_ACCESS                  0x0002  //!< Null pointer access
#define ERROR_DCL_RV_DEV_INVALID_TASKID                    0x0010  //!< invalid task id
#define ERROR_DCL_RV_DEV_INVALID_STATE                     0x0011  //!< invalid state
#define ERROR_DCL_RV_DEV_NULL_PTR_ACCESS                   0x0012  //!< null pointer access
#define ERROR_DCL_RV_DEV_INIT_FCT_ALLOC_FAILED             0x0013  //!< function module allocation failed
#define ERROR_DCL_RV_DEV_CONFIG_CONNECT_FAILED             0x0014  //!< signal slot connect failed
#define ERROR_DCL_RV_DEV_FCTMODULES_ERROR                  0x0015  //!< function module error state reported
#define ERROR_DCL_IRV_DEV_COVER_REFRUN_FCTCALL_FAILED      0x0110  //!< function call failed when calling reference run request (interface class)
#define ERROR_DCL_RV_DEV_COVER_REFRUN_FCTCALL_FAILED       0x0111  //!< function call failed when calling reference run request
#define ERROR_DCL_RV_DEV_COVER_REFRUN_TIMEOUT              0x0112  //!< timeout acknowledge signal
#define ERROR_DCL_IRV_DEV_REQ_COVER_MOVE_FCTCALL_FAILED    0x0120  //!< function call failed when calling cover movement (interface class)
#define ERROR_DCL_RV_DEV_REQ_COVER_MOVE_FCTCALL_FAILED     0x0121  //!< function call failed when calling cover movement
#define ERROR_DCL_RV_DEV_REQ_COVER_MOVE_TIMEOUT            0x0122  //!< timeout acknowledge signal
#define ERROR_DCL_IRV_DEV_REQ_COVER_ACTPOS_FCTCALL_FAILED  0x0130  //!< function call failed when calling actual position request (interface class)
#define ERROR_DCL_RV_DEV_REQ_COVER_ACTPOS_FCTCALL_FAILED   0x0131  //!< function call failed when calling actual position request
#define ERROR_DCL_RV_DEV_REQ_COVER_ACTPOS_TIMEOUT          0x0132  //!< timeout acknowledge signal
#define ERROR_DCL_IRV_DEV_REQ_ACTTEMP_FCTCALL_FAILED       0x0140  //!< function call failed when calling actual temperature request (interface class)
#define ERROR_DCL_RV_DEV_REQ_ACTTEMP_FCTCALL_FAILED        0x0141  //!< function call failed when calling actual temperature request
#define ERROR_DCL_RV_DEV_REQ_ACTTEMP_TIMEOUT               0x0142  //!< timeout acknowledge signal
#define ERROR_DCL_IRV_DEV_SET_TEMP_FCTCALL_FAILED          0x0150  //!< function call failed when setting temperature (interface class)
#define ERROR_DCL_RV_DEV_SET_TEMP_FCTCALL_FAILED           0x0151  //!< function call failed when setting temperature
#define ERROR_DCL_RV_DEV_SET_TEMP_TIMEOUT                  0x0152  //!< timeout acknowledge signal
#define ERROR_DCL_IRV_DEV_SET_STATUS_FCTCALL_FAILED        0x0160  //!< function call failed when setting status of temperature control (interface class)
#define ERROR_DCL_RV_DEV_SET_STATUS_FCTCALL_FAILED         0x0161  //!< function call failed when setting status of temperature control
#define ERROR_DCL_RV_DEV_SET_STATUS_TIMEOUT                0x0162  //!< timeout acknowledge signal
#define ERROR_DCL_IRV_DEV_REQ_STATUS_FCTCALL_FAILED        0x0170  //!< function call failed when requesting actual status of temperature control (interface class)
#define ERROR_DCL_RV_DEV_REQ_STATUS_FCTCALL_FAILED         0x0171  //!< function call failed when requesting actual status of temperature control
#define ERROR_DCL_RV_DEV_REQ_STATUS_TIMEOUT                0x0172  //!< timeout acknowledge signal

#endif /* DEVICE_CONTROL_ERROR_H */

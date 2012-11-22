/****************************************************************************/
/*! \file bmCan.h
 *
 *  \brief CAN message read/write functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to read/write CAN-Messages.
 *       It uses the hardware abstraction layer to access the CAN
 *       controller. Messages read or written are stored in queues.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef BM_CAN_H
#define BM_CAN_H

#include "halLayer.h"

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! CAN message command classes
typedef enum {
    CMD_CLASS_SYSTEM   = 0,     //!< System commands
    CMD_CLASS_TRIGGER  = 1,     //!< Trigger commands
    CMD_CLASS_ERROR    = 2,     //!< Error/warning commands
    CMD_CLASS_UNUSED   = 3,     //!< Not used
    CMD_CLASS_FUNCTION = 4,     //!< Function commands
    CMD_CLASS_PROCESS  = 5,     //!< Process commands/data
    CMD_CLASS_SERVICE  = 6,     //!< Service commands
    CMD_CLASS_ASSEMBLY = 7      //!< Assembly commands
} CanCmdClass_t;

//@{**************************************************************************/
//! CAN-Identifier Subfield Declaration
//****************************************************************************/

// Maximum values to be codeable in subfields of CAN ID
#define CANiD_MAX_NODETYPE      127          //!< Max. node type value
#define CANiD_MAX_NODEINDEX     15           //!< Max. node index value
#define CANiD_MAX_CHANNEL       127          //!< Max. channel value
#define CANiD_MAX_CMDCODE       127          //!< Max. command value
#define CANiD_MAX_CMDCLASS      7            //!< Max. command class value

// Bitshift counts for subfields in CAN ID
#define CANiD_SHIFT_MASTER      0            //!< Master/slave bitmask
#define CANiD_SHIFT_NODETYPE    1            //!< Node type bitmask
#define CANiD_SHIFT_NODEINDEX   8            //!< Node index bitmask
#define CANiD_SHIFT_CHANNEL     12           //!< Channel bitshift count
#define CANiD_SHIFT_CMDCODE     19           //!< Command code bitmask
#define CANiD_SHIFT_CMDCLASS    26           //!< Command class bitmask
#define CANiD_SHIFT_BROADCAST   29           //!< Broadcast bit

// Bitmasks to isolate information fields in CAN ID
#define CANiD_MASK_CMDCLASS     0x1C000000   //!< Command class bitmask
#define CANiD_MASK_CMDCODE      0x03F80000   //!< Command code bitmask
#define CANiD_MASK_CHANNEL      0x0007F000   //!< Channel bitmask
#define CANiD_MASK_NODEINDEX    0x00000F00   //!< Node index bitmask
#define CANiD_MASK_NODETYPE     0x000000FE   //!< Node type bitmask
#define CANiD_MASK_MASTER       0x00000001   //!< Master/slave bitmask
#define CANiD_MASK_BROADCAST    0x80000000   //!< Broadcast bit

//! Bitmasks for combined fields in CAN ID
#define CANiD_MASK_ADDRESS      (CANiD_MASK_NODETYPE | CANiD_MASK_NODEINDEX)

//! Bitmasks for combined fields in CAN ID 
#define CANiD_MASK_COMMAND      (CANiD_MASK_CMDCLASS | CANiD_MASK_CMDCODE)

//@} End of doxygen group

//@{**************************************************************************/
//! CAN Identifiers used by Basemodule
//****************************************************************************/

#define BUILD_CAN_ID(Class,Code,MS) \
                (((UInt32)(Class) << 26) | ((UInt32)(Code) << 19) | (MS))

// CAN-IDs for System Messages
#define MSG_SYS_HARDWARE_ID         BUILD_CAN_ID(CMD_CLASS_SYSTEM,  0, 0)
#define MSG_SYS_ACK_HARDWARE_ID     BUILD_CAN_ID(CMD_CLASS_SYSTEM,  0, 1)
#define MSG_SYS_CONFIGURATION       BUILD_CAN_ID(CMD_CLASS_SYSTEM,  1, 0)
#define MSG_SYS_REQ_CONFIGURATION   BUILD_CAN_ID(CMD_CLASS_SYSTEM,  1, 1)

#define MSG_SYS_SET_NODESTATE       BUILD_CAN_ID(CMD_CLASS_SYSTEM,  2, 1)
#define MSG_SYS_REQ_NODESTATE       BUILD_CAN_ID(CMD_CLASS_SYSTEM,  3, 1)
#define MSG_SYS_NODESTATE           BUILD_CAN_ID(CMD_CLASS_SYSTEM,  3, 0)
#define MSG_SYS_SET_REALTIME        BUILD_CAN_ID(CMD_CLASS_SYSTEM,  4, 1)
#define MSG_SYS_SET_SYSCLOCK        BUILD_CAN_ID(CMD_CLASS_SYSTEM,  5, 1)
#define MSG_SYS_CFG_HEARTBEAT       BUILD_CAN_ID(CMD_CLASS_SYSTEM,  6, 1)
#define MSG_SYS_MASTER_HEARTBEAT    BUILD_CAN_ID(CMD_CLASS_SYSTEM,  7, 1)
#define MSG_SYS_SLAVE_HEARTBEAT     BUILD_CAN_ID(CMD_CLASS_SYSTEM,  7, 0)
#define MSG_SYS_NOTSTOP             BUILD_CAN_ID(CMD_CLASS_SYSTEM,  8, 1)
#define MSG_SYS_ABORTED_BY_NOTSTOP  BUILD_CAN_ID(CMD_CLASS_SYSTEM,  8, 0)
#define MSG_SYS_MASTER_POWER        BUILD_CAN_ID(CMD_CLASS_SYSTEM,  9, 1)
#define MSG_SYS_RESET               BUILD_CAN_ID(CMD_CLASS_SYSTEM,  10, 1)
#define MSG_SYS_ACK_RESET           BUILD_CAN_ID(CMD_CLASS_SYSTEM,  10, 0)

// CAN-IDs for Boot Loader Messages
//! Update required message, the boot loader is waiting for a firmware update
#define MSG_SYS_UPDATE_REQUIRED     BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 0, 0)
//! Update request message, the master is ready to perform the update
#define MSG_SYS_UPDATE_MODE_REQUEST BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 1, 1)
//! Acknowledgement of the update request message
#define MSG_SYS_UPDATE_MODE_ACK     BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 1, 0)
//! Header message of the actual firmware update process
#define MSG_SYS_UPDATE_HEADER       BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 2, 1)
//! Acknowledgement of the update header message
#define MSG_SYS_UPDATE_HEADER_ACK   BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 2, 0)
//! Data message of the actual firmware update process, flow control bit 0
#define MSG_SYS_UPDATE_DATA_0       BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 3, 1)
//! Acknowledgement of the update data message, flow control bit 0
#define MSG_SYS_UPDATE_ACK_0        BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 3, 0)
//! Data message of the actual firmware update process, flow control bit 1
#define MSG_SYS_UPDATE_DATA_1       BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 4, 1)
//! Acknowledgement of the update data message, flow control bit 1
#define MSG_SYS_UPDATE_ACK_1        BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 4, 0)
//! Trailer message of the actual firmware update process
#define MSG_SYS_UPDATE_TRAILER      BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 5, 1)
//! Acknowledgement of the update trailer message
#define MSG_SYS_UPDATE_TRAILER_ACK  BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 5, 0)
//! Update info init message with info type and size.
#define MSG_SYS_UPDATE_INFO_INIT    BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 6, 1)
//! Acknowledgement of the update info init message
#define MSG_SYS_UPDATE_INFO_INIT_ACK BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 6, 0)
//! Update info message of the actual info update process
#define MSG_SYS_UPDATE_INFO         BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 7, 1)
//! Acknowledgement of the update info message
#define MSG_SYS_UPDATE_INFO_ACK     BUILD_CAN_ID(CMD_CLASS_ASSEMBLY, 7, 0)

//! Macro to build error codes
#define BL_BUILD_ERRCODE(Class,Code) BUILD_ERRCODE(MODULE_ID_BOOTLOADER, Class, Code)

// CAN-IDs for Error/Warning/Info Messages
#define MSG_EVENT_INFO              BUILD_CAN_ID(CMD_CLASS_ERROR,   0, 0)
#define MSG_EVENT_WARNING           BUILD_CAN_ID(CMD_CLASS_ERROR,   1, 0)
#define MSG_EVENT_ERROR             BUILD_CAN_ID(CMD_CLASS_ERROR,   2, 0)
#define MSG_EVENT_FATAL             BUILD_CAN_ID(CMD_CLASS_ERROR,   3, 0)

// CAN-IDs for Service Messages
#define MSG_SRV_STATISTICS          BUILD_CAN_ID(CMD_CLASS_SERVICE, 0, 0)
#define MSG_SRV_CFG_STATISTICS      BUILD_CAN_ID(CMD_CLASS_SERVICE, 0, 1)
#define MSG_SRV_REQ_RESET_DATA      BUILD_CAN_ID(CMD_CLASS_SERVICE, 1, 1)
#define MSG_SRV_ACK_RESET_DATA      BUILD_CAN_ID(CMD_CLASS_SERVICE, 1, 0)
#define MSG_SRV_REQ_MEMORY_FORMAT   BUILD_CAN_ID(CMD_CLASS_SERVICE, 5, 1)
#define MSG_SRV_ACK_MEMORY_FORMAT   BUILD_CAN_ID(CMD_CLASS_SERVICE, 5, 0)
#define MSG_SRV_REQ_SERIAL_NUMBER   BUILD_CAN_ID(CMD_CLASS_SERVICE, 6, 1)
#define MSG_SRV_SERIAL_NUMBER       BUILD_CAN_ID(CMD_CLASS_SERVICE, 6, 0)
#define MSG_SRV_REQ_ENDTEST_RESULT  BUILD_CAN_ID(CMD_CLASS_SERVICE, 7, 1)
#define MSG_SRV_ENDTEST_RESULT      BUILD_CAN_ID(CMD_CLASS_SERVICE, 7, 0)
#define MSG_SRV_REQ_HARDWARE_INFO   BUILD_CAN_ID(CMD_CLASS_SERVICE, 8, 1)
#define MSG_SRV_HARDWARE_INFO       BUILD_CAN_ID(CMD_CLASS_SERVICE, 8, 0)
#define MSG_SRV_REQ_SOFTWARE_INFO   BUILD_CAN_ID(CMD_CLASS_SERVICE, 9, 1)
#define MSG_SRV_SOFTWARE_INFO       BUILD_CAN_ID(CMD_CLASS_SERVICE, 9, 0)
#define MSG_SRV_REQ_LOADER_INFO     BUILD_CAN_ID(CMD_CLASS_SERVICE, 10, 1)
#define MSG_SRV_LOADER_INFO         BUILD_CAN_ID(CMD_CLASS_SERVICE, 10, 0)
#define MSG_SRV_REQ_LIFECYCLE_DATA  BUILD_CAN_ID(CMD_CLASS_SERVICE, 11, 1)
#define MSG_SRV_LIFECYCLE_DATA      BUILD_CAN_ID(CMD_CLASS_SERVICE, 11, 0)
#define MSG_SRV_REQ_LAUNCH_DATE     BUILD_CAN_ID(CMD_CLASS_SERVICE, 12, 1)
#define MSG_SRV_LAUNCH_DATE         BUILD_CAN_ID(CMD_CLASS_SERVICE, 12, 0)
#define MSG_SRV_REQ_BOARD_NAME      BUILD_CAN_ID(CMD_CLASS_SERVICE, 13, 1)
#define MSG_SRV_BOARD_NAME          BUILD_CAN_ID(CMD_CLASS_SERVICE, 13, 0)
#define MSG_SRV_REQ_BOARD_OPTIONS   BUILD_CAN_ID(CMD_CLASS_SERVICE, 14, 1)
#define MSG_SRV_BOARD_OPTIONS       BUILD_CAN_ID(CMD_CLASS_SERVICE, 14, 0)
#define MSG_CFG_VOLTAGE_MONITOR     BUILD_CAN_ID(CMD_CLASS_SERVICE, 15, 1)
#define MSG_SRV_REQ_VOLTAGE_STATE   BUILD_CAN_ID(CMD_CLASS_SERVICE, 16, 1)
#define MSG_SRV_VOLTAGE_STATE       BUILD_CAN_ID(CMD_CLASS_SERVICE, 16, 0)
#define MSG_CFG_CURRENT_MONITOR     BUILD_CAN_ID(CMD_CLASS_SERVICE, 17, 1)
#define MSG_SRV_REQ_CURRENT_STATE   BUILD_CAN_ID(CMD_CLASS_SERVICE, 18, 1)
#define MSG_SRV_CURRENT_STATE       BUILD_CAN_ID(CMD_CLASS_SERVICE, 18, 0)
#define MSG_SRV_REQ_UNIQUE_NUMBER   BUILD_CAN_ID(CMD_CLASS_SERVICE, 19, 1)
#define MSG_SRV_UNIQUE_NUMBER       BUILD_CAN_ID(CMD_CLASS_SERVICE, 19, 0)
#define MSG_SRV_REQ_REALTIME        BUILD_CAN_ID(CMD_CLASS_SERVICE, 20, 1)
#define MSG_SRV_REALTIME            BUILD_CAN_ID(CMD_CLASS_SERVICE, 20, 0)
#define MSG_CFG_SAFE_MONITOR        BUILD_CAN_ID(CMD_CLASS_SERVICE, 21, 1)
#define MSG_SRV_REQ_SAFE_STATE      BUILD_CAN_ID(CMD_CLASS_SERVICE, 22, 1)
#define MSG_SRV_SAFE_STATE          BUILD_CAN_ID(CMD_CLASS_SERVICE, 22, 0)

// CAN-IDs for Assembly Messages

// CAN-IDs for Process Data Messages
#define MSG_PROC_CONFIGURE          BUILD_CAN_ID(CMD_CLASS_PROCESS, 0, 1)
#define MSG_PROC_REQ_DATA           BUILD_CAN_ID(CMD_CLASS_PROCESS, 1, 1)
#define MSG_PROC_DATA               BUILD_CAN_ID(CMD_CLASS_PROCESS, 1, 0)

//@} End of doxygen group

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t canWriteMessage  (UInt16  Channel, CanMessage_t *Message);
Error_t canReadMessage   (UInt16 *Channel, CanMessage_t *Message);
Error_t canFlushMessages (UInt32 Timeout);
Error_t canTaskFunction  (void);

Error_t canInitializeLayer (void);

//****************************************************************************/

#endif /*BM_CAN_H*/

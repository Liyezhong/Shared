/****************************************************************************/
/*! \file bmError.h
 *
 *  \brief Error Handling Functions
 *
 *  $Version: $ 0.3
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all error handling functions. This includes
 *       sending of errors to the master via CAN and managing the status
 *       of errors that must explicitly revoked.
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

#ifndef BM_ERROR_H
#define BM_ERROR_H

//@{**************************************************************************/
//! Error Code Definition for Basemodule
//****************************************************************************/

// Macro to build error codes (codes 1...255 are reserved for the HAL)
#define BM_BUILD_ERRCODE(Class,Code) BUILD_ERRCODE(0, Class, Code+256)

// General errors
#define E_PARAMETER_OUT_OF_RANGE   BM_BUILD_ERRCODE(ERRCLASS_ERROR, 1)
#define E_MEMORY_FULL              BM_BUILD_ERRCODE(ERRCLASS_ERROR, 2)
#define E_TIMEOUT                  BM_BUILD_ERRCODE(ERRCLASS_ERROR, 3)
#define E_FUNCTION_UNSUPPORTED     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 4)

// Base module related errors
#define E_ERROR_MESSAGE_LOST       BM_BUILD_ERRCODE(ERRCLASS_ERROR, 10)
#define E_MODULE_TABLE_FULL        BM_BUILD_ERRCODE(ERRCLASS_ERROR, 11)
#define E_MODULE_ALREADY_EXISTS    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 12)
#define E_TASK_TABLE_FULL          BM_BUILD_ERRCODE(ERRCLASS_ERROR, 13)
#define E_TASK_NOT_EXISTS          BM_BUILD_ERRCODE(ERRCLASS_ERROR, 14)
#define E_REALTIME_NOT_SET         BM_BUILD_ERRCODE(ERRCLASS_ERROR, 15)
#define E_BAD_TIME_FORMAT          BM_BUILD_ERRCODE(ERRCLASS_ERROR, 16)
#define E_BAD_DATE_FORMAT          BM_BUILD_ERRCODE(ERRCLASS_ERROR, 17)
#define E_PROCESS_DATA_DISABLED    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 18)
#define E_PROCESS_DATA_UNSUPPORTED BM_BUILD_ERRCODE(ERRCLASS_ERROR, 19)
#define E_STATS_NOT_AVAILABLE      BM_BUILD_ERRCODE(ERRCLASS_ERROR, 20)
#define E_MODULE_INITIALIZATION    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 21)
#define E_PERSISTENTS_INVALID      BM_BUILD_ERRCODE(ERRCLASS_ERROR, 22)
#define E_UNEXPECTED_PARAMETERS    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 23)
#define E_RESET_BY_WATCHDOG        BM_BUILD_ERRCODE(ERRCLASS_ERROR, 24)

// System command related errors
#define E_PROTECTED_COMMAND        BM_BUILD_ERRCODE(ERRCLASS_ERROR, 30)
#define E_FIRMWARE_UPDATE_FAILED   BM_BUILD_ERRCODE(ERRCLASS_ERROR, 31)
#define E_HARDWARE_RESET_FAILED    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 32)
#define E_RESET_PATTERN_SEQUENCE   BM_BUILD_ERRCODE(ERRCLASS_ERROR, 33)
#define E_RESET_PATTERN_TIMEOUT    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 34)
#define E_ILLEGAL_MODE_CHANGE      BM_BUILD_ERRCODE(ERRCLASS_ERROR, 35)
#define E_NODE_BUSY                BM_BUILD_ERRCODE(ERRCLASS_ERROR, 36)

// Service command related errors
#define E_BOOTLOADER_MISSING       BM_BUILD_ERRCODE(ERRCLASS_ERROR, 40)
#define E_BOARD_OPTION_MISSING     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 41)
#define E_INFOBLOCK_MISSING        BM_BUILD_ERRCODE(ERRCLASS_ERROR, 42)
#define E_SERIAL_NUMBER_MISSING    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 43)
#define E_LIFE_TIME_UPDATE         BM_BUILD_ERRCODE(ERRCLASS_ERROR, 44)

// Storage partition access errors
#define E_INVALID_PARTITION_ID     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 50)
#define E_ADDRESS_OUT_OF_RANGE     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 51)
#define E_SIZE_OUT_OF_RANGE        BM_BUILD_ERRCODE(ERRCLASS_ERROR, 52)
#define E_CHECKSUM_ERROR           BM_BUILD_ERRCODE(ERRCLASS_ERROR, 53)
#define E_CHECKSUM_WRITE_ERROR     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 54)
#define E_STORAGE_UNFORMATTED      BM_BUILD_ERRCODE(ERRCLASS_ERROR, 55)
#define E_STORAGE_TOO_SMALL        BM_BUILD_ERRCODE(ERRCLASS_ERROR, 56)
#define E_STORAGE_CLOSE_ERROR      BM_BUILD_ERRCODE(ERRCLASS_ERROR, 57)
#define E_PARTITION_EXISTS         BM_BUILD_ERRCODE(ERRCLASS_ERROR, 59)
#define E_PARTITION_NOT_EXISTS     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 60)
#define E_PARTITION_ALREADY_OPEN   BM_BUILD_ERRCODE(ERRCLASS_ERROR, 61)
#define E_PARTITION_NOT_OPENED     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 62)
#define E_PARTITION_ACCESS_MODE    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 63)
#define E_PARTITION_TABLE_UPDATE   BM_BUILD_ERRCODE(ERRCLASS_ERROR, 64)
#define E_PARTITION_TABLE_FULL     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 65)
#define E_PARTITION_TABLE_CORRUPT  BM_BUILD_ERRCODE(ERRCLASS_ERROR, 66)
#define E_PARTITION_IS_OPEN        BM_BUILD_ERRCODE(ERRCLASS_ERROR, 67)
#define E_PARTITION_RESET_FAILED   BM_BUILD_ERRCODE(ERRCLASS_ERROR, 68)
#define E_PARTITION_HANDLE_INVALID BM_BUILD_ERRCODE(ERRCLASS_ERROR, 69)

// CAN related errors
#define E_UNKNOWN_MESSAGE          BM_BUILD_ERRCODE(ERRCLASS_ERROR, 80)
#define E_ILLEGAL_NODE_INDEX       BM_BUILD_ERRCODE(ERRCLASS_ERROR, 81)
#define E_ILLEGAL_NODE_TYPE        BM_BUILD_ERRCODE(ERRCLASS_ERROR, 82)
#define E_MISSING_PARAMETERS       BM_BUILD_ERRCODE(ERRCLASS_ERROR, 83)
#define E_RECEIVE_MESSAGE_LOST     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 84)
#define E_TRANSMIT_MESSAGE_LOST    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 85)
#define E_COMMAND_REJECTED         BM_BUILD_ERRCODE(ERRCLASS_ERROR, 86)
#define E_QUEUE_ALREADY_CREATED    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 87)
#define E_QUEUE_NOT_ALLOCATED      BM_BUILD_ERRCODE(ERRCLASS_ERROR, 88)
#define E_QUEUE_FULL               BM_BUILD_ERRCODE(ERRCLASS_ERROR, 89)

// Power monitor related errors
#define E_VOLTAGE_MONITOR_OFF      BM_BUILD_ERRCODE(ERRCLASS_ERROR, 90)
#define E_CURRENT_MONITOR_OFF      BM_BUILD_ERRCODE(ERRCLASS_ERROR, 91)
#define E_VOLTAGE_MONITOR_PORT     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 92)
#define E_CURRENT_MONITOR_PORT     BM_BUILD_ERRCODE(ERRCLASS_ERROR, 93)
#define E_VOLTAGE_MONITOR_FAILURE  BM_BUILD_ERRCODE(ERRCLASS_ERROR, 94)
#define E_CURRENT_MONITOR_FAILURE  BM_BUILD_ERRCODE(ERRCLASS_ERROR, 95)
#define E_SUPPLY_VOLTAGE_LOW       BM_BUILD_ERRCODE(ERRCLASS_ERROR, 96)
#define E_SUPPLY_CURRENT_HIGH      BM_BUILD_ERRCODE(ERRCLASS_ERROR, 97)

// Common function module errors
#define E_MODULE_NOT_CONFIGURED    BM_BUILD_ERRCODE(ERRCLASS_ERROR, 100)
#define E_MODULE_NOT_ENABLED       BM_BUILD_ERRCODE(ERRCLASS_ERROR, 101)
#define E_MODULE_NOT_USEABLE       BM_BUILD_ERRCODE(ERRCLASS_ERROR, 102)

// Base module info messages
#define I_INITIALIZATION_START     BM_BUILD_ERRCODE(ERRCLASS_INFO, 1)
#define I_INITIALIZATION_DONE      BM_BUILD_ERRCODE(ERRCLASS_INFO, 2)
#define I_PARTITION_CONVERTED      BM_BUILD_ERRCODE(ERRCLASS_INFO, 3)
#define I_PARTITION_RESET          BM_BUILD_ERRCODE(ERRCLASS_INFO, 4)

#define NO_ERROR                   0   // no error (used as return value)

//@} end of doxygen group

//****************************************************************************/
// Function Prototypes
//****************************************************************************/

Error_t bmSignalEvent (
    UInt16 Channel, Error_t ErrCode, Bool ErrState, UInt32 ErrInfo);
Error_t bmTransmitEvent (
    UInt16 Channel, Error_t ErrCode, UInt32 ErrInfo);

Error_t bmProcessEventCache (void);

//****************************************************************************/

#endif /*BM_ERROR_H*/

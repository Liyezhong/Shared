/****************************************************************************/
/*! \file halError.h
 *
 *  \brief  HAL error definitions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 24.02.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains definition of all HAL error codes
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

#ifndef HAL_ERROR_H
#define HAL_ERROR_H


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Structure to assign error string to error code (for debug only)
typedef struct {
    Error_t ErrCode;          //!< Error code
    const char *ErrText;      //!< Description as a string
} ErrorString_t;


//@{**************************************************************************/
//! Error Code Definition for HAL
//****************************************************************************/

#define HAL_BUILD_ERRCODE(Class,Code) BUILD_ERRCODE(0,Class,Code)

// General HAL errors
#define E_DEVICE_UNCONFIGURED     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 1)
#define E_DEVICE_HANDLE_INVALID   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 3)
#define E_DEVICE_NOT_EXISTS       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 4)
#define E_DEVICE_NOT_OPEN         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 5)
#define E_DEVICE_ALREADY_OPEN     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 6)
#define E_DEVICE_ACCESS_MODE      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 7)
#define E_DEVICE_WRITE_ONLY       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 8)
#define E_DEVICE_READ_ONLY        HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 9)
#define E_DEVICE_BUSY             HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 10)
#define E_DEVICE_LOCKED           HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 11)
#define E_DEVICE_DISABLED         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 12)
#define E_INVALID_PERIPHERAL_ID   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 13)
#define E_INVALID_INTERRUPT_ID    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 14)
#define E_INVALID_PRIORITY        HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 15)
#define E_INVALID_PARAMETER       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 16)
#define E_INVALID_REGISTER_ID     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 17)
#define E_UNKNOWN_DEVICE_ID       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 18)
#define E_UNKNOWN_STATUS_ID       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 19)
#define E_UNKNOWN_CONTROL_ID      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 20)
#define E_HEAP_MEMORY_FULL        HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 21)
#define E_HAL_NOT_INITIALIZED     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 22)
#define E_UNSUPPORTED_FUNCTION    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 23)

// GPIO port related error codes
#define E_PORT_DESCRIPTOR         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 30)
#define E_PORT_NOT_EXISTS         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 31)
#define E_PORT_DEFAULT_VALUE      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 32)
#define E_PORT_POLARITY_MASK      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 33)
#define E_PORT_DIRECTION          HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 34)
#define E_PORT_INTERFACE_TYPE     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 35)

// GPIO pin related error codes
#define E_PIN_DESCRIPTOR          HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 36)
#define E_PIN_NUMBER_INVALID      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 37)
#define E_PIN_TYPE_MISMATCH       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 38)
#define E_PIN_ALLOCATED_TWICE     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 39)
#define E_PIN_NOT_INTERRUPTABLE   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 40)

// Analog i/o related error codes
#define E_ADC_INPUT_NOT_EXISTS    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 41)
#define E_ADC_INPUT_USED_TWICE    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 42)
#define E_ADC_INPUT_SCAN_ERROR    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 43)
#define E_ADC_NOT_YET_SCANNED     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 44)
#define E_ADC_NOT_CALIBRATED      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 45)
#define E_ADC_UNSUPPORTED_WIDTH   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 46)
#define E_ADC_INTEGER_OVERFLOW    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 47)
#define E_DAC_OUTPUT_NOT_EXISTS   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 48)
#define E_DAC_OUTPUT_USED_TWICE   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 49)
#define E_DAC_UNSUPPORTED_WIDTH   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 50)

// UART specific error codes
#define E_SCI_ILLEGAL_FORMAT      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 51)
#define E_SCI_ILLEGAL_BAUDRATE    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 52)
#define E_SCI_FORMAT_MISSING      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 53)
#define E_SCI_FRAMING_ERROR       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 54)
#define E_SCI_PARITY_ERROR        HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 55)
#define E_SCI_OVERRUN_ERROR       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 56)
#define E_SCI_BREAK_DETECED       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 57)
#define E_SCI_RECEIVE_ERROR       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 58)
#define E_SCI_TRANSMIT_ERROR      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 59)
#define E_SCI_NOISE_ERROR         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 60)

// CAN controller related error codes
#define E_CAN_INVALID_BITRATE     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 61)
#define E_CAN_INVALID_SEGTIME     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 62)
#define E_CAN_RECEIVER_OVERRUN    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 63)
#define E_CAN_INVALID_MESSAGE     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 64)
#define E_CAN_INVALID_PARAMETER   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 65)
#define E_CAN_FILTER_COUNT        HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 66)

// Timer controller related error codes
#define E_TIMER_DESCRIPTOR        HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 70)
#define E_TIMER_CAPCOM_CHANNEL    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 71)
#define E_TIMER_CAPCOM_OVERFLOW   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 72)
#define E_TIMER_COUNT_DIRECTION   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 73)
#define E_TIMER_INVALID_PRESCALE  HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 74)
#define E_TIMER_DELAY_LIMIT       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 75)
#define E_VALUE_OUT_OF_RANGE      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 76)

// Stepper related error codes
#define E_STEPPER_DESCRIPTOR      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 80)
#define E_STEPPER_ILLEGAL_MODE    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 81)
#define E_STEPPER_MOTOR_STALLED   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 82)
#define E_STEPPER_SHORT_CIRCUIT   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 83)
#define E_STEPPER_TEMPERATURE     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 84)
#define E_STEPPER_OPEN_LOAD       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 85)
#define E_STEPPER_BUSY            HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 86)
#define E_STEPPER_NO_BUS          HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 87)
#define E_STEPPER_UNDERVOLTAGE    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 88)
#define E_STEPPER_SPI_PATTERN     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 89)

// Storage (FRAM) specific error codes
#define E_STORAGE_READ_ERROR      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 90)
#define E_STORAGE_WRITE_ERROR     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 91)
#define E_STORAGE_OPEN_ERROR      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 92)
#define E_STORAGE_NOT_EXISTS      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 93)
#define E_STORAGE_ADDRESS         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 94)
#define E_STORAGE_SIZE            HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 95)
#define E_STORAGE_CLASS           HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 96)
#define E_STORAGE_PROTECTED       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 97)

// DMA controller related error codes
#define E_DMA_INVALID_MODE        HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 100)
#define E_DMA_INVALID_PRIORITY    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 101)

// External interrupt related error codes
#define E_EXTINT_NOT_EXISTS       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 102)
#define E_EXTINT_USED_TWICE       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 103)

// Realtime clock related error codes
#define E_RTC_NOT_RUNNING         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 104)
#define E_RTC_YEAR_OUT_OF_RANGE   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 105)

// System clock PLL related error codes
#define E_SYSCLOCK_MULTIPLIER     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 106)
#define E_SYSCLOCK_DIVIDER        HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 107)
#define E_SYSCLOCK_RATE           HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 108)

// Watchdog related error codes
#define E_WATCHDOG_INTERVAL       HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 109)

// Flash memory related error codes
#define E_FLASH_ALIGNMENT         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 110)
#define E_FLASH_BLOCKSIZE         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 111)
#define E_FLASH_WRITE             HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 112)
#define E_FLASH_ERASE             HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 113)
#define E_FLASH_VERIFY            HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 114)
#define E_FLASH_WRITE_PROTECTED   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 116)
#define E_FLASH_BUSY              HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 117)
#define E_FLASH_TIMEOUT           HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 118)

// I2C interface related errors
#define E_BUS_INVALID_ADDRESS     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 119)
#define E_BUS_ILLEGAL_BAUDRATE    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 120)
#define E_BUS_ILLEGAL_CLOCKRATE   HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 121)
#define E_BUS_INEXACT_BAUDRATE    HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 122)
#define E_BUS_ALLOCATED_TWICE     HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 123)
#define E_BUS_MEMBER_COUNT        HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 124)
#define E_BUS_MEMBER_INVALID      HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 125)
#define E_BUS_JOB_ABORTED         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 126)
#define E_BUS_ERROR               HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 127)
#define E_BUS_ACKNOWLEDGE         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 128)
#define E_BUS_ARBITRATION         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 129)
#define E_BUS_UNSPECIFIED         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 130)
#define E_BUS_BUSY                HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 131)
#define E_BUS_JOB_TIMEOUT         HAL_BUILD_ERRCODE(ERRCLASS_ERROR, 132)

//@} end of doxygen group

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/
#ifdef DEBUG
UInt32 halGetErrorStrings (const ErrorString_t *ErrorStrings[]);
#endif

//****************************************************************************/

#endif /*HAL_ERROR_H*/

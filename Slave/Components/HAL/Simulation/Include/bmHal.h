/****************************************************************************/
/*! \file bmHal.h
 * 
 *  \brief HAL functions (simulation)
 *
 *  $Version: $ 0.1
 *  $Date:    $ 10.05.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains HAL (hardware abstraction layer) access function
 *       to be used to access the hardware. This is only a simulation of the
 *       HAL, which is usefull to debug higher layer software w/o hardware,
 *       using the code composer studio simulator. It will be replaced later
 *       by a real hardware abstraction layer.  
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

#ifndef BM_HAL_H
#define BM_HAL_H

#ifndef HAL_SIMULATION 
#include "halLayer.h"
#include "halSimulate.h"
#endif

#ifdef HAL_SIMULATION 

//#define HAL_TIMER_MAXCOUNT      MAX_UINT32

//****************************************************************************/

#define CHANNEL_CLASS_MASK      0xFF00
#define CHANNEL_INDEX_MASK      0x00FF

#define CHANNEL_CLASS_DIGIO     0x3200
#define CHANNEL_CLASS_ANALOG    0x7300
#define CHANNEL_CLASS_TIMER     0x4400
#define CHANNEL_CLASS_CAPTURE   0x2600
#define CHANNEL_CLASS_ENCODE    0x1900
#define CHANNEL_CLASS_SERIAL    0x8900
#define CHANNEL_CLASS_SPI       0x6200
#define CHANNEL_CLASS_I2C       0x0900
#define CHANNEL_CLASS_STORAGE   0x2900

#define CHANNELID(Class,Index)  ((Class) | (Index))

//****************************************************************************/
// Logical Channel ID Definition
//****************************************************************************/

#define HAL_PORT_NODE_INDEX     CHANNELID(CHANNEL_CLASS_DIGIO, 0) 
#define HAL_LED1_OUTPUT         CHANNELID(CHANNEL_CLASS_DIGIO, 1) 
#define HAL_LED2_OUTPUT         HAL_LED1_OUTPUT+1

#define HAL_DIGITAL_OUTPUTS     CHANNELID(CHANNEL_CLASS_DIGIO, 3) 
#define HAL_DIGITAL_OUTPUT_0    HAL_DIGITAL_OUTPUTS
#define HAL_DIGITAL_OUTPUT_1    HAL_DIGITAL_OUTPUTS+1
#define HAL_DIGITAL_OUTPUT_2    HAL_DIGITAL_OUTPUTS+2
#define HAL_DIGITAL_OUTPUT_3    HAL_DIGITAL_OUTPUTS+3
#define HAL_DIGITAL_OUTPUT_4    HAL_DIGITAL_OUTPUTS+4

#define HAL_DIGITAL_INPUTS      CHANNELID(CHANNEL_CLASS_DIGIO, 8) 
#define HAL_DIGITAL_INPUT_0     HAL_DIGITAL_INPUTS
#define HAL_DIGITAL_INPUT_1     HAL_DIGITAL_INPUTS+1
#define HAL_DIGITAL_INPUT_2     HAL_DIGITAL_INPUTS+2
#define HAL_DIGITAL_INPUT_3     HAL_DIGITAL_INPUTS+3
#define HAL_DIGITAL_INPUT_4     HAL_DIGITAL_INPUTS+4

#define HAL_CAN_SYSTEM_BUS      0

//****************************************************************************/

#define HAL_VOLTAGE_INPUT       CHANNELID(CHANNEL_CLASS_ANALOG, 0)
#define HAL_CURRENT_INPUT       CHANNELID(CHANNEL_CLASS_ANALOG, 1)

#define HAL_ANALOG_INPUTS       CHANNELID(CHANNEL_CLASS_ANALOG, 2) 
#define HAL_ANALOG_INPUT_0      HAL_ANALOG_INPUTS+0
#define HAL_ANALOG_INPUT_1      HAL_ANALOG_INPUTS+1
#define HAL_ANALOG_INPUT_2      HAL_ANALOG_INPUTS+2
#define HAL_ANALOG_INPUT_3      HAL_ANALOG_INPUTS+3
#define HAL_ANALOG_INPUT_4      HAL_ANALOG_INPUTS+4

#define HAL_ANALOG_OUTPUTS      CHANNELID(CHANNEL_CLASS_ANALOG, 7) 
#define HAL_ANALOG_OUTPUT_0     HAL_ANALOG_OUTPUTS+0
#define HAL_ANALOG_OUTPUT_1     HAL_ANALOG_OUTPUTS+1
#define HAL_ANALOG_OUTPUT_2     HAL_ANALOG_OUTPUTS+2
#define HAL_ANALOG_OUTPUT_3     HAL_ANALOG_OUTPUTS+3
#define HAL_ANALOG_OUTPUT_4     HAL_ANALOG_OUTPUTS+4

#define HAL_TIMER1              CHANNELID(CHANNEL_CLASS_TIMER, 0)
#define HAL_TIMER2              CHANNELID(CHANNEL_CLASS_TIMER, 1)
#define HAL_TIMER_SYSTEM        CHANNELID(CHANNEL_CLASS_TIMER, 2)

#define HAL_ENCODER1            CHANNELID(CHANNEL_CLASS_ENCODE, 0)
#define HAL_ENCODER2            CHANNELID(CHANNEL_CLASS_ENCODE, 1)

#define HAL_STORAGE_FRAM        CHANNELID(CHANNEL_CLASS_STORAGE, 0)
#define HAL_STORAGE_OTP         CHANNELID(CHANNEL_CLASS_STORAGE, 1)
#define HAL_STORAGE_FLASH       CHANNELID(CHANNEL_CLASS_STORAGE, 2)

//****************************************************************************/

typedef enum {
    INTERRUPTS_GLOBAL        = 0,    // Interrupt ID for cpu interrupt mask  
    INTERRUPT_TIMER1         = 13,
    INTERRUPT_TIMER2         = 14,
    INTERRUPT_DATALOG        = 15,
    INTERRUPT_RTOS           = 16,
    INTERRUPT_EMULATE        = 17,
    INTERRUPT_NMI            = 18,
    INTERRUPT_ILLEGAL_OPCODE = 19,
    INTERRUPT_USER_TRAP1     = 20,
    INTERRUPT_USER_TRAP2     = 21,
    INTERRUPT_USER_TRAP3     = 22,
    INTERRUPT_USER_TRAP4     = 23,
    INTERRUPT_USER_TRAP5     = 24,
    INTERRUPT_USER_TRAP6     = 25,
    INTERRUPT_USER_TRAP7     = 26,
    INTERRUPT_USER_TRAP8     = 27,
    INTERRUPT_USER_TRAP9     = 28,
    INTERRUPT_USER_TRAP10    = 29,
    INTERRUPT_USER_TRAP11    = 30,
    INTERRUPT_USER_TRAP12    = 31,
    INTERRUPT_ADC_SEQ1       = 32,
    INTERRUPT_ADC_SEQ2       = 33,
    INTERRUPT_EXTERN1        = 35,     
    INTERRUPT_EXTERN2        = 36,
    INTERRUPT_ADC            = 37,
    INTERRUPT_TIMER0         = 38,
    INTERRUPT_WAKE           = 39,           
    INTERRUPT_PWM1_TZ        = 40,
    INTERRUPT_PWM2_TZ        = 41,
    INTERRUPT_PWM3_TZ        = 42,
    INTERRUPT_PWM4_TZ        = 43,
    INTERRUPT_PWM5_TZ        = 44,
    INTERRUPT_PWM6_TZ        = 45,
    INTERRUPT_PWM1           = 48,
    INTERRUPT_PWM2           = 49,
    INTERRUPT_PWM3           = 50,
    INTERRUPT_PWM4           = 51,
    INTERRUPT_PWM5           = 52,
    INTERRUPT_PWM6           = 53,
    INTERRUPT_CAPTURE1       = 56,
    INTERRUPT_CAPTURE2       = 57,
    INTERRUPT_CAPTURE3       = 58,
    INTERRUPT_CAPTURE4       = 59,
    INTERRUPT_CAPTURE5       = 60,  // nicht beim 28xx
    INTERRUPT_CAPTURE6       = 61,  // dito
    INTERRUPT_QENCODER1      = 64,
    INTERRUPT_QENCODER2      = 65,    
    INTERRUPT_SPI1_RX        = 72,
    INTERRUPT_SPI1_TX        = 73,
    INTERRUPT_SPI2_RX        = 74,  // 2823x: McBSP_RX
    INTERRUPT_SPI2_TX        = 75,  // 2823x: McBSP_TX
    INTERRUPT_SPI3_RX        = 76,  // 2823x: McBSP_RX
    INTERRUPT_SPI3_TX        = 77,  // 2823x: McBSP_TX
    INTERRUPT_SPI4_RX        = 78,  // nur 280x
    INTERRUPT_SPI4_TX        = 79,  // dito                 
    INTERRUPT_DMA1           = 80,  // nicht beim 28xx
    INTERRUPT_DMA2           = 81,  // dito
    INTERRUPT_DMA3           = 82,  // dito
    INTERRUPT_DMA4           = 83,  // dito
    INTERRUPT_DMA5           = 84,  // dito
    INTERRUPT_DMA6           = 85,  // dito
    INTERRUPT_I2C1           = 88,
    INTERRUPT_I2C2           = 89,
    INTERRUPT_SCI3_RX        = 92,  // nicht beim 28xx
    INTERRUPT_SCI3_TX        = 93,  // dito 
    INTERRUPT_SCI1_RX        = 96,
    INTERRUPT_SCI1_TX        = 97,
    INTERRUPT_SCI2_RX        = 98,
    INTERRUPT_SCI2_TX        = 99,
    INTERRUPT_CAN1_RX        = 100,
    INTERRUPT_CAN1_TX        = 101,
    INTERRUPT_CAN2_0         = 102,
    INTERRUPT_CAN2_1         = 103,    
    INTERRUPT_EXTERN3        = 120,  // nicht beim 28xx     
    INTERRUPT_EXTERN4        = 121,  // dito   
    INTERRUPT_EXTERN5        = 122,  // dito  
    INTERRUPT_EXTERN6        = 123,  // dito  
    INTERRUPT_EXTERN7        = 124,  // dito      
    NUMBER_OF_INTERRUPTS     // must be last entry in list
} INTERRUPT_ID_t;

//****************************************************************************/

typedef enum {
    HAL_OPEN_READ  = BIT(0),
    HAL_OPEN_WRITE = BIT(1),
    HAL_OPEN_CLEAR = BIT(2),
    HAL_OPEN_RW    = HAL_OPEN_READ+HAL_OPEN_WRITE
} OPEN_MODES_t;

#define TIMER_MAXVALUE  MAX_UINT32

//! Control IDs for halTimerControl()
typedef enum {
    TIMER_CTRL_START   = BIT(0),   
    TIMER_CTRL_STOP    = BIT(1),
    TIMER_CTRL_RELOAD  = BIT(2),           
    TIMER_INTR_ENABLE  = BIT(3),
    TIMER_INTR_DISABLE = BIT(4),
    TIMER_INTR_CLEAR   = BIT(5)
} TIMER_CONTROL_t;

//! Status IDs for requesting the state of a timer
typedef enum {
    TIMER_STAT_PERIOD,
    TIMER_STAT_IsRUNNING,
    TIMER_INTR_IsENABLED,
    TIMER_INTR_IsFLAGED
} TIMER_STATUS_t;

typedef enum {
    ADDRESS_BOARD_OPTION_BLOCK,
    ADDRESS_BOARD_HARDWARE_INFO,
    ADDRESS_BOOTLOADER_STARTUP,
    ADDRESS_BOOTLOADER_SIGNATURE,
    ADDRESS_BOOTLOADER_UPDATE,    
    ADDRESS_BOOTLOADER_INFO,
    ADDRESS_PROCESSOR_RESET   
} ADDRESS_IDENTIFIER_t;

typedef enum {
    CAN_INTR_ENABLE  = BIT(3),
    CAN_INTR_DISABLE = BIT(4),
    CAN_INTR_CLEAR   = BIT(5)
} CAN_CONTROL_t;

typedef enum {
    DIGIO_STAT_BITCOUNT,
    DIGIO_STAT_MAXVALUE
} DIGIO_STATUS_ID_t;

typedef enum {
    ANAIO_STAT_BITCOUNT,
    ANAIO_STAT_MAXVALUE,
    ANAIO_STAT_OFFSET
} ANAIO_STATUS_ID_t;

typedef enum {
    RESET_CAUSED_BY_POWER_UP,
    RESET_CAUSED_BY_WATCHDOG
} RESET_REASON_t;

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

/*! CAN Message Buffer */
typedef struct {
    UInt32 CanID;       //!< CAN identifier
    UInt16 Length;      //!< length of data
    UInt8  Data[8];     //!< data bytes
    UInt32 Timestamp;   //!< timestamp
} CAN_MESSAGE_t;

typedef struct {
    UInt32 Duration;
    UInt16 StartValue;
    Int16  Offset;
    Int16  Steps;
    Int16  Goto;
} INPUT_PORT_DATA_t;

typedef void (*HAL_INTERRUPT_HANDLER) (UInt16 InterruptID);

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

void*    halGetAddress    (ADDRESS_IDENTIFIER_t OptionID);
ERROR_t  halSetAddress    (ADDRESS_IDENTIFIER_t OptionID, void *Address);

HANDLE_t halCanOpen       (UInt16 Channel);
ERROR_t  halCanControl    (HANDLE_t Handle, UInt16 ParamID, UInt16 Value);
ERROR_t  halCanRead       (HANDLE_t Handle, CAN_MESSAGE_t* Message);
ERROR_t  halCanWrite      (HANDLE_t Handle, CAN_MESSAGE_t* Message);
ERROR_t  halCanInject     (CAN_MESSAGE_t* Message);
ERROR_t  canRegisterInterrupt (
                           UInt16 InterruptID, HAL_INTERRUPT_HANDLER IntHandler);

HANDLE_t halOpenStorage   (UInt16 DeviceID, UInt16 Mode);
ERROR_t  halReadStorage   (HANDLE_t Handle, UInt32 Address, void* buffer, UInt32 Size);
ERROR_t  halWriteStorage  (HANDLE_t Handle, UInt32 Address, void* buffer, UInt32 Size);
HANDLE_t halEraseStorage  (HANDLE_t Handle, UInt32 Address, UInt32 Size);
HANDLE_t halCloseStorage  (HANDLE_t Handle);
UInt32   halStorageSize   (HANDLE_t Handle);

ERROR_t  halTimerControl  (HANDLE_t Handle, UInt16 ControlID);
ERROR_t  halTimerRead     (HANDLE_t Handle, UInt32* Counter);
ERROR_t  halTimerWrite    (HANDLE_t Handle, UInt32 Counter, UInt16 ControlID);
HANDLE_t halTimerOpen     (UInt16  Channel, UInt16 Resolution);

ERROR_t  halDigitalStatus (HANDLE_t Handle, UInt16 StatusID);
ERROR_t  halDigitalWrite  (HANDLE_t Handle, UInt16 value);
ERROR_t  halDigitalRead   (HANDLE_t Handle, UInt16 *value);
HANDLE_t halDigitalOpen   (UInt16  Channel, UInt16 Mode);

ERROR_t  halAnalogStatus  (HANDLE_t Handle, UInt16 StatusID);
ERROR_t  halAnalogWrite   (HANDLE_t Handle, UInt16 value);
ERROR_t  halAnalogRead    (HANDLE_t Handle, UInt16 *value);
HANDLE_t halAnalogOpen    (UInt16  Channel, UInt16 Mode);

RESET_REASON_t halGetResetReason (void);

ERROR_t  halWatchdogTrigger (void);
ERROR_t  halWatchdogDisable (void);
ERROR_t  halWatchdogEnable  (void);
void     halHardwareReset   (void);
Bool     halPowerSupplyGood (void);
ERROR_t  halControlTask     (void);

ERROR_t  halRegisterInterrupt (UInt16 InterruptID, HAL_INTERRUPT_HANDLER IntHandler);
void     halEnableInterrupt   (UInt16 InterruptID);    
void     halDisableInterrupt  (UInt16 InterruptID);

ERROR_t  halRegisterAnalogInputPattern (
            UInt16 Channel, const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize);
            
ERROR_t  halRegisterDigitalInputPattern (
            UInt16 Channel, const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize);

ERROR_t  halInitializeHalLayer (void); 

//****************************************************************************/

#endif /*HAL_SIMULATION*/
#endif /*BM_HAL_H*/

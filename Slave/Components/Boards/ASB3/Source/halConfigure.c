/***********************************************************************************/
/*! \file halConfigure.c
 *
 *  \brief  HAL configuration file
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module is used to configure the hardware abstraction layer.
 *      It must be adopted for each hardware board. The following points
 *      must be configurated:
 *
 *      - Pin multiplexing (GPIO, special functions)
 *      - Logical port (digital in-/outputs) descriptor
 *      - Logical analog input/output descriptor
 *      - Logical timer descriptors
 *      - Logical memory descriptors
 *      - Logical serial busses (I2C, SPI)
 *      - External interrupt inputs
 *      - Logical motor decriptors
 *
 *      This file works in conjunction with the halConfigure.h header
 *      file, which must also be adopted to the boards needs.
 *
 *      ATTENTION: Configuration must match the real hardware, else
 *      damage of hardware is possible.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/***********************************************************************************/

#include "Global.h"
#include "halCommon.h"
#include "halDeviceID.h"
#include "halConfigure.h"
#include "halProcessor.h"


/***********************************************************************************/
// Processor Frequency Settings
/***********************************************************************************/

const UInt32 halSystemFrequency   = 72000000; //!< System clock frequency [Hz]
const UInt32 halCrystalFrequency  = 16000000; //!< Processor crystal frequency [Hz]
const UInt32 halRtcClockFrequency = 0;        //!< Real time crystal frequency [Hz]

const UInt32 halProcessorVoltage  = 3300;     //!< Nominal processor voltage [mV]
const UInt32 halWatchdogInterval  = 2000;     //!< Watchdog interval time [ms]

const UInt32 halNonVolatileSize   = 256;      //!< Non volatile memory size
const UInt32 halNonVolatileCache  = 256;      //!< Non volatile memory cache size

const UInt32 halCanDefaultBitrate = 500000;   //!< CAN bus baudrate [Hz]


/***********************************************************************************/
/*!
 *  \brief   Processor pin descriptor table
 *
 *      The pin descriptor table must contain one entry for each processor
 *      pin to configure. The following informations must be specified for
 *      each single pin:
 *
 *      - Physical port number
 *      - Physical port bit number
 *      - Pin multiplexer setting (GPIO / special function)
 *      - Pin type (direction, attributes)
 *      - Input filtering / output max. speed
 *
 *      Which peripheral i/o signal is mapped to which pin is fixed and can
 *      be influenced only in a limited manner by configuring the peripheral
 *      pin mapping (see halPeripheralMapping).
 *
 ***********************************************************************************/

const halPinDesciptor_t halPinDescriptors[] = {
    { PORT_A,  0, PIN_MUX_SFIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // FanSpeedSensor1 (TIM2_CH1)
    { PORT_A,  1, PIN_MUX_SFIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // FanSpeedSensor2 (TIM2_CH2)
    { PORT_A,  3, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // STP1/Limit1
    { PORT_A,  4, PIN_MUX_GPIO, PIN_TYP_DOUT50, PIN_OPT_NONE }, // SPI1/NSS
    { PORT_A,  5, PIN_MUX_SFIO, PIN_TYP_DOUT50, PIN_OPT_NONE }, // SPI1/SCK
    { PORT_A,  6, PIN_MUX_SFIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // SPI1/MISO
    { PORT_A,  7, PIN_MUX_SFIO, PIN_TYP_DOUT50, PIN_OPT_NONE }, // SPI1/MOSI
    { PORT_A,  8, PIN_MUX_SFIO, PIN_TYP_DOUT50, PIN_OPT_NONE }, // STP1 CLK/MCO
    { PORT_A,  9, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // ControlHeatingElement
    { PORT_A, 11, PIN_MUX_SFIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN1 RX
    { PORT_A, 12, PIN_MUX_SFIO, PIN_TYP_DOUT10, PIN_OPT_NONE }, // CAN1 TX

    { PORT_B,  6, PIN_MUX_SFIO, PIN_TYP_DOUT10, PIN_OPT_OPENDR }, // I2C1/SCL
    { PORT_B,  7, PIN_MUX_SFIO, PIN_TYP_DOUT10, PIN_OPT_OPENDR }, // I2C1/SDA
    { PORT_B, 11, PIN_MUX_GPIO, PIN_TYP_DOUT50, PIN_OPT_NONE }, // TempADC_#CS
    { PORT_B, 12, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // TempADC_Dout#RDY
    { PORT_B, 13, PIN_MUX_SFIO, PIN_TYP_DOUT50, PIN_OPT_NONE }, // TempADC_SCLK (SPI2_SCK)
    { PORT_B, 14, PIN_MUX_SFIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // TempADC_Dout#RDY (SPI2_MISO)
    { PORT_B, 15, PIN_MUX_SFIO, PIN_TYP_DOUT50, PIN_OPT_NONE }, // TempADC_Din (SPI2_MOSI)

    { PORT_C,  0, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN10/MainsCurrentMeasurement
    { PORT_C,  1, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN11/Supply Current
    { PORT_C,  2, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN12/STP Current A
    { PORT_C,  3, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN13/STP Current B
    { PORT_C,  4, PIN_MUX_SFIO, PIN_TYP_ANALOG, PIN_OPT_NONE }, // AIN14/Supply Voltage

    { PORT_D,  0, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // CAN_LBKA
    { PORT_D,  1, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN_ID4
    { PORT_D,  2, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN_ID3
    { PORT_D,  3, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN_ID2
    { PORT_D,  4, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // CAN_ID1
    { PORT_D, 10, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // FanControl1
    { PORT_D, 11, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // FanControl2

    { PORT_E,  0, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // LimitSwitch2
    { PORT_E,  1, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // MainVoltageSwitchRelayControl
    { PORT_E,  4, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // LED1
    { PORT_E,  5, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // LED2
    { PORT_E,  6, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // LED3
    { PORT_E, 12, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // STP1 ENN
    { PORT_E, 13, PIN_MUX_GPIO, PIN_TYP_DINPUT, PIN_OPT_NONE }, // STP1 SG
    { PORT_E, 14, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }, // STP1 STEP
    { PORT_E, 15, PIN_MUX_GPIO, PIN_TYP_DOUT2,  PIN_OPT_NONE }  // STP1 DIR
};

const UInt32 halPinDescriptorCount = ELEMENTS(halPinDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Logical port descriptor table
 *
 *      The hardware abstraction layer offers logical ports. Logical ports
 *      are mapped to a configurable number of bits on physical ports and
 *      can be accessed without influencing other bits on a physical port.
 *      Following parameters must be configured for each logical port:
 *
 *      - Logical port identifier (Device ID)
 *      - Port data direction (input, output)
 *      - Interface (intern, serial, I2C, SPI)
 *      - Physical port number (GPIO)
 *      - Bit position inside physical port
 *      - Port width (number of bits)
 *      - Polarity bitmask (inversion bitmask)
 *      - Initial output value
 *
 *      The physical port bits must be configured as GPIOs to be useable for
 *      logical ports (see pin descriptor table).
 *
 ***********************************************************************************/

const halPortDescriptor_t halPortDescriptors[] = {
    { HAL_CAN_NODE_INDEX,     DIR_INPUT,  BUS_TYPE_INTERN, PORT_D,  1, 4, 0, 0 },
    { HAL_CAN_LOOPBACK,       DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D,  0, 1, 0, 0 },

    { HAL_STATUS_LED1,        DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E,  6, 1, 0, 0 },
    { HAL_STATUS_LED2,        DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E,  4, 1, 0, 0 },
    { HAL_STATUS_LED3,        DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E,  5, 1, 0, 0 },

    { HAL_STEPPER1_ENABLE,    DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E, 12, 1, 1, 0 },
    { HAL_STEPPER1_LIMIT1,    DIR_INPUT,  BUS_TYPE_INTERN, PORT_A,  3, 1, 1, 0 },
    { HAL_STEPPER1_LIMIT2,    DIR_INPUT,  BUS_TYPE_INTERN, PORT_E,  0, 1, 1, 0 },
    { HAL_STEPPER1_SELECT,    DIR_OUTPUT, BUS_TYPE_INTERN, PORT_A,  4, 1, 0, 0 },

    { HAL_SPI_SELECT_0,       DIR_OUTPUT, BUS_TYPE_INTERN, PORT_B, 11, 1, 0, 0 },
    { HAL_TEMP_FANCONTROL_0,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 10, 1, 0, 0 },
    { HAL_TEMP_FANCONTROL_1,  DIR_OUTPUT, BUS_TYPE_INTERN, PORT_D, 11, 1, 0, 0 },
    { HAL_TEMP_MAINVOLTAGE_0, DIR_OUTPUT, BUS_TYPE_INTERN, PORT_E,  1, 1, 0, 0 },
    { HAL_TEMP_CTRLHEATING_0, DIR_OUTPUT, BUS_TYPE_INTERN, PORT_A,  9, 1, 0, 0 },
};
const UInt32 halPortDescriptorCount = ELEMENTS(halPortDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Logical analog channel descriptor table
 *
 *      The hardware abstraction layer offers logical analog channels,
 *      which can be inputs or outputs and can use processor internal
 *      converters or external converters. 
 *
 *      Following parameters must be configured for each logical port:
 *
 *      - Logical channel ID (device ID)
 *      - Data direction (input, output)
 *      - Interface (intern, serial, I2C, SPI)
 *      - Converter resolution (number of bits)
 *      - Physical channel number
 *      - Conversion time
 *      - Scaling facor (value for full scale)
 *      - Initial output value
 *
 ***********************************************************************************/

const halAnalogDescriptor_t halAnalogDescriptors[] = {
    { HAL_SUPPLY_VOLTAGE,    DIR_INPUT, BUS_TYPE_INTERN, 12, 14, 6,  30000, 0 },
    { HAL_SUPPLY_CURRENT,    DIR_INPUT, BUS_TYPE_INTERN, 12, 11, 6,    750, 0 },
    { HAL_STEPPER1_PHASEA,   DIR_INPUT, BUS_TYPE_INTERN, 12, 12, 6,   3000, 0 },
    { HAL_STEPPER1_PHASEB,   DIR_INPUT, BUS_TYPE_INTERN, 12, 13, 6,   3000, 0 },
    { HAL_TEMP_CURRENT_0,    DIR_INPUT, BUS_TYPE_INTERN, 12, 10, 6,   3000, 0 },
    { HAL_TEMP_COMPENSATE_0, DIR_INPUT, BUS_TYPE_AD779x, 24,  0, 0,  18000, 0 },
    { HAL_TEMP_SENSOR_0,     DIR_INPUT, BUS_TYPE_AD779x, 24,  1, 0, -18281, 0 },
    { HAL_TEMP_SENSOR_1,     DIR_INPUT, BUS_TYPE_AD779x, 24,  2, 0, -18281, 0 },
};
const UInt32 halAnalogDescriptorCount = ELEMENTS(halAnalogDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Logical timers descriptor table
 *
 *      The timer descriptor table must contain one entry for each timer and
 *      for each capture/compare unit used. The following informations must be
 *      specified for each single pin:
 *
 *      - Logical timer ID (device ID)
 *      - Physical timer number
 *      - Physical capture/compare unit number
 *      - Priority of interrupt (1=highest, 15=lowest, 0=default)
 *      - Operation of capture/compare unit (capture, compare, disabled)
 *      - Mode of timer resp. capture/compare channel
 *
 *      The interrupt priority must be specified only for the counter, the
 *      priorities specified for the capture/compare units are not evaluated.
 *
 *      Capture/compare units that are not used needs not to be specified.
 *
 ***********************************************************************************/

const halTimerDescriptor_t halTimerDescriptors[] = {

    // Timer 1 programmed for internal compare (used by stepper module)
    { HAL_STEPPER1_TIMER,   0, 0, 0, TIM_SEL_COUNTER, TIM1_MODE_MASTER  },
    { HAL_STEPPER1_TIMER,   0, 0, 0, TIM_SEL_COMPARE, TIM1_MODE_CAPCOM1 },
    { HAL_STEPPER1_TIMER,   0, 1, 0, TIM_SEL_COMPARE, TIM1_MODE_CAPCOM2 },
    { HAL_STEPPER1_TIMER,   0, 2, 0, TIM_SEL_COMPARE, TIM1_MODE_CAPCOM3 },
    { HAL_STEPPER1_TIMER,   0, 3, 0, TIM_SEL_COMPARE, TIM1_MODE_CAPCOM4 },

    // Timer 2 programmed for the measurement of the fan speeds
    { HAL_TEMP_FANSPEED,    1, 0, 0, TIM_SEL_COUNTER, TIM2_MODE_MASTER  },
    { HAL_TEMP_FANSPEED,    1, 0, 0, TIM_SEL_CAPTURE, TIM2_MODE_CAPCOM1 },
    { HAL_TEMP_FANSPEED,    1, 1, 0, TIM_SEL_CAPTURE, TIM2_MODE_CAPCOM2 },
    { HAL_TEMP_FANSPEED,    1, 2, 0, TIM_SEL_DISABLE, TIM2_MODE_CAPCOM3 },
    { HAL_TEMP_FANSPEED,    1, 3, 0, TIM_SEL_DISABLE, TIM2_MODE_CAPCOM4 },
};
const UInt32 halTimerDescriptorCount = ELEMENTS(halTimerDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Stepper motor descriptor table
 *
 *      The stepper motor descriptor table must contain one entry for each motor.
 *      The following informations must be specified for each single motor:
 *
 *      - Logical stepper motor device ID
 *      - SPI number the driver is connected to
 *      - Stepper controller configuration words
 *
 *      Multiple motors can be maintained using one SPI port, as long as the
 *      slave select pin NSS differs. Nevertheless, only one of those motors
 *      can be open at the same time.
 *
 ***********************************************************************************/

const halStepperDescriptor_t halStepperDescriptors[] = {
    { HAL_STEPPER_MOTOR_0, 0, { 0x901B4, 0xD4006, 0xE02D0, 0xA8202 }},
};
const UInt32 halStepperDescriptorCount = ELEMENTS(halStepperDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Memory descriptor table
 *
 *      The memory descriptor table must contain one entry for each logical
 *      memory. The following informations must be configured for each
 *      logical memory:
 *
 *      - Logical device ID
 *      - Storage class (FRAM, EEPROM, FLASH)
 *      - Access mode (Read/Write/Erase)
 *      - Base address
 *      - Size of logical memory
 *
 *      In case a memory device is connected via a serial bus (I2C, SPI) the
 *      upper bits contain the bus number and chip (slave) select bits. See
 *      in the individual device modules for more informations.
 *
 *      A physical memory can contain multiple logical memories by
 *      specifying different different start addresses.
 *
 ***********************************************************************************/

const halStorageDescriptor_t halStorageDescriptors[] = {
   { HAL_STORAGE_FLASH, MEM_CLASS_FLASH, HAL_OPEN_RWE, 0x08000000, 0x20000 },
   { HAL_STORAGE_OTP,   MEM_CLASS_FLASH, HAL_OPEN_RWE, 0x1FFFF000, 0x00800 },
   { HAL_STORAGE_FRAM,  MEM_CLASS_FRAM,  HAL_OPEN_RWE, 0x00000000, 256 }
};
const UInt32 halStorageDescriptorCount = ELEMENTS(halStorageDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Serial devices descriptor table
 *
 *      The serial descriptor table must contain one entry for each logical serial
 *      device (I2C, SPI, CAN, UART). The following informations must be configured
 *      for each device:
 *
 *      - Logical device ID
 *      - Logical output port for chip select (SPI only)
 *      - Bus class (I2C, SPI, CAN)
 *      - Number of bus members (I2C only)
 *      - Physical device/interface number
 *      - Baud rate
 *
 *      The number of bus members is applicable only for I2C busses to specify the
 *      max. number of slaves connected to the bus. In case of SPI, each slave
 *      device connected to the SPI bus must have it's own entry into this table.
 *
 ***********************************************************************************/

const halSerialDescriptor_t halSerialDescriptors[] = {
    { HAL_CAN_SYSTEM, 0,                SIO_TYPE_CAN, 1, 0,  500000 },
    { HAL_BUS_I2C_A,  0,                SIO_TYPE_I2C, 1, 0,  400000 },
    { HAL_BUS_SPI_A,  HAL_SPI_SELECT_0, SIO_TYPE_SPI, 1, 1, 4500000 }
};
const UInt32 halSerialDescriptorCount = ELEMENTS(halSerialDescriptors);


/***********************************************************************************/
/*!
 *  \brief   ADC779x analog input descriptor table
 *
 *      The ADC779x descriptor table must contain one entry for each channel of the
 *      ADC. The following informations must be configured for each device:
 *
 *      - Slave device ID (SPI)
 *      - Input channel number (logical)
 *      - Input scan relative frequency
 *      - AD779x i/o register setting
 *      - AD779x mode register setting
 *      - AD779x configuration register setting
 *
 ***********************************************************************************/

const halAd779xDescriptor_t halAd779xDescriptors[] = {
    { HAL_BUS_SPI_A, 0, 0, 0x3, 0xA, 0x1030 },
    { HAL_BUS_SPI_A, 1, 0, 0x3, 0xA, 0x8E81 },
    { HAL_BUS_SPI_A, 2, 0, 0x3, 0xA, 0xCE82 }
};
const UInt32 halAd779xDescriptorCount = ELEMENTS(halAd779xDescriptors);


/***********************************************************************************/
/*!
 *  \brief   Processor peripheral pin mappings
 *
 *      The peripheral pin mapping descriptor contains the informations
 *      for the various processor internal peripherals and to which pins
 *      the i/o signals of those periperhals are routed.
 *
 *      The defines used in this structure are defined in halConfigure.h
 *      and should be adopted there.
 *
 ***********************************************************************************/

const halPinMapping_t halPinMappings = {
    MAPPING_SPI1,       //!< SPI1 mapping
    MAPPING_I2C1,       //!< I2C1 mapping
    MAPPING_USART1,     //!< USART1 mapping
    MAPPING_USART2,     //!< USART2 mapping
    MAPPING_USART3,     //!< USART3 mapping
    MAPPING_TIMER1,     //!< TIMER1 mapping
    MAPPING_TIMER2,     //!< TIMER2 mapping
    MAPPING_TIMER3,     //!< TIMER3 mapping
    MAPPING_TIMER4,     //!< TIMER4 mapping
    MAPPING_CAN1,       //!< CAN1 alternate function mapping
    MAPPING_DEFAULT,    //!< Port D0/D1 remap to OSC IN/OUT
    MAPPING_TIMER5,     //!< TIMER5 mapping
    MAPPING_DEFAULT,    //!< ADC1 ext. trigger injected mapping
    MAPPING_DEFAULT,    //!< ADC1 ext. trigger regular mapping
    MAPPING_DEFAULT,    //!< ADC2 ext. trigger injected mapping
    MAPPING_DEFAULT,    //!< ADC2 ext. trigger regular mapping
    MAPPING_DEFAULT,    //!< Ethernet MAC I/O mapping
    MAPPING_CAN2,       //!< CAN2 I/O mapping
    MAPPING_DEFAULT,    //!< Ethernet MII/RMII selection
    MAPPING_SWJCFG,     //!< Serial wire JTAG configuration
    MAPPING_DEFAULT,    //!< Reserved
    MAPPING_SPI3,       //!< SPI3 mapping
    MAPPING_DEFAULT,    //!< TIMER2 internal trigger 1 mapping
    MAPPING_DEFAULT,    //!< Ethernet PTP PPS mapping
    MAPPING_DEFAULT,    //!< Reserved
    MAPPING_TIMER15,    //!< TIMER15 mapping
    MAPPING_TIMER16,    //!< TIMER16 mapping
    MAPPING_TIMER17,    //!< TIMER17 mapping
    MAPPING_DEFAULT,    //!< CEC mapping
    MAPPING_DEFAULT,    //!< TIMER1 CH1 mapping
    MAPPING_TIMER9,     //!< TIMER9 mapping
    MAPPING_TIMER10,    //!< TIMER10 mapping
    MAPPING_TIMER11,    //!< TIMER11 mapping
    MAPPING_TIMER13,    //!< TIMER13 mapping
    MAPPING_TIMER14,    //!< TIMER14 mapping
    MAPPING_DEFAULT,    //!< FSMC mapping
    MAPPING_DEFAULT,    //!< TIMER6/7 DMA mapping
    MAPPING_TIMER12,    //!< TIMER12 mapping
    MAPPING_DEFAULT,    //!< Miscellaneous mapping
    MAPPING_DEFAULT     //!< Reserved
};


/***********************************************************************************/
/*!
 *  \brief   Processor information block
 *
 *      The Processor information block contains information about the selected
 *      processor. This includes:
 *
 *      - Processor family and density
 *      - Processor clock rate (max)
 *      - Size of RAM and FLASH memory
 *      - Number of instances of each peripheral
 *      - Timers selection bitmap
 *
 *      The timer selection bitmap indicates which timers are available on
 *      the selected processor (Bit 0..16 are related to TIM1..T17).
 *
 *      The defines used in this structure are defined in halProcessor.h.
 *      They depend on the value of the PROCESSOR define, which should
 *      select one of the supported members of the STM32F10xxx family.
 *      See halProcessor.h for more details.
 *
 ***********************************************************************************/

const halProcessorInfo_t halProcessorInfo = {
    DEVICE_FAMILY,      //!< Processor family ID
    DEVICE_DENSITY,     //!< Processor density ID
    DEVICE_SYSCLOCK,    //!< Processor clock frequency
    NUMBER_OF_GPIOs,    //!< Number of general purpose I/O's
    NUMBER_OF_PORTs,    //!< Number of processor ports
    NUMBER_OF_ADCs,     //!< Number of analog inputs
    NUMBER_OF_DACs,     //!< Number of analog outputs
    NUMBER_OF_CANs,     //!< Number of CAN controllers
    NUMBER_OF_DMAs,     //!< Number of DMA channels
    NUMBER_OF_TIMERS,   //!< Number of hardware timers
    NUMBER_OF_UARTs,    //!< Number of USARTs
    NUMBER_OF_SPIs,     //!< Number of SPI devices
    NUMBER_OF_I2Cs,     //!< Number of I2C devices
    TIMERS_BITMASK,     //!< Timer selection bitmask
    SIZEOF_FLASH,       //!< Size of FLASH memory (MB)
    SIZEOF_RAM          //!< Size of RAM memory (KB)
};

/***********************************************************************************/

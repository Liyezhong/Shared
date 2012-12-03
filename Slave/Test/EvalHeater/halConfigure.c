/****************************************************************************/
/*! \file halConfigure.c
 * 
 *  \brief  HAL configuration file
 *
 *  $Version: $ 0.1
 *  $Date:    $ 11.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module is used to configure the hardware abstraction layer. 
 *       It must be adopted for each hardware board. The following points
 *       must be configurated:
 * 
 *       - Pin multiplexing (GPIO, special functions)
 *       - Input qualification (filtering)
 *       - Pullup resistor enable/disable
 *       - Logical analog channel descriptors
 *       - Logical digital port decriptors
 *       - External interrupt inputs
 *
 *       This file works in conjunction with the halConfigure.h header
 *       file, which must also be adopted to your needs.
 * 
 *       ATTENTION: Configuration must match the real hardware, else 
 *       damage of hardware is possible.
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

#include "Global.h"
#include "halConfigure.h"
#include "halCommon.h"


/*****************************************************************************/
/*! 
 *  \brief   Logical port descriptor table
 *
 *      The hardware abstraction layer offers logical ports. Logical ports
 *      are mapped to a configurable number of bits on physical ports and
 *      can be accessed without influencing other bits on a physical port.
 *      Following parameters must be configured for each logical port:
 * 
 *      - Logical port identifier (channel id)
 *      - Interface (intern, serial, I2C, SPI)
 *      - Port data direction (input, output)
 *      - Port width (number of bits)
 *      - Physical port number (GPIO)
 *      - Position inside pysical port (number of bit shifts)
 *      - Polarity bitmask (inversion bitmask)
 *      - Initial output value
 *
 *      The physical port bits must be configured as GPIOs to be useable for
 *      logical ports (see pin descriptor table). 
 * 
 ****************************************************************************/

const PORT_DESCRIPTOR_t halLogicalPortsTable[] = {
    { HAL_PORT_NODE_INDEX,  PORT_INPUT,  PORT_TYPE_INTERN, 4, 0, 11, 0, 0 },
    { HAL_LED1_OUTPUT,      PORT_OUTPUT, PORT_TYPE_INTERN, 1, 1,  2, 1, 0 },
    { HAL_LED2_OUTPUT,      PORT_OUTPUT, PORT_TYPE_INTERN, 1, 0, 31, 1, 0 },
    { HAL_DIGITAL_OUTPUT_0, PORT_OUTPUT, PORT_TYPE_INTERN, 3, 1, 26, 7, 7 },
    { HAL_DIGITAL_OUTPUT_1, PORT_OUTPUT, PORT_TYPE_INTERN, 1, 0,  0, 0, 0 },
    { HAL_DIGITAL_OUTPUT_2, PORT_OUTPUT, PORT_TYPE_INTERN, 1, 0,  2, 0, 0 },
    { HAL_DIGITAL_OUTPUT_3, PORT_OUTPUT, PORT_TYPE_INTERN, 1, 0,  4, 0, 0 },
    { HAL_DIGITAL_OUTPUT_4, PORT_OUTPUT, PORT_TYPE_INTERN, 1, 0,  6, 0, 0 },
    { HAL_DIGITAL_INPUT_0,  PORT_INPUT,  PORT_TYPE_INTERN, 2, 0,  8, 3, 0 },
    { HAL_DIGITAL_INPUT_1,  PORT_INPUT,  PORT_TYPE_INTERN, 1, 0, 10, 0, 0 },
    { HAL_DIGITAL_INPUT_2,  PORT_INPUT,  PORT_TYPE_INTERN, 1, 1, 16, 0, 0 },
    { HAL_DIGITAL_INPUT_3,  PORT_INPUT,  PORT_TYPE_INTERN, 1, 2, 20, 0, 0 },
    { HAL_DIGITAL_INPUT_4,  PORT_INPUT,  PORT_TYPE_INTERN, 1, 2, 22, 0, 0 },
};
const int halLogicalPortsTableLength = ELEMENTS(halLogicalPortsTable);


/*****************************************************************************/
/*! 
 *  \brief   Logical analog channel descriptor table
 *
 *      The hardware abstraction layer offers logical analog channels,
 *      which can be inputs or outputs and can use processor internal
 *      converters or external converters. PWM units are treated as
 *      analog outputs as well.
 *  
 *      Following parameters must be configured for each logical port:
 * 
 *      - Logical channel identifier (channel id)
 *      - Interface (intern, serial, I2C, SPI)
 *      - Data direction (input, output)
 *      - Converter resolution (number of bits)
 *      - Physical channel number 
 *      - Scaling facor (value for full scale)
 *      - Offset value
 *      - Initial output value
 *
 ****************************************************************************/

const CHANNEL_DESCRIPTOR_t halAnalogChannelTable[] = {    
    { HAL_VOLTAGE_INPUT,    PORT_INPUT,  PORT_TYPE_INTERN, 12, 0, 3300, 0, 0 },
    { HAL_CURRENT_INPUT,    PORT_INPUT,  PORT_TYPE_INTERN, 12, 1, 1000, 0, 0 },
    { HAL_ANALOG_INPUT_0,   PORT_INPUT,  PORT_TYPE_INTERN, 12, 2, 5000, 0, 0 },
    { HAL_ANALOG_INPUT_1,   PORT_INPUT,  PORT_TYPE_INTERN, 12, 3, 3000, 0, 0 },
    { HAL_ANALOG_INPUT_2,   PORT_INPUT,  PORT_TYPE_INTERN, 12, 4, 2000, 0, 0 },
    { HAL_ANALOG_INPUT_3,   PORT_INPUT,  PORT_TYPE_INTERN, 12, 5, 1000, 0, 0 },
    { HAL_ANALOG_INPUT_4,   PORT_INPUT,  PORT_TYPE_INTERN, 12, 6, 1000, 0, 0 }
};
const int halAnalogChannelTableLength = ELEMENTS(halAnalogChannelTable);


/*****************************************************************************/
/*! 
 *  \brief   Processor pin descriptor table
 *
 *      The pin descriptor table must contain one entry for each processor
 *      pin to configure. The following informations must be specified for
 *      each single pin:
 * 
 *      - Physical port number 
 *      - Physical port bit number
 *      - Pin multiplexer setting
 *      - Pin input qualification type select
 *      - Pin input pullup enable/disable
 *      - External interrupt enable and active edge selection
 *
 *      The GPIOxx_MUX constants used in the table below are defined in 
 *      halConfigure.h and should be adopted there. The other constants
 *      are defined in halCommon.h (this file should not be changed).
 * 
 ****************************************************************************/

const PIN_DEFINITION_t halPinDescriptorTable[] = {
    { 0,  0, GPIO00_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0,  1, GPIO01_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0,  2, GPIO02_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0,  3, GPIO03_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0,  4, GPIO04_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0,  5, GPIO05_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0,  6, GPIO06_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0,  7, GPIO07_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    
    { 0,  8, GPIO08_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, XINTERRUPT_1 },
    { 0,  9, GPIO09_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 10, GPIO10_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 11, GPIO11_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 12, GPIO12_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 13, GPIO13_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 14, GPIO14_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 15, GPIO15_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    
    { 0, 16, GPIO16_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 17, GPIO17_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 18, GPIO18_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 19, GPIO19_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 20, GPIO20_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 21, GPIO21_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 22, GPIO22_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 23, GPIO23_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    
    { 0, 24, GPIO24_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 25, GPIO25_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 26, GPIO26_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 27, GPIO27_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 28, GPIO28_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 29, GPIO29_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 30, GPIO30_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 0, 31, GPIO31_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    
    { 1,  0, GPIO32_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1,  1, GPIO33_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1,  2, GPIO34_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    
#ifdef PROCESSOR_2823x
    { 1,  3, GPIO35_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1,  4, GPIO36_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1,  5, GPIO37_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1,  6, GPIO38_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1,  7, GPIO39_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },

    { 1,  8, GPIO40_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1,  9, GPIO41_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 10, GPIO42_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 11, GPIO43_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 12, GPIO44_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 13, GPIO45_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 14, GPIO46_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 15, GPIO47_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },

    { 1, 16, GPIO48_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 17, GPIO49_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 18, GPIO50_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 19, GPIO51_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 20, GPIO52_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 21, GPIO53_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 22, GPIO54_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 23, GPIO55_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },

    { 1, 24, GPIO56_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 25, GPIO57_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 26, GPIO58_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 27, GPIO59_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 28, GPIO60_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 29, GPIO61_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 30, GPIO62_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 1, 31, GPIO63_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },

    { 2,  0, GPIO64_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2,  1, GPIO65_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2,  2, GPIO66_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2,  3, GPIO67_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2,  4, GPIO68_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2,  5, GPIO69_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2,  6, GPIO70_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2,  7, GPIO71_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },

    { 2,  8, GPIO72_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2,  9, GPIO73_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 10, GPIO74_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 11, GPIO75_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 12, GPIO76_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 13, GPIO77_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 14, GPIO78_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 15, GPIO79_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },

    { 2, 16, GPIO80_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 17, GPIO81_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 18, GPIO82_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 19, GPIO83_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 20, GPIO84_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 21, GPIO85_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 22, GPIO86_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT },
    { 2, 23, GPIO87_MUX, INPUT_QUAL_SYSCLK, PULLUP_ENABLED, NO_INTERRUPT }
#endif    
};
const int halPinDescriptorTableLength = ELEMENTS(halPinDescriptorTable);


/*****************************************************************************/
/*! 
 *  \brief   Input qualification sampling period table
 *
 *      The processor has the capability to filter (qualify) each input for
 *      higher noise immunity. Filtering means, that an input value must be
 *      stable for a minimum number of sample periods before it is accepted
 *      as new input level. Qualification can be enabled on a per pin base
 *      (see pin descriptor table). The number of samples on the other hand
 *      can not be programmed on a per pin base, but common for a group of
 *      8 bits. 
 * 
 *      The pin sample period table is used to configure sample rates for
 *      the 8 bit (byte) pin groups. The following informations must be 
 *      specified for each single port byte (each port contains 4 bytes):
 * 
 *      - Physical port number (0...3)
 *      - Physical port byte number (0...3)
 *      - Sample rate (0...255)
 *
 *      The GPIO_QUALx constants used in the table below are defined in 
 *      halConfigure.h and should be adopted there. 
 * 
 ****************************************************************************/

const PIN_QUALIFICATION_t halPinSamplePeriodTable[] = {
    { 0, 0, GPIO_QUAL0  },   //!< Sampling period for GPIO.00 to GPIO.07 
    { 0, 1, GPIO_QUAL8  },   //!< Sampling period for GPIO.08 to GPIO.15 
    { 0, 2, GPIO_QUAL16 },   //!< Sampling period for GPIO.16 to GPIO.23 
    { 0, 3, GPIO_QUAL24 },   //!< Sampling period for GPIO.24 to GPIO.31 
    { 1, 0, GPIO_QUAL32 },   //!< Sampling period for GPIO.32 to GPIO.39
#ifdef PROCESSOR_2823x
    { 1, 1, GPIO_QUAL40 },   //!< Sampling period for GPIO.40 to GPIO.47
    { 1, 2, GPIO_QUAL48 },   //!< Sampling period for GPIO.48 to GPIO.55
    { 1, 3, GPIO_QUAL56 }    //!< Sampling period for GPIO.56 to GPIO.63
#endif    
};
const int halPinSamplePeriodTableLength = ELEMENTS(halPinSamplePeriodTable);


//****************************************************************************/

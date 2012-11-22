/*******************************************************************************/
/*! \file halDescriptor.c
 *
 *  \brief  HAL configuration descriptors
 *
 *  $Version: $ 0.1
 *  $Date:    $ 26.07.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      The HAL is configured to hardware by descriptor tables for the various 
 *      classes of hardware devices. The descriptor tables are defined in the 
 *      HAL configuration file linked in when the firmware for a certain hard-
 *      ware board gets build.
 *
 *      This module contain weak declarations of those tables to be used, if
 *      one or multiple descriptor tables are not defined in the configuration
 *      file, due to the fact, that a hardware class is not present on a
 *      certain hardware board.
 *
 *      The descriptors declared here are all empty, indicating the HAL that
 *      a ressource is not available.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/*******************************************************************************/

#include "Global.h"
#include "halCommon.h"

#ifndef _lint      // Lint can't handle weak declarations

/*******************************************************************************/
// Processor Frequency Settings
/*******************************************************************************/

__weak const UInt32 halSystemFrequency   = 0;  //!< System clock frequency
__weak const UInt32 halCrystalFrequency  = 0;  //!< Processor crystal frequency
__weak const UInt32 halRtcClockFrequency = 0;  //!< Real time crystal frequency

__weak const UInt32 halProcessorVoltage  = 0;  //!< Nominal processor voltage
__weak const UInt32 halWatchdogInterval  = 0;  //!< Watchdog interval time

__weak const UInt32 halNonVolatileSize   = 0;  //!< Non volatile memory size
__weak const UInt32 halNonVolatileCache  = 0;  //!< Non volatile memory cache size



/*******************************************************************************/
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
 *******************************************************************************/

__weak const halPinDesciptor_t halPinDescriptors[] = {0};

//! Processor hardware pin descriptor table size
__weak const UInt32 halPinDescriptorCount = 0;



/*******************************************************************************/
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
 *******************************************************************************/

__weak const halPortDescriptor_t halPortDescriptors[] = {0};

//! Digital input/output port descriptor table size
__weak const UInt32 halPortDescriptorCount = 0;



/*******************************************************************************/
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
 *******************************************************************************/

__weak const halAnalogDescriptor_t halAnalogDescriptors[] = {0};

//! Analog input/output channel descriptor table size
__weak const UInt32 halAnalogDescriptorCount = 0;



/*******************************************************************************/
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
 *******************************************************************************/

__weak const halTimerDescriptor_t halTimerDescriptors[] = {0};

//! Timer descriptor table size
__weak const UInt32 halTimerDescriptorCount = 0;



/*******************************************************************************/
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
 *******************************************************************************/

__weak const halStepperDescriptor_t halStepperDescriptors[] = {0};

//! Stepper motor descriptor table size
__weak const UInt32 halStepperDescriptorCount = 0;



/*******************************************************************************/
/*!
 *  \brief   External interrupts descriptor table
 *
 *      The external interrupt descriptor table must contain one entry for each
 *      external interrupt of the processor. The following informations must be
 *      specified for each interrupt:
 *
 *      - Logical interrupt channel ID
 *      - Physical port number
 *      - Physical port bit number (or EXTINT interrupt line)
 *      - Interrupt priority
 *      - Signal edge sensitivity
 *
 *      The mapping of port bits to the processor internal EXTINT lines are
 *      fixed, therefore the physical port bit also selects the EXTINT line.
 *
 *******************************************************************************/

__weak const halExtIntrDescriptor_t halExtIntrDescriptors[] = {0};

//! External interrupt input descriptor table size
__weak const UInt32 halExtIntrDescriptorCount = 0;


/*******************************************************************************/
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
 *******************************************************************************/

__weak const halStorageDescriptor_t halStorageDescriptors[] = {0};

//! Storage descriptor table size
__weak const UInt32 halStorageDescriptorCount = 0;



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

__weak const halSerialDescriptor_t halSerialDescriptors[] = {0};

//! Serial bus (I2C, SPI) descriptor table size
__weak const UInt32 halSerialDescriptorCount = 0;



/*******************************************************************************/
/*!
 *  \brief   AD779x analog channel descriptor table
 *
 *      Beside the processor internal analog inputs the HAL also supports analog
 *      front-ends out of the AD779x family of sigma delta converters. Theses
 *      devices provides up to 6 analog input channels on a single chip. This
 *      descriptor is dedicated to configure a AD779x converter chip. The
 *      following parameters must be configured for each input channel:
 *
 *      - Logical channel number
 *      - Scan priority (relative frequency)
 *      - I/O register setting
 *      - Mode register setting
 *      - Configuration register setting
 *
 *      The logical channel number is not necessarily equal to the physical
 *      channel number of the chip. The physical channel number is coded in
 *      the configuration register setting. 
 *
 *      The mode and configuration register settings are individual for each
 *      input channel, while the I/O register setting are channel independant.
 *      The I/O setting is therefore taken from the first entry of the table.
 *
 *      The input channels must additionally be configured in the analog
 *      channel descriptor table (halAnalogChannelTable), using the logical
 *      channel number from this table as "physical" channel number.
 *
 *******************************************************************************/

__weak const halAd779xDescriptor_t halAd779xDescriptors[] = {0};

//! External analog frontend AD779x descriptor table size
__weak const UInt32 halAd779xDescriptorCount = 0;



/*******************************************************************************/
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
 *******************************************************************************/

__weak const halPinMapping_t halPinMappings = {0};


/*******************************************************************************/
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
 *******************************************************************************/

__weak const halProcessorInfo_t halProcessorInfo = {0};


/*******************************************************************************/

#endif

/****************************************************************************/
/*! \file halProcessor.h
 *
 *  \brief  Parameters of all supported processors
 *
 *  $Version: $ 0.2
 *  $Date:    $ 17.05.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This header file contains all defines describing the peripherals
 *      and memory sizes of all processors supported by the HAL. Each
 *      processor of the STM32 family is available in different sized
 *      packages having different pin counts. The parameter sets defined
 *      in this file are related to the chip itself, not the package.
 *      It's up to the user not to use i/o's that are not bonded out
 *      if using a lower pin count package.
 *
 *      The PROCESSOR define must be assign one of the processors from
 *      the list of supported processors before including this header.
 *      Depending on the selected processor a set of parameters are
 *      defined, describing the capabilities of the processor.
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


#ifndef HAL_PROCESSOR_H
#define HAL_PROCESSOR_H


//****************************************************************************/
// Supported Processors  (must be assigned to PROCESSOR define)
//****************************************************************************/

// Value Line Processors (Small to High Density)
#define STM32F100x4   1    //!< 24MHz, 16KB Flash, 4KB RAM, Low Density
#define STM32F100x6   2    //!< 24MHz, 32KB Flash, 4KB RAM, Low Density
#define STM32F100x8   3    //!< 24MHz, 64KB Flash, 8KB RAM, Medium Density
#define STM32F100xB   4    //!< 24MHz, 128KB Flash, 8KB RAM, Medium Density
#define STM32F100xC   5    //!< 24MHz, 256KB Flash, 24KB RAM, High Density
#define STM32F100xD   6    //!< 24MHz, 384KB Flash, 32KB RAM, High Density
#define STM32F100xE   7    //!< 24MHz, 512KB Flash, 32KB RAM, High Density

// Access Line Processors (Small to XL Density)
#define STM32F101x4   10   //!< 36MHz, 16KB Flash, 4KB RAM, Low Density
#define STM32F101x6   11   //!< 36MHz, 32KB Flash, 6KB RAM, Low Density
#define STM32F101x8   12   //!< 36MHz, 64KB Flash, 10KB RAM, Medium Density
#define STM32F101xB   13   //!< 36MHz, 128KB Flash, 16KB RAM, Medium Density
#define STM32F101xC   14   //!< 36MHz, 256KB Flash, 32KB RAM, High Density
#define STM32F101xD   15   //!< 36MHz, 384KB Flash, 48KB RAM, High Density
#define STM32F101xE   16   //!< 36MHz, 512KB Flash, 48KB RAM, High Density
#define STM32F101xF   17   //!< 36MHz, 768KB Flash, 80KB RAM, XL Density
#define STM32F101xG   18   //!< 36MHz, 1024kB Flash, 80KB RAM XL Density

// Access Line + USB Processors (Small to Medium Density)
#define STM32F102x4   20   //!< 48MHz, 16KB Flash, 4KB RAM, Low Density
#define STM32F102x6   21   //!< 48MHz, 32KB Flash, 6KB RAM, Low Density
#define STM32F102x8   22   //!< 48MHz, 64KB Flash, 10KB RAM, Medium Density
#define STM32F102xB   23   //!< 48MHz, 128KB Flash, 16KB RAM, Medium Density

// Performance Line Processors (Small to XL Density)
#define STM32F103x4   30   //!< 72MHz, 16KB Flash, 6KB RAM, Low Density
#define STM32F103x6   31   //!< 72MHz, 32KB Flash, 10KB RAM, Low Density
#define STM32F103x8   32   //!< 72MHz, 64KB Flash, 20KB RAM, Medium Density
#define STM32F103xB   33   //!< 72MHz, 128KB Flash, 20KB RAM, Medium Density
#define STM32F103xC   34   //!< 72MHz, 256KB Flash, 48KB RAM, High Density
#define STM32F103xD   35   //!< 72MHz, 384KB Flash, 64KB RAM, High Density
#define STM32F103xE   36   //!< 72MHz, 512KB Flash, 64KB RAM, High Density
#define STM32F103xF   37   //!< 72MHz, 768KB Flash, 96KB RAM, XL Density
#define STM32F103xG   38   //!< 72MHz, 1024KB Flash, 96KB RAM, XL Density

// Connectivity Line Processors (with and w/o Ethernet)
#define STM32F105x8   40   //!< 72MHz, 64KB Flash, 20KB RAM, Medium Density
#define STM32F105xB   41   //!< 72MHz, 128KB Flash, 32KB RAM, Medium Density
#define STM32F105xC   42   //!< 72MHz, 256KB Flash, 64KB RAM, High Density
#define STM32F107xB   43   //!< 72MHz, 128KB Flash, 48KB RAM, Ethernet
#define STM32F107xC   44   //!< 72MHz, 256KB Flash, 64KB RAM, Ethernet


//****************************************************************************/
// Parameters for the supported Processors
//****************************************************************************/

#if PROCESSOR == STM32F100x4
#define DEVICE_FAMILY      DEVICE_FAMILY_F100
#define DEVICE_DENSITY     DEVICE_DENSITY_LOW
#define SIZEOF_FLASH       16
#define SIZEOF_RAM         4

#elif PROCESSOR == STM32F100x6
#define DEVICE_FAMILY      DEVICE_FAMILY_F100
#define DEVICE_DENSITY     DEVICE_DENSITY_LOW
#define SIZEOF_FLASH       32
#define SIZEOF_RAM         4

#elif PROCESSOR == STM32F100x8
#define DEVICE_FAMILY      DEVICE_FAMILY_F100
#define DEVICE_DENSITY     DEVICE_DENSITY_MEDIUM
#define SIZEOF_FLASH       64
#define SIZEOF_RAM         8

#elif PROCESSOR == STM32F100xB
#define DEVICE_FAMILY      DEVICE_FAMILY_F100
#define DEVICE_DENSITY     DEVICE_DENSITY_MEDIUM
#define SIZEOF_FLASH       128
#define SIZEOF_RAM         8

#elif PROCESSOR == STM32F100xC
#define DEVICE_FAMILY      DEVICE_FAMILY_F100
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       256
#define SIZEOF_RAM         24

#elif PROCESSOR == STM32F100xD
#define DEVICE_FAMILY      DEVICE_FAMILY_F100
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       384
#define SIZEOF_RAM         32

#elif PROCESSOR == STM32F100xE
#define DEVICE_FAMILY      DEVICE_FAMILY_F100
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       512
#define SIZEOF_RAM         32

#elif PROCESSOR == STM32F101x4
#define DEVICE_FAMILY      DEVICE_FAMILY_F101
#define DEVICE_DENSITY     DEVICE_DENSITY_LOW
#define SIZEOF_FLASH       16
#define SIZEOF_RAM         4

#elif PROCESSOR == STM32F101x6
#define DEVICE_FAMILY      DEVICE_FAMILY_F101
#define DEVICE_DENSITY     DEVICE_DENSITY_LOW
#define SIZEOF_FLASH       32
#define SIZEOF_RAM         6

#elif PROCESSOR == STM32F101x8
#define DEVICE_FAMILY      DEVICE_FAMILY_F101
#define DEVICE_DENSITY     DEVICE_DENSITY_MEDIUM
#define SIZEOF_FLASH       64
#define SIZEOF_RAM         10

#elif PROCESSOR == STM32F101xB
#define DEVICE_FAMILY      DEVICE_FAMILY_F101
#define DEVICE_DENSITY     DEVICE_DENSITY_MEDIUM
#define SIZEOF_FLASH       128
#define SIZEOF_RAM         16

#elif PROCESSOR == STM32F101xC
#define DEVICE_FAMILY      DEVICE_FAMILY_F101
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       256
#define SIZEOF_RAM         32

#elif PROCESSOR == STM32F101xD
#define DEVICE_FAMILY      DEVICE_FAMILY_F101
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       384
#define SIZEOF_RAM         48

#elif PROCESSOR == STM32F101xE
#define DEVICE_FAMILY      DEVICE_FAMILY_F101
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       512
#define SIZEOF_RAM         48

#elif PROCESSOR == STM32F101xF
#define DEVICE_FAMILY      DEVICE_FAMILY_F101
#define DEVICE_DENSITY     DEVICE_DENSITY_XL
#define SIZEOF_FLASH       768
#define SIZEOF_RAM         80

#elif PROCESSOR == STM32F101xG
#define DEVICE_FAMILY      DEVICE_FAMILY_F101
#define DEVICE_DENSITY     DEVICE_DENSITY_XL
#define SIZEOF_FLASH       1024
#define SIZEOF_RAM         80

#elif PROCESSOR == STM32F102x4
#define DEVICE_FAMILY      DEVICE_FAMILY_F102
#define DEVICE_DENSITY     DEVICE_DENSITY_LOW
#define SIZEOF_FLASH       16
#define SIZEOF_RAM         4

#elif PROCESSOR == STM32F102x6
#define DEVICE_FAMILY      DEVICE_FAMILY_F102
#define DEVICE_DENSITY     DEVICE_DENSITY_LOW
#define SIZEOF_FLASH       32
#define SIZEOF_RAM         6

#elif PROCESSOR == STM32F102x8
#define DEVICE_FAMILY      DEVICE_FAMILY_F102
#define DEVICE_DENSITY     DEVICE_DENSITY_MEDIUM
#define SIZEOF_FLASH       64
#define SIZEOF_RAM         10

#elif PROCESSOR == STM32F102xB
#define DEVICE_FAMILY      DEVICE_FAMILY_F102
#define DEVICE_DENSITY     DEVICE_DENSITY_MEDIUM
#define SIZEOF_FLASH       128
#define SIZEOF_RAM         16

#elif PROCESSOR == STM32F103x4
#define DEVICE_FAMILY      DEVICE_FAMILY_F103
#define DEVICE_DENSITY     DEVICE_DENSITY_LOW
#define SIZEOF_FLASH       16
#define SIZEOF_RAM         6

#elif PROCESSOR == STM32F103x6
#define DEVICE_FAMILY      DEVICE_FAMILY_F103
#define DEVICE_DENSITY     DEVICE_DENSITY_LOW
#define SIZEOF_FLASH       32
#define SIZEOF_RAM         10

#elif PROCESSOR == STM32F103x8
#define DEVICE_FAMILY      DEVICE_FAMILY_F103
#define DEVICE_DENSITY     DEVICE_DENSITY_MEDIUM
#define SIZEOF_FLASH       64
#define SIZEOF_RAM         20

#elif PROCESSOR == STM32F103xB
#define DEVICE_FAMILY      DEVICE_FAMILY_F103
#define DEVICE_DENSITY     DEVICE_DENSITY_MEDIUM
#define SIZEOF_FLASH       128
#define SIZEOF_RAM         20

#elif PROCESSOR == STM32F103xC
#define DEVICE_FAMILY      DEVICE_FAMILY_F103
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       256
#define SIZEOF_RAM         48

#elif PROCESSOR == STM32F103xD
#define DEVICE_FAMILY      DEVICE_FAMILY_F103
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       384
#define SIZEOF_RAM         64

#elif PROCESSOR == STM32F103xE
#define DEVICE_FAMILY      DEVICE_FAMILY_F103
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       512
#define SIZEOF_RAM         64

#elif PROCESSOR == STM32F103xF
#define DEVICE_FAMILY      DEVICE_FAMILY_F103
#define DEVICE_DENSITY     DEVICE_DENSITY_XL
#define SIZEOF_FLASH       768
#define SIZEOF_RAM         96

#elif PROCESSOR == STM32F103xG
#define DEVICE_FAMILY      DEVICE_FAMILY_F103
#define DEVICE_DENSITY     DEVICE_DENSITY_XL
#define SIZEOF_FLASH       1024
#define SIZEOF_RAM         96

#elif PROCESSOR == STM32F105x8
#define DEVICE_FAMILY      DEVICE_FAMILY_F105
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       64
#define SIZEOF_RAM         20

#elif PROCESSOR == STM32F105xB
#define DEVICE_FAMILY      DEVICE_FAMILY_F105
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       128
#define SIZEOF_RAM         32

#elif PROCESSOR == STM32F105xC
#define DEVICE_FAMILY      DEVICE_FAMILY_F105
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       256
#define SIZEOF_RAM         64

#elif PROCESSOR == STM32F107xB
#define DEVICE_FAMILY      DEVICE_FAMILY_F105
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       128
#define SIZEOF_RAM         48

#elif PROCESSOR == STM32F107xC
#define DEVICE_FAMILY      DEVICE_FAMILY_F105
#define DEVICE_DENSITY     DEVICE_DENSITY_HIGH
#define SIZEOF_FLASH       256
#define SIZEOF_RAM         64

#else
#error "Undefined or unknown processor type! Please check PROCESSOR define"
#endif

//! Macro to build a processor ID out of family and density
#define PROCESSOR_ID(f,d)  (((f) << 8) + (d))

//! Build Device ID of selected processor
#define PROCESSOR_TYPE     PROCESSOR_ID(DEVICE_FAMILY, DEVICE_DENSITY)

//****************************************************************************/
// Number of Peripherals for the supported Processor Classes
//****************************************************************************/

#if PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F100, DEVICE_DENSITY_LOW)
#define DEVICE_SYSCLOCK    24      //!< Processor system clock
#define NUMBER_OF_GPIOs    51      //!< Number of general purpose i/o's
#define NUMBER_OF_TIMERS   8       //!< Number of timers
#define NUMBER_OF_CANs     0       //!< Number of CAN controllers
#define NUMBER_OF_USBs     0       //!< Number of USB controllers
#define NUMBER_OF_DMAs     7       //!< Number of DMA channels
#define NUMBER_OF_UARTs    2       //!< Number of UART units
#define NUMBER_OF_SPIs     1       //!< Number of SPI interfaced
#define NUMBER_OF_I2Cs     1       //!< Number of I2C interfaced
#define NUMBER_OF_ADCs     16      //!< Number of ADC channels
#define NUMBER_OF_DACs     2       //!< Number of DAC channels
#define TIMERS_BITMASK     0xC067  //!< Bitmap of timers

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F100, DEVICE_DENSITY_MEDIUM)
#define DEVICE_SYSCLOCK    24
#define NUMBER_OF_GPIOs    80
#define NUMBER_OF_TIMERS   9
#define NUMBER_OF_CANs     0
#define NUMBER_OF_USBs     0
#define NUMBER_OF_DMAs     7
#define NUMBER_OF_UARTs    3
#define NUMBER_OF_SPIs     2
#define NUMBER_OF_I2Cs     2
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     2
#define TIMERS_BITMASK     0xC06F

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F100, DEVICE_DENSITY_HIGH)
#define DEVICE_SYSCLOCK    24
#define NUMBER_OF_GPIOs    112
#define NUMBER_OF_TIMERS   13
#define NUMBER_OF_CANs     0
#define NUMBER_OF_USBs     0
#define NUMBER_OF_DMAs     12
#define NUMBER_OF_UARTs    5
#define NUMBER_OF_SPIs     3
#define NUMBER_OF_I2Cs     2
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     2
#define TIMERS_BITMASK     0xF87F

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F101, DEVICE_DENSITY_LOW)
#define DEVICE_SYSCLOCK    36
#define NUMBER_OF_GPIOs    52
#define NUMBER_OF_TIMERS   2
#define NUMBER_OF_CANs     0
#define NUMBER_OF_USBs     0
#define NUMBER_OF_DMAs     7
#define NUMBER_OF_UARTs    2
#define NUMBER_OF_SPIs     1
#define NUMBER_OF_I2Cs     1
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     0
#define TIMERS_BITMASK     0x0006

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F101, DEVICE_DENSITY_MEDIUM)
#define DEVICE_SYSCLOCK    36
#define NUMBER_OF_GPIOs    80
#define NUMBER_OF_TIMERS   3
#define NUMBER_OF_CANs     0
#define NUMBER_OF_USBs     0
#define NUMBER_OF_DMAs     7
#define NUMBER_OF_UARTs    3
#define NUMBER_OF_SPIs     2
#define NUMBER_OF_I2Cs     2
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     0
#define TIMERS_BITMASK     0x000E

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F101, DEVICE_DENSITY_HIGH)
#define DEVICE_SYSCLOCK    36
#define NUMBER_OF_GPIOs    112
#define NUMBER_OF_TIMERS   6
#define NUMBER_OF_CANs     0
#define NUMBER_OF_USBs     0
#define NUMBER_OF_DMAs     12
#define NUMBER_OF_UARTs    5
#define NUMBER_OF_SPIs     3
#define NUMBER_OF_I2Cs     2
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     2
#define TIMERS_BITMASK     0x007E

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F101, DEVICE_DENSITY_XL)
#define DEVICE_SYSCLOCK    36
#define NUMBER_OF_GPIOs    112
#define NUMBER_OF_TIMERS   12
#define NUMBER_OF_CANs     0
#define NUMBER_OF_USBs     0
#define NUMBER_OF_DMAs     12
#define NUMBER_OF_UARTs    5
#define NUMBER_OF_SPIs     3
#define NUMBER_OF_I2Cs     2
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     2
#define TIMERS_BITMASK     0x3F7E

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F102, DEVICE_DENSITY_LOW)
#define DEVICE_SYSCLOCK    48
#define NUMBER_OF_GPIOs    51
#define NUMBER_OF_TIMERS   2
#define NUMBER_OF_CANs     0
#define NUMBER_OF_USBs     1
#define NUMBER_OF_DMAs     7
#define NUMBER_OF_UARTs    2
#define NUMBER_OF_SPIs     1
#define NUMBER_OF_I2Cs     1
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     0
#define TIMERS_BITMASK     0x0006

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F102, DEVICE_DENSITY_MEDIUM)
#define DEVICE_SYSCLOCK    48
#define NUMBER_OF_GPIOs    51
#define NUMBER_OF_TIMERS   3
#define NUMBER_OF_CANs     0
#define NUMBER_OF_USBs     1
#define NUMBER_OF_DMAs     7
#define NUMBER_OF_UARTs    3
#define NUMBER_OF_SPIs     2
#define NUMBER_OF_I2Cs     2
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     0
#define TIMERS_BITMASK     0x000E

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F103, DEVICE_DENSITY_LOW)
#define DEVICE_SYSCLOCK    72
#define NUMBER_OF_GPIOs    51
#define NUMBER_OF_TIMERS   3
#define NUMBER_OF_CANs     1
#define NUMBER_OF_USBs     1
#define NUMBER_OF_DMAs     7
#define NUMBER_OF_UARTs    2
#define NUMBER_OF_SPIs     1
#define NUMBER_OF_I2Cs     1
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     0
#define TIMERS_BITMASK     0x0007

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F103, DEVICE_DENSITY_MEDIUM)
#define DEVICE_SYSCLOCK    72
#define NUMBER_OF_GPIOs    80
#define NUMBER_OF_TIMERS   4
#define NUMBER_OF_CANs     1
#define NUMBER_OF_USBs     1
#define NUMBER_OF_DMAs     7
#define NUMBER_OF_UARTs    3
#define NUMBER_OF_SPIs     2
#define NUMBER_OF_I2Cs     2
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     0
#define TIMERS_BITMASK     0x000F

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F103, DEVICE_DENSITY_HIGH)
#define DEVICE_SYSCLOCK    72
#define NUMBER_OF_GPIOs    112
#define NUMBER_OF_TIMERS   8
#define NUMBER_OF_CANs     1
#define NUMBER_OF_USBs     1
#define NUMBER_OF_DMAs     12
#define NUMBER_OF_UARTs    5
#define NUMBER_OF_SPIs     3
#define NUMBER_OF_I2Cs     2
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     2
#define TIMERS_BITMASK     0x00FF

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F103, DEVICE_DENSITY_XL)
#define DEVICE_SYSCLOCK    72
#define NUMBER_OF_GPIOs    112
#define NUMBER_OF_TIMERS   14
#define NUMBER_OF_CANs     1
#define NUMBER_OF_USBs     1
#define NUMBER_OF_DMAs     12
#define NUMBER_OF_UARTs    5
#define NUMBER_OF_SPIs     3
#define NUMBER_OF_I2Cs     2
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     2
#define TIMERS_BITMASK     0x3FFF

#elif PROCESSOR_TYPE == PROCESSOR_ID(DEVICE_FAMILY_F105, DEVICE_DENSITY_HIGH)
#define DEVICE_SYSCLOCK    72
#define NUMBER_OF_GPIOs    80
#define NUMBER_OF_TIMERS   7
#define NUMBER_OF_CANs     2
#define NUMBER_OF_USBs     1
#define NUMBER_OF_DMAs     12
#define NUMBER_OF_UARTs    5
#define NUMBER_OF_SPIs     3
#define NUMBER_OF_I2Cs     1
#define NUMBER_OF_ADCs     16
#define NUMBER_OF_DACs     2
#define TIMERS_BITMASK     0x007F
#endif

//! Number of physical GPIO ports (16 bits wide)
#define NUMBER_OF_PORTs    ((NUMBER_OF_GPIOs + 15) / 16)

//****************************************************************************/

#endif

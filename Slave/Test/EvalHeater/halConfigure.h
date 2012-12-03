/****************************************************************************/
/*! \file halConfigure.h
 * 
 *  \brief  HAL configuration header file
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
 *       This file works in conjunction with the halConfigure.c source
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

#ifndef HAL_CONFIGURE_H
#define HAL_CONFIGURE_H

#define PROCESSOR_2823x         // Used processor

//****************************************************************************/
// Board specific settings 
//****************************************************************************/

#define CRYSTAL_FREQUENCY       30000000L

#define PLL_CLOCK_MULIPLIER     10   //!< VCO: 30MHz * 10 = 300 MHz
#define PLL_CLOCK_DIVIDER       2   //!< CLK: 210MZh / 2 = 150 MHz

#define HiSPEED_CLOCK_DIVIDER   1   //!< HSCLK = 150 MHz 
#define LoSPEED_CLOCK_DIVIDER   2   //!< LSCLK = 75 MHz

#define SYSTEM_CLOCK         \
                (CRYSTAL_FREQUENCY * PLL_CLOCK_MULIPLIER / PLL_CLOCK_DIVIDER)

//****************************************************************************/
// Logical Port ID Definition
//****************************************************************************/

// Hardware timers (system timer is reserved for base module)
#define HAL_TIMER1              CHANNELID(CHANNEL_CLASS_TIMER, 0)
#define HAL_TIMER2              CHANNELID(CHANNEL_CLASS_TIMER, 1)
#define HAL_TIMER_SYSTEM        CHANNELID(CHANNEL_CLASS_TIMER, 2)

// Channels used by base module
#define HAL_PORT_NODE_INDEX     CHANNELID(CHANNEL_CLASS_DIGIO, 0) 
#define HAL_LED1_OUTPUT         CHANNELID(CHANNEL_CLASS_DIGIO, 1) 
#define HAL_LED2_OUTPUT         CHANNELID(CHANNEL_CLASS_DIGIO, 2) 

#define HAL_VOLTAGE_INPUT       CHANNELID(CHANNEL_CLASS_ANALOG, 0)
#define HAL_CURRENT_INPUT       CHANNELID(CHANNEL_CLASS_ANALOG, 1)

#define HAL_STORAGE_FRAM        CHANNELID(CHANNEL_CLASS_STORAGE, 0)
#define HAL_STORAGE_OTP         CHANNELID(CHANNEL_CLASS_STORAGE, 1)
#define HAL_STORAGE_FLASH       CHANNELID(CHANNEL_CLASS_STORAGE, 2)

#define HAL_SERIAL_PORT         CHANNELID(CHANNEL_CLASS_SERIAL, 0)

// Channels used by digtal output function module
#define HAL_DIGITAL_OUTPUTS     CHANNELID(CHANNEL_CLASS_DIGIO, 3) 
#define HAL_DIGITAL_OUTPUT_0    HAL_DIGITAL_OUTPUTS
#define HAL_DIGITAL_OUTPUT_1    HAL_DIGITAL_OUTPUTS+1
#define HAL_DIGITAL_OUTPUT_2    HAL_DIGITAL_OUTPUTS+2
#define HAL_DIGITAL_OUTPUT_3    HAL_DIGITAL_OUTPUTS+3
#define HAL_DIGITAL_OUTPUT_4    HAL_DIGITAL_OUTPUTS+4

// Channels used by digtal input function module
#define HAL_DIGITAL_INPUTS      CHANNELID(CHANNEL_CLASS_DIGIO, 8) 
#define HAL_DIGITAL_INPUT_0     HAL_DIGITAL_INPUTS
#define HAL_DIGITAL_INPUT_1     HAL_DIGITAL_INPUTS+1
#define HAL_DIGITAL_INPUT_2     HAL_DIGITAL_INPUTS+2
#define HAL_DIGITAL_INPUT_3     HAL_DIGITAL_INPUTS+3
#define HAL_DIGITAL_INPUT_4     HAL_DIGITAL_INPUTS+4

// Channels used by analog output function module
#define HAL_ANALOG_OUTPUTS      CHANNELID(CHANNEL_CLASS_ANALOG, 2) 
#define HAL_ANALOG_OUTPUT_0     HAL_ANALOG_OUTPUTS
#define HAL_ANALOG_OUTPUT_1     HAL_ANALOG_OUTPUTS+1
#define HAL_ANALOG_OUTPUT_2     HAL_ANALOG_OUTPUTS+2
#define HAL_ANALOG_OUTPUT_3     HAL_ANALOG_OUTPUTS+3
#define HAL_ANALOG_OUTPUT_4     HAL_ANALOG_OUTPUTS+4

// Channels used by analog input function module
#define HAL_ANALOG_INPUTS       CHANNELID(CHANNEL_CLASS_ANALOG, 7) 
#define HAL_ANALOG_INPUT_0      HAL_ANALOG_INPUTS
#define HAL_ANALOG_INPUT_1      HAL_ANALOG_INPUTS+1
#define HAL_ANALOG_INPUT_2      HAL_ANALOG_INPUTS+2
#define HAL_ANALOG_INPUT_3      HAL_ANALOG_INPUTS+3
#define HAL_ANALOG_INPUT_4      HAL_ANALOG_INPUTS+4

// Channels used by stepper motor function module
#define HAL_ENCODER1            CHANNELID(CHANNEL_CLASS_ENCODE, 0)
#define HAL_ENCODER2            HAL_ENCODER1+1

#define HAL_CAN_SYSTEM_BUS      0


//****************************************************************************/
// Processor Pin Definition (must be adopted for each board)
//****************************************************************************/

#ifdef  PROCESSOR_280x   // Mapping for 280x Processors

#define GPIO00_MUX   0   // 0=GPIO0,  1=EPWM1A,  2=Reserved, 3=Reserved
#define GPIO01_MUX   0   // 0=GPIO1,  1=EPWM1B,  2=SPISIMOD, 3=Reserved
#define GPIO02_MUX   0   // 0=GPIO2,  1=EPWM2A,  2=Reserved, 3=Reserved
#define GPIO03_MUX   0   // 0=GPIO3,  1=EPWM2B,  2=SPISOMID, 3=Reserved
#define GPIO04_MUX   0   // 0=GPIO4,  1=EPWM3A,  2=Reserved, 3=Reserved
#define GPIO05_MUX   0   // 0=GPIO5,  1=EPWM3B,  2=SPICLKD,  3=ECAP1
#define GPIO06_MUX   0   // 0=GPIO6,  1=EPWM4A,  2=EPWMSYNI, 3=EPWMSYNO
#define GPIO07_MUX   0   // 0=GPIO7,  1=EPWM4B,  2=SPISTED,  3=ECAP2

#define GPIO08_MUX   0   // 0=GPIO8,  1=EPWM5A,  2=CANTXB,   3=ADCSOCAO
#define GPIO09_MUX   0   // 0=GPIO9,  1=EPWM5B,  2=SCITXDB,  3=ECAP3
#define GPIO10_MUX   0   // 0=GPIO10, 1=EPWM6A,  2=CANRXB,   3=ADCSOCBO
#define GPIO11_MUX   0   // 0=GPIO11, 1=EPWM6B,  2=SCIRXDB,  3=ECAP4
#define GPIO12_MUX   0   // 0=GPIO12, 1=TZ1,     2=CANTXB,   3=SPISIMOB
#define GPIO13_MUX   0   // 0=GPIO13, 1=TZ2,     2=CANRXB,   3=SPISOMIB
#define GPIO14_MUX   0   // 0=GPIO14, 1=TZ3,     2=SCITXDB,  3=SPICLKB
#define GPIO15_MUX   0   // 0=GPIO15, 1=TZ4,     2=SCIRXDB,  3=SPISTEB

#define GPIO16_MUX   0   // 0=GPIO16, 1=SPISIMOA,2=CANTXB,   3=TZ5
#define GPIO17_MUX   0   // 0=GPIO17, 1=SPISOMIA,2=CANRXB,   3=TZ6
#define GPIO18_MUX   0   // 0=GPIO18, 1=SPICLKA, 2=SCITXDB,  3=Reserved
#define GPIO19_MUX   0   // 0=GPIO19, 1=SPISTEA, 2=SCIRXDB,  3=Reserved
#define GPIO20_MUX   0   // 0=GPIO20, 1=EQEP1A,  2=SPISIMOC, 3=CANTXB
#define GPIO21_MUX   0   // 0=GPIO21, 1=EQEP1B,  2=SPISOMIC, 3=CANRXB
#define GPIO22_MUX   0   // 0=GPIO22, 1=EQEP1S,  2=SPICLKC,  3=SCITXDB
#define GPIO23_MUX   0   // 0=GPIO23, 1=EQEP1I,  2=SPISTEC,  3=SCIRXDB
    
#define GPIO24_MUX   0   // 0=GPIO24, 1=ECAP1,   2=EQEP2A,   3=SPISIMOB
#define GPIO25_MUX   0   // 0=GPIO25, 1=ECAP2,   2=EQEP2B,   3=SPISOMIB
#define GPIO26_MUX   0   // 0=GPIO26, 1=ECAP3,   2=EQEP2I,   3=SPICLKB
#define GPIO27_MUX   0   // 0=GPIO27, 1=ECAP4,   2=EQEP2S,   3=SPISTEB
#define GPIO28_MUX   1   // 0=GPIO28, 1=SCIRXDA, 2=Reserved, 3=TZ5
#define GPIO29_MUX   1   // 0=GPIO29, 1=SCITXDA, 2=Reserved, 3=TZ6
#define GPIO30_MUX   0   // 0=GPIO30, 1=CANRXA,  2=Reserved, 3=Reserved
#define GPIO31_MUX   0   // 0=GPIO31, 1=CANTXA,  2=Reserved, 3=Reserved

#define GPIO32_MUX   0   // 0=GPIO32, 1=SDAA,    2=EPWMSYNI, 3=ADCSOCAO
#define GPIO33_MUX   0   // 0=GPIO33, 1=SCLA,    2=EPWMSYNO, 3=ADCSOCBO
#define GPIO34_MUX   0   // 0=GPIO34, 1=Reserve, 2=Reserve,  3=Reserve

// GPIO Qualification Control (sampling period per port byte)
#define GPIO_QUAL0   0   // Sampling period for GPIO.00 - GPIO.07
#define GPIO_QUAL8   0   // Sampling period for GPIO.08 - GPIO.15
#define GPIO_QUAL16  0   // Sampling period for GPIO.16 - GPIO.23
#define GPIO_QUAL24  0   // Sampling period for GPIO.24 - GPIO.31
#define GPIO_QUAL32  0   // Sampling period for GPIO.32 - GPIO.34
#endif

//****************************************************************************/

#ifdef  PROCESSOR_2823x  // Mapping for 2823x Processors

#define GPIO00_MUX   0   // 0=GPIO0,  1=EPWM1A,  2=Reserved, 3=Reserved
#define GPIO01_MUX   0   // 0=GPIO1,  1=EPWM1B,  2=ECAP6,    3=MFSRB
#define GPIO02_MUX   0   // 0=GPIO2,  1=EPWM2A,  2=Reserved, 3=Reserved
#define GPIO03_MUX   0   // 0=GPIO3,  1=EPWM2B,  2=ECAP5,    3=MCLKRB
#define GPIO04_MUX   0   // 0=GPIO4,  1=EPWM3A,  2=Reserved, 3=Reserved
#define GPIO05_MUX   0   // 0=GPIO5,  1=EPWM3B,  2=MFSRA,    3=ECAP1
#define GPIO06_MUX   0   // 0=GPIO6,  1=EPWM4A,  2=EPWMSYNI, 3=EPWMSYNO
#define GPIO07_MUX   0   // 0=GPIO7,  1=EPWM4B,  2=MCLKRA,   3=ECAP2

#define GPIO08_MUX   0   // 0=GPIO8,  1=EPWM5A,  2=CANTXB,   3=ADCSOCAO
#define GPIO09_MUX   0   // 0=GPIO9,  1=EPWM5B,  2=SCITXDB,  3=ECAP3
#define GPIO10_MUX   0   // 0=GPIO10, 1=EPWM6A,  2=CANRXB,   3=ADCSOCBO
#define GPIO11_MUX   0   // 0=GPIO11, 1=EPWM6B,  2=SCIRXDB,  3=ECAP4
#define GPIO12_MUX   0   // 0=GPIO12, 1=TZ1,     2=CANTXB,   3=MDXB
#define GPIO13_MUX   0   // 0=GPIO13, 1=TZ2,     2=CANRXB,   3=MDRB
#define GPIO14_MUX   0   // 0=GPIO14, 1=TZ3,     2=SCITXDB,  3=MCLKXB
#define GPIO15_MUX   0   // 0=GPIO15, 1=TZ4,     2=SCIRXDB,  3=MFSXB

#define GPIO16_MUX   0   // 0=GPIO16, 1=SPISIMOA,2=CANTXB,   3=TZ5
#define GPIO17_MUX   0   // 0=GPIO17, 1=SPISOMIA,2=CANRXB,   3=TZ6
#define GPIO18_MUX   0   // 0=GPIO18, 1=SPICLKA, 2=SCITXDB,  3=CANRXA
#define GPIO19_MUX   0   // 0=GPIO19, 1=SPISTEA, 2=SCIRXDB,  3=CANTXA
#define GPIO20_MUX   0   // 0=GPIO20, 1=EQEP1A,  2=MDXA,     3=CANTXB
#define GPIO21_MUX   0   // 0=GPIO21, 1=EQEP1B,  2=MDRA,     3=CANRXB
#define GPIO22_MUX   0   // 0=GPIO22, 1=EQEP1S,  2=MCLKXA,   3=SCITXDB
#define GPIO23_MUX   0   // 0=GPIO23, 1=EQEP1I,  2=MFSXA,    3=SCIRXDB
    
#define GPIO24_MUX   0   // 0=GPIO24, 1=ECAP1,   2=EQEP2A,   3=SPISIMOB
#define GPIO25_MUX   0   // 0=GPIO25, 1=ECAP2,   2=EQEP2B,   3=SPISOMIB
#define GPIO26_MUX   0   // 0=GPIO26, 1=ECAP3,   2=EQEP2I,   3=SPICLKB
#define GPIO27_MUX   0   // 0=GPIO27, 1=ECAP4,   2=EQEP2S,   3=SPISTEB
#define GPIO28_MUX   1   // 0=GPIO28, 1=SCIRXDA, 2=XZCS6,    3=XZCS6
#define GPIO29_MUX   1   // 0=GPIO29, 1=SCITXDA, 2=XA19,     3=XA19
#define GPIO30_MUX   0   // 0=GPIO30, 1=CANRXA,  2=XA18,     3=XA18
#define GPIO31_MUX   0   // 0=GPIO31, 1=CANTXA,  2=XA17,     3=XA17

#define GPIO32_MUX   0   // 0=GPIO32, 1=SDAA,    2=EPWMSYNI, 3=ADCSOCAO
#define GPIO33_MUX   0   // 0=GPIO33, 1=SCLA,    2=EPWMSYNO, 3=ADCSOCBO
#define GPIO34_MUX   0   // 0=GPIO34, 1=ECAP1,   2=XREADY,   3=XREADY
#define GPIO35_MUX   0   // 0=GPIO35, 1=SCITXDA, 2=XR/W,     3=XR/W
#define GPIO36_MUX   0   // 0=GPIO36, 1=SCIRXDA, 2=XZCS0,    3=XZCS0
#define GPIO37_MUX   0   // 0=GPIO37, 1=ECAP2,   2=XZCS7,    3=XZCS7
#define GPIO38_MUX   0   // 0=GPIO38, 1=Reserve, 2=XWE0,     3=XWE0
#define GPIO39_MUX   0   // 0=GPIO39, 1=Reserve, 2=XA16,     3=XA16

#define GPIO40_MUX   0   // 0=GPIO40, 1=Reserve, 2=XA0/XWE1, 3=XA0/XWE1
#define GPIO41_MUX   0   // 0=GPIO41, 1=Reserve, 2=XA1,      3=XA1
#define GPIO42_MUX   0   // 0=GPIO42, 1=Reserve, 2=XA2,      3=XA2
#define GPIO43_MUX   0   // 0=GPIO43, 1=Reserve, 2=XA3,      3=XA3
#define GPIO44_MUX   0   // 0=GPIO44, 1=Reserve, 2=XA4,      3=XA4
#define GPIO45_MUX   0   // 0=GPIO45, 1=Reserve, 2=XA5,      3=XA5
#define GPIO46_MUX   0   // 0=GPIO46, 1=Reserve, 2=XA6,      3=XA6
#define GPIO47_MUX   0   // 0=GPIO47, 1=Reserve, 2=XA7,      3=XA7

#define GPIO48_MUX   0   // 0=GPIO48, 1=ECAP5,   2=XD31,     3=XD31
#define GPIO49_MUX   0   // 0=GPIO49, 1=ECAP6,   2=XD30,     3=XD30
#define GPIO50_MUX   0   // 0=GPIO50, 1=EQEP1A,  2=XD29,     3=XD29
#define GPIO51_MUX   0   // 0=GPIO51, 1=EQEP1B,  2=XD28,     3=XD28
#define GPIO52_MUX   0   // 0=GPIO52, 1=EQEP1S,  2=XD27,     3=XD27
#define GPIO53_MUX   0   // 0=GPIO53, 1=EQEP1I,  2=XD26,     3=XD26
#define GPIO54_MUX   0   // 0=GPIO54, 1=SPISIMOA,2=XD25,     3=XD25
#define GPIO55_MUX   0   // 0=GPIO55, 1=SPISOMIA,2=XD24,     3=XD24

#define GPIO56_MUX   0   // 0=GPIO56, 1=SPICLKA, 2=XD23,     3=XD23
#define GPIO57_MUX   0   // 0=GPIO57, 1=SPISTEA, 2=XD22,     3=XD22
#define GPIO58_MUX   0   // 0=GPIO58, 1=MCLKRA,  2=XD21,     3=XD21
#define GPIO59_MUX   0   // 0=GPIO59, 1=MFSRA,   2=XD20,     3=XD20
#define GPIO60_MUX   0   // 0=GPIO60, 1=MCLKRB,  2=XD19,     3=XD19
#define GPIO61_MUX   0   // 0=GPIO61, 1=MFSRB,   2=XD18,     3=XD18
#define GPIO62_MUX   0   // 0=GPIO62, 1=SCIRXDC, 2=XD17,     3=XD17
#define GPIO63_MUX   0   // 0=GPIO63, 1=SCITXDC, 2=XD16,     3=XD16

#define GPIO64_MUX   0   // 0=GPIO64, 1=GPIO64,  2=XD15,     3=XD15
#define GPIO65_MUX   0   // 0=GPIO65, 1=GPIO65,  2=XD14,     3=XD14
#define GPIO66_MUX   0   // 0=GPIO66, 1=GPIO66,  2=XD13,     3=XD13
#define GPIO67_MUX   0   // 0=GPIO67, 1=GPIO67,  2=XD12,     3=XD12
#define GPIO68_MUX   0   // 0=GPIO68, 1=GPIO68,  2=XD11,     3=XD11
#define GPIO69_MUX   0   // 0=GPIO69, 1=GPIO69,  2=XD10,     3=XD10
#define GPIO70_MUX   0   // 0=GPIO70, 1=GPIO70,  2=XD9,      3=XD9
#define GPIO71_MUX   0   // 0=GPIO71, 1=GPIO71,  2=XD8,      3=XD8

#define GPIO72_MUX   0   // 0=GPIO72, 1=GPIO72,  2=XD7,      3=XD7
#define GPIO73_MUX   0   // 0=GPIO73, 1=GPIO73,  2=XD6,      3=XD6
#define GPIO74_MUX   0   // 0=GPIO74, 1=GPIO74,  2=XD5,      3=XD5
#define GPIO75_MUX   0   // 0=GPIO75, 1=GPIO75,  2=XD4,      3=XD4
#define GPIO76_MUX   0   // 0=GPIO76, 1=GPIO76,  2=XD3,      3=XD3
#define GPIO77_MUX   0   // 0=GPIO77, 1=GPIO77,  2=XD2,      3=XD2
#define GPIO78_MUX   0   // 0=GPIO78, 1=GPIO78,  2=XD1,      3=XD1
#define GPIO79_MUX   0   // 0=GPIO79, 1=GPIO79,  2=XD0,      3=XD0

#define GPIO80_MUX   0   // 0=GPIO80, 1=GPIO80,  2=XA8,      3=XA8
#define GPIO81_MUX   0   // 0=GPIO81, 1=GPIO81,  2=XA9,      3=XA9
#define GPIO82_MUX   0   // 0=GPIO82, 1=GPIO82,  2=XA10,     3=XA10
#define GPIO83_MUX   0   // 0=GPIO83, 1=GPIO83,  2=XA11,     3=XA11
#define GPIO84_MUX   0   // 0=GPIO84, 1=GPIO84,  2=XA12,     3=XA12
#define GPIO85_MUX   0   // 0=GPIO85, 1=GPIO85,  2=XA13,     3=XA13
#define GPIO86_MUX   0   // 0=GPIO86, 1=GPIO86,  2=XA14,     3=XA14
#define GPIO87_MUX   0   // 0=GPIO87, 1=GPIO87,  2=XA15,     3=XA15

// GPIO Qualification Control (sampling period per port byte)
#define GPIO_QUAL0   0   // Sampling period for GPIO.00 - GPIO.07
#define GPIO_QUAL8   0   // Sampling period for GPIO.08 - GPIO.15
#define GPIO_QUAL16  0   // Sampling period for GPIO.16 - GPIO.23
#define GPIO_QUAL24  0   // Sampling period for GPIO.24 - GPIO.31
#define GPIO_QUAL32  0   // Sampling period for GPIO.32 - GPIO.39
#define GPIO_QUAL40  0   // Sampling period for GPIO.40 - GPIO.47
#define GPIO_QUAL48  0   // Sampling period for GPIO.48 - GPIO.55
#define GPIO_QUAL56  0   // Sampling period for GPIO.56 - GPIO.63
#endif

// GPIO External Interrupt Control (XINT number and edge sensitivity)
#define XINTERRUPT_1     XINT1(XINT_EDGE_RISING)
#define XINTERRUPT_2     XINT2(XINT_EDGE_FALLING)

//****************************************************************************/

#endif /*HAL_CONFIGURE_H*/

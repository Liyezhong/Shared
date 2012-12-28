/*********************************************************************************/
/*! \file halConfigure.h
 *
 *  \brief  HAL configuration file
 *
 *  $Version: $ 0.3
 *  $Date:    $ 20.10.2012
 *  $Author:  $ Brandon Shao
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
 *      This file works in conjunction with the halConfigure.c source
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
/*********************************************************************************/

#ifndef HAL_CONFIGURE_H
#define HAL_CONFIGURE_H

//****************************************************************************/
// Processor Definition (see halProcessor.h for selectable processors)
//****************************************************************************/

#define PROCESSOR   STM32F103xB   //!< Processor type 128kB Flash, 20kB RAM


//****************************************************************************/
// Device ID mapping (function-module to application specific)
//****************************************************************************/

//! Fan control signal
#define HAL_FANCONTROL_SIGNAL    HAL_DIGITAL_OUTPUT_0

//! Lid control signal
#define HAL_LIDCONTROL_SIGNAL    HAL_DIGITAL_OUTPUT_1

//! Heater relay signal
#define HAL_HEATER_RELAY         HAL_DIGITAL_OUTPUT_2

//! Remote Alarm Control signal
#define HAL_REMOTE_ALARM_CONTROL HAL_DIGITAL_OUTPUT_3

//! Remote Alarm Set signal
#define HAL_REMOTE_ALARM_SET     HAL_DIGITAL_OUTPUT_4

//! Remote Alarm Clear signal
#define HAL_REMOTE_ALARM_CLEAR   HAL_DIGITAL_OUTPUT_5

//! Local Alarm Control signal
#define HAL_LOCAL_ALARM_CONTROL  HAL_DIGITAL_OUTPUT_6

//! Local Alarm Set signal
#define HAL_LOCAL_ALARM_SET      HAL_DIGITAL_OUTPUT_7

//! Local Alarm Clear signal
#define HAL_LOCAL_ALARM_CLEAR    HAL_DIGITAL_OUTPUT_8

//! Valve control signal
#define HAL_VALVECONTROL_SIGNAL  HAL_DIGITAL_OUTPUT_9

//! Lid status signal
#define HAL_LIDSTATUS_SIGNAL     HAL_DIGITAL_INPUT_0

/*********************************************************************************/
/*
 *  \brief   Timer configuration
 *
 *      Following settings define the configuration for the various timers.
 *      They are used only in halConfigure.c to initialize the descriptor
 *      table halLogicalTimersTable[]. It would be possible to define the
 *      numerical values directly in the descriptor instead of indirectly
 *      using this defines. The defines on the other side give more room
 *      to comment the individual setting.
 *
 *      See the reference manual for the STM32 family for details concerning
 *      the meaning of the timer configuration bits. TIM_MODE_MASTER define
 *      the timers SMC register, while TIM_MODE_CAPCOMy define the CCM
 *      register (upper byte) and the CCE register (low byte) for capture
 *      or compare unit y.
 *
 *********************************************************************************/

// Timer 1 programmed for the control of the heating elements
#define TIM3_MODE_MASTER    0x0000   //!< Timer slave mode
#define TIM3_MODE_CAPCOM1   0x6C01   //!< Unit 1: Heater control PWM output
#define TIM3_MODE_CAPCOM2   0x6C01   //!< Unit 2: Heater control PWM output
#define TIM3_MODE_CAPCOM3   0x6C01   //!< Unit 3: Heater control PWM output
#define TIM3_MODE_CAPCOM4   0x6C01   //!< Unit 4: Heater control PWM output

/********************************************************************************/
/*
 *  \brief   Pin Mapping configuration
 *
 *      The following settings define the pin selection of the various on-chip
 *      peripherals. Several peripheral i/o's have alternate pin assignment,
 *      which can be configured here.
 *
 *      On each parameter the processors supporting these setting are listed.
 *      The leading STM32 term is omitted, e.g. F100 is used for the STM32F100
 *      family. F10x stands for the families F101, F102, F103, F105 and F107,
 *      but not F100. Parameters defined for high density F103 devices also
 *      apply to connectivity line devices (F105, F107).
 *
 *********************************************************************************/

// Pin Mapping for Timer 1:
//  ------------------------------------------------------------------
// | n |  CH1 | CH2  | CH3  | CH4  | ETR  | BKIN | CH1N | CH2N | CH3N |
// |---|------|------|------|------|------|------|------|------|------|
// | 0 | PA08 | PA09 | PA10 | PA11 | PA12 | PB12 | PB13 | PB14 | PB15 |
// | 1 | PA08 | PA09 | PA10 | PA11 | PA12 | PA06 | PA07 | PB00 | PB01 |
// | 2 | -    |  -   |  -   |  -   | -    |  -   |  -   |  -   |      |
// | 3 | PE09 | PE11 | PE13 | PE14 | PE07 | PE15 | PE08 | PE10 | PE12 |
//  ------------------------------------------------------------------
//
#define MAPPING_TIMER1   0   //!< Configured timer 1 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 2:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100, F10x: Always present
// |---|------|------|------|------|------|
// | 0 | PA00 | PA01 | PA02 | PA03 | PA00 |
// | 1 | PA15 | PB03 | PA02 | PA03 | PA15 |
// | 2 | PA00 | PA01 | PB10 | PB11 | PA00 |
// | 3 | PA15 | PB03 | PB10 | PB11 | PA15 |
//  --------------------------------------
//
#define MAPPING_TIMER2   0   //!< Configured timer 2 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 3:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100, F10x: Always present
// |---|------|------|------|------|------|
// | 0 | PA06 | PA07 | PB00 | PB01 | PD02 |
// | 1 |  -   |  -   |  -   |  -   |  -   |
// | 2 | PB04 | PB05 | PB00 | PB01 | PD02 |
// | 3 | PC06 | PC07 | PC08 | PC09 | PD02 |
//  --------------------------------------
//
#define MAPPING_TIMER3   3   //!< Configured timer 3 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 4:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100, F10x: Present on all but
// |---|------|------|------|------|------|  low density devices
// | 0 | PB06 | PB07 | PB08 | PB09 | PE00 |
// | 1 | PD12 | PD13 | PD14 | PD15 | PE00 |
//  --------------------------------------
//
#define MAPPING_TIMER4   0   //!< Configured timer 4 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 5:
//  --------------------------------------   F100, F10x: Present on high and
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  XL density devices
// |---|------|------|------|------|------|
// | 0 | PA00 | PA01 | PA02 | PA03 | nc   |  LSI is an internal connection
// | 1 | PA00 | PA01 | PA02 | LSI  | nc   |  to the low speed oscillator
//  --------------------------------------
//
#define MAPPING_TIMER5   0   //!< Configured timer 5 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 9:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100: Not present
// |---|------|------|------|------|------|  F10x: XL devices only
// | 0 | PA02 | PA03 | n/a  | n/a  | n/a  |
// | 1 | PE05 | PE06 | n/a  | n/a  | n/a  |
//  --------------------------------------
//
#define MAPPING_TIMER9   0   //!< Configured timer 9 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 10:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100: Not present
// |---|------|------|------|------|------|  F10x: XL devices only
// | 0 | PB08 | n/a  | n/a  | n/a  | n/a  |
// | 1 | PF06 | n/a  | n/a  | n/a  | n/a  |
//  --------------------------------------
//
#define MAPPING_TIMER10  0   //!< Configured timer 10 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 11:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100: Not present
// |---|------|------|------|------|------|  F10x: XL devices only
// | 0 | PB09 | n/a  | n/a  | n/a  | n/a  |
// | 1 | PF07 | n/a  | n/a  | n/a  | n/a  |
//  --------------------------------------
//
#define MAPPING_TIMER11  0   //!< Configured timer 11 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 12:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100: High density devices
// |---|------|------|------|------|------|  F10x: XL devices only
// | 0 | PC04 | PC05 | n/a  | n/a  | n/a  |
// | 1 | PB12 | PB13 | n/a  | n/a  | n/a  |
//  --------------------------------------
//
#define MAPPING_TIMER12  0   //!< Configured timer 12 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 13:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100: High density devices
// |--------------------------------------|  F10x: XL devices only
// | F100 Family:                         |
// |--------------------------------------|
// | 0 | PC08 | n/a  | n/a  | n/a  | n/a  |
// | 1 | PB00 | n/a  | n/a  | n/a  | n/a  |
// |--------------------------------------|
// | F10x Family:                         |
// |--------------------------------------|
// | 0 | PA06 | n/a  | n/a  | n/a  | n/a  |
// | 1 | PF08 | n/a  | n/a  | n/a  | n/a  |
//  --------------------------------------
//
#define MAPPING_TIMER13  0   //!< Configured timer 13 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 14:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100: High density devices
// |--------------------------------------|  F10x: XL devices only
// | F100 Family:                         |
// |--------------------------------------|
// | 0 | PC09 | n/a  | n/a  | n/a  | n/a  |
// | 1 | PB01 | n/a  | n/a  | n/a  | n/a  |
// |--------------------------------------|
// | F10x Family:                         |
// |--------------------------------------|
// | 0 | PA07 | n/a  | n/a  | n/a  | n/a  |
// | 1 | PF09 | n/a  | n/a  | n/a  | n/a  |
//  --------------------------------------
//
#define MAPPING_TIMER14  0   //!< Configured timer 14 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 15:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100: Always present
// |---|------|------|------|------|------|  F10x: Not present
// | 0 | PA02 | PA03 | n/a  | n/a  | n/a  |
// | 1 | PB14 | PB15 | n/a  | n/a  | n/a  |
//  --------------------------------------
//
#define MAPPING_TIMER15  0   //!< Configured timer 15 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 16:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100: Always present
// |---|------|------|------|------|------|  F10x: Not present
// | 0 | PB08 | n/a  | n/a  | n/a  | n/a  |
// | 1 | PA06 | n/a  | n/a  | n/a  | n/a  |
//  --------------------------------------
//
#define MAPPING_TIMER16  0   //!< Configured timer 16 pin mapping

//********************************************************************************/
// Pin Mapping for Timer 17:
//  --------------------------------------
// | n | CH1  | CH2  | CH3  | CH4  | ETR  |  F100: Always present
// |---|------|------|------|------|------|  F10x: Not present
// | 0 | PB09 | n/a  | n/a  | n/a  | n/a  |
// | 1 | PA07 | n/a  | n/a  | n/a  | n/a  |
//  --------------------------------------
//
#define MAPPING_TIMER17  0   //!< Configured timer 17 pin mapping

//********************************************************************************/
// Pin Mapping for Usart 1:
//  --------------------------------------
// | n | TX   | RX   | CK   | CTS  | RTS  |  F100, F10x: Always present
// |---|------|------|------|------|------|
// | 0 | PA09 | PA10 | PA08 | PA11 | PA12 |
// | 1 | PB06 | PB07 | PA08 | PA11 | PA12 |
//  --------------------------------------
//
#define MAPPING_USART1   0   //!< Configured Usart 1 pin mapping

//********************************************************************************/
// Pin Mapping for Usart 2:
//  --------------------------------------
// | n | TX   | RX   | CK   | CTS  | RTS  |  F100, F10x: Always present
// |---|------|------|------|------|------|
// | 0 | PA02 | PA03 | PA04 | PA00 | PA01 |
// | 1 | PD05 | PD06 | PD07 | PD03 | PD04 |
//  --------------------------------------
//
#define MAPPING_USART2   0   //!< Configured Usart 2 pin mapping

//********************************************************************************/
// Pin Mapping for Usart 3:
//  --------------------------------------
// | n | TX   | RX   | CK   | CTS  | RTS  |  F100: Always present
// |---|------|------|------|------|------|  F10x: Not on low density devices
// | 0 | PB10 | PB11 | PB12 | PB13 | PB14 |
// | 1 | PC10 | PC11 | PC12 | PB13 | PB14 |
// | 2 |  -   |  -   |  -   |  -   |  -   |
// | 3 | PD08 | PD09 | PD10 | PD11 | PD12 |
//  --------------------------------------
//
#define MAPPING_USART3   0   //!< Configured Usart 3 pin mapping

//********************************************************************************/
// Pin Mapping for I2C 1:
//  --------------------------------------
// | n | SCL  | SDA  |      |      |      |  F100, F10x: Always present
// |---|------|------|------|------|------|
// | 0 | PB06 | PB07 |      |      |      |
// | 1 | PB08 | PB09 |      |      |      |
//  --------------------------------------
//
#define MAPPING_I2C1     0   //!< Configured I2C 1 pin mapping

//********************************************************************************/
// Pin Mapping for SPI 1:
//  --------------------------------------
// | n | NSS  | SCK  | MISO | MOSI |      |  F100, F10x: Always present
// |---|------|------|------|------|------|
// | 0 | PA04 | PA05 | PA06 | PA07 |      |
// | 1 | PA15 | PB03 | PB04 | PB05 |      |
//  --------------------------------------
//
#define MAPPING_SPI1     0   //!< Configured SPI 1 pin mapping

//********************************************************************************/
// Pin Mapping for SPI 3:
//  --------------------------------------
// | n | NSS  | SCK  | MISO | MOSI |      |  Present on F105 and F107
// |---|------|------|------|------|------|  devices only
// | 0 | PA15 | PB03 | PB04 | PB05 |      |
// | 1 | PA04 | PC10 | PC11 | PC12 |      |
//  --------------------------------------
//
#define MAPPING_SPI3     0   //!< Configured SPI 3 pin mapping

//********************************************************************************/
// Pin Mapping for CAN 1:
//  --------------------------------------
// | n | RxD  | TxD  |      |      |      |   Present on F103, F105, F107
// |---|------|------|------|------|------|   devices only
// | 0 | PA11 | PA12 |      |      |      |
// | 1 |  -   |  -   |      |      |      |
// | 2 | PB08 | PB09 |      |      |      |
// | 3 | PD00 | PD01 |      |      |      |
//  --------------------------------------
//
#define MAPPING_CAN1     0   //!< Configured CAN 1 pin mapping

//********************************************************************************/
// Pin Mapping for CAN 2:
//  --------------------------------------
// | n | RxD  | TxD  |      |      |      |   Present on F105 and F107
// |---|------|------|------|------|------|   devices only
// | 0 | PB12 | PB13 |      |      |      |
// | 1 | PB05 | PB06 |      |      |      |
//  --------------------------------------
//
#define MAPPING_CAN2     0   //!< Configured CAN 2 pin mapping

//********************************************************************************/
// Pin Mapping for JTAG/SWD:
//  --------------------------------------
// | n | PA13 | PA14 | PA15 | PD03 | PB04 |
// |---|------|------|------|------|------|
// | 0 | JTMS | JTCK | JTDI | JTDO | JRST |
// | 1 | JTMS | JTCK | JTDI | JTDO | PB04 |
// | 2 | SWDIO| SWCK | PA15 | PD03 | PB04 |
// | 3 |  -   |  -   |  -   |  -   |  -   |
// | 4 | PA13 | PA14 | PA15 | PD03 | PB04 |
//  --------------------------------------
//
#define MAPPING_SWJCFG   0   //!< Configured JTAG/SWD pin mapping

//********************************************************************************/

#endif /*HAL_CONFIGURE_H*/

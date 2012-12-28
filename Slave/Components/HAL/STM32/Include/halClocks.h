/****************************************************************************/
/*! \file halClocks.h
 *
 *  \brief  Hardware System Control
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to control and initialize the core
 *      system functions. This includes:
 *
 *      - Watchdog control
 *      - Hardware reset control
 *      - Peripheral clock control
 *      - PLL clock initialization
 *
 *      Also included is the main initialization function that calls all
 *      initialization functions in the other HAL layer modules.
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

#ifndef HAL_CLOCKS_H
#define HAL_CLOCKS_H

//@{**************************************************************************/
//! Peripheral Identifier Definition
//****************************************************************************/

#define PERIPHERAL_CLOCK_ID(RegNr,BitNr)  ((RegNr) * 32 + (BitNr))

// Peripheral clock enable identifiers for processor internal peripherals
#define PERIPHERAL_DMA1     PERIPHERAL_CLOCK_ID(0, 0)
#define PERIPHERAL_DMA2     PERIPHERAL_CLOCK_ID(0, 1)
#define PERIPHERAL_SRAM     PERIPHERAL_CLOCK_ID(0, 2)
#define PERIPHERAL_FLITF    PERIPHERAL_CLOCK_ID(0, 4)
#define PERIPHERAL_CRC      PERIPHERAL_CLOCK_ID(0, 6)
#define PERIPHERAL_FSMC     PERIPHERAL_CLOCK_ID(0, 8)
#define PERIPHERAL_SDIO     PERIPHERAL_CLOCK_ID(0, 10)
#define PERIPHERAL_OTGFS    PERIPHERAL_CLOCK_ID(0, 12)
#define PERIPHERAL_ETH      PERIPHERAL_CLOCK_ID(0, 14)
#define PERIPHERAL_ETH_TX   PERIPHERAL_CLOCK_ID(0, 15)
#define PERIPHERAL_ETH_RX   PERIPHERAL_CLOCK_ID(0, 16)
#define PERIPHERAL_WWDG     PERIPHERAL_CLOCK_ID(2, 11)
#define PERIPHERAL_AFIO     PERIPHERAL_CLOCK_ID(1, 0)

#define PERIPHERAL_GPIO_A   PERIPHERAL_CLOCK_ID(1, 2)
#define PERIPHERAL_GPIO_B   PERIPHERAL_CLOCK_ID(1, 3)
#define PERIPHERAL_GPIO_C   PERIPHERAL_CLOCK_ID(1, 4)
#define PERIPHERAL_GPIO_D   PERIPHERAL_CLOCK_ID(1, 5)
#define PERIPHERAL_GPIO_E   PERIPHERAL_CLOCK_ID(1, 6)
#define PERIPHERAL_GPIO_F   PERIPHERAL_CLOCK_ID(1, 7)
#define PERIPHERAL_GPIO_G   PERIPHERAL_CLOCK_ID(1, 8)

#define PERIPHERAL_ADC1     PERIPHERAL_CLOCK_ID(1, 9)
#define PERIPHERAL_ADC2     PERIPHERAL_CLOCK_ID(1, 10)
#define PERIPHERAL_ADC3     PERIPHERAL_CLOCK_ID(1, 15)
#define PERIPHERAL_DAC      PERIPHERAL_CLOCK_ID(2, 29)

#define PERIPHERAL_TIMER1   PERIPHERAL_CLOCK_ID(1, 11)
#define PERIPHERAL_TIMER2   PERIPHERAL_CLOCK_ID(2, 0)
#define PERIPHERAL_TIMER3   PERIPHERAL_CLOCK_ID(2, 1)
#define PERIPHERAL_TIMER4   PERIPHERAL_CLOCK_ID(2, 2)
#define PERIPHERAL_TIMER5   PERIPHERAL_CLOCK_ID(2, 3)
#define PERIPHERAL_TIMER6   PERIPHERAL_CLOCK_ID(2, 4)
#define PERIPHERAL_TIMER7   PERIPHERAL_CLOCK_ID(2, 5)
#define PERIPHERAL_TIMER8   PERIPHERAL_CLOCK_ID(1, 13)
#define PERIPHERAL_TIMER9   PERIPHERAL_CLOCK_ID(1, 19)
#define PERIPHERAL_TIMER10  PERIPHERAL_CLOCK_ID(1, 20)
#define PERIPHERAL_TIMER11  PERIPHERAL_CLOCK_ID(1, 21)
#define PERIPHERAL_TIMER12  PERIPHERAL_CLOCK_ID(2, 6)
#define PERIPHERAL_TIMER13  PERIPHERAL_CLOCK_ID(2, 7)
#define PERIPHERAL_TIMER14  PERIPHERAL_CLOCK_ID(2, 8)
#define PERIPHERAL_TIMER15  PERIPHERAL_CLOCK_ID(1, 16)   
#define PERIPHERAL_TIMER16  PERIPHERAL_CLOCK_ID(1, 17)   
#define PERIPHERAL_TIMER17  PERIPHERAL_CLOCK_ID(1, 18)   

#define PERIPHERAL_SPI1     PERIPHERAL_CLOCK_ID(1, 12)
#define PERIPHERAL_SPI2     PERIPHERAL_CLOCK_ID(2, 14)
#define PERIPHERAL_SPI3     PERIPHERAL_CLOCK_ID(2, 15)
#define PERIPHERAL_UART1    PERIPHERAL_CLOCK_ID(1, 14)
#define PERIPHERAL_UART2    PERIPHERAL_CLOCK_ID(2, 17)
#define PERIPHERAL_UART3    PERIPHERAL_CLOCK_ID(2, 18)
#define PERIPHERAL_UART4    PERIPHERAL_CLOCK_ID(2, 19)
#define PERIPHERAL_UART5    PERIPHERAL_CLOCK_ID(2, 20)
#define PERIPHERAL_I2C1     PERIPHERAL_CLOCK_ID(2, 21)
#define PERIPHERAL_I2C2     PERIPHERAL_CLOCK_ID(2, 22)

#define PERIPHERAL_USB      PERIPHERAL_CLOCK_ID(2, 23)
#define PERIPHERAL_CAN1     PERIPHERAL_CLOCK_ID(2, 25)
#define PERIPHERAL_CAN2     PERIPHERAL_CLOCK_ID(2, 26)
#define PERIPHERAL_BKP      PERIPHERAL_CLOCK_ID(2, 27)
#define PERIPHERAL_PWR      PERIPHERAL_CLOCK_ID(2, 28)

#define MAX_PERIPHERAL_ID   PERIPHERAL_DAC

//@} End of doxygen group

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Reset reason identifier returned by halGetResetReason
typedef enum {
    RESET_CAUSE_UNKNOWN,        //!< Unknown reset cause
    RESET_CAUSED_BY_POWER_ON,   //!< Reset caused by normal power up
    RESET_CAUSED_BY_HARDWARE,   //!< Reset caused by hardware pin
    RESET_CAUSED_BY_SOFTWARE,   //!< Reset caused by software
    RESET_CAUSED_BY_WATCHDOG,   //!< Reset caused by watchdog timeout
    RESET_CAUSED_BY_STANDBY     //!< Reset caused by entering standby/stop
} ResetReason_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halPeripheralClockEnable  (UInt16 PeripheralID, Bool State);
Error_t halPeripheralReset        (UInt16 PeripheralID, Bool State);

UInt32  halGetProcessorClockRate  (void);
UInt32  halGetPeripheralClockRate (void);
UInt32  halGetRealTimeClockRate   (void);
Error_t halClocksInit             (void);

ResetReason_t halGetResetReason   (void);

//****************************************************************************/

#endif /*HAL_CLOCKS_H*/

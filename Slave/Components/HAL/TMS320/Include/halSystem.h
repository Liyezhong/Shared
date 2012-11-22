/****************************************************************************/
/*! \file halSystem.h
 * 
 *  \brief  Hardware System Control 
 *
 *  $Version: $ 0.1
 *  $Date:    $ 19.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to control and initialize the core
 *       system functions. This includes:
 * 
 *       - Watchdog control
 *       - Hardware reset control
 *       - Peripheral clock control 
 *       - PLL clock initialization
 * 
 *       Also included is the main initialization function that calls all
 *       initialization functions in the other HAL layer modules.
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

#ifndef HAL_SYSTEM_H
#define HAL_SYSTEM_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

#define PERIPHERAL_CLOCK_ID(RegNr,BitNr)  ((RegNr) * 32 + (BitNr))

// Peripheral clock enable identifiers for processor internal peripherals
#define PERIPHERAL_ADC      PERIPHERAL_CLOCK_ID(0, 3)
#define PERIPHERAL_I2C      PERIPHERAL_CLOCK_ID(0, 4)
#define PERIPHERAL_SCI_C    PERIPHERAL_CLOCK_ID(0, 5)
#define PERIPHERAL_SPI_C    PERIPHERAL_CLOCK_ID(0, 6)   // 280x only
#define PERIPHERAL_SPI_D    PERIPHERAL_CLOCK_ID(0, 7)   // 280x only
#define PERIPHERAL_SPI_A    PERIPHERAL_CLOCK_ID(0, 8)
#define PERIPHERAL_SPI_B    PERIPHERAL_CLOCK_ID(0, 9)   // 280x only
#define PERIPHERAL_SCI_A    PERIPHERAL_CLOCK_ID(0, 10)
#define PERIPHERAL_SCI_B    PERIPHERAL_CLOCK_ID(0, 11)
#define PERIPHERAL_MCBSP_A  PERIPHERAL_CLOCK_ID(0, 12)  // 2823x only
#define PERIPHERAL_MCBSP_B  PERIPHERAL_CLOCK_ID(0, 13)  // 2823x only
#define PERIPHERAL_CAN_A    PERIPHERAL_CLOCK_ID(0, 14)
#define PERIPHERAL_CAN_B    PERIPHERAL_CLOCK_ID(0, 15)
#define PERIPHERAL_PWM_1    PERIPHERAL_CLOCK_ID(1, 0)
#define PERIPHERAL_PWM_2    PERIPHERAL_CLOCK_ID(1, 1)
#define PERIPHERAL_PWM_3    PERIPHERAL_CLOCK_ID(1, 2)
#define PERIPHERAL_PWM_4    PERIPHERAL_CLOCK_ID(1, 3)
#define PERIPHERAL_PWM_5    PERIPHERAL_CLOCK_ID(1, 4)
#define PERIPHERAL_PWM_6    PERIPHERAL_CLOCK_ID(1, 5)
#define PERIPHERAL_PWM_7    PERIPHERAL_CLOCK_ID(1, 6)
#define PERIPHERAL_PWM_8    PERIPHERAL_CLOCK_ID(1, 7)
#define PERIPHERAL_CAP_1    PERIPHERAL_CLOCK_ID(1, 8)
#define PERIPHERAL_CAP_2    PERIPHERAL_CLOCK_ID(1, 9)
#define PERIPHERAL_CAP_3    PERIPHERAL_CLOCK_ID(1, 10)
#define PERIPHERAL_CAP_4    PERIPHERAL_CLOCK_ID(1, 11)
#define PERIPHERAL_CAP_5    PERIPHERAL_CLOCK_ID(1, 12)  // 2823x only
#define PERIPHERAL_CAP_6    PERIPHERAL_CLOCK_ID(1, 13)  // 2823x only
#define PERIPHERAL_QEP_1    PERIPHERAL_CLOCK_ID(1, 14)
#define PERIPHERAL_QEP_2    PERIPHERAL_CLOCK_ID(1, 15)
#define PERIPHERAL_TIMER0   PERIPHERAL_CLOCK_ID(3, 8)
#define PERIPHERAL_TIMER1   PERIPHERAL_CLOCK_ID(3, 9)
#define PERIPHERAL_TIMER2   PERIPHERAL_CLOCK_ID(3, 10)
#define PERIPHERAL_DMA      PERIPHERAL_CLOCK_ID(3, 11)
#define PERIPHERAL_XINTF    PERIPHERAL_CLOCK_ID(3, 12)
#define PERIPHERAL_GPIO     PERIPHERAL_CLOCK_ID(3, 13)


//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//! Reset reason identifier returned by halGetResetReason
typedef enum {
    RESET_CAUSED_BY_POWER_UP,   //!< Reset caused by normal power up
    RESET_CAUSED_BY_WATCHDOG    //!< Reset caused by watchdog timeout
} RESET_REASON_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

ERROR_t halPeripheralClockEnable (UInt16 PeripheralID, Bool State);
ERROR_t halWatchdogTrigger (void);
ERROR_t halWatchdogDisable (void);
ERROR_t halWatchdogEnable  (void);
void    halHardwareReset   (void);
void    halIdleTask        (void);

RESET_REASON_t halGetResetReason (void);

ERROR_t halInitializeHalLayer (void);

//****************************************************************************/

#endif /*HAL_SYSTEM_H*/

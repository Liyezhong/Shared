/****************************************************************************/
/*! \file halClocks.c
 *
 *  \brief  Processor Clock Control
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to control and initialize the
 *      processor internal clocks:
 *
 *      - Processor core clock
 *      - Peripheral clocks
 *      - PLL clock initialization
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

#include "Global.h"
#include "halCommon.h"
#include "halError.h"
#include "halMain.h"
#include "halClocks.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define HSI_FREQUENCY      8000000      //!< HSI internal oscillator (8 MHz)
#define LSI_FREQUENCY      40000        //!< LSI internal oscillator (40 kHz)

#define PLL_MIN_MULIPLIER  2            //!< minimal PLL multiplier
#define PLL_MAX_MULIPLIER  16           //!< maximal PLL multiplier
#define PLL_MIN_DIVIDER    1            //!< minimal PLL divider
#define PLL_MAX_DIVIDER    2            //!< maximal PLL divider

#define RCC_CR_HSI_ON      0x00000001   //!< HSI clock enable
#define RCC_CR_HSI_RDY     0x00000002   //!< HSI clock ready flag
#define RCC_CR_HSE_ON      0x00010000   //!< HSE clock enable
#define RCC_CR_HSE_RDY     0x00020000   //!< HSE clock ready flag
#define RCC_CR_HSE_BYP     0x00040000   //!< HSE clock bypass
#define RCC_CR_CSS_ON      0x00080000   //!< Clock security system enable
#define RCC_CR_PLL_ON      0x01000000   //!< PLL enable
#define RCC_CR_PLL_RDY     0x02000000   //!< PLL clock ready flag

#define RCC_CFGR_PLL_SRC   0x00010000   //!< PLL entry clock source
#define RCC_CFGR_PLL_XTPRE 0x00020000   //!< HSE divider for PLL entry
#define RCC_CFGR_PLL_MUL   0x003C0000   //!< PLL multiplication factor
#define RCC_CFGR_MCO       0x07000000   //!< Microcontroller clock output
#define RCC_CFGR_MCO_OFF   0x00000000   //!< MCO clock output disabled
#define RCC_CFGR_MCO_HSI   0x05000000   //!< MCO clock output = HSI
#define RCC_CFGR_MCO_HSE   0x06000000   //!< MCO clock output = HSE
#define RCC_CFGR_MCO_PLL   0x07000000   //!< MCO clock output = PLL/2
#define RCC_CFGR_MCO_SYS   0x04000000   //!< MCO clock output = SYSCLOCK

#define RCC_CFGR_SW        0x00000003   //!< System clock switch
#define RCC_CFGR_SW_HSI    0x00000000   //!< HSI selected as system clock
#define RCC_CFGR_SW_HSE    0x00000001   //!< HSE selected as system clock
#define RCC_CFGR_SW_PLL    0x00000002   //!< PLL selected as system clock

#define RCC_CFGR_ADCPRE    0x0000C000   //!< ADC prescaler bitmask
#define RCC_CFGR_ADCPRE_2  0x00000000   //!< ADC prescaler = 2
#define RCC_CFGR_ADCPRE_4  0x00004000   //!< ADC prescaler = 4
#define RCC_CFGR_ADCPRE_6  0x00008000   //!< ADC prescaler = 6
#define RCC_CFGR_ADCPRE_8  0x0000C000   //!< ADC prescaler = 8

#define RCC_CFGR_PPRE2     0x00003800   //!< APB2 prescaler bitmask
#define RCC_CFGR_PPRE2_1   0x00000000   //!< APB2 prescaler = 1
#define RCC_CFGR_PPRE2_2   0x00002000   //!< APB2 prescaler = 2
#define RCC_CFGR_PPRE2_4   0x00002800   //!< APB2 prescaler = 4
#define RCC_CFGR_PPRE2_8   0x00003000   //!< APB2 prescaler = 8
#define RCC_CFGR_PPRE2_16  0x00003800   //!< APB2 prescaler = 16

#define RCC_CFGR_PPRE1     0x00000700   //!< APB1 prescaler bitmask
#define RCC_CFGR_PPRE1_1   0x00000000   //!< APB1 prescaler = 1
#define RCC_CFGR_PPRE1_2   0x00000400   //!< APB1 prescaler = 2
#define RCC_CFGR_PPRE1_4   0x00000500   //!< APB1 prescaler = 4
#define RCC_CFGR_PPRE1_8   0x00000600   //!< APB1 prescaler = 8
#define RCC_CFGR_PPRE1_16  0x00000700   //!< APB1 prescaler = 16

#define RCC_CFGR_HPRE      0x000000F0   //!< AHB prescaler bitmask
#define RCC_CFGR_HPRE_1    0x00000000   //!< AHB prescaler = 1
#define RCC_CFGR_HPRE_2    0x00000080   //!< AHB prescaler = 2
#define RCC_CFGR_HPRE_4    0x00000090   //!< AHB prescaler = 4
#define RCC_CFGR_HPRE_8    0x000000A0   //!< AHB prescaler = 8
#define RCC_CFGR_HPRE_16   0x000000B0   //!< AHB prescaler = 16
#define RCC_CFGR_HPRE_64   0x000000C0   //!< AHB prescaler = 64
#define RCC_CFGR_HPRE_128  0x000000D0   //!< AHB prescaler = 128
#define RCC_CFGR_HPRE_256  0x000000E0   //!< AHB prescaler = 256
#define RCC_CFGR_HPRE_512  0x000000F0   //!< AHB prescaler = 512

#define RCC_CSR_LSI_ON     0x00000001   //!< LSI oscillator enable
#define RCC_CSR_LSI_RDY    0x00000002   //!< LSI oscillator ready

#define RCC_BDCR_BDRST     0x00010000   //!< Backup domain software reset
#define RCC_BDCR_RTCEN     0x00008000   //!< RTC clock enable
#define RCC_BDCR_RTCSEL    0x00000300   //!< RTC clock source selection
#define RCC_BDCR_SEL_OFF   0x00000000   //!< RTC clock source: No clock
#define RCC_BDCR_SEL_LSE   0x00000100   //!< RTC clock source: LSE clock
#define RCC_BDCR_SEL_LSI   0x00000200   //!< RTC clock source: LSI clock
#define RCC_BDCR_SEL_HSE   0x00000300   //!< RTC clock source: HSE clock
#define RCC_BDCR_LSERDY    0x00000002   //!< External oscillator ready
#define RCC_BDCR_LSEON     0x00000001   //!< External oscillator enable

#define RCC_CSR_LPWRRSTF   0x80000000   //!< Low-power reset flag
#define RCC_CSR_WWDGRSTF   0x40000000   //!< Window watchdog reset flag
#define RCC_CSR_IWDGRSTF   0x20000000   //!< Independent watchdog reset flag
#define RCC_CSR_SFTRSTF    0x10000000   //!< Software reset flag
#define RCC_CSR_PORRSTF    0x08000000   //!< POR/PDR reset flag
#define RCC_CSR_PINRSTF    0x04000000   //!< PIN reset flag
#define RCC_CSR_RMVF       0x01000000   //!< Remove reset flag

#define FLASH_ACR_PRFTBS   0x00000020   //!< Prefetch buffer status
#define FLASH_ACR_PRFTBE   0x00000010   //!< Prefetch buffer enable
#define FLASH_ACR_HLFCYA   0x00000008   //!< Flash half cycle access enable
#define FLASH_ACR_LATENCY  0x00000007   //!< Latency (wait states)

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Layout of processor internal reset & clock control registers (RCC)
typedef volatile struct {
    UInt32 CR;          //!< Clock control register
    UInt32 CFGR;        //!< Clock configuration register
    UInt32 CIR;         //!< Clock interrupt register
    UInt32 APB2RSTR;    //!< APB2 peripheral reset register
    UInt32 APB1RSTR;    //!< APB1 peripheral reset register
    UInt32 AHBENR;      //!< AHB Peripheral Clock enable register
    UInt32 APB2ENR;     //!< APB2 peripheral clock enable register
    UInt32 APB1ENR;     //!< APB1 peripheral clock enable register
    UInt32 BDCR;        //!< Backup domain control register
    UInt32 CSR;         //!< Control/status register
} RccRegFile_t;

//! Layout of processor internal flash control registers (RCC)
typedef volatile struct {
    UInt32 ACR;         //!< Flash access control register
    UInt32 KEYR;        //!< FPEC key register
    UInt32 OPTKEYR;     //!< Flash OPTKEY register
    UInt32 SR;          //!< Flash status register
    UInt32 CR;          //!< Flash control register
    UInt32 AR;          //!< Flash address register
    UInt32 RESERVED;    //!< Reserved
    UInt32 OBR;         //!< Option byte register
    UInt32 WRPR;        //!< Write protection register
} FlashRegFile_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Pointer to hardware reset & clock control registers
static RccRegFile_t *RCC = (RccRegFile_t*) 0x40021000;

//! Pointer to flash control registers
static FlashRegFile_t *FLASH = (FlashRegFile_t*) 0x40022000;

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halInitializePLL (UInt16 Multiplier, UInt16 Divider);



/*****************************************************************************/
/*!
 *  \brief   Enable/disable peripheral clock
 *
 *      Enables or disables the clock for a particular peripheral. The
 *      PeripheralID determines which peripheral to address, the State
 *      parameter determines if the clock has to be enabled (true) or
 *      disables (false).
 *
 *      Clocks for unused peripherals should be disabled to reduce power
 *      dissipation and clock emission. After reset all peripheral
 *      clocks are disabled.
 *
 *  \iparam  PeripheralID = ID of addressed peripheral
 *  \iparam  State        = True: enable, False: disable
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halPeripheralClockEnable (UInt16 PeripheralID, Bool State) {

    volatile UInt32 *ClkEnableRegister = NULL;
    UInt16 BitNumber = PeripheralID & 0x1F;
    UInt16 RegNumber = PeripheralID >> 5;

    if (PeripheralID <= MAX_PERIPHERAL_ID) {
        switch (RegNumber) {
            case 0:
                ClkEnableRegister = &RCC->AHBENR;
                break;

            case 1:
                ClkEnableRegister = &RCC->APB2ENR;
                break;

            case 2:
                ClkEnableRegister = &RCC->APB1ENR;
                break;

            default:
                return (E_INVALID_PERIPHERAL_ID);
        }
        if (State) {
            *ClkEnableRegister |= BIT(BitNumber);
        }
        else {
            *ClkEnableRegister &= ~BIT(BitNumber);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Enable/disable peripheral clock
 *
 *      Sets or releases a particular peripheral into reset state. The
 *      PeripheralID determines which peripheral to address, the State
 *      parameter determines if reset has to be activaed (true) or
 *      deactivated (false).
 *
 *  \iparam  PeripheralID = ID of addressed peripheral
 *  \iparam  State        = True: activate, False: deactivate
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halPeripheralReset (UInt16 PeripheralID, Bool State) {

    volatile UInt32 *ResetCtrlRegister = NULL;
    UInt16 BitNumber = PeripheralID & 0x1F;
    UInt16 RegNumber = PeripheralID >> 5;

    if (PeripheralID <= MAX_PERIPHERAL_ID) {
        switch (RegNumber) {
            case 1:
                ResetCtrlRegister = &RCC->APB2RSTR;
                break;

            case 2:
                ResetCtrlRegister = &RCC->APB1RSTR;
                break;

            default:
                return (E_INVALID_PERIPHERAL_ID);
        }
        if (State) {
            *ResetCtrlRegister |= BIT(BitNumber);
        }
        else {
            *ResetCtrlRegister &= ~BIT(BitNumber);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Get processor clock rate
 *
 *      Returns actual processor clock rate in Hz. Results are correct
 *      only, if frequency of the external crystal is properly defined
 *      in the HAL configuration file.
 *
 *  \return  Processor clock rate [Hz]
 *
 ****************************************************************************/

UInt32 halGetProcessorClockRate (void) {

    UInt32 ClockSource = (RCC->CFGR >> 2) & RCC_CFGR_SW;

    if (ClockSource == RCC_CFGR_SW_PLL) {

        UInt32 Division = ((RCC->CFGR & RCC_CFGR_PLL_XTPRE)) ? 2 : 1;
        UInt32 Multiply = ((RCC->CFGR & RCC_CFGR_PLL_MUL) >> 18) + 2;

        if (Multiply >= 16) {
            return (halCrystalFrequency / Division * 16);
        }
        return (halCrystalFrequency / Division * Multiply);
    }
    else if (ClockSource == RCC_CFGR_SW_HSE) {
        return (halCrystalFrequency);
    }
    else if (ClockSource == RCC_CFGR_SW_HSI) {
        return (HSI_FREQUENCY);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Get peripheral clock rate
 *
 *      Returns actual peripheral clock rate in Hz. Results are correct
 *      only, if frequency of the external crystal is properly defined
 *      in the HAL configuration file. APB1 and APB2 clocks are assumed
 *      to be equal (same divider settings).
 *
 *  \return  Peripheral clock rate [Hz]
 *
 ****************************************************************************/

UInt32 halGetPeripheralClockRate (void) {

    UInt32 PeripheralClock = halGetProcessorClockRate();
    UInt32 Prescaler;

    // Analyse AHB clock divider
    Prescaler = (RCC->CFGR & RCC_CFGR_HPRE) >> 4;
    if (Prescaler >= 12) {
        PeripheralClock /= 2UL << (Prescaler - 7);
    }
    else if (Prescaler >= 8) {
        PeripheralClock /= 2UL << (Prescaler - 8);
    }
    // Analyse APB clock divider
    Prescaler = (RCC->CFGR & RCC_CFGR_PPRE1) >> 8;
    if (Prescaler >= 4) {
        PeripheralClock /= 2UL << (Prescaler - 4);
    }
    return (PeripheralClock);
}


/*****************************************************************************/
/*!
 *  \brief   Get real time clock rate
 *
 *      Returns actual real time clock rate in Hz. If the real time clock
 *      source isn't configured, it will be configured here based on the
 *      halRtcClockFrequency constant defined in the HAL configuration.
 *      If halRtcClockFrequency is 0, the system clock crystal is used
 *      as a clock source. If halRtcClockFrequency is > 0 it is assumed
 *      to be the frequency of a dedicated real time clock crystal,
 *      which is then used as clock source. If the clock source differs
 *      from the one actually configured, a backup domain reset will be
 *      performed. This also erases the backup storage content.
 *
 *  \return  Real time clock rate [Hz]
 *
 ****************************************************************************/

UInt32 halGetRealTimeClockRate (void) {

    UInt32 OldState = RCC->BDCR & (RCC_BDCR_RTCSEL | RCC_BDCR_LSEON | RCC_BDCR_RTCEN);
    UInt32 NewState;

    if (halRtcClockFrequency) {
        // Use dedicated real time clock oscillator
        NewState = RCC_BDCR_SEL_LSE | RCC_BDCR_LSEON | RCC_BDCR_RTCEN;
    }
    else {
        // Use system clock divided by 128
        NewState = RCC_BDCR_SEL_HSE | RCC_BDCR_RTCEN;
    }
    // Check if clock is already running in that mode
    if (NewState != OldState) {
        RCC->BDCR = RCC_BDCR_BDRST;
        RCC->BDCR = NewState & ~RCC_BDCR_RTCEN;
        if (halRtcClockFrequency) {    
            while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
        }
        RCC->BDCR |= RCC_BDCR_RTCEN;        
    }
    if (halRtcClockFrequency) {
        return (halRtcClockFrequency);
    }
    return (halCrystalFrequency / 128);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize PLL and dividers
 *
 *      Initializes the system clock PLL and the peripheral clock dividers.
 *      The system clock is derived from an external crystal or oscillator
 *      which is multiplied and/or divided using a phase-locked-loop (PLL).
 *      For best performance the system clock should be near the specified
 *      maximum clock rate for the microcontroller.
 *
 *  \iparam  Multiplier = Crystal clock multiplier
 *  \iparam  Divider    = Crystal clock divider (before PLL)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halInitializePLL (UInt16 Multiplier, UInt16 Divider) {

    UInt32 TempReg;

    if (Multiplier < PLL_MIN_MULIPLIER || Multiplier > PLL_MAX_MULIPLIER) {
        return (E_SYSCLOCK_MULTIPLIER);
    }
    if (Divider < PLL_MIN_DIVIDER || Divider > PLL_MAX_DIVIDER) {
        return (E_SYSCLOCK_DIVIDER);
    }
    // Disable all peripheral clocks
    RCC->AHBENR = RCC->APB2ENR = RCC->APB1ENR = 0;

    // Enable HSE clock oszillator
    RCC->CR |= RCC_CR_HSE_ON;
    while (!(RCC->CR & RCC_CR_HSE_RDY)) {}

    // Select HSE as PLL clock source
    RCC->CFGR = RCC_CFGR_PLL_SRC | RCC_CFGR_SW_HSE;

    // Setup PLL multiplier and divider
    if (Divider > 1) {
        RCC->CFGR |= RCC_CFGR_PLL_XTPRE;
    }
    RCC->CFGR |= ((Multiplier - 2) << 18) & RCC_CFGR_PLL_MUL;

    // Configure prescalers (ADC:6, AHB:1, APB1:2, APB2:1)
    RCC->CFGR |= RCC_CFGR_ADCPRE_6 |
                 RCC_CFGR_PPRE2_2  |
                 RCC_CFGR_PPRE1_2  |
                 RCC_CFGR_HPRE_1;

    // Wait until PLL is locked
    RCC->CR |= RCC_CR_PLL_ON;
    while (!(RCC->CR & RCC_CR_PLL_RDY)) {}

    // Select PLL as clock source
    TempReg = 
        (RCC->CFGR & ~RCC_CFGR_SW_HSE) | RCC_CFGR_SW_PLL | RCC_CFGR_MCO_HSE;
    RCC->CFGR = TempReg;

    // Enable LSI oscillator and wait 'till stable
    RCC->CSR = RCC_CSR_LSI_ON;
    while (!(RCC->CSR & RCC_CSR_LSI_RDY)) {}

    RCC->CIR  = 0;   // disable clock interrupts

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Get reset reason
 *
 *      Returns the cause of the latest reset. Following reasons can
 *      cause a reset:
 *
 *      - Watchdog timeout
 *      - Software request
 *      - Hardware request (NRST-Pin)
 *      - Standby/stop of processor
 *      - Power up
 *
 *  \return  Reset reason
 *
 ****************************************************************************/

ResetReason_t halGetResetReason (void) {

    static ResetReason_t ResetReason = RESET_CAUSE_UNKNOWN;

    if (ResetReason == RESET_CAUSE_UNKNOWN) {

        if (RCC->CSR & RCC_CSR_WWDGRSTF) {
            ResetReason = RESET_CAUSED_BY_WATCHDOG;
        }
        else if (RCC->CSR & RCC_CSR_IWDGRSTF) {
            ResetReason = RESET_CAUSED_BY_WATCHDOG;
        }
        else if (RCC->CSR & RCC_CSR_SFTRSTF) {
            ResetReason = RESET_CAUSED_BY_SOFTWARE;
        }
        else if (RCC->CSR & RCC_CSR_PORRSTF) {
            ResetReason = RESET_CAUSED_BY_POWER_ON;
        }
        else if (RCC->CSR & RCC_CSR_PINRSTF) {
            ResetReason = RESET_CAUSED_BY_HARDWARE;
        }
        else if (RCC->CSR & RCC_CSR_LPWRRSTF) {
            ResetReason = RESET_CAUSED_BY_STANDBY;
        }
        RCC->CSR |= RCC_CSR_RMVF;
    }
    return (ResetReason);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize system clocks
 *
 *      Initializes this module by setting up the PLL and the peripheral
 *      clock dividers.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halClocksInit (void) {

    UInt32 Quotient;
    UInt32 Division;
    UInt32 Multiply;
    Error_t Status;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        if (!halSystemFrequency || !halCrystalFrequency) {
            return (E_SYSCLOCK_RATE);
        }
        if (halSystemFrequency > halProcessorInfo.ClockRate * 1000000) {
            return (E_SYSCLOCK_RATE);
        }     
        FLASH->ACR = FLASH_ACR_PRFTBE | 2;
        //halFlashSetWaitStates (halSystemFrequency);
    
        Quotient = halSystemFrequency * 2 / halCrystalFrequency;
        Division = (Quotient % 2) + 1;
        Multiply = (Quotient * Division / 2);

        if (Quotient == 2) {
            Division = Multiply = 2;
        }
        Status = halInitializePLL (Multiply, Division);
        if (Status != NO_ERROR) {
            return (Status);
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/

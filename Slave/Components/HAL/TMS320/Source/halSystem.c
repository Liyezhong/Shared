/****************************************************************************/
/*! \file halSystem.c
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

#include "Global.h"
#include "halCommon.h"
#include "halError.h"
#include "halConfigure.h"
#include "halInterrupt.h"
#include "halPorts.h"
#include "halAnalog.h"
#include "halTimer.h"
#include "halSerial.h"
#include "halSystem.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define XCLKOUT_DIVIDE_RATIO  2         // 0...3
#define XCLKOUT_DIVIDE_MASK   0x0003    //!< Bitmask for divide ratio

#define MIN_PLL_MULIPLIER     1         //!< Minimal PLL multiplier
#define MAX_PLL_MULIPLIER     10        //!< Maximal PLL multiplier

#define PLLSTS_MCLKSTS_MASK   0x0008    //!< PLL missing clock status bit
#define PLLSTS_MCLKOFF_MASK   0x0040    //!< PLL missing clock-detect disable
#define PLLSTS_CLKINDIV_MASK  0x0002    //!< PLL clock divide bit (28xx only)
#define PLLSTS_PLLLOCKS_MASK  0x0001    //!< PLL lock status bit

#define PLLSTS_DIVSEL_MASK    0x0180    //!< Divide select bits
#define PLLSTS_DIVIDE_BY_4    0x0080    //!< Divide PLL out by 4
#define PLLSTS_DIVIDE_BY_2    0x0100    //!< Divide PLL out by 2
#define PLLSTS_DIVIDE_BY_1    0x0180    //!< Divide PLL out by 1

#define PLLCR_DIVIDER_MASK    0x000F    //!< PLL divider bitmask (PLL control)

#define SCSR_WDINTS           0x0004    //!< Watchdog interrupt status bit
#define SCSR_ENINT            0x0002    //!< Watchdog interrupt enable
#define SCSR_OVERRIDE         0x0001    //!< Watchdog override bit
#define WDCR_WDFLAG           0x0080    //!< Watchdog reset status flag bit
#define WDCR_WDDIS            0x0040    //!< Watchdog disable
#define WDCR_WDCHK            0x0028    //!< Watchdog check bits
#define WDCR_WDPS             7         //!< Watchdog clock prescaler

#define WDKEY_PATTERN_1       0x55      //!< Watchdog key pattern 1
#define WDKEY_PATTERN_2       0xAA      //!< Watchdog key pattern 2

#define MAX_PERIPHERAL_ID     PERIPHERAL_CLOCK_ID(3,31)

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Layout of processor internal system control registers */
typedef struct {
   UInt16 XCLK;             //!< XCLKOUT control register (only on C28xx)
   UInt16 PLLSTS;           //!< PLL statas register
   UInt16 Reserved1[7];     //!< Reserved
   UInt16 PCLKCR2;          //!< Peripheral clock control register 2
   UInt16 HISPCP;           //!< High-speed peripheral clock pre-scaler
   UInt16 LOSPCP;           //!< Low-speed peripheral clock pre-scaler
   UInt16 PCLKCR0;          //!< Peripheral clock control register 0
   UInt16 PCLKCR1;          //!< Peripheral clock control register 1
   UInt16 LPMCR0;           //!< Low-power mode control register 0
   UInt16 Reserved2[1];     //!< Reserved
   UInt16 PCLKCR3;          //!< Peripheral clock control register 3
   UInt16 PLLCR;            //!< PLL control register
   UInt16 SCSR;             //!< System control and status register      
   UInt16 WDCNTR;           //!< WD counter register
   UInt16 Reserved3[1];     //!< Reserved
   UInt16 WDKEY;            //!< WD reset key register
   UInt16 Reserved4[3];     //!< Reserved
   UInt16 WDCR;             //!< WD timer control register
   UInt16 Reserved5[6];     //!< Reserved  
} SysCtrlRegs_t;

static volatile SysCtrlRegs_t* SysCtrl = (SysCtrlRegs_t*) 0x7010;


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static ERROR_t halInitializeClocks (UInt16 Multiplier, UInt16 Divider);


/*****************************************************************************/
/*! 
 *  \brief   Trigger watchdog timer
 *
 *      Triggers and resets the watchdog timer. This function must be called
 *      in a regular time interval to prevent the watchdog from initiating a
 *      hardware reset. 
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halWatchdogTrigger (void)
{
    halUnprotectRegisters();
    SysCtrl->WDKEY = WDKEY_PATTERN_1;
    SysCtrl->WDKEY = WDKEY_PATTERN_2;
    halProtectRegisters();

    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Disable watchdog timer
 *
 *      Disables the watchdog timer. Disabling the watchdog is possible only
 *      in debug mode, i.e. if the override bit in the watchdog control 
 *      register is set.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halWatchdogDisable (void)
{
    if (SysCtrl->WDCR & SCSR_OVERRIDE) {
                
        halUnprotectRegisters();
        SysCtrl->WDCR = WDCR_WDDIS + WDCR_WDCHK;
        halProtectRegisters();    
        return (NO_ERROR);
    }
    return (halSetError(E_UNSUPPORTED_FUNCTION));
}


/*****************************************************************************/
/*! 
 *  \brief   Enable watchdog timer
 *
 *      Enables the watchdog timer. If DEBUG is defined, the override bit
 *      remains set, e.g. the watchdog can be disabled later. In the
 *      opposite case the override bit is cleared, what means that the
 *      watchdog can't be stopped anymore.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halWatchdogEnable (void)
{
    halUnprotectRegisters();
    SysCtrl->WDCR = WDCR_WDCHK + WDCR_WDPS;
    #ifndef DEBUG
        SysCtrl->SCSR |= SCSR_OVERRIDE;
    #endif
    halProtectRegisters();

    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Force a hardware reset
 *
 *      Force a hardware reset by writing an invalid pattern to the
 *      watchdog check bits in the watchdog control register.
 * 
 *  \return  Never returns
 *
 ****************************************************************************/

void halHardwareReset (void)
{
    halUnprotectRegisters();
    SysCtrl->WDCR = 0;
    halProtectRegisters();

    for (;;);   // wait for hardware reset
}


/*****************************************************************************/
/*! 
 *  \brief   Get reset reason
 *
 *      Returns the cause of the latest reset, i.e. if it is caused by a
 *      not triggered watchdog or by a normal external reset.
 * 
 *  \return  Reset reason
 *
 ****************************************************************************/

RESET_REASON_t halGetResetReason (void) {
    
    if (SysCtrl->WDCR & WDCR_WDFLAG) {
        return (RESET_CAUSED_BY_WATCHDOG);   
    }
    return (RESET_CAUSED_BY_POWER_UP);
}


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

ERROR_t halPeripheralClockEnable (UInt16 PeripheralID, Bool State)
{
    const UInt16 BitNumber = PeripheralID & 0x1F;
    const UInt16 RegNumber = PeripheralID >> 5;
    volatile UInt16 *PClkCtrlRegister;
    
    if (RegNumber < 3) {
        switch (RegNumber) {
            case 0:
                PClkCtrlRegister = &SysCtrl->PCLKCR0; 
                break;
                
            case 1:
                PClkCtrlRegister = &SysCtrl->PCLKCR1; 
                break;
                
            case 2:   
                PClkCtrlRegister = &SysCtrl->PCLKCR2; 
                break;
                
            case 3:   
                PClkCtrlRegister = &SysCtrl->PCLKCR3; 
                break;
        }
        if (State) {
            *PClkCtrlRegister |= (1 << BitNumber);
        }
        else {
            *PClkCtrlRegister &= ~(1 << BitNumber);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize system clocks
 *
 *      Initializes the system clock PLL and the peripheral clock dividers.
 *      The system clock is derived from an external crystal or oscillator
 *      which is multiplied and/or divided using a phase-locked-loop (PLL).
 *      The system clock should be near the specified clock rate for the
 *      microcontroller.
 * 
 *  \iparam  Multiplier = Clock-in multiplicator
 *  \iparam  Divider    = Clock-in divider 
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halInitializeClocks (UInt16 Multiplier, UInt16 Divider)
{
    UInt16 i;
    
    if (Divider != 1 && Divider != 2 && Divider != 4) {
        return (halSetError(E_PARAMETER_OUT_OF_RANGE));
    }
    if (Divider == 1 && Multiplier > 0) {
        return (halSetError(E_PARAMETER_OUT_OF_RANGE));
    }
    if (Multiplier < MIN_PLL_MULIPLIER || Multiplier > MAX_PLL_MULIPLIER) {
        return (halSetError(E_PARAMETER_OUT_OF_RANGE));
    }
   // Make sure the PLL is not running in limp mode
    if (SysCtrl->PLLSTS & PLLSTS_MCLKSTS_MASK) {
        return (halSetError(E_HAL_NO_SYSTEM_CLOCK));
    }               
    SysCtrl->PLLSTS &= ~(PLLSTS_DIVSEL_MASK | PLLSTS_CLKINDIV_MASK);
    
    if ((SysCtrl->PLLCR & PLLCR_DIVIDER_MASK) != Multiplier) {
    
        SysCtrl->PLLSTS |= PLLSTS_MCLKOFF_MASK;
        SysCtrl->PLLCR   = Multiplier;

        // Wait for PLL to lock
        while (!(SysCtrl->PLLSTS & PLLSTS_PLLLOCKS_MASK)) {}
    }
    if (Divider == 1) {
        SysCtrl->PLLSTS |= PLLSTS_DIVIDE_BY_1;
        SysCtrl->PLLSTS |= PLLSTS_CLKINDIV_MASK;
    }
    else if (Divider == 2) {
        SysCtrl->PLLSTS |= PLLSTS_DIVIDE_BY_2;
    }
    else {
        SysCtrl->PLLSTS |= PLLSTS_DIVIDE_BY_4;
    }
    // XCLKOUT to SYSCLKOUT ratio
    #ifndef PROCESSOR_2823x 
    SysCtrl->XCLK = XCLKOUT_DIVIDE_RATIO & XCLKOUT_DIVIDE_MASK;
    #endif        
    SysCtrl->PLLSTS &= ~PLLSTS_MCLKOFF_MASK;
    
    // Disable all peripheral clocks
    for (i=0; i < MAX_PERIPHERAL_ID; i++) {
        halPeripheralClockEnable(i, OFF);
    }
    // HISPCP/LOSPCP prescale register settings
    SysCtrl->HISPCP = HiSPEED_CLOCK_DIVIDER;
    SysCtrl->LOSPCP = LoSPEED_CLOCK_DIVIDER;

    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Idel task function
 *
 *      This function is called each time the HAL layer is waiting for an 
 *      event. The application can register a callback which is called when 
 *      entering this state.
 * 
 *  \return  Nothing
 *
 ****************************************************************************/

void halIdleTask (void) {   
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize HAL layer
 *
 *      Initializes the complete hardware abstraction layer by calling all
 *      initialization functions in the other HAL layer modules. Interrupts
 *      are disabled and access to the protected hardware registers are
 *      allowed until initialization is done.
 * 
 *      If initialization fails the function is immediately aborted and
 *      an error code is returned. The hardware is in an undefined state
 *      in this case and the HAL layer function shouldn't be used.
 *      
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/


ERROR_t halInitializeHalLayer (void) {
    
    halGlobalInterruptDisable();    
    halUnprotectRegisters();
    
    if (halInitializeClocks (PLL_CLOCK_MULIPLIER, PLL_CLOCK_DIVIDER) < 0) {        
        halProtectRegisters();
        return (halGetError());
    }
    if (halInitializeInterrupts() != NO_ERROR ||
        halInitializePorts()  != NO_ERROR ||
        halInitializeTimers() != NO_ERROR ||
        halAnalogInitialize() != NO_ERROR ||
        halSerialInitialize() != NO_ERROR) {
            
        halProtectRegisters();
        return (halGetError());
    }    
    halProtectRegisters();
    halGlobalInterruptEnable();    
    
    return (NO_ERROR);
}

//****************************************************************************/

/****************************************************************************/
/*! \file halTimer.c
 *
 *  \brief  Timer access function
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the processor
 *      internal hardware timers, including capture/compare units. 
 *      The timer counters and prescalers are 16 bit wide on the STM32.
 *      Anyway, the interface uses 32 bit parameters for these items.
 *      This allows porting to platforms offering 32 bit counters w/o
 *      changing the interface. Applications should determine the real
 *      width of the counter by calling halTimerStatus, and adapt them-
 *      self to the real width. 
 *
 *      The direction and continuous/one-shot modes are available on most
 *      hardware plattforms (or can be simulated in software), and can
 *      therefore be used without portability risks. The external clock
 *      modes on the other hand may not be available on other processors.
 *      Using one of these modes may lead to incompatibilities if porting
 *      the HAL to another processor architecture is neccessary.
 *
 *      The STM32 supports up to 4 capture/compare units per timer. The
 *      various processors of the STM32 family implement different sub-
 *      sets of the timers, and the timers have different numbers of
 *      capture/compare units. Additionally, some of the timers can only
 *      count up, while others can count up, down, or up/down.
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
#include "halClocks.h"
#include "halMain.h"
#include "halInterrupt.h"
#include "halTimer.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define TIM_CR1_CEN        0x0001u  //!< Counter enable
#define TIM_CR1_ARPE       0x0080u  //!< Auto-reload prescaler enable
#define TIM_CR1_CMS        0x0060u  //!< Center aligned mode
#define TIM_CR1_DIR        0x0010u  //!< Count direction (0=up, 1=down)
#define TIM_CR1_OPM        0x0008u  //!< One-pulse mode enable
#define TIM_CR1_URS        0x0004u  //!< Update request selection
#define TIM_EGR_UG         0x0001u  //!< Update event generation bit

#define TIM_SMCR_ETP       0x8000u  //!< External trigger polarity
#define TIM_SMCR_ECE       0x4000u  //!< External clock enable
#define TIM_SMCR_ETPS      0x3000u  //!< External trigger prescaler
#define TIM_SMCR_ETF       0x0F00u  //!< External trigger filter
#define TIM_SMCR_TS        0x0070u  //!< Trigger selection
#define TIM_SMCR_SMS       0x0007u  //!< Slave mode selection

#define TIM_DIER_UIE       0x0001u  //!< Update interrupt enable
#define TIM_DIER_CC1IE     0x0002u  //!< Capture/compare 1 interrupt enable
#define TIM_DIER_CC2IE     0x0004u  //!< Capture/compare 2 interrupt enable
#define TIM_DIER_CC3IE     0x0008u  //!< Capture/compare 3 interrupt enable
#define TIM_DIER_CC4IE     0x0010u  //!< Capture/compare 4 interrupt enable

#define TIM_CCER_CC1E      0x0001u  //!< Capture/compare unit 1 enable
#define TIM_CCER_CC2E      0x0010u  //!< Capture/compare unit 2 enable
#define TIM_CCER_CC3E      0x0100u  //!< Capture/compare unit 3 enable
#define TIM_CCER_CC4E      0x1000u  //!< Capture/compare unit 4 enable

#define TIM_SR_UIF         0x0001u   //!< Update interrupt flag
#define TIM_SR_CC1IF       0x0002u   //!< Capture/Compare 1 interrupt flag
#define TIM_SR_CC2IF       0x0004u   //!< Capture/Compare 2 interrupt flag
#define TIM_SR_CC3IF       0x0008u   //!< Capture/Compare 3 interrupt flag
#define TIM_SR_CC4IF       0x0010u   //!< Capture/Compare 4 interrupt flag
#define TIM_SR_CC1OF       0x0200u   //!< Capture 1 overflow flag
#define TIM_SR_CC2OF       0x0400u   //!< Capture 2 overflow flag
#define TIM_SR_CC3OF       0x0800u   //!< Capture 3 overflow flag
#define TIM_SR_CC4OF       0x1000u   //!< Capture 4 overflow flag
#define TIM_SR_IFLAGS      0x00FFu   //!< Interrupt flag mask

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Layout of processor internal timer control registers
typedef volatile struct {
    UInt32 CR1;             //!< Timer control register 1
    UInt32 CR2;             //!< Timer control register 2
    UInt32 SMCR;            //!< Timer slave mode control register
    UInt32 DIER;            //!< Timer DMA/Interrupt enable register
    UInt32 SR;              //!< Timer status register
    UInt32 EGR;             //!< Timer event generation register
    UInt32 CCMR[2];         //!< Timer capture/compare mode registers
    UInt32 CCER;            //!< Timer capture/compare enable register
    UInt32 CNT;             //!< Timer counter register
    UInt32 PSC;             //!< Timer prescaler register
    UInt32 ARR;             //!< Timer auto-reload register
    UInt32 RCR;             //!< Timer repetition counter register
    UInt32 CCR[4];          //!< Timer capture/compare registers 1...4
    UInt32 BDTR;            //!< Timer break and dead-time register
    UInt32 DCR;             //!< Timer DMA control register
    UInt32 DMAR;            //!< Timer DMA address for full transfer
} TimerRegFile_t;

//! Structure to hold the state of a serial communication device
typedef struct {
    TimerRegFile_t *TIM;    //!< Pointer to register file
    UInt16 PeripheralID;    //!< Peripheral identifier
    UInt16 InterruptID;     //!< Interrupt identifier
    UInt16 CapComs;         //!< Number of capture/compare units
    UInt16 Mode[4];         //!< Capture/compare channel mode
    UInt16 Flags;           //!< Open flags
    UInt16 Priority;        //!< Interrupt priority
} TimerDevice_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Data table holding the variables for all serial channels
static TimerDevice_t DataTable[] = {
    { (TimerRegFile_t*) 0x40012C00, PERIPHERAL_TIMER1,  INTERRUPT_TIMER1,  4 },
    { (TimerRegFile_t*) 0x40000000, PERIPHERAL_TIMER2,  INTERRUPT_TIMER2,  4 },
    { (TimerRegFile_t*) 0x40000400, PERIPHERAL_TIMER3,  INTERRUPT_TIMER3,  4 },
    { (TimerRegFile_t*) 0x40000800, PERIPHERAL_TIMER4,  INTERRUPT_TIMER4,  4 },
    { (TimerRegFile_t*) 0x40000C00, PERIPHERAL_TIMER5,  INTERRUPT_TIMER5,  4 },
    { (TimerRegFile_t*) 0x40001000, PERIPHERAL_TIMER6,  INTERRUPT_TIMER6,  0 },
    { (TimerRegFile_t*) 0x40001400, PERIPHERAL_TIMER7,  INTERRUPT_TIMER7,  0 },
    { (TimerRegFile_t*) 0x40013400, PERIPHERAL_TIMER8,  INTERRUPT_TIMER8,  4 },
    { (TimerRegFile_t*) 0x40014C00, PERIPHERAL_TIMER9,  INTERRUPT_TIMER9,  2 },
    { (TimerRegFile_t*) 0x40015000, PERIPHERAL_TIMER10, INTERRUPT_TIMER10, 1 },
    { (TimerRegFile_t*) 0x40015400, PERIPHERAL_TIMER11, INTERRUPT_TIMER11, 1 },
    { (TimerRegFile_t*) 0x40001800, PERIPHERAL_TIMER12, INTERRUPT_TIMER12, 2 },
    { (TimerRegFile_t*) 0x40001C00, PERIPHERAL_TIMER13, INTERRUPT_TIMER13, 1 },
    { (TimerRegFile_t*) 0x40002000, PERIPHERAL_TIMER14, INTERRUPT_TIMER14, 1 },
    { (TimerRegFile_t*) 0x40014000, PERIPHERAL_TIMER15, INTERRUPT_TIMER9,  2 },
    { (TimerRegFile_t*) 0x40014400, PERIPHERAL_TIMER16, INTERRUPT_TIMER10, 1 },
    { (TimerRegFile_t*) 0x40014800, PERIPHERAL_TIMER17, INTERRUPT_TIMER11, 1 }
};

//! Interrupt vector table for all timers
static InterruptVector_t TimerIntrVectors[ELEMENTS(DataTable)] = {0};

//! Default timer mode (can be used as parameter for halTimerSetup)
const TimerMode_t halTimerDefaultMode = {
    TIM_MODE_COUNT_DOWN, TIM_MODE_INTERVAL, TIM_MODE_INTERNAL, 0 };


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halTimerGetIndex   (Handle_t Handle);
static Error_t halTimerFindDevice (Device_t DeviceID);


/*****************************************************************************/
/*!
 *  \brief   Open a timer
 *
 *      Opens a hardware timer and returns a handle to be used to access it.
 *      DeviceID must be the numerical "name" of a timer defined in the HAL
 *      configuration file. The timer is set to its default mode: count up
 *      continuously using internal clock with prescale 1.
 *
 *      Handler must be a pointer to an interrupt handler function to be 
 *      called when the device signals an interrupt. It can be NULL if no
 *      interrupts are used. UserTag is a user defined number to be passed 
 *      to the handler function, e.g. as index into a data table in case 
 *      of multiple instances. 
 *
 *      The timer remains in stopped state and must be started explicitly.
 *
 *  \iparam  DeviceID = Logical timer ID
 *  \iparam  UserTag  = User tag to pass thru to interrupt handler
 *  \iparam  Handler  = Interrupt handler function
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halTimerOpen (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler) {

    const Int32 Index = halTimerFindDevice(DeviceID);
    Error_t Status;
    UInt16 UnitNo;

    if (Index >= 0) {
        Handle_t Handle = DEVICE_CLASS_TIMER + Index;
        TimerDevice_t *Data = &DataTable[Index];

        if (Data->Flags & HAL_OPEN_RW) {
            return (E_DEVICE_ALREADY_OPEN);
        }
        Data->Flags |= HAL_OPEN_RW;

        // Enable peripheral clock and do basic hardware setup
        Data->TIM->CR1  = TIM_CR1_URS | TIM_CR1_ARPE;
        Data->TIM->ARR  = MAX_UINT32;
        Data->TIM->SR   = 0;
        Data->TIM->DIER = 0;

        // Clear capture/compare registers
        for (UnitNo=0; UnitNo < Data->CapComs; UnitNo++) {
            Data->TIM->CCR[UnitNo] = 0;
        }
        // Set timer mode; if failed close timer and return error
        Status = halTimerSetup (Handle, &halTimerDefaultMode, 1);
        if (Status != NO_ERROR) {
             halTimerClose (Handle);
             return (Status);
        }
        Status = halInterruptAcquire(Data->InterruptID, Index);
        if (Status != NO_ERROR) {
            halTimerClose (Handle);
            return (Status);
        }
        TimerIntrVectors[Index].Handler = Handler;
        TimerIntrVectors[Index].UserTag = UserTag;

        if (Handler != NULL) {
            halInterruptEnable (Data->InterruptID, Data->Priority);
        }
        return (Handle);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Close an open timer
 *
 *      Closes an open timer, stops it and disables timer interrupts.
 *
 *  \iparam  Handle = Handle of open analog channel
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halTimerClose (Handle_t Handle) {

    const Int32 Index = halTimerGetIndex(Handle);

    if (Index >= 0) {
        halInterruptDisable (DataTable[Index].InterruptID);
        halInterruptRelease (DataTable[Index].InterruptID);

        DataTable[Index].Flags &= ~HAL_OPEN_MODE;
        DataTable[Index].TIM->CR1  = 0;
        DataTable[Index].TIM->DIER = 0;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Setup operation mode of timer
 *
 *      Sets or changes the operational mode of a time according to the
 *      given parameters. The following mode parameters can be set:
 *
 *      - Count direction (up, down, center aligned)
 *      - Continuous vs. one-shot mode
 *      - Clock source (internal, external)
 *      - Clock mode (trigger, reset, gated, encoder)
 *      - Prescale count
 *
 *      If Mode is NULL and/or Prescale is 0, that particular parameter is
 *      not evaluated and the related settings are left unchanged. This
 *      allows to change for instance only the prescaler or only the mode.
 *
 *      Counting down or center aligned is not supported by all timers.
 *      The clock modes offered are described in details in the reference
 *      manual of the STM32F103xx microcontroller (see SMCR register bits
 *      SMS for the timers).
 *
 *  \iparam  Handle   = Handle of open timer
 *  \iparam  Mode     = Pointer to mode definition
 *  \iparam  Prescale = Prescale count
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halTimerSetup (Handle_t Handle, const TimerMode_t *Mode, UInt32 Prescale) {

    const Int32 Index = halTimerGetIndex(Handle);
    UInt16 Control;

    if (Index >= 0) {
        register TimerRegFile_t *TIM = DataTable[Index].TIM;

        if (Mode != NULL) {
            // Setup count direction and continuous/one-shot mode
            Control = TIM->CR1  & ~(TIM_CR1_DIR | TIM_CR1_OPM | TIM_CR1_CMS);
            if (DataTable[Index].CapComs < 4 && Mode->Direction) {
                return (E_TIMER_COUNT_DIRECTION);
            }
            if (Mode->Direction & 1) {
                Control |= TIM_CR1_DIR;
            }
            if (Mode->Direction & 2) {
                Control |= TIM_CR1_CMS;
            }
            if (Mode->OneShot) {
                Control |= TIM_CR1_OPM;
            }
            TIM->CR1 = Control;

            // Setup clock source and clock mode
            Control = TIM->SMCR & ~(TIM_SMCR_SMS | TIM_SMCR_ECE); 
            if (Mode->ClkSource) {
                Control |= TIM_SMCR_ECE;
            }
            if (Mode->ClkMode) {
                Control |= (Mode->ClkMode & TIM_SMCR_SMS);
            }
            TIM->SMCR = Control;
        }
        // Setup precaler
        if (Prescale) {
            if (Prescale > TIMER_MAX_VALUE) {
                return (E_VALUE_OUT_OF_RANGE);
            }
            TIM->PSC = Prescale - 1;
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Read timer register
 *
 *      Reads a timer register of the timer associated with Handle and
 *      copies it to the variable pointed to by Value.Depending on RegID
 *      one of the following registers are read:
 *
 *      - Timer auto reload register
 *      - Timer counter register
 *      - Timer prescaler register
 *
 *  \iparam  Handle = Handle of open timer
 *  \iparam  RegID  = Timer register ID
 *  \iparam  Value  = Pointer to variable to store the result
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halTimerRead (Handle_t Handle, TimRegsID_t RegID, UInt32 *Value) {

    const Int32 Index = halTimerGetIndex(Handle);

    if (Index >= 0) {
        TimerRegFile_t *TIM = DataTable[Index].TIM;

        if (Value == NULL) {
            return (E_INVALID_PARAMETER);
        }
        switch (RegID) {
            case TIM_REG_COUNTER:
                *Value = (UInt16) TIM->CNT;
                break;

            case TIM_REG_RELOAD:
                *Value = (UInt16) TIM->ARR;
                break;

            case TIM_REG_PRESCALER:
                *Value = (UInt16) TIM->PSC + 1;
                break;

            default:
                return (E_INVALID_REGISTER_ID);
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Writes timer register
 *
 *      Writes a new value into the timer register. Depending on the RegID
 *      parameter one of the following registers are written to:
 *
 *      - Timer auto reload register
 *      - Timer counter register
 *      - Timer prescaler register
 *
 *      The timer update interrupt flag is cleared in any case.
 *
 *  \iparam  Handle = Handle of open timer
 *  \iparam  RegID  = Timer register ID
 *  \iparam  Value  = Value to write
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halTimerWrite (Handle_t Handle, TimRegsID_t RegID, UInt32 Value) {

    const Int32 Index = halTimerGetIndex(Handle);

    if (Index >= 0) {
        TimerRegFile_t *TIM = DataTable[Index].TIM;

        if (Value > TIMER_MAX_VALUE) {
            return (E_VALUE_OUT_OF_RANGE);
        }
        switch (RegID) {
            case TIM_REG_RELOAD:
                TIM->ARR = Value;       
                break;

            case TIM_REG_COUNTER:
                TIM->CNT = Value;       
                break;

            case TIM_REG_PRESCALER:
                TIM->PSC = Value - 1;
                break;

            default:
                return (E_INVALID_REGISTER_ID);
        }
        // Clear update interrupt flag
        TIM->SR = ~TIM_SR_UIF;      
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get status of a timer
 *
 *      Returns the status of the timer associated with Handle. Which status
 *      to return depends on the supplied StatusID. Following informations
 *      can be requested:
 *
 *      - Maximal counter value
 *      - Timer input clock rate (internal clock only)
 *      - Timer count rate (in internal clock mode only)
 *      - Number of capture/compare channels
 *      - Actual count direction
 *      - Pending interrupt flags
 *
 *      If a unsupported StatusID is supplied, a 0 is returned.
 *
 *  \iparam  Handle   = Handle of open timer
 *  \iparam  StatusID = Type of status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halTimerStatus (Handle_t Handle, TimStatID_t StatusID) {

    const Int32 Index = halTimerGetIndex(Handle);

    if (Index >= 0) {
        register TimerRegFile_t *TIM = DataTable[Index].TIM;

        switch (StatusID) {
            case TIM_STAT_PENDING:
                return (TIM->SR & TIM->DIER & TIM_SR_IFLAGS);

            case TIM_STAT_COUNTRATE:
                return (halGetPeripheralClockRate() * 2 / (TIM->PSC + 1));

            case TIM_STAT_CLOCKRATE:
                return (halGetPeripheralClockRate() * 2);

            case TIM_STAT_DIRECTION:
                return (TIM->CR1 & TIM_CR1_DIR);

            case TIM_STAT_MAXCOUNT:
                return (TIMER_MAX_VALUE);

            case TIM_STAT_UNITS:
                return (DataTable[Index].CapComs);
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Controls an open timer
 *
 *      Controls the timer associated with Handle in several ways. Depending
 *      on ControlID the following actions can be initiated:
 *
 *      - Start/stop timer
 *      - Reload/retrigger timer
 *      - Enable/disable timer (update) interrupts
 *      - Reset interrupt flag(s)
 *
 *      If a unsupported ControlID is supplied, a 0 is returned.
 *
 *  \iparam  Handle    = Handle of open timer
 *  \iparam  ControlID = Type of control
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halTimerControl (Handle_t Handle, TimCtrlID_t ControlID) {

    const Int32 Index = halTimerGetIndex(Handle);

    if (Index >= 0) {
        register TimerRegFile_t *TIM = DataTable[Index].TIM;

        switch (ControlID) {
            case TIM_CTRL_RELOAD:
                TIM->EGR |= TIM_EGR_UG;
                break;

            case TIM_CTRL_START: 
                TIM->CR1 |= TIM_CR1_CEN;
                break;

            case TIM_CTRL_STOP: 
                TIM->CR1 &= ~TIM_CR1_CEN;
                break;

            case TIM_INTR_ENABLE:
                TIM->DIER |= TIM_DIER_UIE;
                break;

            case TIM_INTR_DISABLE:
                TIM->DIER &= ~TIM_DIER_UIE;
                break;

            case TIM_INTR_CLEAR:
                TIM->SR &= ~TIM_SR_UIF;   
                break;

            default:
                return (E_UNKNOWN_CONTROL_ID);
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Read from timer capture channel
 *
 *      Reads and returns the actual state of the capture register of the
 *      timer associated with Handle. UnitNo determines which channel of
 *      the capture/compare unit to address. Use halTimerStatus() to
 *      request how many units the timer supports (may vary).
 *
 *      Returns 1 if new data was captured since last reading the unit.
 *      If no capture took place since last reading 0 is returned. If a
 *      capture overflow happens since last read, an error is returned.
 *
 *      Reading a capture channel automatically clears the interrupt flag,
 *      therefore there is no need to do this manually.
 *
 *  \iparam  Handle = Handle of open timer
 *  \iparam  UnitNo = Unit number of the capture/compare channel
 *  \iparam  Value  = Buffer to return capture result
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halCapComRead (Handle_t Handle, UInt16 UnitNo, UInt32 *Value) {

    const Int32 Index = halTimerGetIndex(Handle);
    Int32 Status;

    if (Index >= 0) {
        register TimerRegFile_t *TIM = DataTable[Index].TIM;

        if (UnitNo >= DataTable[Index].CapComs) {
            return (E_TIMER_CAPCOM_CHANNEL);
        }
        Status = TIM->SR & (TIM_SR_CC1IF << UnitNo);
        if (Value != NULL) {
            *Value = TIM->CCR[UnitNo];
        }
        if (TIM->SR & (TIM_SR_CC1OF << UnitNo)) {
            TIM->SR &= ~(TIM_SR_CC1OF << UnitNo);

            return (E_TIMER_CAPCOM_OVERFLOW);
        }
        return (Status ? 1 : 0);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Write to timer compare channel
 *
 *      Writes Value to a capture/compare register of the timer associated
 *      with Handle. UnitNo determines which channel of the capture/compare
 *      unit is addressed. Use halTimerStatus() to request how many units
 *      the timer actually supports.
 *
 *      Writing a compare channel automatically clears the interrupt flag,
 *      therefore there is no need to do this manually.
 *
 *  \iparam  Handle = Handle of open timer
 *  \iparam  UnitNo = Unit number of the capture/compare channel
 *  \iparam  Value  = Value to assign to the compare unit
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halCapComWrite (Handle_t Handle, UInt16 UnitNo, UInt32 Value) {

    const Int32 Index = halTimerGetIndex(Handle);

    if (Index >= 0) {
        if (UnitNo >= DataTable[Index].CapComs) {
            return (E_TIMER_CAPCOM_CHANNEL);
        }
        if (DataTable[Index].Mode[UnitNo] != TIM_SEL_COMPARE) {
            return (E_DEVICE_READ_ONLY);
        }
        if (Value > TIMER_MAX_VALUE) {
            return (E_VALUE_OUT_OF_RANGE);
        }
        DataTable[Index].TIM->SR &= ~(TIM_SR_CC1IF << UnitNo);
        DataTable[Index].TIM->CCR[UnitNo] = Value;

        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Write to timer 1 compare channel - Simplified version
 *
 *      ( \sa halCapComWrite ) 
 *       
 *      This is a simplified version of halCapComWrite to achieve
 *      shorter execution time. It should only be used if time
 *      optimization is important.
 *
 *      The ISR of the stepper function-module uses this version to minimize
 *      execution time of the ISR. This allows faster stepper movement.
 *
 *      This version is fixed to use TIMER 1
 *
 *  \iparam  UnitNo = Unit number of the capture/compare channel
 *  \iparam  Value  = Value to assign to the compare unit
 *
 ****************************************************************************/

void halCapComWriteTimer1_Simplified (UInt16 UnitNo, UInt32 Value) {

    DataTable[0].TIM->SR &= ~(TIM_SR_CC1IF << UnitNo);
    DataTable[0].TIM->CCR[UnitNo] = Value;
}


/*****************************************************************************/
/*!
 *  \brief   Get status of capture/compare unit
 *
 *      Returns the status of a capture/compare unit of the timer associated 
 *      with Handle. Which status to return depends on the supplied StatusID. 
 *      The following informations can be requested:
 *
 *      - Capture/compare match flag bits
 *      - Capture/compare overrun flag bits
 *      - Pending interrupt status
 *      - Configured mode (capture, compare, disabled)
 *      - Physical unit number
 *
 *      If a unsupported StatusID is supplied, an error is returned.
 *
 *  \iparam  Handle = Handle of open timer
 *  \iparam  UnitNo = Unit number of the capture/compare channel
 *  \iparam  StatID = Status identifier
 *
 *  \return  Status of capture/compare unit or (negative) error code
 *
 ****************************************************************************/

Error_t halCapComStatus (Handle_t Handle, UInt16 UnitNo, TimStatID_t StatID) {

    const Int32 Index = halTimerGetIndex(Handle);

    if (Index >= 0) {
        register TimerRegFile_t *TIM = DataTable[Index].TIM;

        if (UnitNo >= DataTable[Index].CapComs) {
            return (E_TIMER_CAPCOM_CHANNEL);
        }
        switch (StatID) {
            case TIM_STAT_MATCH:
                return (TIM->SR & (TIM_SR_CC1IF << UnitNo));

            case TIM_STAT_OVERRUN:
                return (TIM->SR & (TIM_SR_CC1OF << UnitNo));

            case TIM_STAT_PENDING:
                return (TIM->SR & TIM->DIER & (TIM_SR_CC1IF << UnitNo));

            case TIM_STAT_UNITMODE:
                return (DataTable[Index].Mode[UnitNo]);

            case TIM_STAT_UNITNO:
                return (UnitNo);
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Controls a capture/control unit
 *
 *      Controls a capture/compare unit of the timer associated with Handle 
 *      in several ways. Depending on ControlID the following actions can 
 *      be initiated:
 *
 *      - Enable/disable capture/compare unit
 *      - Enable/disable capture/compare interrupt
 *      - Clear capture/compare interrupt flag
 *
 *      UnitNo selects the capture/compare unit of the timer.
 *
 *  \iparam  Handle    = Handle of open timer
 *  \iparam  UnitNo    = Capture/compare unit number
 *  \iparam  ControlID = Type of control
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halCapComControl (
            Handle_t Handle, UInt16 UnitNo, TimCtrlID_t ControlID) {

    const Int32 Index = halTimerGetIndex(Handle);

    if (Index >= 0) {
        register TimerRegFile_t *TIM = DataTable[Index].TIM;

        if (UnitNo >= DataTable[Index].CapComs) {
            return (E_TIMER_CAPCOM_CHANNEL);
        }
        switch (ControlID) {
            case TIM_CTRL_START:
                TIM->CCER |= (TIM_CCER_CC1E << (UnitNo * 4));
                break;

            case TIM_CTRL_STOP:
                TIM->CCER &= ~(TIM_CCER_CC1E << (UnitNo * 4));
                break;

            case TIM_INTR_ENABLE:
                TIM->DIER |= (TIM_DIER_CC1IE << UnitNo);
                break;

            case TIM_INTR_DISABLE:
                TIM->DIER &= ~(TIM_DIER_CC1IE << UnitNo);
                break;

            case TIM_INTR_CLEAR:
                TIM->SR &= ~(TIM_SR_CC1IF << UnitNo);   
                break;

            default:
                return (E_UNKNOWN_CONTROL_ID);
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Timer master interrupt handler
 *
 *      This function will be called by the vectored interrupt controller in
 *      case of a Timer interrupt. Channel is the number of the Timer. It
 *      calls the user handler registered with halTimerRegisterHandler(),
 *      passing the interrupt flag bits and the user tag to it. Before
 *      exiting, the handled interrupt flag bits are cleared.
 *
 *  \iparam  Channel = Timer channel number 
 *
 *  \return  Nothing
 *
 ****************************************************************************/
//UInt32 min = 0xffff;
//UInt32 max = 0;
//UInt32 usDuration;

void halTimerInterruptHandler (UInt32 Channel) {
//usDuration=halGetFastTick();

    if (Channel < ELEMENTS(DataTable)) {
        const UInt32 IntrFlags = DataTable[Channel].TIM->SR & 
            DataTable[Channel].TIM->DIER & TIM_SR_IFLAGS;
    
        if (TimerIntrVectors[Channel].Handler != NULL) {
            TimerIntrVectors[Channel].Handler (TimerIntrVectors[Channel].UserTag, IntrFlags);
        }
        DataTable[Channel].TIM->SR = ~IntrFlags;
    }
//usDuration=halGetFastTick()-usDuration;
//if(usDuration<min)
//min=usDuration;
//if(usDuration>max)
//max=usDuration;
}


/*****************************************************************************/
/*!
 *  \brief   Get index of a timer
 *
 *      Returns the index of a timer associated with Handle. The index is
 *      used to address an entry in the TimerData table, which contains
 *      all variables of a timer.
 *
 *  \iparam  Handle = Handle of open analog channel
 *
 *  \return  Timer index or (negative) error code
 *
 ****************************************************************************/

static inline Error_t halTimerGetIndex (Handle_t Handle) {

    const UInt32 Index = Handle ^ DEVICE_CLASS_TIMER;

    if (Index < ELEMENTS(DataTable)) {

        if (!(DataTable[Index].Flags & HAL_OPEN_RW)) {
            return (E_DEVICE_NOT_OPEN);
        }
        return ((Error_t)Index);
    }
    return (E_DEVICE_HANDLE_INVALID);
}


/*****************************************************************************/
/*!
 *  \brief   Search logical timer
 *
 *      Searches the logical timer descriptor for an entry with the asked
 *      DeviceID. If successful, the physical timer number is returned.
 *      If no such entry exists, an error is returned.
 *
 *  \iparam  DeviceID = Logical timer ID
 *
 *  \return  Timer number or (negative) error code
 *
 ****************************************************************************/

static Error_t halTimerFindDevice (Device_t DeviceID) {

    const halTimerDescriptor_t *Descriptor = halTimerDescriptors;
    UInt32 Index;

    if ((DeviceID & DEVICE_CLASS_MASK) == DEVICE_CLASS_TIMER) {

        halInitState_t InitState = halGetInitState();

        if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
            return (E_HAL_NOT_INITIALIZED);
        }
        for (Index=0; Index < halTimerDescriptorCount; Index++) {

            if (Descriptor->DeviceID == DeviceID) {
                if (~DataTable[Descriptor->TimerNo].Flags & HAL_FLAG_INITZED) {
                    return (E_DEVICE_UNCONFIGURED);
                }
                if (Descriptor->TypeID == TIM_SEL_COUNTER) {
                    return (Descriptor->TimerNo);
                }
            }
            Descriptor++; // next time descriptor
        }
    }
    return (E_UNKNOWN_DEVICE_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize timer module
 *
 *      Initializes this module by evaluating the timer descriptor table from
 *      the global HAL configuration file and setting up the appropriate
 *      hardware registers.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halTimerInit (void) {

    const halTimerDescriptor_t *Descriptor = halTimerDescriptors;
    UInt32 i;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        for (i=0; i < halTimerDescriptorCount; i++) {
    
            TimerRegFile_t *TIM  = DataTable[Descriptor->TimerNo].TIM;
            const UInt32 TimerNo = Descriptor->TimerNo;
    
            if (TimerNo >= ELEMENTS(DataTable)) {
                return (E_TIMER_DESCRIPTOR);
            }
            if ((BIT(TimerNo) & halProcessorInfo.TimerMask) == 0) {
                return (E_TIMER_DESCRIPTOR);
            }
            halPeripheralClockEnable (DataTable[TimerNo].PeripheralID, ON);
    
            if (Descriptor->TypeID == TIM_SEL_CAPTURE ||
                Descriptor->TypeID == TIM_SEL_COMPARE) {
    
                const UInt16 ModeCCM = Descriptor->Mode >> 8;
                const UInt16 ModeCCE = Descriptor->Mode & 0x0F;
                const UInt16 UnitNo  = Descriptor->UnitNo;

                if (UnitNo >= DataTable[TimerNo].CapComs) {
                    return (E_TIMER_DESCRIPTOR);
                }
                DataTable[TimerNo].Mode[UnitNo] = Descriptor->TypeID;    

                if (UnitNo < 2) {
                    TIM->CCMR[0] |= ModeCCM << ((UnitNo % 2) * 8);
                }
                else if (UnitNo < 4) {
                    TIM->CCMR[1] |= ModeCCM << ((UnitNo % 2) * 8);
                }
                TIM->CCER |= ModeCCE << (UnitNo * 4);
            }
            else if (Descriptor->TypeID == TIM_SEL_COUNTER) {
                if (Descriptor->Priority) {
                    DataTable[TimerNo].Priority = Descriptor->Priority; 
                }
                else {
                    DataTable[TimerNo].Priority = IRQ_PRIO_DEFAULT-1; 
                }
                TIM->SMCR = Descriptor->Mode; 
            }
            DataTable[TimerNo].Flags = HAL_FLAG_INITZED;
    
            Descriptor++; // increment timer descriptor pointer
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/

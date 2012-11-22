/****************************************************************************/
/*! \file halEncoder.c
 * 
 *  \brief  Quadrature Encoder Functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 22.02.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage quadrature 
 *      encoders to sense the position of a motor. Since the STM32 has
 *      no dedicated quadrature encoder hardware, the timers are used
 *      for this function. To be able to port the software to other
 *      hardware plattforms (which might have dedicated decoders), a
 *      dedicated software interface is provided, that hides the fact
 *      that standard timers are used on STM32 hardware.
 *
 *      The encoder supports up to two reference positions: when the
 *      actual position hits a reference position, an interrupt can
 *      be generated. An index input from an encoder is supported,
 *      which triggers sampling the actual position in an index
 *      register. It can also be configured to generate an interrupt.
 *      The index input and the 2nd reference position are available
 *      alternative only. Which option to use is determined by the
 *      HAL configuration file.
 * 
 *      Direction change interrupts and phase error interrupts are not
 *      implemented, because there is no hardware support for these
 *      features. Nevertheless, the actual data direction can be
 *      requested using the status function.
 *
 *      The timers used as encoders must be configured properly in the 
 *      HAL configuration file to be usable as encoders.
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
#include "halError.h"
#include "halClocks.h"
#include "halCommon.h"
#include "halTimer.h"
#include "halEncoder.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define TIM_UNIT_LIMIT1     3   //!< Timer compare unit for limit 1
#define TIM_UNIT_LIMIT2     2   //!< Timer compare unit for limit 2



/*****************************************************************************/
/*! 
 *  \brief   Open a quadrature encoder
 *
 *      Opens a quadrature encoder and returns a handle. DeviceID must be  
 *      the numerical name"of a quadrature encoder. 
 * 
 *      Handler must be a pointer to an interrupt handler function to be 
 *      called when the device signals an interrupt. It can be NULL if no
 *      interrupts are used. UserTag is a user defined number to be passed 
 *      to the handler function, e.g. as index into a data table in case 
 *      of multiple instances. 
 *
 *  \iparam  DeviceID = Logical device ID
 *  \iparam  UserTag  = User tag to pass thru to interrupt handler
 *  \iparam  Handler  = Interrupt handler function
 * 
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halEncoderOpen (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler) {

    const TimerMode_t TimerMode = { 
        0, TIM_MODE_INTERVAL, 0, TIM_MODE_ENCODER_1 };
    Handle_t Handle;
    Error_t Status;

    if ((Handle = halTimerOpen(DeviceID, UserTag, Handler)) > 0) {

        Status = halTimerSetup (Handle, &TimerMode, 1);

        // Define maximal position for encoder
        if (Status == NO_ERROR) {
            Status = halTimerWrite(Handle, TIM_REG_RELOAD, MAX_UINT16);
        }
        // Enable compare unit to monitor limit 1
        if (Status == NO_ERROR) {
            Status = halCapComControl(Handle, TIM_UNIT_LIMIT1, TIM_CTRL_START);
        }
        // Enable compare unit to monitor limit 2 or index pulse
        if (Status == NO_ERROR) {
            Status = halCapComControl(Handle, TIM_UNIT_LIMIT2, TIM_CTRL_START);
        }
        // Enable timer to count quadrature pulses
        if (Status == NO_ERROR) {
            Status = halTimerControl(Handle, TIM_CTRL_START);
        }
        if (Status != NO_ERROR) {
            halTimerClose(Handle);
            return (Status);
        }
    }
    return (Handle);
}


/*****************************************************************************/
/*! 
 *  \brief   Close quadrature encoder
 *
 *      Closes an open quadrature encoder
 * 
 *  \iparam  Handle = Handle of quadrature encoder
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halEncoderClose (Handle_t Handle) {

    return (halTimerClose(Handle));
}


/*****************************************************************************/
/*! 
 *  \brief   Read from quadrature encoder
 *
 *      Reads register in quadrature encoder associated with Handle and 
 *      copies it to the variable pointed to by Value. RegisterID defines
 *      which register to read, which can be one of the following:
 *  
 *      - Encoder position
 *      - Limit 1 position 
 *      - Limit 2 position or index position
 *      - End position
 *
 *      The index position is available only, if timer channel 3 is
 *      configured as a capture input.
 * 
 *  \iparam  Handle = Handle of quadrature encoder
 *  \iparam  RegID  = ID of register to read from
 *  \oparam  Value  = Pointer to a result variable
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halEncoderRead (Handle_t Handle, EncRegsID_t RegID, UInt32 *Value) {
    
    switch (RegID) {

        case ENC_REG_POSITION:
            return (halTimerRead (Handle, TIM_REG_COUNTER, Value));

        case ENC_REG_ENDPOSITION:
            return (halTimerRead (Handle, TIM_REG_RELOAD, Value));

        case ENC_REG_LIMIT1:
            return (halCapComRead (Handle, TIM_UNIT_LIMIT1, Value));

        case ENC_REG_LIMIT2:
            return (halCapComRead (Handle, TIM_UNIT_LIMIT2, Value));
    }
    return (E_INVALID_REGISTER_ID);
}


/*****************************************************************************/
/*! 
 *  \brief   Write to quadrature encoder
 *
 *      Writes register in quadrature encoder associated with Handle with 
 *      the supplied Value. RegisterID defines which register to write to, 
 *      which can be one of the following:
 *  
 *      - Encoder position
 *      - Limit 1 position 
 *      - Limit 2 position
 *      - End position
 *
 *      The reference position 2 is available only, if timer unit 3 is
 *      configured as a compare output.
 * 
 *  \iparam  Handle = Handle of quadrature encoder
 *  \iparam  RegID  = ID of register to write to
 *  \iparam  Value  = Value to write to register
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halEncoderWrite (Handle_t Handle, EncRegsID_t RegID, UInt32 Value) {

    switch (RegID) {

        case ENC_REG_POSITION:
            return (halTimerWrite (Handle, TIM_REG_COUNTER, Value));

        case ENC_REG_ENDPOSITION:
            return (halTimerWrite (Handle, TIM_REG_RELOAD, Value));

        case ENC_REG_LIMIT1:
            return (halCapComWrite (Handle, TIM_UNIT_LIMIT1, Value));

        case ENC_REG_LIMIT2:
            return (halCapComWrite (Handle, TIM_UNIT_LIMIT2, Value));
    }
    return (E_INVALID_REGISTER_ID);
}

 
/*****************************************************************************/
/*! 
 *  \brief   Get status of quadrature encoder
 *
 *      Returns the status of the quadrature encoder associated with Handle. 
 *      Which status to return depends on StatusID. Following informations
 *      can be requested:
 *
 *      - Actual rotation direction
 *      - Pending interrupt bits
 *      - Index input option available
 *      - Number of supported limits
 * 
 *  \iparam  Handle   = Handle of quadrature encoder
 *  \iparam  StatusID = Type of status requested
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halEncoderStatus (Handle_t Handle, EncStatID_t StatusID) {

    Error_t Status;

    switch (StatusID) {

        case ENC_STAT_DIRECTION:
            return (halTimerStatus (Handle, TIM_STAT_DIRECTION));
    
        case ENC_STAT_PENDING:
            return (halTimerStatus (Handle, TIM_STAT_PENDING));

        case ENC_STAT_INDEX:
            Status = halCapComStatus (Handle, TIM_UNIT_LIMIT2, TIM_STAT_UNITMODE);
            return (Status == TIM_SEL_CAPTURE);

        case ENC_STAT_LIMITS:
            Status = halCapComStatus (Handle, TIM_UNIT_LIMIT2, TIM_STAT_UNITMODE);
            return (Status == TIM_SEL_COMPARE ? 2 : 1);
    }    
    return (E_UNKNOWN_STATUS_ID);
}


/*****************************************************************************/
/*! 
 *  \brief   Controls a quadrature encoder
 *
 *      Controls the encoder associated with Handle in several ways. Depending
 *      on ControlID the following actions can be initiated:
 * 
 *      - Control limit position 1 interrupt
 *      - Control limit position 2 interrupt
 *      - Control encoder overflow/underflow interrupt
 *
 *      State determines if to enable the selected function (TRUE) or 
 *      to disable it (FALSE).
 *
 *      Reference position 2 and index pulse interrupts are available
 *      alternative only. Which one is available is determined by the
 *      HAL configuration file.
 * 
 *  \iparam  Handle    = Handle of quadrature encoder
 *  \iparam  ControlID = Type of control
 *  \iparam  State     = Enable: True, Disable: False
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halEncoderControl (Handle_t Handle, EncCtrlID_t ControlID, Bool State) {

    TimCtrlID_t TimCtrlID;

    switch (ControlID) {

        case ENC_INTR_LIMIT1:
            TimCtrlID = State ? TIM_INTR_ENABLE : TIM_INTR_DISABLE;
            return (halCapComControl (Handle, TIM_UNIT_LIMIT1, TimCtrlID));

        case ENC_INTR_LIMIT2:
            TimCtrlID = State ? TIM_INTR_ENABLE : TIM_INTR_DISABLE;
            return (halCapComControl (Handle, TIM_UNIT_LIMIT2, TimCtrlID));

        case ENC_INTR_OVERFLOW:
            TimCtrlID = State ? TIM_INTR_ENABLE : TIM_INTR_DISABLE;
            return (halTimerControl (Handle, TimCtrlID));
    }    
    return (E_UNKNOWN_CONTROL_ID);
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize quadrature encoders
 *
 *      Initializes this module.
 * 
 *      This function is called only once during startup.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halEncoderInit (void) {

    return (NO_ERROR);   
}

//****************************************************************************/

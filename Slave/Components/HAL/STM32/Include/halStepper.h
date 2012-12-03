/****************************************************************************/
/*! \file halStepper.h
 * 
 *  \brief  Stepper motor controller HAL
 *
 *  $Version: $ 0.1
 *  $Date:    $ 23.02.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access stepper motor controllers
 *      of the type "Trinamic TMC26x" connected via serial SPI ports. It
 *      supports any number of stepper motors. Only stepper motors using
 *      different SPI interfaces can run simultainously, steppers connected
 *      to the same SPI can run only alternative. Chip select outputs are
 *      used to distinguish different stepper controllers on the SPI port.
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

#ifndef HAL_STEPPER_H
#define HAL_STEPPER_H


//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Status IDs for requesting the state of a stepper motor
typedef enum {    
    STP_STAT_MICROSTEPS = 1,    //!< Max. microsteps per full step
    STP_STAT_TEMPERATURE,       //!< Temperature status
    STP_STAT_STALLGUARD,        //!< Stall guard result
    STP_STAT_ERRORFLAGS,        //!< Error flag bits
    STP_STAT_LOCKED             //!< Stepper bus locked flag
} StepStatID_t;

//! Control IDs for controlling a stepper motor
typedef enum {
    STP_CTRL_ENABLE,            //!< Enable driver stage
    STP_CTRL_CLEAR_ERROR,       //!< Clear controller errors
    STP_CTRL_LOCK_BUS,          //!< lock stepper bus
} StepCtrlID_t;

//! Stepper controller error indicator flags
typedef enum {
    STP_ERROR_SG   = 0x01,      //!< Motor stalled
    STP_ERROR_OT   = 0x02,      //!< Overtemperature
    STP_ERROR_S2GA = 0x08,      //!< Phase A short to GND
    STP_ERROR_S2GB = 0x10,      //!< Phase B short to GND
    STP_ERROR_OLA  = 0x20,      //!< Phase A open load
    STP_ERROR_OLB  = 0x40       //!< Phase B open load
} StepErrFlags_t;

//! Stepper controller temperature values
typedef enum {
    STP_TEMP_GOOD,              //!< Controller temperature is ok
    STP_TEMP_WARNING,           //!< Temperature warn level
    STP_TEMP_FAILURE            //!< Temperature too high
} StepTemperature_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halStepperOpenExt (Device_t DeviceID, const UInt32 *Mode, UInt16 Count);
Error_t halStepperOpen    (Device_t DeviceID);
Error_t halStepperClose   (Handle_t Handle);
Error_t halStepperInit    (void);

Error_t halStepperSetup   (Handle_t Handle, const UInt32 *Mode, UInt16 Count);
Error_t halStepperWrite   (Handle_t Handle, UInt32 SinusIndex);
Error_t halStepperCurrent (Handle_t Handle, UInt8 Current);
Error_t halStepperControl (Handle_t Handle, StepCtrlID_t ControlID, Bool State);
Error_t halStepperStatus  (Handle_t Handle, StepStatID_t StatusID);

Error_t halStepperWrite_Simplified (Int32 Index, UInt32 SinIndex);
//****************************************************************************/

#endif /*HAL_STEPPER_H*/

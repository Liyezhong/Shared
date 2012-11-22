/****************************************************************************/
/*! \file halError.c
 * 
 *  \brief  Handle HAL internal errors
 *
 *  $Version: $ 0.1
 *  $Date:    $ 11.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to handle HAL errors.
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
#include "halError.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define ERROR_CLASS_HARD    0x4000      //!< bit to indicate hard errors
#define ERROR_CLASS_SOFT    0x0000      //!< bit to indicate soft errors

#define isHardError(err)    (((err) & ERROR_CLASS_HARD) != 0)

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static HAL_ERROR_HANDLER_t* ErrorHandler = NULL;
static ERROR_t LastError = NO_ERROR;


/*****************************************************************************/
/*! 
 *  \brief   Sets error state
 *
 *      Saves the suppied error code as "last error" and, if a user error
 *      hook function is registered, calls that callback function to inform
 *      the subscriber about the error. 
 * 
 *  \iparam  ErrorCode = error code
 * 
 *  \return  Same as ErrorCode
 *
 ****************************************************************************/

ERROR_t halSetError (ERROR_t ErrorCode) {
    
    LastError = ErrorCode;
    
    if (ErrorCode != NO_ERROR) {
        if (ErrorCode > 0) {
            ErrorCode = E_UNDEFINED_ERROR;
        }     
        if (!isPtrNull(ErrorHandler) && isHardError(ErrorCode)) {
            (*ErrorHandler) (ErrorCode);
        }
    }
    return (ErrorCode);
}


/*****************************************************************************/
/*! 
 *  \brief   Register error hook
 *
 *      Using this function the caller can register a callback function,
 *      which is called each time, an error is raised in the HAL layer.
 *      This might be useful for debugging or if higher software layers
 *      don't want to make error checking on each single HAL function.
 * 
 *      Only one callback function can be registered. Calling this 
 *      function again overwrites the previous registration. Calling 
 *      this function with a NULL pointer deactivates error hooking. 
 *      The causing error is passed to the callback as a parameter.
 * 
 *      Calling this function before initialization the HAL layer is 
 *      allowed. This permits catching of initialization errors.     
 * 
 *  \iparam  Callback = Pointer to callback function
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halRegisterErrorHook (HAL_ERROR_HANDLER_t* Callback) {

   ErrorHandler = Callback;
   return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Returns last error
 *
 *      Returns the last error of the HAL layer to the caller. HAL functions
 *      returning NO_ERROR do not clear the 'last error'.
 * 
 *  \return  ErrorCode of last error
 *
 ****************************************************************************/

ERROR_t halGetError (void) {
    
    return (LastError);
}

//****************************************************************************/

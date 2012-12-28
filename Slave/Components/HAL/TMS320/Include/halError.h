/****************************************************************************/
/*! \file halError.h
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

#ifndef HAL_ERROR_H
#define HAL_ERROR_H

#include "bmError.h"    // currently, errors are defined in base module

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

typedef void(*HAL_ERROR_HANDLER_t)(ERROR_t errCode);

//****************************************************************************/
// Function Prototypes
//****************************************************************************/

ERROR_t halSetError (ERROR_t ErrorCode);
ERROR_t halGetError (void);
ERROR_t halRegisterErrorHook (HAL_ERROR_HANDLER_t* Callback);

//****************************************************************************/

#endif /*HAL_ERROR_H*/

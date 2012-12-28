/****************************************************************************/
/*! \file halMain.h
 *
 *  \brief  Hardware abstraction layer main module
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to initialize the hardware abstraction
 *       layer. This is done by calling the initialization functions in the
 *       various HAL layer modules.
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

#ifndef HAL_MAIN_H
#define HAL_MAIN_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Constants to indicate the state of HAL initialization
typedef enum {
    INIT_NOT_DONE,      //!< Initialization not done
    INIT_IN_PROGRESS,   //!< Initialization in progress
    INIT_SUCCEEDED,     //!< Initialization succeeded
    INIT_FAILED         //!< Initialization failed
} halInitState_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halInitializeLayer (void);
halInitState_t halGetInitState (void);
void halSetInitState (halInitState_t InitState);
Error_t halControlTask (void);

//****************************************************************************/

#endif /*HAL_MAIN_H*/

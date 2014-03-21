/****************************************************************************/
/*! \file halFlash.h
 *
 *  \brief  Flash memory control functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 14.04.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to control the processor internal
 *      flash memory. This includes writing/erasing the flash memory.
 *
 *      The functions implemented in this module are intended to be used
 *      HAL internal only. Use the Storage functions to access the flash
 *      and other memory types from outside the HAL.
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

#ifndef HAL_FLASH_H
#define HAL_FLASH_H


//****************************************************************************/
// Public Constants
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

UInt32  halFlashBlockSize (void);
Error_t halFlashRead  (UInt32 Address, void *Buffer, UInt32 Count);
Error_t halFlashWrite (UInt32 Address, void *Buffer, UInt32 Count);
Error_t halFlashErase (UInt32 Address, UInt32 Count);
Error_t halFlashInit  (void);

Error_t halFlashProtect (Bool State);
Error_t halFlashWaitStates (UInt32 ClockRate);

//****************************************************************************/

#endif /*HAL_FLASH_H*/

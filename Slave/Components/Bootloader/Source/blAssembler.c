/****************************************************************************/
/*! \file blAssembler.c
 *
 *  \brief Bootloader assembler functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.03.2012
 *  $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *       This module contains all assembly function that are used by the boot
 *       loader. They had to be moved to this file, because the Tessy
 *       function parser cannot deal with assembly code.
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

#include "blAssembler.h"


/*****************************************************************************/
/*!
 *  \brief   Initialize Firmware Application's Stack Pointer
 *
 *      This function sets the firmware application's stack pointer before
 *      jumping from boot loader to firmware.
 *
 *  \iparam MainStackPointer = Stack pointer of firmware
 *
 ****************************************************************************/

__asm void blSetMsp(UInt32 MainStackPointer)
{
    msr msp, r0
    bx lr
}

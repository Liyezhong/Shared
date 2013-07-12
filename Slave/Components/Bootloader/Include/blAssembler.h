/****************************************************************************/
/*! \file blAssembler.h
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

#ifndef BL_ASSEMBLER_H
#define BL_ASSEMBLER_H

#include "Global.h"

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

void blSetMsp(UInt32 MainStackPointer);

#endif /*BL_ASSEMBLER_H*/

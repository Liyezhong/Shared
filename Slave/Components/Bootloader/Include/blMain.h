/****************************************************************************/
/*! \file blMain.h
 *
 *  \brief Bootloader main functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.10.2011
 *  $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *       This module is the boot loader of the microcontroller. At every start
 *       of the controller the boot loader checks, if a loadable firmware
 *       image is present in the flash memory of the microcontroller. If this
 *       is the case, a CRC check is performed on the firmware image and the
 *       actual firmware is started. The bootloader is also able to write a
 *       new image into flash memory. The update process can be started, when
 *       no firmware image is stored in the memory or when the boot loader is
 *       explicitly started from the base module for this purpose. The update
 *       is controlled over CAN bus.
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

#ifndef BL_MAIN_H
#define BL_MAIN_H

#include "Global.h"

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

void blStartBootloader (void);

#endif /*BL_MAIN_H*/

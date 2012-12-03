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
 *       This module initializes the bootloader, the base module, and the HAL
 *       by calling their init functions.
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

Error_t blStartBootloader (void);

#endif /*BL_MAIN_H*/

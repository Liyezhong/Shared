/****************************************************************************/
/*! \file bmUpdate.h
 *
 *  \brief Boot loader update functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 09.10.2012
 *  $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *         This module contains functions that update the boot loader of the
 *         Slave. Some of them are also used by the boot loader to itself to
 *         to perform a regular software update.
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

#ifndef BM_UPDATE_H
#define BM_UPDATE_H

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t bmInitUpdate (void);
UInt32 bmGetBootLoaderSize(void);
Error_t bmFlashCrc (UInt32 *Crc, Handle_t Handle, UInt32 Address, UInt32 Length);

//****************************************************************************/

#endif /*BM_UPDATE_H*/

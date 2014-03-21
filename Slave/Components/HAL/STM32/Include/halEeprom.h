/****************************************************************************/
/*! \file halEeprom.h
 *
 *  \brief  Non-volatile memory access functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 09.05.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access a non-volatile serial
 *      FRAM memory (ferro-magnetic RAM) connected via I2C. FRAMs are
 *      compatible with serial EEPROMs on the hardware side. But other
 *      than EEPROMs FRAMs can be written without any write delay,
 *      while EEPROMs need some milliseconds to finish a write. 
 
 *      Since memory read and write operations are synchronous from the
 *      systems point of view, they must be served as fast as possible.
 *      The limited speed of the I2C bus is compensated by optionally
 *      caching the memory in RAM. The content of the non-volatile
 *      memory is read into a RAM mirror on startup and all reads are
 *      further satisfied by accessing the RAM only. Writes are routed
 *      to the RAM mirror, and additionally to non-volatile memory, 
 *      keeping both memory copies consistent. Since write operation 
 *      can be performed asynchronously, the slow access time via I2C 
 *      doesn't matters in this case.
 *
 *      When using larger size memories it might become necessary to
 *      change from mirroring to a less memory wasting algorithm like 
 *      real caching. Actually, memory sizes of 256..512 bytes are  
 *      planned to be used. The memory requirement for mirroring is 
 *      acceptable with these sizes. The current implementation 
 *      supports FRAM sizes up to 512 kByte. 
 *
 *      It would be possible to extend the functionality of this module
 *      to support also EEPROM devices. After write operations a wait
 *      time would be needed to archive this. This could be done by
 *      polling the device via I2C. The EEPROM would return negative
 *      acknowledges as long as the programming is in progress.
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

#ifndef HAL_EEPROM_H
#define HAL_EEPROM_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halEepromRead  (UInt32 Address, void *Buffer, UInt32 Count);
Error_t halEepromWrite (UInt32 Address, void *Buffer, UInt32 Count);
Error_t halEepromErase (UInt32 Address, UInt32 Count);
Error_t halEepromInit  (void);

Error_t halEepromProtect (Bool State);
Error_t halEepromWait (void);

//****************************************************************************/

#endif /*HAL_EEPROM_H*/

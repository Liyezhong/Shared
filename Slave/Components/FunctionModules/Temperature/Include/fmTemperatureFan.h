/****************************************************************************/
/*! \file fmTemperatureFan.h
 * 
 *  \brief Functions determining the speed of a ventilation fan.
 *
 *   $Version: $ 0.4
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *  This file's only task the determination of speed of a ventilation fan.
 *  The unit is rotations per minute.
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

#ifndef FMTEMPERATUREFAN_H
#define FMTEMPERATUREFAN_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t tempFanInit (UInt16 Instances);
Error_t tempFanOpen (UInt16 Instance, UInt8 Fans);
Error_t tempFanControl (UInt16 Instance, Bool Activate);
Error_t tempFanSpeed (UInt16 Instance, UInt8 Fan, UInt16 *Speed);

//****************************************************************************/

#endif /*FMTEMPERATUREFAN_H*/

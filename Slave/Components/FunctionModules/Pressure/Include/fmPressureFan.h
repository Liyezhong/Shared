/****************************************************************************/
/*! \file fmPressureFan.h
 * 
 *  \brief Functions determining the speed of a ventilation fan.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
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

#ifndef FMPRESSUREFAN_H
#define FMPRESSUREFAN_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t pressFanInit (UInt16 Instances);
Error_t pressFanOpen (UInt16 Instance, UInt8 Fans);
Error_t pressFanControl (UInt16 Instance, Bool Activate);
Error_t pressFanSpeed (UInt16 Instance, UInt8 Fan, UInt16 *Speed);

//****************************************************************************/

#endif /*FMPRESSUREFAN_H*/

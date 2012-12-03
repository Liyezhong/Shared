/****************************************************************************/
/*! \file halAD779x.h
 * 
 *  \brief  Multi-channel 16/24 Bit ADC Interface for AD779x
 *
 *  $Version: $ 0.1
 *  $Date:    $ 18.07.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the external
 *      6 channel analog-to-digital converter AD7794 from analog devices.
 *      The chip is connected to the microcontroller via one of the SPI
 *      serial busses.
 *
 *      The functions in this module are not intended to be used from
 *      outside of the HAL. The standard analog input functions can be
 *      used instead.
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

#ifndef HAL_AD779X_H
#define HAL_AD779X_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halAd779xRead   (UInt16 Channel, Int32 *Buffer);
Error_t halAd779xAttach (UInt16 Channel);
Error_t halAd779xDetach (UInt16 Channel);
Error_t halAd779xTask   (void);
Error_t halAd779xInit   (void);

//****************************************************************************/

#endif /*HAL_AD779X_H*/

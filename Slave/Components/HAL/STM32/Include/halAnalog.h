/****************************************************************************/
/*! \file halAnalog.h
 *
 *  \brief  Analog channel i/o
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage logical analog
 *      inputs and outputs. Each logical channel is mapped to a physical
 *      input/output applying a logical channel descriptor table, which is
 *      part of the HAL configuration module. Access to a logical channel
 *      requires opening that channel. The open function returns a handle
 *      which has to be used as a parameter to the read/write functions.
 *      If the channel is not needed anymore, the channel can be closed
 *      again using the close function. Analog output channels can be
 *      read or written, input channels can only be read. Reading an
 *      output channel returns the actual output value.
 *
 *      This implementation uses only one of the ADCs to convert the up to
 *      16 analog input channels. It would be possible to use two ADCs and
 *      distribute the input channels to these two ADCs using simultainous
 *      or interleaved mode. This could half the overall conversion time,
 *      with the impact, that the conversion times of the channels must
 *      be carefully paired. Since this would drastically complicate the
 *      configuration of the HAL, this solution wasn't choosen.
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

#ifndef HAL_ANALOG_H
#define HAL_ANALOG_H

#include "halInterrupt.h"

//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Status IDs for requesting the state of analog I/Os
typedef enum {
    AIO_STAT_BITCOUNT,    //!< Request number of converter bits
    AIO_STAT_MAXVALUE,    //!< Request maximal value
} AnalogStatID_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halAnalogOpen   (Device_t Device, UInt16 Mode, UInt32 UserTag, halIntrHandler_t Handler);
Error_t halAnalogClose  (Handle_t Handle);
Error_t halAnalogInit   (void);

Error_t halAnalogRead   (Handle_t Handle, Int16 *Value);
Error_t halAnalogRead32 (Handle_t Handle, Int32 *Value);
Error_t halAnalogWrite  (Handle_t Handle, UInt16 Value);
Error_t halAnalogStatus (Handle_t Handle, AnalogStatID_t StatusID);

//****************************************************************************/

#endif /*HAL_ANALOG_H*/

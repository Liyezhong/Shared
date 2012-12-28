/****************************************************************************/
/*! \file halAnalog.c
 * 
 *  \brief  Analog channel i/o
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to access and manage logical analog
 *       channels, maps them to physical (processor internal) analog/digital
 *       converters. Also included is the initialization of the processor 
 *       using external hardware configuration tables.
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

#ifndef HAL_ANALOG_H
#define HAL_ANALOG_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Status IDs for requesting the state of analog I/Os
typedef enum {
    ANAIO_STAT_BITCOUNT,    //!< Request number of converter bits
    ANAIO_STAT_MAXVALUE,    //!< Request maximal value
    ANAIO_STAT_OFFSET       //!< Request configured offset
} ANAIO_STATUS_ID_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

HANDLE_t halAnalogOpen   (CHANNEL_t ChannelID, UInt16 Mode);
ERROR_t  halAnalogRead   (HANDLE_t Handle, UInt16 *Value);
ERROR_t  halAnalogWrite  (HANDLE_t Handle, UInt16 Value);
ERROR_t  halAnalogStatus (HANDLE_t Handle, ANAIO_STATUS_ID_t StatusID);
ERROR_t  halAnalogClose  (HANDLE_t Handle);

ERROR_t  halAnalogInitialize (void);

//****************************************************************************/

#endif /*HAL_ANALOG_H*/

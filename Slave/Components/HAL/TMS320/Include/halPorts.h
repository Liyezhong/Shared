/****************************************************************************/
/*! \file halPorts.c
 * 
 *  \brief  Digital port i/o
 *
 *  $Version: $ 0.1
 *  $Date:    $ 11.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to access and manage logical ports,
 *       convert them to physical (processor) ports and controls associated
 *       peripherals. Also included is the initialization of the processor 
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

#ifndef HAL_PORTS_H
#define HAL_PORTS_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Status IDs for requesting the state of digital I/Os
typedef enum {
    DIGIO_STAT_BITCOUNT,    //!< Request width of logical port 
    DIGIO_STAT_MAXVALUE     //!< Request maximal value for port
} DIGIO_STATUS_ID_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

HANDLE_t halDigitalOpen   (CHANNEL_t ChannelID, UInt16 Mode);
ERROR_t  halDigitalRead   (HANDLE_t Handle, UInt16 *Value);
ERROR_t  halDigitalWrite  (HANDLE_t Handle, UInt16 Value);
ERROR_t  halDigitalStatus (HANDLE_t Handle, DIGIO_STATUS_ID_t StatusID);
ERROR_t  halDigitalClose  (HANDLE_t Handle);

ERROR_t  halInitializePorts (void);

//****************************************************************************/

#endif /*HAL_PORTS_H*/

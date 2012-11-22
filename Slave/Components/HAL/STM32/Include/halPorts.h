/****************************************************************************/
/*! \file halPorts.h
 *
 *  \brief  Digital port i/o
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage logical ports,
 *      convert them to physical (processor) ports and controls associated
 *      peripherals. Also included is the initialization of the processor
 *      using external hardware configuration tables.
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

#ifndef HAL_PORTS_H
#define HAL_PORTS_H

//****************************************************************************/
// Public Constants
//****************************************************************************/

//! Status IDs for requesting the state of digital I/Os
typedef enum {
    PORT_STAT_BITCOUNT,    //!< Request width of logical port
    PORT_STAT_MAXVALUE     //!< Request maximal value for port
} PortStatID_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halPortOpen   (Device_t PortID, UInt16 Mode);
Error_t halPortClose  (Handle_t Handle);
Error_t halPortInit   (void);

Error_t halPortRead   (Handle_t Handle, UInt16 *Value);
Error_t halPortWrite  (Handle_t Handle, UInt16 Value);
Error_t halPortStatus (Handle_t Handle, PortStatID_t StatusID);

//****************************************************************************/

#endif /*HAL_PORTS_H*/

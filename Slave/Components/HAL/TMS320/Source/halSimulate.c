/****************************************************************************/
/*! \file halSimulate.c
 * 
 *  \brief  Simulation support functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 12.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains support functions for the simulation of analog
 *       inputs and other functions that are not yet implemented.
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

#include <stdlib.h>
#include "Global.h"
#include "halCommon.h"
#include "halError.h"
#include "halSimulate.h"

//****************************************************************************/
// Implementation
//****************************************************************************/

Bool halPowerSupplyGood (void)
{
    return (TRUE);
}

//****************************************************************************/

ERROR_t halControlTask (void) 
{ 
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halRegisterAnalogInputPattern (
       CHANNEL_t Channel, const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize)
{
    return (NO_ERROR);
}

ERROR_t halRegisterDigitalInputPattern (
       CHANNEL_t Channel, const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize)
{
    return (NO_ERROR);
}

//****************************************************************************/

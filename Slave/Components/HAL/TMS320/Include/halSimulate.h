/****************************************************************************/
/*! \file halSimulate.h
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

#ifndef HAL_SIMULATE_H
#define HAL_SIMULATE_H

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//! Data item for simulation (digital/analog inputs)
typedef struct {
    UInt32 Duration;        //!< Time duration for that item
    UInt16 StartValue;      //!< Initial data value
    Int16  Offset;          //!< Increment value 
    Int16  Steps;           //!< Number of increment steps
    Int16  Goto;            //!< Index of next data item (relative)
} INPUT_PORT_DATA_t;

//! Structure to hold the state for simulation (digital/analog inputs)
typedef struct {
    UInt16 Elements;        //!< Number of data items
    UInt32 StartTime;       //!< Start time for actual item
    UInt16 Value;           //!< Actual port/channel state 
    Int16 Steps;            //!< Actual step in data item
    Int16 Index;            //!< Index of actual data item  
        
    const INPUT_PORT_DATA_t *Data;  //!< table of data items
} INPUT_SIMULATION_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

ERROR_t halRegisterAnalogInputPattern (CHANNEL_t Channel, 
          const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize);            
ERROR_t halRegisterDigitalInputPattern (CHANNEL_t Channel, 
          const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize);
Bool    halPowerSupplyGood (void);

ERROR_t halControlTask  (void);

//****************************************************************************/

#endif /*HAL_SIMULATE_H*/

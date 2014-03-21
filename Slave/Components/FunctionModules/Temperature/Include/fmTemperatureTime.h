/****************************************************************************/
/*! \file fmTemperatureTime.h
 * 
 *  \brief Methods maintaining the operating time.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *  The functions in this file control the operating time of the heating
 *  elements and the electronics. They store and read out minute grained time
 *  values from a non-volatile memory device.
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

#ifndef FMTEMPERATURETIME_H
#define FMTEMPERATURETIME_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

/*! Contains all variables controlling a non-volatile lifecycle counter */
typedef struct {
    UInt16 Start;                   //!< Starting address of the lifecycle counters
    UInt32 TimeMs;                  //!< Accumulated PWM time in milliseconds
    Handle_t Handle;                //!< Handle to access a storage partition (HAL)
    UInt8 NumberHeaters;            //!< Number of heating elments for temperature regulation
    bmParamRange_t *PermDataTable;  //!< Range of the different storage parameters
} TempTimeParams_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t tempTimeResetPartition (const TempTimeParams_t *Params, UInt16 Channel);
Error_t tempTimeInit (UInt16 ModuleID, UInt16 Instance, TempTimeParams_t *Params, UInt8 Number);
UInt32  tempTimeRead (const TempTimeParams_t *Params, UInt8 Number);
Error_t tempTimeReset (const TempTimeParams_t *Params, UInt8 Number);
Error_t tempTimeUpdate (TempTimeParams_t *Params, UInt32 Time);

//****************************************************************************/

#endif /*FMTEMPERATURETIME_H*/

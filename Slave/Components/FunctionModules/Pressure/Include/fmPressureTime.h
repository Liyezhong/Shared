/****************************************************************************/
/*! \file fmPressureTime.h
 * 
 *  \brief Methods maintaining the operating time.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
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

#ifndef FMPRESSURETIME_H
#define FMPRESSURETIME_H

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
    UInt8 NumberPumps;            //!< Number of heating elments for presserature regulation
    bmParamRange_t *PermDataTable;  //!< Range of the different storage parameters
} PressTimeParams_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t pressTimeResetPartition (const PressTimeParams_t *Params, UInt16 Channel);
Error_t pressTimeInit (UInt16 ModuleID, UInt16 Instance, PressTimeParams_t *Params, UInt8 Number);
UInt32  pressTimeRead (const PressTimeParams_t *Params, UInt8 Number);
Error_t pressTimeReset (const PressTimeParams_t *Params, UInt8 Number);
Error_t pressTimeUpdate (PressTimeParams_t *Params, UInt32 Time);

//****************************************************************************/

#endif /*FMPRESSURETIME_H*/

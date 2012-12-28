/****************************************************************************/
/*! \file fmPressurePump.h
 * 
 *  \brief Functions controlling a pumping element.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *  The methods defined here control the pressure of a heating element. The
 *  needed input parameter is computed by a PID controller. There are also
 *  functions that determine the connection between various heating elements.
 *  Furthermore, the current usage in milliamperes of the elements is computed
 *  here.
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

#ifndef FMPRESSUREPUMP_H
#define FMPRESSUREPUMP_H

#include "halLayer.h"

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

/*! Contains parameters for the pump current measurements */
typedef struct {
    UInt16 CurrentGain;             //!< Gain parameter of the current sensor in mA/V
    UInt16 DesiredCurrent;          //!< Desired value of the effective current in milliamperes
    UInt16 DesiredCurThreshold;     //!< Desired current threshold in milliamperes
} PressPumpParams_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t pressPumpInit (PressPumpParams_t **Params, Device_t CurrentChannel, Device_t SwitchChannel, Device_t ControlChannel, UInt16 Instances);
void pressPumpReset (void);
Error_t pressPumpProgress (void);
Bool pressPumpParallel (void);
Error_t pressPumpCheck (void);
Bool pressPumpFailed (void);
Error_t pressPumpActuate (UInt32 OperatingTime, UInt32 EndTime, UInt16 Instance);
UInt16 pressPumpCurrent (void);
UInt16 pressPumpActive(void);

//****************************************************************************/

#endif /*FMPRESSUREPUMP_H*/

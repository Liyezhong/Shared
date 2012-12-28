/****************************************************************************/
/*! \file fmTemperatureHeater.h
 * 
 *  \brief Functions controlling a heating element.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *  The methods defined here control the temperature of a heating element. The
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

#ifndef FMTEMPERATUREHEATER_H
#define FMTEMPERATUREHEATER_H

#include "halLayer.h"

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

/*! Currently supported heaters */
typedef enum {
    TYPE_HEATER_AC, //!< AC heater
    TYPE_HEATER_DC, //!< DC heater
} TempHeaterType_t;

/*! Contains parameters for the heater current measurements */
typedef struct {
    UInt16 CurrentGain;             //!< Gain parameter of the current sensor in mA/V
    UInt16 DesiredCurrent;          //!< Desired value of the effective current in milliamperes
    UInt16 DesiredCurThreshold;     //!< Desired current threshold in milliamperes
} TempHeaterParams_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t tempHeaterInit (TempHeaterParams_t **Params, Device_t CurrentChannel, Device_t SwitchChannel, Device_t ControlChannel, TempHeaterType_t HeaterType, UInt16 Instances);
void tempHeaterReset (void);
Error_t tempHeaterProgress (void);
Error_t tempSampleCurrent (void);
Bool tempHeaterParallel (void);
Error_t tempHeaterCheck (Bool AutoSwitch);
Bool tempHeaterFailed (void);
Error_t tempHeaterActuate (UInt32 OperatingTime, UInt32 EndTime, UInt16 Instance);
UInt16 tempHeaterCurrent (void);
UInt16 tempHeaterActive(void);
void tempCalcEffectiveCurrent(void);

//****************************************************************************/

#endif /*FMTEMPERATUREHEATER_H*/

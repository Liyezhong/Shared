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
    UInt16 CurrentDeviation;        //!< Current deviation in milliamperes when heater is off
    UInt16 CurrentMin230_Serial;    //!< Desired minimal effective current in mA for 230 VAC in Serial mode
    UInt16 CurrentMax230_Serial;    //!< Desired maximal effective current in mA for 230 VAC in Serial mode
    UInt16 CurrentMin100_Serial;    //!< Desired minimal effective current in mA for 100 VAC in Serial mode
    UInt16 CurrentMax100_Serial;    //!< Desired maximal effective current in mA for 100 VAC in Serial mode
    UInt16 CurrentMin100_Parallel;  //!< Desired minimal effective current in mA for 100 VAC in Parallel mode
    UInt16 CurrentMax100_Parallel;  //!< Desired maximal effective current in mA for 100 VAC in Parallel mode
} TempHeaterParams_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t tempHeaterInit (TempHeaterParams_t **Params, Device_t CurrentChannel, Device_t SwitchChannel, Device_t ControlChannel, TempHeaterType_t HeaterType, UInt16 Instances);
void tempHeaterReset (void);
Error_t tempHeaterProgress (TempHeaterType_t HeaterType);
Error_t tempSampleCurrent (TempHeaterType_t HeaterType);
Bool tempHeaterParallel (void);
Error_t tempHeaterCheck (UInt16 Instance, TempHeaterType_t HeaterType, Bool CurrentCheck, Bool AutoSwitch);
Bool tempHeaterFailed (void);
Error_t tempHeaterActuate (UInt32 OperatingTime, UInt32 EndTime, UInt16 Instance);
UInt16 tempHeaterCurrent (void);
UInt16 tempHeaterActive(void);
void tempCalcEffectiveCurrent(UInt16 Instance, TempHeaterType_t HeaterType);
UInt16 tempGetActiveDesiredCurrent(void);
UInt16 tempGetActiveDesiredCurThreshold(void);
UInt8 tempHeaterSwitchState(void);
UInt32 tempGetActiveStatus(void);
Error_t tempSetHeaterSwitchState(UInt8 State);
void tempGetActiveAcCurrentSerial(UInt16* CurMin230_S, UInt16* CurMax230_S, UInt16* CurMin100_S, UInt16* CurMax100_S);
void tempGetActiveAcCurrentParallel(UInt16* CurMin100_P, UInt16* CurMax100_P);
//****************************************************************************/

#endif /*FMTEMPERATUREHEATER_H*/

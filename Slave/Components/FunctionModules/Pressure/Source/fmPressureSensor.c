/****************************************************************************/
/*! \file fmPressureSensor.c
 * 
 *  \brief Functions reading the pressure sensor.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *  This file controls the read out of data from a pressure sensor. It
 *  converts the pressure values to 0.01 steps in degree Celsius. 
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

#include "Global.h"
#include "halLayer.h"
#include "BaseModule.h"
#include "fmPressure.h"
#include "fmPressureSensor.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/
#define SWAP(X,Y) {X=X^Y;Y=X^Y;X=X^Y;}

#define TEMP_SENSOR_MAX 120 //!< Maximal sensor pressure in degree Celsius 

#define PRESS_SENSOR_VOLTAGE_LOW     500  //!< The lowest voltage allowed for pressure sensor 
#define PRESS_SENSOR_VOLTAGE_HIGH    3000 //!< The highest voltage allowed for pressure sensor


//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Private Variables 
//****************************************************************************/

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/


/*****************************************************************************/
/*! 
 *  \brief   Reads a pressure sensor
 *
 *      This method reads an analog input port. The fetched value is then
 *      converted into a pressure using the conversion table of the sensor
 *      type that is passed to this function.
 * 
 *  \iparam  Handle = Handle of the analog input
 *  \iparam  Type = Type of the pressure sensor
 *  \iparam  Compensation = Atmospheric pressure
 *  \oparam  Pressure = Pressure in 0.001 kPa
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t pressSensorRead (Handle_t Handle, PressSensorType_t Type, UInt16 Compensation, Int32* Pressure)
{
    Error_t Error;
    Int16 AdcValue;
    Int16 AdcValue1, AdcValue2, AdcValue3;
       
#if 0
    if ((Error = halAnalogRead (Handle, &AdcValue)) < 0) {
        return (Error);
    }
#endif    
 
#if 1    
	// Median filtering
    if ((Error = halAnalogRead (Handle, &AdcValue1)) < 0) {
        return (Error);
    }
    if ((Error = halAnalogRead (Handle, &AdcValue2)) < 0) {
        return (Error);
    }
    if ((Error = halAnalogRead (Handle, &AdcValue3)) < 0) {
        return (Error);
    }

	if (AdcValue1>AdcValue2) {
	    SWAP(AdcValue1,AdcValue2);
	}
	if (AdcValue1>AdcValue3) {
	    SWAP(AdcValue1,AdcValue3);
	}
	if (AdcValue2>AdcValue3) {
	    SWAP(AdcValue2,AdcValue3);
	}
	
	AdcValue = AdcValue2;
    
    //printf("AD:%d\n", AdcValue);
#endif
    
    // Voltage: 1-6V ==> 0.5-3V
	//*Pressure = 103.42-40.944*(6-AdcValue*2);
    
    if ( AdcValue > PRESS_SENSOR_VOLTAGE_HIGH || AdcValue < PRESS_SENSOR_VOLTAGE_LOW ) {
        return (E_PRESS_SENSOR_OUT_OF_RANGE);
    }
    
	*Pressure = 103420-41*(PRESS_SENSOR_VOLTAGE_HIGH*2-AdcValue*2) - Compensation;

	return (NO_ERROR);

}


//****************************************************************************/

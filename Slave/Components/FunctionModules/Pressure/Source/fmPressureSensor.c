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

#define TEMP_SENSOR_MAX 120 //!< Maximal sensor pressure in degree Celsius 

#define TEMP_SENSOR_VOLTAGE     3000  //!< ADC supply voltage used for the NTC 10K3A1I
#define TEMP_SENSOR_RESISTANCE  10000 //!< Pullup resistance connected to NTC 10K3A1I


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
       

    if ((Error = halAnalogRead (Handle, &AdcValue)) < 0) {
        return (Error);
    }
    
    //printf("Ad:%d\n", AdcValue);
    
    // Voltage: 1-6V ==> 0.5-3V
	//*Pressure = 103.42-40.944*(6-AdcValue*2);
	*Pressure = 103420-41*(6000-AdcValue*2) - Compensation;

	return (NO_ERROR);

}


//****************************************************************************/

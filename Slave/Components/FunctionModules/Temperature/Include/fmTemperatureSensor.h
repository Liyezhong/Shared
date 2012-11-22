/****************************************************************************/
/*! \file fmTemperatureSensor.h
 * 
 *  \brief Functions reading the temperature sensor.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *  This file controls the read out of data from a temperature sensor. It
 *  converts the temperature values to 0.01 steps in degree Celsius. 
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

#ifndef FMTEMPERATURESENSOR_H
#define FMTEMPERATURESENSOR_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

/*! Currently supported temperature sensor families */
typedef enum {
    TYPEK,      //!< Thermocouple of type K (IEC 584)
    NTC10K3A1I, //!< Betatherm NTC 10K3A1I thermistor
    AD595,      //!< Analog Devices AD595 thermocouple amplifier
    TYPET,      //!< Thermocouple of type T (IEC 584)
    PT1000      //!< PT1000 resistance thermometer
} TempSensorType_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t tempSensorRead (Handle_t Handle, TempSensorType_t Type, UInt16 ColdJunction, UInt16* Temperature);

//****************************************************************************/

#endif /*FMTEMPERATURESENSOR_H*/

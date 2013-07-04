/****************************************************************************/
/*! \file fmPressureSensor.h
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

#ifndef FMPRESSURESENSOR_H
#define FMPRESSURESENSOR_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

/*! Currently supported pressure sensor families */
typedef enum {
    TYPE_UNDEF
} PressSensorType_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t pressSensorRead (Handle_t Handle, PressSensorType_t Type, UInt16 Compensation, Int32* Pressure);

//****************************************************************************/

#endif /*FMPRESSURESENSOR_H*/

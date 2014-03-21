/****************************************************************************/
/*! \file ModuleIDs.h
 *
 *  \brief Module ID definition file
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This is the plattform-wide module ID definition file, which contains
 *       all module identifiers of all function modules. Each module ID must
 *       be unique, i.e. can only be assigned once.
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

#ifndef MODULE_ID_H
#define MODULE_ID_H

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

#define MODULE_ID_BASEMODULE      0     //!< Module ID for base module
#define MODULE_ID_DIGITAL_OUT     1     //!< Module ID for digital input
#define MODULE_ID_DIGITAL_IN      2     //!< Module ID for digital output
#define MODULE_ID_ANALOG_OUT      3     //!< Module ID for analog input
#define MODULE_ID_ANALOG_IN       4     //!< Module ID for analog output
#define MODULE_ID_STEPPER         7     //!< Module ID for stepper motor
#define MODULE_ID_JOYSTICK        8     //!< Module ID for adjustment joystick
#define MODULE_ID_RFID11785       9     //!< Module ID for RFID ISO 11785 transponders
#define MODULE_ID_RFID15693       10    //!< Module ID for RFID ISO 15693 transponders
#define MODULE_ID_TEMPERATURE     11    //!< Module ID for temperature control
#define MODULE_ID_UART            12    //!< Module ID for serial interface

#define MODULE_ID_PRESSURE        13    //!< Module ID for serial interface

#define MODULE_ID_SIMUALTION      0xFFF //!< Modules ID for test simulation

//****************************************************************************/

#endif /*MODULE_ID_H*/

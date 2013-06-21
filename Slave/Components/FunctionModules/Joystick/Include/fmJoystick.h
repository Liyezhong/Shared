/****************************************************************************/
/*! \file fmJoystick.h
 * 
 *  \brief Functional module controlling the adjustment joystick.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 19.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This module controls a joystick that is used as an adjustement sensor.
 *      The joystick is needed to calibrate moving parts that are operated by
 *      electric motors. With the help of the joystick the operating range of
 *      such a device can be measured. The joystick signal is sampled by an
 *      analog to digital converter which is the only hardware component
 *      needed by this functional module. The operation parameters are set
 *      through the CAN bus interface. The module is also able to access
 *      non-volatile memory to store the mechanical offsets and the
 *      calibration information of the hardware controlled by this module.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 * 
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice 
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef FMJOYSTICK_H
#define FMJOYSTICK_H

//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/

// Joystick module related errors
//! Module is still disabled
#define E_JOYS_MODULE_DISABLED      BUILD_ERRCODE(MODULE_ID_JOYSTICK, ERRCLASS_ERROR, 0)

// Joystick related CAN identifiers
//! Sets configuration and control parameters
#define MSG_JOYS_SET_CONFIGURATION  BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)
//! Sets the mechanical offset data of the device
#define MSG_JOYS_SET_MECH_OFFSET    BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)

//! Requests the joystick displacement
#define MSG_JOYS_REQ_DISPLACEMENT   BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)
//! Requests the mechanical offset data of the device
#define MSG_JOYS_REQ_MECH_OFFSET    BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 1)

//! Responds the joystick displacement
#define MSG_JOYS_RESP_DISPLACEMENT  BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 0)
//! Responds the mechanical offset data of the device
#define MSG_JOYS_RESP_MECH_OFFSET   BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 0)

//****************************************************************************/
// Public Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t joysInitializeModule (UInt16 ModuleID, UInt16 NumberOfInstances);
void joysRegisterDebugNames  (UInt16 ModuleID);

//****************************************************************************/

#endif /*FMJOYSTICK_H*/

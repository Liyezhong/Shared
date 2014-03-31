/****************************************************************************/
/*! \file fmAnaInput.h
 *
 *  \brief Function Module for Analog Inputs
 *
 *  $Version: $ 0.4
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to control and read an analog
 *       input channel. A channel is generally 16 bit wide; in case the
 *       hardware delivers lesser bits, they will be scaled to 16 bits.
 *
 *       The function module uses logical channels; the mapping to a real
 *       hardware channel is done by the hardware abstraction layer.
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

#ifndef ANALOG_INPUT_H
#define ANALOG_INPUT_H

//@{**************************************************************************/
//! CAN messages used by this function module
//****************************************************************************/

// CAN-IDs for event messages (handled by base module)
#define MSG_AI_EVENT_INFO          MSG_EVENT_INFO
#define MSG_AI_EVENT_WARNING       MSG_EVENT_WARNING
#define MSG_AI_EVENT_ERROR         MSG_EVENT_ERROR
#define MSG_AI_EVENT_FATAL         MSG_EVENT_FATAL

// CAN-IDs for service and process data (handled by base module)
#define MSG_AI_SRV_STATISTICS      MSG_SRV_STATISTICS
#define MSG_AI_SRV_CFG_STATISTICS  MSG_SRV_CFG_STATISTICS
#define MSG_AI_SRV_REQ_RESET_DATA  MSG_SRV_REQ_RESET_DATA
#define MSG_AI_SRV_ACK_RESET_DATA  MSG_SRV_ACK_RESET_DATA
#define MSG_AI_PROC_CONFIGURE      MSG_PROC_CONFIGURE
#define MSG_AI_PROC_REQ_DATA       MSG_PROC_REQ_DATA
#define MSG_AI_PROC_DATA           MSG_PROC_DATA

// CAN-IDs for analog input (handled by function module)
#define MSG_AI_CONFIG_INPUT        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)
#define MSG_AI_CONFIG_LIMITS       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)
#define MSG_AI_REQ_INPUT_STATE     BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)
#define MSG_AI_INPUT_STATE         BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 0)

//@} End of doxygen group

//@{**************************************************************************/
//! Events for this function module (Errors, Warnings, Infos)
//! \remark This function is currently not used.
//****************************************************************************/

#define BUILD_AI_ERRCODE(Class,Code) \
                        BUILD_ERRCODE(MODULE_ID_ANALOG_IN, Class, Code)

#define W_ANAIN_ABOVE_LIMIT       BUILD_AI_ERRCODE(ERRCLASS_WARNING, 1)
#define W_ANAIN_BELOW_LIMIT       BUILD_AI_ERRCODE(ERRCLASS_WARNING, 2)

//@} End of doxygen group

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t aiInitializeModule   (UInt16 ModuleID, UInt16 NumberOfInstances);
void    aiRegisterDebugNames (UInt16 ModuleID);

//****************************************************************************/

#endif /*ANALOG_INPUT_H*/

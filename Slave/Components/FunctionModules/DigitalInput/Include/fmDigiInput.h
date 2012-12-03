/****************************************************************************/
/*! \file fmDigiInput.h
 *
 *  \brief Function module to control digital input ports
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *         This module contains all functions to control and read a digital
 *         input port. A port can be 1...32 bits wide. The function module
 *         uses logical ports; the mapping to a real hardware port is done
 *         by the hardware abstraction layer. Also the width of the port is
 *         determined by the HAL and can't be changed here.
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

#ifndef DIGITAL_INPUT_H
#define DIGITAL_INPUT_H

//@{**************************************************************************/
//! CAN messages used by this function module
//****************************************************************************/

// CAN-IDs for event messages (handled by base module)
#define MSG_DI_EVENT_INFO          MSG_EVENT_INFO
#define MSG_DI_EVENT_WARNING       MSG_EVENT_WARNING
#define MSG_DI_EVENT_ERROR         MSG_EVENT_ERROR
#define MSG_DI_EVENT_FATAL         MSG_EVENT_FATAL

// CAN-IDs for service and process data (handled by base module)
#define MSG_DI_SRV_STATISTICS      MSG_SRV_STATISTICS
#define MSG_DI_SRV_CFG_STATISTICS  MSG_SRV_CFG_STATISTICS
#define MSG_DI_SRV_REQ_RESET_DATA  MSG_SRV_REQ_RESET_DATA
#define MSG_DI_SRV_ACK_RESET_DATA  MSG_SRV_ACK_RESET_DATA
#define MSG_DI_PROC_CONFIGURE      MSG_PROC_CONFIGURE
#define MSG_DI_PROC_REQ_DATA       MSG_PROC_REQ_DATA
#define MSG_DI_PROC_DATA           MSG_PROC_DATA

// CAN-IDs for digital input (handled by function module)
#define MSG_DI_CONFIG_INPUT        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)
#define MSG_DI_CONFIG_LIMITS       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)
#define MSG_DI_REQ_INPUT_STATE     BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)
#define MSG_DI_INPUT_STATE         BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 0)

//@} End of doxygen group

//@{**************************************************************************/
//! Events for this function module (Errors, Warnings, Infos)
//! \remark This function is currently not used.
//****************************************************************************/

#define BUILD_DI_ERRCODE(Class,Code) \
                        BUILD_ERRCODE(MODULE_ID_DIGITAL_IN, Class, Code)

#define W_DIGIN_ABOVE_LIMIT        BUILD_DI_ERRCODE(ERRCLASS_WARNING, 1)
#define W_DIGIN_BELOW_LIMIT        BUILD_DI_ERRCODE(ERRCLASS_WARNING, 2)

//@} End of doxygen group

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t diInitializeModule   (UInt16 ModuleID, UInt16 NumberOfInstances);
void    diRegisterDebugNames (UInt16 ModuleID);

//****************************************************************************/

#endif /*DIGITAL_INPUT_H*/

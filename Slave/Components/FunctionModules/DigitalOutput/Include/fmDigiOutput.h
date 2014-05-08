/****************************************************************************/
/*! \file fmDigiOutput.h
 *
 *  \brief Function module to control digital outputs
 *
 *   $Version: $ 0.3
 *   $Date:    $ 27.01.2011
 *   $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to control and write digital
 *       output ports. It supports enhanced features, like timed output
 *       and blinking. Timed outputs include delays and durations.
 *
 *       If a delay time is specified in an output command, the new output
 *       value is applied not before the delay time has expired. If a
 *       duration time is specified, the new output becomes active for
 *       the specified duration and falls back to the previous value
 *       when the time has expired. Delay and duration can be combined.
 *
 *       Blinking means that the output toggles between the value specified
 *       in the output command and an alternative value. The alternative
 *       value can be either the configured Inactive level, or the output
 *       level been active before the blink command. All blink parameters
 *       can be configured via CAN messages. Blinking can be continuously
 *       or morse style ('n' blinks followed by a pause).
 *
 *       The function module uses logical channels; the mapping to a real
 *       hardware channel is done by the hardware abstraction layer. The
 *       width of an output port can be 1..16 bits and is defined by the
 *       hardware abstraction layer.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DIGITAL_OUTPUT_H
#define DIGITAL_OUTPUT_H

//@{**************************************************************************/
//! CAN messages used by this function module
//****************************************************************************/

// CAN-IDs for event messages (handled by base module)
#define MSG_DO_EVENT_INFO          MSG_EVENT_INFO
#define MSG_DO_EVENT_WARNING       MSG_EVENT_WARNING
#define MSG_DO_EVENT_ERROR         MSG_EVENT_ERROR
#define MSG_DO_EVENT_FATAL         MSG_EVENT_FATAL

// CAN-IDs for service and process data (handled by base module)
#define MSG_DO_SRV_CFG_STATISTICS  MSG_SRV_CFG_STATISTICS
#define MSG_DO_SRV_STATISTICS      MSG_SRV_STATISTICS
#define MSG_DO_SRV_REQ_RESET_DATA  MSG_SRV_REQ_RESET_DATA
#define MSG_DO_SRV_ACK_RESET_DATA  MSG_SRV_ACK_RESET_DATA
#define MSG_DO_PROC_CONFIGURE      MSG_PROC_CONFIGURE
#define MSG_DO_PROC_REQ_DATA       MSG_PROC_REQ_DATA
#define MSG_DO_PROC_DATA           MSG_PROC_DATA

// CAN-IDs for digital output (handled by function module)
#define MSG_DO_CONFIG_OUTPUT       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 1)
#define MSG_DO_CONFIG_BLINKING     BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)
#define MSG_DO_SET_OUTPUT_STATE    BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)
#define MSG_DO_REQ_OUTPUT_STATE    BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 1)
#define MSG_DO_OUTPUT_STATE        BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 0)

#define MSG_DO_REQ_LIFETIME_DATA   BUILD_CAN_ID(CMD_CLASS_SERVICE,  5, 1)
#define MSG_DO_LIFETIME_DATA       BUILD_CAN_ID(CMD_CLASS_SERVICE,  5, 0)

//@} End of doxygen group

//@{**************************************************************************/
//! Events for this function module (Errors, Warnings, Infos)
//****************************************************************************/

#define BUILD_DO_ERRCODE(Class,Code) \
                        BUILD_ERRCODE(MODULE_ID_DIGITAL_OUT, Class, Code)

//@} End of doxygen group

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t doInitializeModule (UInt16 ModuleID, UInt16 Instances);
#ifdef DEBUG
void    doRegisterDebugNames  (UInt16 ModuleID);
#endif

//****************************************************************************/

#endif /*DIGITAL_OUTPUT_H*/

/****************************************************************************/
/*! \file fmInclinometer.h
 * 
 *  \brief Function module to control analog input ports
 *
 *  $Version: $ 0.1
 *  $Date:    $ 09.06.2010
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

#ifndef INCLINOMETER_H
#define INCLINOMETER_H

#include "ModuleIDs.h"

//****************************************************************************/
// CAN Messages for this Function Module
//****************************************************************************/

// CAN-IDs for event messages (handled by base module) 
#define MSG_INC_EVENT_INFO         MSG_EVENT_INFO
#define MSG_INC_EVENT_WARNING      MSG_EVENT_WARNING
#define MSG_INC_EVENT_ERROR        MSG_EVENT_ERROR
#define MSG_INC_EVENT_FATAL        MSG_EVENT_FATAL

// CAN-IDs for service and process data (handled by base module)
#define MSG_INC_SRV_STATISTIC      MSG_SRV_STATISTICS
#define MSG_INC_SRV_CFG_STATISTIC  MSG_SRV_CFG_STATISTICS
#define MSG_INC_PROC_CONFIGURE     MSG_PROC_CONFIGURE
#define MSG_INC_PROC_DATA          MSG_PROC_DATA
    
// CAN-IDs for inclinometer (handled by function module)
#define MSG_INC_INPUT_DATA         BUILD_CAN_ID(CMD_CLASS_FUNCTION, 5, 0)
#define MSG_INC_INPUT_REQUEST      BUILD_CAN_ID(CMD_CLASS_FUNCTION, 6, 1)
#define MSG_INC_INPUT_CONFIG       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 7, 1)
#define MSG_INC_LIMIT_CONFIG       BUILD_CAN_ID(CMD_CLASS_FUNCTION, 8, 1)


//****************************************************************************/
// Events for this Function Module (Errors, Warnings, Infos)
//****************************************************************************/

#define BUILD_INC_ERRCODE(Class,Code) \
                        BUILD_ERRCODE(MODULE_ID_INCLINOMETER, Class, Code)

#define E_INCLINO_NOT_ENABLED      BUILD_INC_ERRCODE(ERRCLASS_ERROR, 1)
#define E_INCLINO_NOT_USABLE       BUILD_INC_ERRCODE(ERRCLASS_ERROR, 2)

#define W_INCLINO_ABOVE_LIMIT      BUILD_INC_ERRCODE(ERRCLASS_WARNING, 1)
#define W_INCLINO_BELOW_LIMIT      BUILD_INC_ERRCODE(ERRCLASS_WARNING, 2)


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

ERROR_t incInitializeModule (UInt16 NumberOfInstances);

//****************************************************************************/

#endif /*INCLINOMETER_H*/

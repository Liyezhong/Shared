/****************************************************************************/
/*! \file bmSystem.h
 *
 *  \brief System message handling
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions handling CAN system messages.
 *       This includes:
 *
 *       - Configuration management (master/slave)
 *       - Mode change requests
 *       - Heartbeat handling
 *       - Emergency stop handling (notstopp)
 *       - Node reset
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

#ifndef BM_SYSTEM_H
#define BM_SYSTEM_H

//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//! Node states definitions
typedef enum {
    NODE_STATE_UNDEFINED = 0,    //!< Undefined state
    NODE_STATE_BOOTING   = 1,    //!< Node is in boot loader
    NODE_STATE_STARTUP   = 2,    //!< Firmware initialization
    NODE_STATE_IDENTIFY  = 3,    //!< Node identification
    NODE_STATE_CONFIGURE = 4,    //!< Node configuration
    NODE_STATE_NORMAL    = 5,    //!< Normal operation mode
    NODE_STATE_ASSEMBLY  = 6,    //!< Assembly mode
    NODE_STATE_SHUTDOWN  = 7,    //!< Shutdown (going to standby)
    NODE_STATE_STANDBY   = 8,    //!< Standby
    NUMBER_OF_NODE_STATES        //!  MUST be last element in enum list
} bmNodeState_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t bmSendHardwareID        (void);
Error_t bmProcessHeartbeat      (void);
Error_t bmProcessEmergencyStop  (void);
Error_t bmInitSystemCommands    (void);
Bool    bmGetEmergencyStop      (void);
void    bmEmergencyStopByVoltage (Bool PowerFail);


bmNodeState_t bmProcessModeChange (void);
bmNodeState_t bmSetNodeState (bmNodeState_t NewState);
bmNodeState_t bmGetNodeState (void);

//****************************************************************************/

#endif /*BM_SYSTEM_H*/

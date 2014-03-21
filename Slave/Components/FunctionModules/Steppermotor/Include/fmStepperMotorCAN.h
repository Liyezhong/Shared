/****************************************************************************/
/*! \file fmStepperMotorCAN.h
 * 
 *  $Rev:    $ 0.1
 *  $Date:   $ 03.07.2012
 *  $Author: $ rainer Boehles
 *
 *  \brief Publics for CAN-communication functions of module 'stepper motor'
 *
 *         
 * <dl compact><dt>Company:</dt><dd> Leica Biosystems Nussloch GmbH </dd></dl>
 *
 * (c) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 * This is unpublished proprietary source code of Leica.
 * The copyright notice does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef FMSTEPPERMOTORCAN_H
#define FMSTEPPERMOTORCAN_H




//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/



//****************************************************************************/
// Module Function Prototypes
//****************************************************************************/

//!< set configuration parameters
Error_t smConfigure(UInt16 Channel, CanMessage_t* Message);

//!< request actual motor position
Error_t smReqPosition (UInt16 Channel, CanMessage_t* Message);

//!< request reference run
Error_t smReferenceRun(UInt16 Channel, CanMessage_t* Message);

//!< acknowledge finished reference run
Error_t smRefRunAck(UInt16 Channel, Int32 Pos, Int8 PosCode, SM_AckState_t Ack);

//!< request actual motor speed
Error_t smReqSpeed (UInt16 Channel, CanMessage_t* Message);

//!< request position movement
Error_t smTargetPosition(UInt16 Channel, CanMessage_t* Message);

//!< acknowledge fo finished movement
Error_t smSendMovementAckn (UInt16 Channel, Int32 Pos, Int16 Speed, SM_AckState_t Ack);

//!< request speed movement
Error_t smTargetSpeed(UInt16 Channel, CanMessage_t* Message);

//!< request to enable / disable the module
Error_t smSetEnableState(UInt16 Channel, CanMessage_t* Message);

//!< request motors operation time
Error_t smReqOperationTime(UInt16 Channel, CanMessage_t* Message);

//!< request motors revolution count
Error_t smReqRevolutionCount(UInt16 Channel, CanMessage_t* Message);

#endif /*FMSTEPPERMOTORCAN_H*/

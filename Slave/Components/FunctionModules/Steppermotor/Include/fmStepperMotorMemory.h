/****************************************************************************/
/*! \file fmStepperMotorLifeTime.h
 * 
 *  $Rev:    $ 0.1
 *  $Date:   $ 02.10.2012
 *  $Author: $ Rainer Boehles
 *
 *  \brief 
 *
 *         
 * <dl compact><dt>Company:</dt><dd> Leica Biosystems Nussloch GmbH </dd></dl>
 *
 * (c) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 * This is unpublished proprietary source code of Leica.
 * The copyright notice does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef FMSTEPPERMOTORLIFETIME_H
#define FMSTEPPERMOTORLIFETIME_H


#define SM_MEMORY_UPDATE_INTERVAL  60000        //!< Life cycle data update interval in ms


typedef struct
{
    Handle_t                Handle;             //!< Non-volatile storage handle 
    UInt32                  Interval;           //!< Storage update interval    
} smMemory_t;



//****************************************************************************/
// Public Constants and Macros 
//****************************************************************************/



//****************************************************************************/
// Module Function Prototypes
//****************************************************************************/

UInt32 smGetRevolution(smMemory_t *Memory);
UInt32 smGetOperationTime(smMemory_t *Memory);

Error_t smResetMemory (UInt16 Instance);

Error_t smFlushMemory(UInt16 Instance, UInt16 Interval);

void smInitMemory(UInt16 Instance);


#endif /*FMSTEPPERMOTORLIFETIME_H*/

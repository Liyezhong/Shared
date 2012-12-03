/****************************************************************************/
/*! \file
 * 
 *  \brief  Interrupt service routine performing the (micro-)stepping of 
 *          function module 'stepper motor'
 *
 *  $Rev:    $ 0.1
 *  $Date:   $ 27.06.2012
 *  $Author: $ Rainer Boehles
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef FMSTEPPERMOTORMOTIONISR_H
#define FMSTEPPERMOTORMOTIONISR_H

//********************************************************************************/
// Public Constants and Definitions
//********************************************************************************/

#define ISR_MICROSTEPS_PER_HALFSTEP         SM_MICROSTEPS_PER_HALFSTEP  //!< step unit used for calculations by ISR
#define ISR_TIMEBASE                        USEC                        //!< time unit used for calculations by ISR


#ifdef STEPPER_ISR_TIME_OPTIMIZATION_OFF
    #define ISR_WORST_CASE_DURATION         40      //!< worst case duration of stepper timer interrupt in µsec
#else
    #define ISR_WORST_CASE_DURATION         20      //!< worst case duration of stepper timer interrupt in µsec
#endif


#define ISR_ENCODER_TIME_LIMIT              100     //!< minimal stepper timer interrupt interval time in µsec, at which step loss can be processed


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

//!< Timer interrupt service routine for motor motion control
void smMotionISR (UInt32 UserTag, UInt32 IntrFlags);


#endif /*FMSTEPPERMOTORMOTIONISR_H*/

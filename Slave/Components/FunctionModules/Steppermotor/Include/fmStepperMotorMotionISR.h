
#ifndef FMSTEPPERMOTORMOTIONISR_H
#define FMSTEPPERMOTORMOTIONISR_H


#define ISR_MICROSTEPS_PER_HALFSTEP			MICROSTEPS_PER_HALFSTEP // step unit used for calculation by isr
#define ISR_TIMEBASE						USEC


#ifdef STEPPER_ISR_TIME_OPTIMIZATION_OFF
    #define ISR_WORST_CASE_DURATION				40		// worst case duration of stepper timer interrupt in µsec
#else
    #define ISR_WORST_CASE_DURATION				20		// worst case duration of stepper timer interrupt in µsec
#endif


    #define ISR_ENCODER_TIME_LIMIT				100		// minimal stepper timer interrupt interval time in µsec, at which step loss can be processed


//!< Timer interrupt service routine for motor motion control
void smMotionISR (UInt32 UserTag, UInt32 IntrFlags);


#endif /*FMSTEPPERMOTORMOTIONISR_H*/

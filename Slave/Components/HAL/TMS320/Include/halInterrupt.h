/****************************************************************************/
/*! \file halInterrupt.h
 * 
 *  \brief  Interrupt Control Functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 11.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to virtualize interrupts. It controls
 *       the peripheral interrupt expansion unit and interrupt stub functions.
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

#ifndef HAL_INTERRUPTS
#define HAL_INTERRUPTS

//****************************************************************************/
// Interrupt Vector Identifier 
//****************************************************************************/

//! Logical interrupt identifier
typedef enum {
    INTERRUPTS_GLOBAL        = 0,    // Interrupt ID for cpu interrupt mask  
    INTERRUPT_TIMER1         = 13,  
    INTERRUPT_TIMER2         = 14,
    INTERRUPT_DATALOG        = 15,
    INTERRUPT_RTOS           = 16,
    INTERRUPT_EMULATE        = 17,
    INTERRUPT_NMI            = 18,
    INTERRUPT_ILLEGAL_OPCODE = 19,
    INTERRUPT_USER_TRAP1     = 20,
    INTERRUPT_USER_TRAP2     = 21,
    INTERRUPT_USER_TRAP3     = 22,
    INTERRUPT_USER_TRAP4     = 23,
    INTERRUPT_USER_TRAP5     = 24,
    INTERRUPT_USER_TRAP6     = 25,
    INTERRUPT_USER_TRAP7     = 26,
    INTERRUPT_USER_TRAP8     = 27,
    INTERRUPT_USER_TRAP9     = 28,
    INTERRUPT_USER_TRAP10    = 29,
    INTERRUPT_USER_TRAP11    = 30,
    INTERRUPT_USER_TRAP12    = 31,
    INTERRUPT_ADC_SEQ1       = 32,
    INTERRUPT_ADC_SEQ2       = 33,
    INTERRUPT_EXTERN1        = 35,     
    INTERRUPT_EXTERN2        = 36,
    INTERRUPT_ADC            = 37,
    INTERRUPT_TIMER0         = 38,
    INTERRUPT_WAKE           = 39,           
    INTERRUPT_PWM1_TZ        = 40,
    INTERRUPT_PWM2_TZ        = 41,
    INTERRUPT_PWM3_TZ        = 42,
    INTERRUPT_PWM4_TZ        = 43,
    INTERRUPT_PWM5_TZ        = 44,
    INTERRUPT_PWM6_TZ        = 45,
    INTERRUPT_PWM1           = 48,
    INTERRUPT_PWM2           = 49,
    INTERRUPT_PWM3           = 50,
    INTERRUPT_PWM4           = 51,
    INTERRUPT_PWM5           = 52,
    INTERRUPT_PWM6           = 53,
    INTERRUPT_CAPTURE1       = 56,
    INTERRUPT_CAPTURE2       = 57,
    INTERRUPT_CAPTURE3       = 58,
    INTERRUPT_CAPTURE4       = 59,
    INTERRUPT_CAPTURE5       = 60,  // nicht beim 28xx
    INTERRUPT_CAPTURE6       = 61,  // dito
    INTERRUPT_QENCODER1      = 64,
    INTERRUPT_QENCODER2      = 65,    
    INTERRUPT_SPI1_RX        = 72,
    INTERRUPT_SPI1_TX        = 73,
    INTERRUPT_SPI2_RX        = 74,  // 2823x: McBSP_RX
    INTERRUPT_SPI2_TX        = 75,  // 2823x: McBSP_TX
    INTERRUPT_SPI3_RX        = 76,  // 2823x: McBSP_RX
    INTERRUPT_SPI3_TX        = 77,  // 2823x: McBSP_TX
    INTERRUPT_SPI4_RX        = 78,  // nur 280x
    INTERRUPT_SPI4_TX        = 79,  // dito                 
    INTERRUPT_DMA1           = 80,  // nicht beim 28xx
    INTERRUPT_DMA2           = 81,  // dito
    INTERRUPT_DMA3           = 82,  // dito
    INTERRUPT_DMA4           = 83,  // dito
    INTERRUPT_DMA5           = 84,  // dito
    INTERRUPT_DMA6           = 85,  // dito
    INTERRUPT_I2C1           = 88,
    INTERRUPT_I2C2           = 89,
    INTERRUPT_SCI3_RX        = 92,  // nicht beim 28xx
    INTERRUPT_SCI3_TX        = 93,  // dito 
    INTERRUPT_SCI1_RX        = 96,
    INTERRUPT_SCI1_TX        = 97,
    INTERRUPT_SCI2_RX        = 98,
    INTERRUPT_SCI2_TX        = 99,
    INTERRUPT_CAN1_RX        = 100,
    INTERRUPT_CAN1_TX        = 101,
    INTERRUPT_CAN2_RX        = 102,
    INTERRUPT_CAN2_TX        = 103,    
    INTERRUPT_EXTERN3        = 120,  // nicht beim 28xx     
    INTERRUPT_EXTERN4        = 121,  // dito   
    INTERRUPT_EXTERN5        = 122,  // dito  
    INTERRUPT_EXTERN6        = 123,  // dito  
    INTERRUPT_EXTERN7        = 124,  // dito      
    NUMBER_OF_INTERRUPTS     // must be last entry in list
} INTERRUPT_ID_t;

//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

typedef void (*HAL_INTERRUPT_HANDLER_t) (INTERRUPT_ID_t InterruptID);

#define halGlobalInterruptEnable()     asm(" clrc INTM")
#define halGlobalInterruptDisable()    asm(" setc INTM")

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

ERROR_t halEnableInterrupt   (INTERRUPT_ID_t InterruptID);
ERROR_t halDisableInterrupt  (INTERRUPT_ID_t InterruptID);
ERROR_t halRegisterInterrupt (
   INTERRUPT_ID_t InterruptID, HAL_INTERRUPT_HANDLER_t InterruptHandler);

ERROR_t halInitializeInterrupts (void);

//****************************************************************************/

#endif /*HAL_INTERRUPTS_H*/

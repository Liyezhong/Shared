/****************************************************************************/
/*! \file halVectors.c
 *
 *  \brief  Interrupt Vector Table
 *
 *  $Version: $ 0.2
 *  $Date:    $ 11.02.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains the interrupt vector table of the slave software
 *      and interrupt stubs for all used interrupts. The purpose of the stubs
 *      is to forward an interrupt specific device index to the real handler
 *      function residing in the individual device modules. This allows to
 *      use common handlers for all devices af a class.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include "Global.h"
#include "halCommon.h"
#include "halVectors.h"

#pragma O3      // maximal optimization
#pragma Otime   // maximal optimization


//****************************************************************************/
// External Variables
//****************************************************************************/

//! Dynamic vectors for shared interrupts (defined in halInterrupt)
extern UInt32 halDynamicVectors[6]; 


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

interrupt void halInterruptUnusedHandler (void);
interrupt void halSysTickInterrupt (void);

void halExceptionHandler (UInt32 *StackFrame);


/*****************************************************************************/
/*!
 *  \brief   Exception handler stub
 *
 *      This is the handler for all exceptions. It calls the real exception
 *      handler and passes the exception stack pointer to it.
 *
 *      Attention: If changing this function, don't use local variables,
 *      because that could change the stack pointer. The stack wouldn't
 *      point to the original exception stack anymore.
 *
 ****************************************************************************/

static interrupt void halInterruptException (void) {

    halExceptionHandler(((UInt32*)__current_sp()));      //lint !e26
}


/*@{*************************************************************************/
/*!
 *  \brief   Interrupt handler for CAN controller
 *
 *      These are the interrupt handler stubs for all interrupts belonging
 *      to the 2 CAN controllers. They call the device specific handlers 
 *      located in the module halCan.c. The device index number is
 *      forwarded to the common handlers in the device module.
 *
 ****************************************************************************/

static interrupt void halInterruptHandlerCanRX1 (void) {
    halCanInterruptHandler(0, 1);
}
static interrupt void halInterruptHandlerCanTX1 (void) {
    halCanInterruptHandler(0, 2);
}
static interrupt void halInterruptHandlerCanER1 (void) {
    halCanInterruptHandler(0, 4);
}
static interrupt void halInterruptHandlerCanRX2 (void) {
    halCanInterruptHandler(1, 1);
}
static interrupt void halInterruptHandlerCanTX2 (void) {
    halCanInterruptHandler(1, 2);
}
static interrupt void halInterruptHandlerCanER2 (void) {
    halCanInterruptHandler(1, 4);
}
/*@} end of doxygen group */


/*@{*************************************************************************/
/*!
 *  \brief   Interrupt handler for SPI controller
 *
 *      These are the interrupt handler stubs for all interrupts belonging
 *      to the SPI controllers. They call the device specific handlers 
 *      located in halSpi.c. The device index number is forwarded to the 
 *      common handlers in the device's module.
 *
 ****************************************************************************/

static interrupt void halInterruptHandlerSpi1 (void) {
    halStepperInterruptHandler(0);
}
static interrupt void halInterruptHandlerSpi2 (void) {
    halStepperInterruptHandler(1);
}
static interrupt void halInterruptHandlerSpi3 (void) {
    halStepperInterruptHandler(2);
}
/*@} end of doxygen group */


/*@{*************************************************************************/
/*!
 *  \brief   Interrupt handler for I2C controller
 *
 *      These are the interrupt handler stubs for all interrupts belonging
 *      to the I2C controllers. They call the device specific handlers 
 *      located in halI2C.c. The device index number is forwarded to the 
 *      common handlers in the device's module.
 *
 ****************************************************************************/

static interrupt void halInterruptHandlerI2C1 (void) {
    halI2cInterruptHandler(0);
}
static interrupt void halInterruptHandlerI2C2 (void) {
    halI2cInterruptHandler(1);
}
/*@} end of doxygen group */


/*@{*************************************************************************/
/*!
 *  \brief   Interrupt handler for UART controller
 *
 *      These are the interrupt handler stubs for all interrupts belonging
 *      to the UART controllers. They call the device specific handlers 
 *      located in halSerial.c. The device index number is forwarded to  
 *      the common handlers in the device's module.
 *
 ****************************************************************************/

static interrupt void halInterruptHandlerUart1 (void) {
    halSerialInterruptHandler(0);
}
static interrupt void halInterruptHandlerUart2 (void) {
    halSerialInterruptHandler(1);
}
static interrupt void halInterruptHandlerUart3 (void) {
    halSerialInterruptHandler(2);
}
static interrupt void halInterruptHandlerUart4 (void) {
    halSerialInterruptHandler(3);
}
static interrupt void halInterruptHandlerUart5 (void) {
    halSerialInterruptHandler(4);
}
/*@} end of doxygen group */


/*@{*************************************************************************/
/*!
 *  \brief   Interrupt handler for DMA channel
 *
 *      These are the interrupt handler stubs for all interrupts belonging
 *      to the DMA controllers. They call the device specific handlers 
 *      located in halDma.c. The device index number is forwarded to the   
 *      common handlers in the device's module.
 *
 ****************************************************************************/

static interrupt void halInterruptHandlerDma1_1 (void) {
    halDmaInterruptHandler(0);
}
static interrupt void halInterruptHandlerDma1_2 (void) {
    halDmaInterruptHandler(1);
}
static interrupt void halInterruptHandlerDma1_3 (void) {
    halDmaInterruptHandler(2);
}
static interrupt void halInterruptHandlerDma1_4 (void) {
    halDmaInterruptHandler(3);
}
static interrupt void halInterruptHandlerDma1_5 (void) {
    halDmaInterruptHandler(4);
}
static interrupt void halInterruptHandlerDma1_6 (void) {
    halDmaInterruptHandler(5);
}
static interrupt void halInterruptHandlerDma1_7 (void) {
    halDmaInterruptHandler(6);
}
static interrupt void halInterruptHandlerDma2_1 (void) {
    halDmaInterruptHandler(7);
}
static interrupt void halInterruptHandlerDma2_2 (void) {
    halDmaInterruptHandler(8);
}
static interrupt void halInterruptHandlerDma2_3 (void) {
    halDmaInterruptHandler(9);
}
static interrupt void halInterruptHandlerDma2_4 (void) {
    halDmaInterruptHandler(10);
}
static interrupt void halInterruptHandlerDma2_5 (void) {
    halDmaInterruptHandler(11);
}
/*@} end of doxygen group */


/*@{*************************************************************************/
/*!
 *  \brief   Interrupt handler for external interrupt input
 *
 *      These are the interrupt handler stubs for all interrupts belonging
 *      to external interrupt inputs. They call the device specific handlers 
 *      located in halExti.c. The device index number is forwarded to the   
 *      common handlers in the device's module.
 *
 ****************************************************************************/

static interrupt void halInterruptHandlerEXTI0 (void) {
    halExtIntInterruptHandler(0);
}
static interrupt void halInterruptHandlerEXTI1 (void) {
    halExtIntInterruptHandler(1);
}
static interrupt void halInterruptHandlerEXTI2 (void) {
    halExtIntInterruptHandler(2);
}
static interrupt void halInterruptHandlerEXTI3 (void) {
    halExtIntInterruptHandler(3);
}
static interrupt void halInterruptHandlerEXTI4 (void) {
    halExtIntInterruptHandler(4);
}
static interrupt void halInterruptHandlerEXTI5 (void) {
    halExtIntInterruptHandler(5);
}
static interrupt void halInterruptHandlerEXTI10 (void) {
    halExtIntInterruptHandler(10);
}
/*@} end of doxygen group */


/*@{*************************************************************************/
/*!
 *  \brief   Interrupt handler for timer 
 *
 *      These are the interrupt handler stubs for all interrupts belonging
 *      to the timers. They call the device specific handlers located in 
 *      halTimer.c. The device index number is forwarded to the common  
 *      handlers in the device's module.
 *
 ****************************************************************************/

static interrupt void halInterruptHandlerTimer1 (void) {
    halTimerInterruptHandler(0);
}
static interrupt void halInterruptHandlerTimer2 (void) {
    halTimerInterruptHandler(1);
}
static interrupt void halInterruptHandlerTimer3 (void) {
    halTimerInterruptHandler(2);
}
static interrupt void halInterruptHandlerTimer4 (void) {
    halTimerInterruptHandler(3);
}
static interrupt void halInterruptHandlerTimer5 (void) {
    halTimerInterruptHandler(4);
}
static interrupt void halInterruptHandlerTimer6 (void) {
    halTimerInterruptHandler(5);
}
static interrupt void halInterruptHandlerTimer7 (void) {
    halTimerInterruptHandler(6);
}
static interrupt void halInterruptHandlerTimer8 (void) {
    halTimerInterruptHandler(7);
}
static interrupt void halInterruptHandlerTimer9 (void) {
    halTimerInterruptHandler(halDynamicVectors[0]);
}
static interrupt void halInterruptHandlerTimer10 (void) {
    halTimerInterruptHandler(halDynamicVectors[1]);
}
static interrupt void halInterruptHandlerTimer11 (void) {
    halTimerInterruptHandler(halDynamicVectors[2]);
}
static interrupt void halInterruptHandlerTimer12 (void) {
    halTimerInterruptHandler(halDynamicVectors[3]);
}
static interrupt void halInterruptHandlerTimer13 (void) {
    halTimerInterruptHandler(halDynamicVectors[4]);
}
static interrupt void halInterruptHandlerTimer14 (void) {
    halTimerInterruptHandler(halDynamicVectors[5]);
}
/*@} end of doxygen group */


/*@{*************************************************************************/
/*!
 *  \brief   Interrupt handler for analog 
 *
 *      These are the interrupt handler stubs for all interrupts belonging
 *      to the analog input. They call the device specific handlers located
 *      in halAnalog.c. The device index number is forwarded to the common  
 *      handlers in the device's module.
 *
 ****************************************************************************/

interrupt void halAnalogInterruptHandler (void);
/*@} end of doxygen group */


/*****************************************************************************/
/*!
 *  \brief   Interrupt vector table
 *
 *      This is the fixed interrupt vector table used by the NVIC peripheral.
 *      All interrupts are routed either to the individual stub functions in
 *      this module, to a central system exception handler, or to a "unused
 *      interrupt" vector.
 *
 *      This table replaces the original interrupt vector table atomatically
 *      activated after reset, which is part of the external boot loader.
 *
 *      Due to restrictions in the hardware the vector table must be aligned
 *      to 512 byte boundaries.
 *
 ****************************************************************************/

const halInterruptVector_t 
          halInterruptVectorTable[] __attribute__ ((aligned (512))) = {
    
    halInterruptException,        //!  0: Top of Stack
    halInterruptException,        //!  1: Reset Handler
    halInterruptException,        //!  2: NMI Handler
    halInterruptException,        //!  3: Hard Fault Handler            
    halInterruptException,        //!  4: MPU Fault Handler
    halInterruptException,        //!  5: Bus Fault Handler
    halInterruptException,        //!  6: Usage Fault Handler
    halInterruptException,        //!  7: Reserved
    halInterruptException,        //!  8: Reserved
    halInterruptException,        //!  9: Reserved
    halInterruptException,        //! 10: Reserved
    halInterruptException,        // 11: SVCall Handler
    halInterruptException,        // 12: Debug Monitor Handler
    halInterruptException,        // 13: Reserved
    halInterruptException,        // 14: PendSV Handler
    halSysTickInterrupt,          // 15: SysTick Handler
    halInterruptUnusedHandler,    // 16: Window Watchdog
    halInterruptUnusedHandler,    // 17: PVD through EXTI Line detect
    halInterruptUnusedHandler,    // 18: Tamper
    halInterruptUnusedHandler,    // 19: RTC
    halInterruptUnusedHandler,    // 20: Flash
    halInterruptUnusedHandler,    // 21: RCC
    halInterruptHandlerEXTI0,     // 22: EXTI Line 0
    halInterruptHandlerEXTI1,     // 23: EXTI Line 1
    halInterruptHandlerEXTI2,     // 24: EXTI Line 2
    halInterruptHandlerEXTI3,     // 25: EXTI Line 3
    halInterruptHandlerEXTI4,     // 26: EXTI Line 4
    halInterruptHandlerDma1_1,    // 27: DMA1 Channel 1
    halInterruptHandlerDma1_2,    // 28: DMA1 Channel 2
    halInterruptHandlerDma1_3,    // 29: DMA1 Channel 3
    halInterruptHandlerDma1_4,    // 30: DMA1 Channel 4
    halInterruptHandlerDma1_5,    // 31: DMA1 Channel 5
    halInterruptHandlerDma1_6,    // 32: DMA1 Channel 6
    halInterruptHandlerDma1_7,    // 33: DMA1 Channel 7
    halAnalogInterruptHandler,    // 34: ADC1 and ADC2
    halInterruptHandlerCanTX1,    // 35: CAN1 TX
    halInterruptHandlerCanRX1,    // 36: CAN1 RX0
    halInterruptHandlerCanRX1,    // 37: CAN1 RX1
    halInterruptHandlerCanER1,    // 38: CAN1 SCE
    halInterruptHandlerEXTI5,     // 39: EXTI Line 5..9
    halInterruptHandlerTimer9,    // 40: TIM9 /TIM15/TIM1_BRK
    halInterruptHandlerTimer10,   // 41: TIM10/TIM16/TIM1_UPD
    halInterruptHandlerTimer11,   // 42: TIM11/TIM17/TIM1_COM
    halInterruptHandlerTimer1,    // 43: TIM1_CC
    halInterruptHandlerTimer2,    // 44: TIM2
    halInterruptHandlerTimer3,    // 45: TIM3
    halInterruptHandlerTimer4,    // 46: TIM4
    halInterruptHandlerI2C1,      // 47: I2C1 Event
    halInterruptHandlerI2C1,      // 48: I2C1 Error
    halInterruptHandlerI2C2,      // 49: I2C2 Event
    halInterruptHandlerI2C2,      // 50: I2C2 Error
    halInterruptHandlerSpi1,      // 51: SPI1
    halInterruptHandlerSpi2,      // 52: SPI2
    halInterruptHandlerUart1,     // 53: USART1
    halInterruptHandlerUart2,     // 54: USART2
    halInterruptHandlerUart3,     // 55: USART3
    halInterruptHandlerEXTI10,    // 56: EXTI Line 10..15
    halInterruptUnusedHandler,    // 57: RTC Alarm 
    halInterruptUnusedHandler,    // 58: USB OTG FS Wakeup
    halInterruptHandlerTimer12,   // 59: TIM12/TIM8_BRK
    halInterruptHandlerTimer13,   // 60: TIM13/TIM8_UPD
    halInterruptHandlerTimer14,   // 61: TIM14/TIM8_COM
    halInterruptHandlerTimer8,    // 62: TIM8_CC
    halInterruptUnusedHandler,    // 63: ADC3
    halInterruptUnusedHandler,    // 64: FSMC
    halInterruptUnusedHandler,    // 65: SDIO
    halInterruptHandlerTimer5,    // 66: TIM5
    halInterruptHandlerSpi3,      // 67: SPI3
    halInterruptHandlerUart4,     // 68: UART4
    halInterruptHandlerUart5,     // 69: UART5
    halInterruptHandlerTimer6,    // 70: TIM6
    halInterruptHandlerTimer7,    // 71: TIM7
    halInterruptHandlerDma2_1,    // 72: DMA2 Channel 1
    halInterruptHandlerDma2_2,    // 73: DMA2 Channel 2
    halInterruptHandlerDma2_3,    // 74: DMA2 Channel 3
    halInterruptHandlerDma2_4,    // 75: DMA2 Channel 4
    halInterruptHandlerDma2_5,    // 76: DMA2 Channel 5
    halInterruptUnusedHandler,    // 77: Ethernet
    halInterruptUnusedHandler,    // 78: Ethernet Wakeup
    halInterruptHandlerCanTX2,    // 79: CAN2 TX
    halInterruptHandlerCanRX2,    // 80: CAN2 RX0
    halInterruptHandlerCanRX2,    // 81: CAN2 RX1
    halInterruptHandlerCanER2,    // 82: CAN2 SCE
    halInterruptUnusedHandler     // 83: USB OTG
};

//****************************************************************************/


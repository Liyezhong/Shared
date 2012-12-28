/****************************************************************************/
/*! \file halVectors.h
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
 *      and interrupt stubs for all used interrupts. 
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

#ifndef HAL_VECTORS_H
#define HAL_VECTORS_H

#define interrupt      __irq     //!< Make intrinsic compiler independent

//****************************************************************************/
//! Physical interrupt identifier (Hardware dependent: don't change)
//****************************************************************************/

typedef enum {
    INTERRUPTS_GLOBAL    = 0,    //!< Interrupt ID for cpu interrupt mask
    RESET                = 1,    //!< Reset entry address
    EXCEPTION_NMI        = 2,    //!< Non-maskable interrupt
    EXCEPTION_HARDFAULT  = 3,    //!< Hard fault exception
    EXCEPTION_MEMORY     = 4,    //!< Memory fault exception
    EXCEPTION_BUS_ERROR  = 5,    //!< Bus error exception
    EXCEPTION_USE_ERROR  = 6,    //!< Usage error exception
    EXCEPTION_SYS_CALL   = 11,   //!< System call trap
    EXCEPTION_DEBUG      = 12,   //!< Debug interrupt
    EXCEPTION_SERVICE    = 14,   //!< Service call trap
    EXCEPTION_SYSTICK    = 15,   //!< System tick timer interrupt

    INTERRUPT_WATCHDOG   = 16,   //!< Watchdog interrupt
    INTERRUPT_VOLTAGE    = 17,   //!< Voltage monitor interrupt
    INTERRUPT_TAMPER     = 18,   //!< Tamper-pin interrupt
    INTERRUPT_RTC        = 19,   //!< Realtime clock interrupt
    INTERRUPT_RTC_ALARM  = 57,   //!< Realtime clock alarm interrupt
    INTERRUPT_FLASH      = 20,   //!< Flash programming interrupt
    INTERRUPT_RCC        = 21,   //!< Clock control interrupt
    INTERRUPT_EXTERN0    = 22,   //!< External input 0 interrupt
    INTERRUPT_EXTERN1    = 23,   //!< External input 1 interrupt
    INTERRUPT_EXTERN2    = 24,   //!< External input 2 interrupt
    INTERRUPT_EXTERN3    = 25,   //!< External input 3 interrupt
    INTERRUPT_EXTERN4    = 26,   //!< External input 4 interrupt
    INTERRUPT_EXTERNx    = 39,   //!< External inputs 5-9 interrupt
    INTERRUPT_EXTERNy    = 56,   //!< External inputs 10-15 interrupt

    INTERRUPT_DMA1_CH1   = 27,   //!< DMAC 1, channel 1 interrupt
    INTERRUPT_DMA1_CH2   = 28,   //!< DMAC 1, channel 2 interrupt
    INTERRUPT_DMA1_CH3   = 29,   //!< DMAC 1, channel 3 interrupt
    INTERRUPT_DMA1_CH4   = 30,   //!< DMAC 1, channel 4 interrupt
    INTERRUPT_DMA1_CH5   = 31,   //!< DMAC 1, channel 5 interrupt
    INTERRUPT_DMA1_CH6   = 32,   //!< DMAC 1, channel 6 interrupt
    INTERRUPT_DMA1_CH7   = 33,   //!< DMAC 1, channel 7 interrupt
    INTERRUPT_DMA2_CH1   = 72,   //!< DMAC 2, channel 1 interrupt
    INTERRUPT_DMA2_CH2   = 73,   //!< DMAC 2, channel 2 interrupt
    INTERRUPT_DMA2_CH3   = 74,   //!< DMAC 2, channel 3 interrupt
    INTERRUPT_DMA2_CH4_5 = 75,   //!< DMAC 2, channel 4+5 interrupt

    INTERRUPT_CAN1_TX    = 35,   //!< CAN 1 transmit interrupt
    INTERRUPT_CAN1_RX1   = 36,   //!< CAN 1 receive interrupt Fifo 1
    INTERRUPT_CAN1_RX2   = 37,   //!< CAN 1 receive interrupt Fifo 2
    INTERRUPT_CAN1_SCE   = 38,   //!< CAN 1 status/error interrupt
    INTERRUPT_CAN2_TX    = 70,   //!< CAN 2 transmit interrupt
    INTERRUPT_CAN2_RX1   = 71,   //!< CAN 2 receive interrupt Fifo 1
    INTERRUPT_CAN2_RX2   = 72,   //!< CAN 2 receive interrupt Fifo 2
    INTERRUPT_CAN2_SCE   = 73,   //!< CAN 2 status/error interrupt

    INTERRUPT_TIMER1     = 43,   //!< Timer 1 interrupt
    INTERRUPT_TIMER2     = 44,   //!< Timer 2 interrupt
    INTERRUPT_TIMER3     = 45,   //!< Timer 3 interrupt
    INTERRUPT_TIMER4     = 46,   //!< Timer 4 interrupt
    INTERRUPT_TIMER5     = 66,   //!< Timer 5 interrupt
    INTERRUPT_TIMER6     = 70,   //!< Timer 6 interrupt
    INTERRUPT_TIMER7     = 71,   //!< Timer 7 interrupt
    INTERRUPT_TIMER8     = 62,   //!< Timer 8 interrupt
    INTERRUPT_TIMER9     = 40,   //!< Timer 9 interrupt
    INTERRUPT_TIMER10    = 41,   //!< Timer 10 interrupt
    INTERRUPT_TIMER11    = 42,   //!< Timer 11 interrupt
    INTERRUPT_TIMER12    = 59,   //!< Timer 12 interrupt
    INTERRUPT_TIMER13    = 60,   //!< Timer 13 interrupt
    INTERRUPT_TIMER14    = 61,   //!< Timer 14 interrupt

    INTERRUPT_I2C1_EVENT = 47,   //!< I2C 1 event interrupt
    INTERRUPT_I2C1_ERROR = 48,   //!< I2C 1 error interrupt
    INTERRUPT_I2C2_EVENT = 49,   //!< I2C 2 event interrupt
    INTERRUPT_I2C2_ERROR = 50,   //!< I2C 2 error interrupt
    INTERRUPT_SPI1       = 51,   //!< SPI 1 interrupt
    INTERRUPT_SPI2       = 52,   //!< SPI 2 interrupt
    INTERRUPT_SPI3       = 67,   //!< SPI 3 interrupt
    INTERRUPT_UART1      = 53,   //!< UART 1 interrupt
    INTERRUPT_UART2      = 54,   //!< UART 2 interrupt
    INTERRUPT_UART3      = 55,   //!< UART 3 interrupt
    INTERRUPT_UART4      = 68,   //!< UART 4 interrupt
    INTERRUPT_UART5      = 69,   //!< UART 5 interrupt
    INTERRUPT_USB        = 58,   //!< USB controller interrupt

    INTERRUPT_ADC1_2     = 34,   //!< A/D converter 1+2 interrupt
    INTERRUPT_ADC3       = 63,   //!< A/D converter 3 interrupt
    INTERRUPT_FSMC       = 64,   //!< FSMC interrupt
    INTERRUPT_SDIO       = 65,   //!< SDIO interrupt

    NUMBER_OF_INTERRUPTS         //!< Must be last entry in list
} InterruptID_t;


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Type of hardware interrupt handler functions
typedef interrupt void (*halInterruptVector_t)(void);

//! Hardware interrupt vector table
extern const halInterruptVector_t halInterruptVectorTable[];


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/
             
void halCanInterruptHandler     (UInt32 DeviceNo, UInt32 IntrFlags);
void halI2cInterruptHandler     (UInt32 DeviceNo);
void halSerialInterruptHandler  (UInt32 DeviceNo);
void halTimerInterruptHandler   (UInt32 DeviceNo);
void halDmaInterruptHandler     (UInt32 DeviceNo);
void halStepperInterruptHandler (UInt32 DeviceNo);
void halExtIntInterruptHandler  (UInt32 ExtIntNo);

//****************************************************************************/

#endif /*HAL_VECTORS_H*/

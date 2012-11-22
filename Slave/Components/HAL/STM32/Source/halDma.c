/****************************************************************************/
/*! \file halDma.c
 *
 *  \brief  Direct Memory Access Controller Functions
 *
 *  $Version: $ 0.3
 *  $Date:    $ 05.05.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to control the processor internal
 *      direct memory access (DMA) controller.
 *
 *      These functions are for HAL internal usage only and are not intended
 *      to be used from outside of the HAL. It supports DMA controller 1 
 *      actually only. Higher density members of the STM32 family also
 *      contain a 2nd controller.
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
#include "halError.h"
#include "halInterrupt.h"
#include "halDma.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define MAX_CHANNEL_COUNT   7        //!< Max number of supported channels

#define DMA_CCR_EN          0x0001   //!< Channel enable
#define DMA_CCR_TCIE        0x0002   //!< Transfer complete interrupt enable
#define DMA_CCR_HTIE        0x0004   //!< Half transfer interrupt enable
#define DMA_CCR_TEIE        0x0008   //!< Transfer error interrupt enable

#define DMA_CCR_DIR         0x0010   //!< Data transfer direction
#define DMA_CCR_CIRC        0x0020   //!< Circular mode
#define DMA_CCR_PINC        0x0040   //!< Peripheral increment mode
#define DMA_CCR_MINC        0x0080   //!< Memory increment mode

#define DMA_CCR_PSIZE       0x0300   //!< Peripheral size
#define DMA_CCR_PSIZE8      0x0000   //!< Peripheral size = 8-bits
#define DMA_CCR_PSIZE16     0x0100   //!< Peripheral size = 16-bits
#define DMA_CCR_PSIZE32     0x0200   //!< Peripheral size = 32-bits

#define DMA_CCR_MSIZE       0x0C00   //!< Memory size
#define DMA_CCR_MSIZE8      0x0000   //!< Memory size = 8-bits
#define DMA_CCR_MSIZE16     0x0400   //!< Memory size = 16-bits
#define DMA_CCR_MSIZE32     0x0800   //!< Memory size = 32-bits

#define DMA_CCR_PL          0x3000   //!< Channel priority level
#define DMA_CCR_PL_LOW      0x0000   //!< Channel priority = Low
#define DMA_CCR_PL_MEDIUM   0x1000   //!< Channel priority = Medium
#define DMA_CCR_PL_HIGH     0x2000   //!< Channel priority = High
#define DMA_CCR_PL_HIGHEST  0x3000   //!< Channel priority = Very High

#define DMA_ISR_FLAG_GRP    0x000F   //!< Flag group for one channel
#define DMA_ISR_GIF         0x0001   //!< Global interrupt flag
#define DMA_ISR_TCIF        0x0002   //!< Transfer complete flag
#define DMA_ISR_HTIF        0x0004   //!< Half transfer flag
#define DMA_ISR_TEIF        0x0008   //!< Transfer error flag


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Register layout of processor internal DMA controller
typedef volatile struct {
    UInt32 ISR;             //!< DMA interrupt status register
    UInt32 IFCR;            //!< DMA interrupt flag clear register
    //! DMA channel regs
    struct Channel_t {         
        UInt32 CCR;         //!< DMA configuration register
        UInt32 CNDTR;       //!< DMA number of data tranfers register
        UInt32 CPAR;        //!< DMA peripheral address register
        UInt32 CMAR;        //!< DMA memory address register
        UInt32 RESERVED;    //!< Reserved
    } CHANNEL[7];           //!< Configuration for CH0...CH7
} DmaRegFile_t;

//! Register layout of a single DMA channel
typedef volatile struct Channel_t DmaChannel_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Hardware registers of the direct memory access controller
static DmaRegFile_t *DMA = (DmaRegFile_t*) 0x40020000;

//! Interrupt vector table for all DMA channels
static InterruptVector_t DmacIntrVectors[MAX_CHANNEL_COUNT];

//! Open channels bit mask (one bit per channel)
static UInt32 OpenBitMask = 0;


/*****************************************************************************/
/*!
 *  \brief   Opens a DMA channel
 *
 *      Opens a DMA channel and returns a handle to be used to control it.
 *      Handler must be a pointer to an interrupt handler function to be 
 *      called when the device signals an interrupt. It can be NULL if no
 *      interrupts are used. UserTag is a user defined number to be passed 
 *      to the handler function, e.g. as index into a data table in case 
 *      of multiple instances. 
 *
 *  \iparam  Channel = Channel number [0...6]
 *  \iparam  UserTag = User tag to pass to handler function
 *  \iparam  Handler = Interrupt handler function
 *
 *  \return  Channel number or (negative) error code
 *
 ****************************************************************************/

Error_t halDmaOpen (UInt16 Channel, UInt32 UserTag, halIntrHandler_t Handler) {

    const UInt8 InterruptID = INTERRUPT_DMA1_CH1 + Channel;

    if (Channel < ELEMENTS(DMA->CHANNEL)) {
        if (OpenBitMask & BIT(Channel)) {
            return (E_DEVICE_ALREADY_OPEN);            
        }
        DmacIntrVectors[Channel].Handler = Handler;
        DmacIntrVectors[Channel].UserTag = UserTag;

        if (Handler != NULL) {
            halInterruptEnable (InterruptID, IRQ_PRIO_DEFAULT);
        }
        OpenBitMask |= BIT(Channel);
        return (Channel);
    }
    return (E_DEVICE_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Close DMA channel
 *
 *      Closes an open DMA channel. Running transmissions are stopped and
 *      the channel is disabled.
 *
 *  \iparam  Channel = DMA channel number [0...6]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halDmaClose (UInt16 Channel) {

    const UInt8 InterruptID = INTERRUPT_DMA1_CH1 + Channel;

    if (Channel < ELEMENTS(DMA->CHANNEL)) {
        if (~OpenBitMask & BIT(Channel)) {
            return (E_DEVICE_NOT_OPEN);            
        }
        DMA->CHANNEL[Channel].CCR = 0;
        halInterruptDisable (InterruptID);

        OpenBitMask &= ~BIT(Channel);
        return (NO_ERROR);
    }
    return (E_DEVICE_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Setup DMA channel
 *
 *      Writes the peripheral address parameters for a DMA block transfer
 *      into the DMA controller's channel registers. The selected channel
 *      must not be busy when calling this function, else the running
 *      transfer will be aborted.
 *
 *  \iparam  Channel = DMA channel number [0...6]
 *  \iparam  Address = Address of peripheral register
 *  \iparam  Mode    = Mode bits
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halDmaSetup (UInt16 Channel, volatile void *Address, UInt16 Mode) {

    const UInt16 Bitmask = 
        DMA_CCR_PSIZE | DMA_CCR_MSIZE | DMA_CCR_CIRC | DMA_CCR_MINC;

    if (Channel < ELEMENTS(DMA->CHANNEL)) {

        DmaChannel_t *DmaChannel = &DMA->CHANNEL[Channel];

        if (Mode & ~Bitmask) {
            return (E_DMA_INVALID_MODE);
        }
        DmaChannel->CCR &= ~DMA_CCR_EN;
        DmaChannel->CCR &= ~Bitmask;
        DmaChannel->CCR |=  Bitmask & Mode;
        DmaChannel->CPAR = (UInt32) Address;

        return (NO_ERROR);
    }
    return (E_DEVICE_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Start a DMA read transfer (peripheral to memory)
 *
 *      Starts a DMA data transfer of 'Count' data items from the peripheral
 *      to the memory location pointed to by Address. The DMA channel must
 *      have been configured previously. Channel selects which DMA channel
 *      to use. If the DMA channel is busy or the channel number is invalid
 *      an error is returned.
 *
 *  \iparam  Channel = DMA channel number [0...6]
 *  \iparam  Address = Memory address to move data to
 *  \iparam  Count   = Number of data items to transfer
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halDmaRead (UInt16 Channel, volatile void *Address, UInt16 Count) {

    if (Channel < ELEMENTS(DMA->CHANNEL)) {
        DmaChannel_t *DmaChannel = &DMA->CHANNEL[Channel];

        if (DmaChannel->CNDTR > 0) {
            return (E_DEVICE_BUSY);
        }
        DmaChannel->CCR &= ~(DMA_CCR_DIR | DMA_CCR_EN);
        if (Address != NULL) {
            DmaChannel->CMAR = (UInt32) Address;
        }
        DmaChannel->CNDTR = Count;
        DmaChannel->CCR  |= DMA_CCR_EN;

        return (NO_ERROR);
    }
    return (E_DEVICE_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Start a DMA write transfer (memory to peripheral)
 *
 *      Starts a DMA data transfer of 'Count' data items from the memory 
 *      location pointed to by Address to the configured peripheral. The 
 *      DMA channel must have been configured before using this function. 
 *      Channel selects which DMA channel to use. If the DMA channel is 
 *      busy or the channel number is invalid an error is returned.
 *
 *  \iparam  Channel = DMA channel number [0...6]
 *  \iparam  Address = Memory address to move data to
 *  \iparam  Count   = Number of data items to transfer
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halDmaWrite (UInt16 Channel, volatile void *Address, UInt16 Count) {

    if (Channel < ELEMENTS(DMA->CHANNEL)) {
        DmaChannel_t *DmaChannel = &DMA->CHANNEL[Channel];

        if (DmaChannel->CNDTR > 0) {
            return (E_DEVICE_BUSY);
        }
        DmaChannel->CCR &= ~(DMA_CCR_DIR | DMA_CCR_EN);
        if (Address != NULL) {
            DmaChannel->CMAR = (UInt32) Address;
        }
        DmaChannel->CNDTR = Count;
        DmaChannel->CCR  |= (DMA_CCR_DIR | DMA_CCR_EN);

        return (NO_ERROR);
    }
    return (E_DEVICE_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Cancel a DMA transfer
 *
 *      Stops a running DMA transfer and returns the number of untransfered
 *      data items. If no transfer is in progress, 0 is returned.
 *
 *  \iparam  Channel = DMA channel number [0...6]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halDmaCancel (UInt16 Channel) {

    if (Channel < ELEMENTS(DMA->CHANNEL)) {

        DMA->CHANNEL[Channel].CCR &= ~DMA_CCR_EN;
        return (DMA->CHANNEL[Channel].CNDTR);
    }
    return (E_DEVICE_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Get number of data items to transfer
 *
 *      Returns the number of data items still to transfer. If the DMA
 *      channel isn't busy, 0 is returned.
 *
 *  \iparam  Channel = DMA channel number [0...6]
 *
 *  \return  Number of data items to transfer or (negative) error code
 *
 ****************************************************************************/

Error_t halDmaCount (UInt16 Channel) {

    if (Channel < ELEMENTS(DMA->CHANNEL)) {
        if (DMA->CHANNEL[Channel].CCR & DMA_CCR_EN) {
            return (DMA->CHANNEL[Channel].CNDTR);
        }
        return (0);
    }
    return (E_DEVICE_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Set priority of a DMA channel
 *
 *      Sets the priority of a DMA channel. Higher values represent higher
 *      priorities.
 *
 *  \iparam  Channel  = DMA channel number [0...6]
 *  \iparam  Priority = DMA priority [0...3]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halDmaSetPriority (UInt16 Channel, UInt8 Priority) {

    static const UInt32 PriorityTable[] = {
        DMA_CCR_PL_LOW, DMA_CCR_PL_MEDIUM, DMA_CCR_PL_HIGH, DMA_CCR_PL_HIGHEST };

    if (Channel < ELEMENTS(DMA->CHANNEL)) {
        if (Priority >= ELEMENTS(PriorityTable)) {
            return (E_DMA_INVALID_PRIORITY);
        }
        DMA->CHANNEL[Channel].CCR ^=
            (DMA->CHANNEL[Channel].CCR ^ PriorityTable[Priority]) & DMA_CCR_PL;

        return (NO_ERROR);
    }
    return (E_DEVICE_NOT_EXISTS);
}


/*****************************************************************************/
/*!
 *  \brief   Enable/disable DMA interrupts
 *
 *      Enables or disables the DMA interrupts specified with Flags. 
 *      Depending on State the selected interrupts are enabled (TRUE) or
 *      disabled (FALSE). The following interrupts are available:
 *
 *      - Terminal count (transfer finished)
 *      - Half transfer (half of the bytes are transfered)
 *      - Transfer error
 *
 *      Multiple interrupts can be controlled with a single call of this 
 *      function by simply OR-ing the interrupt flag bits.
 *
 *  \iparam  Channel = DMA channel number [0...6]
 *  \iparam  Flags   = DMA interrupt selection bits
 *  \iparam  State   = Enable (TRUE), or disable (FALSE)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halDmaControl (UInt16 Channel, UInt16 Flags, Bool State) {

    Flags &= DMA_CCR_TCIE + DMA_CCR_HTIE + DMA_CCR_TEIE;

    if (Channel < ELEMENTS(DMA->CHANNEL)) {
        if (State) {
            DMA->CHANNEL[Channel].CCR |= Flags;
        }
        else {
            DMA->CHANNEL[Channel].CCR &= ~Flags;
        }
        return (NO_ERROR);
    }
    return (E_DEVICE_NOT_EXISTS);
}

 
/*****************************************************************************/
/*!
 *  \brief   DMA interrupt handler
 *
 *      This function will be called by the vectored interrupt controller in
 *      case of a DMA interrupt. Channel is the number of the DMA channel.
 *      It calls the user handler registered with halDmaRegisterHandler(),
 *      passing the interrupt flag bits and the user tag to it. Before
 *      exiting, the handled interrupt flag bits are cleared.
 *
 *  \iparam  Channel = DMA channel number [0...6]
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void halDmaInterruptHandler (UInt32 Channel) {

   if (Channel < ELEMENTS(DMA->CHANNEL)) {
        const UInt32 IntrFlags = (DMA->ISR >> (Channel * 4)) & DMA_ISR_FLAG_GRP;
    
        if (DmacIntrVectors[Channel].Handler != NULL) {
            DmacIntrVectors[Channel].Handler (DmacIntrVectors[Channel].UserTag, IntrFlags);
        }
        DMA->IFCR = IntrFlags << (Channel * 4);
    }
}

//****************************************************************************/


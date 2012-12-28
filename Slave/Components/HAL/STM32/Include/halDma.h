/****************************************************************************/
/*! \file halDma.h
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
 *      These function are for HAL internal usage only and are not intended
 *      to be used from outside of the HAL. It supports DMA controller 1 
 *      actually only.
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

#ifndef HAL_DMA_H
#define HAL_DMA_H

#include "halInterrupt.h"


//****************************************************************************/
// Public Constants
//****************************************************************************/

// DMA Data mode defines
#define DMA_MODE_INCREMENT      0x0080   //!< Auto increment mode
#define DMA_MODE_CIRCULAR       0x0020   //!< Circular mode

// DMA Data transfer item bit width (peripheral device)
#define DMA_DEV_WIDTH8          0x0000   //!< Peripheral size 8 bits
#define DMA_DEV_WIDTH16         0x0100   //!< Peripheral size 16 bits
#define DMA_DEV_WIDTH32         0x0200   //!< Peripheral size 32 bits

// DMA Data transfer item bit width (memory)
#define DMA_MEM_WIDTH8          0x0000   //!< Memory size 8 bits
#define DMA_MEM_WIDTH16         0x0400   //!< Memory size 16 bits
#define DMA_MEM_WIDTH32         0x0800   //!< Memory size 32 bits

// DMA interrupt enable/clear/flag defines
#define DMA_INTR_COMPLETE       0x0002  //!< Transfer complete interrupt
#define DMA_INTR_HALF_XFER      0x0004  //!< Half transfer interrupt
#define DMA_INTR_ERROR          0x0008  //!< Transfer error interrupt

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halDmaOpen    (UInt16 Channel, UInt32 UserTag, halIntrHandler_t Handler);
Error_t halDmaClose   (UInt16 Channel);
Error_t halDmaSetup   (UInt16 Channel, volatile void *Address, UInt16 Mode);
Error_t halDmaRead    (UInt16 Channel, volatile void *Address, UInt16 Count);
Error_t halDmaWrite   (UInt16 Channel, volatile void *Address, UInt16 Count);
Error_t halDmaControl (UInt16 Channel, UInt16 Flags, Bool State);
Error_t halDmaCancel  (UInt16 Channel);
Error_t halDmaCount   (UInt16 Channel);

//****************************************************************************/

#endif /*HAL_DMA_H*/

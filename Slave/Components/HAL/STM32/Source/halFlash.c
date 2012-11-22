/****************************************************************************/
/*! \file halFlash.c
 *
 *  \brief  Flash memory control functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 14.04.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to control the processor internal
 *      flash memory. This includes writing/erasing the flash memory.
 *
 *      The functions implemented in this module are entended to be used
 *      HAL internal only. Use the Storage functions to access the flash
 *      and other memory types from outside the HAL.
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
#include "halClocks.h"
#include "halMain.h"
#include "halSysTick.h"
#include "halWatchdog.h"
#include "halUtilities.h"
#include "halFlash.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define FLASH_BASEADDRESS   0x08000000   //!< Flash memory base address

#define FLASH_UNLOCK_KEY1   0x45670123   //!< Flash unlock key 1
#define FLASH_UNLOCK_KEY2   0xCDEF89AB   //!< Flash unlock key 2

#define FLASH_ACR_PRFTBS    0x00000020   //!< Prefetch buffer status
#define FLASH_ACR_PRFTBE    0x00000010   //!< Prefetch buffer enable
#define FLASH_ACR_HLFCYA    0x00000008   //!< Flash half cycle access enable
#define FLASH_ACR_LATENCY   0x00000007   //!< Latency (wait states)
#define FLASH_ACR_WAIT_0    0x00000000   //!< No wait states
#define FLASH_ACR_WAIT_1    0x00000001   //!< 1 wait states
#define FLASH_ACR_WAIT_2    0x00000002   //!< 2 wait states

#define FLASH_CR_PG         0x00000001   //!< Programming command
#define FLASH_CR_PER        0x00000002   //!< Page erase command
#define FLASH_CR_MER        0x00000004   //!< Mass erase command
#define FLASH_CR_OPTPG      0x00000010   //!< Option byte programming 
#define FLASH_CR_OPTER      0x00000020   //!< Option byte erase command
#define FLASH_CR_STRT       0x00000040   //!< Start erase operation
#define FLASH_CR_LOCK       0x00000080   //!< Lock flash controller

#define FLASH_SR_BSY        0x00000001   //!< Operation in progress
#define FLASH_SR_EOP        0x00000020   //!< End of operation
#define FLASH_SR_PGERR      0x00000004   //!< Programming error flag
#define FLASH_SR_WRPRTERR   0x00000010   //!< Write protection error

#define FLASH_PROG_TIMEOUT  1000         //!< Flash write timeout [ms]

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Layout of processor internal flash control registers
typedef volatile struct {
    UInt32 ACR;         //!< Flash access control register
    UInt32 KEYR;        //!< Flash key register
    UInt32 OPTKEYR;     //!< Flash options key register
    UInt32 SR;          //!< Flash status register
    UInt32 CR;          //!< Flash control register
    UInt32 AR;          //!< Flash address register
    UInt32 RESERVED;    //!< Reserved
    UInt32 OBR;         //!< Option byte register
    UInt32 WRPR;        //!< Write protection register
} FlashRegFile_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Pointer to FLASH control registers
static FlashRegFile_t *FLASH = (FlashRegFile_t*) 0x40022000;

static UInt32 FlashMemorySize = 0;     //!< Size of internal FLASH memory
static UInt32 FlashBlockSize  = 0;     //!< FLASH block size

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halFlashWaitDone (void);


/*****************************************************************************/
/*!
 *  \brief   Returns the flash memory block size
 *
 *      This function returns the block size of the internal flash memory of
 *      the microcontroller.
 *
 *  \return  Block size of controllers internal flash memory
 *
 ****************************************************************************/

UInt32 halFlashBlockSize (void) {

    return (FlashBlockSize);
}


/*****************************************************************************/
/*!
 *  \brief   Read from flash memory
 *
 *      Reads a number of bytes from the Flash memory and copies it into
 *      Buffer. Address must be the Address to read from, relative to the
 *      start address of the Flash memory. If Address is greater than the
 *      Flash memory size or an attempt is made to read beyond the limit
 *      of the Flash memory, an error is returned. Buffer must be large
 *      enough to take 'Count' bytes.
 *
 *  \iparam  Address = Address to read from
 *  \xparam  Buffer  = Buffer to return the read data
 *  \iparam  Count   = Number of bytes to read
 *
 *  \return  Number of bytes read or (negative) error code
 *
 ****************************************************************************/

Error_t halFlashRead (UInt32 Address, void *Buffer, UInt32 Count) {

    UInt8 *Memory = (UInt8*) Address;
    UInt8 *Target = (UInt8*) Buffer;
    UInt32 i;

    if (Buffer != NULL || Count > 0) {
        if (Address >= FlashMemorySize) {
            return (E_STORAGE_ADDRESS);
        }
        if (Address + Count >= FlashMemorySize) {
            return (E_STORAGE_SIZE);
        }
        Memory += FLASH_BASEADDRESS;

        for (i=0; i < Count; i++) {
            Target[i] = Memory[i];
        }
        return (Count);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Write to flash memory
 *
 *      Writes a number of bytes to the Flash memory. Address must be the 
 *      Address to write to, relative to the start address of the Flash 
 *      memory. Buffer must point to the data to write, Count gives the
 *      number of byte to write.
 * 
 *      If Address is greater than the Flash memory size or an attempt is 
 *      made to write beyond the limits of the Flash memory, an error is 
 *      returned. If the Flash memory is write protected (the default)
 *      the operation is aborted and an error returned.
 *
 *      Limitations: Flash memory can be written in 16bit portions only.
 *      Address and Count parameters must be 16bit aligned therefore.
 *
 *  \iparam  Address = Address to write to
 *  \iparam  Buffer  = Pointer to data to write
 *  \iparam  Count   = Number of bytes to write
 *
 *  \return  Number of bytes written or (negative) error code
 *
 ****************************************************************************/

Error_t halFlashWrite (UInt32 Address, void *Buffer, UInt32 Count) {

    UInt16 *Target = (UInt16*) Address;
    UInt16 *Source = (UInt16*) Buffer;
    Error_t Status;
    UInt32 i;

    if (Buffer != NULL || Count > 0) {
        if (Address >= FlashMemorySize) {
            return (E_STORAGE_ADDRESS);
        }
        if (Address + Count > FlashMemorySize) {
            return (E_STORAGE_SIZE);
        }
        if ((Count & 1) || (Address & 1)) {
            return (E_FLASH_ALIGNMENT);
        }    
        if (FLASH->CR & FLASH_CR_LOCK) {
            return (E_FLASH_WRITE_PROTECTED);
        }
        Target += FLASH_BASEADDRESS/2;

        for (i=0; i < Count/2; i++) {

            FLASH->CR |= FLASH_CR_PG;
            Target[i] = Source[i];

            if ((Status = halFlashWaitDone()) != NO_ERROR) {
                return (Status);
            }                        
            if (Target[i] != Source[i]) {
                return (E_FLASH_VERIFY);
            }
        }
        return (Count);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Erase flash memory
 *
 *      Erases a number of pages in Flash memory. Address must be the 
 *      Address of the page to erase, relative to the starting address of 
 *      the Flash memory. Size must give the number of bytes to erase
 *      and must be a multiple of the hardware page size.
 
 *      If Address is greater than the Flash memory size or an attempt is 
 *      made to erase beyond the limits of the Flash memory, an error is 
 *      returned. If the Flash memory is write protected (the default)
 *      the operation is aborted and an error returned.
 *
 *  \iparam  Address = Address of page to erase
 *  \iparam  Count   = Number of bytes to erase
 *
 *  \return  Number of bytes erased or (negative) error code
 *
 ****************************************************************************/

Error_t halFlashErase (UInt32 Address, UInt32 Count) {

    UInt8 *Target = (UInt8*) Address;
    Error_t Status;
    UInt32 i, k;

    if (Count > 0) {
        if (Address >= FlashMemorySize) {
            return (E_STORAGE_ADDRESS);
        }
        if ((Count % FlashBlockSize) != 0) {
            Count = (Count / FlashBlockSize + 1) * FlashBlockSize;
        }
        if (Address + Count > FlashMemorySize) {
            return (E_STORAGE_SIZE);
        }
        if ((Address % FlashBlockSize) != 0) {
            return (E_FLASH_ALIGNMENT);
        }        
        if (FLASH->CR & FLASH_CR_LOCK) {
            return (E_FLASH_WRITE_PROTECTED);
        }
        Target += FLASH_BASEADDRESS;

        for (i=0; i < Count/FlashBlockSize; i++) {
                
            FLASH->CR |= FLASH_CR_PER;
            FLASH->AR  = (UInt32) Target;
            FLASH->CR |= FLASH_CR_STRT;

            if ((Status = halFlashWaitDone()) != NO_ERROR) {
                return (Status);
            }            
            // Verify erased page
            for (k=0; k < FlashBlockSize; k++) {
                if (Target[k] != 0xFF) {
              	    return (E_FLASH_ERASE);
                }	       
            }
            Target += FlashBlockSize;
        }
    }
    return (Count);
}


/*****************************************************************************/
/*!
 *  \brief   Set/remove write protection
 *
 *      Sets or removes the write protection of the FLASH memory. If State 
 *      is TRUE write protection is activated, if State is FALSE it is de-
 *      activated. The default state is write protection on.
 *
 *  \iparam  State = Write protection state (on, off)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halFlashProtect (Bool State) {

    // Return error if Flash still busy
    if (FLASH->SR & FLASH_SR_BSY) {
        return (E_FLASH_BUSY);
    }
    if (!State) {
        // Enable flash write/erase access
        if (FLASH->CR & FLASH_CR_LOCK) {
            FLASH->KEYR = FLASH_UNLOCK_KEY1;  
            FLASH->KEYR = FLASH_UNLOCK_KEY2;
        }
        if (FLASH->CR & FLASH_CR_LOCK) {
            return (E_FLASH_WRITE_PROTECTED);
        }
    }
    else {
        // Disable flash write/erase access
        FLASH->CR |= FLASH_CR_LOCK;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Wait until write/erase done
 *
 *      Waits until the last write or erase operation on the Flash is done.
 *      During waiting the watchdog is triggered periodically. If a maximal
 *      timeout is exhausted and the operation is still ongoing, an error is
 *      returned. The same applies if the Flash controller flags an error
 *      after finishing the operation.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halFlashWaitDone (void) {

    UInt32 StartTime = halSysTickRead();
    UInt32 ErrFlags;

    // Wait until program/erase operation done
    while (FLASH->SR & FLASH_SR_BSY) {
        if (halSysTickRead() - StartTime > FLASH_PROG_TIMEOUT) {
            return (E_FLASH_TIMEOUT);
        }
        halWatchdogTrigger();
    }
    halWatchdogTrigger(); // in case the above loop is not entered

    // Reset all FLASH command bits
    FLASH->CR &= 
        ~(FLASH_CR_PG | FLASH_CR_PER | FLASH_CR_MER | 
          FLASH_CR_OPTPG | FLASH_CR_OPTER);

    // Check for program/erase errors
    ErrFlags = FLASH->SR & (FLASH_SR_WRPRTERR | FLASH_SR_PGERR);
    if (ErrFlags) {
        FLASH->SR |= ErrFlags;

        if (ErrFlags & FLASH_SR_WRPRTERR) {
            return (E_FLASH_WRITE_PROTECTED);
        }
        if (ErrFlags & FLASH_SR_PGERR) {
            return (E_FLASH_WRITE);
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Setup FLASH memory wait states
 *
 *      Sets the optimal number of FLASH memory wait states possible for 
 *      the given ClockRate. If ClockRate is 0, the actual processor
 *      clock rate is used as a reference. The resulting number of
 *      wait states is returned.
 *
 *  \iparam  ClockRate = Processor core clock rate
 *
 *  \return  Number of wait states or (negative) error code
 *
 ****************************************************************************/

Error_t halFlashWaitStates (UInt32 ClockRate) {

    Int32 WaitStates;

    if (!ClockRate) {
        ClockRate = halGetProcessorClockRate();
    }
    if (ClockRate < 24000000) {
        WaitStates = 0;
    }
    else if (ClockRate < 48000000) {
        WaitStates = 1;
    }
    else {
        WaitStates = 2;
    }
    if (ClockRate < 8000000) {
        FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_HLFCYA;
    }
    else {
        FLASH->ACR = FLASH_ACR_PRFTBE | WaitStates;
    }
    return (WaitStates);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize FLASH module
 *
 *      Initializes this module by setting up the FLASH access wait time
 *      and the FLASH parameters.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halFlashInit (void) {

    if (halGetInitState() == INIT_IN_PROGRESS) {
        halFlashWaitStates (halGetProcessorClockRate());
    
        if (halProcessorInfo.DensityID == DEVICE_DENSITY_MEDIUM || 
            halProcessorInfo.DensityID == DEVICE_DENSITY_LOW) {
            FlashBlockSize = 1024;
        }
        else {
            FlashBlockSize = 2048;
        }
        FlashMemorySize = halGetMemorySize(MEMORY_SIZE_FLASH);
    }
    return (NO_ERROR);
}

//****************************************************************************/

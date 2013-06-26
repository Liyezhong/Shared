/****************************************************************************/
/*! \file halEeprom.c
 *
 *  \brief  Non-volatile memory access functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 09.05.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access a non-volatile serial
 *      FRAM memory (ferro-magnetic RAM) connected via I2C. FRAMs are
 *      compatible with serial EEPROMs on the hadware side. But other
 *      than EEPROMs FRAMs can be written without any write delay,
 *      while EEPROMs need some milliseconds to finish a write. 
 
 *      Since memory read and write operations are synchronous from the
 *      systems point of view, they must be served as fast as possible.
 *      The limited speed of the I2C bus is compensated by optionally
 *      caching the memory in RAM. The content of the non-volatile
 *      memory is read into a RAM mirror on startup and all reads are
 *      further satisfied by accessing the RAM only. Writes are routed
 *      to the RAM mirror, and additionally to non-volatile memory, 
 *      keeping both memory copies consistent. Since write operation 
 *      can be performed asynchronously, the slow access time via I2C 
 *      doesn't matters in this case.
 *
 *      When using larger size memories it might become necessary to
 *      change from mirroring to a less memory wasting algorithm like 
 *      real caching. Actually, memory sizes of 256..512 bytes are  
 *      planned to be used. The memory requirement for mirroring is 
 *      acceptable with these sizes. The current implementation 
 *      supports FRAM sizes up to 512 kByte. 
 *
 *      It would be possible to extend the functionality of this module
 *      to support also EEPROM devices. After write operations a wait
 *      time would be needed to archive this. This could be done by
 *      polling the device via I2C. The EEPROM would return negative
 *      acknowledges as long as the programming is in progress.
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

#include <stdlib.h>
#include "Global.h"
#include "halDeviceID.h"
#include "halCommon.h"
#include "halError.h"
#include "halMain.h"
#include "halPorts.h"
#include "halI2cBus.h"
#include "halEeprom.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define FRAM_SLAVE_ID  0xA    //!< I2C slave identifier for FRAMs

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Structure to hold the FRAM data mirror
typedef struct {
    UInt16 Size;         //!< Size of cache [bytes]
    UInt8 *Data;         //!< Cache data buffer
} FramCache_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

static FramCache_t Cache;  //!< Memory cache
static Handle_t PortWP;    //!< Device handle of write protection output
static Handle_t I2cBus;    //!< Device handle of I2C bus

static Error_t LastError;  //!< I2C transaction result

static Bool WriteProtected = FALSE;   //!< Software write protection


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halEepromI2cAddress (UInt32 Address, UInt32 Size);


/*****************************************************************************/
/*!
 *  \brief   Read from FRAM memory
 *
 *      Reads a number of bytes from the FRAM memory and returns it in
 *      Buffer. Address must be the Address to read from, relative to the
 *      start address of the FRAM memory. If Address is greater than the
 *      FRAM memory size or an attempt is made to read beyond the upper
 *      limit of the FRAM memory, an error is returned. Buffer must be
 *      large enough to hold 'Count' bytes.
 *
 *      To speed up operation the data is read from cache memory, provided
 *      that the requested data is in the cache and caching is enabled.  
 *      If data is not in the cache, it is read from FRAM. 
 *
 *  \iparam  Address = Address to read from
 *  \xparam  Buffer  = Buffer to return data
 *  \iparam  Count   = Number of bytes to read
 *
 *  \return  Number of bytes read or (negative) error code
 *
 ****************************************************************************/

Error_t halEepromRead (UInt32 Address, void *Buffer, UInt32 Count) {

    Error_t Status;
    UInt32 i;

    if (Buffer != NULL && Count > 0) {

        Int32 I2cAddress = halEepromI2cAddress(Address, Count); 

        if (I2cAddress < 0) {
            return (E_STORAGE_ADDRESS);
        }
        // If caching enabled, read data from cache
        if (Address + Count <= Cache.Size) {
            UInt8 *Target = (UInt8*) Buffer;

            for (i=0; i < Count; i++) {
                Target[i] = Cache.Data[Address+i];
            }
        }
        // Caching disabled: read data from eeprom
        else {
            Status = halI2cExchange(I2cBus, I2cAddress, Buffer, Count, TRUE);
            if (Status < 0) {
                return (Status);
            }
            // Wait until job is done
            if ((Status = halI2cWait(I2cBus, 0)) < 0) {
                return (Status);
            }
            Status = LastError;
            if (Status < 0) {
                LastError = NO_ERROR;
                return (Status);
            }
        }
        return (Count);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Write to FRAM memory
 *
 *      Writes Count bytes from Buffer to the FRAM memory. Address must be 
 *      the address to write to, relative to the start address of the FRAM.
 *      If Address is greater than the FRAM memory size or any attempt is 
 *      made to write beyond the upper limit of the FRAM memory, an error 
 *      is returned. 
 *
 *      To speed up operation, the data is optionally mirrored in a cache 
 *      memory. The write data is copied into the cache memory and also
 *      into the FRAM, to keep both data copies consistent (write thru
 *      strategy). The write is performed aynchronously in background 
 *      without waiting.
 *
 *      If caching is disabled, data is only written into FRAM. To ensure
 *      that the Buffer can be released after return to the caller, the
 *      function waits until the operation has finished in this case. 
 *
 *  \iparam  Address = Address to write to
 *  \iparam  Buffer  = Buffer containing data to write
 *  \iparam  Count   = Number of bytes to write
 *
 *  \return  Number of bytes written or (negative) error code
 *
 ****************************************************************************/

Error_t halEepromWrite (UInt32 Address, void *Buffer, UInt32 Count) {

    Error_t Status;
    UInt32 i;

    if (Buffer != NULL && Count > 0) {
        Int32 I2cAddress = halEepromI2cAddress(Address, Count); 

        if (I2cAddress < 0) {
            return (E_STORAGE_ADDRESS);
        }
        if (WriteProtected) {
            return (E_STORAGE_PROTECTED);
        }
        // If caching enabled, write data into the cache
        if (Address < Cache.Size) {
            const UInt8 *Source = (UInt8*) Buffer;

            for (i=0; i < Count; i++) {
                if (Address+i < Cache.Size) {
                    Cache.Data[Address+i] = Source[i];
                }
            }
            Status = halI2cExchange (
                I2cBus, I2cAddress, &Cache.Data[Address], Count, FALSE);

            if (Status < 0) {
                return (Status);
            }
        }
        // Caching disabled: write data directly to eeprom 
        else {
            Status = halI2cExchange(I2cBus, I2cAddress, Buffer, Count, FALSE);
            if (Status < 0) {
                return (Status);
            }
            if ((Status = halI2cWait(I2cBus, 0)) < 0) {
                return (Status);
            }
        }
        Status = LastError;
        if (Status < 0) {
            LastError = NO_ERROR;
            return (Status);
        }
        return (Count);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Erase FRAM memory
 *
 *      Erases Count bytes in FRAM memory starting with Address. If Address 
 *      is greater than the FRAM memory size or any attempt is made to write
 *      beyond the upper limit of the FRAM memory, an error is returned. 
 *
 *      Erase is done using the halEepromWrite function. See there to get
 *      more information.
 *
 *  \iparam  Address = Address of block to erase
 *  \iparam  Count   = Number of bytes to erase
 *
 *  \return  Number of bytes erased or (negative) error code
 *
 ****************************************************************************/

Error_t halEepromErase (UInt32 Address, UInt32 Count) {

    UInt8 Zeros[32] = {0};
    UInt32 SegSize;
    Error_t Status;
    
    if (Count) { 
        UInt32 Total = Count;
        do {       
            SegSize = MIN (sizeof(Zeros), Total);
            Status  = halEepromWrite (Address, Zeros, SegSize);
            if (Status < 0) {
                return (Status);
            }
            Address += SegSize; 
            Total -= SegSize;
        } while (Total);
    }
    return (Count);
}


/*****************************************************************************/
/*!
 *  \brief   Set/remove write protection
 *
 *      Sets or removes the write protection of the FRAM memory. If the
 *      hardware write protection is not connected in hardware, write
 *      protection is handled in software.
 *
 *      Write is global for the FRAM chip. If more than one logical
 *      memory is mapped to the same hardware chip, the protection
 *      is applied to all logical memories.
 *
 *  \iparam  State = Write protection state (on, off)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halEepromProtect (Bool State) {

    WriteProtected = State;    

    if (PortWP > 0) {
        return (halPortWrite (PortWP, State));
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   I2C transaction callback
 *
 *      This function is called by the I2C module, when a transaction
 *      (read or write) terminates. The I2C notifies if the transaction 
 *      has been successful (Status is NO_ERROR) or has failed (Status
 *      contains an error code). If more than one transactions are
 *      outstanding, the JobID can be used to identify a certain
 *      transaction.
 *
 *  \iparam  JobID  = Job identifier
 *  \iparam  Status = Job status (error code or NO_ERROR)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static void halEepromCallback (UInt16 JobID, Error_t Status) {

    if (LastError == NO_ERROR) {
        LastError = Status;
    }
}


/*****************************************************************************/
/*!
 *  \brief   Get I2C bus address
 *
 *      Checks Address and Size for validity and returns the device slave
 *      address as expected by the I2C bus interface. The I2C bus can deal
 *      with 7, 11, 19 or 27 bit addresses. The address is complemented by
 *      a 4 bit slave ID, which identifies the FRAM chip. The position of
 *      this slave ID depends on the address size.
 *
 *  \iparam  Address = Memory word address
 *  \iparam  Size    = Number of bytes to read/write
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halEepromI2cAddress (UInt32 Address, UInt32 Size) {

    // Check if address is valid
    if (Address >= halNonVolatileSize) {
        return (E_STORAGE_ADDRESS);
    }
    if (Address + Size > halNonVolatileSize) {
        return (E_STORAGE_SIZE);
    }
    // Address length = 11 Bit (max. 2 kByte)
    if (halNonVolatileSize < BIT(11)) {                 //lint !e506
        return (Address + (FRAM_SLAVE_ID << 11));
    }
    // Address length = 19 Bit (max. 512 kByte)
    if (halNonVolatileSize < BIT(19)) {                 //lint !e506
        return (Address + (FRAM_SLAVE_ID << 19));
    }
    // Address length = 27 Bit (more than enough)
    if (halNonVolatileSize < BIT(27)) {                 //lint !e506
        return (Address + (FRAM_SLAVE_ID << 27));
    }
    return (E_STORAGE_SIZE);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize FRAM module
 *
 *      Initializes this module by opening the I2C bus for communication
 *      with the FRAM, and open the hardware write protection output.
 *      If data caching is enabled, cache memory is allocated from the
 *      heap, and the FRAM content is read into that cache memory.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halEepromInit (void) {

    Error_t Status;
    Error_t JobID;
	//int i;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        // Open I2C bus for communication with the FRAM
        I2cBus = halI2cOpen (HAL_BUS_I2C_A, 0, halEepromCallback);
        if (I2cBus < 0) {
            return (I2cBus);
        }
        // Allocate cache memory and copy FRAM data into it
        if (halNonVolatileCache) {                                   //lint !e506
            const UInt32 CacheSize = halNonVolatileCache;
    
            Cache.Data = calloc (CacheSize, sizeof(UInt8));
            Cache.Size = 0;
    
            if (Cache.Data != NULL) {
                Int32 I2cAddress = halEepromI2cAddress(0, CacheSize); 
    
                JobID = halI2cExchange(
                    I2cBus, I2cAddress, Cache.Data, CacheSize, TRUE);
    
                if (JobID < 0) {
                    return (JobID);
                }
                if ((Status = halI2cWait(I2cBus, 0)) < 0) {
                    return (Status);
                }
                Cache.Size = CacheSize;
            }
        }
        // Open hardware write protection output
        PortWP = halPortOpen (HAL_FRAM_PROTECT, HAL_OPEN_WRITE);
    }    
    return (NO_ERROR);
}

//****************************************************************************/

/****************************************************************************/
/*! \file bmHal.c
 * 
 *  \brief Hardware abstraction layer simulation (HAL)
 *
 *  $Version: $ 0.1
 *  $Date:    $ 10.05.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains HAL (hardware abstraction layer) access function
 *       to be used to access the hardware. This is only a simulation of the
 *       HAL, which is usefull to debug higher layer software w/o hardware,
 *       using the code composer studio simulator. It will be replaced later
 *       by a real hardware abstraction layer. 
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

#ifdef HAL_SIMULATION 

#include <file.h>
#include <string.h>
#include <stdlib.h>
#include "Global.h"
#include "bmCommon.h"
#include "bmDebug.h"
#include "bmError.h"
#include "bmCAN.h"
#include "bmTime.h"
#include "bmUtilities.h"
#include "bmHal.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define STORAGE_SIZE                512
#define BLOCKSIZE                   8

#define VECTOR_RESET                (void**)0x000D00   // Reset vector 
#define VECTOR_BOOTLOADER_STARTUP   (void**)0x000D02   // use INT1 vector
#define VECTOR_BOOTLOADER_UPDATE    (void**)0x000D04   // use INT2 vector
#define VECTOR_BOOTLOADER_SIGNATURE (void**)0x000D06   // use INT3 vector
#define VECTOR_BOOTLOADER_INFO      (void**)0x000D08   // use INT4 vector
#define VECTOR_BOARD_OPTIONS        (void**)0x000D0A   // use INT5 vector
#define VECTOR_BOARD_INFO           (void**)0x000D0C   // use INT6 vector

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

typedef struct {
	UInt32 Counter;
	UInt32 Divider;
    UInt32 Resolution;
    UInt32 Period;
	Bool   Running;
	Bool   Opened;
} HAL_TIMER_t;

typedef struct {
    UInt16 Elements;
    UInt32 StartTime;
    UInt16 Value;
    Int16 Steps;
    Int16 Index;
    const INPUT_PORT_DATA_t* Data;
} INPUT_SIMULATION_t;

typedef void (*BOOTLOADER_VECTOR)(void);
   
//****************************************************************************/
// Private Variables 
//****************************************************************************/

static INPUT_SIMULATION_t InputPatterns[20] = {0};

static HAL_INTERRUPT_HANDLER canTransmitInterruptHandler = NULL;
static HAL_INTERRUPT_HANDLER canReceiveInterruptHandler  = NULL;

static Bool canReceiveInterruptEnable  = FALSE;
static Bool canTransmitInterruptEnable = FALSE;

static CAN_MESSAGE_t canInMessage = {0};
static HAL_TIMER_t Timers[3] = {0};

static UInt16 UnPacked[BLOCKSIZE];


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static ERROR_t halRegisterInputPattern (
    UInt16 Channel, const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize);
static Bool halGetInputPattern (UInt16 Channel, UInt16* Value);


//****************************************************************************/
// Implementation 
//****************************************************************************/

void *halGetAddress (ADDRESS_IDENTIFIER_t AddressID)
{
    switch (AddressID) {
        case ADDRESS_BOARD_OPTION_BLOCK:
            return (*VECTOR_BOARD_OPTIONS);

        case ADDRESS_BOARD_HARDWARE_INFO:
            return (*VECTOR_BOARD_INFO);
            
        case ADDRESS_BOOTLOADER_STARTUP:
            return (*VECTOR_BOOTLOADER_STARTUP);
            
        case ADDRESS_BOOTLOADER_UPDATE:
            return (*VECTOR_BOOTLOADER_UPDATE);
            
        case ADDRESS_BOOTLOADER_SIGNATURE:
            return (*VECTOR_BOOTLOADER_SIGNATURE);
            
        case ADDRESS_BOOTLOADER_INFO:
            return (*VECTOR_BOOTLOADER_INFO);            
    
        case ADDRESS_PROCESSOR_RESET:
            return (*VECTOR_RESET);            
    }
    return (NULL);
}

//****************************************************************************/

ERROR_t halSetAddress (ADDRESS_IDENTIFIER_t AddressID, void *Address)
{
    switch (AddressID) {
        case ADDRESS_BOOTLOADER_STARTUP:
            *VECTOR_BOOTLOADER_STARTUP = Address; break;
            
        case ADDRESS_BOOTLOADER_UPDATE:
            *VECTOR_BOOTLOADER_UPDATE = Address; break;
            
        case ADDRESS_BOOTLOADER_SIGNATURE:
            *VECTOR_BOOTLOADER_SIGNATURE = Address; break;
            
        case ADDRESS_BOARD_HARDWARE_INFO:
            *VECTOR_BOARD_INFO = Address; break;
            
        case ADDRESS_BOOTLOADER_INFO:
            *VECTOR_BOOTLOADER_INFO = Address; break;
            
        case ADDRESS_BOARD_OPTION_BLOCK:
            *VECTOR_BOARD_OPTIONS = Address; break;
    }
    return (NO_ERROR);
}


//****************************************************************************/

HANDLE_t halCanOpen (UInt16 Channel)
{
    return (0x1234);
}

//****************************************************************************/

ERROR_t halCanControl (HANDLE_t Handle, UInt16 InterruptID, UInt16 Value)
{
    Bool oldTransmitInterruptEnable = canTransmitInterruptEnable;
    
    if (InterruptID == INTERRUPT_CAN1_RX) {
        if (Value == CAN_INTR_ENABLE)
            canReceiveInterruptEnable = TRUE; 
        else if (Value == CAN_INTR_DISABLE)
            canReceiveInterruptEnable = FALSE; 
    }
    if (InterruptID == INTERRUPT_CAN1_TX) {
        if (Value == CAN_INTR_ENABLE) {
            canTransmitInterruptEnable = TRUE;

            if (!oldTransmitInterruptEnable && canTransmitInterruptHandler) {
                (*canTransmitInterruptHandler) (INTERRUPT_CAN1_TX);
            }
        } 
        else if (Value == CAN_INTR_DISABLE)
            canTransmitInterruptEnable = FALSE; 
    }
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halCanWrite (HANDLE_t Handle, CAN_MESSAGE_t* Message)
{
    if (Handle != 0x1234 || Message == NULL) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    dbgPrintCanMessage (Message, 'w');    
    return (1); 
}

//****************************************************************************/

ERROR_t halCanRead (HANDLE_t Handle, CAN_MESSAGE_t* Message)
{
    if (Handle != 0x1234)
        return (E_PARAMETER_OUT_OF_RANGE);
    
    if (Message != NULL) {        
        if (canInMessage.CanID != 0) {
            dbgPrintCanMessage (&canInMessage, 'r');    
            *Message = canInMessage;
            canInMessage.CanID = 0;
            return (1);
        }
    }    
    return (0);        
}

//****************************************************************************/

ERROR_t halCanInject (CAN_MESSAGE_t* Message)
{
    if (!isPtrNull(Message)) {
        if (canInMessage.CanID == 0) {
            canInMessage = *Message;
        }
        if (canReceiveInterruptHandler && canReceiveInterruptEnable) {
            (*canReceiveInterruptHandler) (INTERRUPT_CAN1_RX);
        }
    }    
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t canRegisterInterrupt ( 
    UInt16 InterruptID, HAL_INTERRUPT_HANDLER Handler)
{
    if (InterruptID == INTERRUPT_CAN1_RX) {
        canReceiveInterruptHandler = Handler;
    }
    if (InterruptID == INTERRUPT_CAN1_TX) {
        canTransmitInterruptHandler = Handler;
    }
    return (NO_ERROR);
}   

//****************************************************************************/

HANDLE_t halDigitalOpen (UInt16 Channel, UInt16 Mode)
{
    UInt16 Index = Channel & CHANNEL_INDEX_MASK;
    
    if (Index >= 10) {
        return (E_HAL_INVALID_CHANNEL);
    }
    if (Mode == HAL_OPEN_READ) {
        return (CHANNEL_CLASS_DIGIO + Index);
    }
    if (Mode == HAL_OPEN_WRITE) {
        return (CHANNEL_CLASS_DIGIO + Index);
    }
    return (E_HAL_INVALID_MODE);
}

//****************************************************************************/

ERROR_t halDigitalRead (HANDLE_t Handle, UInt16* value)
{    
    UInt16 Channel = Handle & CHANNEL_INDEX_MASK;
    
    if ((Handle & CHANNEL_CLASS_MASK) != CHANNEL_CLASS_DIGIO) {
        return (E_INVALID_HANDLE);
    }
    if (value != NULL) {
        if (halGetInputPattern(Channel+10, value)) {
            dbgPrint("Digital port[%d] == 0x%x", Channel, *value);
        }   
    }   
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halDigitalWrite (HANDLE_t Handle, UInt16 value)
{
    UInt16 Channel = Handle & CHANNEL_INDEX_MASK;
    
    if ((Handle & CHANNEL_CLASS_MASK) != CHANNEL_CLASS_DIGIO) {
        return (E_INVALID_HANDLE);
    }
    dbgPrint("Digital port[%d] := 0x%x", Channel, value);   
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halDigitalStatus (HANDLE_t Handle, UInt16 StatusID)
{
    switch (StatusID) {
        case 0: // return port size
            return (16);    
            
        case 1: // return maximum port value
            return (0xFFFF);    
    }
    return (0);
}

//****************************************************************************/

HANDLE_t halAnalogOpen (UInt16 Channel, UInt16 Mode)
{
    UInt16 Index = Channel & CHANNEL_INDEX_MASK;

    if (Index >= 10) {
        return (E_HAL_INVALID_CHANNEL);
    }
    if (Mode == HAL_OPEN_READ) {
        return (CHANNEL_CLASS_ANALOG + Index);
    }
    if (Mode == HAL_OPEN_WRITE) {
        return (CHANNEL_CLASS_ANALOG + Index);
    }
    return (E_HAL_INVALID_MODE);
}

//****************************************************************************/

ERROR_t halAnalogRead (HANDLE_t Handle, UInt16* value)
{
    UInt16 Channel = Handle & CHANNEL_INDEX_MASK;
    
    if ((Handle & CHANNEL_CLASS_MASK) != CHANNEL_CLASS_ANALOG) {
        return (E_INVALID_HANDLE);
    }
    if (value != NULL) {
        if (halGetInputPattern(Channel, value)) {
            dbgPrint("Analog port[%d] == 0x%x", Channel, *value);
        }
    }   
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halAnalogWrite (HANDLE_t Handle, UInt16 value)
{
    UInt16 Channel = Handle & CHANNEL_INDEX_MASK;
    
    if ((Handle & CHANNEL_CLASS_MASK) != CHANNEL_CLASS_ANALOG) {
        return (E_INVALID_HANDLE);
    }
    dbgPrint("Analog port[%d] := 0x%x", Channel, value);   

    return (NO_ERROR);
}

//****************************************************************************/

static Bool halGetInputPattern (UInt16 Channel, UInt16* Value)
{
    INPUT_SIMULATION_t* Pattern = &InputPatterns[Channel];
    Bool ChangeFlag = FALSE;
    
    if (Channel < ELEMENTS(InputPatterns) && Pattern->Elements) {
        
        if (Timers[2].Counter - Pattern->StartTime > 
                Pattern->Data[Pattern->Index].Duration) {

            UInt16 oldValue = Pattern->Value;
                                    
            if (Pattern->Steps == 0) {
                
                Pattern->Index += Pattern->Data[Pattern->Index].Goto;
                if (Pattern->Index >= Pattern->Elements) {
                    Pattern->Index = 0;
                }
                Pattern->Value = Pattern->Data[Pattern->Index].StartValue;
                Pattern->Steps = Pattern->Data[Pattern->Index].Steps;
            }
            else {
                Pattern->Value += Pattern->Data[Pattern->Index].Offset;
                Pattern->Steps--;
            }
            if (oldValue != Pattern->Value) {
                ChangeFlag = TRUE;
            }
            Pattern->StartTime = Timers[2].Counter;
        }
        *Value = Pattern->Value;
    }
    else *Value = 0;
    
    return (ChangeFlag);
}

//****************************************************************************/

static ERROR_t halRegisterInputPattern (
        UInt16 Channel, const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize)
{
    INPUT_SIMULATION_t* Pattern = &InputPatterns[Channel];

    Channel &= CHANNEL_INDEX_MASK;

    if (Channel < ELEMENTS(InputPatterns) && !isPtrNull(DataTable)) {
        
        Pattern->Elements = TableSize;
        Pattern->Data  = DataTable;
        Pattern->Steps = Pattern->Data[0].Steps;
        Pattern->Value = Pattern->Data[0].StartValue;
        Pattern->Index = 0;
        Pattern->StartTime = Timers[2].Counter;
        return (NO_ERROR);    
    }
    return (E_PARAMETER_OUT_OF_RANGE);    
}

//****************************************************************************/

ERROR_t halRegisterAnalogInputPattern (
        UInt16 Channel, const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize)
{
    return (halRegisterInputPattern(Channel, DataTable, TableSize));
}

ERROR_t halRegisterDigitalInputPattern (
        UInt16 Channel, const INPUT_PORT_DATA_t* DataTable, UInt16 TableSize)
{
    return (halRegisterInputPattern(Channel+10, DataTable, TableSize));
}

//****************************************************************************/

HANDLE_t halOpenStorage (UInt16 DeviceID, UInt16 Mode)
{
    char* Names[] = { 
        "partition.dat", "otp-flash.dat", "prg-flash.dat", "can-data.dat" };
    HANDLE_t Handle;
    UInt16 Flags;
    
    if (DeviceID < ELEMENTS(Names)) {
        if (Mode & HAL_OPEN_READ) {
            Flags = O_RDONLY + O_BINARY;
        }
        if (Mode & HAL_OPEN_WRITE) {
            Flags = O_RDWR + O_CREAT + O_BINARY;
            if (DeviceID == 3) {
                Flags |= O_TRUNC;
            }
        }    
        if ((Handle = open (Names[DeviceID], Flags, 0777)) < 0) {
            return (E_STORAGE_OPEN_ERROR);
        }
        return (Handle);
    }
    return (E_STORAGE_OPEN_ERROR);
}

//****************************************************************************/

HANDLE_t halCloseStorage (HANDLE_t Handle)
{
    close (Handle);
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halReadStorage (HANDLE_t Handle, UInt32 Address, void* buffer, UInt32 Size)
{
    UInt8 *ptr = buffer;
    UInt32 Total = Size;
    UInt16 Byte1, Byte2;
    UInt32 Count;
    UInt16 i;

    // convert word count/address to byte count/address (if necessary)
    if (BITS_PER_BYTE == 16) {
        Size *= 2; Address *= 2;
    }
    if (lseek (Handle, Address, SEEK_SET) < 0) {
        return (E_STORAGE_READ_ERROR);
    }    
    Count = min(BLOCKSIZE, Size);

    while (Count) {
        
        if (read (Handle, (char*)UnPacked, Count) != Count) {
            return (E_STORAGE_READ_ERROR);
        }
        for (i=0; i < Count; i+=2) {
            Byte1  = UnPacked[i+0] << 8;
            Byte2  = UnPacked[i+1] & 0xFF;            
            *ptr++ = Byte1 | Byte2;             
        }
        Size -= Count;
        Count = min(BLOCKSIZE, Size);
    }        
    return (Total);
}

//****************************************************************************/

ERROR_t halWriteStorage (HANDLE_t Handle, UInt32 Address, void* buffer, UInt32 Size)
{
    UInt8 *ptr = buffer;
    UInt32 Total = Size;
    UInt32 Count;
    UInt16 i;
    
    // convert word count/address to byte count/address (if necessary)
    if (BITS_PER_BYTE == 16) {
        Size *= 2; Address *= 2;
    }
    if (lseek (Handle, Address, SEEK_SET) < 0) {
        return (E_STORAGE_WRITE_ERROR);
    }    
    Count = min(BLOCKSIZE, Size);

    while (Count) {
                    
        for (i=0; i < Count;) {
            UnPacked[i++] = *ptr >> 8; 
            UnPacked[i++] = *ptr & 0xFF;
            ptr++;
        }
        if (write (Handle, (char*)UnPacked, Count) != Count) {
            return (E_STORAGE_WRITE_ERROR);
        }
        Size -= Count;
        Count = min(BLOCKSIZE, Size);
    }
    return (Total);
}

//****************************************************************************/

HANDLE_t halEraseStorage (HANDLE_t Handle, UInt32 Address, UInt32 Size)
{
    char buffer[16] = {0};
    UInt32 Count;

    if (Size > halStorageSize(Handle)) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }        
    Count = min(sizeof(buffer), Size);

    while (Count) {        
        halWriteStorage (Handle, Address, buffer, Count);
        Address += Count; Size -= Count;
        Count = min(sizeof(buffer), Size);
    }
    return (NO_ERROR);
}

//****************************************************************************/

UInt32 halStorageSize (HANDLE_t Handle)
{
    UInt32 Size = STORAGE_SIZE;
    
    // return word count on TMS320 plattform
    if (BITS_PER_BYTE == 16) {
        Size = STORAGE_SIZE / 2;
    }
    return (Size);
}

//****************************************************************************/

void halHardwareReset (void)
{
    BOOTLOADER_VECTOR *ResetVector;
    
    dbgPrint ("Performing Hardware Reset");
    ResetVector = (BOOTLOADER_VECTOR*) halGetAddress(ADDRESS_BOOTLOADER_STARTUP);
    (*ResetVector)();
}

//****************************************************************************/

ERROR_t halWatchdogTrigger (void)
{
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halWatchdogEnable (void)
{
    return (NO_ERROR);
}

//****************************************************************************/

RESET_REASON_t halGetResetReason (void) {
    
    return (RESET_CAUSED_BY_POWER_UP);
}

//****************************************************************************/

void halEnableInterrupt (UInt16 InterruptID)  {}    
void halDisableInterrupt(UInt16 InterruptID)  {}

//****************************************************************************/

ERROR_t halRegisterInterrupt ( 
    UInt16 InterruptID, HAL_INTERRUPT_HANDLER Handler)
{
    return (NO_ERROR);
}
   
//****************************************************************************/

Bool halPowerSupplyGood (void)
{
    return (TRUE);
}

//****************************************************************************/

HANDLE_t halTimerOpen (UInt16 Channel, UInt16 Resolution)
{
    int myChannel = Channel & CHANNEL_INDEX_MASK;
    
    if (myChannel < 0 || myChannel >= ELEMENTS(Timers)) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    Timers[myChannel].Resolution = 2; //Resolution;
    Timers[myChannel].Opened = TRUE;
    
    return (Channel);
}

//****************************************************************************/

ERROR_t halTimerRead (HANDLE_t Handle, UInt32* Counter)
{    
    int Channel = Handle & CHANNEL_INDEX_MASK;
    
    if (Channel < 0 || Channel >= ELEMENTS(Timers)) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (!Timers[Channel].Opened) {
        return (E_HAL_DEVICE_NOT_OPEN);
    }
    if (Counter != NULL) {
        *Counter = Timers[Channel].Counter;
    }
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halTimerWrite (HANDLE_t Handle, UInt32 Counter, UInt16 ControlID)
{    
    int Channel = Handle & CHANNEL_INDEX_MASK;
    
    if (Channel < 0 || Channel >= ELEMENTS(Timers)) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (!Timers[Channel].Opened) {
        return (E_HAL_DEVICE_NOT_OPEN);
    }
    if (Counter != NULL) {
        Timers[Channel].Period = Counter;
    }
    halTimerControl (Handle, ControlID);
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halTimerControl (HANDLE_t Handle, UInt16 ControlID)
{
    int Channel = Handle & CHANNEL_INDEX_MASK;
    
    if (Channel < 0 || Channel >= ELEMENTS(Timers)) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    if (!Timers[Channel].Opened) {
        return (E_HAL_DEVICE_NOT_OPEN);
    }
    if (ControlID & TIMER_CTRL_START) {
        Timers[Channel].Running = TRUE;
    }
    if (ControlID & TIMER_CTRL_STOP) {
        Timers[Channel].Running = FALSE;
    }
    if (ControlID & TIMER_CTRL_RELOAD) {
        Timers[Channel].Counter = Timers[Channel].Period;
    }
    return (NO_ERROR);
}

//****************************************************************************/

ERROR_t halControlTask (void)
{
    const int Step = 10;
	int i;
	
	for (i=0; i < ELEMENTS(Timers); i++) {
		
		if (Timers[i].Running) {
			if (++Timers[i].Divider >= Timers[i].Resolution) {
				Timers[i].Divider = 0;
                if (Timers[i].Counter <= Step) {
                    Timers[i].Counter = Timers[i].Period;
                }
                else {
                    Timers[i].Counter -= Step;
                }
			}
		}
	}
    return (NO_ERROR);    
}

//****************************************************************************/

ERROR_t halInitializeHalLayer (void) 
{
    return (NO_ERROR);
}

//****************************************************************************/

#endif

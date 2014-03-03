/****************************************************************************/
/*! \file halSerial.c
 *
 *  \brief  Serial communication interface
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the on-chip
 *      asynchronous serial communication interfaces (UART).
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
#include "halInterrupt.h"
#include "halSerial.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define UART_SR_CTS         0x0200   //!< CTS changed flag
#define UART_SR_TXE         0x0080   //!< Transmit data register empty
#define UART_SR_TC          0x0040   //!< Transmission complete
#define UART_SR_RXNE        0x0020   //!< Read data register not empty
#define UART_SR_IDLE        0x0010   //!< IDLE line detected
#define UART_SR_OE          0x0008   //!< Overrun error flag
#define UART_SR_NE          0x0004   //!< Noise error flag
#define UART_SR_FE          0x0002   //!< Framing error flag
#define UART_SR_PE          0x0001   //!< Parity errorflag
#define UART_SR_ERROR       0x000F   //!< Or of OE, NE, PE, PE

#define UART_CR1_UE         0x2000   //!< USART enable
#define UART_CR1_M          0x1000   //!< Word length
#define UART_CR1_PCE        0x0400   //!< Parity control enable
#define UART_CR1_PS         0x0200   //!< Parity selection
#define UART_CR1_TXEIE      0x0080   //!< TXE interrupt enable
#define UART_CR1_RXNEIE     0x0020   //!< RXNE interrupt enable
#define UART_CR1_IDLEIE     0x0010   //!< IDLE interrupt enable
#define UART_CR1_TE         0x0008   //!< Transmitter enable
#define UART_CR1_RE         0x0004   //!< Receiver enable
#define UART_CR1_SBK        0x0001   //!< Send break
     
#define UART_CR2_STOP       0x3000   //!< Number of stop bits
#define UART_CR2_STOP1      0x0000   //!< Number of stop bits = 1
#define UART_CR2_STOP2      0x2000   //!< Number of stop bits = 2
#define UART_CR2_CLKEN      0x0800   //!< Clock pin enable
#define UART_CR2_CPOL       0x0400   //!< Clock pin polarity
#define UART_CR2_CPHA       0x0200   //!< Clock pin phase
#define UART_CR3_CTSE       0x0020   //!< CTS hardware flow control 
#define UART_CR3_RTSE       0x0010   //!< RTS hardware flow control 

#define UART_BRR_FRAC       0x000F   //!< Baudrate DIV Fraction
#define UART_BRR_MANT       0xFFF0   //!< Baudrate DIV Mantissa


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Layout of processor internal serial communication interface registers
typedef volatile struct {
    UInt32 SR;              //!< USART Status register
    UInt32 DR;              //!< USART Data register
    UInt32 BRR;             //!< USART Baud rate register
    UInt32 CR1;             //!< USART Control register 1
    UInt32 CR2;             //!< USART Control register 2
    UInt32 CR3;             //!< USART Control register 3
    UInt32 GTPR;            //!< USART Guard time and prescaler register
} SciRegFile_t;

//! Structure to hold the state of a serial communication device
typedef struct {
    SciRegFile_t *UART;     //!< Pointer to register file
    UInt16 PeripheralID;    //!< Peripheral identifier
    UInt16 InterruptID;     //!< Interrupt identifier
    UInt16 Flags;           //!< Open flags
    UInt16 Bitmask;         //!< Data bit mask
} SciDevice_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Data table holding the variables for all serial channels
static SciDevice_t DataTable[] = {
    { (SciRegFile_t*) 0x40013800, PERIPHERAL_UART1, INTERRUPT_UART1 },
    { (SciRegFile_t*) 0x40004400, PERIPHERAL_UART2, INTERRUPT_UART2 },
    { (SciRegFile_t*) 0x40004800, PERIPHERAL_UART3, INTERRUPT_UART3 },
    { (SciRegFile_t*) 0x40004C00, PERIPHERAL_UART4, INTERRUPT_UART4 },
    { (SciRegFile_t*) 0x40005000, PERIPHERAL_UART5, INTERRUPT_UART5 }
};

//! Default data format and baudrate (can be used on halSerialSetup)
const SciFormat_t halSerialDefaultMode = { 
    8, 1, SCI_PARITY_NONE, SCI_FLOW_NONE, 9600 
};

//! Interrupt vector table for all UART devices
static InterruptVector_t SerialIntrVectors[ELEMENTS(DataTable)] = {0};


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halSerialBaudrate   (Handle_t Handle, UInt32 Baudrate);
static Error_t halSerialGetIndex   (Handle_t Handle, UInt16 Mode);
static Error_t halSerialFindDevice (Device_t DeviceID);


/*****************************************************************************/
/*!
 *  \brief   Open a serial communication interface
 *
 *      Opens a serial communication interface and returns a handle to
 *      be used to read/write from/to that interface. DeviceID must be
 *      the numerical "name" of a serial device defined in the HAL
 *      configuration file. 
 *
 *      Handler must be a pointer to an interrupt handler function to be 
 *      called when the device signals an interrupt. It can be NULL if no
 *      interrupts are used. UserTag is a user defined number to be passed 
 *      to the handler function, e.g. as index into a data table in case 
 *      of multiple instances. 
 *
 *  \iparam  DeviceID = Serial device ID
 *  \iparam  UserTag  = User tag to pass thru to interrupt handler
 *  \iparam  Handler  = Interrupt handler function
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halSerialOpen (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler) {

    const Int32 Index = halSerialFindDevice(DeviceID);
    Error_t Status;

    if (Index >= 0) {
        const Int32 UnitNo = halSerialDescriptors[Index].UnitNo;       
        const Int32 Handle = DEVICE_CLASS_SERIAL + UnitNo;
        SciDevice_t *Data  = &DataTable[UnitNo];

        if (Data->Flags & HAL_OPEN_RW) {
            return (E_DEVICE_ALREADY_OPEN);
        }
        Data->Flags |= HAL_OPEN_RW;
    
        Status = halSerialSetup(Handle, &halSerialDefaultMode);
        if (Status == NO_ERROR) {
            if (halSerialDescriptors[Index].Baudrate) {
                halSerialBaudrate (Handle, halSerialDescriptors[Index].Baudrate);
            }
            SerialIntrVectors[UnitNo].Handler = Handler;
            SerialIntrVectors[UnitNo].UserTag = UserTag;
        
            if (Handler != NULL) {
                halInterruptEnable (Data->InterruptID, IRQ_PRIO_DEFAULT);
            }
            if (Data->Flags & HAL_OPEN_WRITE) {
                Data->UART->CR1 |= UART_CR1_TE;
            }
            if (Data->Flags & HAL_OPEN_READ) {
                Data->UART->CR1 |= UART_CR1_RE;
            }
            // Read data register to reset error flags
            Status = Data->UART->DR;                             
    
            return (Handle);                               //lint !e438
        }
        halSerialClose (Handle);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Close serial communication interface
 *
 *      Closes an open serial communication interface. All communication is
 *      immediately stopped and the SCI hardware disabled. Data not yet send
 *      will be discarded.
 *
 *  \iparam  Handle = Handle of serial communication channel
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halSerialClose (Handle_t Handle) {

    const Int32 Index = halSerialGetIndex(Handle, HAL_OPEN_RW);

    if (Index >= 0) {
        halInterruptDisable (DataTable[Index].InterruptID);
        DataTable[Index].UART->CR1 = UART_CR1_UE;
        DataTable[Index].Flags &= ~HAL_OPEN_MODE;;
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Read from serial communication channel
 *
 *      Reads the next received data byte from the serial communication
 *      interface associated with Handle and returns the result in the
 *      buffer pointed to by Buffer.
 *
 *      Exactly one data byte is read and returned, as far as an unread
 *      data byte is available. If no new receive data is available, the
 *      function terminates immediately without returning any data. The
 *      return parameter indicates the number of received data bytes,
 *      which can be eigther 0 or 1. In case of an error, a negative 
 *      error code is returned.
 *
 *  \iparam  Handle = Handle of serial communication channel
 *  \iparam  Buffer = Pointer to data buffer
 *
 *  \return  Number of read bytes [0,1] or (negative) error code
 *
 ****************************************************************************/

Error_t halSerialRead (Handle_t Handle, UInt16 *Buffer) {

    const Int32 Index = halSerialGetIndex(Handle, HAL_OPEN_READ);

    if (Index >= 0) {

        UInt16 Status = DataTable[Index].UART->SR;

        if (Status & UART_SR_RXNE) {
            if (Buffer != NULL) {
                *Buffer = DataTable[Index].UART->DR & DataTable[Index].Bitmask;
            }
            if (Status & UART_SR_ERROR) {
                Error_t ErrCode;

                if (Status & UART_SR_FE) {
                   ErrCode = E_SCI_FRAMING_ERROR;
                }
                else if (Status & UART_SR_PE) {
                   ErrCode = E_SCI_PARITY_ERROR;
                }
                else if (Status & UART_SR_OE) {
                   ErrCode = E_SCI_OVERRUN_ERROR;
                }
                else if (Status & UART_SR_NE) {
                   ErrCode = E_SCI_NOISE_ERROR;
                }
                else {
                   ErrCode = E_SCI_RECEIVE_ERROR;
                }
                return (ErrCode);
            }
            return (1); // one data byte read
        }
        return (0); // no data available
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Write to serial communication channel
 *
 *      Writes a byte to the serial communication interface associated
 *      with Handle and returns the number of bytes successfully written,
 *      which can be 1 if the write succeeds or 0 if it fails. In case
 *      of an error, a negative error code is returned.
 *
 *      If the data byte can't be written immediately, because the
 *      transmitter is still busy transmitting previously data, the
 *      function terminates immediately without writing data.
 *
 *  \iparam  Handle = Handle of serial communication channel
 *  \iparam  Data   = Data to write
 *
 *  \return  Number of written bytes [0,1] or (negative) error code
 *
 ****************************************************************************/

Error_t halSerialWrite (Handle_t Handle, UInt16 Data) {

    const Int32 Index = halSerialGetIndex(Handle, HAL_OPEN_WRITE);

    if (Index >= 0) {
        SciRegFile_t *UART = DataTable[Index].UART;

        if (UART->SR & UART_SR_TXE) {
            UART->DR = Data;
            return (1); // one data byte written
        }
        return (0); // no data written
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Get status of serial communication channel
 *
 *      Returns the status of the serial communication interface associated
 *      with Handle. What status to return depends on the supplied StatusID.
 *      The following informations can be requested:
 *
 *      - Receiver ready status
 *      - Transmitter ready status
 *      - Transmitter empty status (all data send)
 *      - Receive line idle status
 *
 *      If a unsupported StatusID is supplied, an error is returned.
 *
 *  \iparam  Handle   = Handle of serial communication channel
 *  \iparam  StatusID = Type of status
 *
 *  \return  Status value or (negative) error code
 *
 ****************************************************************************/

Error_t halSerialStatus (Handle_t Handle, SciStatID_t StatusID) {

    const Int32 Index = halSerialGetIndex(Handle, HAL_OPEN_RW);

    if (Index >= 0) {
        SciRegFile_t *UART = DataTable[Index].UART;

        switch (StatusID) {

            case SCI_STAT_RxREADY:
                return (UART->SR & UART_SR_RXNE);

            case SCI_STAT_TxREADY:
                return (UART->SR & UART_SR_TXE);

            case SCI_STAT_TxEMPTY:
                return (UART->SR & UART_SR_TC);

            case SCI_STAT_RxIDLE:
                return (UART->SR & UART_SR_IDLE);
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Control serial communication channel
 *
 *      Controls the serial communication interface associated with Handle.
 *      Depending on ControlID the following actions can be initiated:
 *
 *      - Receiver interrupt enable/disable
 *      - Transmitter interrupt enable/disable
 *      - Idle line interrupt enable/disable
 *      - Send break frame
 *
 *      State determines if to enable (TRUE) or disable (FALSE) the 
 *      selected function.
 *
 *  \iparam  Handle    = Handle of serial communication channel
 *  \iparam  ControlID = Control type identifier
 *  \iparam  State     = Enable: True, Disable: False
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halSerialControl (Handle_t Handle, SciCtrlID_t ControlID, Bool State) {

    const Int32 Index = halSerialGetIndex(Handle, HAL_OPEN_RW);
    UInt32 EnableBitmask;

    if (Index >= 0) {
        switch (ControlID) {

            case SCI_INTR_RxREADY:
                EnableBitmask = UART_CR1_RXNEIE;
                break;

            case SCI_INTR_TxREADY:
                EnableBitmask = UART_CR1_TXEIE;
                break;

            case SCI_INTR_RxIDLE:
                EnableBitmask = UART_CR1_IDLEIE;
                break;

            case SCI_CTRL_TxBREAK:
                EnableBitmask = UART_CR1_SBK;
                break;

            default:
                return (E_UNKNOWN_CONTROL_ID);
        }
        if (State) {
            DataTable[Index].UART->CR1 |= EnableBitmask;
        }
        else {
            DataTable[Index].UART->CR1 &= ~EnableBitmask;
        }
        return (NO_ERROR);
    }
    return (Index);
}

/*****************************************************************************/
/*!
 *  \brief   Set mode of serial communication channel
 *
 *      Sets the data format and bitrate of the serial communication
 *      interface associated with Handle. Format defines the number of 
 *      databits, the number of stop bits, parity mode (none, even, odd) 
 *      and the baudrate.
 *
 *      The hardware supports only the following data formats:
 *
 *      - 7 data bits with parity (even or odd)
 *      - 8 data bits with and without parity
 *      - 9 data bits without parity
 *
 *      Baud rates below 600 bits/s are not supported. If Format->Baudrate 
 *      is zero, the actual baudrate is left unchanged.
 *
 *  \iparam  Handle = Handle of serial communication channel
 *  \iparam  Format = Data format (data bits, stop bits, parity)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halSerialSetup (Handle_t Handle, const SciFormat_t *Format) {

    const Int32 Index = halSerialGetIndex(Handle, HAL_OPEN_RW);
    const UInt16 Databits = Format->Databits;
    const UInt16 Parity = Format->Parity;

    if (Index >= 0) {
        SciRegFile_t *UART = DataTable[Index].UART;

        if (Format == NULL) {
            return (E_SCI_FORMAT_MISSING);
        }
        // Setup number of data bits
        if ((Databits == 7 && Parity != SCI_PARITY_NONE) ||
            (Databits == 8 && Parity == SCI_PARITY_NONE)) {
            UART->CR1 &= ~UART_CR1_M;
        }
        else if ((Databits == 8 && Parity != SCI_PARITY_NONE) ||
            (Databits == 9 && Parity == SCI_PARITY_NONE)) {
            UART->CR1 |= UART_CR1_M;
        }
        else {
            return (E_SCI_ILLEGAL_FORMAT);
        }
        // Setup parity mode (none, odd, even)
        if (Parity) {
            if (Parity == SCI_PARITY_EVEN) {
                UART->CR1 &= ~UART_CR1_PS;
            }
            else if (Parity == SCI_PARITY_ODD) {
                UART->CR1 |= UART_CR1_PS;
            }
            UART->CR1 |= UART_CR1_PCE;
        }
        UART->CR2 = (UART->CR2 & UART_CR2_STOP) |
            (Format->Stopbits-1) ? UART_CR2_STOP2 : UART_CR2_STOP1;

        DataTable[Index].Bitmask = ~(MAX_UINT32 << Databits);

        // Setup hardware flow control (RTS/CTS/None)
        UART->CR3 &= ~(SCI_FLOW_CTS | SCI_FLOW_RTS);   //lint !e655
        if (Format->FlowCtrl & SCI_FLOW_CTS) {
            UART->CR3 |= UART_CR3_CTSE;
        }
        if (Format->FlowCtrl & SCI_FLOW_RTS) {
            UART->CR3 |= UART_CR3_RTSE;
        }
        if (Format->Baudrate) {
            return (halSerialBaudrate (Handle, Format->Baudrate));
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Set baud rate of serial communication channel
 *
 *      Sets the bitrate of the serial communication interface associated
 *      with Handle. Baudrates below 600 bits/s are not supported if
 *      peripheral clock rate is set to 36MHz.
 *
 *  \iparam  Handle   = Handle of serial communication channel
 *  \iparam  Baudrate = Data rate (bits/sec)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halSerialBaudrate (Handle_t Handle, UInt32 Baudrate) {

    const Int32 Index = halSerialGetIndex(Handle, HAL_OPEN_RW);
    UInt32 Division;
    UInt32 Fraction;

    if (Index >= 0) {
        SciRegFile_t *UART = DataTable[Index].UART;

        if (!Baudrate) {
            return (E_SCI_ILLEGAL_BAUDRATE);
        }
        Division = (halGetPeripheralClockRate() / 16) * 1000 / Baudrate;
        Fraction = (Division % 1000) * 16;
        Division = (Division / 1000  * 16) + (Fraction + 500) / 1000;

        if (Division & ~MAX_UINT16) {
            return (E_SCI_ILLEGAL_BAUDRATE);
        }
        UART->BRR = Division;

        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Serial master interrupt handler
 *
 *      This function will be called by the vectored interrupt controller in
 *      case of a UART interrupt. Channel is the number of the UART channel.
 *      It calls the user handler registered with halSerialRegisterHandler(),
 *      passing the interrupt flag bits and the user tag to it. Before
 *      exiting, the handled interrupt flag bits are cleared.
 *
 *  \iparam  Channel = Serial channel number 
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void halSerialInterruptHandler (UInt32 Channel) {

    if (Channel < ELEMENTS(DataTable)) {
        UInt32 IntrFlags = DataTable[Channel].UART->SR >> 4;
    
        if (SerialIntrVectors[Channel].Handler != NULL) {
            SerialIntrVectors[Channel].Handler (SerialIntrVectors[Channel].UserTag, IntrFlags);
        }
    }
}


/*****************************************************************************/
/*!
 *  \brief   Get index of a serial communication channel
 *
 *      Returns the index of the serial communication interface associated
 *      with Handle. The index is used to address an entry in the SciData
 *      table, which contains all variables of that channel.
 *
 *      The channel must be opened in a mode compliant to the supplied
 *      Mode, else an error is returned.
 *
 *  \iparam  Handle = Handle of open analog channel
 *  \iparam  Mode   = Open mode
 *
 *  \return  Serial channel index or (negative) error code
 *
 ****************************************************************************/

static Error_t halSerialGetIndex (Handle_t Handle, UInt16 Mode) {

    register const UInt32 Index = Handle ^ DEVICE_CLASS_SERIAL;

    if (Index < ELEMENTS(DataTable)) {

        const UInt16 Flags = DataTable[Index].Flags;

        if (!(Flags & HAL_OPEN_RW)) {
            return (E_DEVICE_NOT_OPEN);
        }
        if (!(Flags & Mode)) {
            return (E_DEVICE_ACCESS_MODE);
        }
        return ((Error_t) Index);
    }
    return (E_DEVICE_HANDLE_INVALID);
}


/*****************************************************************************/
/*!
 *  \brief   Find logical serial communication channel
 *
 *      Searches the logical serial device descriptor for an entry with 
 *      the asked DeviceID. If successful, the index into the descriptor
 *      table is returned. If no such entry exists, an error is returned.
 *
 *  \iparam  DeviceID = Serial device ID
 *
 *  \return  Index or (negative) error code
 *
 ****************************************************************************/

static Error_t halSerialFindDevice (Device_t DeviceID) {

    UInt32 Index;

    if ((DeviceID & DEVICE_CLASS_MASK) == DEVICE_CLASS_SERIAL) {

        halInitState_t InitState = halGetInitState();

        if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
            return (E_HAL_NOT_INITIALIZED);
        }
        for (Index=0; Index < halSerialDescriptorCount; Index++) {

            if (halSerialDescriptors[Index].DeviceID == DeviceID) {
                UInt32 UnitNo = halSerialDescriptors[Index].UnitNo;

                if (UnitNo >= ELEMENTS(DataTable)) { 
                   return (E_DEVICE_NOT_EXISTS);
                }
                if (~DataTable[UnitNo].Flags & HAL_FLAG_INITZED) {
                    return (E_DEVICE_UNCONFIGURED);
                }
                return (Index);
            }
        }
    }
    return (E_UNKNOWN_DEVICE_ID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize serial communication module
 *
 *      Initializes the processor's on-chip serial communication interfaces-
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halSerialInit (void) {

    UInt32 SupportedUARTs = MIN(halProcessorInfo.CountUART, ELEMENTS(DataTable));
    UInt32 i;

    if (halGetInitState() == INIT_IN_PROGRESS) {
        const halSerialDescriptor_t *Descriptor = halSerialDescriptors;

        for (i=0; i < halSerialDescriptorCount; i++) {
            if (Descriptor->BusType == SIO_TYPE_UART) {
                           
                const UInt32 UnitNo = Descriptor->UnitNo;
    
                // Check if port number is legal
                if (UnitNo >= SupportedUARTs) {
                    return (E_PORT_NOT_EXISTS);
                }
                // Check if interface already initialized
                if (DataTable[UnitNo].Flags & HAL_FLAG_INITZED) {
                    return (E_BUS_ALLOCATED_TWICE);
                }
                halPeripheralClockEnable (
                    DataTable[UnitNo].PeripheralID, ON);

                DataTable[UnitNo].UART->CR1 = UART_CR1_UE;
                DataTable[UnitNo].Flags = HAL_FLAG_INITZED;
            }
            Descriptor++;
        }
    }
    return (NO_ERROR);
}

//****************************************************************************/

/****************************************************************************/
/*! \file halSerial.c
 * 
 *  \brief  Serial communication interface
 *
 *  $Version: $ 0.1
 *  $Date:    $ 21.10.2010
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains functions to access and manage the on-chip
 *       serial communication interfaces (SCI) of the processor.
 * 
 *       ATTENTION: This is still an early version of the module. 
 *       Development was stopped after changing the processor for
 *       the colorado/sepia project. For instance, the hardware  
 *       FIFO extension of the SCI peripherals are not yet used.
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

#include "Global.h"
#include "halError.h"
#include "halConfigure.h"
#include "halCommon.h"
#include "halSystem.h"
#include "halSerial.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define SCICTL1_RxENA       0x0001   //!< SCI software reset
#define SCICTL1_TxENA       0x0002   //!< SCI software reset
#define SCICTL1_SLEEP       0x0004   //!< SCI software reset
#define SCICTL1_TxWAKE      0x0008   //!< SCI software reset
#define SCICTL1_SWRESET     0x0020   //!< SCI software reset
#define SCICTL1_ERRINTENA   0x0040   //!< SCI receive error interrupt enable

#define SCICTL2_TxINTENA    0x0001   //!< SCITXBUF-register interrupt enable
#define SCICTL2_RxBKINTENA  0x0002   //!< Receiver-buffer/break interrupt enable
#define SCICTL2_TxEMPTY     0x0040   //!< SCI Transmitter empty flag
#define SCICTL2_TxRDY       0x0080   //!< SCI Transmit data register ready flag

#define SCICCR_DATABITS     0x0007   //!< SCI Character-length control bits    
#define SCICCR_LOOPBACK     0x0010   //!< SCI Loop Back test mode enable    
#define SCICCR_ENA_PARITY   0x0020   //!< SCI parity enable
#define SCICCR_EVEN_PARITY  0x0040   //!< SCI parity odd/even selection
#define SCICCR_STOPBITS     0x0080   //!< SCI number of stop bits

#define SCIRXST_RxWAKE      0x0002   //!< Receiver wake-up-detect flag
#define SCIRXST_PE          0x0004   //!< SCI parity-error flag
#define SCIRXST_OE          0x0008   //!< SCI overrun-error flag
#define SCIRXST_FE          0x0010   //!< SCI framing-error flag
#define SCIRXST_BRKDT       0x0020   //!< SCI break-detect flag
#define SCIRXST_RxRDY       0x0040   //!< SCI receiver-ready flag
#define SCIRXST_RxERROR     0x0080   //!< SCI receive error flag

#define SCIRXBUF_DATA       0x00FF  //!< Receive data register bismask

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Layout of processor internal serial communication interface registers */
typedef struct {
    UInt16 SciCCR;          //!< SCI communications control register
    UInt16 SciCtrl1;        //!< SCI Control Register 1
    UInt16 SciHiBaud;       //!< SCI Baud Register, High Bits
    UInt16 SciLoBaud;       //!< SCI Baud Register, Low Bits
    UInt16 SciCtrl2;        //!< SCI Control Register 2
    UInt16 SciRxStat;       //!< SCI Receive Status Register
    UInt16 SciRxEmu;        //!< SCI Receive Emulation Data Buffer Register
    UInt16 SciRxData;       //!< SCI Receive Data Buffer Register
    UInt16 Reserved1[1];    //!< Reserved
    UInt16 SciTxData;       //!< SCI Transmit Data Buffer Register
    UInt16 SciFifoTx;       //!< SCI FIFO Transmit Register
    UInt16 SciFifoRx;       //!< SCI FIFO Receive Register
    UInt16 SciFifoCtrl;     //!< SCI FIFO Control Register
    UInt16 Reserved2[2];    //!< Reserved
    UInt16 SciPri;          //!< SCI Priority Control Register
} SCI_REGISTERS_t;

/*! Structure to hold the state of a serial communication device */
typedef struct {
    volatile SCI_REGISTERS_t* SciRegs;  //!< pointer to SCI register file
    UInt16 Flags;                       //!< open flag
} SCI_DEVICE_t;


//****************************************************************************/
// Private Variables 
//****************************************************************************/

/*! Data table holding the variables for all serial channels */
static SCI_DEVICE_t SciData[] = {
    { (SCI_REGISTERS_t*) 0x007050, 0 },  //!< SCI-A register file address
    { (SCI_REGISTERS_t*) 0x007750, 0 }   //!< SCI-B register file address 
};

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static volatile SCI_REGISTERS_t *halSerialRegisters (HANDLE_t Handle);
static ERROR_t halGetSerialIndex (HANDLE_t Handle, UInt16 Mode);


/*****************************************************************************/
/*! 
 *  \brief   Open a serial communication interface
 *
 *      Opens a serial communication interface and returns a handle to 
 *      be used to read/write from/to that interface. ChannelID must 
 *      be the numerical "name" of a serial channel defined in the HAL
 *      configuration file. 
 * 
 *      Format defines the data format (number of data bits, number of
 *      stop bits, parity off/even/odd) to be used. If a null pointer
 *      is supplied, the format is left as is. Baudrate specifies the
 *      transmission rate in bits per second. If zero is supplied, the
 *      baudrate is left as is. 
 * 
 *  \iparam  ChannelID = Serial channel ID
 *  \iparam  Baudrate  = Data rate (bits per second)
 *  \iparam  Format    = Data format
 * 
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

ERROR_t halSerialOpen (CHANNEL_t Channel, SCI_FORMAT_t *Format, UInt16 Baudrate) {

    const Int32 Index = Channel & CHANNEL_INDEX_MASK;
    ERROR_t Status;
    
     if ((Channel & CHANNEL_CLASS_MASK) != CHANNEL_CLASS_SERIAL) {
       return (halSetError(E_HAL_INVALID_CHANNEL));
    }    
    if (Index >= ELEMENTS(SciData)) {
       return (halSetError(E_HAL_INVALID_CHANNEL));
    }
    if (SciData[Index].Flags & HAL_OPEN_RW) {
        return (halSetError(E_HAL_DEVICE_IN_USE));
    }
    SciData[Index].Flags = HAL_OPEN_RW;

    if ((Status = halSerialMode (Channel, Format, Baudrate)) == NO_ERROR) {
    
        SciData[Index].SciRegs->SciCtrl1 = 
            SCICTL1_RxENA + SCICTL1_TxENA + SCICTL1_SWRESET;
            
        return (Channel);
    }
    halSerialClose (Channel);
    
    return (Status);    
}


/*****************************************************************************/
/*! 
 *  \brief   Open a serial communication interface
 *
 *      Closes an open serial communication interface. All communication is
 *      immediately stopped and the SCI hardware disabled. 
 * 
 *  \iparam  Handle = Handle of serial communication channel
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halSerialClose (HANDLE_t Handle) {

    const Int32 Index = halGetSerialIndex(Handle, HAL_OPEN_RW);
    
    if (Index >= 0) {
        SciData[Index].SciRegs->SciCtrl1 = 0;
        SciData[Index].SciRegs->SciCtrl2 = 0;    
        SciData[Index].Flags = 0;    
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
 *      data byte is available. If no new receive data byte is available,
 *      the behaivor depends on the Blocking flag. If Blocking is false,
 *      the function terminates immediately without returning any data. 
 *      If blocking is true, it waits until new data is received. The
 *      return parameter indicates the number of received data bytes
 *      and can be 0 (no data read) or 1 (one data byte read). In case
 *      of an error, a negative error code is returned.
 *  
 *      In blocking mode, the halIdleTask is called periodically.
 * 
 *  \iparam  Handle   = Handle of serial communication channel
 *  \iparam  Buffer   = Pointer to data buffer 
 *  \iparam  Blocking = Blocking mode flag
 * 
 *  \return  Number of read bytes [0,1] or (negative) error code
 *
 ****************************************************************************/

ERROR_t halSerialRead (HANDLE_t Handle, UInt8* Buffer, Bool Blocking) {

    volatile SCI_REGISTERS_t* SciRegs = halSerialRegisters(Handle);
    ERROR_t errno;
    
    if (isPtrNull(SciRegs)) {
       return (halGetError());
    }
    do {        
        if (SciRegs->SciRxStat & SCIRXST_RxRDY) {
        
            if (!isPtrNull(Buffer)) {
                *Buffer = SciRegs->SciRxData & SCIRXBUF_DATA;
            }
            if (SciRegs->SciRxStat & SCIRXST_RxERROR) {
            
                if (SciRegs->SciRxStat & SCIRXST_FE) {
                   errno = E_SCI_FRAMING_ERROR;
                }
                else if (SciRegs->SciRxStat & SCIRXST_PE) {
                   errno = E_SCI_PARITY_ERROR;
                }
                else if (SciRegs->SciRxStat & SCIRXST_OE) {
                   errno = E_SCI_OVERRUN_ERROR;
                }
                else {
                   errno = E_SCI_BREAK_DETECED;
                }
                return (halSetError(errno));
            }            
            return (1); // one data byte read
        }
        if (SciRegs->SciRxStat & SCIRXST_BRKDT) {
        
            SciRegs->SciCtrl1 &= ~SCICTL1_SWRESET;
            return (halSetError(E_SCI_BREAK_DETECED));
        }
        halIdleTask();
        
    } while (Blocking);
    
    return (0); // no data read
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
 *      behaivor depends on the Blocking flag. If Blocking is false,
 *      the function terminates immediately without writing the data. 
 *      If blocking is true, it waits until the transmitter is ready
 *      to take the next data byte. 
 *  
 *      In blocking mode, the halIdleTask is called periodically.
 * 
 *  \iparam  Handle   = Handle of serial communication channel
 *  \iparam  Buffer   = Data to write 
 *  \iparam  Blocking = Blocking mode flag
 * 
 *  \return  Number of written bytes [0,1] or (negative) error code
 *
 ****************************************************************************/

ERROR_t halSerialWrite (HANDLE_t Handle, UInt8 Data, Bool Blocking) {

    volatile SCI_REGISTERS_t *SciRegs = halSerialRegisters(Handle);
    
    if (isPtrNull(SciRegs)) {
       return (halGetError());
    }
    do {     
        if (SciRegs->SciCtrl2 & SCICTL2_TxRDY) {
        
            SciRegs->SciTxData = Data;            
            return (1); // one data byte written
        }
        halIdleTask();
    } while (Blocking);
    
    return (0); // no data written
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
 *      - Break signal detected
 *      - Receive error detected
 *      - Framing error detected
 *      - Overrun error detected
 *      - Parity error detected
 * 
 *      If a unsupported StatusID is supplied, an error is returned.
 * 
 *  \iparam  Handle   = Handle of serial communication channel
 *  \iparam  StatusID = Type of status
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halSerialStatus (HANDLE_t Handle, SCI_STATUS_t StatusID) {
    
    volatile SCI_REGISTERS_t* SciRegs = halSerialRegisters(Handle);
    
    if (isPtrNull(SciRegs)) {
       return (halGetError());
    }
    switch (StatusID) {
    
        case SCI_STAT_RX_READY:
            return (SciRegs->SciRxStat & SCIRXST_RxRDY);

        case SCI_STAT_TX_READY:
            return (SciRegs->SciCtrl2 & SCICTL2_TxRDY);

        case SCI_STAT_TX_EMPTY:
            return (SciRegs->SciCtrl2 & SCICTL2_TxEMPTY);
            
        case SCI_STAT_RX_BREAK:
            return (SciRegs->SciRxStat & SCIRXST_BRKDT);
        
        case SCI_STAT_RECEIVE_ERROR:
            return (SciRegs->SciRxStat & SCIRXST_RxERROR);

        case SCI_STAT_FRAMING_ERROR:
            return (SciRegs->SciRxStat & SCIRXST_FE);

        case SCI_STAT_OVERRUN_ERROR:
            return (SciRegs->SciRxStat & SCIRXST_OE);
            
        case SCI_STAT_PARITY_ERROR:
            return (SciRegs->SciRxStat & SCIRXST_PE);
            
        default: break;            
    }
    return (halSetError(E_PARAMETER_OUT_OF_RANGE));
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
 *      - Reset channel
 * 
 *  \iparam  Handle    = Handle of serial communication channel
 *  \iparam  ControlID = Control type identifier
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halSerialControl (HANDLE_t Handle, SCI_CONTROL_t ControlID) {
    
    volatile SCI_REGISTERS_t* SciRegs = halSerialRegisters(Handle);
    
    if (!isPtrNull(SciRegs)) {
        switch (ControlID) {
        
            case SCI_INTR_RxENABLE:
                SciRegs->SciCtrl1 |= SCICTL1_ERRINTENA;
                SciRegs->SciCtrl2 |= SCICTL2_RxBKINTENA;          
                break;
    
            case SCI_INTR_RxDISABLE:
                SciRegs->SciCtrl1 &= ~SCICTL1_ERRINTENA;
                SciRegs->SciCtrl2 &= ~SCICTL2_RxBKINTENA;          
                break;
    
            case SCI_INTR_TxENABLE:
                SciRegs->SciCtrl2 |= SCICTL2_TxINTENA;
                break;
    
            case SCI_INTR_TxDISABLE:
                SciRegs->SciCtrl2 &= ~SCICTL2_TxINTENA;
                break;
    
            case SCI_CTRL_RESET:
                SciRegs->SciCtrl1 &= ~SCICTL1_SWRESET;          
                SciRegs->SciCtrl1 |=  SCICTL1_SWRESET;          
                break;
        }
        return (NO_ERROR);
    }
    return (halGetError());
}


/*****************************************************************************/
/*! 
 *  \brief   Set mode od serial communication channel 
 *
 *      Sets the data format and bitrate of the serial communication 
 *      interface associated with Handle.
 * 
 *      Format defines the number of data bits, the number of stop bits
 *      and the parity mode (none, even, odd). Baudrate defines the data
 *      bitrate in bits per second. If Format is NULL and/or Baudrate is
 *      zero, that setting is left unchanged. 
 * 
 *  \iparam  Handle   = Handle of serial communication channel
 *  \iparam  Format   = Data format (data bits, stop bits, parity)
 *  \iparam  Baudrate = Data rate (bits/sec)
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halSerialMode (HANDLE_t Handle, SCI_FORMAT_t *Format, UInt16 Baudrate) {

    volatile SCI_REGISTERS_t* SciRegs = halSerialRegisters(Handle);
    UInt16 Control;
    
    if (isPtrNull(SciRegs)) {
       return (halGetError());
    }
    if (Baudrate) {
        UInt32 Divider = 
            SYSTEM_CLOCK / ((UInt32)LoSPEED_CLOCK_DIVIDER * Baudrate * 8) - 1; 
            
        if (Divider < 1 || Divider > MAX_UINT16) {
            return (halSetError(E_PARAMETER_OUT_OF_RANGE));           
        }   
        SciRegs->SciLoBaud = (Divider & 0xFF);
        SciRegs->SciHiBaud = (Divider >> 8) & 0xFF;
    }
    if (!isPtrNull(Format)) {
        if (!Format->Databits || !Format->Stopbits) {
            return (halSetError(E_PARAMETER_OUT_OF_RANGE));
        }
        Control = (Format->Databits - 1) & SCICCR_DATABITS;
        if (Format->Stopbits) {
            Control |= SCICCR_STOPBITS;
        }
        if (Format->Parity) {
            if (Format->Parity == SCI_PARITY_EVEN) {
                Control |= SCICCR_EVEN_PARITY;
            }
            Control |= SCICCR_ENA_PARITY;
        }
        SciRegs->SciCCR = Control;
    }
    return (NO_ERROR);
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
 *  \return  serial communication channel index or (negative) error code
 *
 ****************************************************************************/

static ERROR_t halGetSerialIndex (HANDLE_t Handle, UInt16 Mode) {

    register const Int32 Index = Handle ^ CHANNEL_CLASS_SERIAL;

    if (Index < ELEMENTS(SciData) && Index >= 0) {
        
        register const UInt16 Flags = SciData[Index].Flags;
        
        if (!(Flags & HAL_OPEN_RW)) {
            return (E_HAL_DEVICE_NOT_OPEN);
        }                        
        if (!(Flags & Mode)) {
            return (E_HAL_INVALID_MODE);
        }                        
        return (Index);
    }
    return (E_INVALID_HANDLE);   
}


/*****************************************************************************/
/*! 
 *  \brief   Get serial communication channel register file
 *
 *      Returns the base address of the peripheral register file's base
 *      address for the serial communication interface associated to
 *      with Handle
 * 
 *      In case of an error, NULL is returned
 * 
 *  \iparam  Handle = Handle of open analog channel
 * 
 *  \return  Peripheral register file address (or NULL)
 *
 ****************************************************************************/

static volatile SCI_REGISTERS_t* halSerialRegisters (HANDLE_t Handle) {

    const Int32 Index = halGetSerialIndex(Handle, HAL_OPEN_RW);
    
    if (Index >= 0) {
        return (SciData[Index].SciRegs);
    }
    return (NULL);
}


/*****************************************************************************/
/*! 
 *  \brief   Initialize serial communication module
 *
 *      Initializes the processor's on-chip serial communication interfaces
 *      by setting up the appropriate peripheral registers.
 * 
 *      This function is called only once during startup.
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t halSerialInitialize (void) {
   
    int i;
   
    for (i=0; i < ELEMENTS(SciData); i++) {
    
        volatile SCI_REGISTERS_t *SciRegs = SciData[i].SciRegs;
        
        // initialize all registers to zero
        SciRegs->SciCtrl1 = 0;
        SciRegs->SciCtrl2 = 0; 
        SciRegs->SciCCR   = 0;
        SciRegs->SciPri   = 0;

        // release device from reset state
        SciRegs->SciCtrl1 |= SCICTL1_SWRESET;
    }
    halPeripheralClockEnable (PERIPHERAL_SCI_A, ON);
    halPeripheralClockEnable (PERIPHERAL_SCI_B, ON);
    return (NO_ERROR);
}

//****************************************************************************/

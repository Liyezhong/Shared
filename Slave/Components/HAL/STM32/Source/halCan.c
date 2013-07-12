/****************************************************************************/
/*! \file halCan.c
 *
 *  \brief CAN message read/write functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage CAN controllers.
 *      It supports up to 2 CAN controllers (STM32 connectivity line).
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
#include "halCommon.h"
#include "halClocks.h"
#include "halMain.h"
#include "halInterrupt.h"
#include "halSysTick.h"
#include "halError.h"
#include "halCan.h"


#define CANiD_REMOTE_REQUEST  BIT(31) //!< Remote request indication

//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define FILTER_COUNT    14           //!< filters per CAN controller

// CAN Bit Timing definition (see CAN specification for explanation)
#define CAN_TSEG1       15           //!< Time quantas for bit segment 1
#define CAN_TSEG2       2            //!< Time quantas for bit segment 2
#define CAN_SJW         2            //!< Sync jump width (in quantas)

#define CAN_MCR_RESET   0x00008000   //!< Master reset CAN controller
#define CAN_MCR_TTCM    0x00000080   //!< Time triggered communication mode
#define CAN_MCR_ABOM    0x00000040   //!< Automatic bus-off management
#define CAN_MCR_NART    0x00000020   //!< No automatic retransmission
#define CAN_MCR_AWUM    0x00000010   //!< Automatic wakeup mode
#define CAN_MCR_RFLM    0x00000008   //!< Receive FIFO locked mode
#define CAN_MCR_TXFP    0x00000004   //!< Transmit FIFO priority
#define CAN_MCR_SLEEP   0x00000002   //!< Sleep mode request
#define CAN_MCR_INRQ    0x00000001   //!< Initialization mode request

#define CAN_MSR_SLAKI   0x00000010   //!< Sleep acknowledge interrupt flag
#define CAN_MSR_WKUI    0x00000008   //!< Wakeup interrupt flag
#define CAN_MSR_ERRI    0x00000004   //!< Error interrupt flag
#define CAN_MSR_SLAK    0x00000002   //!< Sleep mode acknowledge
#define CAN_MSR_INAK    0x00000001   //!< Initialization mode acknowledge

#define CAN_BTR_BRP     0x000003FF   //!< Baud rate prescaler bitmask
#define CAN_BTR_TS1     0x000F0000   //!< Time segment 1 bitmask
#define CAN_BTR_TS2     0x00700000   //!< Time segment 2 bitmask
#define CAN_BTR_SJW     0x03000000   //!< Resynchronization jump width
#define CAN_BTR_LBKM    0x40000000   //!< Loopback mode enable (debug)
#define CAN_BTR_SILM    0x80000000   //!< Silent mode enable (debug)

#define CAN_IER_FMPIE1  0x00000010   //!< FIFO message pending irq enable
#define CAN_IER_FMPIE0  0x00000002   //!< FIFO message pending irq enable
#define CAN_IER_FOVIE0  0x00000008   //!< FIFO overrun irq enable
#define CAN_IER_FOVIE1  0x00000040   //!< FIFO overrun irq enable
#define CAN_IER_TMEIE   0x00000001   //!< Transmit mailbox empty irq enable
#define CAN_IER_ERRIE   0x00008000   //!< Error master irq enable
#define CAN_IER_LECIE   0x00000800   //!< Last error code irq enable
#define CAN_IER_BOFIE   0x00000400   //!< Bus-off irq enable
#define CAN_IER_EPVIE   0x00000200   //!< Error passive irq enable
#define CAN_IER_EWGIE   0x00000100   //!< Error warning irq enable

#define CAN_IDR_TXRQ    0x00000001   //!< Transmit request
#define CAN_IDR_RTR     0x00000002   //!< Remote transmission request
#define CAN_IDR_IDE     0x00000004   //!< Identifier extension
#define CAN_IDR_ID      0xFFFFFFF8   //!< Message identifer mask

#define CAN_DTR_TIME    0xFFFF0000   //!< Message time stamp
#define CAN_DTR_FMI     0x0000FF00   //!< Filter match index
#define CAN_DTR_DLC     0x0000000F   //!< Data length code

#define CAN_ESR_REC     0xFF000000   //!< Receive error counter
#define CAN_ESR_TEC     0x00FF0000   //!< Transmit error counter
#define CAN_ESR_LEC     0x00000070   //!< Last bus error code
#define CAN_ESR_BOFF    0x00000004   //!< Bus-off flag
#define CAN_ESR_EPVF    0x00000002   //!< Error passive flag
#define CAN_ESR_EWGF    0x00000001   //!< Error warning flag

#define CAN_RFR_FMP     0x00000003   //!< FIFO messages pending
#define CAN_RFR_FULL    0x00000008   //!< FIFO full
#define CAN_RFR_FOVR    0x00000010   //!< FIFO overrun
#define CAN_RFR_RFOM    0x00000020   //!< Release FIFO output mailbox

#define CAN_TSR_TME     0x1C000000   //!< Transmit mailbox empty
#define CAN_TSR_TME0    0x04000000   //!< Transmit mailbox 0 empty
#define CAN_TSR_TME1    0x08000000   //!< Transmit mailbox 1 empty
#define CAN_TSR_TME2    0x10000000   //!< Transmit mailbox 2 empty
#define CAN_TSR_CODE    0x03000000   //!< Mailbox code
#define CAN_TSR_ABRQ    0x00808080   //!< Abort request for mailbox
#define CAN_TSR_ABRQ0   0x00000080   //!< Abort request for mailbox 0
#define CAN_TSR_ABRQ1   0x00008000   //!< Abort request for mailbox 1
#define CAN_TSR_ABRQ2   0x00800000   //!< Abort request for mailbox 2
#define CAN_TSR_TERR    0x00080808   //!< Transmission error of mailbox
#define CAN_TSR_TERR0   0x00000008   //!< Transmission error of mailbox 0
#define CAN_TSR_TERR1   0x00000800   //!< Transmission error of mailbox 1
#define CAN_TSR_TERR2   0x00080000   //!< Transmission error of mailbox 2
#define CAN_TSR_ALST    0x00040404   //!< Arbitration lost for mailbox
#define CAN_TSR_ALST0   0x00000004   //!< Arbitration lost for mailbox 0
#define CAN_TSR_ALST1   0x00000400   //!< Arbitration lost for mailbox 1
#define CAN_TSR_ALST2   0x00040000   //!< Arbitration lost for mailbox 2

#define CAN_FMR_FINIT   0x00000001   //!< Filter initialization mode
#define CAN_FMR_CAN2SB  0x00003F00   //!< CAN controller 2 start bank

//! CAN bit timing configuration word
#define CAN_BIT_TIMING  \
    ((CAN_TSEG1-1) << 16) | ((CAN_TSEG2-1) << 20) | ((CAN_SJW-1) << 24);


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Register layout of CAN controller's mailboxes
typedef volatile struct {
    UInt32 IDR;                 //!< mailbox message identifier register
    UInt32 DTR;                 //!< mailbox data length control
    UInt32 DLR;                 //!< mailbox data low register
    UInt32 DHR;                 //!< mailbox data high register
} CanMailBox_t;

//! Register layout of CAN controller acceptance filter
typedef volatile struct {
    UInt32 FR1;                 //!< CAN Filter bank register 1
    UInt32 FR2;                 //!< CAN Filter bank register 2
} CanFilters_t;

//! Register layout of processor internal CAN controller
typedef volatile struct {
    UInt32 MCR;                 //!< CAN master control register
    UInt32 MSR;                 //!< CAN master status register
    UInt32 TSR;                 //!< CAN transmit status register
    UInt32 RFR[2];              //!< CAN receive FIFO control/status registers
    UInt32 IER;                 //!< CAN interrupt enable register
    UInt32 ESR;                 //!< CAN error status register
    UInt32 BTR;                 //!< CAN bit timing register
    UInt32 Reserved0[88];       //!< Reserved
    CanMailBox_t TxMailBox[3];  //!< CAN transmit mailbox registers
    CanMailBox_t RxMailBox[2];  //!< CAN Receive fifo registers
    UInt32 Reserved1[12];       //!< Reserved
    UInt32 FMR;                 //!< CAN filter master register
    UInt32 FM1R;                //!< CAN filter mode register
    UInt32 Reserved2;           //!< Reserved
    UInt32 FS1R;                //!< CAN filter scale register
    UInt32 Reserved3;           //!< Reserved
    UInt32 FFA1R;               //!< CAN filter FIFO assignment register
    UInt32 Reserved4;           //!< Reserved
    UInt32 FA1R;                //!< CAN filter activation register
    UInt32 Reserved5[8];        //!< Reserved
    CanFilters_t Filters[28];   //!< CAN Filter bank registers
} CanRegFile_t;

//! Structure to hold the state of a serial communication device
typedef struct {
    CanRegFile_t *CAN;          //!< Pointer to register file
    UInt16 PeripheralID;        //!< Peripheral identifier
    UInt16 InterruptID;         //!< Interrupt identifier
    UInt16 Flags;               //!< Flag bits
} CanDevice_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Data table holding the variables of all supported CAN controllers
static CanDevice_t DataTable[] = {
    { (CanRegFile_t*) 0x40006400, PERIPHERAL_CAN1, INTERRUPT_CAN1_TX },
    { (CanRegFile_t*) 0x40006800, PERIPHERAL_CAN2, INTERRUPT_CAN2_TX }
};

//! Interrupt vector table for all UART devices
static InterruptVector_t CanIntrVectors[ELEMENTS(DataTable)] = {0};


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halCanSetBaudrate (Handle_t Handle, UInt32 Bitrate);
static Error_t halCanFindDevice  (Device_t DeviceID, Bool CheckHalStatus);
static Error_t halCanGetIndex    (Handle_t Handle);


/*****************************************************************************/
/*!
 *  \brief   Open a CAN controller device
 *
 *      Opens a CAN controller device and returns a handle to be used to
 *      read/write from/to that device. DeviceID must be the "name" of a
 *      CAN device defined in the HAL configuration file. The controller
 *      is setup to the baudrate defined in the HAL configuration file.
 *
 *      Handler must be a pointer to an interrupt handler function to be 
 *      called when the device signals an interrupt. It can be NULL if no
 *      interrupts are used. UserTag is a user defined number to be passed 
 *      to the handler function, e.g. as index into a data table in case 
 *      of multiple instances. 
 *
 *      If parameter 'CheckHalStatus' is FALSE, then the CAN device can
 *      be opened even if this module isn't initialized.
 *      For regular use 'CheckHalStatus' should be set to TRUE.
 *
 *  \iparam  DeviceID       = CAN device ID
 *  \iparam  UserTag        = User tag to pass thru to interrupt handler
 *  \iparam  Handler        = Interrupt handler function
 *  \iparam  CheckHalStatus = if true, then HAL is checked to be initialized
 *
 *  \return  Handle or (negative) error code
 *
 ****************************************************************************/

Error_t halCanOpen (Device_t DeviceID, UInt32 UserTag, halIntrHandler_t Handler, Bool CheckHalStatus) {

    const Int32 Index = halCanFindDevice(DeviceID, CheckHalStatus);
    Error_t Status;
    UInt32 i;

    if (Index >= 0) {
        const Int32 UnitNo = halSerialDescriptors[Index].UnitNo;       
        const Int32 Handle = DEVICE_CLASS_CAN + UnitNo;

        if (DataTable[UnitNo].Flags & HAL_OPEN_RW) {
            return (E_DEVICE_ALREADY_OPEN);
        }
        DataTable[UnitNo].Flags |= HAL_OPEN_RW;

        Status = halCanSetBaudrate(Handle, halSerialDescriptors[Index].Baudrate);
        if (Status != NO_ERROR) {
            halCanClose (Handle);
            return (Status);
        }
        CanIntrVectors[UnitNo].Handler = Handler;
        CanIntrVectors[UnitNo].UserTag = UserTag;
    
        if (Handler != NULL) {
            for (i=0; i < 4; i++) {
                halInterruptEnable (
                    DataTable[UnitNo].InterruptID+i, IRQ_PRIO_DEFAULT);
            }
        }    
        return (Handle);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Close CAN controller
 *
 *      Closes an open CAN controller device. All communication is immediately
 *      stopped and the CAN hardware disabled. Data not send will be discarded.
 *
 *  \iparam  Handle = Handle of CAN interface
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halCanClose (Handle_t Handle) {

    const Int32 Index = halCanGetIndex(Handle);
    UInt32 i;

    if (Index >= 0) {
        halCanSetup (Handle, NULL, 0);
        DataTable[Index].CAN->MCR = CAN_MCR_RESET;
        DataTable[Index].CAN->TSR = CAN_TSR_ABRQ | CAN_TSR_TERR | CAN_TSR_ALST;
        DataTable[Index].CAN->IER = 0;

        for (i=0; i < 4; i++) {
            halInterruptDisable (DataTable[Index].InterruptID+i);
        }
        for (i=0; i < 2; i++) {
            DataTable[Index].CAN->RFR[i] |= CAN_RFR_FOVR | CAN_RFR_FULL;
        }
        DataTable[Index].Flags &= ~HAL_OPEN_RW;
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Write message to CAN bus
 *
 *      Writes a message to the CAN controller associated with Handle to send
 *      it via CAN bus. The function is non-blocking; if the message can't be
 *      written because all message buffers are full, it returns immediately.
 *      Returns the number of messages successfully written, which can be 1
 *      if the write succeeds or 0 if the controller is busy. In case of an
 *      error, a negative error code is returned.
 *
 *  \iparam  Handle  = Handle of CAN controller
 *  \iparam  Message = Pointer to message buffer
 *
 *  \return  Number of written messages [0,1] or (negative) error code
 *
 ****************************************************************************/

Error_t halCanWrite (Handle_t Handle, CanMessage_t *Message) {

    const Int32 Index = halCanGetIndex(Handle);
    CanMailBox_t *Mailbox;
    UInt32 FreeBox;
    UInt32 i;

    if (Index < 0) {
        return (Index);
    }
    if (Message == NULL || Message->Length > 8) {
        return (E_CAN_INVALID_MESSAGE);
    }
    if (DataTable[Index].CAN->TSR & CAN_TSR_TME) {

        FreeBox = (DataTable[Index].CAN->TSR & CAN_TSR_CODE) >> 24;
        Mailbox = &DataTable[Index].CAN->TxMailBox[FreeBox];

        Mailbox->IDR = ((Message->CanID << 3) & CAN_IDR_ID) | CAN_IDR_IDE;

        if (Message->CanID & CANiD_REMOTE_REQUEST) {
            Mailbox->IDR |= CAN_IDR_RTR;
        }
        Mailbox->DLR = Mailbox->DHR = 0;

        for (i=0; i < Message->Length; i++) {
            if (i <= 3) {
                Mailbox->DLR |= Message->Data[i] << (i * 8);
            }
            else {
                Mailbox->DHR |= Message->Data[i] << ((i & 3) * 8);
            }
        }
        Mailbox->DTR = Message->Length & CAN_DTR_DLC;
        Mailbox->IDR |= CAN_IDR_TXRQ;
        return (1);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Read message from CAN bus
 *
 *      Reads next message received from the CAN controller associated
 *      with Handle and returns it in the buffer pointed to by Message.
 *      Exactly one message is read and returned, as far as an unread
 *      message is available. If no new receive message is available,
 *      the function terminates immediately without returning any data.
 *      The return parameter indicates the number of returned messages
 *      and can be 0 or 1. In case of an error, a negative error code
 *      is returned.
 *
 *  \iparam  Handle  = Handle of CAN controller
 *  \iparam  Message = Pointer to message buffer
 *
 *  \return  Number of read messages [0,1] or (negative) error code
 *
 ****************************************************************************/

Error_t halCanRead (Handle_t Handle, CanMessage_t *Message) {

    const Int32 Index = halCanGetIndex(Handle);
    UInt32 FifoNo;
    UInt32 i;

    if (Index < 0) {
        return (Index);
    }
    if (Message == NULL) {
        return (E_CAN_INVALID_PARAMETER);
    }
    for (FifoNo = 0; FifoNo < 2; FifoNo++) {

        if (DataTable[Index].CAN->RFR[FifoNo] & CAN_RFR_FMP) {
            CanMailBox_t *Mailbox = &DataTable[Index].CAN->RxMailBox[FifoNo];

            Message->CanID = (Mailbox->IDR & CAN_IDR_ID) >> 3;
            if (Mailbox->IDR & CAN_IDR_RTR) {
                Message->CanID |= CANiD_REMOTE_REQUEST;
            }
            Message->Length = MIN (Mailbox->DTR & CAN_DTR_DLC, 8);

            for (i = 0; i < 8; i++) {
                if (i >= 4) {
                    Message->Data[i] = (Mailbox->DHR >> ((i & 3) * 8));
                }
                else {
                    Message->Data[i] = (Mailbox->DLR >> (i * 8));
                }
            }
            //Message->Timestamp = (Mailbox->DTR & CAN_DTR_TIME) >> 16;
            Message->Timestamp = halSysTickRead();

            // Release message in FIFO
            DataTable[Index].CAN->RFR[FifoNo] |= CAN_RFR_RFOM;
            return (1);
        }
    }
    // Check for FIFO overrun; return error in case of
    if ((DataTable[Index].CAN->RFR[0] & CAN_RFR_FOVR) ||
        (DataTable[Index].CAN->RFR[1] & CAN_RFR_FOVR)) {

        DataTable[Index].CAN->RFR[0] &= ~CAN_RFR_FOVR;
        DataTable[Index].CAN->RFR[1] &= ~CAN_RFR_FOVR;
        return (E_CAN_RECEIVER_OVERRUN);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Get status of CAN controller
 *
 *      Returns the status of the CAN controller associated with Handle.
 *      What status to return depends on the supplied StatusID. The
 *      following informations can be requested:
 *
 *      - Receiver messages available (no. of messages)
 *      - Transmitter ready status (mailbox free)
 *      - Bus status (active, passive, bus-off)
 *      - Transmit error counter
 *      - Receive error counter
 *      - Last CAN bus error code
 *
 *      If a unsupported StatusID is supplied, an error is returned.
 *
 *  \iparam  Handle   = Handle of CAN interface
 *  \iparam  StatusID = Type of status
 *
 *  \return  Status value or (negative) error code
 *
 ****************************************************************************/

Error_t halCanStatus (Handle_t Handle, CanStatID_t StatusID) {

    const Int32 Index = halCanGetIndex(Handle);

    if (Index >= 0) {
        CanRegFile_t *CAN = DataTable[Index].CAN;

        switch (StatusID) {

            case CAN_STAT_RxCOUNT:
                return ((CAN->RFR[0] & CAN_RFR_FMP) + (CAN->RFR[1] & CAN_RFR_FMP));

            case CAN_STAT_TxREADY:
                return (CAN->TSR & CAN_TSR_TME);

            case CAN_STAT_TxEMPTY:
                return !(~CAN->TSR & CAN_TSR_TME);

            case CAN_STAT_RxERRORS:
                return ((CAN->ESR & CAN_ESR_REC) >> 24);

            case CAN_STAT_TxERRORS:
                return ((CAN->ESR & CAN_ESR_TEC) >> 16);

            case CAN_STAT_BUS_ERROR:
                return ((CAN->ESR & CAN_ESR_LEC) >> 4);

            case CAN_STAT_BUS_STATE:
                if (CAN->ESR & CAN_ESR_BOFF) {
                    return (CAN_STATE_BUSOFF);
                }
                if (CAN->ESR & CAN_ESR_EPVF) {
                    return (CAN_STATE_PASSIVE);
                }
                if (CAN->ESR & CAN_ESR_EWGF) {
                    return (CAN_STATE_WARNING);
                }
                return (CAN_STATE_ACTIVE);
        }
        return (E_UNKNOWN_STATUS_ID);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Control CAN controller interrupts
 *
 *      Enable/disable CAN controller interrupts. ControlID determines which
 *      interrupt to enable/disable. The following interrupts are available:
 *
 *      - Receive message available
 *      - Transmit mailbox empty
 *      - Bus-off state entered
 *      - Passive state entered
 *      - Warning state entered
 *      - Receiver overrun error
 *      - CAN error detected
 *
 *      State determines if to enable (TRUE) or disable (FALSE) the 
 *      selected interrupt.
 *
 *  \iparam  Handle    = Handle of CAN controller
 *  \iparam  ControlID = Type of control
 *  \iparam  State     = Enable: True, Disable: False
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halCanControl (Handle_t Handle, CanCtrlID_t ControlID, Bool State) {

    const Int32 Index = halCanGetIndex(Handle);
    UInt32 EnableBitmask;

    if (Index >= 0) {
        switch (ControlID) {

            case CAN_INTR_RxREADY:
                EnableBitmask = CAN_IER_FMPIE1 | CAN_IER_FMPIE0;
                break;

            case CAN_INTR_TxREADY:
                EnableBitmask = CAN_IER_TMEIE;
                break;

            case CAN_INTR_ERROR:
                EnableBitmask = CAN_IER_LECIE | CAN_IER_ERRIE;
                break;

            case CAN_INTR_BUSOFF:
                EnableBitmask = CAN_IER_BOFIE | CAN_IER_ERRIE;
                break;

            case CAN_INTR_PASSIVE:
                EnableBitmask = CAN_IER_EPVIE | CAN_IER_ERRIE;
                break;

            case CAN_INTR_WARNING:
                EnableBitmask = CAN_IER_EWGIE | CAN_IER_ERRIE;
                break;

            case CAN_INTR_OVERRUN:
                EnableBitmask = CAN_IER_FOVIE1 | CAN_IER_FOVIE0;
                break;

            default:
                return (E_UNKNOWN_CONTROL_ID);
        }
        if (!State) {
            DataTable[Index].CAN->IER &= ~(EnableBitmask & ~CAN_IER_ERRIE);
        }
        else {
            DataTable[Index].CAN->IER |= EnableBitmask;
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Setup CAN acceptance filters
 *
 *      Configures CAN acceptance filters for the CAN controller associated
 *      with Handle. Filters must be a table of acceptance filter settings,
 *      Count must contain the number of entries in that table. Filters may
 *      be NULL and Count be zero to remove all actual active filters.
 *      Each filter item must contain a CAN ID and a bitmask. Incoming
 *      messages will be checked by 1st masking the CAN ID with the filter
 *      mask, and 2nd comparing the CAN ID of the message with the CAN ID
 *      of the filter. All bits that are 0 in the filter mask are don't
 *      care during comparision.
 *
 *  \iparam  Handle  = Handle of CAN controller
 *  \iparam  Filters = Acceptance filter table
 *  \iparam  Count   = Number of entries in filter table
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halCanSetup (Handle_t Handle, CanIdFilter_t *Filters, UInt16 Count) {

    const Int32 Index = halCanGetIndex(Handle);
    UInt32 Offset;
    UInt32 i = 0;

    if (Index >= 0) {
        if (Count >= FILTER_COUNT) {
            return (E_CAN_FILTER_COUNT);
        }
        if (Filters == NULL && Count > 0) {
            return (E_CAN_INVALID_PARAMETER);
        }
        Offset = Index ? FILTER_COUNT : 0;

        // program and activate specified filters
        if (Filters != NULL) {
            for (i=0; i < Count; i++) {
    
                DataTable[0].CAN->FA1R &= ~BIT(Offset);
                DataTable[0].CAN->Filters[Offset].FR1 = Filters[i].CanID << 3;
                DataTable[0].CAN->Filters[Offset].FR2 = Filters[i].Mask  << 3;
                DataTable[0].CAN->FA1R |= BIT(Offset);
                Offset++;
            }
        }
        // deactivate all unused filters
        for (; i < FILTER_COUNT; i++) {
            DataTable[0].CAN->FA1R &= ~BIT(Offset++);
        }
        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   Setup CAN bus data rate
 *
 *      Configures the CAN controller to the given Baudrate and sets up
 *      the bit timing accordingly. If the requested baudrate can not be
 *      programmed or derives more than 1%, an error is returned.
 *
 *  \iparam  Handle   = Handle of CAN controller
 *  \iparam  Baudrate = CAN bus bitrate
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halCanSetBaudrate (Handle_t Handle, UInt32 Baudrate) {

    const UInt32 Clockrate = halGetPeripheralClockRate();
    const Int32 Index = halCanGetIndex(Handle);
    UInt32 RealRate;
    UInt32 Division;

    if (Index >= 0) {
        if (!Baudrate) {
            return (E_CAN_INVALID_BITRATE);
        }
        // Calculate and check clock divider
        Division = Clockrate / Baudrate / (CAN_TSEG1 + CAN_TSEG2 + 1);
        if (Division < 1 || Division > CAN_BTR_BRP) {
            return (E_CAN_INVALID_BITRATE);
        }
        // Check if resulting bitrate deviates more than 1% from requested one
        RealRate = Clockrate / (CAN_TSEG1 + CAN_TSEG2 + 1) / Division;
        if ((UInt32) abs (RealRate - Baudrate) > Baudrate / 100) {
            return (E_CAN_INVALID_BITRATE);
        }
        // Enter initialization mode
        DataTable[Index].CAN->MCR = CAN_MCR_INRQ;
        while (~DataTable[Index].CAN->MSR & CAN_MSR_INAK) {}

        // Setup master control register
        DataTable[Index].CAN->MCR =
            CAN_MCR_ABOM | CAN_MCR_RFLM | CAN_MCR_INRQ | CAN_MCR_TTCM;

        // Setup CAN bit timing register
        DataTable[Index].CAN->BTR = ((Division - 1) & CAN_BTR_BRP) | CAN_BIT_TIMING;
        //DataTable[Index].CAN->BTR |= CAN_BTR_LBKM ;//| CAN_BTR_SILM;

        // Exit initialization mode
        DataTable[Index].CAN->MCR &= ~CAN_MCR_INRQ;
        while (DataTable[Index].CAN->MSR & CAN_MSR_INAK) {}

        return (NO_ERROR);
    }
    return (Index);
}


/*****************************************************************************/
/*!
 *  \brief   CAN master interrupt handler
 *
 *      This function will be called by the vectored interrupt controller in
 *      case of a CAN interrupt. Channel is the number of the CAN channel.
 *      It calls the user handler registered with halCanRegisterHandler(),
 *      passing the interrupt flag bits and the user tag to it. Before
 *      exiting, the handled interrupt flag bits are cleared.
 *
 *  \iparam  Channel   = CAN device number 
 *  \iparam  IntrFlags = Interrupt flag bits
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void halCanInterruptHandler (UInt32 Channel, UInt32 IntrFlags) {

    if (Channel < ELEMENTS(DataTable)) {
        if (CanIntrVectors[Channel].Handler != NULL) {
            CanIntrVectors[Channel].Handler (CanIntrVectors[Channel].UserTag, IntrFlags);
        }
        if (IntrFlags & CAN_IFLAG_SYSTEM) {
            DataTable[Channel].CAN->MSR |= CAN_MSR_ERRI;
        }
    }
}


/*****************************************************************************/
/*!
 *  \brief   Find logical CAN controller
 *
 *      Searches the serial device descriptor for an entry with the asked
 *      DeviceID. If found, the index into the descrptor table is returned.
 *      If the requested hardware doesn't exist or the device isn't initialized,
 *      an error is returned.
 *
 *      If parameter 'CheckHalStatus' is FALSE, then the CAN device can
 *      be accessed even if this module isn't initialized.
 *      For regular use 'CheckHalStatus' should be set to TRUE.
 *
 *  \iparam  DeviceID       = Logical CAN device ID
 *  \iparam  CheckHalStatus = if true, then HAL is checked to be initialized
 *
 *  \return  Descriptor table index or (negative) error code
 *
 ****************************************************************************/

static Error_t halCanFindDevice (Device_t DeviceID, Bool CheckHalStatus) {

    UInt32 Index;

    if ((DeviceID & DEVICE_CLASS_MASK) == DEVICE_CLASS_CAN) {

        halInitState_t InitState = halGetInitState();

        if (CheckHalStatus) {
            if (InitState == INIT_NOT_DONE || InitState == INIT_FAILED) {
                return (E_HAL_NOT_INITIALIZED);
            }
        }
        for (Index=0; Index < halSerialDescriptorCount; Index++) {

            if (halSerialDescriptors[Index].DeviceID == DeviceID) {

                const UInt32 UnitNo = halSerialDescriptors[Index].UnitNo;

                if (UnitNo >= halProcessorInfo.CountCAN ||
                    UnitNo >= ELEMENTS(DataTable)) {
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
 *  \brief   Get index of a CAN controller
 *
 *      Returns the index of the CAN controller associated with Handle. The
 *      index is used to address an entry in the Devices table, which contains
 *      all variables of that controller. If Handle is invalid or the device
 *      not opened, an error is returned.
 *
 *  \iparam  Handle = Handle of CAN controller
 *
 *  \return  CAN controller index or (negative) error code
 *
 ****************************************************************************/

static Error_t halCanGetIndex (Handle_t Handle) {

    const UInt32 Index = Handle ^ DEVICE_CLASS_CAN;

    if (Index < ELEMENTS(DataTable)) {

        if (!(DataTable[Index].Flags & HAL_OPEN_RW)) {
            return (E_DEVICE_NOT_OPEN);
        }
        return ((Error_t)Index);
    }
    return (E_DEVICE_HANDLE_INVALID);
}


/*****************************************************************************/
/*!
 *  \brief   Initialize CAN controllers
 *
 *      Initializes the processor's on-chip CAN controllers. Up to two CAN
 *      controllers are supported. In case of 2 controllers, the acceptance
 *      filter registers are shared and are to be programmed in the 1st CAN
 *      controller's register file.
 *
 *      Enables the peripheral clocks, resets the controller, and programs
 *      the general behaivor of the acceptance filters.
 *
 *      This function is called only once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halCanInit (void) {

    UInt32 SupportedCANs = MIN(halProcessorInfo.CountCAN, ELEMENTS(DataTable));
    UInt32 i, k;

    if (halGetInitState() == INIT_IN_PROGRESS) {
    
        for (i=0; i < halSerialDescriptorCount; i++) {
            const UInt32 UnitNo = halSerialDescriptors[i].UnitNo;
    
            if (halSerialDescriptors[i].BusType == SIO_TYPE_CAN) {

                // Check if port number is legal
                if (UnitNo >= SupportedCANs) {
                    return (E_PORT_NOT_EXISTS);
                }
                // Check if interface already initialized
                if (DataTable[UnitNo].Flags & HAL_FLAG_INITZED) {
                    return (E_BUS_ALLOCATED_TWICE);
                }
                // Enable peripheral clocks and reset CAN device
                halPeripheralClockEnable (DataTable[UnitNo].PeripheralID, ON);
                DataTable[UnitNo].CAN->MCR = CAN_MCR_RESET;

                // Check if bit timing parameters are acceptable
                if (CAN_TSEG1 > 16 || CAN_TSEG2 > 8 || CAN_SJW > 4) {   //lint !e506
                    return (E_CAN_INVALID_SEGTIME);
                }
                // Enter filter initialization mode
                DataTable[0].CAN->FMR = CAN_FMR_FINIT;
            
                // Set all filters to 32 bit identifier/mask mode
                for (k=0; k < FILTER_COUNT * 2; k++) {
                    const UInt32 Bitmask = BIT(k);
            
                    DataTable[0].CAN->FM1R &= ~Bitmask;
                    DataTable[0].CAN->FS1R |=  Bitmask;
                    if (k & 1) {
                        DataTable[0].CAN->FFA1R |= Bitmask;
                    }
                    else {
                        DataTable[0].CAN->FFA1R &= ~Bitmask;
                    }
                    // Disable filter
                    DataTable[0].CAN->FA1R &= ~Bitmask;
                }
                // Enter active filter mode
                DataTable[0].CAN->FMR |=  CAN_FMR_CAN2SB & (FILTER_COUNT << 8);
                DataTable[0].CAN->FMR &= ~CAN_FMR_FINIT;

                DataTable[UnitNo].Flags = HAL_FLAG_INITZED;
            }
        }
    }    
    return (NO_ERROR);
}

//****************************************************************************/

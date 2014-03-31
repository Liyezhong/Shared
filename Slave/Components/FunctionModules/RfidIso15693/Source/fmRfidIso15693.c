/****************************************************************************/
/*! \file fmRfidIso15693.c
 *
 *  \brief Functional module controlling RFID ISO 15693 communication.
 *
 *   $Version: $ 0.2
 *   $Date:    $ 23.03.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This module contains the top level source code of the RFID ISO/IEC
 *      15693 communication controller. The code in this file implements
 *      functions, that are needed to communicate with RFID transponders
 *      implementing the ISO/IEC 15693 standard. The module controls the EM
 *      Microelectronic EM4094 RFID front end IC via the Hardware Abstraction
 *      Layer (HAL). For this purpose, the module uses digital I/O ports and a
 *      timer unit to control the time critical communication procedure
 *      through the generation of interrupts.
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
#include "Basemodule.h"
#include "fmRfidIso15693.h"
#include "fmRfidIso15693Link.h"

//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

/* Global defines of the RFID module */
#define MODULE_VERSION      0x0002  //!< Version number of module

/* Mode bits for the Flags member of the module instance data */
#define MODE_MODULE_ENABLE  0x1     //!< Module on/off bit

#define RFID15693_INIT_TIME 10      //!< Time between compare interrupts in microseconds
#define RFID15693_COMPARE_TIME 128  //!< Time between compare interrupts in 13.56 Mhz cycles

#define RFID15693_CAPTURE_CHANNEL 2 //!< Timer channel used for the capture input
#define RFID15693_COMPARE_CHANNEL 3 //!< Timer channel used for the compare output

#define RFID15693_TIMEOUT 15        //!< Timeout for an RFID transaction
#define RFID15693_INIT_TIMEOUT 333  //!< Timeout after the initialization

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

/*! State of the front end initialization */
typedef enum {
    STATE_UNINIT,
    STATE_HIGH_DIN,
    STATE_LOW_DIN,
    STATE_LOW_DCLK,
    STATE_HIGH_DCLK,
    STATE_ACK,
    STATE_WAIT,
    STATE_INIT
} InitState_t;

/*! Contains all variables for a instance of this module */
typedef struct {
    bmModuleState_t ModuleState; //!< Module state
    Handle_t HandleEn;           //!< Handle to access a digital output port (HAL)
    Handle_t HandleOut;          //!< Handle to access a digital output port (HAL)
    Handle_t HandleTimer;        //!< Handle to access a hardware timer (HAL)
    UInt16 Channel;              //!< Logical CAN channel
    UInt16 Flags;                //!< Mode control flag bits

    UInt32 OptionBits;           //!< Option bits of the EM Microelectronic 4094 front end
    UInt64 UniqueId;             //!< The current unique ID used for RFID transactions

    InitState_t InitState;       //!< State of the initialization procedure of the front end
    Error_t IrqError;            //!< Stores the code of an error occurring in an interrupt handler
    UInt8 BitCount;              //!< Counting the number of bits already transferred
    UInt32 TimeMask;             //!< The size of the timer register
    UInt32 OldCount;             //!< Old counter value from the capture unit
    UInt32 StartTime;            //!< Time the transaction is started in ms
    Rfid15693Stream_t Stream;    //!< Data structure containing information of an RFID transaction
} InstanceData_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

static UInt16 InstanceCount = 0;    //!< number of module instances
static UInt16 ModuleIdentifier = 0; //!< module identifier

static InstanceData_t* DataTable;   //!< data table for all instances

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t rfid15693ModuleControl (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t rfid15693ModuleStatus  (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t rfid15693ModuleTask    (UInt16 Instance);

static Error_t rfid15693OptionBitsInit (InstanceData_t* Data);
static Error_t rfid15693StartCompare (InstanceData_t *Data);
static Error_t rfid15693SendResponse (InstanceData_t* Data);
static Error_t rfid15693Complete (InstanceData_t* Data);

static Error_t rfid15693SetConfig (UInt16 Channel, CanMessage_t* Message);
static Error_t rfid15693AcquireUid (UInt16 Channel, CanMessage_t* Message);
static Error_t rfid15693SetUid (UInt16 Channel, CanMessage_t* Message);
static Error_t rfid15693GetUid (UInt16 Channel, CanMessage_t* Message);
static Error_t rfid15693ReadBlock (UInt16 Channel, CanMessage_t* Message);
static Error_t rfid15693WriteBlock (UInt16 Channel, CanMessage_t* Message);
static Error_t rfid15693LockBlock (UInt16 Channel, CanMessage_t* Message);

static void rfid15693InterruptHandler (UInt32 Channel, UInt32 IntrFlags);
static Error_t rfid15693InitIntHandler (InstanceData_t *Data);
static Error_t rfid15693CompareIntHandler (InstanceData_t *Data);
static Error_t rfid15693CaptureIntHandler (InstanceData_t *Data);

static Error_t rfid15693HandleOpen (InstanceData_t *Data, Int16 Instance);

/*****************************************************************************/
/*!
 *  \brief   Module Control Function
 *
 *      This function is called by the base module to control this function
 *      module. Depending on the ControlID parameter, the following actions
 *      are performed:
 *
 *      - MODULE_CONTROL_RESUME
 *      - MODULE_CONTROL_WAKEUP
 *      - MODULE_CONTROL_STOP
 *      - MODULE_CONTROL_SHUTDOWN
 *      - MODULE_CONTROL_RESET
 *      - MODULE_CONTROL_FLUSH_DATA
 *      - MODULE_CONTROL_RESET_DATA
 *
 *  \iparam  Instance  = Instance number of this module [in]
 *  \iparam  ControlID = Control code to select sub-function [in]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693ModuleControl (UInt16 Instance, bmModuleControlID_t ControlID)
{
    InstanceData_t* Data = &DataTable[Instance];

    switch (ControlID)
    {
        case MODULE_CONTROL_RESUME:
        case MODULE_CONTROL_WAKEUP:
            Data->ModuleState = MODULE_STATE_READY;
            if ((Data->Flags & MODE_MODULE_ENABLE) != 0) {
                return halPortWrite(Data->HandleEn, 1);
            }
            break;

        case MODULE_CONTROL_STOP:
            Data->ModuleState = MODULE_STATE_STOPPED;
            return halPortWrite(Data->HandleEn, 0);

        case MODULE_CONTROL_SHUTDOWN:
            Data->ModuleState = MODULE_STATE_STANDBY;
            return halPortWrite(Data->HandleEn, 0);

        case MODULE_CONTROL_RESET:
            Data->Flags &= ~MODE_MODULE_ENABLE;
            return halPortWrite(Data->HandleEn, 0);

        case MODULE_CONTROL_FLUSH_DATA:
        case MODULE_CONTROL_RESET_DATA:
            break;

        default:
            return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Module Status Request
 *
 *      This function is called by the base module to request the status of
 *      this function module. Depending on the StatusID parameter, the
 *      following status values are returned:
 *
 *      - MODULE_STATUS_STATE
 *      - MODULE_STATUS_VALUE
 *      - MODULE_STATUS_MODULE_ID
 *      - MODULE_STATUS_INSTANCES
 *      - MODULE_STATUS_VERSION
 *
 *  \iparam  Instance = Instance number of this module
 *  \iparam  StatusID = selects which status is requested
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693ModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID)
{
    InstanceData_t* Data = &DataTable[Instance];

    switch (StatusID)
    {
        case MODULE_STATUS_STATE:
            if ((Data->Flags & MODE_MODULE_ENABLE) == 0) {
                return ((Error_t) MODULE_STATE_DISABLED);
            }
            return ((Error_t) Data->ModuleState);

        case MODULE_STATUS_VALUE:
            return (Data->OptionBits);

        case MODULE_STATUS_MODULE_ID:
            return (ModuleIdentifier);

        case MODULE_STATUS_INSTANCES:
            return (InstanceCount);

        case MODULE_STATUS_VERSION:
            return (MODULE_VERSION);

        default: break;
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief   Module Task Function
 *
 *      This function is called by the task scheduler periodically.
 *      It's purpose is to perform all actions to provide the modules
 *      functionality.
 *
 *  \iparam  Instance = Instance number of this module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

static Error_t rfid15693ModuleTask (UInt16 Instance)
{
    Error_t Error;
    InstanceData_t* Data = &DataTable[Instance];

    // Control the option bits programming sequence
    Error = rfid15693OptionBitsInit (Data);
    if (Error < NO_ERROR) {
        bmSignalEvent (Data->Channel, Error, TRUE, 0);
        return ((Error_t) Data->ModuleState);
    }

    if (Data->ModuleState == MODULE_STATE_BUSY && Data->InitState == STATE_INIT) {
        Error = rfid15693Complete(Data);
        // Check for possible errors from the interrupt handlers
        if (Error < NO_ERROR) {
            Data->ModuleState = MODULE_STATE_READY;
            bmSignalEvent (Data->Channel, Error, TRUE, Data->Stream.ReceiveState);
            Data->IrqError = NO_ERROR;
            return ((Error_t) Data->ModuleState);
        }
        // Check for transaction completeness
        else if (Error == 1) {
            Data->ModuleState = MODULE_STATE_READY;
            Error = rfid15693LinkCheckMessage(&Data->Stream);
            if (Error < NO_ERROR) {
                UInt8 ErrorCode = (Error == E_RFID15693_RESPONSE_ERRORFLAG) ?
                        Data->Stream.RxBuffer[1] : Data->Stream.RxCount;
                bmSignalEvent (Data->Channel, Error, TRUE, ErrorCode);
                return ((Error_t) Data->ModuleState);
            }
            else {
                Error = rfid15693SendResponse(Data);
                if (Error < NO_ERROR) {
                    bmSignalEvent (Data->Channel, Error, TRUE, 0);
                    return ((Error_t) Data->ModuleState);
                }
            }
        }
    }

    return ((Error_t) Data->ModuleState);
}


/*****************************************************************************/
/*!
 *  \brief   Controlling option bits programming
 *
 *      This function starts the programming of the option bits of the RFID
 *      front end IC. The programming itself is performed through an interrupt
 *      handler which emulates an SPI like serial interface. The method also
 *      checks, if the transmission of the option bits already finished. If
 *      this is the case, an acknowledgement message is transmitted to the
 *      master.
 *
 *  \xparam  Data = Contains all module instance variables
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

static Error_t rfid15693OptionBitsInit (InstanceData_t* Data)
{
    Error_t Error;

    if ((Data->Flags & MODE_MODULE_ENABLE) != 0 && Data->ModuleState == MODULE_STATE_BUSY) {
        if (Data->InitState == STATE_UNINIT) {
            TimerMode_t TimerMode;

            TimerMode.Direction = TIM_MODE_COUNT_UP;
            TimerMode.OneShot = TIM_MODE_INTERVAL;
            TimerMode.ClkSource = TIM_MODE_INTERNAL;
            TimerMode.ClkMode = 0;

            Error = halTimerStatus (Data->HandleTimer, TIM_STAT_CLOCKRATE);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Error = halTimerSetup (Data->HandleTimer, &TimerMode, Error / 1000000);
            if (Error < NO_ERROR) {
                return (Error);
            }
            // Start the timer
            Error = halTimerWrite (Data->HandleTimer, TIM_REG_RELOAD, RFID15693_INIT_TIME);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Error = halTimerControl (Data->HandleTimer, TIM_CTRL_RELOAD);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Error = halTimerControl (Data->HandleTimer, TIM_INTR_ENABLE);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Error = halTimerControl (Data->HandleTimer, TIM_CTRL_START);
            if (Error < NO_ERROR) {
                return (Error);
            }
            // Set the output ports
            Error = halPortWrite (Data->HandleOut, 2);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Data->IrqError = NO_ERROR;
            Data->InitState = STATE_HIGH_DIN;
        }
        else if (Data->InitState == STATE_ACK) {
            TimerMode_t TimerMode;

            TimerMode.Direction = TIM_MODE_COUNT_UP;
            TimerMode.OneShot = TIM_MODE_INTERVAL;
            TimerMode.ClkSource = TIM_MODE_EXTERNAL;
            TimerMode.ClkMode = TIM_MODE_ENCODER_3 | TIM_MODE_ENCODER_2 | TIM_MODE_ENCODER_1;

            Error = halTimerSetup (Data->HandleTimer, &TimerMode, 1);
            if (Error < NO_ERROR) {
                return (Error);
            }

            Data->StartTime = bmGetTime();
            Data->InitState = STATE_WAIT;
        }
        else if (Data->InitState == STATE_WAIT) {
            if (bmTimeExpired(Data->StartTime) > RFID15693_INIT_TIMEOUT) {
                Data->ModuleState = MODULE_STATE_READY;
                Data->InitState = STATE_INIT;
                bmSignalEvent (Data->Channel, E_RFID15693_ACKNOWLEDGEMENT, TRUE, 0);
            }
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Starts the timer compare process
 *
 *      This function starts the compare interrupt handler that is responsible
 *      for the transmission of data to the RFID front end IC. To do so, the
 *      method enables the required compare interrupt and programs the time of
 *      the next compare event into a specified timer unit.
 *
 *  \xparam  Data = Contains all module instance variables
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

static Error_t rfid15693StartCompare (InstanceData_t* Data)
{
    Error_t Error;

    Data->StartTime = bmGetTime ();

    Error = halTimerWrite (Data->HandleTimer, TIM_REG_RELOAD, RFID15693_COMPARE_TIME);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Error = halTimerControl (Data->HandleTimer, TIM_CTRL_RELOAD);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Error = halTimerControl (Data->HandleTimer, TIM_INTR_ENABLE);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Error = halTimerControl (Data->HandleTimer, TIM_CTRL_START);
    if (Error < NO_ERROR) {
        return (Error);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Sends a response message to the master
 *
 *      This function sends back a response message to the master, when a RFID
 *      response message was successfully received from the tag.
 *
 *  \xparam  Data = Contains all module instance variables
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

static Error_t rfid15693SendResponse(InstanceData_t* Data)
{
    CanMessage_t Message;

    switch(Data->Stream.TxBuffer[1]) {
        case RFID15693_LINK_CMD_INVENTORY:
            Data->UniqueId = rfid15693GetUniqueId (&Data->Stream);
            Message.CanID = MSG_RFID15693_RESP_ACQUIRE_UID;
            bmSetMessageItem (&Message, Data->UniqueId >> 32, 0, 4);
            bmSetMessageItem (&Message, Data->UniqueId, 4, 4);
            Message.Length = 8;
            break;
        case RFID15693_LINK_CMD_SINGLE_READ:
            Message.CanID = MSG_RFID15693_RESP_READ_BLOCK;
            bmSetMessageItem (&Message, Data->Stream.TxBuffer[10], 0, 1);
            bmSetMessageItem (&Message, rfid15693GetDataBlock(&Data->Stream), 1, 4);
            Message.Length = 5;
            break;
        case RFID15693_LINK_CMD_SINGLE_WRITE:
            Message.CanID = MSG_RFID15693_RESP_WRITE_BLOCK;
            bmSetMessageItem (&Message, Data->Stream.TxBuffer[10], 0, 1);
            Message.Length = 1;
            break;
        case RFID15693_LINK_CMD_BLOCK_LOCK:
            Message.CanID = MSG_RFID15693_RESP_LOCK_BLOCK;
            bmSetMessageItem (&Message, Data->Stream.TxBuffer[10], 0, 1);
            Message.Length = 1;
            break;
        default:
            return (E_PARAMETER_OUT_OF_RANGE);
    }

    return (canWriteMessage (Data->Channel, &Message));
}


/*****************************************************************************/
/*!
 *  \brief   Checks an RFID transaction for completion
 *
 *      This method checks if an RFID transaction has completed successfully.
 *      If this is the case, it will return TRUE.
 *
 *  \iparam  Data = This structure contains the transaction state
 *
 *  \return  1 when transaction complete, NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693Complete (InstanceData_t* Data)
{
    // Check for possible errors from the interrupt handlers
    if (Data->IrqError < NO_ERROR) {
        return (Data->IrqError);
    }
    // Check for transaction completeness
    else if (Data->Stream.ReceiveState == LINK_RXSTATE_EOF) {
        return (1);
    }
    // Check for a transaction timeout
    else if (bmTimeExpired(Data->StartTime) > RFID15693_TIMEOUT) {
        Error_t Error = halCapComControl (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, TIM_INTR_DISABLE);
        if (Error < NO_ERROR) {
            return (Error);
        }
        Error = halCapComControl (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, TIM_CTRL_STOP);
        if (Error < 0) {
            return (Error);
        }
        return (E_RFID15693_TRANSACTION_TIMEOUT);
    }

    return 0;
}

/*****************************************************************************/
/*!
 *  \brief  Activates or deactivates the module
 *
 *      This function is called by the CAN message dispatcher when a message
 *      enabling or disabling this function module is received from the
 *      master. The parameters in the message are transfered to the data
 *      structure of the addressed module instance. The modified settings
 *      influence the behavior of the module task. The following settings will
 *      be modified:
 *
 *      - Module enable bit
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693SetConfig (UInt16 Channel, CanMessage_t* Message)
{
    InstanceData_t* Data;
    Error_t Instance = bmGetInstance(Channel);

    if (Instance < NO_ERROR) {
        return (Instance);
    }
    else if (Instance >= InstanceCount) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    Data = &DataTable[Instance];

    if (Message->Length == 1) {
        if (Data->ModuleState == MODULE_STATE_READY) {
            Data->Flags = bmGetMessageItem(Message, 0, 1);
            if ((Data->Flags & MODE_MODULE_ENABLE) == 0) {
                Data->InitState = STATE_UNINIT;
                bmSignalEvent (Data->Channel, E_RFID15693_ACKNOWLEDGEMENT, TRUE, 0);
            }
            else if (Data->InitState != STATE_INIT) {
                Data->ModuleState = MODULE_STATE_BUSY;
            }
            else {
                bmSignalEvent (Data->Channel, E_RFID15693_ACKNOWLEDGEMENT, TRUE, 0);
            }
            return (halPortWrite (Data->HandleEn, Data->Flags));
        }
        return (E_RFID15693_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Issues an inventory message returning the UID
 *
 *      This function is called by the CAN message dispatcher when a message
 *      starting an inventory command is received from the master. The command
 *      is sent to the RFID transponder which returns a message containing UID
 *      of the transponder. The parameters in the message are transfered to
 *      the data structure of the addressed module instance. The modified
 *      settings influence the behavior of the module task.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693AcquireUid (UInt16 Channel, CanMessage_t* Message)
{
    Error_t Error;
    InstanceData_t* Data;
    Error_t Instance = bmGetInstance(Channel);

    if (Instance < NO_ERROR) {
        return (Instance);
    }
    else if (Instance >= InstanceCount) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    Data = &DataTable[Instance];

    if (Message->Length == 0) {
        if ((Data->Flags & MODE_MODULE_ENABLE) == 0) {
            return (E_MODULE_NOT_ENABLED);
        }
        else if (Data->ModuleState == MODULE_STATE_READY) {
            rfid15693LinkInventory (&Data->Stream);
            Error = rfid15693StartCompare (Data);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Error = halPortWrite (Data->HandleOut, 1);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Data->ModuleState = MODULE_STATE_BUSY;
            return (NO_ERROR);
        }
        return (E_RFID15693_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Sets the UID required for RFID transactions
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting the unique ID is received from the master. The UID is needed
 *      to address a certain RFID transponder in future transactions. The
 *      "Acquire UID" message overwrites the UID, when the transaction was
 *      executed correctly. The parameters in the message are transfered to
 *      the data structure of the addressed module instance. The modified
 *      settings influence the behavior of the module task. The following
 *      settings will be modified:
 *
 *      - Unique ID according to ISO/IEC 15693
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693SetUid (UInt16 Channel, CanMessage_t* Message)
{
    InstanceData_t* Data;
    Error_t Instance = bmGetInstance(Channel);

    if (Instance < NO_ERROR) {
        return (Instance);
    }
    else if (Instance >= InstanceCount) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    Data = &DataTable[Instance];

    if (Message->Length == 8) {
        if ((Data->Flags & MODE_MODULE_ENABLE) == 0) {
            return (E_MODULE_NOT_ENABLED);
        }
        else if (Data->ModuleState == MODULE_STATE_READY) {
            Data->UniqueId = bmGetMessageItem(Message, 0, 4);
            Data->UniqueId = Data->UniqueId << 32;
            Data->UniqueId |= bmGetMessageItem(Message, 4, 4);
            return (NO_ERROR);
        }
        return (E_RFID15693_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Requests the UID stored in the function module
 *
 *      This function is called by the CAN message dispatcher when a message
 *      requesting the unique ID is received from the master. The UID is
 *      either set, when it is fetched from the RFID transponder or when it is
 *      explicitly set by the master. As a response a UID response message is
 *      sent back to the master. The following settings will be responded:
 *
 *      - Unique ID according to ISO/IEC 15693
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693GetUid (UInt16 Channel, CanMessage_t* Message)
{
    InstanceData_t* Data;
    Error_t Instance = bmGetInstance(Channel);

    if (Instance < NO_ERROR) {
        return (Instance);
    }
    else if (Instance >= InstanceCount) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    Data = &DataTable[Instance];

    if (Message->Length == 0) {
        if ((Data->Flags & MODE_MODULE_ENABLE) == 0) {
            return (E_MODULE_NOT_ENABLED);
        }
        else if (Data->ModuleState == MODULE_STATE_READY) {
            CanMessage_t RespMessage;

            RespMessage.CanID = MSG_RFID15693_RESP_UID;
            bmSetMessageItem (&RespMessage, Data->UniqueId >> 32, 0, 4);
            bmSetMessageItem (&RespMessage, Data->UniqueId, 4, 4);
            RespMessage.Length = 8;
            return (canWriteMessage(Data->Channel, &RespMessage));
        }
        return (E_RFID15693_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Starts a read block transaction
 *
 *      This function is called by the CAN message dispatcher when a message
 *      starting a read single block command is received from the master. The
 *      command is sent to the RFID transponder which returns an
 *      acknowledgement and the requested data. The parameters in the message
 *      are transfered to the data structure of the addressed module instance.
 *      The modified settings influence the behavior of the module task. The
 *      following settings will be modified:
 *
 *      - Address of the memory block
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693ReadBlock (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Address;
    Error_t Error;
    InstanceData_t* Data;
    Error_t Instance = bmGetInstance(Channel);

    if (Instance < NO_ERROR) {
        return (Instance);
    }
    else if (Instance >= InstanceCount) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    Data = &DataTable[Instance];

    if (Message->Length == 1) {
        if ((Data->Flags & MODE_MODULE_ENABLE) == 0) {
            return (E_MODULE_NOT_ENABLED);
        }
        else if (Data->ModuleState == MODULE_STATE_READY) {
            Address = bmGetMessageItem(Message, 0, 1);

            rfid15693LinkReadSingleBlock (&Data->Stream, Data->UniqueId, Address);
            Error = rfid15693StartCompare (Data);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Error = halPortWrite (Data->HandleOut, 1);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Data->ModuleState = MODULE_STATE_BUSY;
            return (NO_ERROR);
        }
        return (E_RFID15693_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Starts a write block transaction
 *
 *      This function is called by the CAN message dispatcher when a message
 *      starting a write single block command is received from the master. The
 *      command is sent to the RFID transponder which returns an
 *      acknowledgement message. The parameters in the message are transfered
 *      to the data structure of the addressed module instance. The modified
 *      settings influence the behavior of the module task. The following
 *      settings will be modified:
 *
 *      - Address of the memory block
 *      - Data word written to the block
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693WriteBlock (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Address;
    UInt32 DataWord;
    Error_t Error;
    InstanceData_t* Data;
    Error_t Instance = bmGetInstance(Channel);

    if (Instance < NO_ERROR) {
        return (Instance);
    }
    else if (Instance >= InstanceCount) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    Data = &DataTable[Instance];

    if (Message->Length == 5) {
        if ((Data->Flags & MODE_MODULE_ENABLE) == 0) {
            return (E_MODULE_NOT_ENABLED);
        }
        else if (Data->ModuleState == MODULE_STATE_READY) {
            Address = bmGetMessageItem(Message, 0, 1);
            DataWord = bmGetMessageItem(Message, 1, 4);

            rfid15693LinkWriteSingleBlock (&Data->Stream, Data->UniqueId, Address, DataWord);
            Error = rfid15693StartCompare (Data);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Error = halPortWrite (Data->HandleOut, 1);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Data->ModuleState = MODULE_STATE_BUSY;
            return (NO_ERROR);
        }
        return (E_RFID15693_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Starts a lock block transaction
 *
 *      This function is called by the CAN message dispatcher when a message
 *      starting a lock block command is received from the master. The command
 *      is sent to the RFID transponder which returns an acknowledgement
 *      message. The parameters in the message are transfered to the data
 *      structure of the addressed module instance. The modified settings
 *      influence the behavior of the module task. The following settings will
 *      be modified:
 *
 *      - Address of the memory block
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693LockBlock (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Address;
    Error_t Error;
    InstanceData_t* Data;
    Error_t Instance = bmGetInstance(Channel);

    if (Instance < NO_ERROR) {
        return (Instance);
    }
    else if (Instance >= InstanceCount) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    Data = &DataTable[Instance];

    if (Message->Length == 1) {
        if ((Data->Flags & MODE_MODULE_ENABLE) == 0) {
            return (E_MODULE_NOT_ENABLED);
        }
        else if (Data->ModuleState == MODULE_STATE_READY) {
            Address = bmGetMessageItem(Message, 0, 1);

            rfid15693LinkLockBlock (&Data->Stream, Data->UniqueId, Address);
            Error = rfid15693StartCompare (Data);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Error = halPortWrite (Data->HandleOut, 1);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Data->ModuleState = MODULE_STATE_BUSY;
            return (NO_ERROR);
        }
        return (E_RFID15693_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Interrupt handler issued by a timer unit
 *
 *      The interrupt handler is called when the counter of a timer unit
 *		reaches a specified compare value or when an event is captured on a
 *		timer channel. The handler sets the modulation signal of the RFID
 *      front end and controls the transmission of RFID messages. When the
 *      message transfer completes successfully, the capture interrupt
 *		responsible for the reception of messages is activated.
 *
 *  \iparam  Channel = user information
 *  \iparam  IntrFlags = interrupt flags
 *
 ****************************************************************************/

static void rfid15693InterruptHandler (UInt32 Channel, UInt32 IntrFlags)
{
    Error_t Error;
    InstanceData_t *Data;

    if (Channel >= InstanceCount) {
        return;
    }
    Data = &DataTable[Channel];

    Error = halCapComStatus (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, TIM_STAT_PENDING);
    if (Error == 0) {
        // Handling the update event interrupt
        if (Data->InitState != STATE_INIT) {
            Data->IrqError = rfid15693InitIntHandler (Data);
        }
        else {
            Data->IrqError = rfid15693CompareIntHandler (Data);
        }
    }
    else if (Error > 0) {
        // Handling the capture interrupt
        Data->IrqError = rfid15693CaptureIntHandler (Data);
    }
    else {
        Data->IrqError = Error;
    }

    if (Data->IrqError < NO_ERROR) {
        Error = halTimerControl (Data->HandleTimer, TIM_INTR_DISABLE);
        if (Error < NO_ERROR) {
            Data->IrqError = Error;
        }
        Error = halCapComControl (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, TIM_INTR_DISABLE);
        if (Error < NO_ERROR) {
            Data->IrqError = Error;
        }
    }

    return;
}


/*****************************************************************************/
/*!
 *  \brief   Programs the option bits into the RFID front end
 *
 *      This function is called by the interrupt handler that is issued, when
 *      a timer unit receives a compare event. It sets the clock and the data
 *      outputs that are connected to the RFID front end IC. The method
 *      basically consists of a state machine controlling the different stages
 *      of the programming sequence.
 *
 *  \xparam  Data = Contains all module instance variables
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

static Error_t rfid15693InitIntHandler (InstanceData_t *Data)
{
    Error_t Error;
    UInt16 Output = 0;

    switch (Data->InitState) {
        case STATE_HIGH_DIN:
            Output = 3;
            Data->InitState = STATE_LOW_DIN;
            break;
        case STATE_LOW_DIN:
            Output = 2;
            Data->BitCount = 0;
            Data->InitState = STATE_LOW_DCLK;
            break;
        case STATE_LOW_DCLK:
            if (Data->BitCount == 32) {
                Error = halTimerControl (Data->HandleTimer, TIM_CTRL_STOP);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                Output = 0;
                Data->InitState = STATE_ACK;
            }
            else {
                Output = (Data->OptionBits >> Data->BitCount) & 0x1;
                Data->InitState = STATE_HIGH_DCLK;
            }
            break;
        case STATE_HIGH_DCLK:
            Output = ((Data->OptionBits >> Data->BitCount) & 0x1) | 0x2;
            Data->BitCount++;
            Data->InitState = STATE_LOW_DCLK;
            break;
    }

    return (halPortWrite (Data->HandleOut, Output));
}


/*****************************************************************************/
/*!
 *  \brief   Transmits a bit stream to an RFID transponder
 *
 *      This function is called by the interrupt handler that is issued, when
 *      a timer unit receives a compare event. It reads single data bits from
 *      a prepared bit stream and controls the modulation input of the RFID
 *      front end accordingly. The function implements the high data rate mode
 *      specified in the ISO/IEC 15693 standard.
 *
 *  \xparam  Data = Contains all module instance variables
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

static Error_t rfid15693CompareIntHandler (InstanceData_t *Data)
{
    Error_t Error;
    UInt8 PairOfBits;
    UInt16 Output = 0;

    switch (Data->Stream.TransmitState) {
        case LINK_TXSTATE_SOF:
            if (Data->Stream.PosCount == 5) {
                Output = 1;
            }
            else if (Data->Stream.PosCount == 7) {
                Data->Stream.TransmitState = LINK_TXSTATE_DATA;
            }
            break;
        case LINK_TXSTATE_DATA:
            PairOfBits = rfid15693LinkReadBit (Data->Stream.TxBuffer, Data->Stream.TxCount);
            PairOfBits |= rfid15693LinkReadBit (Data->Stream.TxBuffer, Data->Stream.TxCount+1) << 1;
            if (Data->Stream.PosCount == (2 * PairOfBits + 1))  {
                Output = 1;
            }
            if (Data->Stream.PosCount == 7) {
                Data->Stream.TxCount += 2;
                if (Data->Stream.TxCount == Data->Stream.TxLength) {
                    Data->Stream.TransmitState = LINK_TXSTATE_EOF;
                }
            }
            break;
        case LINK_TXSTATE_EOF:
            if (Data->Stream.PosCount == 2) {
                Output = 1;
            }
            else if (Data->Stream.PosCount == 3) {
                Data->Stream.TransmitState = LINK_TXSTATE_FINISHED;
                Error = halTimerControl (Data->HandleTimer, TIM_INTR_DISABLE);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                Error = halTimerWrite (Data->HandleTimer, TIM_REG_RELOAD, MAX_UINT16);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                Error = halTimerControl (Data->HandleTimer, TIM_CTRL_RELOAD);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                Error = halCapComControl (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, TIM_CTRL_START);
                if (Error < 0) {
                    return (Error);
                }
                Error = halCapComControl (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, TIM_INTR_ENABLE);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                Error = halCapComRead (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, &Data->OldCount);
                if (Error < 0 && Error != E_TIMER_CAPCOM_OVERFLOW) {
                    return (Error);
                }
            }
            break;
    }

    if (Data->Stream.PosCount == 7) {
        Data->Stream.PosCount = 0;
    }
    else {
        Data->Stream.PosCount++;
    }

    return (halPortWrite (Data->HandleOut, Output));
}

/*****************************************************************************/
/*!
 *  \brief   Interrupt handler issued by the capture channel
 *
 *      The interrupt handler is called when the capture channel detects a
 *      transition on the DOUT signal coming from the RFID front end. The
 *      handler measures the time since the last transition. This way, it is
 *      able to identify single bits in the incoming data stream. When the
 *      message transfer completes successfully, the interrupt is deactivated
 *      again.
 *
 *  \iparam  Data = Instance data structure
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693CaptureIntHandler (InstanceData_t *Data)
{
    UInt32 Time;
    Error_t Error;
    UInt32 NewCount;
    const UInt32 Threshold = 64;

    Error = halCapComRead (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, &NewCount);
    if (Error < 0) {
        return (Error);
    }
    Time = Data->TimeMask & (NewCount - Data->OldCount);
    Data->OldCount = NewCount;

    switch(Data->Stream.ReceiveState) {
        case LINK_RXSTATE_WAIT:
            if (Time >= 256 - Threshold && Time <= 256 + Threshold) {
                Data->Stream.ReceiveState = LINK_RXSTATE_SOF1;
            }
            break;
        case LINK_RXSTATE_SOF1:
            if (Time >= 256 - Threshold && Time <= 256 + Threshold) {
                Data->Stream.ReceiveState = LINK_RXSTATE_SOF2;
            }
            else {
                return (E_RFID15693_UNKNOWN_RXSYMBOL);
            }
            break;
        case LINK_RXSTATE_SOF2:
            if (Time >= 512 - Threshold && Time <= 512 + Threshold) {
                Data->Stream.ReceiveState = LINK_RXSTATE_DATA;
            }
            else {
                return (E_RFID15693_UNKNOWN_RXSYMBOL);
            }
            break;
        case LINK_RXSTATE_DATA:
            if (Time >= 256 - Threshold && Time <= 256 + Threshold) {
                if (Data->Stream.SymbolShift == TRUE) {
                    Data->Stream.ReceiveState = LINK_RXSTATE_EOF;
                }
                else {
                    rfid15693LinkWriteBit(Data->Stream.RxBuffer, Data->Stream.RxCount++, 0);
                }
                Data->Stream.SymbolShift = TRUE;
            }
            else if (Time >= 512 - Threshold && Time <= 512 + Threshold) {
                if (Data->Stream.SymbolShift == TRUE) {
                    rfid15693LinkWriteBit(Data->Stream.RxBuffer, Data->Stream.RxCount++, 0);
                }
                else {
                    rfid15693LinkWriteBit(Data->Stream.RxBuffer, Data->Stream.RxCount++, 1);
                }
            }
            else if (Time >= 768 - Threshold && Time <= 768 + Threshold) {
                if (Data->Stream.SymbolShift == FALSE) {
                    return (E_RFID15693_UNKNOWN_RXSYMBOL);
                }
                else {
                    rfid15693LinkWriteBit(Data->Stream.RxBuffer, Data->Stream.RxCount++, 1);
                }
                Data->Stream.SymbolShift = FALSE;
            }
            else {
                return (E_RFID15693_UNKNOWN_RXSYMBOL);
            }
            break;
        case LINK_RXSTATE_EOF:
            Error = halCapComControl (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, TIM_INTR_DISABLE);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Error = halCapComControl (Data->HandleTimer, RFID15693_CAPTURE_CHANNEL, TIM_CTRL_STOP);
            if (Error < 0) {
                return (Error);
            }
            break;
    }

    return (NO_ERROR);
}

/*****************************************************************************/
/*!
 *  \brief   Opens the handles of the peripherals
 *
 *  This functions accesses the hardware abstraction layer (HAL) top open the
 *  devices handles of the different microcontroller peripherals.
 *
 *  \iparam  Data = Data of a module instance
 *  \iparam  Instance = Instance number
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid15693HandleOpen (InstanceData_t *Data, Int16 Instance)
{
    Error_t Error;

    // Open digital ports for RFID control signals
    Data->HandleEn = halPortOpen (HAL_RFID15693_EN + Instance, HAL_OPEN_WRITE);
    if (Data->HandleEn < NO_ERROR) {
        return (Data->HandleEn);
    }
    Data->HandleOut = halPortOpen (HAL_RFID15693_OUT + Instance, HAL_OPEN_WRITE);
    if (Data->HandleOut < NO_ERROR) {
        return (Data->HandleOut);
    }

    // Open hardware timer for the RFID transmission
    Data->HandleTimer = halTimerOpen (HAL_RFID15693_TIM + Instance, Instance, rfid15693InterruptHandler);
    if (Data->HandleTimer < 0) {
        return (Data->HandleTimer);
    }
    Error = halTimerStatus (Data->HandleTimer, TIM_STAT_MAXCOUNT);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Data->TimeMask = Error;
    /*Error = halTimerControl (Data->HandleTimer, TIM_CTRL_START);
    if (Error < NO_ERROR) {
        return (Error);
    }*/

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes this function module
 *
 *      This function is called once during startup by the base module. It
 *      registers itself to the task scheduler and registers all handler
 *      functions to the CAN message dispatcher. Furthermore, it opens the HAL
 *      devices used by the various instances of this module and
 *      allocates/initializes storage for the variables for each instance of
 *      this module.
 *
 *  \iparam  ModuleID = Module identifier
 *  \iparam  Instances = Number of instances of this module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t rfid15693InitializeModule (UInt16 ModuleID, UInt16 Instances)
{
    static bmCallbackEntry_t Commands[] = {
        { MSG_RFID15693_SET_CONFIG, rfid15693SetConfig },
        { MSG_RFID15693_ACQUIRE_UID, rfid15693AcquireUid },
        { MSG_RFID15693_SET_UID, rfid15693SetUid },
        { MSG_RFID15693_REQ_UID, rfid15693GetUid },
        { MSG_RFID15693_READ_BLOCK, rfid15693ReadBlock },
        { MSG_RFID15693_WRITE_BLOCK, rfid15693WriteBlock },
        { MSG_RFID15693_LOCK_BLOCK, rfid15693LockBlock },
    };

    static bmModuleInterface_t Interface = {
        rfid15693ModuleTask,
        rfid15693ModuleControl,
        rfid15693ModuleStatus
    };
    Error_t Status;
    UInt16 i;

    // allocate module instances data storage
    DataTable = calloc (Instances, sizeof(InstanceData_t));
    if (NULL == DataTable) {
        return (E_MEMORY_FULL);
    }
    // register function module to the scheduler
    Status = bmRegisterModule (ModuleID, Instances, &Interface);
    if (Status < NO_ERROR) {
        return (Status);
    }
    // bind CAN commands to the handler functions
    Status = canRegisterMessages (ModuleID, Commands, ELEMENTS(Commands));
    if (Status < NO_ERROR) {
        return (Status);
    }
    #ifdef DEBUG
    rfid15693RegisterDebugNames(ModuleID);
    #endif

    // open channels and ports for all modules
    for (i=0; i < Instances; i++) {
        if ((Status = bmGetChannel(bmGetTaskID(ModuleID, i))) < NO_ERROR) {
            return (Status);
        }
        DataTable[i].Channel = Status;
        DataTable[i].Flags = 0;

        // Get the option bits of the EM4094 front end IC, the default bits support ISO/IEC 15693
        DataTable[i].OptionBits = bmGetBoardOptions (ModuleID, i, 0x02004013);
        if (DataTable[i].OptionBits == 0) {
            return (E_BOARD_OPTION_MISSING);
        }

        // Open the HAL device handlers
        Status = rfid15693HandleOpen (&DataTable[i], i);
        if (Status < NO_ERROR) {
            return (Status);
        }

        DataTable[i].ModuleState = MODULE_STATE_READY;
    }
    InstanceCount = Instances;
    ModuleIdentifier = ModuleID;

    return (NO_ERROR);
}

//****************************************************************************/

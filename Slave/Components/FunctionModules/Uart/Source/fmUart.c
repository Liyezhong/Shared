/****************************************************************************/
/*! \file fmUart.c
 * 
 *  \brief Functional module controlling a serial communications interface.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 26.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *  This module connects the CAN interface which is used for master to slave
 *  communication with the internal serial communication interface of the
 *  microcontroller. It forwards configuration parameters to the serial
 *  interface implemented in the hardware abstraction layer (HAL). Beside of
 *  that, it writes to and reads from this serial interface. There are two
 *  ways data read from the UART controller is forwarded to the master. These
 *  modes are called request mode and immediate mode. In request mode, the
 *  data is sent in response to explicit requests received from the master.
 *  In immediate mode, the slave itself immediately sends data words fetched
 *  from the serial interface.
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
#include "fmUart.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

/* Global defines of the serial interface module */
#define MODULE_VERSION      0x0001  //!< Version number of module

/* Mode bits for the Flags member of the module instance data */
#define MODE_MODULE_ENABLE  0x1     //!< Module on/off bit
#define MODE_COMMUNICATION  0x2     //!< Request (0) or immediate (1) read communication

#define FORMAT_STOP_BITS      0x01  //!< One or two stop bits
#define FORMAT_PARITY_MODES   0x06  //!< 0 = No parity, 1 = Odd parity, 3 = Even parity
#define FORMAT_CHAR_LENGTH    0x38  //!< Bit length of a single character
#define FORMAT_FLOW_CONTROL   0xC0  //!< 0 = No flow control, 1 = CTS signal, 2 = RTS signal, 3 = Both signals

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Contains all variables for a instance of this module */
typedef struct {
    bmModuleState_t ModuleState; //!< Module state
    Handle_t Handle;             //!< Handle to access a serial interface controller (HAL)
    UInt16 Channel;              //!< Logical CAN channel
    UInt16 Flags;                //!< Mode control flag bits

    UInt8 TxCount;               //!< Transmit buffer fill counter
    UInt8 TxBuffer[8];           //!< Transmit buffer
    UInt8 RxCount;               //!< Receive buffer fill counter
    UInt8 RxBuffer[8];           //!< Receive buffer
    SciFormat_t SciFormat;       //!< Holding the format of a UART character
} InstanceData_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static UInt16 InstanceCount = 0;        //!< number of module instances
static UInt16 ModuleIdentifier = 0;     //!< module identifier

static InstanceData_t* DataTable;  //!< data table for all instances

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Error_t uartModuleControl (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t uartModuleStatus  (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t uartModuleTask    (UInt16 Instance);

static Error_t uartSetConf (UInt16 Channel, CanMessage_t* Message);
static Error_t uartSetData (UInt16 Channel, CanMessage_t* Message);
static Error_t uartGetData (UInt16 Channel, CanMessage_t* Message);

static Error_t uartDataWrite (InstanceData_t *Data);
static Error_t uartDataRead  (InstanceData_t *Data);
static Error_t uartSendData  (UInt16 Channel, const UInt8 *RxBuffer, UInt8 RxCount);
static Error_t uartSendAck   (UInt16 Channel);

/*****************************************************************************/
/*! 
 *  \brief   Module Control Function
 *
 *      This function is called by the base module to control this function
 *      module. Depending on the ControlID parameter, the following actions
 *      are performed:
 * 
 *      - MODULE_CONTROL_STOP
 *      - MODULE_CONTROL_RESUME
 *      - MODULE_CONTROL_STANDBY  
 *      - MODULE_CONTROL_WAKEUP
 *      - MODULE_CONTROL_SHUTDOWN        
 * 
 *  \iparam  Instance  = Instance number of this module [in]
 *  \iparam  ControlID = Control code to select sub-function [in]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t uartModuleControl (UInt16 Instance, bmModuleControlID_t ControlID)
{
    InstanceData_t* Data = &DataTable[Instance];

    switch (ControlID)
    {
        case MODULE_CONTROL_RESUME:
        case MODULE_CONTROL_WAKEUP:
            Data->ModuleState = MODULE_STATE_READY;
            break;

        case MODULE_CONTROL_STOP:
            Data->ModuleState = MODULE_STATE_STOPPED;
            break;

        case MODULE_CONTROL_SHUTDOWN:
            Data->ModuleState = MODULE_STATE_STANDBY;
            break;

        case MODULE_CONTROL_RESET:
            Data->Flags &= ~MODE_MODULE_ENABLE;
            break;
                                
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
 *      - MODULE_STATUS_INSTANCES
 *      - MODULE_STATUS_MODULE_ID
 *      - MODULE_STATUS_POWER_STATE
 *      - MODULE_STATUS_VERSION
 *      - MODULE_STATUS_STATE
 *      - MODULE_STATUS_ABORTED
 *      - MODULE_STATUS_VALUE
 * 
 *  \iparam  Instance = Instance number of this module
 *  \iparam  StatusID = selects which status is requested
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t uartModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID)
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
            return (Data->SciFormat.Baudrate);
        
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

static Error_t uartModuleTask (UInt16 Instance)
{
    Error_t Error;
    InstanceData_t* Data = &DataTable[Instance];
    
    if ((Data->Flags & MODE_MODULE_ENABLE) != 0) {
        if (Data->ModuleState == MODULE_STATE_BUSY) {
            // Write data to the serial device
            Error = uartDataWrite (Data);
            if (Error < NO_ERROR) {
                bmSignalEvent (Data->Channel, Error, TRUE, 0);
                return ((Error_t) Data->ModuleState);
            }
            // Transmission successful
            if (Data->TxCount == 0) {
                Data->ModuleState = MODULE_STATE_READY;
                Error = uartSendAck (Data->Channel);
                if (Error < NO_ERROR) {
                    bmSignalEvent (Data->Channel, Error, TRUE, 0);
                    return ((Error_t) Data->ModuleState);
                }
            }
        }
        if (Data->ModuleState == MODULE_STATE_BUSY || Data->ModuleState == MODULE_STATE_READY) {
            // Read available data
            Error = uartDataRead (Data);
            if (Error < NO_ERROR) {
                bmSignalEvent (Data->Channel, Error, TRUE, 0);
                return ((Error_t) Data->ModuleState);
            }
            // Check if the module is in immediate mode
            if ((Data->Flags & MODE_COMMUNICATION) != 0 && Data->RxCount != 0) {
                // Send available data to the master
                Error = uartSendData (Data->Channel, Data->RxBuffer, Data->RxCount);
                if (Error < NO_ERROR) {
                    bmSignalEvent (Data->Channel, Error, TRUE, 0);
                    return ((Error_t) Data->ModuleState);
                }
                Data->RxCount = 0;
            }
        }
    }
    
    return ((Error_t) Data->ModuleState);
}


/*****************************************************************************/
/*! 
 *  \brief  Writes from a buffer to the serial interface
 *
 *  This method writes single bytes to a serial interface covered through the
 *  hardware abstraction layer. This operation is performed until the transmit
 *  buffer of the instance is empty or the serial interface is not able to
 *  receive data words any more.
 * 
 *  \xparam  Data = Data of a module instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t uartDataWrite (InstanceData_t *Data)
{
    Error_t Error;
    
    while (Data->TxCount > 0) {
        Error = halSerialWrite(Data->Handle, Data->TxBuffer[Data->TxCount - 1]);
        if (Error < 0) {
            return (Error);
        }
        else if (Error == 0) {
            return (NO_ERROR);
        }
        else {
            Data->TxCount--;
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief  Reads data from the serial interface into a buffer
 *
 *  This method reads single bytes from a serial interface covered through the
 *  hardware abstraction layer. This operation is performed until the receive
 *  buffer of the instance is full or there is no data available at the serial
 *  interface any more.
 * 
 *  \xparam  Data = Data of a module instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t uartDataRead (InstanceData_t *Data)
{
    UInt16 Temp;
    Error_t Error;
    
    while (Data->RxCount < 8) {
        Error = halSerialRead(Data->Handle, &Temp);
        Data->RxBuffer[Data->RxCount] = (UInt8) Temp;
        if (Error < 0) {
            return (Error);
        }
        else if (Error == 0) {
            return (NO_ERROR);
        }
        else {
            Data->RxCount++;
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief  Sets various parameters of serial controller interface
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      setting the configuration parameters of the UART module is received
 *      from the master. The message activates the module, switches between
 *      both data read communication modes, sets the data symbol parameters
 *      and the baud rate. The parameters in the message are transfered to
 *      the data structure of the addressed module instance. The modified
 *      settings influence the behavior of the module task. The following
 *      settings will be modified:
 *
 *      - Module enable bit
 *      - Read communication mode (0 = immediate / 1 = request)
 *      - Number of stop bits
 *      - Parity bit or not
 *      - Even or odd parity
 *      - Data Length
 *      - Baud rate
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t uartSetConf (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Format;
    Error_t Error;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];
 
    if (Message->Length == 6) {
        Data->Flags = bmGetMessageItem(Message, 0, 1);
        Format = bmGetMessageItem(Message, 1, 1);

        Data->SciFormat.Stopbits = (Format & FORMAT_STOP_BITS) + 1;
        Data->SciFormat.Parity = (Format & FORMAT_PARITY_MODES) >> 1;
        Data->SciFormat.Databits = ((Format & FORMAT_CHAR_LENGTH) >> 3) + 1;
        Data->SciFormat.FlowCtrl = (Format & FORMAT_FLOW_CONTROL) >> 6;
        
        Data->SciFormat.Baudrate = bmGetMessageItem(Message, 2, 4);
        
        Error = halSerialSetup(Data->Handle, &Data->SciFormat);
        if (Error < NO_ERROR) {
            return (Error);
        }

        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief  Sets the data words written over the serial interface
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      writing data to the serial communication interface is received from
 *      the master. The parameters in the message are transfered to the data
 *      structure of the addressed module instance. The modified settings
 *      influence the behavior of the module task. The following settings will
 *      be modified:
 *
 *      - TX Buffer (up to 8 bytes)
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t uartSetData (UInt16 Channel, CanMessage_t* Message)
{
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];
 
    if (Message->Length != 0) {
        if ((Data->Flags & MODE_MODULE_ENABLE) != 0) {
            if (Data->ModuleState == MODULE_STATE_READY) {
                Data->TxCount = 0;
                while (Data->TxCount < Message->Length) {
                    Data->TxBuffer[(Message->Length - Data->TxCount) - 1] = bmGetMessageItem(Message, Data->TxCount, 1);
                    Data->TxCount++;
                }
                Data->ModuleState = MODULE_STATE_BUSY;
                return (NO_ERROR);
            }
            return (E_UART_MODULE_BUSY);
        }
        return (E_UART_MODULE_DISABLED);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief  Fetches the data words read from the serial interface
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      reading data from the serial communication interface is received from
 *      the master. The parameters in the message are transfered to the data
 *      structure of the addressed module instance. The modified settings
 *      influence the behavior of the module task. The following settings will
 *      be modified:
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t uartGetData (UInt16 Channel, CanMessage_t* Message)
{
    Error_t Error;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if ((Data->Flags & MODE_MODULE_ENABLE) != 0) {
        if ((Data->Flags & MODE_COMMUNICATION) == 0) {
            Error = uartSendData (Data->Channel, Data->RxBuffer, Data->RxCount);
            if (Error < NO_ERROR) {
                return (Error);
            }
            Data->RxCount = 0;
            return (NO_ERROR);
        }
        return (E_UART_IMMEDIATE_MODE);
    }
    return (E_UART_MODULE_DISABLED);
}


/*****************************************************************************/
/*! 
 *  \brief   Send analog input value to the master
 *
 *      Sends the data pointed to by the "Data" parameter on the CAN-Bus.
 *      The message contains the port data value and (optionally) the
 *      time the input changes state. The time is in milliseconds of 
 *      the actual day.
 * 
 *  \iparam  Channel = Channel ID of the instance
 *  \iparam  RxBuffer = Receive buffer
 *  \iparam  RxCount = Receive buffer fill counter
 * 
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t uartSendData (UInt16 Channel, const UInt8 *RxBuffer, UInt8 RxCount)
{
    UInt8 i;
    CanMessage_t Message;

    Message.CanID = MSG_UART_RESP_DATA;
    Message.Length = RxCount;

    for (i = 0; i < RxCount; i++) {
        bmSetMessageItem (&Message, RxBuffer[i], i, 1);
    }
    
    return (canWriteMessage(Channel, &Message));
}


/*****************************************************************************/
/*! 
 *  \brief   Acknwledge the transmission of data over UART
 *
 *      This function sends an acknowledgement message to the master that
 *      indicates, that data received by this functional module is passed to
 *      the UART controller.
 * 
 *  \iparam  Channel = Channel ID of the instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t uartSendAck (UInt16 Channel)
{
    CanMessage_t Message;

    Message.CanID = MSG_UART_ACK_TRANSMIT;
    Message.Length = 0;
    
    return (canWriteMessage(Channel, &Message));
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

Error_t uartInitializeModule (UInt16 ModuleID, UInt16 Instances)
{
    static bmCallbackEntry_t Commands[] = {
        { MSG_UART_SET_CONF, uartSetConf },
        { MSG_UART_SET_DATA, uartSetData },
        { MSG_UART_REQ_DATA, uartGetData },
    };
    
    static bmModuleInterface_t Interface = {
        uartModuleTask, 
        uartModuleControl, 
        uartModuleStatus
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
    if (Status < 0) {
        return (Status);
    }
    // bind CAN commands to the handler functions
    Status = canRegisterMessages (ModuleID, Commands, ELEMENTS(Commands));
    if (Status < 0) {
        return (Status);
    }
    #ifdef DEBUG
    uartRegisterDebugNames(ModuleID);
    #endif

    // open channels and ports for all modules
    for (i=0; i < Instances; i++) {
        
        DataTable[i].Channel = bmGetChannel(bmGetTaskID(ModuleID, i));
        DataTable[i].Flags = 0;
        
        DataTable[i].Handle = halSerialOpen (HAL_SERIAL_PORT + i, 0, NULL);
        if (DataTable[i].Handle < 0) {
            return (DataTable[i].Handle);
        }
        
        DataTable[i].ModuleState = MODULE_STATE_READY;
    }
    InstanceCount = Instances;
    ModuleIdentifier = ModuleID;
    
    return (NO_ERROR);
}

//****************************************************************************/

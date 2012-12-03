/****************************************************************************/
/*! \file fmRfidIso11785.c
 * 
 *  \brief Functional module controlling RFID ISO 11785 communication.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 12.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This module contains the top level source code of the RFID ISO 11785
 *      communication controller. The code in this file implements functions,
 *      that are needed to communicate with the EM Microelectronic EM4205/4305
 *      RFID transponder. This transponder uses a propietary protocol on top
 *      of ISO 11785 and thus is not standard compliant. The module controls
 *      the EM Microelectronic EM4095 RFID front end IC via the Hardware
 *      Abstraction Layer (HAL). For this purpose, the module uses digital I/O
 *      ports and a timer unit to control the time critical communication
 *      procedure through the generation of interrupts.
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
#include "fmRfidIso11785.h"
#include "fmRfidIso11785Link.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

/* Global defines of the RFID module */ 
#define MODULE_VERSION      0x0001  //!< Version number of module

/* Mode bits for the Flags member of the module instance data */
#define MODE_MODULE_ENABLE  0x1     //!< Module on/off bit

/* RFID transponder register file addresses */
#define ADDR_UID            1       //!< Address of the UID number register
#define ADDR_PASSWORD       2       //!< Address of the password register
#define ADDR_CONFIG         4       //!< Address of the configuration register
#define ADDR_USER           5       //!< Address of the first user data register
#define ADDR_USER_MAX       9       //!< Number of 32 bit user registers

#define RFID11785_TIME_FIRST_STOP    55 //!< Time of the first field stop in 125 kHz Cycles
#define RFID11785_TIME_MOD_OFF       9  //!< Time of the mod off state in 125 kHz Cycles
#define RFID11785_TIME_MOD_ON        23 //!< Time of the mod on state in 125 kHz Cycles

#define RFID11785_CAPTURE_CHANNEL   2 //!< Timer channel used for the capture input
#define RFID11785_COMPARE_CHANNEL   3 //!< Timer channel used for the compare output

#define ANTENNA_SWITCH_INTERVAL 20  //!< Time antennas need to be switched in milliseconds

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

/*! Contains message types needed to communicate with the RFID transponder */
typedef enum {
    SetConfig,      //!< Set config
    Login,          //!< Login
    WriteUserData,  //!< Write user data
    WritePassword,  //!< Write password
    WriteConfig,    //!< Write configuration
    ReadUserData,   //!< Read user data
    ReadUidNumber   //!< Read UID number
} Rfid11785Command_t;

/*! Contains all variables for a instance of this module */
typedef struct {
    bmModuleState_t ModuleState;    //!< Module state
    Handle_t HandleShdOut;          //!< Handle to access a digital output port (HAL)
    Handle_t HandleModOut;          //!< Handle to access a digital output port (HAL)
    Handle_t HandleDevSelOut;       //!< Handle to access a digital output port (HAL)
    Handle_t HandleTimer;           //!< Handle to access a hardware timer (HAL)
    UInt16 Channel;                 //!< Logical CAN channel
    UInt16 Flags;                   //!< Mode control flag bits

    UInt8 CycleNumber;              //!< Number of cycles needed to send one bit
    UInt8 AntennaNumber;            //!< Number of RFID antennas available
    UInt8 DeviceSelect;             //!< Multiplexer select input
        
    UInt32 AccTime;                 //!< Accumulates the time values for the different compare interrupts
    UInt32 TimeMask;                //!< The size of the timer register
    UInt32 SwitchTime;              //!< The time the antenna is switched
    Bool Started;                   //!< Indicates if the communication was started

    UInt8 CurrentAddress;           //!< Address of the ongoing transaction
    Rfid11785Command_t Ongoing;     //!< Ongoing RFID command
    Rfid11785Stream_t DataStream;   //!< Containg the status of an RFID transaction
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

static Error_t rfid11785ModuleControl (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t rfid11785ModuleStatus  (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t rfid11785ModuleTask    (UInt16 Instance);

static Error_t rfid11785RespondAck    (UInt16 Channel, Rfid11785Command_t Ongoing, UInt8 Address, UInt32 ReadData);
static Error_t rfid11785StartModOutput(InstanceData_t *Data);
static Error_t rfid11785SetModOutput  (InstanceData_t *Data, UInt32 Count, UInt16 Value);
static Error_t rfid11785SetDeviceSel  (Handle_t Handle, UInt8 DeviceSelect, UInt8 AntennaNumber);
static Error_t rfid11785HandleOpen    (InstanceData_t *Data, Int16 Instance);

static Error_t rfid11785SetConfig     (UInt16 Channel, CanMessage_t *Message);
static Error_t rfid11785SendLogin     (UInt16 Channel, CanMessage_t *Message);
static Error_t rfid11785WriteUserData (UInt16 Channel, CanMessage_t *Message);
static Error_t rfid11785WritePassword (UInt16 Channel, CanMessage_t *Message);
static Error_t rfid11785WriteConfig   (UInt16 Channel, CanMessage_t *Message);

static Error_t rfid11785ReadUserData  (UInt16 Channel, CanMessage_t *Message);
static Error_t rfid11785ReadUidNumber (UInt16 Channel, CanMessage_t *Message);

static void rfid11785InterruptHandler (UInt32 Channel, UInt32 IntrFlags);
static Error_t rfid11785CompareHandler (InstanceData_t *Data);
static Error_t rfid11785CaptureHandler (InstanceData_t *Data);

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

static Error_t rfid11785ModuleControl (UInt16 Instance, bmModuleControlID_t ControlID)
{
    InstanceData_t* Data = &DataTable[Instance];

    switch (ControlID)
    {
        case MODULE_CONTROL_RESUME:
        case MODULE_CONTROL_WAKEUP:
            Data->ModuleState = MODULE_STATE_READY;
            if ((Data->Flags & MODE_MODULE_ENABLE) != 0) {
                return halPortWrite(Data->HandleShdOut, 0);
            }
            break;

        case MODULE_CONTROL_STOP:
            Data->ModuleState = MODULE_STATE_STOPPED;
            return halPortWrite(Data->HandleShdOut, 1);

        case MODULE_CONTROL_SHUTDOWN:
            Data->ModuleState = MODULE_STATE_STANDBY;
            return halPortWrite(Data->HandleShdOut, 1);

        case MODULE_CONTROL_RESET:
            Data->Flags &= ~MODE_MODULE_ENABLE;
            return halPortWrite(Data->HandleShdOut, 1);
                                
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

static Error_t rfid11785ModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID)
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
            return (Data->DeviceSelect);
        
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
 *  \riskid  SWRA 5.5.1: Unreliable and wrong recognition of racks
 *
 *      This function is called by the task scheduler periodically. 
 *      It's purpose is to perform all actions to provide the modules
 *      functionality.
 * 
 *  \iparam  Instance = Instance number of this module
 *
 *  \return  Module instance state or (negative) error code
 *
 *****************************************************************************/

static Error_t rfid11785ModuleTask (UInt16 Instance)
{
    Int8 Result;
    Error_t Error;
    InstanceData_t* Data = &DataTable[Instance];

    if (Data->ModuleState == MODULE_STATE_BUSY) {
        if ((bmTimeExpired (Data->SwitchTime) >= ANTENNA_SWITCH_INTERVAL) && (Data->Started == FALSE)) {
            if ((Error = rfid11785StartModOutput (Data)) < 0) {
                Data->ModuleState = MODULE_STATE_READY;
                bmSignalEvent (Data->Channel, Error, TRUE, 0);
                return ((Error_t) Data->ModuleState);
            }
            Data->Started = TRUE;
        }

        Result = rfid11785LinkComplete (&Data->DataStream);
        if (Result == 1) {
            Data->ModuleState = MODULE_STATE_READY;
            Result = rfid11785LinkPatternOk (&Data->DataStream);
            if (Result == 1) {
                if (Data->DataStream.ReadCommand == TRUE) {
                    if (rfid11785LinkParityOk (&Data->DataStream) == FALSE) {
                        bmSignalEvent (Data->Channel, E_RFID11785_PARITY_ERROR, TRUE, 0);
                        return ((Error_t) Data->ModuleState);
                    }
                    else {
                        Error = rfid11785RespondAck (Data->Channel, Data->Ongoing, Data->CurrentAddress,
                            rfid11785LinkGetData (&Data->DataStream));
                        if (Error < NO_ERROR) {
                            bmSignalEvent (Data->Channel, Error, TRUE, 0);
                            return ((Error_t) Data->ModuleState);
                        }
                    }
                }
                else {
                    Error = rfid11785RespondAck (Data->Channel, Data->Ongoing, Data->CurrentAddress, 0);
                    if (Error < NO_ERROR) {
                        bmSignalEvent (Data->Channel, Error, TRUE, 0);
                        return ((Error_t) Data->ModuleState);
                    }
                }
            }
            else if (Result == 0) {
                bmSignalEvent (Data->Channel, E_RFID11785_ERROR_PATTERN, TRUE, 0);
                return ((Error_t) Data->ModuleState);
            }
            else {
                bmSignalEvent (Data->Channel, E_RFID11785_UNKNOWN_PATTERN, TRUE, 0);
                return ((Error_t) Data->ModuleState);
            }
        }
        else if (Result == -1) {
            Data->ModuleState = MODULE_STATE_READY;
            bmSignalEvent (Data->Channel, E_RFID11785_TRANSACTION_TIMEOUT, TRUE, 0);
            return ((Error_t) Data->ModuleState);
        }
        else if (Result == -2) {
            Data->ModuleState = MODULE_STATE_READY;
            bmSignalEvent (Data->Channel, Data->DataStream.IrqError, TRUE, 0);
            return ((Error_t) Data->ModuleState);
        }
    }

    return ((Error_t) Data->ModuleState);
}


/*****************************************************************************/
/*! 
 *  \brief  Indicates a successful RFID transaction
 *
 *      This function sends a CAN message that shows that an RFID transaction
 *      completed successfully. In case of a read transaction, the data read
 *      from the device is also transmitted. Write or configuration responses
 *      do not contain any data.
 * 
 *  \iparam Channel = Channel of a instance of the functional module
 *  \iparam Ongoing = Ongoing RFID command
 *  \iparam Address = Address of the ongoing transaction
 *  \iparam ReadData = Data word returned to the master
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
static Error_t rfid11785RespondAck (UInt16 Channel, Rfid11785Command_t Ongoing, UInt8 Address, UInt32 ReadData)
{
    CanMessage_t Message;
    
    switch (Ongoing) {
        case SetConfig:
            Message.CanID = MSG_RFID11785_ACK_CONFIG;
            Message.Length = 0;
            break;
        case Login:
            Message.CanID = MSG_RFID11785_RESP_LOGIN;
            Message.Length = 0;
            break;
        case WriteUserData:
            Message.CanID = MSG_RFID11785_RESP_WRITE_USER_DATA;
            Message.Length = 1;
            bmSetMessageItem (&Message, Address, 0, 1);
            break;
        case WritePassword:
            Message.CanID = MSG_RFID11785_RESP_WRITE_PASSWORD;
            Message.Length = 0;
            break;
        case WriteConfig:
            Message.CanID = MSG_RFID11785_RESP_WRITE_CONFIG;
            Message.Length = 0;
            break;
        case ReadUserData:
            Message.CanID = MSG_RFID11785_RESP_READ_USER_DATA;
            Message.Length = 5;
            bmSetMessageItem (&Message, Address, 0, 1);
            bmSetMessageItem (&Message, ReadData, 1, 4);
            break;
        case ReadUidNumber:
            Message.CanID = MSG_RFID11785_RESP_READ_UID_NUMBER;
            Message.Length = 4;
            bmSetMessageItem (&Message, ReadData, 0, 4);
            break;
    }
    
    return (canWriteMessage (Channel, &Message)); 
}


/*****************************************************************************/
/*! 
 *  \brief  Starts the modulation output responsible for RFID transmissions
 *
 *      This function starts the hardware timer that generates the interrupts
 *      that drive the modulation output signal to the RFID front end IC. It
 *      also sets the first signal state which will be active until the next
 *      timer interrupt occurs. It sets the timer, enables the timer interrupt
 *      and starts it.
 * 
 *  \xparam  Data = Main data structure of a module instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785StartModOutput (InstanceData_t *Data)
{
    Error_t Error;
    
    Error = halPortWrite (Data->HandleModOut, 1);
    if (Error < NO_ERROR) {
        return (Error);
    }

    Error = halTimerRead (Data->HandleTimer, TIM_REG_COUNTER, &Data->AccTime);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Data->AccTime = Data->TimeMask & (Data->AccTime + RFID11785_TIME_FIRST_STOP);
    Error = halCapComWrite (Data->HandleTimer, RFID11785_COMPARE_CHANNEL, Data->AccTime);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Error = halCapComControl (Data->HandleTimer, RFID11785_COMPARE_CHANNEL, TIM_INTR_ENABLE);
    if (Error < 0) {
        return (Error);
    } 
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief  Sets the modulation output responsible for RFID transmissions
 *
 *      This function sets the hardware timer that generates the interrupts
 *      that drive the modulation output signal to the RFID front end IC. It
 *      also sets the signal state which will be active until the next timer
 *      interrupt occurs.
 * 
 *  \xparam  Data = Main data structure of a module instance
 *  \iparam  Count = Number of timer cycles
 *  \iparam  Value = Digital output value
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785SetModOutput (InstanceData_t *Data, UInt32 Count, UInt16 Value)
{
    Error_t Error;
        
    Error = halPortWrite(Data->HandleModOut, Value);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Data->AccTime = Data->TimeMask & (Data->AccTime + Count);
    Error = halCapComWrite (Data->HandleTimer, RFID11785_COMPARE_CHANNEL, Data->AccTime);
    if (Error < NO_ERROR) {
        return (Error);
    }
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief  Outputs the antenna select signals
 *
 *      This method converts a number into the corresponding antenna select
 *      signals and writes this information to the digital output port.
 * 
 *  \iparam  Handle = Handle of the output port
 *  \iparam  DeviceSelect = Antenna to be selected
 *  \iparam  AntennaNumber = Number of antennas
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785SetDeviceSel (Handle_t Handle, UInt8 DeviceSelect, UInt8 AntennaNumber)
{
    UInt8 Code;
    
    if (DeviceSelect >= AntennaNumber) {
        return (E_PARAMETER_OUT_OF_RANGE);
    }

    if (AntennaNumber <= 1) {
        return (NO_ERROR);
    }
    else if (AntennaNumber <= 5) {
        switch (DeviceSelect) {
            case 0:
                Code = 0;
                break;
            case 1:
                Code = 1;
                break;
            case 2:
                Code = 4;
                break;
            case 3:
                Code = 6;
                break;
            case 4:
                Code = 8;
                break;
            default:
                return (E_PARAMETER_OUT_OF_RANGE);
        }
    }
    else {
        switch (DeviceSelect) {
            case 0:
                Code = 0;
                break;
            case 1:
                Code = 2;
                break;
            case 2:
                Code = 8;
                break;
            case 3:
                Code = 12;
                break;
            case 4:
                Code = 16;
                break;
            case 5:
                Code = 18;
                break;
            case 6:
                Code = 24;
                break;
            case 7:
                Code = 28;
                break;
            case 8:
                Code = 1;
                break;
            default:
                return (E_PARAMETER_OUT_OF_RANGE);
        }
    }

    return (halPortWrite(Handle, Code));
}


/*****************************************************************************/
/*! 
 *  \brief  Sets the antenna multiplexer input and the RFID data rate
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      setting the antenna multiplexer input received from the master. The
 *      message also sets the current data rate of the module, the data rate
 *      of the RFID transponder is set by the "Write RFID Configuration"
 *      message. The parameters in the message are transfered to the data
 *      structure of the addressed module instance. The modified settings
 *      influence the behavior of the module task. The following settings will
 *      be modified:
 *
 *      - Antenna multiplexer select input
 *      - Data rate used by the functional module
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785SetConfig (UInt16 Channel, CanMessage_t* Message)
{
    Error_t Error;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        if (Data->ModuleState == MODULE_STATE_READY) {
            Data->Flags = bmGetMessageItem(Message, 0, 1);
            Data->DeviceSelect = (Data->Flags & 0x3C) >> 2;

            // Activate or deactivate the shutdown signal to the RFID frontend IC
            if((Data->Flags & MODE_MODULE_ENABLE) != 0) {           
                if((Error = halPortWrite(Data->HandleShdOut, 0)) < 0) {
                    return (Error);
                }
            }
            else {
                if((Error = halPortWrite(Data->HandleShdOut, 1)) < 0) {
                    return (Error);
                }
            }

            // Set the antenna selection pins
            if((Error = rfid11785SetDeviceSel(Data->HandleDevSelOut, Data->DeviceSelect, Data->AntennaNumber)) < 0) {
                return (Error);
            }

            switch ((Data->Flags & 0xC0) >> 6) {
                // RF / 64
                case 0:
                    Data->CycleNumber = 64;
                    break;
                // RF / 32
                case 1:
                    Data->CycleNumber = 32;
                    break;
                // RF / 16
                case 2:
                    Data->CycleNumber = 16;
                    break;
                // RF / 8
                case 3:
                    Data->CycleNumber = 8;
                    break; 
                default: break;
            }

            Data->SwitchTime = bmGetTime();
            return (rfid11785RespondAck (Channel, SetConfig, 0, 0));
        }
        return (E_RFID11785_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief  Isssues a login message to the RFID transponder
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      issuing an RFID login transaction is received from the master. The
 *      parameters in the message are transfered to the data structure of the
 *      addressed module instance. The modified settings influence the
 *      behavior of the module task. The following settings will be modified:
 *
 *      - RFID transponder password
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785SendLogin (UInt16 Channel, CanMessage_t* Message)
{
    UInt32 Password;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 4) {
        if (Data->ModuleState == MODULE_STATE_READY && (Data->Flags & MODE_MODULE_ENABLE) != 0) {
            Password = bmGetMessageItem(Message, 0, 4);
            
            Data->Ongoing = Login;
            rfid11785LinkStartLogin(Password, &Data->DataStream);
            Data->ModuleState = MODULE_STATE_BUSY;
            Data->Started = FALSE;
            return (NO_ERROR);
        }
        return (E_RFID11785_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief  Sets a user register of the RFID transponder
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      setting a user register is received from the master. The parameters in
 *      the message are transfered to the data structure of the addressed
 *      module instance. The modified settings influence the behavior of the
 *      module task. The following settings will be modified:
 *
 *      - User data address
 *      - User data
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785WriteUserData (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Address;
    UInt32 DataWord;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 5) {
        if (Data->ModuleState == MODULE_STATE_READY && (Data->Flags & MODE_MODULE_ENABLE) != 0) {
            Address = bmGetMessageItem(Message, 0, 1);
            DataWord = bmGetMessageItem(Message, 1, 4);
            if(Address >= ADDR_USER_MAX) {
                return (E_PARAMETER_OUT_OF_RANGE);
            }
 
            Data->CurrentAddress = Address;
            Data->Ongoing = WriteUserData;
            rfid11785LinkStartWrite(Address + ADDR_USER, DataWord, &Data->DataStream);
            Data->ModuleState = MODULE_STATE_BUSY;
            Data->Started = FALSE;
            return (NO_ERROR);
        }
        return (E_RFID11785_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief  Sets the password register of the RFID transponder
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      setting the password register is received from the master. The
 *      parameters in the message are transfered to the data structure of the
 *      addressed module instance. The modified settings influence the
 *      behavior of the module task. The following settings will be modified:
 *
 *      - RFID transponder password
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785WritePassword (UInt16 Channel, CanMessage_t* Message)
{
    UInt32 Password;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 4) {
        if (Data->ModuleState == MODULE_STATE_READY && (Data->Flags & MODE_MODULE_ENABLE) != 0) {
            Password = bmGetMessageItem(Message, 0, 4);

            Data->Ongoing = WritePassword;
            rfid11785LinkStartWrite(ADDR_PASSWORD, Password, &Data->DataStream);
            Data->ModuleState = MODULE_STATE_BUSY;
            Data->Started = FALSE;
            return (NO_ERROR);
        }
        return (E_RFID11785_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief  Sets the config register of the RFID transponder
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      setting some bits of the the config register of the RFID transponder
 *      is received from the master. This message sets the login bits and
 *      the data rate parameter in the configuration register. When this
 *      message has been issued, the data rate of this module should also be
 *      changed with the "set login parameter" message. The parameters in the
 *      message are transfered to the data structure of the addressed module
 *      instance. The modified  settings influence the behavior of the module
 *      task. The following settings will be modified:
 *
 *      - Read login bit
 *      - Write login bit
 *      - Data rate written to config register
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785WriteConfig (UInt16 Channel, CanMessage_t* Message)
{
    UInt32 Mode;
    UInt32 ConfigWord;
    
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        if (Data->ModuleState == MODULE_STATE_READY && (Data->Flags & MODE_MODULE_ENABLE) != 0) {
            Mode = bmGetMessageItem(Message, 0, 1);
 
            // Set Bi-phase encoding bit
            ConfigWord = 0x80;
            // Read login bit
            ConfigWord |= (Mode & 0x1) << 18;
            // Write login bit
            ConfigWord |= (Mode & 0x2) << 19;
            // Reader talk first bit
            ConfigWord |= (Mode & 0x4) << 22;

            switch ((Mode & 0x30) >> 4) {
                // RF / 64
                case 0:
                    ConfigWord |= 0x1f;
                    break;
                // RF / 32
                case 1:
                    ConfigWord |= 0xf;
                    break;
                // RF / 16
                case 2:
                    ConfigWord |= 0x7;
                    break;
                // RF / 8
                case 3:
                    ConfigWord |= 0x3;
                    break;
                default: break;
            }
           
            Data->Ongoing = WriteConfig;
            rfid11785LinkStartWrite(ADDR_CONFIG, ConfigWord, &Data->DataStream);
            Data->ModuleState = MODULE_STATE_BUSY;
            Data->Started = FALSE;
            return (NO_ERROR);
        }
        return (E_RFID11785_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief   Requests the data from a user register of the RFID transponder
 *
 *      This function is called by the CAN message dispatcher when a user
 *      register get message is received from the master. As a response a RFID
 *      data word message is sent back to the master. The following settings
 *      will be responded:
 *
 *      - User data
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785ReadUserData (UInt16 Channel, CanMessage_t* Message)
{   
    UInt8 Address;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        if (Data->ModuleState == MODULE_STATE_READY && (Data->Flags & MODE_MODULE_ENABLE) != 0) {
            Address = bmGetMessageItem(Message, 0, 1);
            if(Address >= ADDR_USER_MAX) {
                return (E_PARAMETER_OUT_OF_RANGE);
            }
            
            Data->CurrentAddress = Address;
            Data->Ongoing = ReadUserData;
            rfid11785LinkStartRead(Address + ADDR_USER, &Data->DataStream);
            Data->ModuleState = MODULE_STATE_BUSY;
            Data->Started = FALSE;
            return (NO_ERROR);
        }
        return (E_RFID11785_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}

/*****************************************************************************/
/*! 
 *  \brief   Requests the UID of the RFID transponder
 *
 *      This function is called by the CAN message dispatcher when a UID
 *      register get message is received from the master. As a response the
 *      UID number is sent back to the master. The following settings will
 *      be responded:
 *
 *      - Unique identification number
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t rfid11785ReadUidNumber (UInt16 Channel, CanMessage_t* Message)
{   
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 0) {
        if (Data->ModuleState == MODULE_STATE_READY && (Data->Flags & MODE_MODULE_ENABLE) != 0) {
            Data->Ongoing = ReadUidNumber;
            rfid11785LinkStartRead(ADDR_UID, &Data->DataStream);
            Data->ModuleState = MODULE_STATE_BUSY;
            Data->Started = FALSE;
            return (NO_ERROR);
        }
        return (E_RFID11785_TRANSACTION_ACTIVE);
    }
    return (E_MISSING_PARAMETERS);
}

/*****************************************************************************/
/*! 
 *  \brief   Interrupt handler issued by a timer unit
 *
 *      The interrupt handler is called when the counter of a timer unit
 *		reaches a specified compare value or when an event is captured on a
 *		timer channel. The handler sets the MOD signal to the RFID front end
 *      and this way controls the transmission of RFID messages. When the
 *      message transfer completes successfully, the capture interrupt
 *		responsible for the reception of messages is activated.
 * 
 *  \iparam  Channel = user infomation
 *  \iparam  IntrFlags = interrupt flags
 *
 ****************************************************************************/

static void rfid11785InterruptHandler (UInt32 Channel, UInt32 IntrFlags)
{
    Error_t Error;
    InstanceData_t *Data = &DataTable[Channel];

    // Handling the compare interrupt
    Error = halCapComStatus (Data->HandleTimer, RFID11785_COMPARE_CHANNEL, TIM_STAT_PENDING);
    if (Error < 0) {
        Data->DataStream.IrqError = Error;
    }
    else if (Error > 0) {
        Data->DataStream.IrqError = rfid11785CompareHandler (Data);
    }
    
    // Handling the capture interrupt
    Error = halCapComStatus (Data->HandleTimer, RFID11785_CAPTURE_CHANNEL, TIM_STAT_PENDING);
    if (Error < 0) {
        Data->DataStream.IrqError = Error;
    }
    else if (Error > 0) {
        Data->DataStream.IrqError = rfid11785CaptureHandler (Data);
    }
        
    // Deactivate the interrupts in case of an error
    if (Data->DataStream.IrqError < NO_ERROR) {
        Error = halCapComControl (Data->HandleTimer, RFID11785_COMPARE_CHANNEL, TIM_INTR_DISABLE);
        if (Error < NO_ERROR) {
            Data->DataStream.IrqError = Error;
        }
        Error = halCapComControl (Data->HandleTimer, RFID11785_CAPTURE_CHANNEL, TIM_INTR_DISABLE);
        if (Error < NO_ERROR) {
            Data->DataStream.IrqError = Error;
        }
    }
            
    return;
}

/*****************************************************************************/
/*! 
 *  \brief   Handler for the compare channel
 *
 *      The function is called by the timer interrupt handler, when the
 *      compare timer reaches a certain value. The handler sets the MOD signal
 *      to the RFID front end and this way controls the transmission of RFID
 *      messages.
 * 
 *  \iparam  Data = Instance data structure
 *
 ****************************************************************************/

static Error_t rfid11785CompareHandler (InstanceData_t *Data)
{
    Error_t Error;
    Rfid11785Stream_t *DataStream;

    DataStream = &Data->DataStream;
    
    if (DataStream->TxCount < DataStream->TxLength) {
        if (DataStream->CycleCount == 0) {
            Error = rfid11785SetModOutput (Data, RFID11785_TIME_MOD_OFF, 0);
            if (Error < NO_ERROR) {
                return (Error);
            }
            DataStream->CycleCount++;
        }
        else {
            if(rfid11785LinkReadBit (DataStream->TxBuffer, DataStream->TxCount) == 0) {
                Error = rfid11785SetModOutput (Data, RFID11785_TIME_MOD_ON, 1);
                if (Error < NO_ERROR) {
                    return (Error);
                }
            }
            else {
                Error = rfid11785SetModOutput (Data, RFID11785_TIME_MOD_ON, 0);
                if (Error < NO_ERROR) {
                    return (Error);
                }
            }
            DataStream->CycleCount = 0;
            DataStream->TxCount++;
        }
    }
    else if (DataStream->RespWait != 0) {
        Error = rfid11785SetModOutput (Data, DataStream->RespWait, 0);
        if (Error < NO_ERROR) {
            return (Error);
        }
        DataStream->RespWait = 0;
    }
    else {
        DataStream->OldTime = 0;
        DataStream->OldCount = 0;
        Error = halCapComControl (Data->HandleTimer, RFID11785_COMPARE_CHANNEL, TIM_INTR_DISABLE);
        if (Error < NO_ERROR) {
            return (Error);
        }
        Error = halCapComRead (Data->HandleTimer, RFID11785_CAPTURE_CHANNEL, &DataStream->OldCount);
        if (Error < 0 && Error != E_TIMER_CAPCOM_OVERFLOW) {
            return Error;
        }
        Error = halCapComControl (Data->HandleTimer, RFID11785_CAPTURE_CHANNEL, TIM_INTR_ENABLE);
        if (Error < NO_ERROR) {
            return (Error);
        }
    }
    
    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief   Handler issued by the capture channel
 *
 *      The function is called by the timer interrupt handler, when the
 *      capture channel detects a transition on the DEMOD signal coming from
 *      the RFID front end. The handler measures the time since the last
 *      transition. This way, it is able to identify single bits in the
 *      incoming data stream. When the message transfer completes
 *      successfully, the timer interrupt is deactivated again.
 * 
 *  \iparam  Data = Instance data structure
 *
 ****************************************************************************/

static Error_t rfid11785CaptureHandler (InstanceData_t *Data)
{
    Error_t Error;
    UInt32 Time;
    UInt16 BitTime;
    UInt32 NewCount;
    Rfid11785Stream_t *DataStream;
    
    BitTime = Data->CycleNumber / 8;
    DataStream = &Data->DataStream;

    Error = halCapComRead (Data->HandleTimer, RFID11785_CAPTURE_CHANNEL, &NewCount);
    if (Error < 0) {
        return Error;
    }
    Time = Data->TimeMask & (NewCount - DataStream->OldCount);
    DataStream->OldCount = NewCount;

    if (DataStream->RxCount < DataStream->RxLength) {
        if (Time > (7 * BitTime) && Time < (9 * BitTime)) {
            rfid11785LinkWriteBit(DataStream->RxBuffer, DataStream->RxCount, 0);
            DataStream->RxCount++;
        }
        else if (Time > (11 * BitTime) && Time < (13 * BitTime)) {
            if (DataStream->OldTime % 2 == 0) {
                rfid11785LinkWriteBit(DataStream->RxBuffer, DataStream->RxCount, 1);
                DataStream->RxCount++;
                DataStream->OldTime++;
            }
            else {
                rfid11785LinkWriteBit(DataStream->RxBuffer, DataStream->RxCount, 1);
                DataStream->RxCount++;
                if (DataStream->RxCount < DataStream->RxLength) {
                    rfid11785LinkWriteBit(DataStream->RxBuffer, DataStream->RxCount, 0);
                    DataStream->RxCount++;
                }
                DataStream->OldTime++;
            }
        }
        else if (Time > (15 * BitTime) && Time < (17 * BitTime)) {
            rfid11785LinkWriteBit(DataStream->RxBuffer, DataStream->RxCount, 1);
            DataStream->RxCount++;
            if (DataStream->RxCount < DataStream->RxLength) {
                rfid11785LinkWriteBit(DataStream->RxBuffer, DataStream->RxCount, 1);
                DataStream->RxCount++;
            }
        }
    }
    
    if (DataStream->RxCount == DataStream->RxLength - rfid11785LinkGetMissing(DataStream)) {
        Error = halCapComControl (Data->HandleTimer, RFID11785_CAPTURE_CHANNEL, TIM_INTR_DISABLE);
        if (Error < NO_ERROR) {
            return (Error);
        }
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

static Error_t rfid11785HandleOpen (InstanceData_t *Data, Int16 Instance)
{
    Error_t Error;
    TimerMode_t TimerMode;
    
    TimerMode.Direction = TIM_MODE_COUNT_UP;
    TimerMode.OneShot = TIM_MODE_INTERVAL;
    TimerMode.ClkSource = TIM_MODE_EXTERNAL;
    TimerMode.ClkMode = TIM_MODE_ENCODER_3 | TIM_MODE_ENCODER_2 | TIM_MODE_ENCODER_1;

    // Open digital ports for RFID control signals
    Data->HandleShdOut = halPortOpen (HAL_RFID11785_SHD + Instance, HAL_OPEN_WRITE);
    if (Data->HandleShdOut < 0) {
        return (Data->HandleShdOut);
    }
    Data->HandleModOut = halPortOpen (HAL_RFID11785_MOD + Instance, HAL_OPEN_WRITE);
    if (Data->HandleModOut < 0) {
        return (Data->HandleModOut);
    }
    if (Data->AntennaNumber > 1) {
        Data->HandleDevSelOut = halPortOpen (HAL_RFID11785_ANT + Instance, HAL_OPEN_WRITE);
        if (Data->HandleDevSelOut < 0) {
            return (Data->HandleDevSelOut);
        }
    }
    
    // Open hardware timer for the RFID transmission
    Data->HandleTimer = halTimerOpen (HAL_RFID11785_TIM + Instance, Instance, rfid11785InterruptHandler);
    if (Data->HandleTimer < 0) {
        return (Data->HandleTimer);
    }
    Error = halTimerSetup (Data->HandleTimer, &TimerMode, 1);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Error = halTimerStatus (Data->HandleTimer, TIM_STAT_MAXCOUNT);
    if (Error < NO_ERROR) {
        return (Error);
    }
    Data->TimeMask = Error;
    Error = halTimerControl (Data->HandleTimer, TIM_CTRL_START);
    if (Error < NO_ERROR) {
        return (Error);
    }
    
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

Error_t rfid11785InitializeModule (UInt16 ModuleID, UInt16 Instances)
{
    static bmCallbackEntry_t Commands[] = {
        { MSG_RFID11785_SET_CONFIG, rfid11785SetConfig },
        { MSG_RFID11785_SEND_LOGIN, rfid11785SendLogin },
        { MSG_RFID11785_WRITE_USER_DATA, rfid11785WriteUserData },
        { MSG_RFID11785_WRITE_PASSWORD, rfid11785WritePassword },
        { MSG_RFID11785_WRITE_CONFIG, rfid11785WriteConfig },
        { MSG_RFID11785_READ_USER_DATA, rfid11785ReadUserData },
        { MSG_RFID11785_READ_UID_NUMBER, rfid11785ReadUidNumber },
    };
    
    static bmModuleInterface_t Interface = {
        rfid11785ModuleTask, 
        rfid11785ModuleControl, 
        rfid11785ModuleStatus
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
    rfid11785RegisterDebugNames(ModuleID);
    #endif

    // open channels and ports for all modules
    for (i=0; i < Instances; i++) {
        
        DataTable[i].Channel = bmGetChannel(bmGetTaskID(ModuleID, i));
        DataTable[i].Flags = 0;

        // Get the number of antennas connected to the board
        DataTable[i].AntennaNumber = bmGetBoardOptions (ModuleID, i, 1);
        if (DataTable[i].AntennaNumber == 0) {
            return (E_BOARD_OPTION_MISSING);
        }
 
        // Open the HAL device handlers
        Status = rfid11785HandleOpen (&DataTable[i], i);
        if (Status < 0) {
            return (Status);
        }
 
        // Setting default RFID transission speed
        DataTable[i].CycleNumber = 32;
        DataTable[i].SwitchTime = bmGetTime();
        DataTable[i].ModuleState = MODULE_STATE_READY;
    }
    InstanceCount = Instances;
    ModuleIdentifier = ModuleID;
    
    return (NO_ERROR);
}

//****************************************************************************/

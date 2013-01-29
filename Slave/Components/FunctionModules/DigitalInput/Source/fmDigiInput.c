/****************************************************************************/
/*! \file fmDigiInput.c
 *
 *  \brief Function Module for Digital Inputs
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *         This module contains all functions to control and read a digital
 *         input port. A port can be 1...32 bits wide. The function module
 *         uses logical ports; the mapping to a real hardware port is done
 *         by the hardware abstraction layer. Also the width of the port is
 *         determined by the HAL and can't be changed here.
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

#include <stdlib.h>
#include "Global.h"
#include "Basemodule.h"
#include "fmDigiInput.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define DI_MODULE_VERSION       0x0002  //!< Version number of module
#define DI_DEFAULT_SAMPLE_RATE  10      //!< Default sample rate (ms)
#define DI_DEFAULT_SAMPLE_COUNT 1       //!< Default sample count

/*! Mode bits for the Flags member of the module instance data */
typedef union {
    struct {
        UInt8 Reserved:6;       //!< Reserved for future use
        Bool UseTimestamp:1;    //!< Timestamp enable bit
        Bool ModuleEnable:1;    //!< Module enable bit
    } Bits;                     //!< Bit access
    UInt8 Byte;                 //!< Byte access
} diInstanceFlags_t;

/*! Limit mode bits for the Flags member of the module instance data */
/*! \remark This function is currently not used. */
typedef union {
    struct {
        Bool CheckOverrun:1;    //!< Enable overrun checking
        Bool CheckUnderrun:1;   //!< Enable underrun checking
        UInt8 Reserved:2;       //!< Reserved for future use
        Bool SendWarning:1;     //!< Enable warnings
        Bool SendData:1;        //!< Enable sending data
    } Bits;                     //!< Bit access
    UInt8 Byte;                 //!< Byte access
} diLimitFlags_t;

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Specifies a limit for monitoring the port input values
typedef struct {
    diLimitFlags_t Flags;   //!< Mode control flag bits
    Bool AboveFlag;         //!< Input is above limit
    UInt16 Limit;           //!< Limit to supervise
    UInt16 Delta;           //!< Hysterese to prevent bouncing
} diInputLimit_t;


//! Contains all variables for a instance of this module
typedef struct {
    bmModuleState_t ModuleState;    //!< Module state
    Handle_t Handle;                //!< Handle to access port (HAL)
    UInt16 Channel;                 //!< Logical CAN channel
    diInstanceFlags_t Flags;        //!< Mode control flag bits
    Bool CheckLimits;               //!< Limit check enable
    Bool OldValueValid;             //!< Old value valid flag

    UInt32 SampleRate;              //!< Sample rate (ms)
    UInt32 SampleTime;              //!< Time of last sampling
    UInt16 SampleCount;             //!< Debouncing counter
    UInt16 Samples;                 //!< Required number of samples

    UInt16 CurValue;                //!< Actual state of input
    UInt16 OldValue;                //!< Input value last sent
    UInt16 TmpValue;                //!< Input value in transition
    UInt16 Polarity;                //!< Data polarity (exor mask)
    UInt16 Threshold;               //!< Min delta before sending
    UInt32 TimeStamp;               //!< Time of last state change

    diInputLimit_t Limits[2];       //!< Limits to monitor
} diInstanceData_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

static UInt16 diInstanceCount = 0;      //!< Number of module instances
static UInt16 diModuleID = 0;           //!< Module identifier

static diInstanceData_t *diDataTable;   //!< Data table for all instances

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t diModuleControl  (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t diModuleStatus   (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t diModuleTask     (UInt16 Instance);

static Error_t diReqInputValue  (UInt16 Channel, CanMessage_t *Message);
static Error_t diConfigureInput (UInt16 Channel, CanMessage_t *Message);
static Error_t diConfigureLimit (UInt16 Channel, CanMessage_t *Message);

static Error_t diSendInputValue (diInstanceData_t *Data);
static Error_t diGetStableInput (diInstanceData_t *Data);
static Bool    diMonitorLimits  (diInstanceData_t *Data);

static void diResetInstanceData(UInt16 Instance);


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
 *      - MODULE_CONTROL_WAKEUP
 *      - MODULE_CONTROL_SHUTDOWN
 *      - MODULE_CONTROL_FLUSH_DATA
 *      - MODULE_CONTROL_RESET_DATA
 *      - MODULE_CONTROL_RESET
 *
 *  \iparam  Instance  = Instance number of this module
 *  \iparam  ControlID = Control code to select sub-function
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t diModuleControl (UInt16 Instance, bmModuleControlID_t ControlID) {

    diInstanceData_t *Data = &diDataTable[Instance];

    switch (ControlID) {

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
            diResetInstanceData(Instance);
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
 *      - MODULE_STATUS_VERSION
 *      - MODULE_STATUS_STATE
 *      - MODULE_STATUS_VALUE
 *
 *  \iparam  Instance = Instance number of this module
 *  \iparam  StatusID = selects which status is requested
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t diModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID) {

    diInstanceData_t *Data = &diDataTable[Instance];

    switch (StatusID) {

        case MODULE_STATUS_STATE:
            if (Data->Flags.Bits.ModuleEnable == FALSE) {
                return (MODULE_STATE_DISABLED);
            }
            return (Data->ModuleState);

        case MODULE_STATUS_VALUE:
            return (Data->CurValue);

        case MODULE_STATUS_MODULE_ID:
            return (diModuleID);

        case MODULE_STATUS_INSTANCES:
            return (diInstanceCount);

        case MODULE_STATUS_VERSION:
            return (DI_MODULE_VERSION);
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
 *  \return  State of instance or (negative) error code
 *
 *****************************************************************************/

static Error_t diModuleTask (UInt16 Instance) {

    diInstanceData_t *Data = &diDataTable[Instance];
    UInt16 DataRequests = 0;
    Error_t Status;
    UInt16 Delta;

    if (Data->Flags.Bits.ModuleEnable == TRUE) {
        if (bmTimeExpired(Data->SampleTime) >= Data->SampleRate) {
            Data->SampleTime = bmGetTime();
            Status = diGetStableInput(Data);
            if (Status < NO_ERROR) {
                bmSignalEvent(Data->Channel, Status, TRUE, 0);
                return (Status);
            }
            Delta = Status;

            if (Data->SampleCount >= Data->Samples) {
                // debouncing done; take over new value
                if (Data->Threshold) {
                    if (Delta >= Data->Threshold) {
                        if (Data->OldValueValid == TRUE) {
                            DataRequests++;
                        }
                        else {
                            Data->OldValue = Data->CurValue;
                            Data->OldValueValid = TRUE;
                        }
                    }
                }
                if (Data->CheckLimits == TRUE) {
                    if (diMonitorLimits (Data)) {
                        DataRequests++;
                    }
                }
                if (Data->Flags.Bits.UseTimestamp == TRUE) {
                    Data->TimeStamp = bmGetSecondsOfTheDay();
                }
                if (DataRequests) {
                    diSendInputValue (Data);
                }
                Data->SampleCount = 0;
            }
            Data->SampleTime = bmGetTime();
        }
        return (Data->ModuleState);
    }
    return (MODULE_STATE_DISABLED);
}


/*****************************************************************************/
/*!
 *  \brief   Reads analog input channel and filters it.
 *
 *      Reads the analog input value, saves it in the data history buffer,
 *      calculates the mean value using the history buffer and stores the
 *      result in the given data structure. The variable "Data->Filter"
 *      defines how many entries in the history buffer will be taken
 *      into account for averaging.
 *
 *      Returns the difference of the averaged input value and the last
 *      sent value.
 *
 *  \xparam  Data = Pointer to module instance's data
 *
 *  \return  Difference between new and last value or (negative) error code
 *
 *****************************************************************************/

static Error_t diGetStableInput (diInstanceData_t *Data) {

    UInt16 NewValue;
    Error_t Status;

    Status = halPortRead (Data->Handle, &NewValue);
    if (Status < NO_ERROR) {
        return (Status);
    }
    NewValue ^= Data->Polarity;

    if (NewValue == Data->CurValue || NewValue != Data->TmpValue) {
        Data->SampleCount = 0;
    }
    if (++Data->SampleCount >= Data->Samples) {
        Data->CurValue = NewValue;
    }
    Data->TmpValue = NewValue;

    return (bmGetDelta (Data->OldValue, Data->CurValue));
}


/*****************************************************************************/
/*!
 *  \brief   Monitors the user supplied limits
 *
 *  \remark  This function is currently not used.
 *
 *      Checks if the current input value has rised above or fallen below one
 *      of the two user supplied limits. If this is the case, an appropriate
 *      warning message is send (if enabled) and/or the sending of a data
 *      message is requested (if enabled).
 *
 *      Returns the "send data" request. The caller has to send the actual
 *      input data if this flag is TRUE.
 *
 *  \xparam  Data = Pointer to module instance's data
 *
 *  \return  Data send request
 *
 *****************************************************************************/

static Bool diMonitorLimits (diInstanceData_t *Data) {

    Bool SendRequest = FALSE;
    diInputLimit_t *Limits;
    Error_t Status;
    UInt16 i;

    for (i=0; i < ELEMENTS(Data->Limits); i++) {

        Status = NO_ERROR;  // reset status variable
        Limits = &Data->Limits[i];

        if (Limits->AboveFlag == FALSE) {
            // warning if input value rises above limit
            if (Data->CurValue > Limits->Limit + Limits->Delta) {
                if (Limits->Flags.Bits.CheckOverrun == TRUE) {
                    Status = W_DIGIN_ABOVE_LIMIT;
                }
                Limits->AboveFlag = TRUE;
            }
        }
        if (Limits->AboveFlag == TRUE) {
            // warning if input value falls below limit
            if (Data->CurValue < Limits->Limit - Limits->Delta) {
                if (Limits->Flags.Bits.CheckUnderrun == TRUE) {
                    Status = W_DIGIN_BELOW_LIMIT;
                }
                Limits->AboveFlag = FALSE;
            }
        }
        // send warning and/or data if limit passed over
        if (Status != NO_ERROR) {
            if (Limits->Flags.Bits.SendWarning == TRUE) {
                bmSignalEvent (Data->Channel, Status, TRUE, i);
            }
            if (Limits->Flags.Bits.SendData == TRUE) {
                SendRequest = TRUE;
            }
        }
    }
    return (SendRequest);
}

/*****************************************************************************/
/*!
 *  \brief   Send message containing digital input data
 *
 *      Sends the data pointed to by the Data parameter on the CAN-Bus.
 *      The message contains the port data value and (optionally) the
 *      time the port changes state. The time is in milliseconds of the
 *      actual day.
 *
 *  \iparam  Data = Pointer to module instance's data
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t diSendInputValue (diInstanceData_t *Data) {

    CanMessage_t Message;

    Message.CanID = MSG_DI_INPUT_STATE;
    bmSetMessageItem (&Message, Data->CurValue, 0, 2);

    if (Data->Flags.Bits.UseTimestamp == TRUE) {
        bmSetMessageItem (&Message, Data->TimeStamp, 2, 4);
        Message.Length = 6;
    }
    else {
        Message.Length = 2;
    }
    Data->OldValue = Data->CurValue;
    Data->OldValueValid = TRUE;

    return (canWriteMessage(Data->Channel, &Message));
}


/*****************************************************************************/
/*!
 *  \brief   Request the digital ports input value
 *
 *      This function is called by the CAN message dispatcher when a data
 *      request message is received from the master. As a response a data
 *      message is sent back to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t diReqInputValue (UInt16 Channel, CanMessage_t *Message) {

    diInstanceData_t *Data = &diDataTable[bmGetInstance(Channel)];

    if (Message->Length == 0) {
        if (Data->Flags.Bits.ModuleEnable == TRUE) {
            return (diSendInputValue(Data));
        }
        return (E_MODULE_NOT_ENABLED);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Set configuration parameters of the digital input
 *
 *      This function is called by the CAN message dispatcher when an input
 *      configuration message is received from the master. The parameters
 *      in the message are transfered to the data structure of the addressed
 *      module instance. The modified settings influence the behavior of the
 *      module task function. The following settings will be modified:
 *
 *      - Mode flag bits
 *      - Port polarity bitmask
 *      - Threshold for autom. sending
 *      - input sample rate
 *      - input sample count
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t diConfigureInput (UInt16 Channel, CanMessage_t *Message) {

    diInstanceData_t *Data = &diDataTable[bmGetInstance(Channel)];

    if (Data->Handle < 0) {
        return (E_MODULE_NOT_USEABLE);
    }
    if (Message->Length == 7) {
        Bool ModuleEnableOld = Data->Flags.Bits.ModuleEnable;

        Data->Flags.Byte = bmGetMessageItem(Message, 0, 1);
        Data->Polarity   = bmGetMessageItem(Message, 1, 2);
        Data->Threshold  = bmGetMessageItem(Message, 3, 2);
        Data->SampleRate = bmGetMessageItem(Message, 5, 1);
        Data->Samples    = bmGetMessageItem(Message, 6, 1) + 1;

        if (Data->Flags.Bits.ModuleEnable == FALSE) {
            Data->SampleCount = DI_DEFAULT_SAMPLE_COUNT;
            Data->OldValueValid = FALSE;
        }
        if (!Data->SampleRate) {
            Data->SampleRate = DI_DEFAULT_SAMPLE_RATE;
        }
        if (Data->Flags.Bits.ModuleEnable == TRUE && ModuleEnableOld == FALSE) {
            Data->SampleTime = bmGetTime();
        }
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Sets limit parameters for monitoring
 *
 *  \remark  This function is currently not used.
 *
 *      This function is called by the CAN message dispatcher when a limit
 *      configuration message is received from the master. The parameters
 *      in the message are transfered to the data structure of the addressed
 *      module instance. The modified settings influence the behavior of the
 *      module task function. The following settings will be set for each
 *      of the two possible limits:
 *
 *      - Limit mode flag bits
 *      - Limit value
 *      - Limit hysterese
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t diConfigureLimit (UInt16 Channel, CanMessage_t *Message) {

    diInstanceData_t *Data = &diDataTable[bmGetInstance(Channel)];
    diInputLimit_t *Limits;
    UInt16 i;

    if (Message->Length == 8) {
        Data->Limits[0].Flags.Byte = bmGetMessageItem(Message, 0, 1);
        Data->Limits[0].Limit = bmGetMessageItem(Message, 1, 2);
        Data->Limits[0].Delta = bmGetMessageItem(Message, 6, 2);

        Data->Limits[1].Flags.Byte = bmGetMessageItem(Message, 3, 1);
        Data->Limits[1].Limit = bmGetMessageItem(Message, 4, 2);
        Data->Limits[1].Delta = bmGetMessageItem(Message, 6, 2);

        Data->CheckLimits = FALSE;

        for (i=0; i < ELEMENTS(Data->Limits); i++) {
            Limits = &Data->Limits[i];

            if (Data->CurValue > Limits->Limit + Limits->Delta) {
                Limits->AboveFlag = TRUE;
            }
            else {
                Limits->AboveFlag = FALSE;
            }
            if (Limits->Flags.Bits.CheckOverrun == TRUE || Limits->Flags.Bits.CheckUnderrun == TRUE) {
                Data->CheckLimits = TRUE;
            }
        }
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Resets the instance data
 *
 *      This function resets all members of the instance data to their
 *      intialization values. This function is called, when the module control
 *      action MODULE_CONTROL_RESET is received.
 *
 *  \iparam  Instance  = Instance number of this module
 *
 ****************************************************************************/
static void diResetInstanceData(UInt16 Instance) {

    diInstanceData_t *Data = &diDataTable[Instance];
    UInt8 i;

    Data->ModuleState = MODULE_STATE_READY;
    Data->Flags.Byte = 0;
    Data->CheckLimits = FALSE;
    Data->OldValueValid = FALSE;

    Data->SampleRate = DI_DEFAULT_SAMPLE_RATE;
    Data->SampleTime = 0;
    Data->SampleCount = DI_DEFAULT_SAMPLE_COUNT;
    Data->Samples = 0;
    
    Data->CurValue = 0;
    Data->OldValue = 0;
    Data->TmpValue = 0;
    Data->Polarity = 0;
    Data->Threshold = 0;
    Data->TimeStamp = 0;

    for (i = 0; i < 2; i++) {
        Data->Limits[i].Flags.Byte = 0;
        Data->Limits[i].AboveFlag = FALSE;
        Data->Limits[i].Limit = 0;
        Data->Limits[i].Delta = 0;
    }
}


/*****************************************************************************/
/*!
 *  \brief   Initializes this function module
 *
 *      This function is called once during startup by the base module.
 *      It registers itself to the task scheduler and registers all
 *      handler functions to the CAN message dispatcher. Furthermore, it
 *      opens the digital input channel used by the various instances of
 *      this module and allocates/initializes storage for the variables
 *      for each instance of this module.
 *
 *  \iparam  ModuleID  = Module identifier
 *  \iparam  Instances = Number of instances of this module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t diInitializeModule (UInt16 ModuleID, UInt16 Instances) {

    static bmCallbackEntry_t Commands[] = {
        { MSG_DI_REQ_INPUT_STATE, diReqInputValue  },
        { MSG_DI_CONFIG_INPUT,    diConfigureInput },
        { MSG_DI_CONFIG_LIMITS,   diConfigureLimit }
    };
    static bmModuleInterface_t Interface = {
        diModuleTask, diModuleControl,  diModuleStatus
    };
    Error_t Status;
    UInt16 i;

    // allocate module instances data storage
    diDataTable = calloc (Instances, sizeof(diInstanceData_t));
    if (diDataTable == NULL) {
        return (E_MEMORY_FULL);
    }
    // register function module to the scheduler
    Status = bmRegisterModule (ModuleID, Instances, &Interface);
    if (Status < 0) {
        return (Status);
    }
    // registers text strings for debug console
    #ifdef DEBUG
    diRegisterDebugNames(ModuleID);
    #endif

    // bind CAN commands to the handler functions
    Status = canRegisterMessages (ModuleID, Commands, ELEMENTS(Commands));
    if (Status < 0) {
        return (Status);
    }
    // open channels and ports for all modules
    for (i=0; i < Instances; i++) {
        diDataTable[i].Handle = halPortOpen (HAL_DIGITAL_INPUTS + i, HAL_OPEN_READ);
        if (diDataTable[i].Handle < NO_ERROR) {
            return (diDataTable[i].Handle);
        }

        diDataTable[i].Channel = bmGetChannel(bmGetTaskID(ModuleID, i));
        diResetInstanceData(i);
    }
    diInstanceCount = Instances;
    diModuleID = ModuleID;

    return (NO_ERROR);
}

//****************************************************************************/


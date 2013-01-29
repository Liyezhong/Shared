/****************************************************************************/
/*! \file fmAnaInput.c
 *
 *  \brief Function Module for Analog Inputs
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to control and read an analog
 *       input channel. A channel is generally 16 bit wide; in case the
 *       hardware delivers lesser bits, they will be scaled to 16 bits.
 *
 *       The function module uses logical channels; the mapping to a real
 *       hardware channel is done by the hardware abstraction layer.
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
#include "fmAnaInput.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define AI_MODULE_VERSION       0x0002  //!< Version number of module
#define AI_DEFAULT_SAMPLE_RATE  10      //!< Default sample rate (ms)

/*! Mode bits for the Flags member of the module instance data */
typedef union {
    struct {
        UInt8 Reserved:5;       //!< Reserved for future use
        Bool FastSampling:1;    //!< Fast sampling enable
        Bool UseTimestamp:1;    //!< Timestamp enable bit
        Bool ModuleEnable:1;    //!< Module enable bit
    } Bits;                     //!< Bit access
    UInt8 Byte;                 //!< Byte access
} aiInstanceFlags_t;

/*! Limit mode bits for the Flags member of the module instance data */
typedef union {
    struct {
        Bool CheckOverrun:1;    //!< Enable overrun checking
        Bool CheckUnderrun:1;   //!< Enable underrun checking
        UInt8 Reserved:2;       //!< Reserved for future use
        Bool SendWarning:1;     //!< Enable warnings
        Bool SendData:1;        //!< Enable sending data
    } Bits;                     //!< Bit access
    UInt8 Byte;                 //!< Byte access
} aiLimitFlags_t;

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Specifies a limit for monitoring the port input values
typedef struct {
    aiLimitFlags_t Flags;   //!< External mode control flag bits
    Bool AboveFlag;         //!< Input is above limit
    Int16  Limit;           //!< Limit to supervise
    UInt16 Delta;           //!< Hysterese to prevent bouncing
} aiInputLimit_t;

//! Contains all variables for a instance of this module
typedef struct {
    bmModuleState_t ModuleState;    //!< Module state
    Handle_t Handle;                //!< Handle to access port (HAL)
    UInt16 Channel;                 //!< Logical CAN channel
    aiInstanceFlags_t Flags;        //!< Mode control flag bits
    Bool CheckLimits;               //!< Limit check enable

    UInt16 SampleRate;              //!< Sample rate (ms)
    UInt32 SampleTime;              //!< Time of last sampling
    UInt16 Threshold;               //!< Min delta before sending
    UInt32 TimeStamp;               //!< Time of last state change
    UInt16 Filter;                  //!< Number of samples to average
    Int16  MinValue;                //!< Minimal ampltitude measured
    Int16  MaxValue;                //!< Maximal ampltitude measured
    Error_t InterruptError;         //!< Error code from the interrupt handler

    Int16  CurValue;                //!< Actual state of input
    Int16  OldValue;                //!< Input value last sent
    Int16  History[16];             //!< History of input value (for filtering)
    UInt16 InCount;                 //!< Number of samples in history buffer
    UInt16 NextIn;                  //!< Next-in pointer into history buffer

    aiInputLimit_t Limits[2];       //!< Limits to monitor
} aiInstanceData_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

static UInt16 aiInstanceCount = 0;      //!< Number of module instances
static UInt16 aiModuleID = 0;           //!< Module identifier

static aiInstanceData_t *aiDataTable;   //!< Data table for all instances


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t aiModuleControl  (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t aiModuleStatus   (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t aiModuleTask     (UInt16 Instance);

static Error_t aiConfigureLimit (UInt16 Channel, CanMessage_t *Message);
static Error_t aiConfigureInput (UInt16 Channel, CanMessage_t *Message);
static Error_t aiReqInputValue  (UInt16 Channel, CanMessage_t *Message);

static Error_t aiGetFilteredInput (aiInstanceData_t *Data);
static Bool    aiMonitorLimits  (aiInstanceData_t *Data);
static Error_t aiSendInputValue (aiInstanceData_t *Data);
static void    aiFastSampling   (UInt32 Instance, UInt32 IntrFlags);

static void aiResetInstanceData(UInt16 Instance);

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
 *  \iparam  Instance  = Instance number of this module [in]
 *  \iparam  ControlID = Control code to select sub-function [in]
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t aiModuleControl (UInt16 Instance, bmModuleControlID_t ControlID) {

    aiInstanceData_t *Data = &aiDataTable[Instance];

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
            aiResetInstanceData(Instance);
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

static Error_t aiModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID) {

    aiInstanceData_t *Data = &aiDataTable[Instance];

    switch (StatusID) {

        case MODULE_STATUS_STATE:
            if (Data->Flags.Bits.ModuleEnable == FALSE) {
                return (MODULE_STATE_DISABLED);
            }
            return (Data->ModuleState);

        case MODULE_STATUS_VALUE:
            return (Data->CurValue);

        case MODULE_STATUS_MODULE_ID:
            return (aiModuleID);

        case MODULE_STATUS_INSTANCES:
            return (aiInstanceCount);

        case MODULE_STATUS_VERSION:
            return (AI_MODULE_VERSION);
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

static Error_t aiModuleTask (UInt16 Instance)
{
    aiInstanceData_t *Data = &aiDataTable[Instance];
    Bool DataRequest = FALSE;
    Error_t Status;
    UInt16 Delta;

    if (Data->Flags.Bits.ModuleEnable == TRUE) {
        if (bmTimeExpired(Data->SampleTime) >= Data->SampleRate) {
            Data->SampleTime = bmGetTime();
            Status = aiGetFilteredInput(Data);
            if (Status < NO_ERROR) {
                bmSignalEvent(Data->Channel, Status, TRUE, 0);
                return (Status);
            }
            Delta = Status;

            if (Data->InCount >= Data->Filter+1) {
                if (Data->Threshold) {
                    if (Delta >= Data->Threshold) {
                        if (Data->InCount > Data->Filter+1) {
                            DataRequest = TRUE;
                        }
                        else {
                            Data->OldValue = Data->CurValue;
                        }
                    }
                }
                if (Data->CheckLimits == TRUE) {
                    if (aiMonitorLimits (Data)) {
                        DataRequest = TRUE;
                    }
                }
            }
            if (Data->Flags.Bits.UseTimestamp == TRUE) {
                Data->TimeStamp = bmGetSecondsOfTheDay();
            }
            if (DataRequest) {
                aiSendInputValue (Data);
            }
        }
        return (Data->ModuleState);
    }
    return (MODULE_STATE_DISABLED);
}


/*****************************************************************************/
/*!
 *  \brief   Get filtered analog input value
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

static Error_t aiGetFilteredInput (aiInstanceData_t *Data) {

    Int32 Average = 0;
    Error_t Status;
    UInt16 Index;
    UInt16 Count;
    UInt16 i;

    if (Data->Flags.Bits.FastSampling == TRUE) {
        if ((Status = halAnalogControl (Data->Handle, AIO_INTR_DISABLE)) < NO_ERROR) {
            return (Status);
        }
        Data->History[Data->NextIn] = Data->MaxValue - Data->MinValue;
        Data->MinValue = MAX_INT16;
        Data->MaxValue = MIN_INT16;
        Status = Data->InterruptError;
        Data->InterruptError = NO_ERROR;
        if ((Status = halAnalogControl (Data->Handle, AIO_INTR_ENABLE)) < NO_ERROR) {
            return (Status);
        }
    }
    else {
        Status = halAnalogRead (Data->Handle, &Data->History[Data->NextIn]);
    }
    if (Status < NO_ERROR) {
        return (Status);
    }

    if (Data->InCount < ELEMENTS(Data->History)) {
        Data->InCount++;
    }
    Count = MIN(Data->Filter+1, Data->InCount);

    Index = Data->NextIn - Count;
    if (Index >= ELEMENTS(Data->History)) {
       Index += ELEMENTS(Data->History);
    }

    for (i=0; i < Count; i++) {
        if (++Index >= ELEMENTS(Data->History)) {
            Index = 0;
        }
        Average += Data->History[Index];
    }
    if (Count > 0) {
        Data->CurValue = Average / Count;
    }
    if (++Data->NextIn >= ELEMENTS(Data->History)) {
        Data->NextIn = 0;
    }
    return (bmGetDelta(Data->OldValue, Data->CurValue));
}


/*****************************************************************************/
/*!
 *  \brief   Monitors the user supplied limits
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

static Bool aiMonitorLimits (aiInstanceData_t *Data) {

    Bool SendRequest = FALSE;
    aiInputLimit_t *Limits;
    Error_t Status;
    UInt16 i;

    for (i=0; i < ELEMENTS(Data->Limits); i++) {

        Status = NO_ERROR;  // reset status variable
        Limits = &Data->Limits[i];

        if (Limits->AboveFlag == FALSE) {

            // warning if input value rises above limit
            if (Data->CurValue > Limits->Limit + Limits->Delta) {
                if (Limits->Flags.Bits.CheckOverrun == TRUE) {
                    Status = W_ANAIN_ABOVE_LIMIT;
                }
                Limits->AboveFlag = TRUE;
            }
        }
        if (Limits->AboveFlag == TRUE) {

            // warning if input value falls below limit
            if (Data->CurValue < Limits->Limit - Limits->Delta) {
                if (Limits->Flags.Bits.CheckUnderrun == TRUE) {
                    Status = W_ANAIN_BELOW_LIMIT;
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
 *  \brief   Send analog input value to the master
 *
 *      Sends the data pointed to by the "Data" parameter on the CAN-Bus.
 *      The message contains the port data value and (optionally) the
 *      time the input changes state. The time is in milliseconds of
 *      the actual day.
 *
 *  \iparam  Data = Pointer to module instance's data
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t aiSendInputValue (aiInstanceData_t *Data) {

    CanMessage_t Message;

    Message.CanID = MSG_AI_INPUT_STATE;
    bmSetMessageItem (&Message, Data->CurValue, 0, 2);

    if (Data->Flags.Bits.UseTimestamp == TRUE) {
        bmSetMessageItem (&Message, Data->TimeStamp, 2, 4);
        Message.Length = 6;
    }
    else {
        Message.Length = 2;
    }
    Data->OldValue = Data->CurValue;

    return (canWriteMessage(Data->Channel, &Message));
}


/*****************************************************************************/
/*!
 *  \brief   Interrupt handler for fast sampling
 *
 *      This method computes the amplitude of a periodical high frequency
 *      waveform. The frequency is only limited by the performance of the ADC.
 *      The routine is triggered by the end of conversion flag of the ADC.
 *      It stores the minimum and maximum measured throughout a sampling
 *      period. These values are evaluated at the end of each sampling period
 *      by the task function.
 *
 *  \iparam  Instance = function module instance
 *  \iparam  IntrFlags = interrupt flags
 *
 ****************************************************************************/
 
static void aiFastSampling (UInt32 Instance, UInt32 IntrFlags) {

    aiInstanceData_t *Data = &aiDataTable[Instance];
    Int16 Value;

    if (Data->InterruptError == NO_ERROR &&
            Data->Flags.Bits.ModuleEnable == TRUE && Data->Flags.Bits.FastSampling == TRUE) {
        Data->InterruptError = halAnalogRead (Data->Handle, &Value);
        if (Data->InterruptError >= NO_ERROR) {
            if (Value < Data->MinValue) {
                Data->MinValue = Value;
            }
            if (Value > Data->MaxValue) {
                Data->MaxValue = Value;
            }
        }
    }
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

static Error_t aiReqInputValue (UInt16 Channel, CanMessage_t *Message) {

    aiInstanceData_t *Data = &aiDataTable[bmGetInstance(Channel)];

    if (Message->Length == 0) {
        if (Data->Flags.Bits.ModuleEnable == TRUE) {
            return (aiSendInputValue(Data));
        }
        return (E_MODULE_NOT_ENABLED);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Sets the configuration parameters of the digital input
 *
 *      This function is called by the CAN message dispatcher when an input
 *      configuration message is received from the master. The parameters
 *      in the message are transfered to the data structure of the addressed
 *      module instance. The modified settings influence the behavior of the
 *      module task. The following settings will be modified:
 *
 *      - Mode flag bits
 *      - Threshold for autom. sending
 *      - Sample rate
 *      - Sample filter (number of samples to average over)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t aiConfigureInput (UInt16 Channel, CanMessage_t *Message) {

    aiInstanceData_t *Data = &aiDataTable[bmGetInstance(Channel)];
    Error_t Status;

    if (Data->Handle < 0) {
        return (E_MODULE_NOT_USEABLE);
    }
    if (Message->Length == 5) {
        Bool ModuleEnableOld = Data->Flags.Bits.ModuleEnable;
        Bool FastSamplingOld = Data->Flags.Bits.FastSampling;

        Data->Flags.Byte = bmGetMessageItem(Message, 0, 1);
        Data->SampleRate = bmGetMessageItem(Message, 1, 1);
        Data->Filter     = bmGetMessageItem(Message, 2, 1);
        Data->Threshold  = bmGetMessageItem(Message, 3, 2);

        if (Data->Filter > ELEMENTS(Data->History)) {
            Data->Flags.Bits.ModuleEnable = FALSE;
            return (E_PARAMETER_OUT_OF_RANGE);
        }
        if (Data->Flags.Bits.ModuleEnable == FALSE || FastSamplingOld != Data->Flags.Bits.FastSampling) {
            Data->InCount = 0;
        }
        if (!Data->SampleRate) {
            Data->SampleRate = AI_DEFAULT_SAMPLE_RATE;
        }
        if (Data->Flags.Bits.ModuleEnable == TRUE && ModuleEnableOld == FALSE) {
            Data->SampleTime = bmGetTime();
        }

        if ((Status = halAnalogControl (Data->Handle, AIO_INTR_DISABLE)) < NO_ERROR) {
            return (Status);
        }
        Data->MinValue = MAX_INT16;
        Data->MaxValue = MIN_INT16;
        if ((Status = halAnalogControl (Data->Handle, AIO_INTR_ENABLE)) < NO_ERROR) {
            return (Status);
        }

        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Sets limit parameters for monitoring
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

static Error_t aiConfigureLimit (UInt16 Channel, CanMessage_t *Message) {

    aiInstanceData_t *Data = &aiDataTable[bmGetInstance(Channel)];
    aiInputLimit_t *Limits;
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
static void aiResetInstanceData(UInt16 Instance) {

    aiInstanceData_t *Data = &aiDataTable[Instance];
    UInt8 i;

    Data->ModuleState = MODULE_STATE_READY;
    Data->Flags.Byte = 0;
    Data->CheckLimits = FALSE;

    Data->SampleRate = AI_DEFAULT_SAMPLE_RATE;
    Data->SampleTime = 0;
    Data->Threshold = 0;
    Data->TimeStamp = 0;
    Data->Filter = 0;
    Data->MinValue = 0;
    Data->MaxValue = 0;
    Data->InterruptError = NO_ERROR;

    Data->CurValue = 0;
    Data->OldValue = 0;
    for (i = 0; i < 16; i++) {
        Data->History[i] = 0;
    }
    Data->InCount = 0;
    Data->NextIn = 0;

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
 *      opens the analog input channel used by the various instances of
 *      this module and allocates/initializes storage for the variables
 *      for each instance of this module.
 *
 *  \iparam  ModuleID  = Module identifier
 *  \iparam  Instances = Number of instances of this module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t aiInitializeModule (UInt16 ModuleID, UInt16 Instances) {

    static bmCallbackEntry_t Commands[] = {
        { MSG_AI_REQ_INPUT_STATE, aiReqInputValue  },
        { MSG_AI_CONFIG_INPUT,    aiConfigureInput },
        { MSG_AI_CONFIG_LIMITS,   aiConfigureLimit }
    };
    static bmModuleInterface_t Interface = {
        aiModuleTask, aiModuleControl, aiModuleStatus
    };
    Error_t Status;
    UInt16 i;

    // allocate module instances data storage
    aiDataTable = calloc (Instances, sizeof(aiInstanceData_t));
    if (aiDataTable == NULL) {
        return (E_MEMORY_FULL);
    }
    // register function module to the scheduler
    Status = bmRegisterModule (ModuleID, Instances, &Interface);
    if (Status < 0) {
        return (Status);
    }
    // registers text strings for debug console
    #ifdef DEBUG
    aiRegisterDebugNames(ModuleID);
    #endif

    // bind CAN commands to the handler functions
    Status = canRegisterMessages (ModuleID, Commands, ELEMENTS(Commands));
    if (Status < 0) {
        return (Status);
    }
    // open channels and ports for all modules
    for (i=0; i < Instances; i++) {
        aiDataTable[i].Handle  = halAnalogOpen(HAL_ANALOG_INPUTS + i, HAL_OPEN_READ, i, aiFastSampling);
        if (aiDataTable[i].Handle < NO_ERROR) {
            return (aiDataTable[i].Handle);
        }

        aiDataTable[i].Channel = bmGetChannel (bmGetTaskID(ModuleID, i));
        aiResetInstanceData(i);
    }
    aiInstanceCount = Instances;
    aiModuleID = ModuleID;

    return (NO_ERROR);
}

//****************************************************************************/



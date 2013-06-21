/****************************************************************************/
/*! \file fmInclinometer.c
 * 
 *  \brief Function module to control inclinometer
 *
 *  $Version: $ 0.1
 *  $Date:    $ 26.07.2010
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
#include "bmCommon.h"
#include "Basemodule.h"
#include "fmInclinometer.h"


//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

#define MODULE_ID              0x76     //!< Module ID of analog input module 
#define MODULE_VERSION         0x0001   //!< Version number of module
#define DEFAULT_SAMPLE_RATE    10       //!< Default sample rate (ms)
#define DEFAULT_THRESHOLD      1        //!< Default threshold (auto send mode)

/* Mode bits for the Flags member of the module instance data */
#define MODE_MODULE_ENABLED    0x0080   //!< Flags: Master enable bit
#define MODE_USE_TIMESTAMP     0x0040   //!< Flags: Timestamp enable bit
#define MODE_CHECK_XLIMITS     0x0100   //!< Flags: Limit check enable
#define MODE_CHECK_YLIMITS     0x0200   //!< Flags: Limit check enable

/* Limit mode bits for the Flags member of the module instance data */
#define LIMIT_ENABLE_BITS      0x0003   //!< Limit: Enable bit mask
#define LIMIT_CHECK_OVERRUN    0x0001   //!< Limit: Enable overrun checking
#define LIMIT_CHECK_UNDERRUN   0x0002   //!< Limit: Enable underrun checking
#define LIMIT_SEND_WARNING     0x0010   //!< Limit: Enable warnings
#define LIMIT_SEND_DATA        0x0020   //!< Limit: Enable sending data
#define LIMIT_ABOVE_FLAG       0x0100   //!< Limit: input is above limit

typedef enum {
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
} INCL_ORIENTATION_t;

#define CFG_ORIENTATION        0x0003   //!< Flags: sensor orientation


//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Specifies a limit for monitoring the port input values */
typedef struct {
    UInt16 Flags;               //!< Mode control flag bits
    UInt16 Limit;               //!< Limit to supervise
    UInt16 Delta;               //!< Hysterese to prevent bouncing
} INCLINO_LIMIT_t;


/*! Contains all variables for a instance of this module */
typedef struct {
    HANDLE_t Handle;            //!< Handle to access port (HAL)
    UInt16 TaskID;              //!< Logical CAN channel
    UInt16 Status;              //!< status of instance
    UInt16 Flags;               //!< Mode control flag bits

    UInt16 SampleRate;          //!< Sample rate (ms)
    UInt32 SampleTime;          //!< Time of last sampling
    UInt16 Threshold;           //!< Min delta before sending 
    UInt32 TimeStamp;           //!< Time of last state change

    UInt16 curValueX;           //!< Current incline of x-axis
    UInt16 curValueY;           //!< Current incline of x-axis
    UInt16 oldValueX;           //!< Incline value of x-axis last sent
    UInt16 oldValueY;           //!< Incline value of x-axis last sent
    UInt16 OffsetX;             //!< calibration offset for x-axis
    UInt16 OffsetY;             //!< calibration offset for y-axis
    UInt16 ScaleX;              //!< scaling factor for x-axis
    UInt16 ScaleY;              //!< scaling factor for y-axis

    INCLINO_LIMIT_t XLimits[2]; //!< limits to monitor on x axis
    INCLINO_LIMIT_t YLimits[2]; //!< limits to monitor on y axis
    
} INCLINOMETER_DATA_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

static INCLINOMETER_DATA_t* DataTable;  //!< data table for all instances
static UInt16 InstanceCount = 0;        //!< number of module instances


//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static ERROR_t incModuleControl  (UInt16 Instance, MODULE_CONTROL_ID_t ControlID);
static ERROR_t incModuleStatus   (UInt16 Instance, MODULE_STATUS_ID_t StatusID);
static ERROR_t incModuleTask     (UInt16 Instance);

static ERROR_t incConfigLimits   (UInt16 Channel, CAN_MESSAGE_t* Message);
static ERROR_t incConfigInput    (UInt16 Channel, CAN_MESSAGE_t* Message);
static ERROR_t incReqInputValue  (UInt16 Channel, CAN_MESSAGE_t* Message);

static UInt16  incFilteredInput  (INCLINOMETER_DATA_t* Data);
static ERROR_t incSendInputValue (INCLINOMETER_DATA_t* Data);
static Bool    incMonitorLimits  (INCLINOMETER_DATA_t* Data, INCLINO_LIMIT_t *Limits);

static ERROR_t incGetInclinomation (INCLINOMETER_DATA_t* Data);


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

static ERROR_t incModuleControl (UInt16 Instance, MODULE_CONTROL_ID_t ControlID)
{
    INCLINOMETER_DATA_t* Data = &DataTable[Instance];

    switch (ControlID)
    {
        case MODULE_CONTROL_STOP:
            Data->Status = MODULE_STATE_STOPPED;
            break;

        case MODULE_CONTROL_RESUME:
            Data->Status = MODULE_STATE_IDLE;
            break;
            
        case MODULE_CONTROL_STANDBY:
            Data->Status = MODULE_STATE_STANDBY;
            break;
            
        case MODULE_CONTROL_WAKEUP:
            Data->Status = MODULE_STATE_IDLE;
            break;
            
        case MODULE_CONTROL_SHUTDOWN:
            Data->Status = MODULE_STATE_SHUTDOWN;
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
 *      - MODULE_STATUS_ENABLED
 *      - MODULE_STATUS_STATE
 *      - MODULE_STATUS_VALUE
 *      - MODULE_STATUS_ABORTED
 * 
 *  \iparam  Instance = Instance number of this module
 *  \iparam  StatusID = selects which status is requested
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static ERROR_t incModuleStatus (UInt16 Instance, MODULE_STATUS_ID_t StatusID)
{
    INCLINOMETER_DATA_t* Data = &DataTable[Instance];

    switch (StatusID)
    {
        case MODULE_STATUS_ENABLED:
            return (Data->Flags & MODE_MODULE_ENABLED);
                        
        case MODULE_STATUS_STATE:
            return (Data->Status);
            
//        case MODULE_STATUS_VALUE:
//            return (Data->curValue);

        case MODULE_STATUS_ABORTED:
            return (FALSE);
        
        case MODULE_STATUS_MODULE_ID:
            return (MODULE_ID);
            
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
 *  \return  State of instance or (negative) error code
 *
 *****************************************************************************/


static ERROR_t incModuleTask (UInt16 Instance)
{
    INCLINOMETER_DATA_t* Data = &DataTable[Instance];
    Bool DataRequest = FALSE;
    ERROR_t Status;
    UInt16 Delta;            

    if (Data->Flags & MODE_MODULE_ENABLED) {
    
        if (bmTimeExpired(Data->SampleTime) > Data->SampleRate) {
   
            if ((Status = incGetInclinomation(Data)) < 0) {
                return (Status);
            }
            // check if x-axis incline exceeds limits         
            Delta = bmGetDelta(Data->curValueX, Data->oldValueX);
            if (Delta >= Data->Threshold) {
                DataRequest = TRUE;
            }
            if (Data->Flags & MODE_CHECK_XLIMITS) {
                if (incMonitorLimits (Data, Data->XLimits)) {
                    DataRequest = TRUE;
                }
            }             
            // check if y-axis incline exceeds limits         
            Delta = bmGetDelta(Data->curValueY, Data->oldValueY);
            if (Delta >= Data->Threshold) {
                DataRequest = TRUE;
            }                        
            if (Data->Flags & MODE_CHECK_YLIMITS) {
                if (incMonitorLimits (Data, Data->YLimits)) {
                    DataRequest = TRUE;            
                }
            }
            // send inclination message (if requested) 
            if (DataRequest) {
                incSendInputValue (Data);
            }
            Data->SampleTime = bmGetTime();
        }
    }
    return (Data->Status);    
}


/*****************************************************************************/
/*! 
 *  \brief   Read actual inclino
 *
 *      Reads actual x/y inclinomation, transforms read values according
 *      to the configured orientation, adjusts it by adding the programmed
 *      offsets and scales it by multiplying it with a scale factor.
 * 
 *  \iparam  Instance = Instance number of this module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 *****************************************************************************/

static ERROR_t incGetInclinomation (INCLINOMETER_DATA_t* Data)
{
    Int16 ValueX, ValueY;
    Int16 Result[2];
    ERROR_t Status;
    
    if ((Status = halInclinoRead (Data->Handle, INCLINOMATION, Result)) < 0) {
        return (Status);
    }
    switch (Data->Flags & CFG_ORIENTATION) {
        case ROTATE_0: 
            ValueX = Result[0]; 
            ValueY = Result[1]; 
            break;
            
        case ROTATE_90: 
            ValueX =  Result[1]; 
            ValueY = -Result[0]; 
            break;
            
        case ROTATE_180: 
            ValueX = -Result[0]; 
            ValueY = -Result[1]; 
            break;
            
        case ROTATE_270: 
            ValueX = -Result[1]; 
            ValueY =  Result[0]; 
            break;
                        
        default: break;         
    }
    Data->curValueX = (ValueX * Data->ScaleX) + Data->OffsetX;
    Data->curValueY = (ValueY * Data->ScaleY) + Data->OffsetY;
    
    return (NO_ERROR);
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

static Bool incMonitorLimits (INCLINOMETER_DATA_t* Data, INCLINO_LIMIT_t *Limits)
{
    Bool SendRequest = FALSE;
    ERROR_t Status;
    UInt16 i;
    
    for (i=0; i < 2; i++) {
        
        Status = NO_ERROR;  // reset status variable
        
        // warning if input value rises above limit
        if (Limits->Flags & LIMIT_CHECK_OVERRUN &&
           (Limits->Flags & LIMIT_ABOVE_FLAG) == 0) {
                        
            if (Data->curValue > Limits->Limit + Limits->Delta) {                
                Limits->Flags |= LIMIT_ABOVE_FLAG;
                Status = W_INCLINO_ABOVE_LIMIT;
            }
        }
        // warning if input value falls below limit
        if (Limits->Flags & LIMIT_CHECK_UNDERRUN &&
            Limits->Flags & LIMIT_ABOVE_FLAG) {
            
            if (Data->curValue < Limits->Limit - Limits->Delta) {    
                Limits->Flags &= ~LIMIT_ABOVE_FLAG;
                Status = W_INCLINO_BELOW_LIMIT;
            }
        }
        // send warning and/or data if limit passed over
        if (Status != NO_ERROR) {
            if (Limits->Flags & LIMIT_SEND_WARNING) { 
                bmSignalEvent (Data->TaskID, Status, TRUE, i);
            }                                                                
            if (Limits->Flags & LIMIT_SEND_DATA) 
                SendRequest = TRUE;                
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

static ERROR_t incSendInputValue (INCLINOMETER_DATA_t* Data)
{
    CAN_MESSAGE_t Message;

    Message.CanID  = MSG_INC_INPUT_DATA;
    bmSetMessageItem (&Message, Data->curValueX, 0, 2);    
    bmSetMessageItem (&Message, Data->curValueY, 2, 2);        
    Message.Length = 2;

    Data->oldValueX = Data->curValueX;
    Data->oldValueY = Data->curValueY;

    return (bmCanWriteMessage(Data->TaskID, &Message));
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

static ERROR_t incReqInputValue (UInt16 Channel, CAN_MESSAGE_t* Message)
{
    INCLINOMETER_DATA_t* Data = &DataTable[bmGetInstance(Channel)];
    
    if (Data->Flags & MODE_MODULE_ENABLED) {
        
        incSendInputValue(Data);
        return (NO_ERROR);
    }        
    return (E_INCLINO_NOT_ENABLED);
}


/*****************************************************************************/
/*! 
 *  \brief  Sets the configuration parameters of the digital input
 *
 *      This function is called by the CAN message dispatcher when an input 
 *      configuration message is received from the master. The parameters
 *      in the message are transfered to the data structure of the addressed
 *      module instance. The modified settings influence the behaivor of the
 *      module task. The following settings will be modified:
 *
 *      - Mode flag bits
 *      - Threshold for autom. sending
 *      - sample rate
 *      - sample filter (number of samples to average over) 
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static ERROR_t incConfigInput (UInt16 Channel, CAN_MESSAGE_t* Message)
{
    INCLINOMETER_DATA_t* Data = &DataTable[bmGetInstance(Channel)];
    
    if (!Data->Handle) {
        return (E_INCLINO_NOT_USABLE);        
    }
    if (Message->Length >= 5) {
        
        Data->Flags      = bmGetMessageItem(Message, 0, 1);
        Data->Threshold  = bmGetMessageItem(Message, 3, 2);
        Data->SampleRate = bmGetMessageItem(Message, 1, 1);
//      Data->Filter     = bmGetMessageItem(Message, 2, 1);
        
        if (!Data->Threshold) {
            Data->Threshold = DEFAULT_THRESHOLD;
        }
        if (!Data->SampleRate) {
            Data->SampleRate = DEFAULT_SAMPLE_RATE;
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
 *      module instance. The modified settings influence the behaivor of the
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

static ERROR_t incConfigLimits (UInt16 Channel, CAN_MESSAGE_t* Message)
{
    INCLINOMETER_DATA_t* Data = &DataTable[bmGetInstance(Channel)];
    INCLINO_LIMIT_t* Limits;
    UInt16 i;
    
    if (Message->Length >= 8) {
        
        Data->XLimits[0].Flags = bmGetMessageItem(Message, 0, 1);
        Data->XLimits[0].Limit = bmGetMessageItem(Message, 1, 2);
        Data->XLimits[0].Delta = bmGetMessageItem(Message, 6, 2);
        
        Data->XLimits[1].Flags = bmGetMessageItem(Message, 3, 1);
        Data->XLimits[1].Limit = bmGetMessageItem(Message, 4, 2);
        Data->XLimits[1].Delta = bmGetMessageItem(Message, 6, 2);

        Data->Flags &= ~MODE_CHECK_XLIMITS;
        
        for (i=0; i < ELEMENTS(Data->XLimits); i++) {
            Limits = &Data->XLimits[i];

            if (Data->curValue > Limits->Limit + Limits->Delta)                 
                Limits->Flags |= LIMIT_ABOVE_FLAG;
            else
                Limits->Flags &= ~LIMIT_ABOVE_FLAG;
                            
            if (Limits->Flags & LIMIT_ENABLE_BITS)                                     
                Data->Flags |= MODE_CHECK_LIMITS;
        }
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
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
 *  \iparam  NumberOfInstances = Number of instances of this module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

ERROR_t incInitializeModule (UInt16 NumberOfInstances)
{
    static CALLBACK_ENTRY_t Commands[] = {
        { MSG_INC_INPUT_REQUEST, incReqInputValue },
        { MSG_INC_INPUT_CONFIG,  incConfigInput   },
        { MSG_INC_LIMIT_CONFIG,  incConfigLimits  }
    };        
    static MODULE_INTERFACE_t Interface = {
        incModuleTask, 
        incModuleControl, 
        incModuleStatus
    };
    ERROR_t Status;
    UInt16 i;
    
    // allocate module instances data storage
    DataTable = calloc (NumberOfInstances, sizeof(INCLINOMETER_DATA_t));
    if (isPtrNull(DataTable)) {
        return (E_MEMORY_FULL);
    }    
    // register function module to the scheduler    
    Status = bmRegisterModule (MODULE_ID, NumberOfInstances, &Interface);
    if (Status < 0) {
        return (Status);
    }
    // bind CAN commands to the handler functions
    Status = bmRegisterMessage (MODULE_ID, Commands, ELEMENTS(Commands));
    if (Status < 0) {
        return (Status);
    }
    // open channels and ports for all modules
    for (i=0; i < NumberOfInstances; i++) {
        
        DataTable[i].TaskID = bmGetTaskID(MODULE_ID, i);
        DataTable[i].Handle = halInclinoOpen(i, HAL_OPEN_READ);
    }
    InstanceCount = NumberOfInstances;
        
    return (NO_ERROR);
}

//****************************************************************************/



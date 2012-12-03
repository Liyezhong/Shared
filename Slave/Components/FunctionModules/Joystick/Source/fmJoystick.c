/****************************************************************************/
/*! \file fmJoystick.c
 * 
 *  \brief Functional module controlling the adjustment joystick.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 19.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This module controls a joystick that is used as an adjustement sensor.
 *      The joystick is needed to calibrate moving parts that are operated by
 *      electric motors. With the help of the joystick the operating range of
 *      such a device can be measured. The joystick signal is sampled by an
 *      analog to digital converter which is the only hardware component
 *      needed by this functional module. The operation parameters are set
 *      through the CAN bus interface. The module is also able to access
 *      non-volatile memory to store the mechanical offsets and the
 *      calibration information of the hardware controlled by this module.
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
#include "bmPermData.h"
#include "fmJoystick.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/

/* Global defines of the joystick module */
#define MODULE_VERSION      0x0001  //!< Version number of module

/* Mode bits for the Flags member of the module instance data */
#define MODE_MODULE_ENABLE  0x1  //!< Module on/off bit
#define MODE_COMMUNICATION  0x2  //!< Permanent (1) or threshold (0) communication
#define MODE_SET_CENTER     0x4  //!< Command storing the center position
#define MODE_CALIBRATION    0x8  //!< Calibration mode is active
#define MODE_CONTACT        0x10 //!< Joystick has contact to an object

/* Definition of non-volatile data elements */
#define PARAM_LAYOUT_VERSION BUILD_PARAMETER(0,2)   //!< Partition layout version
#define PARAM_MECH_OFFSET    BUILD_PARAMETER(2,4)   //!< Mechanical offset data
#define PARAM_CENTER_X       BUILD_PARAMETER(6,2)   //!< Center of the X axis
#define PARAM_CENTER_Y       BUILD_PARAMETER(8,2)   //!< Center of the X axis
#define PARAM_MIN_X          BUILD_PARAMETER(10,2)  //!< Minimum value of the X axis
#define PARAM_MAX_X          BUILD_PARAMETER(12,2)  //!< Maximum value of the X axis
#define PARAM_MIN_Y          BUILD_PARAMETER(14,2)  //!< Minimum value of the Y axis
#define PARAM_MAX_Y          BUILD_PARAMETER(16,2)  //!< Maximum value of the Y axis
#define PARAM_TOTAL_SIZE     18                     //!< Size of the partition

#define PARTITION_VERSION 1  //!< Partition layout version

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

/*! Contains all variables for a instance of this module */
typedef struct {
    Handle_t Handle;    //!< Handle of the analog input port connected to the axis (HAL)
    Int16 Displacement; //!< Displacement value of the axis
    Int16 Center;       //!< Center position of the joystick axis
    Int16 Minimum;      //!< Minimal value of the joystick axis
    Int16 Maximum;      //!< Maximal value of the joystick axis
} JoystickAxis_t;

/*! Contains all variables for a instance of this module */
typedef struct {
    bmModuleState_t ModuleState; //!< Module state
    Handle_t HandleStorage;      //!< Handle to access a storage partition (HAL)
    UInt16 Channel;              //!< Logical CAN channel
    UInt16 Flags;                //!< Mode control flag bits

    UInt16 UpperThreshold;       //!< Upper threshold for threshold communication mode
    UInt16 LowerThreshold;       //!< Lower threshold for threshold communication mode
    UInt16 SamplingTime;         //!< Sampling time of the ADC
    UInt32 SampleTimestamp;      //!< Time the last sample was taken
    JoystickAxis_t XAxis;        //!< The position data concerning the X axis
    JoystickAxis_t YAxis;        //!< The position data concerning the Y axis
} InstanceData_t;

//****************************************************************************/
// Private Variables 
//****************************************************************************/

/*! Persistent parameter descriptor table (used for verification) */
static const bmParamRange_t PermDataTable[] = {
    { PARAM_LAYOUT_VERSION, 1, PARTITION_VERSION, PARTITION_VERSION },
    { PARAM_MECH_OFFSET, 0, MAX_UINT32, 0 },
    { PARAM_CENTER_X, 0, MAX_UINT16, 0 },
    { PARAM_CENTER_Y, 0, MAX_UINT16, 0 },    
    { PARAM_MIN_X, 0, MAX_UINT16, MAX_INT16 },
    { PARAM_MAX_X, 0, MAX_UINT16, (UInt16) MIN_INT16 },
    { PARAM_MIN_Y, 0, MAX_UINT16, MAX_INT16 },
    { PARAM_MAX_Y, 0, MAX_UINT16, (UInt16) MIN_INT16 },
};

static UInt16 InstanceCount = 0;        //!< number of module instances
static UInt16 ModuleIdentifier = 0;     //!< module identifier

static InstanceData_t* DataTable;  //!< data table for all instances

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/

static Error_t joysModuleControl (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t joysModuleStatus  (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t joysModuleTask    (UInt16 Instance);

static Error_t joysHandleAxis   (JoystickAxis_t *Axis, UInt16 Flags);
static Error_t joysContactCheck (InstanceData_t *Data);

static Error_t joysSetConfiguration (UInt16 Channel, CanMessage_t* Message);
static Error_t joysSetVoltageRange (UInt16 Channel, CanMessage_t* Message);
static Error_t joysSetMechOffset (UInt16 Channel, CanMessage_t* Message);
static Error_t joysGetDisplacement (UInt16 Channel, CanMessage_t* Message);
static Error_t joysGetMechOffset (UInt16 Channel, CanMessage_t* Message);

static Error_t joysVerifyPartition (InstanceData_t *Data);
static Error_t joysResetPartition (InstanceData_t *Data);

static Error_t joysSendDisplacement (UInt16 Channel, UInt16 DisplacementX, UInt16 DisplacementY);

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

static Error_t joysModuleControl (UInt16 Instance, bmModuleControlID_t ControlID)
{
    Error_t Error = NO_ERROR;
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
            break;

        case MODULE_CONTROL_RESET_DATA:
            Error = joysResetPartition(Data);
            break;

        default:
            return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (Error);
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

static Error_t joysModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID)
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
            return (((Int32)Data->XAxis.Displacement << 16) | Data->YAxis.Displacement);

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

static Error_t joysModuleTask (UInt16 Instance)
{
    Error_t Error;
    InstanceData_t* Data = &DataTable[Instance];

    if ((Data->Flags & MODE_MODULE_ENABLE) != 0) {
        Error = joysHandleAxis (&Data->XAxis, Data->Flags);
        if (Error < NO_ERROR) {
            bmSignalEvent (Data->Channel, Error, TRUE, 0);
            return ((Error_t) Data->ModuleState);
        }
        Error = joysHandleAxis (&Data->YAxis, Data->Flags);
        if (Error < NO_ERROR) {
            bmSignalEvent (Data->Channel, Error, TRUE, 0);
            return ((Error_t) Data->ModuleState);
        }

        // Store the center position
        if ((Data->Flags & MODE_SET_CENTER) != 0) {
            Data->Flags &= ~(MODE_SET_CENTER);

            Error = bmSetStorageItem(Data->HandleStorage, PARAM_CENTER_X, Data->XAxis.Center);
            if (Error < NO_ERROR) {
                bmSignalEvent (Data->Channel, Error, TRUE, 0);
                return ((Error_t) Data->ModuleState);
            }
            Error = bmSetStorageItem(Data->HandleStorage, PARAM_CENTER_Y, Data->YAxis.Center);
            if (Error < NO_ERROR) {
                bmSignalEvent (Data->Channel, Error, TRUE, 0);
                return ((Error_t) Data->ModuleState);
            }
        }

        if ((Data->Flags & MODE_COMMUNICATION) != 0) {
            // Permanent communication
            if (bmTimeExpired(Data->SampleTimestamp) >= Data->SamplingTime) {
                Data->SampleTimestamp = bmGetTime();
                
                Error = joysSendDisplacement (Data->Channel, Data->XAxis.Displacement, Data->YAxis.Displacement);
                if (Error < NO_ERROR) {
                    bmSignalEvent (Data->Channel, Error, TRUE, 0);
                    return ((Error_t) Data->ModuleState);
                }
            }
        }
        else {
            // Threshold communication
            Error = joysContactCheck (Data);
            if (Error < NO_ERROR) {
                bmSignalEvent (Data->Channel, Error, TRUE, 0);
                return ((Error_t) Data->ModuleState);
            }
        }
    }

    return ((Error_t) Data->ModuleState);
}

/*****************************************************************************/
/*! 
 *  \brief  This function reads the joystick and computes its displacement
 *
 *  This function reads an ADC which is connected to an axis of the adjustment
 *  joystick. When calibration modes are set, the center and maximal range of
 *  the stick are also computed. After that, the displacement factors are
 *  calculated. 
 * 
 *  \xparam  Axis = Containing data of a joystick axis
 *  \iparam  Flags = Control flags of a module instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t joysHandleAxis (JoystickAxis_t *Axis, UInt16 Flags)
{
    Error_t Error;
    Int16 SamplingValue;

    // Read the joystick axis
    Error = halAnalogRead (Axis->Handle, &SamplingValue);
    if (Error < NO_ERROR) {
        return (Error);
    }

    // Store the center position
    if ((Flags & MODE_SET_CENTER) != 0) {
        Axis->Center = SamplingValue;
    }

    // Check for new extremes
    if ((Flags & MODE_CALIBRATION) != 0) {
        if (SamplingValue < Axis->Minimum) {
            Axis->Minimum = SamplingValue;
        }
        if (SamplingValue > Axis->Maximum) {
            Axis->Maximum = SamplingValue;
        }
    }

    // Truncate the sampling value to the minimum or maximum
    if (SamplingValue < Axis->Minimum) {
        SamplingValue = Axis->Minimum;
    }
    if (SamplingValue > Axis->Maximum) {
        SamplingValue = Axis->Maximum;
    }

    // Compute the displacement factors
    if(SamplingValue >= Axis->Center) {
        if (Axis->Maximum - Axis->Center != 0) {
            Axis->Displacement = ((Int32)(SamplingValue - Axis->Center) * MAX_INT16) / (Axis->Maximum - Axis->Center);
        }
        else {
            Axis->Displacement = 0;
        }
    }
    else {
        if (Axis->Center - Axis->Minimum != 0) {
            Axis->Displacement = ((Int32)(Axis->Center - SamplingValue) * MIN_INT16) / (Axis->Center - Axis->Minimum);
        }
        else {
            Axis->Displacement = 0;
        }
    }

    return (NO_ERROR);
}


/*****************************************************************************/
/*! 
 *  \brief  Checks for contacts and reports them
 *
 *  This function computes the joysticks's total threshold from its center
 *  position. When a contact is detected or the contact is not detected any
 *  more, a message containing the displacement factors of the joystick axes
 *  is forwarded to the master.
 * 
 *  \iparam  Data = Containing data of a module instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t joysContactCheck (InstanceData_t *Data)
{
    Error_t Error;
    UInt32 Threshold;

    // Compute the square sum of the displacement factors 
    Threshold = Data->XAxis.Displacement * Data->XAxis.Displacement + Data->YAxis.Displacement * Data->YAxis.Displacement;

    if ((Data->Flags & MODE_CONTACT) == 0 && (Threshold >= (UInt32) Data->UpperThreshold * Data->UpperThreshold)) {
        Data->Flags |= MODE_CONTACT;
        Error = joysSendDisplacement (Data->Channel, Data->XAxis.Displacement, Data->YAxis.Displacement);
        if (Error < NO_ERROR) {
            return (Error);
        }
    }
    else if ((Data->Flags & MODE_CONTACT) != 0 && (Threshold < (UInt32) Data->LowerThreshold * Data->LowerThreshold)) {
        Data->Flags &= ~(MODE_CONTACT);
        Error = joysSendDisplacement (Data->Channel, Data->XAxis.Displacement, Data->YAxis.Displacement);
        if (Error < NO_ERROR) {
            return (Error);
        }
    }

    return (NO_ERROR);
}

/*****************************************************************************/
/*! 
 *  \brief  Sets various parameters controlling the operation procedure
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      setting the configuration parameters of the jostick module is received
 *      from the master. The message activates the module, switches between
 *      both displacement value communication modes, sets threshold values and
 *      the ADC sampling time. The parameters in the message are transfered to
 *      the data structure of the addressed module instance. The modified
 *      settings influence the behavior of the module task. The following
 *      settings will be modified:
 *
 *      - Module enabled bit
 *      - Displacement communication mode (1 = permanent / 0 = contact)
 *      - Calibration mode active
 *      - ADC sampling time
 *      - Upper threshold
 *      - Lower threshold
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t joysSetConfiguration (UInt16 Channel, CanMessage_t* Message)
{
    UInt16 Mode;
    Error_t Error;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 7) {
        Mode = bmGetMessageItem(Message, 0, 1);
        Data->SamplingTime = bmGetMessageItem(Message, 1, 2);
        Data->UpperThreshold = bmGetMessageItem(Message, 3, 2);
        Data->LowerThreshold = bmGetMessageItem(Message, 5, 2);

        if ((Mode & MODE_CALIBRATION) != 0) {
            Data->XAxis.Minimum = MAX_INT16;
            Data->XAxis.Maximum = MIN_INT16;
            Data->YAxis.Minimum = MAX_INT16;
            Data->YAxis.Maximum = MIN_INT16;
            Data->Flags |= MODE_CALIBRATION;
        }
        else {
            if ((Data->Flags & MODE_CALIBRATION) != 0) {
                Error = bmSetStorageItem(Data->HandleStorage, PARAM_MIN_X, Data->XAxis.Minimum);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                Error = bmSetStorageItem(Data->HandleStorage, PARAM_MAX_X, Data->XAxis.Maximum);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                Error = bmSetStorageItem(Data->HandleStorage, PARAM_MIN_Y, Data->YAxis.Minimum);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                Error = bmSetStorageItem(Data->HandleStorage, PARAM_MAX_Y, Data->YAxis.Maximum);
                if (Error < NO_ERROR) {
                    return (Error);
                }
                Data->Flags &= ~MODE_CALIBRATION;
            }
            else {
                Data->XAxis.Minimum = bmGetStorageItem(Data->HandleStorage, PARAM_MIN_X, MAX_INT16);
                Data->XAxis.Maximum = bmGetStorageItem(Data->HandleStorage, PARAM_MAX_X, (UInt16) MIN_INT16);
                Data->YAxis.Minimum = bmGetStorageItem(Data->HandleStorage, PARAM_MIN_Y, MAX_INT16);
                Data->YAxis.Maximum = bmGetStorageItem(Data->HandleStorage, PARAM_MAX_Y, (UInt16) MIN_INT16);
            }
        }

        Data->XAxis.Center = bmGetStorageItem(Data->HandleStorage, PARAM_CENTER_X, 0);
        Data->YAxis.Center = bmGetStorageItem(Data->HandleStorage, PARAM_CENTER_Y, 0);

        // Set the bits to zero
        Data->Flags &= ~(MODE_MODULE_ENABLE | MODE_COMMUNICATION | MODE_SET_CENTER);
        // Set the public mode flags
        Data->Flags |= (MODE_MODULE_ENABLE | MODE_COMMUNICATION | MODE_SET_CENTER) & Mode;

        Data->SampleTimestamp = bmGetTime();
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief  Sets the minimum and maximum voltages of both axes
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      setting the configuration parameters of the jostick module is received
 *      from the master. The message overwrites the default minimum and
 *      maximum voltages of both joystick axes.The modified settings influence
 *      the behavior of the module task. The following settings will be
        modified:
 *
 *      - X axis minimum voltage (tenth of mV)
 *      - X axis maximum voltage (tenth of mV)
 *      - Y axis minimum voltage (tenth of mV)
 *      - Y axis maximum voltage (tenth of mV)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t joysSetVoltageRange (UInt16 Channel, CanMessage_t* Message)
{
    Error_t Error;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 8) {
        Data->XAxis.Minimum = bmGetMessageItem(Message, 0, 2);
        Data->XAxis.Maximum = bmGetMessageItem(Message, 2, 2);
        Data->YAxis.Minimum = bmGetMessageItem(Message, 4, 2);
        Data->YAxis.Maximum = bmGetMessageItem(Message, 6, 2);

        Error = bmSetStorageItem(Data->HandleStorage, PARAM_MIN_X, Data->XAxis.Minimum);
        if (Error < NO_ERROR) {
            return (Error);
        }
        Error = bmSetStorageItem(Data->HandleStorage, PARAM_MAX_X, Data->XAxis.Maximum);
        if (Error < NO_ERROR) {
            return (Error);
        }
        Error = bmSetStorageItem(Data->HandleStorage, PARAM_MIN_Y, Data->YAxis.Minimum);
        if (Error < NO_ERROR) {
            return (Error);
        }
        Error = bmSetStorageItem(Data->HandleStorage, PARAM_MAX_Y, Data->YAxis.Maximum);
        if (Error < NO_ERROR) {
            return (Error);
        }
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief  Sets the device specific mechanical offset data
 *
 *      This function is called by the CAN message dispatcher when a message 
 *      setting the mechanical offset parameters of the jostick module is
 *      received from the master. The parameters in the message are transfered
 *      to the data structure of the addressed module instance and stored in a
 *      non-volatile memory module. The following settings will be modified:
 *
 *      - Mechanical offset data (4 bytes)
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t joysSetMechOffset (UInt16 Channel, CanMessage_t* Message)
{
    Error_t Error;
    UInt32 MechOffset;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 4) {
        MechOffset = bmGetMessageItem(Message, 0, 4);
        
        Error = bmSetStorageItem (Data->HandleStorage, PARAM_MECH_OFFSET, MechOffset);
        if (Error < 0) {
            return (Error);
        }

        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*! 
 *  \brief  Requests the displacement factors of the joystick
 *
 *      This function is called by the CAN message dispatcher when a
 *      displacement factors get message is received from the master. As a
 *      response the displacement factor of the joystick is sent back to the
 *      master. The following settings will be responded:
 *
 *      - Displacement factor of the X axis
 *      - Displacement factor of the Y axis
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t joysGetDisplacement (UInt16 Channel, CanMessage_t* Message)
{
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if ((Data->Flags & MODE_MODULE_ENABLE) != 0) {
        return joysSendDisplacement (Channel, Data->XAxis.Displacement, Data->YAxis.Displacement);
    }
    return (E_JOYS_MODULE_DISABLED);
}


/*****************************************************************************/
/*! 
 *  \brief  Requests the mechanical offset from the non-volatile memory
 *
 *      This function is called by the CAN message dispatcher when a
 *      mechanical offset data get message is received from the master. As a
 *      response the mechanical offset data from the non-volatile storage
 *      memory is sent back to the master. The following settings will be
 *      responded:
 *
 *      - Mechanical offset data
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t joysGetMechOffset (UInt16 Channel, CanMessage_t* Message)
{
    UInt32 MechOffset;
    CanMessage_t RespMessage;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    MechOffset = bmGetStorageItem (Data->HandleStorage, PARAM_MECH_OFFSET, 0);
    
    RespMessage.CanID = MSG_JOYS_RESP_MECH_OFFSET;
    bmSetMessageItem (&RespMessage, MechOffset, 0, 4);
    RespMessage.Length = 4;
    
    return (canWriteMessage(Data->Channel, &RespMessage));
}


/*****************************************************************************/
/*! 
 *  \brief   Sends the displacement factors of the joystick
 *
 *      Sends the displacement factors of the joystick over the CAN-Bus. The
        message only contains the data value.
 * 
 *  \iparam  Channel = Channel ID
 *  \iparam  DisplacementX = displacement factor of the X axis
 *  \iparam  DisplacementY = displacement factor of the Y axis
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t joysSendDisplacement (UInt16 Channel, UInt16 DisplacementX, UInt16 DisplacementY)
{
    CanMessage_t Message;

    Message.CanID = MSG_JOYS_RESP_DISPLACEMENT;
    bmSetMessageItem (&Message, DisplacementX, 0, 2);
    bmSetMessageItem (&Message, DisplacementY, 2, 2);
    Message.Length = 4;
    
    return (canWriteMessage(Channel, &Message));
}


/******************************************************************************/
/*! 
 *  \brief   Reset non-volatile data to default
 *
 *      Resets all data in instance's data partition to defaults and sends
 *      an info message about that fact to the master for logging.
 * 
 *      This function is called by the base module via the module control
 *      function, whenever a reset-counter service command is received.
 *
 *  \xparam  Data = Module instance data pointer
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t joysResetPartition (InstanceData_t *Data)
{
    Error_t Status;
    
    if (Data->HandleStorage >= 0) {               
        bmSignalEvent(Data->Channel, I_PARTITION_RESET, 1, 0);
        Data->XAxis.Minimum = MAX_INT16;
        Data->XAxis.Maximum = MIN_INT16;
        Data->YAxis.Minimum = MAX_INT16;
        Data->YAxis.Maximum = MIN_INT16;
                        
        Status = bmResetStorageItems (Data->HandleStorage, PermDataTable, ELEMENTS(PermDataTable));
        return (Status);
    }
    return (E_STORAGE_OPEN_ERROR);
}


/******************************************************************************/
/*! 
 *  \brief   Verify module's data partition
 *
 *      Verifies the content of the data partition for plausibility. Each 
 *      data item must be inside the range defined for that item. If only
 *      a single data item is outside it's range, ALL data items are reset 
 *      to defaults. If the partition is ok, the partition layout version 
 *      is checked. If the layout version in the partition is older than 
 *      the one required by the actual software, data layout in the  
 *      partition will be converted. 
 * 
 *      If the layout version is zero, all data items in the partition
 *      are set to defaults as well. This may happen, if the partition
 *      was erased due to a checksum error or during manufacturing.
 * 
 *  \xparam  Data = Module instance data pointer
 * 
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/

static Error_t joysVerifyPartition (InstanceData_t *Data)
{
    Error_t Error;
    UInt32 LayoutVersion;

    if (Data->HandleStorage >= 0) {
        if (!bmVerifyStorageItems (Data->HandleStorage, PermDataTable, ELEMENTS(PermDataTable))) {
            bmSignalEvent (Data->Channel, E_PERSISTENTS_INVALID, 1, 0);
            Error = joysResetPartition (Data);
            if (Error < NO_ERROR) {
                return (Error);
            }
        }
        LayoutVersion = bmGetStorageItem (Data->HandleStorage, PARAM_LAYOUT_VERSION, 0);

        // if required, upgrade partition to new layout format
        if (LayoutVersion < PARTITION_VERSION) {
            // currently no conversion required, this signal is sent out anyway to please the Master
            bmSignalEvent (Data->Channel, I_PARTITION_CONVERTED, 1, LayoutVersion);
        }
        return (NO_ERROR);
    }
    return (E_STORAGE_OPEN_ERROR);
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

Error_t joysInitializeModule (UInt16 ModuleID, UInt16 Instances)
{
    static bmCallbackEntry_t Commands[] = {
        { MSG_JOYS_SET_CONFIGURATION, joysSetConfiguration },
        { MSG_JOYS_SET_VOLTAGE_RANGE, joysSetVoltageRange },
        { MSG_JOYS_SET_MECH_OFFSET, joysSetMechOffset },
        { MSG_JOYS_REQ_DISPLACEMENT, joysGetDisplacement },
        { MSG_JOYS_REQ_MECH_OFFSET, joysGetMechOffset }
    };
    
    static bmModuleInterface_t Interface = {
        joysModuleTask, 
        joysModuleControl, 
        joysModuleStatus
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
    #ifdef DEBUG
    joysRegisterDebugNames(ModuleID);
    #endif

    // bind CAN commands to the handler functions
    Status = canRegisterMessages (ModuleID, Commands, ELEMENTS(Commands));
    if (Status < 0) {
        return (Status);
    }
    // open channels and ports for all modules
    for (i=0; i < Instances; i++) {
        
        DataTable[i].Channel = bmGetChannel(bmGetTaskID(ModuleID, i));
        DataTable[i].Flags = 0;

        //Start time recording        
        DataTable[i].SampleTimestamp = bmGetTime();
 
        // Open analog input of the joystick X axis
        DataTable[i].XAxis.Handle = halAnalogOpen (HAL_JOYSTICK_XAXIS + i, HAL_OPEN_READ, 0, NULL);
        if (DataTable[i].XAxis.Handle < 0) {
            return (DataTable[i].XAxis.Handle);
        }
        
        // Open analog input of the joystick Y axis
        DataTable[i].YAxis.Handle = halAnalogOpen (HAL_JOYSTICK_YAXIS + i, HAL_OPEN_READ, 0, NULL);
        if (DataTable[i].YAxis.Handle < 0) {
            return (DataTable[i].YAxis.Handle);
        }

        // Open storage
        DataTable[i].HandleStorage = bmOpenPermStorage (ModuleID, i, PARAM_TOTAL_SIZE);
        if (DataTable[i].HandleStorage < 0) {
            return (DataTable[i].HandleStorage);
        }

        // Verify storage
        Status = joysVerifyPartition(&DataTable[i]);
        if (Status < 0) {
            return (Status);
        }
       
        DataTable[i].ModuleState = MODULE_STATE_READY;
    }
    InstanceCount = Instances;
    ModuleIdentifier = ModuleID;
    
    return (NO_ERROR);
}

//****************************************************************************/

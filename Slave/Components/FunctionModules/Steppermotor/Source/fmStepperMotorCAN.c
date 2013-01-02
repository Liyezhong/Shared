/****************************************************************************/
/*! \file
 * 
 *  \brief CAN-communication functions of module 'stepper motor'
 *
 *  $Version: $ 0.1
 *  $Date:    $ 03.07.2012
 *  $Author: $ Rainer Boehles
 *
 *  \b Description:
 *
 *      This module contains the functions to send / receive CAN-bus messages
 *      used by FM 'stepper motor' control.
 *
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include <stdlib.h> 

#include "Global.h"

#include "fmStepperMotor.h"
#include "fmStepperMotorConfig.h"
#include "fmStepperMotorCAN.h"


//! Macro to signal event and set NACK status
#define SM_SIGNAL_EVENT(event)          \
    {                                               \
        bmSignalEvent (Channel, event, TRUE, 0);    \
        Ack = SM_NACK;                              \
    }                                               \



/****************************************************************************/
/*! 
 *  \brief   Send CAN message for enable / disable acknowledgement
 *
 *      Sends the can massage to acknowledge that stepper module
 *      enable/disabe request was successful 
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Ack     = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smSendSetEnableAck(UInt16 Channel, SM_AckState_t Ack)
{
    CanMessage_t Message;
    Msg_EnableAckData_t *AckData = (Msg_EnableAckData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_SET_ENABLE_ACK;
    Message.Length = MSG_SMOT_SET_ENABLE_ACK_DLC;

    AckData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/******************************************************************************/
/*! 
 *  \brief   Enable / Disable the module
 *
 *      This function is called by the CAN message dispatcher when a enable/disable
 *      request message is received from the master.
 *
 *      For debug purposes this message can also be used to switch of the need for
 *      execution of an initial reference run.
 *
 *      On success an acknowledge message is sent back to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Pointer to received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/  
Error_t smSetEnableState(UInt16 Channel, CanMessage_t* Message)
{
    Error_t RetCode  = NO_ERROR;
    SM_AckState_t Ack = SM_ACK;

    smData_t* Data = &smDataTable[bmGetInstance(Channel)];
    
    Msg_EnableData_t *EnableData = (Msg_EnableData_t*)Message->Data;

    if (EnableData->enable != Data->Flags.Enable)
    {
        if (EnableData->enable)     // enable the stepper
        {
            if ((RetCode = smEnable(Data, EnableData->dbg_skipRefRun)) < 0)
                SM_SIGNAL_EVENT (RetCode);
        }
        else                        // disable the stepper
        {
            if ((RetCode = smCloseDevices(Data)) < 0)   // close hal devices, this will also switch off driver stage
                SM_SIGNAL_EVENT (RetCode);

            Data->Flags.Enable = 0;
        }
    }

    return smSendSetEnableAck(Channel, Ack);
}


/*****************************************************************************/
/*! 
 *  \brief   Send CAN message to acknowledge termination of reference run
 *
 *      Sends can massage to acknowledge that reference run is finished.
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Pos     = Position (in half-step)
 *  \iparam  PosCode = Limit switch position code
 *  \iparam  Ack     = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smRefRunAck(UInt16 Channel, Int32 Pos, Int8 PosCode, SM_AckState_t Ack)
{
    CanMessage_t Message;
    Msg_RefRunAckData_t *AckData = (Msg_RefRunAckData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_REFERENCE_RUN_ACK;
    Message.Length = MSG_SMOT_REFERENCE_RUN_ACK_DLC;

    VAL_TO_DB4 (Pos, AckData->pos);
    AckData->posCode = PosCode;
    AckData->ack     = Ack;

    return (canWriteMessage(Channel, &Message));
}


/*****************************************************************************/
/*! 
 *  \brief   Send CAN message to acknowledge the reference run request
 *
 *      Sends the can massage to acknowledge that the received
 *      reference run request was accepted.
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Ack     = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smReferenceRunReqAck(UInt16 Channel, SM_AckState_t Ack)
{
    CanMessage_t Message;
    Msg_RefRunReqAckData_t *AckData = (Msg_RefRunReqAckData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_REQ_REF_RUN_ACK;
    Message.Length = MSG_SMOT_REQ_REF_RUN_ACK_DLC;

    AckData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/******************************************************************************/
/*! 
 *  \brief  Request reference run
 *
 *      This function is called by the CAN message dispatcher when a reference
 *      run request message is received from the master.
 *      Module state is checked. If everything is ok the movement is started.
 *      
 *      A acknowledge message for the request is sent back to the master
 *
 *      Later, when the reference run is finished, an additional acknowledge
 *      message is sent back to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/  
Error_t smReferenceRun(UInt16 Channel, CanMessage_t* Message)
{
    Error_t RetCode = NO_ERROR;
    SM_AckState_t Ack = SM_ACK;

    smData_t* Data = &smDataTable[bmGetInstance(Channel)];
    
    Msg_RefRunData_t *RefRunData = (Msg_RefRunData_t*)Message->Data;

    if (MSG_SMOT_REQ_REF_RUN_DLC != Message->Length)
        SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

    // check module state
    if (!Data->Flags.Enable)
        SM_SIGNAL_EVENT (E_MODULE_NOT_ENABLED);
    if ((SM_STATE_INIT != Data->State) && (SM_STATE_IDLE != Data->State))
        SM_SIGNAL_EVENT (E_COMMAND_REJECTED);

    // check profile index
    Data->RefRun.Profile = RefRunData->profile;
    if(Data->RefRun.Profile >= Data->Profiles.Count)
        SM_SIGNAL_EVENT (E_PARAMETER_OUT_OF_RANGE);

    // acknowledge the request
    if ((RetCode = smReferenceRunReqAck(Channel, Ack)) < 0) {
        return RetCode;
    }

    // if everything is ok then initiate the reference run
    if (SM_ACK == Ack)
    {
        // skip ref-run movements if reference position is not defined
        if (0 == Data->RefRun.Config.RefPos) {
            Data->RefRun.State = SM_RRS_ASSIGN_OFFSET;
        }
        else {
            Data->RefRun.State = SM_RRS_FAST_MOTION_START;
        }

    // set stepper module state accordingly, to mark that reference run is active
    // inside "smRefRunTask" the final acknowledge message for the reference run will be sent to master
        Data->State = SM_STATE_REFRUN;
    }

    return RetCode;
}


/*****************************************************************************/
/*! 
 *  \brief   Send CAN message to acknowledge the position request
 *
 *      Sends the can massage to acknowledge that the received
 *      position request was accepted.
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Ack     = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smTargetPositionReqAck(UInt16 Channel, SM_AckState_t Ack)
{
    CanMessage_t Message;
    Msg_TargetPosAckData_t *AckData = (Msg_TargetPosAckData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_TARGET_POS_ACK;
    Message.Length = MSG_SMOT_TARGET_POS_ACK_DLC;

    AckData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/******************************************************************************/
/*! 
 *  \brief  Request movement to target position
 *
 *      This function is called by the CAN message dispatcher when a position
 *      movement request message is received from the master.
 *      Movement parameters and module state are checked. If everything is ok
 *      the movement is started.
 *      
 *      A acknowledge message for the request is sent back to the master
 *
 *      Later, when the movement is finished, an additional acknowledge message
 *      is sent back to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/  
Error_t smTargetPosition(UInt16 Channel, CanMessage_t* Message)
{
    Error_t RetCode = NO_ERROR;             
    SM_AckState_t Ack = SM_ACK;

    Int32   Position;
    UInt8   ProfileIndex;

    smData_t* Data = &smDataTable[bmGetInstance(Channel)];

    Msg_TargetPositionData_t *PositionData  = (Msg_TargetPositionData_t*)Message->Data;

    if (MSG_SMOT_TARGET_POS_DLC != Message->Length)
        SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

    // check profile index
    ProfileIndex = PositionData->profile;
    if(ProfileIndex >= Data->Profiles.Count)
        SM_SIGNAL_EVENT (E_PARAMETER_OUT_OF_RANGE);
    
    // check target position
    DB4_TO_VAL (PositionData->pos, Position);
    if (Position < Data->Motor.FrameworkConfig.MinPosition)
        SM_SIGNAL_EVENT (E_PARAMETER_OUT_OF_RANGE);
    if (Position > Data->Motor.FrameworkConfig.MaxPosition)
        SM_SIGNAL_EVENT (E_PARAMETER_OUT_OF_RANGE);

    // check module state
    if (!Data->Flags.Enable)
        SM_SIGNAL_EVENT (E_MODULE_NOT_ENABLED);
    if (SM_STATE_IDLE != Data->State)
        SM_SIGNAL_EVENT (E_COMMAND_REJECTED);

    // if everything is ok then movement can start)
    if (SM_ACK == Ack)
    {
        // start movement if requested position is different from actual postion
        if (Position != Data->Motion.Pos) {
            RetCode = smPositionRequest (Data->Instance, Position, ProfileIndex);
        }
        if (RetCode < 0)
        {
            SM_SIGNAL_EVENT (RetCode);
        }
        else
        {
            // set stepper module state accordingly, to mark that a movement to target postion is active
            // inside "smModuleTask" the final acknowledge message for the movement will be sent to master
            Data->State = SM_STATE_POSITION;
        }
    }

    // acknowledge the request
    return smTargetPositionReqAck(Channel, Ack);
}


/*****************************************************************************/
/*! 
 *  \brief   Send CAN message to acknowledge the speed request
 *
 *      Sends the can massage to acknowledge that the received
 *      speed request was accepted.
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Ack     = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smTargetSpeedReqAck(UInt16 Channel, SM_AckState_t Ack)
{
    CanMessage_t Message;
    Msg_TargetSpeedAckData_t *AckData   = (Msg_TargetSpeedAckData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_TARGET_SPEED_ACK;
    Message.Length = MSG_SMOT_TARGET_SPEED_ACK_DLC;

    AckData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/******************************************************************************/
/*! 
 *  \brief  Request movement with target speed
 *
 *      This function is called by the CAN message dispatcher when a speed
 *      movement request message is received from the master.
 *      Movement parameters and module state are checked. If everything is ok
 *      the movement is started.
 *      
 *      A acknowledge message for the request is sent back to the master
 *
 *      Later, when the movement reached target speed, an additional acknowledge
 *      message is sent back to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/  
Error_t smTargetSpeed(UInt16 Channel, CanMessage_t* Message)
{
    Error_t RetCode = NO_ERROR;             
    SM_AckState_t Ack = SM_ACK;

    Int16   Speed;
    UInt8   ProfileIndex;

    smData_t* Data = &smDataTable[bmGetInstance(Channel)];

    Msg_TargetSpeedData_t *SpeedData = (Msg_TargetSpeedData_t*)Message->Data;

    if (MSG_SMOT_TARGET_SPEED_DLC != Message->Length)
        SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

    // check profile index
    ProfileIndex = SpeedData->profile;
    if(ProfileIndex >= Data->Profiles.Count)
        SM_SIGNAL_EVENT (E_PARAMETER_OUT_OF_RANGE);
    
    // get target speed
    DB2_TO_VAL (SpeedData->speed, Speed);

    // check module state
    if (!Data->Flags.Enable)
        SM_SIGNAL_EVENT (E_MODULE_NOT_ENABLED);
    if ((SM_STATE_IDLE != Data->State) && (SM_STATE_SPEED != Data->State) && (SM_STATE_POSITION != Data->State))
        SM_SIGNAL_EVENT (E_COMMAND_REJECTED);
  
    // if everything is ok then movement can start)
    if (SM_ACK == Ack)
    {
        // start movement if requested position is different from actual postion
        if ((RetCode = smSpeedRequest (Data->Instance, Speed, ProfileIndex)) < 0)
        {
            SM_SIGNAL_EVENT (RetCode);
        }
        else
        {
            // set steper module state accordingly, to mark that a movement to reach target speed is active
            // inside "smModuleTask" the final acknowledge message for the movement will be sent to master
            Data->State = SM_STATE_SPEED;
        }
    }

    // acknowledge the request
    return smTargetSpeedReqAck(Channel, Ack);
}


/*****************************************************************************/
/*! 
 *  \brief   Send actual motor position data to the master
 *
 *      Sends CAN message with position data to master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Pos     = Position (in half-step)
 *  \iparam  PosCode = Limit switch position code
 *  \iparam  Ack     = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smSendPosition(UInt16 Channel, Int32 Pos, Int8 PosCode, SM_AckState_t Ack)
{
    CanMessage_t Message;
    Msg_PositionData_t *PositionData    = (Msg_PositionData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_ACT_POS;
    Message.Length = MSG_SMOT_ACT_POS_DLC;

    VAL_TO_DB4 (Pos, PositionData->pos);
    PositionData->posCode = PosCode;
    PositionData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/******************************************************************************/
/*! 
 *  \brief  Request the actual position
 *
 *      This function is called by the CAN message dispatcher when a position
 *      data request message is received from the master.
 *      If module is enabled a data message is sent back to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/  
Error_t smReqPosition(UInt16 Channel, CanMessage_t* Message)
{
    SM_AckState_t Ack = SM_ACK;

    smData_t *Data = &smDataTable[bmGetInstance(Channel)];
    
    if (MSG_SMOT_ACT_POS_REQ_DLC != Message->Length)
        SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

    if(!Data->Flags.Enable)
        SM_SIGNAL_EVENT (E_MODULE_NOT_ENABLED);

    return smSendPosition(Channel, Data->Motion.Pos, Data->LimitSwitches.PosCode.Value, Ack);
}


/*****************************************************************************/
/*! 
 *  \brief   Send actual motor speed to the master
 *
 *      Sends CAN message with speed and position data to master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Pos     = Position ( in half-step )
 *  \iparam  Speed   = Speed ( in half steps/s )
 *  \iparam  Ack     = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smSendSpeed(UInt16 Channel, Int32 Pos, Int16 Speed, SM_AckState_t Ack)
{
    CanMessage_t Message;
    Msg_SpeedData_t *SpeedData = (Msg_SpeedData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_ACT_SPEED;
    Message.Length = MSG_SMOT_ACT_SPEED_DLC;

    VAL_TO_DB2 (Speed, SpeedData->speed);
    SpeedData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/******************************************************************************/
/*! 
 *  \brief  Request the actual speed
 *
 *      This function is called by the CAN message dispatcher when a speed 
 *      data request message is received from the master.
 *      If module is enabled a data message is sent back to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/  
Error_t smReqSpeed(UInt16 Channel, CanMessage_t* Message)
{
    SM_AckState_t Ack = SM_ACK;

    smData_t *Data = &smDataTable[bmGetInstance(Channel)];

    Int16 Speed = Data->Motion.Nominal.v >> 5; // speed (in half-step/s²) = velocity / ISR_MICROSTEPS_PER_HALFSTEP;
    if (Data->Motion.Param[Data->Motion.ActSet].NegPosCount) {
        Speed = -Speed;
    }
    
    if (MSG_SMOT_ACT_SPEED_REQ_DLC != Message->Length)
        SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

    if(!Data->Flags.Enable)
        SM_SIGNAL_EVENT (E_MODULE_NOT_ENABLED);

    return smSendSpeed(Channel, Data->Motion.Pos, Speed, Ack);
}


/*****************************************************************************/
/*! 
 *  \brief   Send CAN message to acknowledge the last movement
 *
 *      Sends the can massage to acknowledge that the received
 *      position or speed request is finished.
 *
 *      The CAN message reports the status of the finished movement
 *      to inform the master if movement was successful or terminated
 *      because of an error.
 *      As additional info the CAN message also reports the actual
 *      motor position and speed.
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Pos     = Position ( in half-step )
 *  \iparam  Speed   = Speed ( in half steps/s )
 *  \iparam  Ack     = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smSendMovementAckn(UInt16 Channel, Int32 Pos, Int16 Speed, SM_AckState_t Ack)
{
    CanMessage_t Message;
    Msg_MovementAckData_t *AckData = (Msg_MovementAckData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_MOVEMENT_ACK;
    Message.Length = MSG_SMOT_MOVEMENT_ACK_DLC;

    VAL_TO_DB4 (Pos, AckData->pos);    
    VAL_TO_DB2 (Speed, AckData->speed);
    AckData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/*****************************************************************************/
/*! 
 *  \brief   Send CAN message for configuration data acknowledge
 *
 *      Sends the can massage to acknowledge that the received
 *      configuration data was accepted.
 * 
 *  \iparam  Channel = Logical channel number
 *  \iparam  Ack     = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smSendConfigAck(UInt16 Channel, SM_AckState_t Ack)
{
    CanMessage_t Message;
    Msg_ConfigAckData_t *AckData = (Msg_ConfigAckData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_CONFIG_ACK;
    Message.Length = MSG_SMOT_CONFIG_ACK_DLC;

    AckData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/*****************************************************************************/
/*!
 *  \brief  Sets the configuration parameters
 *
 *      This function is called by the CAN message dispatcher when an stepper
 *      configuration message is received from the master. The parameters
 *      in the message are transfered to the data structure of the addressed
 *      module instance. The parameters influence the behaivor of the
 *      stepper movement. In general parameters can be divided into this two
 *      groups
 *
 *      - Stepper parameters
 *      - Motion profile parameters
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
Error_t smConfigure(UInt16 Channel, CanMessage_t* Message)
{
    Error_t RetCode = NO_ERROR;
    SM_AckState_t Ack = SM_ACK;

    smData_t* Data = &smDataTable[bmGetInstance(Channel)];

    ConfigData_Param_t *ConfigParam = (ConfigData_Param_t*)Message->Data;

// configuration is only allowed if stepper module is disabled
    if(Data->Flags.Enable)
        SM_SIGNAL_EVENT (E_SMOT_INVALID_STATE);

    if (! ConfigParam->index.type.profileData) {
        RetCode = smConfigureParam(Data, ConfigParam, Message->Length);
    }
    else 
    {   // profile parameters
        if (NULL == Data->Profiles.Set)
            SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

        if (ConfigParam->index.profile.no >= Data->Profiles.Count)
            SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

        if (ConfigParam->index.profile.no >= MAX_MOTION_PROFIL)
            SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

        RetCode = smConfigureProfile(&(Data->Profiles), ConfigParam->index.profile.no, ConfigParam, Message->Length);
    }

    if (NO_ERROR != RetCode)
        SM_SIGNAL_EVENT (RetCode);

    // acknowledge reception of parameters
    return smSendConfigAck(Data->Channel, Ack);
}


/*****************************************************************************/
/*!
 *  \brief   Send motor revolution count (life cycle data)
 *
 *      Sends total revolution count of the motor via CAN to the master.
 *
 *  \iparam  Channel         = Logical channel number
 *  \iparam  RevolutionCount = motor's revolution count
 *  \iparam  Ack             = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
static Error_t bmSendRevolutionCount (UInt16 Channel, UInt32 RevolutionCount, SM_AckState_t Ack)
{
    CanMessage_t Message;

    Msg_RevolutionsData_t *RevolutionsData = (Msg_RevolutionsData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_REVCOUNT;
    Message.Length = MSG_SMOT_REVCOUNT_DLC;

    VAL_TO_DB4 (RevolutionCount, RevolutionsData->count);
    RevolutionsData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/******************************************************************************/
/*! 
 *  \brief  Request motor revolution count (life cycle data)
 *
 *      This function is called by the CAN message dispatcher whenever the
 *      life cycle command "request revolution count" is received.
 *      A data message is always sent back to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/  
Error_t smReqRevolutionCount(UInt16 Channel, CanMessage_t* Message)
{
    smData_t *Data = &smDataTable[bmGetInstance(Channel)];

    SM_AckState_t Ack = SM_ACK;

    if (MSG_SMOT_REVCOUNT_REQ_DLC != Message->Length)
        SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

    return bmSendRevolutionCount(Channel, Data->Motor.LifeCycle.Revolutions.Count, Ack);
}


/*****************************************************************************/
/*!
 *  \brief   Send motor operation time (life cycle data)
 *
 *      Sends total operation time of the motor via CAN to the master.
 *
 *  \iparam  Channel       = Logical channel number
 *  \iparam  OperationTime = motors operation time in hours
 *  \iparam  Ack           = success/failed status
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
static Error_t smSendOperationTime (UInt16 Channel, UInt32 OperationTime, SM_AckState_t Ack)
{
    CanMessage_t Message;

    Msg_OperationTimeData_t *OperationTimeData = (Msg_OperationTimeData_t*)Message.Data;

    Message.CanID  = MSG_SMOT_OPTIME;
    Message.Length = MSG_SMOT_OPTIME_DLC;

    VAL_TO_DB4 (OperationTime, OperationTimeData->hours);
    OperationTimeData->ack = Ack;

    return (canWriteMessage(Channel, &Message));
}


/******************************************************************************/
/*! 
 *  \brief  Request motor operation time (life cycle data)
 *
 *      This function is called by the CAN message dispatcher whenever the
 *      life cycle command "request operation time" is received.
 *      A data message is always sent back to the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ******************************************************************************/  
Error_t smReqOperationTime(UInt16 Channel, CanMessage_t* Message)
{
    smData_t *Data = &smDataTable[bmGetInstance(Channel)];

    SM_AckState_t Ack = SM_ACK;
    UInt32 OperationTime;

    if (MSG_SMOT_OPTIME_REQ_DLC != Message->Length)
        SM_SIGNAL_EVENT (E_UNEXPECTED_PARAMETERS);

    if (Data->Memory.Handle < 0)
        SM_SIGNAL_EVENT (E_STORAGE_READ_ERROR);

    OperationTime = smGetOperationTime(&Data->Memory);

    return smSendOperationTime(Channel, OperationTime / 60, Ack);
}

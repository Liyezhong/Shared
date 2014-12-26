/****************************************************************************/
/*! \file fmPressure.c
 *
 *  \brief Functional Module controlling pressure regulation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 20.08.2012
 *   $Author:  $ Brandon Shao
 *
 *  \b Description:
 *
 *      This module contains the top level source code of a pressure
 *      controller. The code in this file binds different subcomponents, that
 *      are generally needed for pressure regulation. The module controls
 *      various pumps and pressure indicators through the hardware
 *      abstraction layer. It furthermore watches ventilation fans, the
 *      lifetime of pumping elements and sets parameters for pressure
 *      regulation.
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
#include "fmPressure.h"
#include "fmPressureFan.h"
#include "fmPressurePump.h"
#include "fmPressurePid.h"
#include "fmPressureSensor.h"
#include "fmPressureTime.h"

//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

/* Global defines of the pressure module */
#define MODULE_VERSION      0x0001  //!< Version number of module

/* Mode bits for the Flags member of the module instance data */
#define MODE_MODULE_ENABLE   0x0001  //!< The regulation is on or off
#define MODE_AUTO_TUNE       0x0002  //!< Auto tuning is in progress
#define MODE_PRESS_PHASE     0x0004  //!< 0: pumping phase, 1: hold phase
#define MODE_PRESS_MODE      0x0008  //!< 0: Pressure, 1: Vacuum
#define MODE_PRESS_ACTUATE   0x0010  //!< 0: ON/OFF, 1: PWM
#define MODE_PRESS_RANGE     0x0020  //!< Actual pressure is in required range

/* Time constants for sensor error report */
#define SENSOR_ERROR_DURATION  3000  //!< Time duration in miliseconds to confirm sensor error

/* Default error value for pressure sensor out of range */
#define SENSOR_ERROR_VALUE     29900 //!< Default return value if sensor out of range 

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

/*! State of the module task */
typedef enum {
    STATE_IDLE,     //!< Wait for sampling time
    STATE_SAMPLE,   //!< Sample all inputs and calculate output
    STATE_CONTROL   //!< Write the control output
} PressTaskState_t;

/*! Current operating phase of the module (pumping or hold) */
typedef enum {
    PHASE_PUMP,     //!< Pump phase
    PHASE_HOLD      //!< Hold phase
} PressPumpPhase_t;

/*! Current pump working mode (pressure or vacuum) */
typedef enum {
    MODE_PRESSURE,  //!< Pressure phase (to positive)
    MODE_VACUUM     //!< Vacuum phase (to negtive)
} PressPumpMode_t;

/*! Current pump actuating method (ON/OFF or PWM) */
typedef enum {
    ACTUATE_ONOFF,  //!< ON/OFF actuating
    ACTUATE_PWM     //!< PWM actuating
} PressPumpActuateMode_t;

/*! Contains all variables for a instance of this module */
typedef struct {
    bmModuleState_t ModuleState; //!< Module state
    Handle_t *HandlePress;       //!< Handle to access analog input ports(HAL) for pressure sensor
    Handle_t *HandleValve;       //!< Handle to valve control ports
    UInt16 Channel;              //!< Logical CAN channel
    UInt16 Flags;                //!< Mode control flag bits
    PressTaskState_t State;      //!< The module task state
    UInt8 Priority;              //!< Heating priority in hold phase (0 is highest)

    Int32 AtmPress;              //!< Atmospheric pressure  
    Int32 DesiredPress;          //!< Desired pressure (in Pa steps)
    UInt16 TolerancePress;       //!< Pressure tolerance (in Pa steps)

    Int32* ServicePress;         //!< Pressure of a certain sensor in 0.001 KPa steps     
    UInt8* ServiceValve;         //!< Valve status
    
    UInt32* SensorErrTimestamp;  //!< Time stamp for trigeering sensor error reporting

    UInt32 AutoTuneStartTime;    //!< Time in milliseconds auto tuning was started
    UInt32 AutoTuneDuration;     //!< Duration of the auto tuning algorithm in milliseconds
    UInt8  AutoTunePidNumber;    //!< Number of controller currently tuned
    Int32  ActuatingValue;       //!< Most recent actuating value for pressure regulation
    UInt8  ActuatingPwmWidth;    //!< PWM duty in percentage

    UInt8 NumberSensors;         //!< Number of independent pressure sensors
    UInt8 NumberPumps;           //!< Number of pumping elements
    UInt8 NumberPid;             //!< Number of cascaded PID controllers
    UInt8 NumberValves;          //!< Number of air valves
    
    PressSensorType_t SensorType; //!< Type of pressure sensor used by this module
    PressTimeParams_t TimeParams; //!< Input and output parameters of the lifecycle counters
    PressPidParams_t *PidParams;  //!< Input and output parameters of the PID controller
#ifdef ASB15_VER_B    
    PressPwmParams_t *PwmParams;  //!< Input parameters and coefficients of the PWM controller
#endif    
} InstanceData_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

static UInt16 InstanceCount = 0;        //!< number of module instances
static UInt16 ModuleIdentifier = 0;     //!< module identifier

static InstanceData_t* DataTable;  //!< data table for all instances

/*! Pump or hold phase */
static PressPumpPhase_t PressPhase;
/*! Pressure or Vacuum mode */
static PressPumpMode_t  PressPumpMode;
/*! Actuating mode - on/off or PWM */
static PressPumpActuateMode_t PressPumpActuateMode;
/*! Time in milliseconds the last pressure value was sampled */
static UInt32 PressSampleTimestamp;
/*! Sampling time of the module in milliseconds */
static UInt32 PressSamplingTime = 1000;

/*! Public parameters for the pumping element control functionality */
static PressPumpParams_t *pressPumpParams;
/*! Public parameters for the fan element control functionality */
static PressFanParams_t *pressFanParams;

/*! Flag for indicating change of pressure setting */
static Bool PressChanged = FALSE;
/*! Flag for indicating if pump check skipped after pressure changed */
static Bool PumpCheckSkipped = FALSE;

/*! Flag for indicating change of fan status */
static Bool FanChanged = FALSE;
/*! Flag for indicating if fan check skipped after status changed */
static Bool FanCheckSkipped = FALSE;

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t pressModuleControl (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t pressModuleStatus  (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t pressModuleTask    (UInt16 Instance);

static Int8    pressFindRoot (void);
static Error_t pressShutDown (InstanceData_t *Data, Error_t Error, UInt16 Instance);
static Error_t pressFetchCheck  (InstanceData_t *Data, UInt16 Instance, Bool *Fail);
static Error_t pressNotifRange  (InstanceData_t *Data);
static Error_t pressRegulation  (InstanceData_t *Data, UInt16 Instance);
static Error_t pressReadOptions (InstanceData_t *Data, UInt16 ModuleID, UInt16 Instance);
static Error_t pressDeviceAlloc (InstanceData_t *Data);
static Error_t pressHandleOpen  (InstanceData_t *Data, UInt16 Instance);
#ifdef ASB15_VER_B
static Bool    pressPwmParamsOk (const PressPwmParams_t *Param);
#endif

static Error_t pressSetPressure          (UInt16 Channel, CanMessage_t* Message);
static Error_t pressSetCurrentWatchdog   (UInt16 Channel, CanMessage_t* Message);
static Error_t pressSetFanWatchdog       (UInt16 Channel, CanMessage_t* Message);
static Error_t pressSetPidParameters     (UInt16 Channel, CanMessage_t* Message);
static Error_t pressSetPumpTime          (UInt16 Channel, CanMessage_t* Message);
static Error_t pressSetValve             (UInt16 Channel, CanMessage_t* Message);
static Error_t pressSetCalibration       (UInt16 Channel, CanMessage_t* Message);
static Error_t pressSetFan               (UInt16 Channel, CanMessage_t* Message);
#ifdef ASB15_VER_B
static Error_t pressSetPwmParameters     (UInt16 Channel, CanMessage_t* Message);
#endif

static Error_t pressGetPressure          (UInt16 Channel, CanMessage_t* Message);
static Error_t pressGetPidParameters     (UInt16 Channel, CanMessage_t* Message);
static Error_t pressGetPumpTime          (UInt16 Channel, CanMessage_t* Message);
static Error_t pressGetServiceSensor     (UInt16 Channel, CanMessage_t* Message);
static Error_t pressGetHardware          (UInt16 Channel, CanMessage_t* Message);

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

static Error_t pressModuleControl (UInt16 Instance, bmModuleControlID_t ControlID)
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
            Error = pressTimeResetPartition (&Data->TimeParams, Data->Channel);
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

static Error_t pressModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID)
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
            return (Data->ServicePress[0] / 10);
        
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

static Error_t pressModuleTask (UInt16 Instance)
{
    Bool Fail = FALSE;
    UInt8 i;
    Error_t Error;

    InstanceData_t* Data = &DataTable[Instance];


    if (Data->ModuleState == MODULE_STATE_READY) {
    
        Error = pressSampleCurrent();  
        if (Error < 0) {
            return (pressShutDown (Data, Error, Instance));
        }

        Error = pressFanSampleCurrent();  
        if (Error < 0) {
            dbgPrint("Fan sampling current error.\n");
        }
                    
        //if (Instance == pressFindRoot ()) {
        if (Instance == 0) {
            if (bmTimeExpired (PressSampleTimestamp) >= PressSamplingTime) {
                // Update the sampling time
                PressSampleTimestamp = bmGetTime();
                for (i = 0; i < InstanceCount; i++) {
                    DataTable[i].State = STATE_SAMPLE;
                }

                if ( PumpCheckSkipped && PressChanged ) {
                    PressChanged = FALSE;
                    PumpCheckSkipped = FALSE;
                }

                if ( FanCheckSkipped && FanChanged ) {
                    FanChanged = FALSE;
                    FanCheckSkipped = FALSE;
                }
            }
        }
        
        
        if (Data->State == STATE_SAMPLE) {
        
            //dbgPrint("Pump current:%d\n", pressPumpCurrent());
            
            if ((Data->Flags & MODE_MODULE_ENABLE) == 0 ) {
                Data->State = STATE_CONTROL;
            }
            
            // check current and sensors
            Error = pressFetchCheck(Data, Instance, &Fail);
            if (Error != NO_ERROR) {
                return (pressShutDown (Data, Error, Instance));
            }
            // If one of the subsystems fails, shutdown and quit
            if (Fail == TRUE) {
                Data->Flags &= ~(MODE_MODULE_ENABLE);
                return ((Error_t) Data->ModuleState);
            }            
        }
        
        Error = pressFanProgress ();
        if (Error != NO_ERROR) {
            dbgPrint("Fan progress error.\n");
        }
    }


    // Is pumping active?
    if (Data->ModuleState == MODULE_STATE_READY && (Data->Flags & MODE_MODULE_ENABLE) != 0) {
        // Progress the current sensor evaluation
        if ((PressPhase == PHASE_PUMP) && (PressPumpActuateMode == ACTUATE_ONOFF) ) {
            Error = pressPumpProgress (FALSE);
        }
        else {
            Error = pressPumpProgress (FALSE);
        }
        if (Error != NO_ERROR) {
            return (pressShutDown (Data, Error, Instance));
        }
        
        // Check if the sampling time expired already
        if (Data->State == STATE_SAMPLE) {

            Data->State = STATE_CONTROL;
            
            // Check if the pressure is within the required range
            Error = pressNotifRange(Data);
            if (Error != NO_ERROR) {
                return (pressShutDown (Data, Error, Instance));
            }
                     
            // Regulate pressure
            Error = pressRegulation(Data, Instance);
            if (Error != NO_ERROR) {
                return (pressShutDown (Data, Error, Instance));
            }

        }

#if 1 
        else if (Data->State == STATE_CONTROL && ((Data->Flags & MODE_MODULE_ENABLE) != 0)) {
            if ((PressPhase == PHASE_PUMP)) {
            
#ifdef ASB15_VER_A            
                UInt32 OperatingTime = 0;
                // Check later. ActuatingValue already >= 0 ?
                if (Data->ActuatingValue > 0) {
                    OperatingTime = (Data->ActuatingValue * PressSamplingTime) / (MAX_INT16);
                }
                
                Data->State = STATE_IDLE;
                
                // Control the pumping elements
                Error = pressPumpActuate (OperatingTime, PressSampleTimestamp + PressSamplingTime, Instance);
                if (Error != NO_ERROR) {
                    return (pressShutDown (Data, Error, Instance));
                }
#endif

#ifdef ASB15_VER_B
                UInt32 OperatingTime = 0;
                if ( PressPumpActuateMode == ACTUATE_PWM ) {
                                 
                    Int32 ActuatingPwmWidth = 0;
                    
                    if (Data->ActuatingPwmWidth > 0) {
                        ActuatingPwmWidth = Data->ActuatingPwmWidth;
                    }
                    
                    //dbgPrint("PWM: %d\n", ActuatingPwmWidth);
    
                    Data->State = STATE_IDLE;
                    
                    // Control the pumping elements
                    Error = pressPumpActuatePwm (ActuatingPwmWidth, PressSampleTimestamp + PressSamplingTime, Instance, &OperatingTime);
                    if (Error != NO_ERROR) {
                        return (pressShutDown (Data, Error, Instance));
                    }
                }
                else if ( PressPumpActuateMode == ACTUATE_ONOFF ) {
                
                    //TODO:
                    //Find a more accurate coefficient to convert PID output to opt. time
                    if (Data->ActuatingValue > 0) {
                        OperatingTime = (Data->ActuatingValue * PressSamplingTime) / (MAX_INT16);
                    }
                    
                    //dbgPrint("Opt[Be]: %d\n", OperatingTime);
                    
                    Data->State = STATE_IDLE;
                    
                    // Control the pumping elements
                    Error = pressPumpActuate (&OperatingTime, PressSampleTimestamp + PressSamplingTime, Instance);
                    if (Error != NO_ERROR) {
                        return (pressShutDown (Data, Error, Instance));
                    }                
                }
#endif                
                
                // Update the pump operating time
                Error = pressTimeUpdate (&Data->TimeParams, OperatingTime);
                if (Error != NO_ERROR) {
                    return (pressShutDown (Data, Error, Instance));
                }
                
                //dbgPrint("Opt[Af]: %d\n", OperatingTime);
            }
            
#ifdef ASB15_VER_B_PWMHOLD
            else if (PressPhase == PHASE_HOLD) {
                UInt32 OperatingTime = 0;
                Int32 ActuatingPwmWidth = 0;
                                
                                            
                if ( (Data->DesiredPress > 0 && Data->ServicePress[0] < Data->DesiredPress-200) || 
                     (Data->DesiredPress < 0 && Data->ServicePress[0] > Data->DesiredPress+200) ) {
                     
                    if (Data->ActuatingPwmWidth > 0) {
                        ActuatingPwmWidth = Data->ActuatingPwmWidth;
                    }
                }
                
                //dbgPrint("PWM: %d\n", ActuatingPwmWidth);                
                
                Data->State = STATE_IDLE;
                
                // Control the pumping elements
                Error = pressPumpActuate (ActuatingPwmWidth, PressSampleTimestamp + PressSamplingTime, Instance, &OperatingTime);
                if (Error != NO_ERROR) {
                    return (pressShutDown (Data, Error, Instance));
                }
                
                // Update the pump operating time
                Error = pressTimeUpdate (&Data->TimeParams, OperatingTime);
                if (Error != NO_ERROR) {
                    return (pressShutDown (Data, Error, Instance));
                }
                                
            }
#endif            
        }
#endif
    }

    else {
#ifdef ASB15_VER_A    
        Error = pressPumpActuate (0, PressSampleTimestamp + PressSamplingTime, Instance);
#endif

#ifdef ASB15_VER_B
        Error = pressPumpActuate (NULL, PressSampleTimestamp + PressSamplingTime, Instance);
#endif

        
        if (Error < NO_ERROR) {
            return (pressShutDown (Data, Error, Instance));
        }

    }
    return ((Error_t) Data->ModuleState);
}


/*****************************************************************************/
/*!
 *  \brief  Find the root instance
 *
 *  This small method returns the ID of the so-called root instance. This is
 *  necessary for hold mode, when more than one pumping circuit have to be
 *  controlled. The root instance is the first instance, that is currently
 *  active.
 *
 *  \return  ID of the root instance
 *
 ****************************************************************************/

static Int8 pressFindRoot (void)
{
    UInt8 i;
    for (i = 0; i < InstanceCount; i++) {
        if (DataTable[i].ModuleState == MODULE_STATE_READY && (DataTable[i].Flags & MODE_MODULE_ENABLE) != 0 ) {
            return (i);
        }
    }
    return (-1);
}


/*****************************************************************************/
/*!
 *  \brief  Deactivates the module and sends an error message
 *
 *      This method deactivates an instance of the pressure function
 *      module, when an error occured. It also forwards the error code to the
 *      master via CAN Bus. After that, it returns the module state as an 
 *      error code.
 *
 *  \xparam  Data = Data of one instance of the functional module
 *  \iparam  Error = Error code forwarded
 *  \iparam  Instance = Instance number
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressShutDown (InstanceData_t *Data, Error_t Error, UInt16 Instance)
{
    UInt8 i;
    
    Data->Flags &= ~(MODE_MODULE_ENABLE);
    bmSignalEvent (Data->Channel, Error, TRUE, 0);
    
    for(i = 0; i < Data->NumberPid; i++) {
        pressPidReset (&Data->PidParams[i]);
    }
#ifdef ASB15_VER_A
    Error = pressPumpActuate (0, PressSampleTimestamp + PressSamplingTime, Instance);
#endif    
    
#ifdef ASB15_VER_B
    Error = pressPumpEnablePower(FALSE, Instance);
    if (Error < NO_ERROR) {
        return (Error);
    }
#endif

#ifdef ASB15_VER_B
    Error = pressPumpActuate (NULL, PressSampleTimestamp + PressSamplingTime, Instance);
#endif  

    if (Error < NO_ERROR) {
        return (Error);
    }
    
    return ((Error_t) Data->ModuleState);
}

/*****************************************************************************/
/*!
 *  \brief  Reads all sensors available and checks their ranges
 *
 *      This method reads all sensor data available to the functional module,
 *      including fan speeds, pump current, pressure sensors. It also
 *      checks if these values are in a valid range. If such a range is
 *      violated, an error message is sent over the CAN Bus.
 *
 *  \xparam  Data = Data of one instance of the functional module
 *  \iparam  Instance = Number of the instance calling the function
 *  \oparam  Fail = Did one of the tests fail
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressFetchCheck (InstanceData_t *Data, UInt16 Instance, Bool *Fail)
{
    UInt8 i;
    Error_t Error = NO_ERROR;

    if ( Instance == 0 ) {
        pressCalcEffectiveCurrent(Instance);
        pressFanCalcEffectiveCurrent(Instance);
    }
    
    if ((Data->Flags & MODE_MODULE_ENABLE) != 0 ) {
        if ( Instance == pressFindRoot () ) {
            //Error = pressPumpCheck();
            if ( PressChanged ) {
                Error = pressPumpCheck (FALSE);
                PumpCheckSkipped = TRUE;
            }
            else {
                Error = pressPumpCheck (TRUE);
            }

            if (Error < 0) {
                return Error;
            }
        }
        
        if (pressPumpFailed () == TRUE) {
            *Fail = TRUE;
            bmSignalEvent (Data->Channel, E_PRESS_CURRENT_OUT_OF_RANGE, TRUE, pressPumpCurrent ());
        }
            
    }
 
    // Measure and check fan current

    //Error = pressFanCheck();
    if ( FanChanged ) {
        Error = pressFanCheck (FALSE);
        FanCheckSkipped = TRUE;
    }
    else {
        Error = pressFanCheck (TRUE);
    }

    if (Error < 0) {
        return Error;
    }
    
    if (pressFanFailed () == TRUE) {
        bmSignalEvent (Data->Channel, E_PRESS_FAN_OUT_OF_RANGE, TRUE, pressFanCurrent ());
    }
              

    // Measure and check the pressure
    for (i = 0; i < Data->NumberSensors; i++) {
        if ((Error = pressSensorRead (Data->HandlePress[i], Data->SensorType, Data->AtmPress, &Data->ServicePress[i])) < 0) {
            //dbgPrint("%d:E ",i);
            
            if ( Data->SensorErrTimestamp[i] == 0 ) {
                Data->SensorErrTimestamp[i] = bmGetTime();
            }
            else if (bmTimeExpired(Data->SensorErrTimestamp[i]) >= SENSOR_ERROR_DURATION) {
                Data->ServicePress[i] = 299000;
                if ((Data->Flags & MODE_MODULE_ENABLE) != 0 ) {
                    return (Error);
                }
            }
            
        }
        else {
            
            if ( Data->SensorErrTimestamp[i] != 0 ) {
                if (bmTimeExpired(Data->SensorErrTimestamp[i]) >= SENSOR_ERROR_DURATION) {
                    Data->SensorErrTimestamp[i] = 0;
                }
            }
        }
    }
    //dbgPrint("\n");

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Checks if the pressure enters or leaves the required range
 *
 *      This function checks if the actual pressure curve meassured by the
 *      functional module enters or leaves the desired pressure range set
 *      by the master. If this is the case, a notification is sent to the
 *      master. There are separate messages for entering and leaving the
 *      pressure range.
 *
 *  \xparam  Data = Data of one instance of the functional module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressNotifRange (InstanceData_t *Data)
{
    CanMessage_t Message;

    // Pressure curve leaves desired range
    if (Data->ServicePress[0] + Data->TolerancePress < Data->DesiredPress ||
        Data->ServicePress[0] > Data->DesiredPress + Data->TolerancePress) {
        if ((Data->Flags & MODE_PRESS_RANGE) != 0) {
            Data->Flags &= ~(MODE_PRESS_RANGE);
            Message.CanID = MSG_PRESS_NOTI_OUT_OF_RANGE;
            Message.Length = 2;
            bmSetMessageItem (&Message, Data->ServicePress[0]/10, 0, 2);
            dbgPrint("Pressure out of range\n");
            return (canWriteMessage(Data->Channel, &Message));
        }
    }
    // Pressure curve enters desired range
    else {
        if ((Data->Flags & MODE_PRESS_RANGE) == 0) {
            Data->Flags |= MODE_PRESS_RANGE;
            Message.CanID = MSG_PRESS_NOTI_IN_RANGE;
            Message.Length = 2;
            bmSetMessageItem (&Message, Data->ServicePress[0]/10, 0, 2);
            dbgPrint("Pressure in range\n");
            return (canWriteMessage(Data->Channel, &Message));
        }
    }

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Executes the PID algorithm
 *
 *      This function executes the PID algorithm that is needed for
 *      pressure control. It also performs auto tuning and is aware of the
 *      different behavior in pumping and hold phase. It also does cascaded
 *      control if required.
 *
 *  \xparam Data = Data of one instance of the functional module
 *  \iparam Instance = Number of the instance calling the function
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressRegulation (InstanceData_t *Data, UInt16 Instance)
{
    UInt8 i;
    UInt8 j;
    UInt8 First;
    Error_t Error;
    Int32 DevPress = 0;
    CanMessage_t Message;
    Int32 PidOutput = 0;
    UInt8 Direction = ((PressPumpMode == MODE_PRESSURE)?0:1);
#ifdef ASB15_VER_B    
    Int32 ActuatingValuePWM = 0;
#endif

    // PID and auto tuning algorithm
    if ((Data->Flags & MODE_AUTO_TUNE) != 0) {
        First = Data->AutoTunePidNumber;
        pressPidAutoProgress (&Data->PidParams[First], Data->DesiredPress, Data->ServicePress[First]);
    }
    else {
        First = 0;
        if (pressPidCalculate (&Data->PidParams[First], Data->DesiredPress, Data->ServicePress[First], Direction) == FALSE) {
            return (E_PRESS_PID_NOT_CONFIGURED);
        }
    }

    // Cascaded PID controller
    for (i = First + 1; i < Data->NumberPid; i++) {
        PidOutput = pressPidGetOutput (&Data->PidParams[i-1]);
        if (PidOutput < 0) {
            PidOutput = 0;
        }
        if (pressPidCalculate (&Data->PidParams[i], pressPidGetOutput (&Data->PidParams[i-1]), Data->ServicePress[i], Direction) == FALSE) {
            return (E_PRESS_PID_NOT_CONFIGURED);
        }
    }
    Data->ActuatingValue = pressPidGetOutput (&Data->PidParams[Data->NumberPid - 1]);
    if (Data->ActuatingValue < 0) {
        Data->ActuatingValue = 0;
    }
    
#ifdef ASB15_VER_B    
    if ( !pressPwmParamsOk (Data->PwmParams) ) {
        return (E_PRESS_PWM_NOT_CONFIGURED);
    }
        
    ActuatingValuePWM = Data->ActuatingValue;
    if (ActuatingValuePWM < Data->PwmParams->MinActuatingValue) {
        ActuatingValuePWM = Data->PwmParams->MinActuatingValue;
    }    
    else if (ActuatingValuePWM > Data->PwmParams->MaxActuatingValue) {
        ActuatingValuePWM = Data->PwmParams->MaxActuatingValue;
    }
    
    Data->ActuatingPwmWidth = (Data->PwmParams->PwmCoeff1*ActuatingValuePWM + Data->PwmParams->PwmCoeff2)/10000;
    if(Data->ActuatingPwmWidth > 100) {
        Data->ActuatingPwmWidth = 100;
    }
#endif    

    // Priority computation for hold phase
    for (i = 0; i < InstanceCount; i++) {
        DataTable[i].Priority = 0;
        DevPress = (Int16) DataTable[i].DesiredPress - DataTable[i].ServicePress[0];
        for (j = 0; j < InstanceCount; j++) {
            if(DataTable[j].ModuleState == MODULE_STATE_READY && (DataTable[j].Flags & MODE_MODULE_ENABLE) != 0) {
                if (DataTable[j].DesiredPress > DataTable[j].ServicePress[0] + DevPress) {
                    DataTable[i].Priority++;
                }
                else if (DataTable[j].DesiredPress == DataTable[j].ServicePress[0] + DevPress && i > j) {
                    DataTable[i].Priority++;
                }
            }
        }
    }

    // Stop auto tuning
    if ((Data->Flags & MODE_AUTO_TUNE) != 0) {
        if (bmTimeExpired(Data->AutoTuneStartTime) >= Data->AutoTuneDuration) {
            pressPidAutoStop (&Data->PidParams[Data->AutoTunePidNumber]);
            if (Data->AutoTunePidNumber == 0) {
                Data->Flags &= ~(MODE_MODULE_ENABLE | MODE_AUTO_TUNE);
                Message.CanID = MSG_PRESS_NOTI_AUTO_TUNE;
                Message.Length = 0;
                Error = canWriteMessage(Data->Channel, &Message);
                if (Error != NO_ERROR) {
                    return (Error);
                }
            }
            else {
                Data->AutoTunePidNumber--;
                pressPidAutoStart (&Data->PidParams[Data->AutoTunePidNumber]);
            }
        }
    }

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Sets the pressure related parameters of the module
 *
 *      This function is called by the CAN message dispatcher when a
 *      pressure set message is received from the master. The parameters
 *      in the message are transfered to the data structure of the addressed
 *      module instance. The modified settings influence the behavior of the
 *      module task. The following settings will be modified:
 *
 *      - Regulation active bit (1 = on, 0 = off)
 *      - Start auto tuning bit
 *      - Regulation phase bit (1 = hold, 0 = pumping)
 *      - Desired pressure (in KPa)
 *      - Pressure tolerance (in KPa)
 *      - Sampling time (in miliseconds)
 *      - Auto tuning duration (in seconds)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressSetPressure (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 i;
    Bool Running;
    Error_t Status = NO_ERROR;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length != 8) {
        return (E_MISSING_PARAMETERS);
    }

    // Pressure control in idle mode only
    if (Data->ModuleState != MODULE_STATE_READY) {
        return (E_PRESS_MODULE_INACTIVE);
    }

    Running = ((Data->Flags & MODE_MODULE_ENABLE) != 0);

    // Start parameter evaluation
    Data->Flags = bmGetMessageItem(Message, 0, 1);
    Data->DesiredPress = (Int32)((Int16)bmGetMessageItem(Message, 1, 2)*10);
    Data->TolerancePress = bmGetMessageItem(Message, 3, 1)*1000;
    //PressSamplingTime = bmGetMessageItem(Message, 3, 2) * 10;
#ifdef ASB15_VER_A
    PressSamplingTime = bmGetMessageItem(Message, 4, 2) * 2;
#endif
#ifdef ASB15_VER_B    
    PressSamplingTime = bmGetMessageItem(Message, 4, 2);
#endif
    Data->AutoTuneDuration = bmGetMessageItem(Message, 6, 2) * 1000;

#ifdef ASB15_VER_A    
    // Set the sampling time
    for (i = 0; i < Data->NumberPid; i++) {
        Data->PidParams[i].Ts = bmGetMessageItem(Message, 4, 2);
    }
#endif

#ifdef ASB15_VER_B    
    // Set the sampling time
    for (i = 0; i < Data->NumberPid; i++) {
        Data->PidParams[i].Ts = bmGetMessageItem(Message, 4, 2)/10;
    }
#endif
        
    dbgPrint("Sample Time:%d\n", PressSamplingTime);

    // Set operating phase
    if((Data->Flags & MODE_PRESS_PHASE) != 0) {
        PressPhase = PHASE_HOLD;
    }
    else {
        PressPhase = PHASE_PUMP;
    }
    
    if((Data->Flags & MODE_PRESS_MODE) != 0) {
        PressPumpMode = MODE_VACUUM;
    }
    else {
        PressPumpMode = MODE_PRESSURE;
    }
    
    if((Data->Flags & MODE_PRESS_ACTUATE) != 0) {
        PressPumpActuateMode = ACTUATE_PWM;
    }
    else {
        PressPumpActuateMode = ACTUATE_ONOFF;
    }    
    
    // Start auto-tuning
    if ((Data->Flags & MODE_AUTO_TUNE) != 0) {
        Data->Flags |= MODE_AUTO_TUNE;
        Data->AutoTuneStartTime = bmGetTime ();
        Data->AutoTunePidNumber = Data->NumberPid - 1;
        Data->ActuatingValue = 0;
        pressPidAutoStart (&Data->PidParams[Data->AutoTunePidNumber]);
    }

    // If the module was or is deactivated, reset some parameters
    if (Running == FALSE || (Data->Flags & MODE_MODULE_ENABLE) == 0) {
        for (i = 0; i < Data->NumberPid; i++) {
            pressPidReset(&Data->PidParams[i]);
        }
        pressPumpReset();
        Data->State = STATE_IDLE;

        PressChanged = TRUE;
        PumpCheckSkipped = FALSE;
    }
    
#ifdef ASB15_VER_B    
    // Enable/Disable 24V power supply of pump
    pressPumpEnablePower( ( (Data->Flags & MODE_MODULE_ENABLE) == 0 )?0:1, bmGetInstance(Channel) );
#endif    
    
    return (Status);



}


/*****************************************************************************/
/*!
 *  \brief  Sets current watchdog parameters
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting pump current watchdog parameters is received from the master.
 *      The parameters in the message are transfered to the data structure of 
 *      the addressed module instance. The modified settings influence the 
 *      behavior of the module task. The following settings will be modified:
 *
 *      - Current sensor gain factor (mA/V)
 *      - Desired pump current (in milliampere)
 *      - Pump current threshold (in milliampere)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressSetCurrentWatchdog (UInt16 Channel, CanMessage_t* Message)
{
    if (Message->Length == 6) {
        pressPumpParams->CurrentGain = bmGetMessageItem (Message, 0, 2);
        pressPumpParams->DesiredCurrent = bmGetMessageItem (Message, 2, 2);
        pressPumpParams->DesiredCurThreshold = bmGetMessageItem (Message, 4, 2);
        dbgPrint("Pump current watchdog:%d %d\n", pressPumpParams->DesiredCurrent, pressPumpParams->DesiredCurThreshold);
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Sets the fan speed watchdog parameters (Obsolete)
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting the fan speed watchdog parameters is received from the master.
 *      The parameters in the message are transfered to the data structure of
 *      the addressed module instance. The modified settings influence the
 *      behavior of the module task. The following settings will be modified:
 *
 *      - Desired fan speed (in RPM)
 *      - Fan speed threshold (in RPM)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressSetFanWatchdog (UInt16 Channel, CanMessage_t* Message)
{
    if (Message->Length == 6) {
        pressFanParams->CurrentGain = bmGetMessageItem (Message, 0, 2);
        pressFanParams->DesiredCurrent = bmGetMessageItem (Message, 2, 2);
        pressFanParams->DesiredCurThreshold = bmGetMessageItem (Message, 4, 2);
        dbgPrint("Fan current watchdog:%d %d\n", pressFanParams->DesiredCurrent, pressFanParams->DesiredCurThreshold);
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);    
}


/*****************************************************************************/
/*!
 *  \brief  Sets the gain and time parameters of a PID controller
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting the gain and time parameters of the PID controller is received
 *      from the master. The parameters in the message are transfered to the
 *      data structure of the addressed module instance. The modified settings
 *      influence the behavior of the module task. The following settings will
 *      be modified:
 *
 *      - Maximal pressure permitted to the PID (in KPa)
 *      - Minimal pressure permitted to the PID (in KPa) 
 *      - PID controller gain parameter
 *      - PID controller reset time in hundredth of a second
 *      - PID controller derivative time in hundredth of a second
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressSetPidParameters (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 8) {
        Number = bmGetMessageItem(Message, 0, 1);
        if (Number >= Data->NumberPid) {
            return (E_PARAMETER_OUT_OF_RANGE);
        }
        Data->PidParams[Number].Ready = TRUE;
        Data->PidParams[Number].MaxPress = bmGetMessageItem(Message, 1, 1) * 1000;
        Data->PidParams[Number].MinPress = (Int32)((Int8)bmGetMessageItem(Message, 2, 1) * 1000);
        Data->PidParams[Number].Kc = bmGetMessageItem(Message, 3, 2);  // Caution: Error
        Data->PidParams[Number].Ti = bmGetMessageItem(Message, 5, 2);
        Data->PidParams[Number].Td = bmGetMessageItem(Message, 7, 1);

#if 1
        dbgPrint("PID: %d %d %d %d %d\n", 
        Data->PidParams[Number].MaxPress,
        Data->PidParams[Number].MinPress,
        Data->PidParams[Number].Kc,
        Data->PidParams[Number].Ti,
        Data->PidParams[Number].Td);
#endif

        pressPidReset(&Data->PidParams[Number]);
        
        if (Number == Data->NumberPid - 1) {
            Data->PidParams[Number].Range = MAX_INT32;
        }
        if (Number != 0) {
            Data->PidParams[Number-1].Range = Data->PidParams[Number].MaxPress;
        }
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Sets parameters for PWM output
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting the PWM parameters is received from the master. The parameters 
 *      in the message are transfered to the data structure of the addressed 
 *      module instance. The following settings will be modified:
 *
 *      - Maximal actuating value of PID output
 *      - Minimal actuating value of PID output
 *      - Maximal PWM duty mapping to the maximal actuating value
 *      - Minimal PWM duty mapping to the minimal actuating value
 *      - PWM coefficient 1
 *      - PWM coefficient 2
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
#ifdef ASB15_VER_B
static Error_t pressSetPwmParameters (UInt16 Channel, CanMessage_t* Message)
{
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 6) {

        Data->PwmParams->Ready = TRUE;
        Data->PwmParams->MaxActuatingValue = bmGetMessageItem(Message, 0, 2);
        Data->PwmParams->MinActuatingValue = bmGetMessageItem(Message, 2, 2);
        Data->PwmParams->MaxPwmDuty = bmGetMessageItem(Message, 4, 1);
        Data->PwmParams->MinPwmDuty = bmGetMessageItem(Message, 5, 1);
        
        Data->PwmParams->PwmCoeff1 = (Int32)((Data->PwmParams->MaxPwmDuty - Data->PwmParams->MinPwmDuty)*10000.0/(Data->PwmParams->MaxActuatingValue - Data->PwmParams->MinActuatingValue) + 0.5);
        Data->PwmParams->PwmCoeff2 = (Int32)(Data->PwmParams->MinPwmDuty*10000.0 - Data->PwmParams->PwmCoeff1*Data->PwmParams->MinActuatingValue + 0.5);       
        
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}
#endif


/*****************************************************************************/
/*!
 *  \brief  Reset the operating timers of the module
 *
 *      This function is called by the CAN message dispatcher when a message
 *      resetting the operating timers of the pumping elements is received
 *      from the master. The parameters in the message are transfered to the
 *      data structure of the addressed module instance. The modified settings
 *      influence the behavior of the module task. The following settings will
 *      be modified:
 *
 *      - Number of the pumping element
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressSetPumpTime (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    Error_t Error;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        Number = bmGetMessageItem(Message, 0, 1);
        if (Number < Data->NumberPumps) {
            Error = pressTimeReset(&Data->TimeParams, Number);
            if(Error < 0) {
                return (Error);
            }
            return (NO_ERROR);
        }
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (E_MISSING_PARAMETERS);
}



/*****************************************************************************/
/*!
 *  \brief  Set the valve status of the module
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting valve of the pumping elements is received from the master.
 *      It turns on/off corresponding valve according to the parameter in the  
 *      received message.
 *
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressSetValve (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    UInt8 ValveStatus;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 2) {

        Number = bmGetMessageItem(Message, 0, 1);
        if (Number >= Data->NumberValves) {            
            return (E_PARAMETER_OUT_OF_RANGE);
        }

        ValveStatus = bmGetMessageItem(Message, 1, 1);
        if (ValveStatus > 1) {
            return (E_PARAMETER_OUT_OF_RANGE);
        }

        return halPortWrite(Data->HandleValve[Number], ValveStatus);
                
    }
    return (E_MISSING_PARAMETERS);
}



/*****************************************************************************/
/*!
 *  \brief  Set the fan status of the module
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting fan of the pumping elements is received from the master.
 *      It turns on/off corresponding fan according to the parameter in the  
 *      received message.
 *
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressSetFan (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 FanStatus;
    UInt16 Instance;

    Instance = bmGetInstance(Channel);
    
    if (Message->Length == 1) {

        FanStatus = bmGetMessageItem(Message, 0, 1);
        if (FanStatus > 1) {
            return (E_PARAMETER_OUT_OF_RANGE);
        }

        if (FanStatus == 1) {
            FanChanged = TRUE;
            FanCheckSkipped = FALSE;
        }

        return pressFanControl(FanStatus, Instance);
                
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Set/Reset the atm. pressure for calculation of pressure sensor
 *
 *      This function is called by the CAN message dispatcher when a message
 *      resetting the atm. pressure of the pumping elements is received
 *      from the master. It then sets atm. pressure to current pressure or 
 *      resets it to zero according to the parameter in the received message.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
static Error_t pressSetCalibration (UInt16 Channel, CanMessage_t* Message)
{
    Bool AtmCalibration;


    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        AtmCalibration = (bmGetMessageItem(Message, 0, 1) != 0);
        if (AtmCalibration) {
            Data->AtmPress = Data->ServicePress[0];            
        }
        else {
            Data->AtmPress = 0;
        }
        dbgPrint("Set Atm. pressure to %d\n", Data->AtmPress);
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}

/*****************************************************************************/
/*!
 *  \brief   Requests the pressure and the state vector of the module
 *
 *      This function is called by the CAN message dispatcher when a
 *      pressure get message is received from the master. As a response a
 *      pressure message is sent back to the master. The following settings
 *      will be responded:
 *
 *      - Regulation active bit (1 = on, 0 = off)
 *      - Regulation phase bit (1 = hold, 0 = pumping)
 *      - Start auto tuning bit
 *      - Desired pressure (in 0.1 KPa)
 *      - Pressure tolerance (in 0.1 KPa)
 *      - Sampling time (in milliseconds)
 *      - Auto tuning duration (in seconds)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressGetPressure (UInt16 Channel, CanMessage_t* Message)
{
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (PressPhase == PHASE_HOLD) {
        Data->Flags |= MODE_PRESS_PHASE;
    }
    else {
        Data->Flags &= ~(MODE_PRESS_PHASE);
    }

    RespMessage.CanID = MSG_PRESS_RESP_PRESSURE;
    bmSetMessageItem (&RespMessage, Data->Flags, 0, 1);
    bmSetMessageItem (&RespMessage, Data->DesiredPress / 1000, 1, 1);
    bmSetMessageItem (&RespMessage, Data->TolerancePress / 1000, 2, 1);
    bmSetMessageItem (&RespMessage, PressSamplingTime, 3, 2);
    bmSetMessageItem (&RespMessage, Data->AutoTuneDuration / 1000, 5, 2);
    RespMessage.Length = 7;

    return (canWriteMessage(Data->Channel, &RespMessage));
}


/*****************************************************************************/
/*!
 *  \brief   Requests the gain and time parameters of a PID controller
 *
 *      This function is called by the CAN message dispatcher when the gain
 *      and time parameters of a PID controller are requested by the master.
 *      As a response the PID gain and time parameters are sent back to the
 *      master. The following settings will be responded:
 *
 *      - Maximal pressure permitted (in KPa)
 *      - Minimal pressure permitted (in KPa) 
 *      - PID controller gain parameter
 *      - PID controller reset time in hundredth of a second
 *      - PID controller derivative time in hundredth of a second
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressGetPidParameters (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        Number = bmGetMessageItem(Message, 0, 1);
        if(Number < Data->NumberPid) {
            RespMessage.CanID = MSG_PRESS_RESP_PID_PARAMS;
            bmSetMessageItem (&RespMessage, Number, 0, 1);
            bmSetMessageItem (&RespMessage, Data->PidParams[Number].MaxPress / 1000, 1, 1);
            bmSetMessageItem (&RespMessage, ((Int8)(Data->PidParams[Number].MinPress / 1000)), 2, 1);
            bmSetMessageItem (&RespMessage, Data->PidParams[Number].Kc, 3, 2);
            bmSetMessageItem (&RespMessage, Data->PidParams[Number].Ti, 5, 2);
            bmSetMessageItem (&RespMessage, Data->PidParams[Number].Td, 7, 1);
            RespMessage.Length = 8;
            return (canWriteMessage(Data->Channel, &RespMessage));
        }
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Requests the operating time of a single pump
 *
 *      This function is called by the CAN message dispatcher when the
 *      operating of a pumping element is requested by the master. The number
 *      of the pumping element is sent in the first byte of the rquest
 *      message. As a response the operating time is sent back to the master.
 *      The following settings will be responded:
 *
 *      - Number of the pumping element
 *      - Operating time of a pumping element in seconds
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressGetPumpTime (UInt16 Channel, CanMessage_t* Message)
{
    UInt32 Time;
    UInt8 Number;
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        Number = bmGetMessageItem(Message, 0, 1);
        if(Number < Data->NumberPumps) {
            Time = pressTimeRead(&Data->TimeParams, Number);
            RespMessage.CanID = MSG_PRESS_RESP_PUMP_TIME;
            bmSetMessageItem (&RespMessage, Number, 0, 1);
            bmSetMessageItem (&RespMessage, Time, 1, 4);
            RespMessage.Length = 5;
            return (canWriteMessage(Data->Channel, &RespMessage));
        }
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Requests the pressure measured by a single sensor
 *
 *      This function is called by the CAN message dispatcher when the
 *      pressure measured by a single sensor is requested by the master.
 *      The following settings will be responded:
 *
 *      - Number of the pressure sensor
 *      - Pressure from the sensor (in hundredth of KPa)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressGetServiceSensor (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        Number = bmGetMessageItem(Message, 0, 1);
        if(Number < Data->NumberSensors) {
            RespMessage.CanID = MSG_PRESS_RESP_SERVICE_SENSOR;
            bmSetMessageItem (&RespMessage, Number, 0, 1);
            bmSetMessageItem (&RespMessage, Data->ServicePress[Number]/10, 1, 2);
            RespMessage.Length = 3;
            return (canWriteMessage(Data->Channel, &RespMessage));
        }
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Requests the current through the pumping elements
 *
 *      This function is called by the CAN message dispatcher when the current
 *      through the pumping elements is requested by the master. The following
 *      settings will be responded:
 *
 *      - Current at the pumping elements (in milliampere)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressGetHardware (UInt16 Channel, CanMessage_t* Message)
{
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    RespMessage.CanID = MSG_PRESS_RESP_HARDWARE;
    bmSetMessageItem (&RespMessage, Data->NumberSensors, 0, 1);
    bmSetMessageItem (&RespMessage, Data->NumberPumps, 1, 1);
    bmSetMessageItem (&RespMessage, Data->NumberPid, 2, 1);
    bmSetMessageItem (&RespMessage, pressPumpCurrent(), 3, 2);
    RespMessage.Length = 5;

    return (canWriteMessage(Data->Channel, &RespMessage));
}


/*****************************************************************************/
/*!
 *  \brief   Reads the board options and stores them
 *
 *  This functions reads the 32 bit data word that contains the so-called
 *  board options. This data word contains information about the diffrent
 *  peripherals connected to the hardware, e.g. the number and the type of
 *  pressure sensors.
 *
 *  \oparam  Data = Instance Data
 *  \iparam  ModuleID = ID of this module
 *  \iparam  Instance = Module instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressReadOptions (InstanceData_t *Data, UInt16 ModuleID, UInt16 Instance)
{

#if 1
    UInt32 Options;
       
    Options = bmGetBoardOptions (ModuleID, Instance, 0);
    if (Options == 0) {
        return (E_BOARD_OPTION_MISSING);
    }

    Data->NumberSensors = Options & 0xf;
    Data->NumberPumps = (Options >> 4) & 0xf;
    Data->NumberPid = (Options >> 8) & 0xf;
    Data->NumberValves = (Options >> 12) & 0xf;
    

#endif

    
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Dynamic allocation of data structures
 *
 *  This functions allocates the different data structures that are needed by
 *  the various peripherals connected to the function module.
 *
 *  \oparam  Data = Instance Data
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressDeviceAlloc (InstanceData_t *Data)
{
    if (Data->NumberSensors > 0) {
        Data->HandlePress = calloc (Data->NumberSensors, sizeof(Handle_t));
        if (NULL == Data->HandlePress) {
            return (E_MEMORY_FULL);
        }
        Data->ServicePress = calloc (Data->NumberSensors, sizeof(UInt16));
        if (NULL == Data->ServicePress) {
            return (E_MEMORY_FULL);
        }
        Data->SensorErrTimestamp = calloc (Data->NumberSensors, sizeof(UInt32));
        if (NULL == Data->SensorErrTimestamp) {
            return (E_MEMORY_FULL);
        }
    }
    if (Data->NumberPid > 0) {
        Data->PidParams = calloc (Data->NumberPid, sizeof(PressPidParams_t));
        if (NULL == Data->PidParams) {
            return (E_MEMORY_FULL);
        }
 
 #ifdef ASB15_VER_B       
        Data->PwmParams = calloc (1, sizeof(PressPwmParams_t));
        if (NULL == Data->PwmParams) {
            return (E_MEMORY_FULL);
        }
#endif        
    }
    if (Data->NumberValves > 0) {
        Data->HandleValve = calloc (Data->NumberValves, sizeof(Handle_t));
        if (NULL == Data->HandleValve) {
            return (E_MEMORY_FULL);
        }
        Data->ServiceValve = calloc (Data->NumberValves, sizeof(UInt16));
        if (NULL == Data->NumberValves) {
            return (E_MEMORY_FULL);
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
 *  \xparam  Data = Instance Data
 *  \iparam  Instance = Module instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t pressHandleOpen (InstanceData_t *Data, UInt16 Instance)
{
    UInt16 i;
    UInt16 j;
    //Error_t Error;
    
    i = Instance;

    // Open pressure sensors
    for (j = 0; j < Data->NumberSensors; j++) {
        Data->HandlePress[j] = halAnalogOpen (HAL_PRESS_SENSOR + Data->NumberSensors * i + j, HAL_OPEN_READ, 0, NULL);
        if (Data->HandlePress[j] < 0) {
            return (Data->HandlePress[j]);
        }
    }

    // Open valve contro ports
    for (j = 0; j < Data->NumberValves; j++) {
        Data->HandleValve[j] = halPortOpen (HAL_PRESS_CTRLVALVE + Data->NumberValves * i + j, HAL_OPEN_RW);
        if (Data->HandleValve[j] < 0) {
            return (Data->HandleValve[j]);
        }
    }
    
    return (NO_ERROR);
}


#ifdef ASB15_VER_B
Bool pressPwmParamsOk (const PressPwmParams_t *Param)
{
    if (Param->MinActuatingValue >= Param->MaxActuatingValue) {
        return FALSE;
    }
    return Param->Ready;
}
#endif

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

Error_t pressInitializeModule (UInt16 ModuleID, UInt16 Instances)
{
    static bmCallbackEntry_t Commands[] = {
        { MSG_PRESS_SET_PRESSURE,        pressSetPressure },
        { MSG_PRESS_SET_CURRENT_WATCHDOG,pressSetCurrentWatchdog },
        { MSG_PRESS_SET_FAN_WATCHDOG,    pressSetFanWatchdog },
        { MSG_PRESS_SET_PID_PARAMS,      pressSetPidParameters },
        { MSG_PRESS_SET_PUMP_TIME,       pressSetPumpTime },
        { MSG_PRESS_REQ_PRESSURE,        pressGetPressure },
        { MSG_PRESS_REQ_PID_PARAMS,      pressGetPidParameters },
        { MSG_PRESS_REQ_PUMP_TIME,       pressGetPumpTime },
        { MSG_PRESS_REQ_SERVICE_SENSOR,  pressGetServiceSensor },
        { MSG_PRESS_REQ_HARDWARE,        pressGetHardware },
        { MSG_PRESS_SET_VALVE,           pressSetValve },
        { MSG_PRESS_SET_CALIBRATION,     pressSetCalibration },
        { MSG_PRESS_SET_FAN,             pressSetFan },
#ifdef ASB15_VER_B        
        { MSG_PRESS_SET_PWM_PARAMS,      pressSetPwmParameters }
#endif        
    };

    static bmModuleInterface_t Interface = {
        pressModuleTask,
        pressModuleControl,
        pressModuleStatus
    };
    Error_t Status;
    UInt16 i;
    UInt16 j;
#if 0    
    UInt16 Channel;
    InstanceData_t* Data;
#endif

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
    if (Status < NO_ERROR) {
        return (Status);
    }
    #ifdef DEBUG
    pressRegisterDebugNames (ModuleID);
    #endif

    // Initialize fan current measurements
    Status = pressFanInit (&pressFanParams, HAL_PRESS_FANCURRENT, HAL_PRESS_FANCONTROL, Instances);
    if (Status < NO_ERROR) {
        return (Status);
    }
    
    //tempPumpInit (TempPumpParams_t **Params, Device_t CurrentChannel, Device_t SwitchChannel, Device_t ControlChannel, UInt16 Instances)
    // Initialize pump control handling
#ifdef ASB15_VER_A    
    Status = pressPumpInit (&pressPumpParams, HAL_PRESS_CURRENT, HAL_PRESS_MAINVOLTAGE, HAL_PRESS_CTRLPUMPING, Instances);
#endif

#ifdef ASB15_VER_B
    Status = pressPumpInit (&pressPumpParams, HAL_PRESS_CURRENT, HAL_PRESS_CTRLPUMPING, HAL_PRESS_PWM_CTRL, Instances);
#endif
    
    if (Status < NO_ERROR) {
        return Status;
    }
    // open channels and ports for all modules
    for (i=0; i < Instances; i++) {

        DataTable[i].Channel = bmGetChannel(bmGetTaskID(ModuleID, i));
        DataTable[i].Flags = 0;

        // Get the number of peripheral elements connected to the board
        Status = pressReadOptions (&DataTable[i], ModuleID, i);
        if (Status < NO_ERROR) {
            return (Status);
        }

        // Allocating device resources in respect to the number of peripherals
        Status = pressDeviceAlloc (&DataTable[i]);
        if (Status < NO_ERROR) {
            return (Status);
        }

        // Opening the handles of the peripherals
        Status = pressHandleOpen (&DataTable[i], i);
        if (Status < NO_ERROR) {
            return (Status);
        }

        // Init PID parameters
        for (j = 0; j < DataTable[i].NumberPid; j++) {
            pressPidReset (&DataTable[i].PidParams[j]);
            DataTable[i].PidParams[j].Ready = FALSE;
        };
       
#ifdef ASB15_VER_B        
        // Init PWM parameters
        DataTable[i].PwmParams->Ready = FALSE;
#endif            
        
        Status = pressTimeInit (ModuleID, i, &DataTable[i].TimeParams, DataTable[i].NumberPumps);
        if (Status < NO_ERROR) {
            return Status;
        }
        
        // Set default Atm. pressure
        DataTable[i].AtmPress = 0;
        
        // Init time stamps for sensor error detection
        for (j = 0; j < DataTable[i].NumberSensors; j++) {
            DataTable[i].SensorErrTimestamp[j] = 0;
        };

        // Start time recording
        PressSampleTimestamp = bmGetTime();
        DataTable[i].State = STATE_IDLE;
        DataTable[i].ModuleState = MODULE_STATE_READY;
    }
    
    InstanceCount = Instances;
    ModuleIdentifier = ModuleID;

// Only for debug use only
#if 0

    Channel = 1;
    Data = &DataTable[bmGetInstance(Channel)];

    ///////////////////////////////////////////////////////////
    // Set PID parameters
    Data = &DataTable[bmGetInstance(Channel)];
    Data->PidParams[0].Ready = TRUE;
    Data->PidParams[0].MaxPress = 100*1000;
    Data->PidParams[0].MinPress = -100*1000;
//    Data->PidParams[0].Kc = 50;
//    Data->PidParams[0].Ti = 2*60*100;
//    Data->PidParams[0].Td = 0;
    Data->PidParams[0].Kc = 30;
    Data->PidParams[0].Ti = 50000;
    //Data->PidParams[0].Ti = 0;
    Data->PidParams[0].Td = 0;
    pressPidReset(&Data->PidParams[0]);
    
    //if (0 == Data->NumberPid - 1) {
        Data->PidParams[0].Range = MAX_INT32;
    //}

    //////////////////////////////////////////////////////////
    // Current watchdog
    pressPumpParams->CurrentGain = 1241;
    pressPumpParams->DesiredCurrent = 600;
    pressPumpParams->DesiredCurThreshold = 500;

    pressFanParams->CurrentGain = 1241;
    pressFanParams->DesiredCurrent = 600;
    pressFanParams->DesiredCurThreshold = 500;
    
    //////////////////////////////////////////////////////////
    // Set Pressure
     
    Data->Flags = 1;
    Data->DesiredPress = (-30)*1000;
    Data->TolerancePress = 3*1000;
    PressSamplingTime = 200;
    Data->AutoTuneDuration = 0;

    // Set the sampling time
    for (i = 0; i < Data->NumberPid; i++) {
        Data->PidParams[i].Ts = 20;
    }
    
    Data->PwmParams->MaxActuatingValue = 8000;
    Data->PwmParams->MinActuatingValue = 200;
    Data->PwmParams->MaxPwmDuty = 90;
    Data->PwmParams->MinPwmDuty = 50;
    
    Data->PwmParams->PwmCoeff1 = (Int32)((Data->PwmParams->MaxPwmDuty - Data->PwmParams->MinPwmDuty)*10000.0/(Data->PwmParams->MaxActuatingValue - Data->PwmParams->MinActuatingValue) + 0.5);
    Data->PwmParams->PwmCoeff2 = (Int32)(Data->PwmParams->MinPwmDuty*10000.0 - Data->PwmParams->PwmCoeff1*Data->PwmParams->MinActuatingValue + 0.5);
      

    //PressPumpMode = MODE_PRESSURE;
    PressPumpMode = MODE_VACUUM;
    
    PressPumpActuateMode = ACTUATE_PWM;

    for (i = 0; i < Data->NumberPid; i++) {
        pressPidReset(&Data->PidParams[i]);
    }
    pressPumpReset();
    Data->State = STATE_IDLE;
    
    Status = pressFanControl(TRUE, bmGetInstance(Channel));

    //halPortWrite(Data->HandleValve[1], 1);  //Vacuum
    halPortWrite(Data->HandleValve[0], 1);    //Pressure

#endif

    return (NO_ERROR);
}

//****************************************************************************/

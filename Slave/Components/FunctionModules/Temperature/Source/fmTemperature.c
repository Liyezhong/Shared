/****************************************************************************/
/*! \file fmTemperature.c
 *
 *  \brief Functional Module controlling temperature regulation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 29.06.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *      This module contains the top level source code of a temperature
 *      controller. The code in this file binds different subcomponents, that
 *      are generally needed for temperature regulation. The module controls
 *      various heaters and temperature indicators through the hardware
 *      abstraction layer. It furthermore watches ventilation fans, the
 *      lifetime of heating elements and sets parameters for temperature
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
#include "fmTemperature.h"
#include "fmTemperatureFan.h"
#include "fmTemperatureHeater.h"
#include "fmTemperaturePid.h"
#include "fmTemperatureSensor.h"
#include "fmTemperatureTime.h"

//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

/* Global defines of the temperature module */
#define MODULE_VERSION      0x0001  //!< Version number of module

/* Mode bits for the Flags member of the module instance data */
#define MODE_MODULE_ENABLE  0x0001  //!< The regulation is on or off
#define MODE_AUTO_TUNE      0x0002  //!< Auto tuning is in progress
#define MODE_TEMP_PHASE     0x0004  //!< 0: heating phase, 1: hold phase
#define MODE_VOLTAGE_RANGE  0x0008  //!< Voltage range detected: 0 is 200 to 240V, 1 is 100 to 127V
#define MODE_TEMP_RANGE     0x0010  //!< Actual temperature is in required range

/* Time constants for slope change detection */
#define SLOPE_DETECT_LONG_DURATION  5 //!< Longer duration in seconds for detection
#define SLOPE_DETECT_SHORT_DURATION 3 //!< Shorter duration in seconds for detection

/* Time constants for sensor error report */
#define SENSOR_ERROR_DURATION  3000   //!< Time duration in miliseconds to confirm sensor error

/* Default error value for temp. sensor out of range */
#define SENSOR_ERROR_VALUE     29900  //!< Default return value if sensor out of range 

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

/*! State of the module task */
typedef enum {
    STATE_IDLE,     //!< Wait for sampling time
    STATE_SAMPLE,   //!< Sample all inputs and calculate output
    STATE_CONTROL   //!< Write the control output
} TempTaskState_t;

/*! Current operating phase of the module (heating or hold) */
typedef enum {
    PHASE_HEAT, //!< Heating phase
    PHASE_HOLD  //!< Hold phase
} TempHeaterPhase_t;

/*! Contains all variables for a instance of this module */
typedef struct {
    bmModuleState_t ModuleState; //!< Module state
    Handle_t *HandleTemp;        //!< Handle to access analog input ports (HAL)
    Handle_t HandleCompensate;   //!< Handle to access analog input port (HAL)
    UInt16 Channel;              //!< Logical CAN channel
    UInt16 Flags;                //!< Mode control flag bits
    TempTaskState_t State;       //!< The module task state
    UInt8 Priority;              //!< Heating priority in hold phase (0 is highest)

    UInt16 DesiredTemp;          //!< Desired temperature (in 0.01 degree Celsius steps)
    UInt16 ToleranceTemp;        //!< Temperature tolerance (in 0.01 degree Celsius steps)

    UInt16* ServiceTemp;         //!< Temperature of a certain sensor in 0.01 degree Celsius steps     
    UInt16* ServiceFan;          //!< Speed of the fans in rotations per minute

    UInt16 DesiredFanSpeed;      //!< Desired speed for the fan watchdog in RPM
    UInt16 DesiredFanThreshold;  //!< Desired threshold for the fan watchdog in RPM

    UInt32 AutoTuneStartTime;    //!< Time in milliseconds auto tuning was started
    UInt32 AutoTuneDuration;     //!< Duration of the auto tuning algorithm in milliseconds
    UInt8 AutoTunePidNumber;     //!< Number of controller currently tuned
    Int16 ActuatingValue;        //!< Most recent actuating value for temperature regulation

    UInt8 NumberSensors;         //!< Number of independent temperature sensors
    UInt8 NumberHeaters;         //!< Number of heating elements
    UInt8 NumberFans;            //!< Number of ventilation fans to be watched
    UInt8 NumberPid;             //!< Number of cascaded PID controllers
    UInt8 IndexPidSensor;        //!< Index of sensor used for PID calculation.
                                 //!< 0xF: Sensor with lowest temperature is used for calculation

    UInt8 DetectSlope;           //!< 0: Disable, 1: Enable
    UInt8 SlopeTimeInterval;     //!< Time in milliseconds
    UInt16 SlopeTempChange;      //!< Temperature for slope detection(in 0.01 degree Celsius steps)
    UInt8 SlopeSampleCnt;        //!< Number of total temperature samples from the very beginning
    
    TempHeaterType_t HeaterType; //!< 0: AC, 1: DC      
    TempSensorType_t SensorType; //!< Type of temperature sensor used by this module
    TempTimeParams_t TimeParams; //!< Input and output parameters of the lifecycle counters
    TempPidParams_t *PidParams;  //!< Input and output parameters of the PID controller
    
    UInt16* TempArray;           //!< Array of temp. data for slope change detection
    UInt8   TempArraySize;       //!< Size of temp. array
    UInt8   TempArrayIndex;      //!< Current index of temp. array for writing
    UInt8   SamplesPerSec;       //!< Number of temperature samples per second
    UInt32* SensorErrTimestamp;  //!< Time stamp for trigeering sensor error reporting
    
} InstanceData_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

static UInt16 InstanceCount = 0;        //!< number of module instances
static UInt16 ModuleIdentifier = 0;     //!< module identifier

static InstanceData_t* DataTable;  //!< data table for all instances

/*! Current heating priority for hold mode */
static UInt8 TempPriority;
/*! Heat or hold phase */
static TempHeaterPhase_t TempPhase;
/*! Time in milliseconds the last temperature value was sampled */
static UInt32 TempSampleTimestamp;
/*! Sampling time of the module in milliseconds */
static UInt32 TempSamplingTime = 1000;

/*! Public parameters for the heating element control functionality */
static TempHeaterParams_t *tempHeaterParams;

/*! Total number of sensors used in all instances */
static UInt32 TempNumSensors = 0;

/*! Flag for indicating change of temperature setting */
static Bool TempChanged = FALSE;
/*! Flag for indicating if heater check skipped after temperature changed */
static Bool HeaterCheckSkipped = FALSE;
/*! Flag for AC heater auto switching between 110 and 220V */
static Bool ACHeaterAutoSwitch = TRUE;

//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t tempModuleControl (UInt16 Instance, bmModuleControlID_t ControlID);
static Error_t tempModuleStatus  (UInt16 Instance, bmModuleStatusID_t StatusID);
static Error_t tempModuleTask    (UInt16 Instance);

static Int8    tempFindRoot (void);
static Error_t tempShutDown (InstanceData_t *Data, Error_t Error, UInt16 Instance);
static Error_t tempFetchCheck  (InstanceData_t *Data, UInt16 Instance, Bool *Fail);
static Error_t tempNotifRange  (InstanceData_t *Data);
static Error_t tempRegulation  (InstanceData_t *Data, UInt16 Instance);
static Error_t tempReadOptions (InstanceData_t *Data, UInt16 ModuleID, UInt16 Instance);
static Error_t tempDeviceAlloc (InstanceData_t *Data);
static Error_t tempHandleOpen  (InstanceData_t *Data, UInt16 Instance);
static Error_t tempNotifySlope (InstanceData_t *Data, UInt8 TempChangeDir);

static Error_t tempSetTemperature         (UInt16 Channel, CanMessage_t* Message);
static Error_t tempSetFanWatchdog         (UInt16 Channel, CanMessage_t* Message);
static Error_t tempSetCurrentWatchdog     (UInt16 Channel, CanMessage_t* Message);
static Error_t tempSetPidParameters       (UInt16 Channel, CanMessage_t* Message);
static Error_t tempSetHeaterTime          (UInt16 Channel, CanMessage_t* Message);
static Error_t tempSetSwitchState         (UInt16 Channel, CanMessage_t* Message);
static Error_t tempSetAcCurrentWatchdog   (UInt16 Channel, CanMessage_t* Message);
static Error_t tempSetAcCurrentWatchdogExt(UInt16 Channel, CanMessage_t* Message);

static Error_t tempGetTemperature       (UInt16 Channel, CanMessage_t* Message);
static Error_t tempGetPidParameters     (UInt16 Channel, CanMessage_t* Message);
static Error_t tempGetHeaterTime        (UInt16 Channel, CanMessage_t* Message);
static Error_t tempGetServiceSensor     (UInt16 Channel, CanMessage_t* Message);
static Error_t tempGetServiceFan        (UInt16 Channel, CanMessage_t* Message);
static Error_t tempGetHardware          (UInt16 Channel, CanMessage_t* Message);

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

static Error_t tempModuleControl (UInt16 Instance, bmModuleControlID_t ControlID)
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
            Error = tempTimeResetPartition (&Data->TimeParams, Data->Channel);
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

static Error_t tempModuleStatus (UInt16 Instance, bmModuleStatusID_t StatusID)
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
            return (Data->ServiceTemp[0]);
        
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

static Error_t tempModuleTask (UInt16 Instance)
{
    Bool Fail = FALSE;
    UInt8 i;
    Error_t Error;
    Int16 TempDelta = 0;

    InstanceData_t* Data = &DataTable[Instance];

    if (DataTable[Instance].ModuleState == MODULE_STATE_COUNT) {
        return (Error_t)MODULE_STATE_COUNT;
    }

    if (Data->DetectSlope == 1 && Data->TempArray == NULL) {
        // Only set sampling array if temp. module has been configured.
        if ((Data->Flags & MODE_MODULE_ENABLE) != 0 && TempSamplingTime>0 ) {
            Data->TempArraySize = (SLOPE_DETECT_LONG_DURATION*1000)/TempSamplingTime;
            Data->SamplesPerSec = 1000/TempSamplingTime;
            dbgPrint("TempArraySize:%d, SamplesPerSec:%d\n", Data->TempArraySize, Data->SamplesPerSec);
            if ( Data->TempArraySize > 0 ) {
                Data->TempArray = calloc (Data->TempArraySize, sizeof(UInt16));
                if (NULL == Data->TempArray) {
                    return (E_MEMORY_FULL);
                }
            }
        }
    }
    
    if (Data->ModuleState == MODULE_STATE_READY) {


        Error = tempSampleCurrent(Data->HeaterType);  
        if (Error < 0) {
            return (tempShutDown (Data, Error, Instance));
        }


        if ( Instance == 0 ) {
            if (bmTimeExpired (TempSampleTimestamp) >= TempSamplingTime) {
                // Update the sampling time
                TempSampleTimestamp = bmGetTime();
                TempPriority = 0;
                for (i = 0; i < InstanceCount; i++) {
                    DataTable[i].State = STATE_SAMPLE;
                }
                
                if ( HeaterCheckSkipped && TempChanged ) {
                    TempChanged = FALSE;
                    HeaterCheckSkipped = FALSE;
                }
            }
        }

        
        if (Data->State == STATE_SAMPLE) {
        
            if ((Data->Flags & MODE_MODULE_ENABLE) == 0 ) {
                Data->State = STATE_CONTROL;
            }                       

            // Read and check sensors
            Error = tempFetchCheck(Data, Instance, &Fail);
            if (Error != NO_ERROR) {
                Data->State = STATE_IDLE;
                return (tempShutDown (Data, Error, Instance));
            }
            
            // If one of the subsystems fails, shutdown and quit
            if (Fail == TRUE) {
                Data->State = STATE_IDLE;
                Data->Flags &= ~(MODE_MODULE_ENABLE);
                return ((Error_t) Data->ModuleState);
            }
            
        }              
    
    }

    // Is heating active?
    if (Data->ModuleState == MODULE_STATE_READY && (Data->Flags & MODE_MODULE_ENABLE) != 0) {
        // Progress the current sensor evaluation
        Error = tempHeaterProgress (Data->HeaterType);
        if (Error < 0) {
            return (tempShutDown (Data, Error, Instance));
        }
        
        // Check if the sampling time expired already
        if (Data->State == STATE_SAMPLE) {
            Data->State = STATE_CONTROL;     
            
            // Check if the temperature is within the required range
            Error = tempNotifRange(Data);
            if (Error != NO_ERROR) {
                return (tempShutDown (Data, Error, Instance));
            }

            // Slope detection
            if (Data->DetectSlope == 1 && Data->TempArray != NULL) {
                
                Data->TempArray[Data->TempArrayIndex++] = Data->ServiceTemp[0];
                Data->TempArrayIndex %= Data->TempArraySize;                
                if (Data->SlopeSampleCnt < Data->TempArraySize) {
                    Data->SlopeSampleCnt++;
                }
                else {
                    //tempNotifySlope(Data, 1);
                    TempDelta = 
                    Data->ServiceTemp[0]- Data->TempArray[(Data->TempArrayIndex+Data->SamplesPerSec*(SLOPE_DETECT_LONG_DURATION-SLOPE_DETECT_SHORT_DURATION)) % Data->TempArraySize];
                    dbgPrint("D3: %d, T: %d\n", TempDelta, Data->ServiceTemp[0]);
                    if (TempDelta < (-Data->SlopeTempChange)) {
                        dbgPrint("Level up [3].\n");
                        tempNotifySlope(Data, 1);
                    }
                    else if (TempDelta > 1000) {
                        //dbgPrint("Level down.\n");
                        tempNotifySlope(Data, 0);
                    }
                    else {
                        TempDelta = Data->ServiceTemp[0]- Data->TempArray[Data->TempArrayIndex];
                        dbgPrint("D5: %d, T: %d\n", TempDelta, Data->ServiceTemp[0]);
                        if (TempDelta < (-Data->SlopeTempChange)) {
                            dbgPrint("Level up [5].\n");
                            tempNotifySlope(Data, 1);
                        }
                    }

                }
            }
            
            // Regulate temperature
            Error = tempRegulation(Data, Instance);
            if (Error != NO_ERROR) {
                return (tempShutDown (Data, Error, Instance));
            }
        }
        else if (Data->State == STATE_CONTROL) {
            if (TempPhase == PHASE_HEAT || (tempHeaterActive () == 0 && Data->Priority == TempPriority)) {
                UInt32 OperatingTime = (Data->ActuatingValue * TempSamplingTime) / MAX_INT16;
                
                Data->State = STATE_IDLE;
                TempPriority++;                
                
                // Control the heating elements
                Error = tempHeaterActuate (OperatingTime, TempSampleTimestamp + TempSamplingTime, Instance);
                if (Error != NO_ERROR) {
                    return (tempShutDown (Data, Error, Instance));
                }
                // Update the heater operating time
                Error = tempTimeUpdate (&Data->TimeParams, OperatingTime);
                if (Error != NO_ERROR) {
                    return (tempShutDown (Data, Error, Instance));
                }
            }
        }
    }
    else {
        if (tempFindRoot () < 0) {
            tempHeaterReset ();
        }
        Error = tempHeaterActuate (0, TempSampleTimestamp + TempSamplingTime, Instance);
        if (Error < NO_ERROR) {
            return (tempShutDown (Data, Error, Instance));
        }
        Error = tempFanControl(Instance, FALSE);
        if (Error < NO_ERROR) {
            return (tempShutDown (Data, Error, Instance));
        }
    }
    return ((Error_t) Data->ModuleState);
}


/*****************************************************************************/
/*!
 *  \brief  Find the root instance
 *
 *  This small method returns the ID of the so-called root instance. This is
 *  necessary for hold mode, when more than one heating circuit have to be
 *  controlled. The root instance is the first instance, that is currently
 *  active.
 *
 *  \return  ID of the root instance
 *
 ****************************************************************************/

static Int8 tempFindRoot (void)
{
    Int8 i;

    for (i = 0; i < InstanceCount; i++) {
        if (DataTable[i].ModuleState == MODULE_STATE_READY && (DataTable[i].Flags & MODE_MODULE_ENABLE) != 0) {
            return (i);
        }
    }
    return (-1);
}


/*****************************************************************************/
/*!
 *  \brief  Deactivates the module and sends an error message
 *
 *      This method deactivates an instance of the temperature fufnction
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

static Error_t tempShutDown (InstanceData_t *Data, Error_t Error, UInt16 Instance)
{
    UInt8 i;
    
    Data->Flags &= ~(MODE_MODULE_ENABLE);
    dbgPrint("Heater shutDown\n");
    bmSignalEvent (Data->Channel, Error, TRUE, 0);
    
    for(i = 0; i < Data->NumberPid; i++) {
        tempPidReset (&Data->PidParams[i]);
    }
    Error = tempHeaterActuate (0, TempSampleTimestamp + TempSamplingTime, Instance);
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
 *      including fan speeds, heater current, temperature sensors. It also
 *      checks if these values are in a valid range. If such a range is
 *      violated, an error message is sent over the CAN Bus.
 *
 *  \xparam  Data = Data of one instance of the functional module
 *  \iparam Instance = Number of the instance calling the function
 *  \oparam  Fail = Did one of the tests fail
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempFetchCheck (InstanceData_t *Data, UInt16 Instance, Bool *Fail)
{
    UInt8 i;
    UInt16 Compensation = 0;
    Error_t Error = NO_ERROR;
    static UInt16 CompPre = 2500;
 
 
    // Compute and check heater current      
    if ( Instance == 0 ) {
        tempCalcEffectiveCurrent(Instance, Data->HeaterType);
        //dbgPrint("Heater current[%d]:%d\n", Instance, tempHeaterCurrent());
    }
    if ((Data->Flags & MODE_MODULE_ENABLE) != 0 ) {

        if ( Instance == tempFindRoot () ) {
            if ( TempChanged ) {
                Error = tempHeaterCheck (Instance, Data->HeaterType, FALSE, ACHeaterAutoSwitch);
                HeaterCheckSkipped = TRUE;
            }
            else {
                Error = tempHeaterCheck (Instance, Data->HeaterType, TRUE, ACHeaterAutoSwitch);
            }
            if (Error < 0) {
                return Error;
            }
        }       

  
        if (tempHeaterFailed () == TRUE) {
            *Fail = TRUE;
            dbgPrint("I[Err]:%d ", Instance);
            bmSignalEvent (Data->Channel, E_TEMP_CURRENT_OUT_OF_RANGE, TRUE, tempHeaterCurrent ());
            bmSignalEvent (Data->Channel, E_TEMP_CURRENT_OUT_OF_RANGE, TRUE, tempGetActiveStatus ());
        }
  
    }


    // Read and check fan speeds
    for (i = 0; i < Data->NumberFans; i++) {
        Error = tempFanSpeed (Instance, i, &Data->ServiceFan[i]);
        if (Error < 0) {
            return Error;
        }
        else if (Error > 0) {
            if (Data->ServiceFan[i] + Data->DesiredFanThreshold < Data->DesiredFanSpeed ||
                    Data->ServiceFan[i] > Data->DesiredFanSpeed + Data->DesiredFanThreshold) {
                *Fail = TRUE;
                bmSignalEvent (Data->Channel, E_TEMP_FAN_OUT_OF_RANGE, TRUE, i);
            }
        }
    }
   

    // Read cold junction temperature sensor
    if (Data->SensorType == TYPEK || Data->SensorType == TYPET) {
        if ((Error = tempSensorRead (Data->HandleCompensate, PT1000, 0, &Compensation)) < 0) {
            //return (Error);
            Compensation = CompPre;
        }
        else {
            CompPre = Compensation;
        }
    }

    //printf("I:%d ", Instance);


    // Measure and check the temperature
    for (i = 0; i < Data->NumberSensors; i++) {
        if ((Error = tempSensorRead (Data->HandleTemp[i], Data->SensorType, Compensation, &Data->ServiceTemp[i])) < 0) {
            //printf("[%d]:E ", i);
            if ( Data->SensorErrTimestamp[i] == 0 ) {
                Data->SensorErrTimestamp[i] = bmGetTime();
            }
            else if (bmTimeExpired(Data->SensorErrTimestamp[i]) >= SENSOR_ERROR_DURATION) {
                Data->ServiceTemp[i] = SENSOR_ERROR_VALUE;
                if ((Data->Flags & MODE_MODULE_ENABLE) != 0 ) {
                    return (Error);
                }
            }
        }
        else {
            //printf("[%d]:%d ", i, Data->ServiceTemp[i]);
            
            if ( Data->SensorErrTimestamp[i] != 0 ) {
                if (bmTimeExpired(Data->SensorErrTimestamp[i]) >= SENSOR_ERROR_DURATION) {
                    Data->SensorErrTimestamp[i] = 0;
                }
            }
        }
    }
    
    //printf("\n");


#if 0
    // Compare all temperature sensors
    for (i = 0; i < (Data->NumberSensors / Data->NumberPid) - 1; i++) {
        for (j = i + 1; j < Data->NumberSensors; j++) {
            if (Data->ServiceTemp[i] + Data->ToleranceTemp < Data->ServiceTemp[j] ||
                    Data->ServiceTemp[i] > Data->ServiceTemp[j] + Data->ToleranceTemp) {
                 *Fail = TRUE;
                 bmSignalEvent (Data->Channel, E_TEMP_SENSORS_INCONSISTENT, TRUE, 0);
            }
        }
    }
#endif

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Checks if the temperature enters or leaves the required range
 *
 *      This function checks if the actual temperature curve meassured by the
 *      functional module enters or leaves the desired temperature range set
 *      by the master. If this is the case, a notification is sent to the
 *      master. There are separate messages for entering and leaving the
 *      temperature range.
 *
 *  \xparam  Data = Data of one instance of the functional module
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempNotifRange (InstanceData_t *Data)
{
    CanMessage_t Message;

    // Temperature curve leaves desired range
    if (Data->ServiceTemp[0] + Data->ToleranceTemp < Data->DesiredTemp ||
        Data->ServiceTemp[0] > Data->DesiredTemp + Data->ToleranceTemp) {
        if ((Data->Flags & MODE_TEMP_RANGE) != 0) {
            Data->Flags &= ~(MODE_TEMP_RANGE);
            Message.CanID = MSG_TEMP_NOTI_OUT_OF_RANGE;
            Message.Length = 2;
            bmSetMessageItem (&Message, Data->ServiceTemp[0], 0, 2);
            dbgPrint("Temperature out of range[%d]\n", Data->ServiceTemp[0]);
            return (canWriteMessage(Data->Channel, &Message));
        }
    }
    // Temperature curve enters desired range
    else {
        if ((Data->Flags & MODE_TEMP_RANGE) == 0) {
            Data->Flags |= MODE_TEMP_RANGE;
            Message.CanID = MSG_TEMP_NOTI_IN_RANGE;
            Message.Length = 2;
            bmSetMessageItem (&Message, Data->ServiceTemp[0], 0, 2);
            dbgPrint("Temperature in range\n");
            return (canWriteMessage(Data->Channel, &Message));
        }
    }

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Checks if the temperature enters or leaves the required range
 *
 *      This function checks if the actual temperature curve meassured by the
 *      functional module enters or leaves the desired temperature range set
 *      by the master. If this is the case, a notification is sent to the
 *      master. There are separate messages for entering and leaving the
 *      temperature range.
 *
 *  \iparam  Data = Data of one instance of the functional module
 *  \iparam  TempChangeDir = 0:temperature rises up, 1:temperature falls down
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempNotifySlope (InstanceData_t *Data, UInt8 TempChangeDir)
{
    CanMessage_t Message;

    Message.CanID = MSG_TEMP_NOTI_SLOPE_DETECTED;
    Message.Length = 1;
    bmSetMessageItem (&Message, TempChangeDir, 0, 1);
    return (canWriteMessage(Data->Channel, &Message));

    //return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Executes the PID algorithm
 *
 *      This function executes the PID algorithm that is needed for
 *      temperature control. It also performs auto tuning and is aware of the
 *      different behavior in heating and hold phase. It also does cascaded
 *      control if required.
 *
 *  \xparam Data = Data of one instance of the functional module
 *  \iparam Instance = Number of the instance calling the function
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempRegulation (InstanceData_t *Data, UInt16 Instance)
{
    UInt8 i;
    UInt8 j;
    UInt8 First = 0, SensorIndex = 0;
    Error_t Error;
    Int16 DevTemp = 0;
    CanMessage_t Message;

    // PID and auto tuning algorithm
    if ((Data->Flags & MODE_AUTO_TUNE) != 0) {
        First = Data->AutoTunePidNumber;
        tempPidAutoProgress (&Data->PidParams[First], Data->DesiredTemp, Data->ServiceTemp[First]);
    }
    else {
        First = 0;
#if 0
        if (tempPidCalculate (&Data->PidParams[First], Data->DesiredTemp, Data->ServiceTemp[First]) == FALSE) {
            return (E_TEMP_PID_NOT_CONFIGURED);
        }
#endif

#if 1
        if ( Data->IndexPidSensor == 0 ) {
            SensorIndex = First;
        }
        else if ( Data->NumberSensors > 1 && Data->IndexPidSensor == 0xF ) {
            if ( Data->ServiceTemp[0] < Data->ServiceTemp[1] ) {
            
                if ( Data->ServiceTemp[1] < Data->PidParams[First].MaxTemp ) {
                    SensorIndex = 0;
                }
                else {
                    SensorIndex = 1;
                }
                
            }
            else {  //  Data->ServiceTemp[1] <= Data->ServiceTemp[0]
            
                if ( Data->ServiceTemp[0] < Data->PidParams[First].MaxTemp ) {
                    SensorIndex = 1;
                }
                else {
                    SensorIndex = 0;
                }

            }
        }
#endif

        if (tempPidCalculate (&Data->PidParams[First], Data->DesiredTemp, Data->ServiceTemp[SensorIndex]) == FALSE) {
            return (E_TEMP_PID_NOT_CONFIGURED);
        }
    }

    // Cascaded PID controller
    for (i = First + 1; i < Data->NumberPid; i++) {
        if (tempPidCalculate (&Data->PidParams[i], tempPidGetOutput (&Data->PidParams[i-1]), Data->ServiceTemp[i]) == FALSE) {
            return (E_TEMP_PID_NOT_CONFIGURED);
        }
    }
    Data->ActuatingValue = tempPidGetOutput (&Data->PidParams[Data->NumberPid - 1]);
    if (Data->ActuatingValue < 0) {
        Data->ActuatingValue = 0;
    }

    // Priority computation for hold phase
    for (i = 0; i < InstanceCount; i++) {
        DataTable[i].Priority = 0;
        DevTemp = (Int16) DataTable[i].DesiredTemp - DataTable[i].ServiceTemp[0];
        for (j = 0; j < InstanceCount; j++) {
            if(DataTable[j].ModuleState == MODULE_STATE_READY && (DataTable[j].Flags & MODE_MODULE_ENABLE) != 0) {
                if (DataTable[j].DesiredTemp > DataTable[j].ServiceTemp[0] + DevTemp) {
                    DataTable[i].Priority++;
                }
                else if (DataTable[j].DesiredTemp == DataTable[j].ServiceTemp[0] + DevTemp && i > j) {
                    DataTable[i].Priority++;
                }
            }
        }
    }

    // Stop auto tuning
    if ((Data->Flags & MODE_AUTO_TUNE) != 0) {
        if (bmTimeExpired(Data->AutoTuneStartTime) >= Data->AutoTuneDuration) {
            tempPidAutoStop (&Data->PidParams[Data->AutoTunePidNumber]);
            if (Data->AutoTunePidNumber == 0) {
                Data->Flags &= ~(MODE_MODULE_ENABLE | MODE_AUTO_TUNE);
                Message.CanID = MSG_TEMP_NOTI_AUTO_TUNE;
                Message.Length = 0;
                Error = canWriteMessage(Data->Channel, &Message);
                if (Error != NO_ERROR) {
                    return (Error);
                }
            }
            else {
                Data->AutoTunePidNumber--;
                tempPidAutoStart (&Data->PidParams[Data->AutoTunePidNumber]);
            }
        }
    }

    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief  Sets the temperature related parameters of the module
 *
 *      This function is called by the CAN message dispatcher when a
 *      temperature set message is received from the master. The parameters
 *      in the message are transfered to the data structure of the addressed
 *      module instance. The modified settings influence the behavior of the
 *      module task. The following settings will be modified:
 *
 *      - Regulation active bit (1 = on, 0 = off)
 *      - Start auto tuning bit
 *      - Regulation phase bit (1 = hold, 0 = heating)
 *      - Desired temperature (in degree Celsius)
 *      - Temperature tolerance (in degree Celsius)
 *      - Sampling time (in hundredth of seconds)
 *      - Auto tuning duration (in seconds)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempSetTemperature (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 i;
    Bool Running;
    Error_t Status = NO_ERROR;
 
#ifdef DEBUG
    UInt16 InstanceID = bmGetInstance(Channel);
#endif
    
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length != 8) {
        return (E_MISSING_PARAMETERS);
    }

    // Temperature control in idle mode only
    if (Data->ModuleState != MODULE_STATE_READY) {
        return (E_TEMP_MODULE_INACTIVE);
    }

    Running = ((Data->Flags & MODE_MODULE_ENABLE) != 0);

    // Start parameter evaluation
    Data->Flags = bmGetMessageItem(Message, 0, 1);
    Data->DesiredTemp = bmGetMessageItem(Message, 1, 1) * 100;
    Data->ToleranceTemp = bmGetMessageItem(Message, 2, 1) * 100;
    TempSamplingTime = bmGetMessageItem(Message, 3, 2);
    Data->AutoTuneDuration = bmGetMessageItem(Message, 5, 2) * 1000;
    //Data->SlopeTimeInterval = bmGetMessageItem(Message, 7, 1) * 1000;
    Data->SlopeTempChange = bmGetMessageItem(Message, 7, 1) * 100;

    // Set the sampling time
    for (i = 0; i < Data->NumberPid; i++) {
        Data->PidParams[i].Ts = bmGetMessageItem(Message, 3, 2)/10;
    }

    // Set operating phase
    if((Data->Flags & MODE_TEMP_PHASE) != 0) {
        TempPhase = PHASE_HOLD;
    }
    else {
        TempPhase = PHASE_HEAT;
    }
    
    dbgPrint("TC Id:%d, Flag:%d, Sample Time:%d\n", InstanceID, Data->Flags, TempSamplingTime);
    
    // Start auto-tuning
    if ((Data->Flags & MODE_AUTO_TUNE) != 0) {
        Data->Flags |= MODE_AUTO_TUNE;
        Data->AutoTuneStartTime = bmGetTime ();
        Data->AutoTunePidNumber = Data->NumberPid - 1;
        Data->ActuatingValue = 0;
        tempPidAutoStart (&Data->PidParams[Data->AutoTunePidNumber]);
    }

    // If the module was or is deactivated, reset some parameters
    if (Running == FALSE || (Data->Flags & MODE_MODULE_ENABLE) == 0) {
        for (i = 0; i < Data->NumberPid; i++) {
            //tempPidReset(&Data->PidParams[i]);
        }
        //tempHeaterReset();
        
        TempChanged = TRUE;
        HeaterCheckSkipped = FALSE;
        
        Data->State = STATE_IDLE;
        Data->SlopeSampleCnt = 0;
        Status = tempFanControl(bmGetInstance(Channel), ((Data->Flags & MODE_MODULE_ENABLE) != 0) ? TRUE : FALSE);
    }
    return (Status);
}


/*****************************************************************************/
/*!
 *  \brief  Sets the fan speed watchdog parameters
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

static Error_t tempSetFanWatchdog (UInt16 Channel, CanMessage_t* Message)
{
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 4) {
        Data->DesiredFanSpeed = bmGetMessageItem (Message, 0, 2);
        Data->DesiredFanThreshold = bmGetMessageItem (Message, 2, 2);
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Sets current watchdog parameters
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting fan speed and heater current watchdog parameters is received
 *      from the master. The parameters in the message are transfered to the
 *      data structure of the addressed module instance. The modified settings
 *      influence the behavior of the module task. The following settings will
 *      be modified:
 *
 *      - Current sensor idle output voltage (mV)
 *      - Current sensor gain factor (mA/V)
 *      - Desired heater current (in milliampere)
 *      - Heater current threshold (in milliampere)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempSetCurrentWatchdog (UInt16 Channel, CanMessage_t* Message)
{
    UInt16 Instance = bmGetInstance(Channel);
    
    if (Message->Length == 8) {
        tempHeaterParams[Instance].CurrentGain = bmGetMessageItem (Message, 0, 2);
        tempHeaterParams[Instance].DesiredCurrent = bmGetMessageItem (Message, 2, 2);
        tempHeaterParams[Instance].DesiredCurThreshold = bmGetMessageItem (Message, 4, 2);
        tempHeaterParams[Instance].CurrentDeviation = bmGetMessageItem (Message, 6, 2);
        //tempHeaterParams[Instance].CurrentDeviation = 200;
        dbgPrint("Current gain: %d\n", tempHeaterParams[Instance].CurrentGain);
        dbgPrint("WD:%d %d %d\n", tempHeaterParams[Instance].DesiredCurrent, tempHeaterParams[Instance].DesiredCurThreshold, tempHeaterParams[Instance].CurrentDeviation);
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Sets AC current watchdog parameters
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting heater current watchdog parameters is received from the master. 
 *      The parameters in the message are transfered to the data structure of 
 *      the addressed module instance. The modified settings influence the 
 *      behavior of the module task. The following settings will be modified:
 *
 *      - Current sensor idle output voltage (mV)
 *      - Current sensor gain factor (mA/V)
 *      - Desired heater current (in milliampere)
 *      - Heater current threshold (in milliampere)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempSetAcCurrentWatchdog (UInt16 Channel, CanMessage_t* Message)
{
    UInt16 Instance = bmGetInstance(Channel);
    
    if (Message->Length == 8) {
        tempHeaterParams[Instance].CurrentMin230_Serial   = bmGetMessageItem (Message, 0, 2);
        tempHeaterParams[Instance].CurrentMax230_Serial   = bmGetMessageItem (Message, 2, 2);
        tempHeaterParams[Instance].CurrentMin100_Serial   = bmGetMessageItem (Message, 4, 2);
        tempHeaterParams[Instance].CurrentMax100_Serial   = bmGetMessageItem (Message, 6, 2);
        dbgPrint("WD:%d %d %d %d\n", tempHeaterParams[Instance].CurrentMin230_Serial, 
                                     tempHeaterParams[Instance].CurrentMax230_Serial, 
                                     tempHeaterParams[Instance].CurrentMin100_Serial,
                                     tempHeaterParams[Instance].CurrentMax100_Serial);
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Sets extra AC current watchdog parameters
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting heater current watchdog parameters is received from the master. 
 *      The parameters in the message are transfered to the data structure of 
 *      the addressed module instance. The modified settings influence the 
 *      behavior of the module task. The following settings will be modified:
 *
 *      - Current sensor idle output voltage (mV)
 *      - Current sensor gain factor (mA/V)
 *      - Desired heater current (in milliampere)
 *      - Heater current threshold (in milliampere)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempSetAcCurrentWatchdogExt (UInt16 Channel, CanMessage_t* Message)
{
    UInt16 Instance = bmGetInstance(Channel);
    
    if (Message->Length == 4) {
        tempHeaterParams[Instance].CurrentMin100_Parallel = bmGetMessageItem (Message, 0, 2);
        tempHeaterParams[Instance].CurrentMax100_Parallel = bmGetMessageItem (Message, 2, 2);
        dbgPrint("WD:%d %d\n", tempHeaterParams[Instance].CurrentMin100_Parallel,
                               tempHeaterParams[Instance].CurrentMax100_Parallel);
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
 *      - Maximal temperature permitted to the PID (in degree Celsius)
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

static Error_t tempSetPidParameters (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 8) {
        Number = bmGetMessageItem(Message, 0, 1);
        if (Number >= Data->NumberPid) {
            return (E_PARAMETER_OUT_OF_RANGE);
        }
        Data->PidParams[Number].Ready = TRUE;
        Data->PidParams[Number].MaxTemp = bmGetMessageItem(Message, 1, 1) * 100;  //Caution: Error
        Data->PidParams[Number].Kc = bmGetMessageItem(Message, 2, 2);  //Caution: Error
        Data->PidParams[Number].Ti = bmGetMessageItem(Message, 4, 2);
        Data->PidParams[Number].Td = bmGetMessageItem(Message, 6, 2);
        //tempPidReset(&Data->PidParams[Number]);
        
        if (Number == Data->NumberPid - 1) {
            Data->PidParams[Number].Range = MAX_INT16;
        }
        if (Number != 0) {
            Data->PidParams[Number-1].Range = Data->PidParams[Number].MaxTemp;
        }
        
        dbgPrint("PID:%d %d %d\n", Data->PidParams[Number].Kc, Data->PidParams[Number].Ti, Data->PidParams[Number].Td  );
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief  Reset the operating timers of the module
 *
 *      This function is called by the CAN message dispatcher when a message
 *      resetting the operating timers of the heating elements is received
 *      from the master. The parameters in the message are transfered to the
 *      data structure of the addressed module instance. The modified settings
 *      influence the behavior of the module task. The following settings will
 *      be modified:
 *
 *      - Number of the heating element
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempSetHeaterTime (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    Error_t Error;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        Number = bmGetMessageItem(Message, 0, 1);
        if (Number < Data->NumberHeaters) {
            Error = tempTimeReset(&Data->TimeParams, Number);
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
 *  \brief  Sets heater switch state
 *
 *      This function is called by the CAN message dispatcher when a message
 *      setting heater switch state is received from the master. The electric 
 *      power circuit of the heating elements will be switched immediately.
 *      The following settings will be modified:
 *
 *      - State of heater switch (SERIAL/PARALLEL)
 *      - Boolean flag of AC heater auto switch (TRUE/FALSE)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
static Error_t tempSetSwitchState (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 i;
    Int8 State;
    Int8 AutoSwitch;
    InstanceData_t* Data;    
    Bool Running = FALSE;
    Error_t Error;
    
    for (i = 0; i < InstanceCount; i++) {
        Data = &DataTable[i];
        if ((Data->Flags & MODE_MODULE_ENABLE) != 0) {
            Running = TRUE;
            break;
        }
    }
      
    if (Running) {
        return (E_TEMP_MODULE_ACTIVE);
    }

    if (Message->Length == 2) {
    
        State = (Int8)bmGetMessageItem (Message, 0, 1);
        if (State >= 0) {
            Error = tempSetHeaterSwitchState((UInt8)State);
            if(Error < 0) {
                return (Error);
            }
            dbgPrint("Switch State set to %d\n", State);
        }
        
        AutoSwitch = (Int8)bmGetMessageItem (Message, 1, 1);
        if (AutoSwitch >= 0) {
            ACHeaterAutoSwitch = (Bool)AutoSwitch;
            dbgPrint("Auto Switch: %d\n", AutoSwitch);
        }
        
        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}

/*****************************************************************************/
/*!
 *  \brief   Requests the temperature and the state vector of the module
 *
 *      This function is called by the CAN message dispatcher when a
 *      temperature get message is received from the master. As a response a
 *      temperature message is sent back to the master. The following settings
 *      will be responded:
 *
 *      - Regulation active bit (1 = on, 0 = off)
 *      - Regulation phase bit (1 = hold, 0 = heating)
 *      - Start auto tuning bit
 *      - Desired temperature (in degree Celsius)
 *      - Temperature tolerance (in degree Celsius)
 *      - Sampling time (in milliseconds)
 *      - Auto tuning duration (in seconds)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempGetTemperature (UInt16 Channel, CanMessage_t* Message)
{
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (TempPhase == PHASE_HOLD) {
        Data->Flags |= MODE_TEMP_PHASE;
    }
    else {
        Data->Flags &= ~(MODE_TEMP_PHASE);
    }
    
    if (tempHeaterParallel() == TRUE) {
        Data->Flags |= MODE_VOLTAGE_RANGE;
    }
    else {
        Data->Flags &= ~(MODE_VOLTAGE_RANGE);
    }

    RespMessage.CanID = MSG_TEMP_RESP_TEMP;
    bmSetMessageItem (&RespMessage, Data->Flags, 0, 1);
    bmSetMessageItem (&RespMessage, Data->DesiredTemp / 100, 1, 1);
    bmSetMessageItem (&RespMessage, Data->ToleranceTemp / 100, 2, 1);
    bmSetMessageItem (&RespMessage, TempSamplingTime, 3, 2);
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
 *      - Maximal temperature permitted (in degree Celsius)
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

static Error_t tempGetPidParameters (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        Number = bmGetMessageItem(Message, 0, 1);
        if(Number < Data->NumberPid) {
            RespMessage.CanID = MSG_TEMP_RESP_PID_PARAMS;
            bmSetMessageItem (&RespMessage, Number, 0, 1);
            bmSetMessageItem (&RespMessage, Data->PidParams[Number].MaxTemp / 100, 1, 1);
            bmSetMessageItem (&RespMessage, Data->PidParams[Number].Kc, 2, 2);
            bmSetMessageItem (&RespMessage, Data->PidParams[Number].Ti, 4, 2);
            bmSetMessageItem (&RespMessage, Data->PidParams[Number].Td, 6, 2);
            RespMessage.Length = 8;
            return (canWriteMessage(Data->Channel, &RespMessage));
        }
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Requests the operating time of a single heater
 *
 *      This function is called by the CAN message dispatcher when the
 *      operating of a heating element is requested by the master. The number
 *      of the heating element is sent in the first byte of the rquest
 *      message. As a response the operating time is sent back to the master.
 *      The following settings will be responded:
 *
 *      - Number of the heating element
 *      - Operating time of a heating element in seconds
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempGetHeaterTime (UInt16 Channel, CanMessage_t* Message)
{
    UInt32 Time;
    UInt8 Number;
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        Number = bmGetMessageItem(Message, 0, 1);
        if(Number < Data->NumberHeaters) {
            Time = tempTimeRead(&Data->TimeParams, Number);
            RespMessage.CanID = MSG_TEMP_RESP_HEATER_TIME;
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
 *  \brief   Requests the temperature measured by a single sensor
 *
 *      This function is called by the CAN message dispatcher when the
 *      temperature measured by a single sensor is requested by the master.
 *      The following settings will be responded:
 *
 *      - Number of the temperature sensor
 *      - Temperature from the sensor (in hundredth of degree Celsius)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempGetServiceSensor (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        Number = bmGetMessageItem(Message, 0, 1);
        if(Number < Data->NumberSensors) {
            RespMessage.CanID = MSG_TEMP_RESP_SERVICE_SENSOR;
            bmSetMessageItem (&RespMessage, Number, 0, 1);
            bmSetMessageItem (&RespMessage, Data->ServiceTemp[Number], 1, 2);
            RespMessage.Length = 3;
            return (canWriteMessage(Data->Channel, &RespMessage));
        }
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Requests the speed of a single ventilation fan
 *
 *      This function is called by the CAN message dispatcher when the speed
 *      of a single ventilation fan is requested by the master.The following
 *      settings will be responded:
 *
 *      - Number of the ventilation fan
 *      - Speed of the fan (in rotations per minute)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempGetServiceFan (UInt16 Channel, CanMessage_t* Message)
{
    UInt8 Number;
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    if (Message->Length == 1) {
        Number = bmGetMessageItem(Message, 0, 1);
        if(Number < Data->NumberFans) {
            RespMessage.CanID = MSG_TEMP_RESP_SERVICE_FAN;
            bmSetMessageItem (&RespMessage, Number, 0, 1);
            bmSetMessageItem (&RespMessage, Data->ServiceFan[Number], 1, 4);
            RespMessage.Length = 5;
            return (canWriteMessage(Data->Channel, &RespMessage));
        }
        return (E_PARAMETER_OUT_OF_RANGE);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Requests the current through the heating elements
 *
 *      This function is called by the CAN message dispatcher when the current
 *      through the heating elements is requested by the master. The following
 *      settings will be responded:
 *
 *      - Current at the heating elements (in milliampere)
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempGetHardware (UInt16 Channel, CanMessage_t* Message)
{
    CanMessage_t RespMessage;

    InstanceData_t* Data = &DataTable[bmGetInstance(Channel)];

    RespMessage.CanID = MSG_TEMP_RESP_HARDWARE;
    bmSetMessageItem (&RespMessage, Data->NumberSensors, 0, 1);
    bmSetMessageItem (&RespMessage, Data->NumberFans, 1, 1);
    bmSetMessageItem (&RespMessage, Data->NumberHeaters, 2, 1);
    bmSetMessageItem (&RespMessage, Data->NumberPid, 3, 1);
    bmSetMessageItem (&RespMessage, tempHeaterCurrent(), 4, 2);
    bmSetMessageItem (&RespMessage, tempHeaterSwitchState(), 6, 1);
    RespMessage.Length = 7;

    return (canWriteMessage(Data->Channel, &RespMessage));
}


/*****************************************************************************/
/*!
 *  \brief   Reads the board options and stores them
 *
 *  This functions reads the 32 bit data word that contains the so-called
 *  board options. This data word contains information about the diffrent
 *  peripherals connected to the hardware, e.g. the number and the type of
 *  temperature sensors.
 *
 *  \oparam  Data = Instance Data
 *  \iparam  ModuleID = ID of this module
 *  \iparam  Instance = Module instance
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t tempReadOptions (InstanceData_t *Data, UInt16 ModuleID, UInt16 Instance)
{
    UInt8 Type;
    UInt32 Options;
    
    Options = bmGetBoardOptions (ModuleID, Instance, 0);
    if (Options == 0) {
        return (E_BOARD_OPTION_MISSING);
    }

    Data->NumberSensors = Options & 0xf;
    Data->NumberHeaters = (Options >> 4) & 0xf;
    Data->NumberFans = (Options >> 8) & 0xf;
    Data->NumberPid = (Options >> 12) & 0xf;

    Type = (Options >> 16) & 0xf;
    switch (Type) {
        case 0:
            Data->SensorType = TYPEK;
            break;
        case 1:
            Data->SensorType = NTC10K3A1I;
            break;
        case 2:
            Data->SensorType = AD595;
            break;
        case 3:
            Data->SensorType = TYPET;
            break;
        case 4:
            Data->SensorType = NTCGT103F;
            break;           
        default:
            return (E_TEMP_SENSOR_NOT_SUPPORTED);
    }

    Data->IndexPidSensor = (Options >> 20) & 0xf;

    Type = (Options >> 24) & 0xf;
    switch (Type) {
        case 0:
            Data->HeaterType = TYPE_HEATER_AC;
            break;
        case 1:
            Data->HeaterType = TYPE_HEATER_DC;
            break;           
        default:
            return (E_TEMP_HEATER_NOT_SUPPORTED);
    }

    Data->DetectSlope = (Options >> 28) & 0xf;
    
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

static Error_t tempDeviceAlloc (InstanceData_t *Data)
{
    if (Data->NumberSensors > 0) {
        Data->HandleTemp = calloc (Data->NumberSensors, sizeof(Handle_t));
        if (NULL == Data->HandleTemp) {
            return (E_MEMORY_FULL);
        }
        Data->ServiceTemp = calloc (Data->NumberSensors, sizeof(UInt16));
        if (NULL == Data->ServiceTemp) {
            return (E_MEMORY_FULL);
        }
        Data->SensorErrTimestamp = calloc (Data->NumberSensors, sizeof(UInt32));
        if (NULL == Data->SensorErrTimestamp) {
            return (E_MEMORY_FULL);
        }        
    }
    if (Data->NumberFans > 0) {
        Data->ServiceFan = calloc (Data->NumberFans, sizeof(UInt16));
        if (NULL == Data->ServiceFan) {
            return (E_MEMORY_FULL);
        }
    }
    if (Data->NumberPid > 0) {
        Data->PidParams = calloc (Data->NumberPid, sizeof(TempPidParams_t));
        if (NULL == Data->PidParams) {
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

static Error_t tempHandleOpen (InstanceData_t *Data, UInt16 Instance)
{
    UInt16 i;
    UInt16 j;
    Error_t Error;
    
    i = Instance;
    
    // Open cold junction temperature sensors
    if (Data->SensorType == TYPEK || Data->SensorType == TYPET) {
        Data->HandleCompensate = halAnalogOpen (HAL_TEMP_COMPENSATE + i, HAL_OPEN_READ, 0, NULL);
        if (Data->HandleCompensate < 0) {
            return (Data->HandleCompensate);
        }
    }

    // Open temperature sensors
    for (j = 0; j < Data->NumberSensors; j++) {
        //Data->HandleTemp[j] = halAnalogOpen (HAL_TEMP_SENSOR + Data->NumberSensors * i + j, HAL_OPEN_READ, 0, NULL);
        Data->HandleTemp[j] = halAnalogOpen (HAL_TEMP_SENSOR + (TempNumSensors++), HAL_OPEN_READ, 0, NULL);     
        if (Data->HandleTemp[j] < 0) {
            return (Data->HandleTemp[j]);
        }
    }

    Error = tempFanOpen (i, Data->NumberFans);
    if (Error < 0) {
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

Error_t tempInitializeModule (UInt16 ModuleID, UInt16 Instances)
{
    static bmCallbackEntry_t Commands[] = {
        { MSG_TEMP_SET_TEMP, tempSetTemperature },
        { MSG_TEMP_SET_FAN_WATCHDOG, tempSetFanWatchdog },
        { MSG_TEMP_SET_CURRENT_WATCHDOG, tempSetCurrentWatchdog },
        { MSG_TEMP_SET_PID_PARAMS, tempSetPidParameters },
        { MSG_TEMP_SET_HEATER_TIME, tempSetHeaterTime },
        { MSG_TEMP_REQ_TEMP, tempGetTemperature },
        { MSG_TEMP_REQ_PID_PARAMS, tempGetPidParameters },
        { MSG_TEMP_REQ_HEATER_TIME, tempGetHeaterTime },
        { MSG_TEMP_REQ_SERVICE_SENSOR, tempGetServiceSensor },
        { MSG_TEMP_REQ_SERVICE_FAN, tempGetServiceFan },
        { MSG_TEMP_REQ_HARDWARE, tempGetHardware },
        { MSG_TEMP_SET_SWITCH_STATE, tempSetSwitchState },
        { MSG_TEMP_SET_AC_CURRENT_WATCHDOG, tempSetAcCurrentWatchdog },
        { MSG_TEMP_SET_AC_CURRENT_WATCHDOG_EXT, tempSetAcCurrentWatchdogExt } 
    };

    static bmModuleInterface_t Interface = {
        tempModuleTask,
        tempModuleControl,
        tempModuleStatus
    };
    Error_t Status;
    UInt16 i;
    UInt16 j;
    
    // Only for debug use
#if 0    
    UInt16 Channel;
    InstanceData_t* Data;
    UInt16 Instance;
#endif

    // allocate module instances data storage
    DataTable = calloc (Instances, sizeof(InstanceData_t));
    if (NULL == DataTable) {
        return (E_MEMORY_FULL);
    }

    for (i=0; i < Instances; i++) {
        DataTable[i].ModuleState = MODULE_STATE_COUNT;      // Invalid state
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
    tempRegisterDebugNames (ModuleID);
    #endif

    // Initialize fan speed measurements
    Status = tempFanInit(Instances);
    if (Status < NO_ERROR) {
        return (Status);
    }
    
    // Reset total number of sensors for all instances
    TempNumSensors = 0;

    // open channels and ports for all modules
    for (i=0; i < Instances; i++) {

        DataTable[i].Channel = bmGetChannel(bmGetTaskID(ModuleID, i));
        DataTable[i].Flags = 0;

        // Get the number of peripheral elements connected to the board
        Status = tempReadOptions (&DataTable[i], ModuleID, i);
        if (Status < NO_ERROR) {
            return (Status);
        }

        // Allocating device resources in respect to the number of peripherals
        Status = tempDeviceAlloc (&DataTable[i]);
        if (Status < NO_ERROR) {
            return (Status);
        }

        // Opening the handles of the peripherals
        Status = tempHandleOpen (&DataTable[i], i);
        if (Status < NO_ERROR) {
            return (Status);
        }

        // Init PID parameters
        for (j = 0; j < DataTable[i].NumberPid; j++) {
            tempPidReset (&DataTable[i].PidParams[j]);
            DataTable[i].PidParams[j].Ready = FALSE;
        };
        
        Status = tempTimeInit (ModuleID, i, &DataTable[i].TimeParams, DataTable[i].NumberHeaters);
        if (Status < NO_ERROR) {
            return Status;
        }

        // Start time recording
        TempSampleTimestamp = bmGetTime();
        DataTable[i].State = STATE_IDLE;
        DataTable[i].ModuleState = MODULE_STATE_READY;
        DataTable[i].SlopeSampleCnt = 0;
        
        DataTable[i].TempArray = NULL;
        DataTable[i].TempArraySize = 0;
        DataTable[i].TempArrayIndex = 0;
        DataTable[i].SamplesPerSec = 0;
        
        // Init time stamps for sensor error detection
        for (j = 0; j < DataTable[i].NumberSensors; j++) {
            DataTable[i].SensorErrTimestamp[j] = 0;
        };
    }

    // Initialize current measurement
    Status = tempHeaterInit (&tempHeaterParams, HAL_TEMP_CURRENT, HAL_TEMP_MAINVOLTAGE, HAL_TEMP_CTRLHEATING, DataTable[0].HeaterType, Instances);
    if (Status < NO_ERROR) {
        return Status;
    }
    
    InstanceCount = Instances;
    ModuleIdentifier = ModuleID;



// Initialization for non-IDENTIFICATION mode
// Only for debug use
#if 0

// For level sensor
    Channel = 1;
    Data = &DataTable[bmGetInstance(Channel)];
    Instance = bmGetInstance(Channel);
    ///////////////////////////////////////////////////////////
    // Set PID parameters
    Data = &DataTable[bmGetInstance(Channel)];
    Data->PidParams[0].Ready = TRUE;
    Data->PidParams[0].MaxTemp = 125 * 100;
    Data->PidParams[0].Kc = 1212;
    Data->PidParams[0].Ti = 1000;
    Data->PidParams[0].Td = 80;
    tempPidReset(&Data->PidParams[0]);
    
    //if (0 == Data->NumberPid - 1) {
        Data->PidParams[0].Range = MAX_INT16;
    //}

    //////////////////////////////////////////////////////////
    // Current watchdog
    tempHeaterParams[Instance].CurrentGain = 7813;
    tempHeaterParams[Instance].DesiredCurrent = 3000;
    tempHeaterParams[Instance].DesiredCurThreshold = 2800;
    tempHeaterParams[Instance].CurrentDeviation = 200;

    //////////////////////////////////////////////////////////
    // Set Temperature
     
    Data->Flags = 1;
    Data->DesiredTemp = 12000;
    Data->ToleranceTemp = 500;
    TempSamplingTime = 500;
    Data->AutoTuneDuration = 0;
    Data->SlopeTempChange = 99*100;

    // Set the sampling time
    for (i = 0; i < Data->NumberPid; i++) {
        Data->PidParams[i].Ts = 100;
    }

    TempPhase = PHASE_HEAT;

    for (i = 0; i < Data->NumberPid; i++) {
        tempPidReset(&Data->PidParams[i]);
    }
    tempHeaterReset();
    Data->State = STATE_IDLE;
    Status = tempFanControl(bmGetInstance(Channel), ((Data->Flags & MODE_MODULE_ENABLE) != 0) ? TRUE : FALSE);
#endif


// For retort and wax bath

#if 0

    Channel = 1;
    Data = &DataTable[bmGetInstance(Channel)];
    Instance = bmGetInstance(Channel);
    ///////////////////////////////////////////////////////////
    // Set PID parameters
    Data = &DataTable[bmGetInstance(Channel)];
    Data->PidParams[0].Ready = TRUE;
    Data->PidParams[0].MaxTemp = 100 * 100;
    Data->PidParams[0].Kc = 6000;
    Data->PidParams[0].Ti = 1000;
    Data->PidParams[0].Td = 80;
    tempPidReset(&Data->PidParams[0]);
    
    //if (0 == Data->NumberPid - 1) {
        Data->PidParams[0].Range = MAX_INT16;
    //}

    //////////////////////////////////////////////////////////
    // Current watchdog
    tempHeaterParams[Instance].CurrentGain = 7813;
    tempHeaterParams[Instance].DesiredCurrent = 350;
    tempHeaterParams[Instance].DesiredCurThreshold = 100;
    tempHeaterParams[Instance].CurrentDeviation = 200;


    //////////////////////////////////////////////////////////
    // Set Temperature
     
    Data->Flags = 1;
    Data->DesiredTemp = 4000;
    Data->ToleranceTemp = 500;
    TempSamplingTime = 500;
    Data->AutoTuneDuration = 0;
    Data->SlopeTempChange = 99*100;

    // Set the sampling time
    for (i = 0; i < Data->NumberPid; i++) {
        Data->PidParams[i].Ts = 100;
    }

    TempPhase = PHASE_HEAT;

    for (i = 0; i < Data->NumberPid; i++) {
        tempPidReset(&Data->PidParams[i]);
    }
    tempHeaterReset();
    Data->State = STATE_IDLE;
    Status = tempFanControl(bmGetInstance(Channel), ((Data->Flags & MODE_MODULE_ENABLE) != 0) ? TRUE : FALSE);

#endif


#if 0

    Channel = 2;
    Data = &DataTable[bmGetInstance(Channel)];
    Instance = bmGetInstance(Channel);
    ///////////////////////////////////////////////////////////
    // Set PID parameters
    Data = &DataTable[bmGetInstance(Channel)];
    Data->PidParams[0].Ready = TRUE;
    Data->PidParams[0].MaxTemp = 100 * 100;
    Data->PidParams[0].Kc = 6000;
    Data->PidParams[0].Ti = 1000;
    Data->PidParams[0].Td = 80;
    tempPidReset(&Data->PidParams[1]);
    
    //if (0 == Data->NumberPid - 1) {
        Data->PidParams[0].Range = MAX_INT16;
    //}

    //////////////////////////////////////////////////////////
    // Current watchdog
    tempHeaterParams[Instance].CurrentGain = 7813;
    tempHeaterParams[Instance].DesiredCurrent = 760;
    tempHeaterParams[Instance].DesiredCurThreshold = 100;
    tempHeaterParams[Instance].CurrentDeviation = 200;


    //////////////////////////////////////////////////////////
    // Set Temperature
     
    Data->Flags = 1;
    Data->DesiredTemp = 8000;
    Data->ToleranceTemp = 500;
    TempSamplingTime = 500;
    Data->AutoTuneDuration = 0;
    Data->SlopeTempChange = 99*100;

    // Set the sampling time
    for (i = 0; i < Data->NumberPid; i++) {
        Data->PidParams[i].Ts = 100;
    }

    TempPhase = PHASE_HEAT;

    for (i = 0; i < Data->NumberPid; i++) {
        tempPidReset(&Data->PidParams[i]);
    }
    tempHeaterReset();
    Data->State = STATE_IDLE;
    Status = tempFanControl(bmGetInstance(Channel), ((Data->Flags & MODE_MODULE_ENABLE) != 0) ? TRUE : FALSE);

#endif

#if 0

    Channel = 3;
    Data = &DataTable[bmGetInstance(Channel)];
    Instance = bmGetInstance(Channel);
    ///////////////////////////////////////////////////////////
    // Set PID parameters
    Data = &DataTable[bmGetInstance(Channel)];
    Data->PidParams[0].Ready = TRUE;
    Data->PidParams[0].MaxTemp = 90 * 100;
    Data->PidParams[0].Kc = 6000;
    Data->PidParams[0].Ti = 1000;
    Data->PidParams[0].Td = 80;
    tempPidReset(&Data->PidParams[1]);
    
    //if (0 == Data->NumberPid - 1) {
        Data->PidParams[0].Range = MAX_INT16;
    //}

    //////////////////////////////////////////////////////////
    // Current watchdog
    tempHeaterParams[Instance].CurrentGain = 7813;
    tempHeaterParams[Instance].DesiredCurrent = 1750;
    tempHeaterParams[Instance].DesiredCurThreshold = 100;
    tempHeaterParams[Instance].CurrentDeviation = 200;


    //////////////////////////////////////////////////////////
    // Set Temperature
     
    Data->Flags = 1;
    Data->DesiredTemp = 8000;
    Data->ToleranceTemp = 500;
    TempSamplingTime = 500;
    Data->AutoTuneDuration = 0;
    Data->SlopeTempChange = 99*100;

    // Set the sampling time
    for (i = 0; i < Data->NumberPid; i++) {
        Data->PidParams[i].Ts = 100;
    }

    TempPhase = PHASE_HEAT;

    for (i = 0; i < Data->NumberPid; i++) {
        tempPidReset(&Data->PidParams[i]);
    }
    tempHeaterReset();
    Data->State = STATE_IDLE;
    Status = tempFanControl(bmGetInstance(Channel), ((Data->Flags & MODE_MODULE_ENABLE) != 0) ? TRUE : FALSE);

#endif

#if 0

    Channel = 4;
    Data = &DataTable[bmGetInstance(Channel)];
    Instance = bmGetInstance(Channel);
    ///////////////////////////////////////////////////////////
    // Set PID parameters
    Data = &DataTable[bmGetInstance(Channel)];
    Data->PidParams[0].Ready = TRUE;
    Data->PidParams[0].MaxTemp = 90 * 100;
    Data->PidParams[0].Kc = 6000;
    Data->PidParams[0].Ti = 1000;
    Data->PidParams[0].Td = 80;
    tempPidReset(&Data->PidParams[1]);
    
    //if (0 == Data->NumberPid - 1) {
        Data->PidParams[0].Range = MAX_INT16;
    //}

    //////////////////////////////////////////////////////////
    // Current watchdog
    tempHeaterParams[Instance].CurrentGain = 7813;
    tempHeaterParams[Instance].DesiredCurrent = 1000;
    tempHeaterParams[Instance].DesiredCurThreshold = 100;
    tempHeaterParams[Instance].CurrentDeviation = 200;


    //////////////////////////////////////////////////////////
    // Set Temperature
     
    Data->Flags = 1;
    Data->DesiredTemp = 8000;
    Data->ToleranceTemp = 500;
    TempSamplingTime = 500;
    Data->AutoTuneDuration = 0;
    Data->SlopeTempChange = 99*100;

    // Set the sampling time
    for (i = 0; i < Data->NumberPid; i++) {
        Data->PidParams[i].Ts = 100;
    }

    TempPhase = PHASE_HEAT;

    for (i = 0; i < Data->NumberPid; i++) {
        tempPidReset(&Data->PidParams[i]);
    }
    tempHeaterReset();
    Data->State = STATE_IDLE;
    Status = tempFanControl(bmGetInstance(Channel), ((Data->Flags & MODE_MODULE_ENABLE) != 0) ? TRUE : FALSE);

#endif


    return (NO_ERROR);
}

//****************************************************************************/

/****************************************************************************/
/*! \file ModuleConfig.h
 *
 *  \brief
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.07.2010
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Description:
 *
 *       This module contains the declaration of the class CModuleConfig
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DEVICECONTROL_MODULECONFIG_H
#define DEVICECONTROL_MODULECONFIG_H

#include <QMap>
#include <QString>
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "DeviceControl/Include/Global/DeviceControlError.h"

#include "ModuleIDs.h"

namespace DeviceControl
{

/****************************************************************************/
/*!
 *  \brief  This is the base class for CAN-Object configuration.
 *
 *      The class contains configuration parameter equal for all CAN-Object
 *      (CANNode and all CAN-function modules). The class is instanced while
 *      hw configuration is read. The class transfers the configuration
 *      parameters to the CAN-object classes, which will be created later.
 */
/****************************************************************************/
class CModuleConfig
{
public:
    CModuleConfig() {
        m_ObjectType = CAN_OBJ_TYPE_UNDEF;
        m_sCANNodeType = 0;
        m_sCANNodeIndex = 0;
        pParent = 0;
        m_sChannel = 0;
        m_sOrderNr = 0;
    }

    virtual ~CModuleConfig() {}

    /*! enum defines for all CANObject types  */
    typedef enum {
        CAN_OBJ_TYPE_NODE             = MODULE_ID_BASEMODULE,   ///< CAN node (basis module)
        CAN_OBJ_TYPE_DIGITAL_OUT_PORT = MODULE_ID_DIGITAL_OUT,  ///< digital output
        CAN_OBJ_TYPE_DIGITAL_IN_PORT  = MODULE_ID_DIGITAL_IN,   ///< digital input
        CAN_OBJ_TYPE_ANALOG_OUT_PORT  = MODULE_ID_ANALOG_OUT,   ///< analog output
        CAN_OBJ_TYPE_ANALOG_IN_PORT   = MODULE_ID_ANALOG_IN,    ///< analog input
        CAN_OBJ_TYPE_STEPPERMOTOR     = MODULE_ID_STEPPER,      ///< stepper motor
        CAN_OBJ_TYPE_JOYSTICK         = MODULE_ID_JOYSTICK,     ///< joystick
        CAN_OBJ_TYPE_RFID11785        = MODULE_ID_RFID11785,    ///< RFID reader type 11785
        CAN_OBJ_TYPE_RFID15693        = MODULE_ID_RFID15693,    ///< RFID reader
        CAN_OBJ_TYPE_TEMPERATURE_CTL  = MODULE_ID_TEMPERATURE,  ///< temperature control
        CAN_OBJ_TYPE_UART             = MODULE_ID_UART,         ///< serial interface UART
#ifdef PRE_ALFA_TEST
        CAN_OBJ_TYPE_PRESSURE_CTL     = MODULE_ID_PRESSURE,
#endif
        CAN_OBJ_TYPE_DEVICEMNG        = 98,                     ///< Device processing
        CAN_OBJ_TYPE_UNDEF            = 99                      ///< undefined type
    } CANObjectType_t;

    CANObjectType_t m_ObjectType;   //!< Type of the object
    QString m_strKey;               //!< unique key of the object
    QString m_strName;              //!< Name of the object (used by GUI)

    short m_sCANNodeType;           //!< Type of the CANNode (hardware identifier)
    short m_sCANNodeIndex;          //!< Index of the CANNode (to differenciate equal hardware identifiers)
    const CModuleConfig* pParent;   //!< Pointer to parent instance (if fct-module, otherwise NULL)
    quint8 m_sChannel;              //!< The channel of the CAN object
    short m_sOrderNr;               //!< Order number, used by GUI
};

/*! \brief This class transfers the CANDigitInput-Object configuration.
*    The class contains configuration parameter of the CANDigitInput-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleDigitInput : public CModuleConfig
{
public:
    CANFctModuleDigitInput() { m_bEnabled = 0;
                               m_bTimeStamp = 0;
                               m_sPolarity = 0;
                               m_sSupervision = 0;
                               m_bInterval = 0;
                               m_bDebounce = 0;
                              };
    quint8  m_bEnabled;     //!< Enabled flag
    quint8  m_bTimeStamp;   //!< Time stamp flag
    quint16 m_sPolarity;    //!< input polarity
    quint16 m_sSupervision; //!< Supervision flag
    quint8  m_bInterval;    //!< Read intervall
    quint8  m_bDebounce;    //!< Input value debounce setting
};

/*! \brief This class transfers the CANDigitOutput-Object configuration.
*    The class contains configuration parameter of the CANDigitOutput-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleDigitOutput : public CModuleConfig
{
public:
    CANFctModuleDigitOutput() {
        m_bEnabled = 0;
        m_bInaktivAtShutdown = 0;
        m_bInaktivAtEmgyStop = 0;
        m_sPolarity = 0;
        m_sOutvalInactiv = 0;
        m_sLivetimeLimit = 0;
    }

    quint8  m_bEnabled;             //!< Enable/Disable modul
    quint8  m_bInaktivAtShutdown;   //!< set inaktiv in shutdown mode
    quint8  m_bInaktivAtEmgyStop;   //!< set inaktiv in emergency stop mode
    quint16 m_sPolarity;            //!< output polarity
    quint16 m_sOutvalInactiv;       //!< output value when inactiv
    quint16 m_sLivetimeLimit;       //!< on/off limit for lifetim counter
};

/*! \brief This class transfers the CANAnalogInput-Object configuration.
*    The class contains configuration parameter of the CANAnalogInput-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleAnalogInput : public CModuleConfig
{
public:
    CANFctModuleAnalogInput() {
        m_bEnabled = 0;
        m_bTimeStamp = 0;
        m_bFastSampling = 0;
        m_sLimitAutoSend = 0;
        m_sInterval = 0;
        m_sDebounce = 0;
        m_bLimitValue1SendExceed = 0;
        m_bLimitValue1SendBelow = 0;
        m_bLimitValue1SendWarnMsg = 0;
        m_bLimitValue1SendDataMsg = 0;
        m_sLimitValue1 = 0;
        m_bLimitValue2SendExceed = 0;
        m_bLimitValue2SendBelow = 0;
        m_bLimitValue2SendWarnMsg = 0;
        m_bLimitValue2SendDataMsg = 0;
        m_sLimitValue2 = 0;
        m_sHysteresis = 0;
    }

    quint8 m_bEnabled;      //!< Enabled flag
    quint8 m_bTimeStamp;    //!< Time stamp flag
    quint8 m_bFastSampling; //!< Supervision flag
    quint16 m_sLimitAutoSend;   //!< auto limit activation level
    quint8 m_sInterval;     //!< Read intervall
    quint8 m_sDebounce;     //!< Input value debounce setting

    quint8  m_bLimitValue1SendExceed;  //!< limit upper level
    quint8  m_bLimitValue1SendBelow;   //!< limit lower level
    quint8  m_bLimitValue1SendWarnMsg; //!< limit warning flag
    quint8  m_bLimitValue1SendDataMsg; //!< limit data flag
    quint16 m_sLimitValue1;            //!< limit upper value

    quint8  m_bLimitValue2SendExceed;  //!< limit2 upper level
    quint8  m_bLimitValue2SendBelow;   //!< limit2 lower level
    quint8  m_bLimitValue2SendWarnMsg; //!< limit2 warning flag
    quint8  m_bLimitValue2SendDataMsg; //!< limit2 data flag
    quint16 m_sLimitValue2;            //!< limit2 upper value

    quint16 m_sHysteresis;            //!< hysteresis
};

/*! \brief This class transfers the CANAnalogOutput-Object configuration.
*    The class contains configuration parameter of the CANAnalogOutput-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleAnalogOutput : public CModuleConfig
{
public:
    CANFctModuleAnalogOutput() {
        m_bEnabled = 0;
        m_bInaktivAtShutdown = 0;
        m_bInaktivAtEmgyStop = 0;
        m_sMode = 0;
        m_sBitCount = 0;
        m_sOutvalInactiv = 0;
        m_sLivetimeLimit = 0;
    }

    quint8  m_bEnabled;             //!< Enable/Disable modul
    quint8  m_bInaktivAtShutdown;   //!< set inaktiv in shutdown mode
    quint8  m_bInaktivAtEmgyStop;   //!< set inaktiv in emergency stop mode
    quint8  m_sMode;                //!< operation mode
    quint8  m_sBitCount;            //!< resolution, number of bits
    quint16 m_sOutvalInactiv;       //!< output value when inactiv
    quint16 m_sLivetimeLimit;       //!< on/off limit for lifetime counter
};

/*! \brief This class transfers the limit switch configuration data
*    The class contains the parameter of a stepper motor's limit switch configuration
*    The class is instanced while hardware configuration is read. The class transfers
*    the configuration parameters to the CANStepperMotor classes, which will be created later.
*
*/
class CANFctModuleLimitSwitch
{
public:
    CANFctModuleLimitSwitch() {
        bIndex = 0;
        bExists = 0;
        bOrientation = 0;
        bPolarity = 0;
        bSampleRate = 0;
        bDebounce = 0;
    }

    quint8  bIndex;       //!< Index of the limit switch
    quint8  bExists;      //!< Flag to mark it as exists
    quint8  bOrientation; //!< orientation how to run into the limit switch ( cw - ccw)
    quint8  bPolarity;    //!< input polarity
    quint8  bSampleRate;  //!< sample rate
    quint8  bDebounce;    //!< debounce counter
};


/*! \brief This class transfers the limit switch position code configuration data
*    The class contains the parameter of a stepper motor's limit switch code configuration
*    The class is instanced while hardware configuration is read. The class transfers
*    the configuration parameters to the CANStepperMotor classes, which will be created later.
*
*/
class CANFctModulePosCode
{
public:
    CANFctModulePosCode() {
        bValid = 0;
        bStop = 0;
        bStopDir = 1;   // == ROTATION_DIR_CW
        position = 0;
        width = 0;
        deviation = 0;
#ifdef PRE_ALFA_TEST
        bRotDirCheck = 0;
        hitSkip = 0;
#endif
    }

    quint8  bValid;         //!< set when this position code represents a valid value
    quint8  bStop;          //!< determines if motor should be stopped when this position is crossed
    quint8  bStopDir;       //!< stop motor when motor runs in this direction into the limit switch ( cw - ccw)
    qint32  position;       //!< centered limit switch position in half-steps
    quint8  width;          //!< active limit switch width in half-steps
    quint8  deviation;      //!< tolerated deviation (+/-) in half-steps
#ifdef PRE_ALFA_TEST
    quint8  bRotDirCheck;
    quint8  hitSkip;
#endif
};


/*! \brief This class transfers the motions profile data
*    The class contains the parameter of a stepper motor's motion profile
*    The class is instanced while hardware configuration is read. The class transfers
*    the configuration parameters to the CANStepperMotor classes, which will be created later.
*
*/
class CANFctModuleMotionProfile
{
public:
    CANFctModuleMotionProfile() {
        sSpeedMin = 0;
        sSpeedMax = 0;
        sAcc = 0;
        sDec = 0;
        sAccTime = 0;
        sDecTime = 0;
        bMicroSteps = 0;
        bRampType = 0;
        bIndex = 0;
    }

    quint16 sSpeedMin;  //!< minimal speed [half steps/sec]
    quint16 sSpeedMax;  //!< target speed [half steps/sec]
    quint16 sAcc;       //!< acceleration [half steps/sec²]
    quint16 sDec;       //!< deceleration [half steps/sec²]
    quint16 sAccTime;   //!< time of acceleration
    quint16 sDecTime;   //!< time of deceleration ramp
    quint8  bMicroSteps;//!< micro step mode
    quint8  bRampType;  //!< ramp type (s-curve)
    quint8  bIndex;     //!< referencing index of the motion profile
};

//! Type definition of the motion profile map for stepper motors
typedef QMap<short, CANFctModuleMotionProfile> MotionProfileMap;

/*! \brief This class transfers the CANStepperMotor-Object configuration.
*    The class contains configuration parameter of the CANStepperMotor-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleStepperMotor : public CModuleConfig
{
public:
    CANFctModuleStepperMotor() {
        rotationType = ROTATION_TYPE_UNDEF;
        sResolution = 0;
        sResetPosition = 0;
        bDirection = ROTATION_DIR_UNDEF;
        bEncoderType = 0;
        sEncoderResolution = 0;
        bEncoderDir = ROTATION_DIR_UNDEF;
        lMinPosition = 0;
        lMaxPosition = 0;
        sMinSpeed = 0;
        sMaxSpeed = 0;
        refRunRefPos = 1;
/*
#ifdef PRE_ALFA_TEST
        refRunRefPosSkip = 0;
#endif
*/
        lRefRunMaxDistance = 0;
        sRefRunTimeout = 0;
        lRefRunReverseDistance = 0;
        lRefPosOffset = 0;
        sRefRunSlowSpeed = 0;
        sRefRunHighSpeed = 0;
        sStepLossWarnLimit = 0;
        sStepLossErrorLimit = 0;
        sCurrentLimit = 0;

        runCurrentScale = 0;
        stopCurrentScale = 0;
        stopCurrentDelay = 0;
        driverType = DRIVER_DEFAULT;
    }

    /*! enum rotation direction  */
    typedef enum {
        ROTATION_TYPE_UNDEF  = 0,   ///< undefined
        ROTATION_TYPE_LINEAR = 1,   ///< linear
        ROTATION_TYPE_ROT    = 2    ///< rotatorically
    } RotationType_t;

    /*! enum rotation direction  */
    typedef enum {
        ROTATION_DIR_UNDEF = 0, ///< undefined
        ROTATION_DIR_CW    = 1, ///< clockwise
        ROTATION_DIR_CCW   = 2  ///< counter clockwise
    } RotationDir_t;

    // position control
    RotationType_t rotationType;    //!< rotation type
    quint16        sResolution;     //!< motor resolution (half-steps per revolution)
    quint16        sResetPosition;  //!< count of halfsteps for one revolution. ignored if zero.
    RotationDir_t  bDirection;      //!< the motor's rotation dir (causes an increasing step counter)
    quint8         bEncoderType;    //!< encoder type
    quint16        sEncoderResolution;  //!< encoder resolution
    RotationDir_t  bEncoderDir;     //!< enxoder direction
    CANFctModuleLimitSwitch LimitSwitch1;   //!< limit switch 1
    CANFctModuleLimitSwitch LimitSwitch2;   //!< limit switch 2

    CANFctModulePosCode PosCode1;   //!< limit switch position code 1
    CANFctModulePosCode PosCode2;   //!< limit switch position code 2
    CANFctModulePosCode PosCode3;   //!< limit switch position code 3

    qint32 lMinPosition;        //!< minimum motor position in half-steps
    qint32 lMaxPosition;        //!< maximum motor position in half-steps

    qint16 sMinSpeed;           //!< minimal speed in half-steps/sec
    qint16 sMaxSpeed;           //!< miximal speed in half-steps/sec

    // reference run parameters
    qint8         refRunRefPos;            //!< the limit switch position code used as reference position
    qint32        lRefRunMaxDistance;      //!< maximum number of half-steps during reference run
#ifdef PRE_ALFA_TEST
    quint16        sRefRunTimeout;         //work around, need value bigger than 32767 !< maximum duration to perform each movement in ms
#else
    qint16        sRefRunTimeout;          //!< maximum duration to perform each movement in ms
#endif
    qint32        lRefRunReverseDistance;  //!< distance for reverse move between high and low speed cycle
    qint32        lRefPosOffset;           //!< offset between the reference position and internal position system
    qint16        sRefRunSlowSpeed;        //!< low speed during reference run in half-steps/sec
    qint16        sRefRunHighSpeed;        //!< high speed during reference run in half-steps/sec
/*
#ifdef PRE_ALFA_TEST
    quint8        refRunRefPosSkip;
#endif
*/

    MotionProfileMap listMotionProfiles; //!< list of the motors motion profiles

    // supervision
    qint16       sStepLossWarnLimit;   //!< warning limit for step loss errors
    qint16       sStepLossErrorLimit;  //!< error limit for step loss errors
    qint16       sCurrentLimit;        //!< limit for current supervision

    // current scale settings
    quint8       runCurrentScale;      //!< run current scale in runCurrent*1/32 percent, used when motor is moving
    quint8       stopCurrentScale;     //!< stop current scale in stopCurrent*1/32 percent, used when motor is standing still
    quint16      stopCurrentDelay;     //!< delay from motor standing still to applying stop current in ms

    // motor driver configuration
    //! List of supported stepper motor drivers
    typedef enum {
        DRIVER_DEFAULT = 0, //!< Default driver, default configuration from the HAL configuration file is used
        DRIVER_TMC26X = 1   //!< Trinamic TMC26x driver
    } DriverType_t;

    DriverType_t driverType;    //!< Driver type

    //! Trinamic tmc26x configuration register
    typedef struct {
        quint32 drvConf;
        quint32 sgcsConf;
        quint32 smartEn;
        quint32 chopConf;
    } TMC26xReg_t;

    TMC26xReg_t tmc26x; //!< Trinamic tmc26x configuration register
};

/*! \brief This class transfers the CANRFID11785-Object configuration.
*    The class contains configuration parameter of the CANRFID11785-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleRFID11785 : public CModuleConfig
{
public:
    CANFctModuleRFID11785() {
    }
};

/*! \brief This class transfers the CANRFID15693-Object configuration.
*    The class contains configuration parameter of the CANRFID15693-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleRFID15693 : public CModuleConfig
{
public:
    CANFctModuleRFID15693() { m_bType = 0;
                              m_bProtocol = 0;
                              m_DataRate = 0;
                            };
    quint8 m_bType;     //!< interface type
    quint8 m_bProtocol; //!< protocol type
    quint8 m_DataRate;  //!< data transmition rate in relation to RFID signal
};

/*! \brief This class transfers the CANInclinometer-Object configuration.
*    The class contains configuration parameter of the CANInclinometer-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleInclinometer : public CModuleConfig
{
public:
    CANFctModuleInclinometer() { m_bType = 0; };

    quint8 m_bType;     //!< interface type

};

/*! \brief This class transfers the PID controller data
*    The class contains the parameters of a temperature module's PID controller
*    The class is instanced while hardware configuration is read. The class transfers
*    the configuration parameters to the TemperatureControl classes, which will be created later.
*
*/
class CANFctPidController
{
public:
    CANFctPidController() {
        sMaxTemperature = 0;
        sControllerGain = 0;
        sResetTime = 0;
        sDerivativeTime = 0;
    }
    quint16 sMaxTemperature;    //!< Max target temperature of the controller [°C]
    quint16 sControllerGain;    //!< PID controller gain parameter
    quint16 sResetTime;         //!< Integral parameter of the PID [hundredth of seconds]
    quint16 sDerivativeTime;    //!< Derivative parameter of the PID [hundredth of seconds]
};

/*! \brief This class transfers the CANTempCtrl-Object configuration.
*    The class contains configuration parameter of the CANTempCtrl-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleTempCtrl : public CModuleConfig
{
public:
    CANFctModuleTempCtrl() {
        bTempTolerance = 0;
        sSamplingPeriod = 0;
        sFanSpeed = 0;
        sFanThreshold = 0;
        sCurrentGain = 0;
        sHeaterCurrent = 0;
        sHeaterThreshold = 0;
    }
    quint8 bTempTolerance;      //!< temperature tolerance in degree Celsius
    quint16 sSamplingPeriod;    //!< sampling period in hundredth of seconds
    quint16 sFanSpeed;          //!< fan speed in rotations per minute
    quint16 sFanThreshold;      //!< fan threshold in rotations per minute
    quint16 sCurrentGain;       //!< current sensor gain in mA/V
    quint16 sHeaterCurrent;     //!< current through the heaters in mA
    quint16 sHeaterThreshold;   //!< current threshold of the heaters in mA
    QList<CANFctPidController> listPidControllers;  //!< List of PID parameter sets
};
#ifdef PRE_ALFA_TEST

class CANPressureFctPidController
{
public:
    CANPressureFctPidController() {
        sMaxPressure = 0;
        sMinPressure = 0;
        sControllerGain = 0;
        sResetTime = 0;
        sDerivativeTime = 0;
    }
    qint16 sMaxPressure;        //!< Max target pressure of the
    qint16 sMinPressure;
    quint16 sControllerGain;    //!< PID controller gain parameter
    quint16 sResetTime;         //!< Integral parameter of the PID [hundredth of seconds]
    qint8 sDerivativeTime;    //!< Derivative parameter of the PID [hundredth of seconds]
};

class CANPressureFctPwmController
{
public:
    CANPressureFctPwmController() {
    sMaxActuatingValue = 0;
    sMinActuatingValue = 0;
    sMaxPwmDuty = 0;
    sMinPwmDuty = 0;
}

    quint16 sMaxActuatingValue;
    quint16 sMinActuatingValue;
    quint8 sMaxPwmDuty;
    quint8 sMinPwmDuty;
};

class CANFctModulePressureCtrl : public CModuleConfig
{
public:
    CANFctModulePressureCtrl() {
        bPressureTolerance = 0;
        sSamplingPeriod = 0;
        sFanSpeed = 0;
        sFanThreshold = 0;
        sCurrentGain = 0;
        sPumpCurrent = 0;
        sPumpThreshold = 0;
    }
    quint8 bPressureTolerance;      //!<
    quint16 sSamplingPeriod;    //!< sampling period in hundredth of seconds
    quint16 sFanSpeed;          //!< fan speed in rotations per minute
    quint16 sFanThreshold;      //!< fan threshold in rotations per minute
    quint16 sCurrentGain;       //!< current sensor gain in mA/V
    quint16 sPumpCurrent;       //!< current through the pumps in mA
    quint16 sPumpThreshold;     //!< current threshold of the pumps in mA
    QList<CANPressureFctPidController> listPidControllers;  //!< List of PID parameter sets
    CANPressureFctPwmController pwmController;
};
#endif
/*! \brief This class transfers the CANoystick-Object configuration.
*    The class contains configuration parameter of the CANoystick-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleJoystick : public CModuleConfig
{
public:
    CANFctModuleJoystick() { m_bCommModeThresHold = 0;
                             m_sSampleRate = 0;
                             m_sUpperThreshold = 0;
                             m_sLowerThreshold = 0;};

    quint8  m_bCommModeThresHold;  //!< threshold
    quint16 m_sSampleRate;        //!< input sample rate
    quint16 m_sUpperThreshold;    //!< upper limit
    quint16 m_sLowerThreshold;    //!< lower limit
};

/*! \brief This class transfers the CANUART-Object configuration.
*    The class contains configuration parameter of the CANUART-Object
*    The class is instanced while hw configuration is read. The class transfers
*    the configuration parameters to the CAN-object classes, which will be created later.
*
*/
class CANFctModuleUART : public CModuleConfig
{
public:
    CANFctModuleUART() { m_bEnabled = 0;
                         m_bReadCommunicationMode = 0;
                         m_bStopBits = 0;
                         m_bParityEnabled = 0;
                         m_bParity = 0;
                         m_sBaudrate = 0;
                       };

    quint8  m_bEnabled;               //!< module enabled / disabled
    quint8  m_bReadCommunicationMode; //!< read communication mode
    quint8  m_bStopBits;              //!< number of stop bits
    quint8  m_bParityEnabled;         //!< parity bit enable
    quint8  m_bParity;                //!< parity setting
    quint16 m_sBaudrate;              //!< Baudrate
};

/*! \brief This class keeps the configuration data of the base device class.
*
*
*/
/*! fuction module list, assignment between function module key and it's identifier */
typedef QMap<QString, quint32> DeviceFctModList;

/****************************************************************************/
/*!
 *  \brief  Base class for configuration classes
 */
/****************************************************************************/
class BaseDeviceConfiguration
{
public:
    BaseDeviceConfiguration() {
        m_InstanceID = DEVICE_INSTANCE_ID_UNDEFINED;
        m_Optional = false;
        m_OrderNr = 0;
    }

    QString m_Type;                     //!< device type
    DevInstanceID_t m_InstanceID;       //!< Instance id
    bool m_Optional;                    //!< optional device
    quint8 m_OrderNr;                   //!< creation order number
    DeviceFctModList m_DevFctModList;   //!< list of the function modules need by the device
};

} //namespace

#endif //DEVICECONTROL_MODULECONFIG_H

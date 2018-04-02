//#ifndef DEVICEMODULEMODEL_H
//#define DEVICEMODULEMODEL_H


//class DeviceModuleModel
//{
//public:
//    DeviceModuleModel();

//protected:
//    CTemperatureControl** m_pTempCtrl;         //!< Temperature control
//    qreal m_CurrentTemperatures[OVEN_TEMP_CTRL_NUM][MAX_SENSOR_PER_TEMP_CTRL];                    //!< Current temperature
//    qreal m_TargetTemperatures[OVEN_TEMP_CTRL_NUM];

//    // rv

//    CANFctModuleStepperMotor m_Config;        //!< Copy of configuration.
//    CStepperMotor* m_pMotorRV;                //!< Oven cover motor

//    qint32 m_CurrentLimitSwitchCode;                //!< Current limit switchs' code
//    quint32 m_CurrentLowerLimit;                    //!< Current lower limit
//    RVPosition_t m_RVCurrentPosition;               //!< Current rotary valve postion
//    RVPosition_t m_RVPrevPosition;                  //!< Privious rotary valve postion
//    qint32 m_CurrentPosition;                       //!< Current position (stored by asynchronous call)
//    qreal m_CurrentTemperature[MAX_SENSOR_PER_TEMP_CTRL];                     //!< Current temperature
//    qreal m_TargetTemperature;                      //!< Current temperature
//    TempCtrlStatus_t m_TargetTempCtrlStatus;        //!< Target temperature control status; for verification of action result.
//    TempCtrlStatus_t m_CurrentTempCtrlStatus;       //!< Current temperature control status
//    qint64 m_LastGetTempTime[50];                   //!< Last get temperature time
//    TempCtrlMainsVoltage_t m_MainsVoltageStatus;    //!< Mains voltage state of the heaters
//    QMutex m_Mutex;                                 //!< Protects the task handling thread from request functions
//    qint64 m_LastGetTCCurrentTime;       ///<  Definition/Declaration of variable m_LastGetTCCurrentTime
//    TempCtrlHardwareStatus_t m_TCHardwareStatus;       ///<  Definition/Declaration of variable m_TCHardwareStatus




//    //oven
//    CTemperatureControl* m_pTempCtrls[OVEN_TEMP_CTRL_NUM];              //!< Temperature control modules of the device
//    CDigitalInput* m_pLidDigitalInput;                                  //!< Digital input function module for the lid

//    qreal m_CurrentTemperatures[OVEN_TEMP_CTRL_NUM][MAX_SENSOR_PER_TEMP_CTRL];                    //!< Current temperature
//    qreal m_TargetTemperatures[OVEN_TEMP_CTRL_NUM];                     //!< Current temperature
//    TempCtrlStatus_t m_TargetTempCtrlStatus[OVEN_TEMP_CTRL_NUM];        //!< Target temperature control status; for verification of action result.
//    TempCtrlStatus_t m_CurrentTempCtrlStatus[OVEN_TEMP_CTRL_NUM];       //!< Current temperature control status
//    qint64           m_LastGetTempCtrlStatus[OVEN_TEMP_CTRL_NUM];       //!< time for update temp ctrl status
//    TempCtrlMainsVoltage_t m_MainsVoltageStatus[OVEN_TEMP_CTRL_NUM];    //!< Mains voltage state of the heaters
//    qint64 m_LastGetTempTime[OVEN_TEMP_CTRL_NUM][MAX_SENSOR_PER_TEMP_CTRL];                    //!< Last get temperature time
//    qint64 m_LastGetLidStatusTime;                                      //!< Last get lid status time
//    QMap<quint32, OVENTempCtrlType_t> m_InstTCTypeMap;                  //!< Map between instance ID and temperatre control modules
//    quint16 m_LidStatus;     //!< Target output value; for verification of action result
//    TempCtrlHardwareStatus_t m_TCHardwareStatus[OVEN_TEMP_CTRL_NUM];    //!< Hardware status of the heaters
//    qint64 m_LastGetTCCurrentTime[OVEN_TEMP_CTRL_NUM];                  //!< Last time of getting current

//    // al
//    CPressureControl* m_pPressureCtrl;                   //!< Pressure control FM
//    CTemperatureControl* m_pTempCtrls[AL_TEMP_CTRL_NUM]; //!< Temperature controls FMs for the heaters
//    //CDigitalOutput* m_pFanDigitalOutput;                 //!< Digital output FM for the fan

//    float m_CurrentPressure;                             //!< Current pressure
//    float m_PressureDrift;                               //!< Current pressure drift
//    qint32 m_WorkingPressurePositive;                    //!< Positive working pressure
//    qint32 m_WorkingPressureNegative;                    //!< Negative working pressure
//    PressureCtrlStatus_t m_TargetPressureCtrlStatus;     //!< Target pressure control status; for verification of action result.
//    PressureCtrlStatus_t m_CurrentPressureCtrlStatus;    //!< Current pressure control status
//    qint64 m_LastGetPressureTime;                        //!< Last time of getting pressure
//    QList<qreal> m_PIDDataList;                          //!< PID parameters list

//    qreal m_CurrentTemperatures[AL_TEMP_CTRL_NUM][MAX_SENSOR_PER_TEMP_CTRL];   //!< Current temperature
//    qreal m_TargetTemperatures[AL_TEMP_CTRL_NUM];                     //!< Current temperature
//    TempCtrlStatus_t m_TargetTempCtrlStatus[AL_TEMP_CTRL_NUM];        //!< Target temperature control status; for verification of action result.
//    TempCtrlStatus_t m_CurrentTempCtrlStatus[AL_TEMP_CTRL_NUM];       //!< Current temperature control status
//    TempCtrlMainsVoltage_t m_MainsVoltageStatus[AL_TEMP_CTRL_NUM];    //!< Mains voltage state of the heaters
//    TempCtrlHardwareStatus_t m_TCHardwareStatus[AL_TEMP_CTRL_NUM];    //!< Hardware status of the heaters
//    quint32 m_SuckingTime[16];                                        //!< in mec, idx 1-13 is port 1-13, idx 0 is unused
//    qint64 m_LastGetTempTime[AL_TEMP_CTRL_NUM][5];                    //!< Last time of getting temperature
//    qint64 m_LastGetTCCurrentTime[AL_TEMP_CTRL_NUM];                  //!< Last time of getting current
//    QMap<quint32, ALTempCtrlType_t> m_InstTCTypeMap;                  //!< Map between instance ID and temperature control

//    qint16 m_TargetDOOutputValue;     //!< Target output value; for verification of action result
//    quint32 m_DOLifeTime;             //!< Digital output life time
//    quint32 m_DOLifeCycles;           //!< Digital output life cycles

//    // rt
//    CTemperatureControl* m_pTempCtrls[RT_TEMP_CTRL_NUM];              //!< Temperature control function modules used in this device
//    CDigitalOutput* m_pLockDigitalOutput;                             //!< Digital output for lock
//    CDigitalInput* m_pLockDigitalInput;                               //!< Digital input for lock
//    TempCtrlHardwareStatus_t m_HardwareStatus[RT_TEMP_CTRL_NUM];      //!< Hardware status for temperature control modules

//    qreal m_CurrentTemperatures[RT_TEMP_CTRL_NUM][MAX_SENSOR_PER_TEMP_CTRL];                  //!< Current temperature
//    qreal m_TargetTemperatures[RT_TEMP_CTRL_NUM];                     //!< Current temperature
//    TempCtrlStatus_t m_TargetTempCtrlStatus[RT_TEMP_CTRL_NUM];        //!< Target temperature control status; for verification of action result.
//    TempCtrlStatus_t m_CurrentTempCtrlStatus[RT_TEMP_CTRL_NUM];       //!< Current temperature control status
//    TempCtrlMainsVoltage_t m_MainsVoltageStatus[RT_TEMP_CTRL_NUM];    //!< Mains voltage state of the heaters
//    qint64 m_LastGetTempTime[RT_TEMP_CTRL_NUM][MAX_SENSOR_PER_TEMP_CTRL];                    //!< Last get temperature time
//    QMap<quint32, RTTempCtrlType_t> m_InstTCTypeMap;                  //!< Map between instance ID and temperature control FMs

//    qint64 m_LastGetLockStatusTime;                                   //!< Last get lock status time
//    qint64 m_LastGetHardwareStatusTime[RT_TEMP_CTRL_NUM];             //!< Last time for getting hardware status
//    qint16 m_TargetDOOutputValue;                                     //!< Target output value; for verification of action result
//    qint16 m_LockStatus;                                              //!< Target output value; for verification of action result


//    //pp
//    CDigitalOutput* m_pDigitalOutputs[PER_DO_TOTAL_NUM]; //!< Digital outputs FMs used in this device
//    CDigitalInput* m_pDigitalInputs[PER_DI_TOTAL_NUM]; //!< Digital inputs FMs used in this device
//    QMap<quint32, PerDOType_t> m_InstDOTypeMap;        //!< Map between instance ID and digital output function modules
//    qint16 m_TargetDOOutputValues[PER_DO_TOTAL_NUM];   //!< Target output value; for verification of action result
//    qint16 m_TargetDIInputValues[PER_DI_TOTAL_NUM];    //!< Target input value; for verification of action result
//    QMap<quint32, PerDIType_t> m_InstDITypeMap;        //!< Map between instance ID and digital input function modules
//    qint64 m_LastGetLocalAlarmStatusTime;              //!< Last get local alarm status time
//    qint64 m_LastGetRemoteAlarmStatusTime;             //!< Last get remote alarm status time

//};

//#endif // DEVICEMODULEMODEL_H

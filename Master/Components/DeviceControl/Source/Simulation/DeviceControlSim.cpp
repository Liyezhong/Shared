#include <DeviceControl/Include/Simulation/DeviceControlSim.h>
#include "DeviceControl/hwconfig/hwconfig-pimpl.hpp"
#include <Global/Include/SystemPaths.h>


namespace DeviceControl
{

DeviceControlSim::DeviceControlSim(int n)
//    : mb_StandBy(false),
//      mb_Stop(false)
    : hwconfigFilename("hw_specification.xml")

{
    CreateDevices();
}

DeviceControlSim::~DeviceControlSim()
{
    delete m_pDeviceConfig;
}

void DeviceControlSim::CreateDevices()
{
    if(m_pDeviceConfig == NULL)
    {
        if(!ReadDeviceConfig())
            return;
    }

    m_deviceList.insert("Retort_A", QVector<CtrlBase*>({new CtrlBase("ALTemp"),
                                                       new CtrlBase("ALPressure"),
                                                       new CtrlBase("ALFan"),
                                                       new CtrlBase("RVTemp"),
                                                       new CtrlBase("RVPos"),
                                                       new CtrlBase("RTTemp"),
                                                       new CtrlBase("RTLock"),
                                                       new CtrlBase("OvernTemp"),
                                                        new CtrlBase("OvenLid"),
                                                        new CtrlBase("RVVolatage"),
                                                        new CtrlBase("ALVolatage"),
                                                        new CtrlBase("OvenVolatage"),
                                                        new CtrlBase("IDAlarm"),
                                                        new CtrlBase("Alarm")
                                                      }));


    m_deviceList.insert("Retort_B", QVector<CtrlBase*>({new CtrlBase("ALTemp"),
                                                       new CtrlBase("ALPressure"),
                                                       new CtrlBase("ALFan"),
                                                       new CtrlBase("RVTemp"),
                                                       new CtrlBase("RVPos"),
                                                       new CtrlBase("RTTemp"),
                                                       new CtrlBase("RTLock"),
                                                       new CtrlBase("OvernTemp"),
                                                        new CtrlBase("OvenLid"),
                                                        new CtrlBase("RVVolatage"),
                                                        new CtrlBase("ALVolatage"),
                                                        new CtrlBase("OvenVolatage"),
                                                        new CtrlBase("IDAlarm"),
                                                        new CtrlBase("Alarm")
                                                      }));
    m_deviceList.insert("Common", QVector<CtrlBase*>({new CtrlBase("ALTemp"),
                                                       new CtrlBase("ALPressure"),
                                                       new CtrlBase("ALFan"),
                                                       new CtrlBase("RVTemp"),
                                                       new CtrlBase("RVPos"),
                                                       new CtrlBase("RTTemp"),
                                                       new CtrlBase("RTLock"),
                                                       new CtrlBase("OvernTemp"),
                                                      new CtrlBase("OvenLid"),
                                                      new CtrlBase("RVVolatage"),
                                                      new CtrlBase("ALVolatage"),
                                                      new CtrlBase("OvenVolatage"),
                                                      new CtrlBase("IDAlarm"),
                                                      new CtrlBase("Alarm")
                                                      }));
}

CtrlBase* DeviceControlSim:: GetSubCtrl(const QString id, const QString device)
{
    CtrlBase* ctrl = nullptr;
    for(auto itor = m_deviceList[id].begin(); itor != m_deviceList[id].end(); itor++)
    {
        if((*itor)->GetName() == device)
        {            
            ctrl = *itor;
            break;
        }
    }
    if(ctrl == nullptr)
        qDebug()<<"DeviceControlSim, RetortName: " + id + "Device: " + device;
    return ctrl;
}

void DeviceControlSim::Standby()
{
    //    mb_StandBy = true;
}

void DeviceControlSim::EmergencyStop()
{
    //    mb_Stop = true;
}

void DeviceControlSim::Destroy()
{

}


ReturnCode_t DeviceControlSim::StartConfigurationService()
{
    qDebug()<<__FUNCTION__;


    emit ReportConfigurationFinished(DEVICE_INSTANCE_ID_DEVPROC, DCL_ERR_FCT_CALL_SUCCESS);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RestartConfigurationService()
{
    qDebug()<<__FUNCTION__;
    emit ReportConfigurationFinished(DEVICE_INSTANCE_ID_DEVPROC, DCL_ERR_FCT_CALL_SUCCESS);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::StartDiagnosticService()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::CloseDiagnosticService()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::StartAdjustmentService()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

CBaseDevice *DeviceControlSim::GetDevice(quint32 InstanceID)
{

}

CBaseModule *DeviceControlSim::GetNode(bool First)
{

}

ReturnCode_t DeviceControlSim::ALSetPressureCtrlON()
{
    qDebug()<<__FUNCTION__;

    GetSubCtrl(m_Sender, "ALPressure")->SetOnOff(true);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALSetPressureCtrlOFF()
{
    GetSubCtrl(m_Sender, "ALPressure")->SetOnOff(false);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALReleasePressure()
{
    GetSubCtrl(m_Sender, "ALPressure")->SetTarget(0);
    GetSubCtrl(m_Sender, "ALPressure")->SetOnOff(true);

    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALPressure(float targetPressure)
{
    GetSubCtrl(m_Sender, "ALPressure")->SetTarget(targetPressure);

    GetSubCtrl(m_Sender, "ALPressure")->SetOnOff(true);

    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALVaccum(float targetPressure)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALDraining(quint32 DelayTime, float targetPressure, bool IgnorePressure)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::IDForceDraining(quint32 RVPos, float targetPressure, const QString &ReagentGrpID)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALFilling(quint32 DelayTime, bool EnableInsufficientCheck, bool SafeReagent4Paraffin)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALStopCmdExec(quint8 CmdType)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALFillingForService(quint32 DelayTime, bool EnableInsufficientCheck)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

qreal DeviceControlSim::ALGetRecentPressure()
{


    return GetSubCtrl(m_Sender, "ALPressure")->GetCurrent();
}

ReturnCode_t DeviceControlSim::ALSetTempCtrlON(ALTempCtrlType_t Type)
{
    GetSubCtrl(m_Sender, "ALTemp")->SetOnOff(true);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALSetTempCtrlOFF(ALTempCtrlType_t type)
{
    GetSubCtrl(m_Sender, "ALTemp")->SetOnOff(false);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALSetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALStartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

qreal DeviceControlSim::ALGetRecentTemperature(ALTempCtrlType_t Type, quint8 Index)
{
    return GetSubCtrl(m_Sender, "ALTemp")->GetCurrent();
}

TempCtrlState_t DeviceControlSim::ALGetTemperatureControlState(ALTempCtrlType_t Type)
{
    return TEMPCTRL_STATE_OFF;
}

ReturnCode_t DeviceControlSim::ALTurnOnFan()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALTurnOffFan()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALAllStop()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALBreakAllOperation()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALSetPressureDrift(qreal pressureDrift)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALStartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::ALControlValve(quint8 ValveIndex, quint8 ValveState)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

bool DeviceControlSim::ALGetHeatingStatus(ALTempCtrlType_t Type)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RVSetTempCtrlON()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RVSetTempCtrlOFF()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RVSetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RVStartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange)
{
    GetSubCtrl(m_Sender, "RVTemp")->SetOnOff(true);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

qreal DeviceControlSim::RVGetRecentTemperature(quint32 Index)
{
    return GetSubCtrl(m_Sender, "RVTemp")->GetCurrent();
}

TempCtrlState_t DeviceControlSim::RVGetTemperatureControlState()
{
    if(GetSubCtrl(m_Sender, "RVTemp")->GetStatus())
    {
        return TEMPCTRL_STATE_INSIDE_RANGE;
    }

    return TEMPCTRL_STATE_OFF;
}

ReturnCode_t DeviceControlSim::RVReqMoveToInitialPosition(RVPosition_t RVPosition)
{
    auto ctrl = static_cast<RVPosCtrl*>(GetSubCtrl(m_Sender, "RVTemp"));
    ctrl->SetPos(RVPosition);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RVReqMoveToRVPosition(RVPosition_t RVPosition)
{
    auto ctrl = static_cast<RVPosCtrl*>(GetSubCtrl(m_Sender, "RVTemp"));
    ctrl->SetPos(RVPosition);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

RVPosition_t DeviceControlSim::RVReqActRVPosition()
{
    auto ctrl = static_cast<RVPosCtrl*>(GetSubCtrl(m_Sender, "RVTemp"));
    return ctrl->GetRVPos();
//    return RV_TUBE_1;
}

ReturnCode_t DeviceControlSim::RVReqMoveToCurrentTubePosition(RVPosition_t CurrentRVPosition)
{
    auto ctrl = static_cast<RVPosCtrl*>(GetSubCtrl(m_Sender, "RVTemp"));
    ctrl->SetPos(CurrentRVPosition);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

quint32 DeviceControlSim::GetCurrentLowerLimit()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RVStartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RVSetTemperatureSwitchState(qint8 HeaterVoltage, qint8 AutoType)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::OvenSetTempCtrlON(OVENTempCtrlType_t Type)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::OvenSetTempCtrlOFF(OVENTempCtrlType_t type)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::OvenSetTemperaturePid(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::OvenStartTemperatureControl(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

qreal DeviceControlSim::OvenGetRecentTemperature(OVENTempCtrlType_t Type, quint8 Index)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

bool DeviceControlSim::OvenGetHeatingStatus(OVENTempCtrlType_t Type)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

quint16 DeviceControlSim::OvenGetRecentLidStatus()
{
    return GetSubCtrl(m_Sender, "OvenLid")->GetCurrent();
}

TempCtrlState_t DeviceControlSim::OvenGetTemperatureControlState(OVENTempCtrlType_t Type)
{
    return TEMPCTRL_STATE_OFF;
}

ReturnCode_t DeviceControlSim::OvenStartTemperatureControlWithPID(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RTSetTempCtrlON(RTTempCtrlType_t Type)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RTSetTempCtrlOFF(RTTempCtrlType_t Type)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RTSetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RTStartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RTStartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

qreal DeviceControlSim::RTGetRecentTemperature(RTTempCtrlType_t Type, quint8 Index)
{
    return GetSubCtrl(m_Sender, "RTTemp")->GetCurrent();
}

TempCtrlState_t DeviceControlSim::RTGetTemperatureControlState(RTTempCtrlType_t Type)
{
    return TEMPCTRL_STATE_OFF;
}

ReturnCode_t DeviceControlSim::RTUnlock()
{
    GetSubCtrl(m_Sender, "RTLock")->SetOnOff(false);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RTLock()
{
    GetSubCtrl(m_Sender, "RTLock")->SetOnOff(true);
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::RTSetTemperatureSwitchState(RTTempCtrlType_t Type, qint8 HeaterVoltage, qint8 AutoType)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

quint16 DeviceControlSim::RTGetRecentLockStatus()
{
    return 1; // always closed
}

ReturnCode_t DeviceControlSim::PerTurnOffMainRelay()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::PerTurnOnMainRelay()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::PerControlAlarm(bool On, bool Remote)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

quint16 DeviceControlSim::PerGetRecentAlarmStatus(qint8 type)
{
    return GetSubCtrl(m_Sender, "Alarm")->GetCurrent();
}

ReturnCode_t DeviceControlSim::IDBottleCheck(QString ReagentGrpID, RVPosition_t TubePos)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::IDSealingCheck(qreal ThresholdPressure, RVPosition_t SealPosition)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

CFunctionModule *DeviceControlSim::GetFunctionModuleRef(quint32 InstanceID, const QString &Key)
{
    return NULL;
}

quint16 DeviceControlSim::IDGetSlaveCurrent(HimSlaveType_t Type)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

quint16 DeviceControlSim::IDGetSlaveVoltage(HimSlaveType_t Type)
{
    quint16 voltage = UNDEFINED_2_BYTE;
    switch(Type)
    {
    case Slave_3:
        return GetSubCtrl(m_Sender, "RVVolatage")->GetCurrent();

    case Slave_5:
        return GetSubCtrl(m_Sender, "OvenVolatage")->GetCurrent();
    case Slave_15:
        return GetSubCtrl(m_Sender, "ALVolatage")->GetCurrent();
    }
    return voltage;
}

CBaseModule *DeviceControlSim::GetBaseModule(HimSlaveType_t Type)
{
    return NULL;
}

quint16 DeviceControlSim::IDGetRemoteAlarmStatus()
{
    return GetSubCtrl(m_Sender, "IDAlarm")->GetCurrent();
}

quint16 DeviceControlSim::IDGetLocalAlarmStatus()
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

ReturnCode_t DeviceControlSim::IDSetAlarm(qint8 opcode)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

void DeviceControlSim::InitArchiveServiceInforState()
{

}

void DeviceControlSim::ArchiveServiceInfor()
{

}

void DeviceControlSim::NotifySavedServiceInfor(const QString &deviceType)
{

}

void DeviceControlSim::ResetActiveCarbonFilterLifeTime(quint32 setVal)
{

}

bool DeviceControlSim::GetCurrentVoltageStatus()
{

}

ReportError_t DeviceControlSim::GetSlaveModuleReportError(quint8 errorCode, const QString &devName, quint32 sensorName)
{
    return ReportError_t();
}

quint16 DeviceControlSim::GetSensorCurrent(const QString &DevName, quint8 Index)
{
    return DCL_ERR_FCT_CALL_SUCCESS;
}

quint8 DeviceControlSim::GetHeaterSwitchType(const QString &DevName)
{return DCL_ERR_FCT_CALL_SUCCESS;

}

int DeviceControlSim::DelaySomeTime(int DelayTime)
{
    return 0;
}

void DeviceControlSim::OnError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp)
{

}

void DeviceControlSim::OnInitializationFinished(ReturnCode_t)
{

}

void DeviceControlSim::OnConfigurationFinished(ReturnCode_t)
{

}

void DeviceControlSim::OnStartNormalOperationMode(ReturnCode_t)
{

}

void DeviceControlSim::OnErrorWithInfo(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo)
{

}

void DeviceControlSim::OnDiagnosticServiceClosed(qint16 DiagnosticResult)
{

}

void DeviceControlSim::OnDestroyFinished()
{

}

void DeviceControlSim::OnTimeOutSaveServiceInfor()
{

}

void DeviceControlSim::OnTimeOutSaveLifeCycleRecord()
{

}

void DeviceControlSim::Start()
{
    emit ReportInitializationFinished(DEVICE_INSTANCE_ID_DEVPROC, DCL_ERR_FCT_CALL_SUCCESS);
}


hwconfigType* DeviceControlSim::GetDeviceConfig() const
{   
   return m_pDeviceConfig;
}

bool DeviceControlSim::ReadDeviceConfig()
{
    using xml_schema::parser_error;
    parser_error e;
    hwconfig_paggr hw_p;
    xml_schema::document_pimpl hw_doc_p(hw_p.root_parser(),hw_p.root_name());
    if((e = hw_doc_p._error()))
    {
        return false;

    }
    hw_p.pre();
    if((e = hw_p._error()))
    {
        return false;
    }

    auto filename = Global::SystemPaths::Instance().GetSettingsPath()+ "/";
    filename += hwconfigFilename;
    QFile file(filename);

    if(!file.exists())
    {
        qDebug() << "File: " + filename + "not exist";
        return false;
    }

    QByteArray reader;
    if(file.open(QFile::ReadOnly))
    {
        reader = file.readAll();
        qDebug() << reader;
    }
    else
    {
        return false;
    }

    hw_doc_p.parse(filename.toStdString());
    if((e = hw_doc_p._error()))
    {
        return false;
    }
    m_pDeviceConfig = hw_p.post();
    if((e = hw_p._error()))
    {
        return false;

    };

    return true;
}
};


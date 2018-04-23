#ifndef IDEVICECONTROLSIM_H
#define IDEVICECONTROLSIM_H
#include "DeviceControl/Include/Interface/IDeviceControl.h"
#include "functional"

namespace DeviceControl {

class CtrlBase
{
public:
    CtrlBase(QString name)
        : m_strName(name)
        , m_bOn(false)
        , m_Current(0)
        , m_Target(0)
        , m_Index(0)
        , m_bDisposed(false)
    {
        m_ValueList.clear();
    }

    ~CtrlBase()
    {
        Dispose();
    }

    void Dispose()
    {
        if(!m_bDisposed)
        {
            std::for_each(m_ValueList.begin(), m_ValueList.end(), [](ValueNode* node)
                {
                    if(node != nullptr)
                    {
                        delete node;
                    }
                }
            );
            m_ValueList.clear();

            m_bDisposed = true;
        }
    }

    QString GetName() const{return m_strName;}

    bool IsOnOff() const {return m_bOn;}

    typedef struct{
        QDateTime timestamp;
        qreal value;
    }ValueNode;

    typedef std::function<void(qreal, qreal, qreal, QVector<ValueNode*>*)> fUpdate;

    static void Update1(qreal current, qreal target, qreal factor, QVector<ValueNode*>* valueList)
    {
        if(current < target*(1- factor))
        {
            ValueNode* data = new ValueNode{QDateTime::currentDateTime(), target};
            valueList->append(data);
            return;
        }

        for(auto d = current; d < target; d += (target-current)*factor)
        {
            ValueNode* data = new ValueNode{QDateTime::currentDateTime(), d};
            valueList->append(data);
        }
    }

    void SetOnOff(bool on)
    {
        if(on)
        {
          /*  // load fake data

            auto f = std::bind(&CtrlBase::Update1, m_Current, m_Target, 0.2, &m_ValueList);
            f()*/;
        }
        else
        {
            m_Current = 0;
            m_Index = 0;
        }
        m_bOn = on;
    }

    bool GetStatus() const {return m_bOn;}

    void SetTarget(qreal target)
    {
        m_Target = target;
        m_Current = 0;
        m_Index = 0;


        auto f = std::bind(&CtrlBase::Update1, m_Current, m_Target, 0.2, &m_ValueList);
        f();
    }

    qreal GetCurrent()
    {
        if(m_bDisposed)
            return 0;       

        if (!m_ValueList.isEmpty())
        {
            m_Current = m_ValueList[m_Index]->value;
        }

        if ( (m_Index < m_ValueList.count()-1) && !m_ValueList.isEmpty())
        {
            m_Index++;
        }

        return m_Current;
    }

private:
    CtrlBase(const CtrlBase &);
    CtrlBase& operator = (const CtrlBase &);
    QString m_strName;
    bool m_bOn;
    qreal m_Current;
    qreal m_Target;
    int m_Index;
    QVector<ValueNode*> m_ValueList;
    bool m_bDisposed;
};

class RVPosCtrl: public CtrlBase
{
public:
    RVPosCtrl()
        :CtrlBase("RVPos")
    {
    }

    RVPosition_t GetRVPos()const {return m_CurrentPos;}
    void SetPos(RVPosition_t pos)
    {
        m_TargetPos = pos;
        m_CurrentPos = m_TargetPos;
    }

private:
    RVPosition_t m_CurrentPos;
    RVPosition_t m_TargetPos;
};
    typedef QSharedPointer<CtrlBase> CtrlBaseShPtr_t;
class DeviceControlSim: public QObject, public IDeviceControl
{
    Q_OBJECT
    // IDeviceControl interface

public:
    DeviceControlSim(int n);
    ~DeviceControlSim();
    hwconfigType * GetDeviceConfig() const override;
    void EmergencyStop() override;
    void Standby() override;
    void Destroy() override;
    ReturnCode_t StartConfigurationService() override;
    ReturnCode_t RestartConfigurationService() override;
    ReturnCode_t StartDiagnosticService() override;
    ReturnCode_t CloseDiagnosticService() override;
    ReturnCode_t StartAdjustmentService() override;
    CBaseDevice *GetDevice(quint32 InstanceID) override;
    CBaseModule *GetNode(bool First) override;
    ReturnCode_t ALSetPressureCtrlON() override;
    ReturnCode_t ALSetPressureCtrlOFF() override;
    ReturnCode_t ALReleasePressure() override;
    ReturnCode_t ALPressure(float targetPressure) override;
    ReturnCode_t ALVaccum(float targetPressure) override;
    ReturnCode_t ALDraining(quint32 DelayTime, float targetPressure, bool IgnorePressure) override;
    ReturnCode_t IDForceDraining(quint32 RVPos, float targetPressure, const QString &ReagentGrpID) override;
    ReturnCode_t ALFilling(quint32 DelayTime, bool EnableInsufficientCheck, bool SafeReagent4Paraffin) override;
    ReturnCode_t ALStopCmdExec(quint8 CmdType) override;
    ReturnCode_t ALFillingForService(quint32 DelayTime, bool EnableInsufficientCheck) override;
    qreal ALGetRecentPressure() override;
    ReturnCode_t ALSetTempCtrlON(ALTempCtrlType_t Type) override;
    ReturnCode_t ALSetTempCtrlOFF(ALTempCtrlType_t type) override;
    ReturnCode_t ALSetTemperaturePid(ALTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) override;
    ReturnCode_t ALStartTemperatureControl(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange) override;
    qreal ALGetRecentTemperature(ALTempCtrlType_t Type, quint8 Index) override;
    TempCtrlState_t ALGetTemperatureControlState(ALTempCtrlType_t Type) override;
    ReturnCode_t ALTurnOnFan() override;
    ReturnCode_t ALTurnOffFan() override;
    ReturnCode_t ALAllStop() override;
    ReturnCode_t ALBreakAllOperation() override;
    ReturnCode_t ALSetPressureDrift(qreal pressureDrift) override;
    ReturnCode_t ALStartTemperatureControlWithPID(ALTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) override;
    ReturnCode_t ALControlValve(quint8 ValveIndex, quint8 ValveState) override;
    bool ALGetHeatingStatus(ALTempCtrlType_t Type) override;
    ReturnCode_t RVSetTempCtrlON() override;
    ReturnCode_t RVSetTempCtrlOFF() override;
    ReturnCode_t RVSetTemperaturePid(quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) override;
    ReturnCode_t RVStartTemperatureControl(qreal NominalTemperature, quint8 SlopeTempChange) override;
    qreal RVGetRecentTemperature(quint32 Index) override;
    TempCtrlState_t RVGetTemperatureControlState() override;
    ReturnCode_t RVReqMoveToInitialPosition(RVPosition_t RVPosition) override;
    ReturnCode_t RVReqMoveToRVPosition(RVPosition_t RVPosition) override;
    RVPosition_t RVReqActRVPosition() override;
    ReturnCode_t RVReqMoveToCurrentTubePosition(RVPosition_t CurrentRVPosition) override;
    quint32 GetCurrentLowerLimit() override;
    ReturnCode_t RVStartTemperatureControlWithPID(qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) override;
    ReturnCode_t RVSetTemperatureSwitchState(qint8 HeaterVoltage, qint8 AutoType) override;
    ReturnCode_t OvenSetTempCtrlON(OVENTempCtrlType_t Type) override;
    ReturnCode_t OvenSetTempCtrlOFF(OVENTempCtrlType_t type) override;
    ReturnCode_t OvenSetTemperaturePid(OVENTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) override;
    ReturnCode_t OvenStartTemperatureControl(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange) override;
    qreal OvenGetRecentTemperature(OVENTempCtrlType_t Type, quint8 Index) override;
    bool OvenGetHeatingStatus(OVENTempCtrlType_t Type) override;
    quint16 OvenGetRecentLidStatus() override;
    TempCtrlState_t OvenGetTemperatureControlState(OVENTempCtrlType_t Type) override;
    ReturnCode_t OvenStartTemperatureControlWithPID(OVENTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) override;
    ReturnCode_t RTSetTempCtrlON(RTTempCtrlType_t Type) override;
    ReturnCode_t RTSetTempCtrlOFF(RTTempCtrlType_t Type) override;
    ReturnCode_t RTSetTemperaturePid(RTTempCtrlType_t Type, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) override;
    ReturnCode_t RTStartTemperatureControl(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange) override;
    ReturnCode_t RTStartTemperatureControlWithPID(RTTempCtrlType_t Type, qreal NominalTemperature, quint8 SlopeTempChange, quint16 MaxTemperature, quint16 ControllerGain, quint16 ResetTime, quint16 DerivativeTime) override;
    qreal RTGetRecentTemperature(RTTempCtrlType_t Type, quint8 Index) override;
    TempCtrlState_t RTGetTemperatureControlState(RTTempCtrlType_t Type) override;
    ReturnCode_t RTUnlock() override;
    ReturnCode_t RTLock() override;
    ReturnCode_t RTSetTemperatureSwitchState(RTTempCtrlType_t Type, qint8 HeaterVoltage, qint8 AutoType) override;
    quint16 RTGetRecentLockStatus() override;
    ReturnCode_t PerTurnOffMainRelay() override;
    ReturnCode_t PerTurnOnMainRelay() override;
    ReturnCode_t PerControlAlarm(bool On, bool Remote) override;
    quint16 PerGetRecentAlarmStatus(qint8 type) override;
    ReturnCode_t IDBottleCheck(QString ReagentGrpID, RVPosition_t TubePos) override;
    ReturnCode_t IDSealingCheck(qreal ThresholdPressure, RVPosition_t SealPosition) override;
    CFunctionModule *GetFunctionModuleRef(quint32 InstanceID, const QString &Key) override;
    quint16 IDGetSlaveCurrent(HimSlaveType_t Type) override;
    quint16 IDGetSlaveVoltage(HimSlaveType_t Type) override;
    CBaseModule *GetBaseModule(HimSlaveType_t Type) override;
    quint16 IDGetRemoteAlarmStatus() override;
    quint16 IDGetLocalAlarmStatus() override;
    ReturnCode_t IDSetAlarm(qint8 opcode) override;
    void InitArchiveServiceInforState() override;
    void ArchiveServiceInfor() override;
    void NotifySavedServiceInfor(const QString &deviceType) override;
    void ResetActiveCarbonFilterLifeTime(quint32 setVal) override;
    bool GetCurrentVoltageStatus() override;
    ReportError_t GetSlaveModuleReportError(quint8 errorCode, const QString &devName, quint32 sensorName) override;
    quint16 GetSensorCurrent(const QString &DevName, quint8 Index) override;
    quint8 GetHeaterSwitchType(const QString &DevName) override;
    int DelaySomeTime(int DelayTime) override;

signals:
    void ReportInitializationFinished(quint32, ReturnCode_t) override;
    void ReportConfigurationFinished(quint32, ReturnCode_t) override;
    void ReportStartNormalOperationMode(quint32, ReturnCode_t) override;
    void ReportError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp) override;
    void ReportErrorWithInfo(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo) override;
    void ReportDestroyFinished() override;
    void DeviceShutdown() override;
    void ReportLevelSensorStatus1() override;
    void ReportFillingTimeOut2Min() override;
    void ReportDrainingTimeOut2Min() override;
    void ReportGetServiceInfo(ReturnCode_t ReturnCode, const DataManager::CModule &ModuleInfo, const QString &) override;
    void ReportSavedServiceInfor(const QString &deviceType) override;

public slots:
    void OnError(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp) override;
    void OnInitializationFinished(ReturnCode_t) override;
    void OnConfigurationFinished(ReturnCode_t) override;
    void OnStartNormalOperationMode(ReturnCode_t) override;
    void OnErrorWithInfo(quint32 instanceID, quint16 usErrorGroup, quint16 usErrorID, quint16 usErrorData, QDateTime timeStamp, QString strErrorInfo) override;
    void OnDiagnosticServiceClosed(qint16 DiagnosticResult) override;
    void OnDestroyFinished() override;
    void OnTimeOutSaveServiceInfor() override;
    void OnTimeOutSaveLifeCycleRecord() override;

private:    
    void CreateDevices();
    CtrlBase* GetSubCtrl(const QString id, const QString device);
    bool ReadDeviceConfig();

    QMap<QString, QVector<CtrlBase*>> m_deviceList;
    const QString hwconfigFilename;

public:
    void Start();
};

}

#endif

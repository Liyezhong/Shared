#include <QTest>
#include <QSignalSpy>
#include <QDebug>

#include "DeviceControl/Include/Devices/DeviceLight.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl {

enum FM_INSTANCE_ID {
    FM_DIGITAL_OUTPUT = 1
};

class TestLight : public QObject {
    Q_OBJECT
public:
    TestLight();
    ~TestLight();

private slots:
    void init();
    void cleanup();

    void utTestLight();
    void utTestLightSignals();
private: 
    // Device
    CDeviceLight *mp_Light;

    // Function Modules
    CDigitalOutput mp_FmDoLed;

    // Device Processing
    DeviceProcessing m_DeviceProcessing;
    DeviceModuleList_t m_DevFctModList;

    // Device Command Processor
    CMockDcp m_MockDcp;
};

TestLight::TestLight() : mp_Light(NULL)
{
    quint32 FctInstanceID;

    FctInstanceID = FM_DIGITAL_OUTPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_DrawerLEDBlockedKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &mp_FmDoLed);
}
    
   
TestLight::~TestLight()
{
}

void TestLight::init() 
{  
    mp_FmDoLed.Reset();
    mp_Light = new CDeviceLight(m_DeviceProcessing, m_DevFctModList, DEVICE_INSTANCE_ID_LIGHT);

    if (!connect(&m_MockDcp, SIGNAL(Configure()), 
                  mp_Light, SIGNAL(Configure()))) {
        QFAIL("Cannot connect Configure signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Initialize()), 
                  mp_Light, SIGNAL(Initialize()))) {
        QFAIL("Cannot connect Initialize signal.");
    }
     if (!connect(&m_MockDcp, SIGNAL(SetLightOn()),
                  mp_Light, SIGNAL(SetLightOn()))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(SetLightOff()),
                  mp_Light, SIGNAL(SetLightOff()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    // give instantiated state machine a chance to start and process events
    QCoreApplication::processEvents();

    THREAD_PROCESS();
}

void TestLight::cleanup() 
{  
    delete mp_Light;
}

void TestLight::utTestLight()
{
//    //  flow: Start-> LightOff -> LightOn ->LightOff
    QSignalSpy SpyLightOn(mp_Light, SIGNAL( LightOn(ReturnCode_t)));
    QSignalSpy SpyLightOff(mp_Light, SIGNAL( LightOff(ReturnCode_t)));

    QVERIFY( mp_Light->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Light->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Light->GetStates().contains("LightOff") );

    m_MockDcp.EmitSetLightOn();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOn") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_SUCCESS,1);
    QVERIFY( mp_Light->GetStates().contains("LightOn") );
    QCOMPARE( SpyLightOn.count(), 1 );

    m_MockDcp.EmitSetLightOff();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOff") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_SUCCESS,0);
    QVERIFY( mp_Light->GetStates().contains("LightOff") );
    QCOMPARE( SpyLightOff.count(), 1 );

 }

void TestLight::utTestLightSignals()
{
    //  flow: Start-> LightOff -> LightOn -> LightOff-> LightOn -> LightOff->

    QSignalSpy SpyLightOn(mp_Light, SIGNAL( LightOn(ReturnCode_t)));
    QSignalSpy SpyLightOff(mp_Light, SIGNAL( LightOff(ReturnCode_t)));
    QSignalSpy SpyNeedInitialize(mp_Light, SIGNAL( NeedInitialize(ReturnCode_t)));

    QVERIFY( mp_Light->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Light->GetStates().contains("Configured") );

    QCOMPARE( SpyNeedInitialize.count(), 0 );
    QCOMPARE( SpyLightOff.count(), 0 );
    QCOMPARE( SpyLightOn.count(), 0 );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Light->GetStates().contains("LightOff") );

    m_MockDcp.EmitSetLightOn();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOn") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_SUCCESS,1);
    QVERIFY( mp_Light->GetStates().contains("LightOn") );
    QCOMPARE( SpyLightOn.count(), 1 );

    m_MockDcp.EmitSetLightOff();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOff") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_SUCCESS,0);
    QVERIFY( mp_Light->GetStates().contains("LightOff") );
    QCOMPARE( SpyLightOff.count(), 1 );

    m_MockDcp.EmitSetLightOn();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOn") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_SUCCESS,1);
    QVERIFY( mp_Light->GetStates().contains("LightOn") );
    QCOMPARE( SpyLightOn.count(), 2 );

    m_MockDcp.EmitSetLightOff();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOff") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_SUCCESS,0);
    QVERIFY( mp_Light->GetStates().contains("LightOff") );
    QCOMPARE( SpyLightOff.count(), 2 );

    m_MockDcp.EmitSetLightOn();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOn") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_FAILED,1);
    QCOMPARE( SpyLightOn.count(), 2 );
    QCOMPARE( SpyNeedInitialize.count(), 1 );

    QVERIFY( mp_Light->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Light->GetStates().contains("LightOff") );

    m_MockDcp.EmitSetLightOn();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOn") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_SUCCESS,1);
    QVERIFY( mp_Light->GetStates().contains("LightOn") );
    QCOMPARE( SpyLightOn.count(), 3 );

    m_MockDcp.EmitSetLightOff();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOff") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_FAILED,0);
    QCOMPARE( SpyLightOff.count(), 2 );
    QCOMPARE( SpyNeedInitialize.count(), 2 );
    QVERIFY( mp_Light->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Light->GetStates().contains("LightOff") );

    m_MockDcp.EmitSetLightOn();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOn") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_SUCCESS,1);
    QVERIFY( mp_Light->GetStates().contains("LightOn") );
    QCOMPARE( SpyLightOn.count(), 4 );

    m_MockDcp.EmitSetLightOff();
    QVERIFY( mp_Light->GetStates().contains("SwitchingOff") );

    mp_FmDoLed.EmitReportActOutputValue(1234,DCL_ERR_FCT_CALL_SUCCESS,0);
    QVERIFY( mp_Light->GetStates().contains("LightOff") );
    QCOMPARE( SpyLightOff.count(), 3 );
}

} // end namespace DeviceControl

QTEST_MAIN(DeviceControl::TestLight)

#include "TestLight.moc"

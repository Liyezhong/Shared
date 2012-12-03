#include <QTest>
#include <QSignalSpy>
#include <QDebug>

#include "DeviceControl/Include/Devices/DeviceWater.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"
#include "FunctionModule.h"

namespace DeviceControl {
#define MAX_WATER_VALVE_COUNT 6

enum FM_INSTANCE_ID {
    FM_DIGITAL_OUTPUT = 1,
    FM_DIGITAL_INPUT
};

class TestWater : public QObject {
    Q_OBJECT
public:
    TestWater();
    ~TestWater();

private slots:
    void init();
    void cleanup();
    void utTestWater();
    void utTestLiquidLevel();
private: 
    // Device
    CDeviceWater *mp_Water;

    // Function Modules
    CDigitalOutput m_pWaterValve[MAX_WATER_VALVE_COUNT];   //!< 6* water valve
    CDigitalInput m_pLiquidLevel;

    // Device Processing
    DeviceProcessing m_DeviceProcessing;
    DeviceModuleList_t m_DevFctModList;

    // Device Command Processor
    CMockDcp m_MockDcp;
};

TestWater::TestWater() : mp_Water(NULL)
{
    quint32 FctInstanceID;


    FctInstanceID = FM_DIGITAL_OUTPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_WaterValve1Key, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_pWaterValve[0]);

    FctInstanceID = FM_DIGITAL_OUTPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_WaterValve2Key, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_pWaterValve[1]);

    FctInstanceID = FM_DIGITAL_OUTPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_WaterValve3Key, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_pWaterValve[2]);

    FctInstanceID = FM_DIGITAL_OUTPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_WaterValve4Key, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_pWaterValve[3]);

    FctInstanceID = FM_DIGITAL_OUTPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_WaterValve5Key, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_pWaterValve[4]);

    FctInstanceID = FM_DIGITAL_OUTPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_WaterValve6Key, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_pWaterValve[5]);

    FctInstanceID = FM_DIGITAL_INPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_WaterLiquidLevelKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_pLiquidLevel);
}
    
   
TestWater::~TestWater()
{
}

void TestWater::init() 
{  
    m_pWaterValve[0].Reset();
    m_pWaterValve[1].Reset();
    m_pWaterValve[2].Reset();
    m_pWaterValve[3].Reset();
    m_pWaterValve[4].Reset();
    m_pWaterValve[5].Reset();

    m_pLiquidLevel.Reset();

    mp_Water = new CDeviceWater(m_DeviceProcessing, m_DevFctModList, DEVICE_INSTANCE_ID_WATER);

    if (!connect(&m_MockDcp, SIGNAL(Configure()),
                  mp_Water, SIGNAL(Configure()))) {
        QFAIL("Cannot connect Configure signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Initialize()),
                  mp_Water, SIGNAL(Initialize()))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(SetValveOn(WaterValveID_t)),
                   mp_Water, SLOT(SetValveOn(WaterValveID_t)))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(SetValveOff(WaterValveID_t)),
                   mp_Water, SLOT(SetValveOff(WaterValveID_t)))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(ReadLiquidLevel()),
                   mp_Water, SLOT(ReadLiquidLevel()))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_pWaterValve[0], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                   mp_Water, SLOT(SetValveAckn(quint32 , ReturnCode_t , quint16 )))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_pWaterValve[1], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                   mp_Water, SLOT(SetValveAckn(quint32 , ReturnCode_t , quint16 )))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_pWaterValve[2], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                   mp_Water, SLOT(SetValveAckn(quint32 , ReturnCode_t , quint16 )))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_pWaterValve[3], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                   mp_Water, SLOT(SetValveAckn(quint32 , ReturnCode_t , quint16 )))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_pWaterValve[4], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                   mp_Water, SLOT(SetValveAckn(quint32 , ReturnCode_t , quint16 )))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_pWaterValve[5], SIGNAL(ReportOutputValueAckn(quint32, ReturnCode_t, quint16)),
                   mp_Water, SLOT(SetValveAckn(quint32 , ReturnCode_t , quint16 )))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_pLiquidLevel, SIGNAL(ReportActInputValue(quint32, ReturnCode_t, quint16)),
                   mp_Water, SLOT(LiquidLevelStatus(quint32 , ReturnCode_t , quint16 )))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    // give instantiated state machine a chance to start and process events
    QCoreApplication::processEvents();

    THREAD_PROCESS();
}

void TestWater::cleanup() 
{  
    delete mp_Water;
}

void TestWater::utTestWater()
{
//    //  flow: Start-> ValveOn -> ValveOff,
    QSignalSpy SpyValveOn(mp_Water, SIGNAL( ReportSetValveOn(ReturnCode_t,quint8)));
    QSignalSpy SpyValveOff(mp_Water, SIGNAL( ReportSetValveOff(ReturnCode_t,quint8)));

    QVERIFY( mp_Water->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    m_MockDcp.EmitInitialize();

    m_MockDcp.EmitSetValveOn(WATER_VALVE_ID_1);
    m_pWaterValve[0].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    QCOMPARE( SpyValveOn.count(),1);

    m_MockDcp.EmitSetValveOn(WATER_VALVE_ID_2);
    m_pWaterValve[1].EmitReportActOutputValue(1235, DCL_ERR_FCT_CALL_SUCCESS, 1);
    QCOMPARE( SpyValveOn.count(),2);

    m_MockDcp.EmitSetValveOn(WATER_VALVE_ID_3);
    m_pWaterValve[2].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    QCOMPARE( SpyValveOn.count(),3);

    m_MockDcp.EmitSetValveOn(WATER_VALVE_ID_4);
    m_pWaterValve[3].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    QCOMPARE( SpyValveOn.count(),4);

    m_MockDcp.EmitSetValveOn(WATER_VALVE_ID_5);
    m_pWaterValve[4].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    QCOMPARE( SpyValveOn.count(),5);

    m_MockDcp.EmitSetValveOn(WATER_VALVE_ID_6);
    m_pWaterValve[5].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    QCOMPARE( SpyValveOn.count(),12);


    m_MockDcp.EmitSetValveOff(WATER_VALVE_ID_1);
    m_pWaterValve[0].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCOMPARE( SpyValveOff.count(),1);

    m_MockDcp.EmitSetValveOff(WATER_VALVE_ID_2);
    m_pWaterValve[1].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCOMPARE( SpyValveOff.count(),2);

    m_MockDcp.EmitSetValveOff(WATER_VALVE_ID_3);
    m_pWaterValve[2].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCOMPARE( SpyValveOff.count(),3);

    m_MockDcp.EmitSetValveOff(WATER_VALVE_ID_4);
    m_pWaterValve[3].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCOMPARE( SpyValveOff.count(),4);

    m_MockDcp.EmitSetValveOff(WATER_VALVE_ID_5);
    m_pWaterValve[4].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCOMPARE( SpyValveOff.count(),5);

    m_MockDcp.EmitSetValveOff(WATER_VALVE_ID_6);
    m_pWaterValve[5].EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCOMPARE( SpyValveOff.count(),12);



 }
void TestWater::utTestLiquidLevel()
{
    // Read Liquid Level. Report->Block
    QSignalSpy SpyLiquidLevel(mp_Water, SIGNAL( ReportLiquidLevel(ReturnCode_t, bool))) ;
    QSignalSpy SpyLiquidBlock(mp_Water, SIGNAL( WaterBlockage()));

    QVERIFY( mp_Water->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    m_MockDcp.EmitInitialize();

    m_MockDcp.EmitReadLiquidLevel();
    m_pLiquidLevel.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_pLiquidLevel.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    QCOMPARE( SpyLiquidLevel.count(),1);
    QCOMPARE( SpyLiquidBlock.count(),2);

}

} // end namespace DeviceControl

QTEST_MAIN(DeviceControl::TestWater)

#include "TestWater.moc"

#include <QTest>
#include <QSignalSpy>
#include <QDebug>

#include "DeviceControl/Include/Devices/DeviceDrawer.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl {

enum FM_INSTANCE_ID {
    FM_STEPPER_MOTOR = 1,
    FM_DIGITAL_INPUT,
    FM_DIGITAL_OUTPUT_1,
    FM_DIGITAL_OUTPUT_2,
    FM_RFID_11785
};

class TestDrawer : public QObject {
    Q_OBJECT
public:
    TestDrawer();
    ~TestDrawer();

private slots:
    void init();
    void cleanup();

    void utTest();
    void utTestBase();
    void utTestError();
    void utTestDrawerMotion();

private:
    // Device
    CDeviceDrawer *mp_Drawer;

    // Function Modules
    CStepperMotor m_Motor;
    CDigitalInput m_Button;
    CDigitalOutput m_LedFree;
    CDigitalOutput m_LedBlocked;
    CRfid11785 m_Rfid11785;

    // Device Processing
    DeviceProcessing m_DeviceProcessing;
    DeviceModuleList_t m_DevFctModList;

    // Device Command Processor
    CMockDcp m_MockDcp;
};

TestDrawer::TestDrawer() : mp_Drawer(NULL)
{
    quint32 FctInstanceID;

    FctInstanceID = FM_STEPPER_MOTOR;
    m_DevFctModList.insert(CANObjectKeyLUT::m_MotorDrawerKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_Motor);

    FctInstanceID = FM_DIGITAL_INPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_DrawerButtonKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_Button);

    FctInstanceID = FM_DIGITAL_OUTPUT_1;
    m_DevFctModList.insert(CANObjectKeyLUT::m_DrawerLEDFreeKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_LedFree);

    FctInstanceID = FM_DIGITAL_OUTPUT_2;
    m_DevFctModList.insert(CANObjectKeyLUT::m_DrawerLEDBlockedKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_LedBlocked);

    FctInstanceID = FM_RFID_11785;
    m_DevFctModList.insert(CANObjectKeyLUT::m_DrawerRFIDKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_Rfid11785);
}

TestDrawer::~TestDrawer()
{
}

void TestDrawer::init()
{  
    m_Motor.Reset();
    m_Button.Reset();
    m_LedFree.Reset();
    m_LedBlocked.Reset();
    m_Rfid11785.Reset();

    mp_Drawer = new CDeviceDrawer(m_DeviceProcessing, m_DevFctModList, DEVICE_INSTANCE_ID_LOADER);

    if (!connect(&m_MockDcp, SIGNAL(Configure()), 
                  mp_Drawer, SIGNAL(Configure()))) {
        QFAIL("Cannot connect Configure signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Initialize()), 
                  mp_Drawer, SIGNAL(Initialize()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(ReadRfid(LoaderRFIDChannel_t)), 
                  mp_Drawer, SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)))) {
        QFAIL("Cannot connect ReadRfid signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Block()), 
                  mp_Drawer, SIGNAL(Block()))) {
        QFAIL("Cannot connect Block signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Unblock()), 
                  mp_Drawer, SIGNAL(Unblock()))) {
        QFAIL("Cannot connect Unblock signal.");
    }

    // give instantiated state machine a chance to start and process events
    QCoreApplication::processEvents();

    // give some time for the thread to start
    usleep(100*1000);
}

void TestDrawer::cleanup()
{  
    m_DeviceProcessing.EmitShutdown();

    delete mp_Drawer;
}

void TestDrawer::utTest()
{
    //QSKIP("skipped", SkipAll);
    QVERIFY( mp_Drawer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();

    // Request ReadRfid in wrong state
    QSignalSpy nackSpy(mp_Drawer, SIGNAL( ReportReadRackRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32) ));
    m_MockDcp.EmitReadRfid(LOADER_ID_RFID_1);
    QCOMPARE( nackSpy.count(), 1 );
    QCOMPARE( nackSpy.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_INVALID_STATE );
    m_MockDcp.EmitInitialize();

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_LedFree.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_LedBlocked.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Drawer->GetStates().contains("Working") );

    QSignalSpy ackSpy(mp_Drawer, SIGNAL( ReportReadRackRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32) ));

    m_MockDcp.EmitReadRfid(LOADER_ID_RFID_1);
    QCOMPARE(ackSpy.count(), 0);

    m_Rfid11785.EmitReportStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    m_Rfid11785.EmitReportLoginAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    m_Rfid11785.EmitReportUID(1234, DCL_ERR_FCT_CALL_SUCCESS, 123456);
    m_Rfid11785.EmitReportUserData(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 654321);

    QCOMPARE( ackSpy.count(), 1 );
    QCOMPARE( ackSpy.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
    QCOMPARE( ackSpy.at(0).at(1).value<LoaderRFIDChannel_t>(), LOADER_ID_RFID_1 );
    QCOMPARE( ackSpy.at(0).at(2).toInt(), 123456 );
    QCOMPARE( ackSpy.at(0).at(3).toInt(), 654321 );
}

void TestDrawer::utTestBase()
{
    //QSKIP("skipped", SkipAll);
    
    QVERIFY( mp_Drawer->GetStates().contains("Start") );

    // Request Initialize in wrong state
    QSignalSpy nackSpy(mp_Drawer, SIGNAL( ReportInitialize(ReturnCode_t) ));
    m_MockDcp.EmitInitialize();
    QCoreApplication::processEvents();
    QCOMPARE( nackSpy.count(), 1 );
    QCOMPARE( nackSpy.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_INVALID_STATE );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Drawer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Drawer->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Drawer->GetStates().contains("StepperInitRefRun") );

    QSignalSpy initSpy(mp_Drawer, SIGNAL(ReportInitialize(ReturnCode_t)));

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_LedFree.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_LedBlocked.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

    QVERIFY( mp_Drawer->GetStates().contains("Working") );
    QCOMPARE( initSpy.count(), 1 );
    QCOMPARE( initSpy.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestDrawer::utTestError()
{
    //QSKIP("skipped", SkipAll);

    m_MockDcp.EmitConfigure();
    QCoreApplication::processEvents();

    QSignalSpy initSpy(mp_Drawer, SIGNAL( ReportInitialize(ReturnCode_t) ));

    m_Motor.SetNextError();
    m_MockDcp.EmitInitialize();
    QCoreApplication::processEvents();
    QVERIFY( mp_Drawer->GetStates().contains("Configured") );
    QCOMPARE( initSpy.count(), 1 );
    QCOMPARE( initSpy.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_FAILED );
}

void TestDrawer::utTestDrawerMotion()
{
    m_MockDcp.EmitConfigure();
    QCoreApplication::processEvents();

    m_MockDcp.EmitInitialize();
    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_LedFree.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_LedBlocked.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Drawer->GetStates().contains("Closed") );
    
    m_MockDcp.EmitBlock();
    QVERIFY( mp_Drawer->GetStates().contains("Blocking") );
    m_LedFree.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_LedBlocked.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Drawer->GetStates().contains("Blocked") );

    QSignalSpy ackSpy(mp_Drawer, SIGNAL(ReportUnblock(ReturnCode_t)));

    m_MockDcp.EmitUnblock();
    // emit again in wrong state
    m_MockDcp.EmitUnblock();
    QVERIFY( mp_Drawer->GetStates().contains("Unblocking") );
    m_LedFree.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_LedBlocked.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Drawer->GetStates().contains("Closed") );
    QCOMPARE(ackSpy.count(), 2 );
    QCOMPARE(ackSpy.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_INVALID_STATE );
    QCOMPARE(ackSpy.at(1).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );

    m_Button.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    m_Button.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QCoreApplication::processEvents();
    QVERIFY( mp_Drawer->GetStates().contains("Open") );
    m_Button.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    m_Button.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QCoreApplication::processEvents();
    QVERIFY( mp_Drawer->GetStates().contains("Closed") );
}

} // end namespace DeviceControl

QTEST_MAIN(DeviceControl::TestDrawer)

#include "TestDrawer.moc"

// vi: set ts=4 sw=4 et:

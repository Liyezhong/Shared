#include <QTest>
#include <QSignalSpy>
#include <QDebug>

#include "DeviceControl/Include/Devices/DeviceXyz.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl {

enum FM_INSTANCE_ID {
    FM_STEPPER_MOTOR_X = 1,
    FM_STEPPER_MOTOR_Y,
    FM_STEPPER_MOTOR_Z,
    FM_RFID_11785
};

class TestXyz : public QObject {
    Q_OBJECT
public:
    TestXyz();
    ~TestXyz();

private slots:
    void init();
    void cleanup();

    void utTest();
    void utTestAttachDetach();

private:
    // Device
    CDeviceXyz *mp_XyzDevice;

    // Function Modules
    CStepperMotor m_XAxisMotor;
    CStepperMotor m_YAxisMotor;
    CStepperMotor m_ZAxisMotor;
    CRfid11785 m_Rfid11785;

    // Device Processing
    DeviceProcessing m_DeviceProcessing;
    DeviceModuleList_t m_DevFctModList;

    // Device Command Processor
    CMockDcp m_MockDcp;
};

TestXyz::TestXyz() : mp_XyzDevice(NULL)
{
    quint32 FctInstanceID;

    FctInstanceID = FM_STEPPER_MOTOR_X;
    m_DevFctModList.insert(CANObjectKeyLUT::m_MotorXAxisKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_XAxisMotor);

    FctInstanceID = FM_STEPPER_MOTOR_Y;
    m_DevFctModList.insert(CANObjectKeyLUT::m_MotorYAxisKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_YAxisMotor);

    FctInstanceID = FM_STEPPER_MOTOR_Z;
    m_DevFctModList.insert(CANObjectKeyLUT::m_MotorZAxisKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_ZAxisMotor);

    FctInstanceID = FM_RFID_11785;
    m_DevFctModList.insert(CANObjectKeyLUT::m_GrapplerRFIDKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_Rfid11785);
}

TestXyz::~TestXyz()
{
}

void TestXyz::init()
{  
    m_XAxisMotor.Reset();
    m_YAxisMotor.Reset();
    m_ZAxisMotor.Reset();
    m_Rfid11785.Reset();

    mp_XyzDevice = new CDeviceXyz(m_DeviceProcessing, m_DevFctModList, DEVICE_INSTANCE_ID_GRAPPLER_2);

    if (!connect(&m_MockDcp, SIGNAL(Configure()), 
                  mp_XyzDevice, SIGNAL(Configure()))) {
        QFAIL("Cannot connect Configure signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Initialize()), 
                  mp_XyzDevice, SIGNAL(Initialize()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Abort()),
                  mp_XyzDevice, SIGNAL(Abort()))) {
        QFAIL("Cannot connect Abort signal.");
    }

//    if (!connect(&m_MockDcp, SIGNAL(ReadRfid(LoaderRFIDChannel_t)),
//                  mp_XyzDevice, SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)))) {
//        QFAIL("Cannot connect ReadRfid signal.");
//    }

    if (!connect(&m_MockDcp, SIGNAL(MoveEmptyTo(quint16,quint16)),
                 mp_XyzDevice, SIGNAL(MoveEmptyTo(quint16,quint16))))
    {
        QFAIL("Cannot connect MoveEmptyTo signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(TransportRackTo(quint16,quint16)),
                 mp_XyzDevice, SIGNAL(TransportRackTo(quint16,quint16))))
    {
        QFAIL("Cannot connect TransportRackTo signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(AttachRack()),
                 mp_XyzDevice, SIGNAL(AttachRack())))
    {
        QFAIL("Cannot connect AttachRack signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(DetachRack()),
                 mp_XyzDevice, SIGNAL(DetachRack())))
    {
        QFAIL("Cannot connect DetachRack signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(LetDownRack()),
                 mp_XyzDevice, SIGNAL(LetDownRack())))
    {
        QFAIL("Cannot connect LetDownRack signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(PullUpRack()),
                 mp_XyzDevice, SIGNAL(PullUpRack())))
    {
        QFAIL("Cannot connect PullUpRack signal");
    }

    // give instantiated state machine a chance to start and process events
    QCoreApplication::processEvents();

    // give some time for the thread to start
    THREAD_PROCESS();
}

void TestXyz::cleanup()
{  
    m_DeviceProcessing.EmitShutdown();

    delete mp_XyzDevice;
}

void TestXyz::utTest()
{
    quint16 TestCount = 0;
    //QSKIP("skipped", SkipAll);
    QVERIFY( mp_XyzDevice->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_XyzDevice->GetStates().contains("Configured") );

    // Simulate & verify Initilization completion
    {
        qDebug() << endl << "Test " << ++TestCount << ": Simulate & verify Initilization completion";

        m_MockDcp.EmitInitialize();
        QSignalSpy initSpy(mp_XyzDevice, SIGNAL(ReportInitialize(ReturnCode_t)));

        m_ZAxisMotor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
        m_ZAxisMotor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

        m_YAxisMotor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
        m_YAxisMotor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

        m_XAxisMotor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
        m_XAxisMotor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

        QCOMPARE(initSpy.count(), 1);
        QVERIFY(mp_XyzDevice->GetStates().contains("Working") );
        QVERIFY(mp_XyzDevice->GetStates().contains("MoveXyz") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Navigator") );
        QVERIFY(mp_XyzDevice->GetStates().contains("ReadRackRfid") );
    }

    // Simulate & verify MoveEmptyTo
    {
        qDebug() << endl << "Test " << ++TestCount << ": Simulate & verify MoveEmptyTo";

        m_MockDcp.EmitMoveEmptyTo(1, 2);
        QSignalSpy moveEmptyAck(mp_XyzDevice, SIGNAL(ReportMoveEmptyTo(ReturnCode_t)));

        QVERIFY(mp_XyzDevice->GetStates().contains("Move"));
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_MOVE_EMPTY);
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving"));
        QVERIFY(mp_XyzDevice->GetStates().contains("X") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Y") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Z") );

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

        QVERIFY((mp_XyzDevice->GetStates().contains("Move") == FALSE));
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_IDLE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") == FALSE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Idle") );
        QCOMPARE(moveEmptyAck.count(), 1);
        QCOMPARE(moveEmptyAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
    }

    // Simulate & verify TransportRackTo
    {
        qDebug() << endl << "Test " << ++TestCount << ": Simulate & verify TransportRackTo";

        m_MockDcp.EmitTransportRack(2, 2);
        QSignalSpy transportRackAck(mp_XyzDevice, SIGNAL(ReportTransportRackTo(ReturnCode_t)));

        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_MOVE_RACK);
        QVERIFY(mp_XyzDevice->GetStates().contains("Move") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") );
        QVERIFY(mp_XyzDevice->GetStates().contains("X") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Y") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Z") );

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

        QVERIFY(mp_XyzDevice->GetStates().contains("Move") == FALSE);
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_IDLE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") == FALSE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Idle") );
        QCOMPARE(transportRackAck.count(), 1);
        QCOMPARE(transportRackAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
    }

    // Simulate & verify Abort
    {
        qDebug()  << endl << "Test " << ++TestCount << ": Simulate & verify Abort";

        m_MockDcp.EmitTransportRack(2, 2);

        QVERIFY(mp_XyzDevice->GetStates().contains("Move") );
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_MOVE_RACK);
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") );

        m_MockDcp.EmitAbort();
        QSignalSpy reportAbort(mp_XyzDevice, SIGNAL(ReportAbort(ReturnCode_t)));

        QVERIFY(mp_XyzDevice->GetStates().contains("Aborting") );
        QVERIFY(mp_XyzDevice->GetStates().contains("X") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Y") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Z") );

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

        QVERIFY(mp_XyzDevice->GetStates().contains("Move") == FALSE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Aborting") == FALSE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Idle") );
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_IDLE);
        QCOMPARE(reportAbort.count(), 1);
        QCOMPARE(reportAbort.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
    }
}

void TestXyz::utTestAttachDetach()
{
    quint16 TestCount = 0;
    //QSKIP("skipped", SkipAll);
    QVERIFY( mp_XyzDevice->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_XyzDevice->GetStates().contains("Configured") );

    // Simulate & verify Initilization completion
    {
        qDebug() << endl << "Test " << ++TestCount << ": Simulate & verify Initilization completion";

        m_MockDcp.EmitInitialize();
        QSignalSpy initSpy(mp_XyzDevice, SIGNAL(ReportInitialize(ReturnCode_t)));

        m_ZAxisMotor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
        m_ZAxisMotor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

        m_YAxisMotor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
        m_YAxisMotor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

        m_XAxisMotor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
        m_XAxisMotor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

        QCOMPARE(initSpy.count(), 1);
        QVERIFY(mp_XyzDevice->GetStates().contains("Working") );
        QVERIFY(mp_XyzDevice->GetStates().contains("MoveXyz") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Navigator") );
        QVERIFY(mp_XyzDevice->GetStates().contains("ReadRackRfid") );
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_IDLE);
    }

    // Simulate & verify MoveEmptyTo
    {
        qDebug() << endl << "Test " << ++TestCount << ": Simulate & verify MoveEmptyTo";

        m_MockDcp.EmitMoveEmptyTo(1, 1);
        QSignalSpy moveEmptyAck(mp_XyzDevice, SIGNAL(ReportMoveEmptyTo(ReturnCode_t)));

        QVERIFY(mp_XyzDevice->GetStates().contains("Move"));
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") );
        QVERIFY(mp_XyzDevice->GetStates().contains("X") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Y") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Z") );

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

        QVERIFY((mp_XyzDevice->GetStates().contains("Move") == FALSE));
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_IDLE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") == FALSE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Idle") );
        QCOMPARE(moveEmptyAck.count(), 1);
        QCOMPARE(moveEmptyAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
    }

    // Simulate & verify AttachRack
    {
        qDebug() << endl << "Test " << ++TestCount << ": Simulate & verify AttachRack";

        m_MockDcp.EmitAttachRack();
        QSignalSpy attachRackAck(mp_XyzDevice, SIGNAL(ReportAttachRack(ReturnCode_t)));

        QVERIFY(mp_XyzDevice->GetStates().contains("Move"));
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_ATTACH);
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") );
        QVERIFY(mp_XyzDevice->GetStates().contains("X") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Y") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Z") );

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 120, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 120, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

        QVERIFY((mp_XyzDevice->GetStates().contains("Move") == FALSE));
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_IDLE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") == FALSE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Idle") );
        QCOMPARE(attachRackAck.count(), 1);
        QCOMPARE(attachRackAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
    }

    // Simulate & verify DetachRack
    {
        qDebug() << endl << "Test " << ++TestCount << ": Simulate & verify DetachRack";

        m_MockDcp.EmitDetachRack();
        QSignalSpy detachRackAck(mp_XyzDevice, SIGNAL(ReportDetachRack(ReturnCode_t)));

        QVERIFY(mp_XyzDevice->GetStates().contains("Move"));
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_DETACH);
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") );
        QVERIFY(mp_XyzDevice->GetStates().contains("X") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Y") );
        QVERIFY(mp_XyzDevice->GetStates().contains("Z") );

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 120, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

        m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
        m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 50, 0);

        QVERIFY((mp_XyzDevice->GetStates().contains("Move") == FALSE));
        QCOMPARE(mp_XyzDevice->GetCurrentstate(), XYZ_IDLE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Moving") == FALSE);
        QVERIFY(mp_XyzDevice->GetStates().contains("Idle") );
        QCOMPARE(detachRackAck.count(), 1);
        QCOMPARE(detachRackAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
    }
}

} // end namespace DeviceControl

QTEST_MAIN(DeviceControl::TestXyz)

#include "TestXyz.moc"

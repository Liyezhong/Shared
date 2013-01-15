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

    void utTestMoveEmpty();
    void utTestMoveRack();
    void utTestAttachRack();
    void utTestDetachRack();
    void utTestLetDownRack();
    void utTestPullupRack();

    void utTestAbortMoveEmpty();
    void utTestAbortMoveRack();
    void utTestAbortAttachRack();
    void utTestAbortDetachRack();
    void utTestAbortLetDownRack();
    void utTestAbortPullupRack();

    void utTestMoveEmptyError();

private:
    // Device
    CDeviceXyz *mp_LeftXyzDevice;
    CDeviceXyz *mp_RightXyzDevice;

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

    void DeviceInit();
};

TestXyz::TestXyz() : mp_LeftXyzDevice(NULL), mp_RightXyzDevice(NULL)
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

    mp_LeftXyzDevice = new CDeviceXyz(m_DeviceProcessing,
                                      m_DevFctModList, DEVICE_INSTANCE_ID_XYZ);

    mp_RightXyzDevice = new CDeviceXyz(m_DeviceProcessing,
                                       m_DevFctModList, DEVICE_INSTANCE_ID_XYZ);

    if (!connect(&m_MockDcp, SIGNAL(Configure()), 
                  mp_LeftXyzDevice, SIGNAL(Configure()))) {
        QFAIL("Cannot connect Configure signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Initialize()), 
                  mp_LeftXyzDevice, SIGNAL(Initialize()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Abort()),
                  mp_LeftXyzDevice, SIGNAL(Abort()))) {
        QFAIL("Cannot connect Abort signal.");
    }

//    if (!connect(&m_MockDcp, SIGNAL(ReadRfid(LoaderRFIDChannel_t)),
//                  mp_XyzDevice, SIGNAL(ReadRackRfid(LoaderRFIDChannel_t)))) {
//        QFAIL("Cannot connect ReadRfid signal.");
//    }

    if (!connect(&m_MockDcp, SIGNAL(MoveEmptyTo(quint16,quint16)),
                 mp_LeftXyzDevice, SIGNAL(MoveEmptyTo(quint16,quint16))))
    {
        QFAIL("Cannot connect MoveEmptyTo signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(MoveRackTo(quint16,quint16)),
                 mp_LeftXyzDevice, SIGNAL(MoveRackTo(quint16,quint16))))
    {
        QFAIL("Cannot connect TransportRackTo signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(AttachRack()),
                 mp_LeftXyzDevice, SIGNAL(AttachRack())))
    {
        QFAIL("Cannot connect AttachRack signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(DetachRack()),
                 mp_LeftXyzDevice, SIGNAL(DetachRack())))
    {
        QFAIL("Cannot connect DetachRack signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(LetDownRack()),
                 mp_LeftXyzDevice, SIGNAL(LetDownRack())))
    {
        QFAIL("Cannot connect LetDownRack signal");
    }

    if (!connect(&m_MockDcp, SIGNAL(PullUpRack()),
                 mp_LeftXyzDevice, SIGNAL(PullUpRack())))
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

    delete mp_LeftXyzDevice;
}

void TestXyz::DeviceInit()
{
    QVERIFY( mp_LeftXyzDevice->GetStates().contains("Start") );
    m_MockDcp.EmitConfigure();
    QVERIFY( mp_LeftXyzDevice->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QSignalSpy initSpy(mp_LeftXyzDevice, SIGNAL(ReportInitialize(ReturnCode_t)));

    m_ZAxisMotor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    m_ZAxisMotor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

    m_YAxisMotor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    m_YAxisMotor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

    m_XAxisMotor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    m_XAxisMotor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);

    QCOMPARE(initSpy.count(), 1);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Working"));
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("MoveXyz"));
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Navigator"));
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("ReadRackRfid"));
}

void TestXyz::utTestMoveEmpty()
{
    // Device Initilization
    DeviceInit();

    // Simulate & verify MoveEmptyTo
    m_MockDcp.EmitMoveEmptyTo(4, 2);
    QSignalSpy moveEmptyAck(mp_LeftXyzDevice, SIGNAL(ReportMoveEmptyTo(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_EMPTY);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving"));
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("X") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Y") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Z") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(moveEmptyAck.count(), 1);
    QCOMPARE(moveEmptyAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestXyz::utTestMoveRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate & verify TransportRackTo
    m_MockDcp.EmitMoveRack(5, 2);
    QSignalSpy transportRackAck(mp_LeftXyzDevice, SIGNAL(ReportMoveRackTo(ReturnCode_t)));

    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_RACK);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("X") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Y") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Z") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move") == FALSE);
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(transportRackAck.count(), 1);
    QCOMPARE(transportRackAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestXyz::utTestAttachRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate MoveEmptyTo
    m_MockDcp.EmitMoveEmptyTo(3, 1);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Simulate & verify AttachRack
    m_MockDcp.EmitAttachRack();
    QSignalSpy attachRackAck(mp_LeftXyzDevice, SIGNAL(ReportAttachRack(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_ATTACH);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("X") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Y") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Z") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 120, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 120, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(attachRackAck.count(), 1);
    QCOMPARE(attachRackAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestXyz::utTestDetachRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate MoveEmptyTo
    m_MockDcp.EmitMoveEmptyTo(3, 1);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Simulate AttachRack
    m_MockDcp.EmitAttachRack();

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 120, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 120, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Simulate & verify DetachRack
    m_MockDcp.EmitDetachRack();
    QSignalSpy detachRackAck(mp_LeftXyzDevice, SIGNAL(ReportDetachRack(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_DETACH);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("X") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Y") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Z") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 120, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 50, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(detachRackAck.count(), 1);
    QCOMPARE(detachRackAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestXyz::utTestLetDownRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate MoveEmptyTo
    m_MockDcp.EmitMoveEmptyTo(3, 1);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Simulate & verify LetDownrack
    m_MockDcp.EmitLetDownRack();
    QSignalSpy letDownAck(mp_LeftXyzDevice, SIGNAL(ReportLetDownRack(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_LET_DOWN_RACK);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("X") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Y") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Z") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(letDownAck.count(), 1);
    QCOMPARE(letDownAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS);
}

void TestXyz::utTestPullupRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate MoveEmptyTo
    m_MockDcp.EmitMoveEmptyTo(4, 1);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Simulate & verify PullupRack
    m_MockDcp.EmitPullupRack();
    QSignalSpy pullUpAck(mp_LeftXyzDevice, SIGNAL(ReportPullUpRack(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_PULL_UP_RACK);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("X") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Y") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Z") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(pullUpAck.count(), 1);
    QCOMPARE(pullUpAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestXyz::utTestAbortMoveEmpty()
{
    // Device Initilization
    DeviceInit();

    // Simulate MoveEmptyTo
    m_MockDcp.EmitMoveEmptyTo(4, 2);
    QSignalSpy moveEmptyAck(mp_LeftXyzDevice, SIGNAL(ReportMoveEmptyTo(ReturnCode_t)));

    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_EMPTY);

    // Abort in between
    m_MockDcp.EmitAbort();
    QSignalSpy reportAbort(mp_LeftXyzDevice, SIGNAL(ReportAbort(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("X") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Y") );
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Z") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(reportAbort.count(), 1);
    QCOMPARE(reportAbort.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );

    // Resend the same command
    m_MockDcp.EmitMoveEmptyTo(4, 2);
    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(moveEmptyAck.count(), 1);
    QCOMPARE(moveEmptyAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestXyz::utTestAbortMoveRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate MoveRackTo
    m_MockDcp.EmitMoveRack(5, 2);
    QSignalSpy transportRackAck(mp_LeftXyzDevice, SIGNAL(ReportMoveRackTo(ReturnCode_t)));

    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_RACK);

    // Abort in between
    m_MockDcp.EmitAbort();
    QSignalSpy reportAbort(mp_LeftXyzDevice, SIGNAL(ReportAbort(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") == FALSE);
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(reportAbort.count(), 1);
    QCOMPARE(reportAbort.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS);

    // Resend same command
    m_MockDcp.EmitMoveRack(5, 2);
    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move") == FALSE);
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(transportRackAck.count(), 1);
    QCOMPARE(transportRackAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS);
}

void TestXyz::utTestAbortAttachRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate Move Empty & Attach Rack
    m_MockDcp.EmitMoveEmptyTo(4, 1);
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_EMPTY);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 1000, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 500, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    m_MockDcp.EmitAttachRack();
    QSignalSpy attachRackAck(mp_LeftXyzDevice, SIGNAL(ReportAttachRack(ReturnCode_t)));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_ATTACH);

//    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
//    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
//    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Abort
    m_MockDcp.EmitAbort();
    QSignalSpy reportAbort(mp_LeftXyzDevice, SIGNAL(ReportAbort(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(reportAbort.count(), 1);
    QCOMPARE(reportAbort.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );

    // Resend Attach
    m_MockDcp.EmitAttachRack();
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_ATTACH);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

//    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
//    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
//    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(attachRackAck.count(), 1);
    QCOMPARE(attachRackAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestXyz::utTestAbortDetachRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate Move Empty
    m_MockDcp.EmitMoveRack(4, 1);
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_RACK);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 1000, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 500, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Simulate & verify DetachRack -> Abort -> DetachRack
    m_MockDcp.EmitDetachRack();
    QSignalSpy detachRackAck(mp_LeftXyzDevice, SIGNAL(ReportDetachRack(ReturnCode_t)));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_DETACH);

//    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
//    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
//    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Abort
    m_MockDcp.EmitAbort();
    QSignalSpy reportAbort(mp_LeftXyzDevice, SIGNAL(ReportAbort(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(reportAbort.count(), 1);
    QCOMPARE(reportAbort.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );

    // Resend DetachRack
    m_MockDcp.EmitDetachRack();
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_DETACH);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

//    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
//    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 200, 0);
//    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(detachRackAck.count(), 1);
    QCOMPARE(detachRackAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestXyz::utTestAbortLetDownRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate MoveEmptyTo
    m_MockDcp.EmitMoveEmptyTo(3, 1);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Simulate & verify LetDownrack -> Abort -> LetDownrack
    m_MockDcp.EmitLetDownRack();
    QSignalSpy letDownAck(mp_LeftXyzDevice, SIGNAL(ReportLetDownRack(ReturnCode_t)));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_LET_DOWN_RACK);

    // Abort
    m_MockDcp.EmitAbort();
    QSignalSpy reportAbort(mp_LeftXyzDevice, SIGNAL(ReportAbort(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") == FALSE);
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(reportAbort.count(), 1);
    QCOMPARE(reportAbort.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS);

    // Resend
    m_MockDcp.EmitLetDownRack();
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_LET_DOWN_RACK);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(letDownAck.count(), 1);
    QCOMPARE(letDownAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS);
}

void TestXyz::utTestAbortPullupRack()
{
    // Device Initilization
    DeviceInit();

    // Simulate MoveEmptyTo
    m_MockDcp.EmitMoveEmptyTo(4, 1);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Simulate & verify PullupRack -> Abort -> PullUpAck
    m_MockDcp.EmitPullupRack();
    QSignalSpy pullUpAck(mp_LeftXyzDevice, SIGNAL(ReportPullUpRack(ReturnCode_t)));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_PULL_UP_RACK);

    // Abort
    m_MockDcp.EmitAbort();
    QSignalSpy reportAbort(mp_LeftXyzDevice, SIGNAL(ReportAbort(ReturnCode_t)));

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") );

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Aborting") == FALSE);
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(reportAbort.count(), 1);
    QCOMPARE(reportAbort.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS);

    // Resend
    m_MockDcp.EmitPullupRack();
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_PULL_UP_RACK);

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving") == FALSE);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Idle") );
    QCOMPARE(pullUpAck.count(), 1);
    QCOMPARE(pullUpAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_SUCCESS );
}

void TestXyz::utTestMoveEmptyError()
{
    // Device Initilization
    DeviceInit();

    // Simulate & verify MoveEmptyTo
    QSignalSpy moveEmptyAck(mp_LeftXyzDevice, SIGNAL(ReportMoveEmptyTo(ReturnCode_t)));

    // Error in X Axis
    m_MockDcp.EmitMoveEmptyTo(4, 2);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_EMPTY);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving"));

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_FAILED, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(moveEmptyAck.count(), 1);
    QCOMPARE(moveEmptyAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_FAILED);

    // Error in Y Axis
    m_MockDcp.EmitMoveEmptyTo(4, 2);
    moveEmptyAck.clear();
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_EMPTY);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving"));

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_FAILED, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(moveEmptyAck.count(), 1);
    QCOMPARE(moveEmptyAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_FAILED);

    // Error in Z Axis
    m_MockDcp.EmitMoveEmptyTo(4, 2);
    moveEmptyAck.clear();
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_EMPTY);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving"));

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_FAILED, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(moveEmptyAck.count(), 1);
    QCOMPARE(moveEmptyAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_FAILED);

    // Error in X Axis
    // Error in Y Axis
    // Error in Z Axis
    m_MockDcp.EmitMoveEmptyTo(4, 2);
    moveEmptyAck.clear();
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_EMPTY);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving"));

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_FAILED, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_FAILED, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_FAILED, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(moveEmptyAck.count(), 1);
    QCOMPARE(moveEmptyAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_FAILED);

    // Different errors in Y & Z Axis
    m_MockDcp.EmitMoveEmptyTo(4, 2);
    moveEmptyAck.clear();
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Move"));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_MOVE_EMPTY);
    QVERIFY(mp_LeftXyzDevice->GetStates().contains("Moving"));

    m_XAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 100, 0);
    m_YAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_FAILED, 100, 0);
    m_ZAxisMotor.EmitReportMovementAckn(1234, DCL_ERR_TIMEOUT, 0, 0);

    QVERIFY((mp_LeftXyzDevice->GetStates().contains("Move") == FALSE));
    QCOMPARE(mp_LeftXyzDevice->GetCurrentstate(), XYZ_STATE_IDLE);
    QCOMPARE(moveEmptyAck.count(), 1);
    QCOMPARE(moveEmptyAck.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_TIMEOUT);

}

} // end namespace DeviceControl

QTEST_MAIN(DeviceControl::TestXyz)

#include "TestXyz.moc"

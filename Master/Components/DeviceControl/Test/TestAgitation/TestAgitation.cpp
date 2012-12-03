#include <QtCore/QString>
#include <QtTest/QtTest>

#include "DeviceControl/Include/Devices/DeviceAgitation.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl {

enum FM_INSTANCE_ID {
    FM_STEPPER_MOTOR = 1
};

class TestAgitation : public QObject
{
    Q_OBJECT

public:
    TestAgitation();

private Q_SLOTS:
    void init();
    void cleanup();

    void utTestRotateStop();
    void utTestAgitationError();
    void utTestAgitationStopResumeMoveIdle();
    void utTestAgitationMoveAbortMove();

private:
    // Device
    CDeviceAgitation *mp_Agitation;

    // Function Modules
    CStepperMotor m_Motor;

    // Device Processing
    DeviceProcessing m_DeviceProcessing;
    DeviceModuleList_t m_DevFctModList;

    // Device Command Processor
    CMockDcp m_MockDcp;
};

TestAgitation::TestAgitation() : m_DeviceProcessing(this)
{
    quint32 FctInstanceID;

    FctInstanceID = FM_STEPPER_MOTOR;
    m_DevFctModList.insert(CANObjectKeyLUT::m_MotorAgitationKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_Motor);
}

void TestAgitation::init()
{
    m_Motor.Reset();
    mp_Agitation = new CDeviceAgitation(m_DeviceProcessing, m_DevFctModList, DEVICE_INSTANCE_ID_AGITATION);

    if (!connect(&m_MockDcp, SIGNAL(Configure()),
                  mp_Agitation, SIGNAL(Configure()))) {
        QFAIL("Cannot connect Configure signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Initialize()),
                  mp_Agitation, SIGNAL(Initialize()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Rotate(quint32, quint8)),
                  mp_Agitation, SIGNAL(Rotate(quint32, quint8)))) {
        QFAIL("Cannot connect to Rotate Signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Move(quint32, quint8)),
                  mp_Agitation, SIGNAL(Move(quint32, quint8)))) {
        QFAIL("Cannot connect to Move Signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Stop()),
                  mp_Agitation, SIGNAL(Stop()))) {
        QFAIL("Cannot connect to Stop Signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Resume()),
                  mp_Agitation, SIGNAL(Resume()))) {
        QFAIL("Cannot connect to Resume Signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Abort()),
                  mp_Agitation, SIGNAL(Abort()))) {
        QFAIL("Cannot connect to Abort Signal.");
    }

       // give instantiated state machine a chance to start and process events
    QCoreApplication::processEvents();

    THREAD_PROCESS();
}

void TestAgitation::cleanup()
{
    delete mp_Agitation;
}

void TestAgitation::utTestRotateStop()
{
    // Normal flow: Idle-> Rotate -> Idle

    QVERIFY( mp_Agitation->GetStates().contains("Start") );

    QSignalSpy nackSpy(mp_Agitation, SIGNAL( RotateAck() ));

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Agitation->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Agitation->GetStates().contains("Initializing") );


    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Agitation->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Agitation->GetStates().contains("Working") );
    QVERIFY( mp_Agitation->GetStates().contains("StepperIdle") );

    m_MockDcp.EmitRotate(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperChangingSpeed") );

    m_Motor.EmitReportSpeed(1234,DCL_ERR_FCT_CALL_SUCCESS,3);
    QVERIFY( mp_Agitation->GetStates().contains("StepperRotationActive") );
    QCOMPARE( nackSpy.count(), 1 );


    m_MockDcp.EmitRotate(4,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperChangingSpeed") );

    m_Motor.EmitReportSpeed(1234,DCL_ERR_FCT_CALL_SUCCESS,0);
    QVERIFY( mp_Agitation->GetStates().contains("StepperIdle") );
    QCOMPARE( nackSpy.count(), 2 );

    m_MockDcp.EmitRotate(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperChangingSpeed") );


    m_Motor.EmitReportSpeed(1234,DCL_ERR_FCT_CALL_SUCCESS,3);
    QVERIFY( mp_Agitation->GetStates().contains("StepperRotationActive") );
    QCOMPARE( nackSpy.count(), 3 );

    m_MockDcp.EmitRotate(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperChangingSpeed") );

    m_Motor.EmitReportSpeed(1234,DCL_ERR_FCT_CALL_SUCCESS,3);
    QVERIFY( mp_Agitation->GetStates().contains("StepperRotationActive") );
    QCOMPARE( nackSpy.count(), 4 );

    m_MockDcp.EmitMove(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperMoving") );

    m_MockDcp.EmitStop();
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0,0);
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopped") );

    m_MockDcp.EmitResume();
    QVERIFY( mp_Agitation->GetStates().contains("StepperMoving") );

    m_MockDcp.EmitStop();
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0,0);
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopped") );

    m_MockDcp.EmitAbort();
    QVERIFY( mp_Agitation->GetStates().contains("StepperIdle") );


}
void TestAgitation::utTestAgitationError()
{
    // Exceptional Flow: Open before Initialization

    QVERIFY( mp_Agitation->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Agitation->GetStates().contains("Configured") );

    // Emit open signal before initializing
    m_MockDcp.EmitRotate(2,1);

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Agitation->GetStates().contains("Initializing") );
}

void TestAgitation::utTestAgitationStopResumeMoveIdle()
{
    // Test Stop & Resume command flow

    QVERIFY( mp_Agitation->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Agitation->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Agitation->GetStates().contains("Initializing") );

    QVERIFY( mp_Agitation->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Agitation->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Agitation->GetStates().contains("StepperIdle") );

    m_MockDcp.EmitRotate(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperChangingSpeed") );

    m_Motor.EmitReportSpeed(1234,DCL_ERR_FCT_CALL_SUCCESS,3);
    QVERIFY( mp_Agitation->GetStates().contains("StepperRotationActive") );

    m_MockDcp.EmitMove(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperMoving") );

    m_MockDcp.EmitStop();
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0,0);
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopped") );

    m_MockDcp.EmitResume();
    QVERIFY( mp_Agitation->GetStates().contains("StepperMoving") );

    m_MockDcp.EmitStop();
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0,0);
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopped") );

    m_MockDcp.EmitResume();
    QVERIFY( mp_Agitation->GetStates().contains("StepperMoving") );

     m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0,0);
    QVERIFY( mp_Agitation->GetStates().contains("StepperIdle") );
}

void TestAgitation::utTestAgitationMoveAbortMove()
{
    // Test Abort command flow
    // Rotate -> Move -> Abort -> Rotate-> Move

    QVERIFY( mp_Agitation->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Agitation->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Agitation->GetStates().contains("Initializing") );

    QVERIFY( mp_Agitation->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Agitation->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Agitation->GetStates().contains("StepperIdle") );

    m_MockDcp.EmitRotate(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperChangingSpeed") );

    m_Motor.EmitReportSpeed(1234,DCL_ERR_FCT_CALL_SUCCESS,3);
    QVERIFY( mp_Agitation->GetStates().contains("StepperRotationActive") );

    m_MockDcp.EmitMove(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperMoving") );

    m_MockDcp.EmitStop();
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0,0);
    QVERIFY( mp_Agitation->GetStates().contains("StepperStopped") );

    m_MockDcp.EmitAbort();
    QVERIFY( mp_Agitation->GetStates().contains("StepperIdle") );

    m_MockDcp.EmitRotate(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperChangingSpeed") );

    m_Motor.EmitReportSpeed(1234,DCL_ERR_FCT_CALL_SUCCESS,3);
    QVERIFY( mp_Agitation->GetStates().contains("StepperRotationActive") );

    m_MockDcp.EmitMove(2,1);
    QVERIFY( mp_Agitation->GetStates().contains("StepperMoving") );
}


} // end namespace DeviceControl


QTEST_MAIN(DeviceControl::TestAgitation);

#include "TestAgitation.moc"

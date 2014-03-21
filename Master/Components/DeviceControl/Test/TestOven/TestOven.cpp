#include <QtCore/QString>
#include <QtTest/QtTest>

#include "DeviceControl/Include/Devices/DeviceOven.h"

#ifdef Q_UNIT_TEST
#include "Simulator.h"
#endif

namespace DeviceControl {

enum FM_INSTANCE_ID {
    FM_STEPPER_MOTOR = 1,
    FM_TEMP_CONTROL
};

class TestOven : public QObject
{
    Q_OBJECT

public:
    TestOven();

private Q_SLOTS:
    void init();
    void cleanup();

    void utTestOvenOpenClose();
    void utTestOvenError_1();
    void utTestOvenError_2();
    void utTestOvenInitFailed();
    void utTestOvenStopResume();
    void utTestOvenOpenAbortOpen();
    void utTestOvenOpenAbortClose();
    void utTestOvenCloseAbortClose();
    void utTestOvenCloseAbortOpen();
    void utTestOvenStepperError();

private:
    // Device
    CDeviceOven *mp_Oven;

    // Function Modules
    CStepperMotor m_Motor;
    CTemperatureControl m_TempControl;

    // Device Processing
    DeviceProcessing m_DeviceProcessing;
    DeviceModuleList_t m_DevFctModList;

    // Device Command Processor
    CMockDcp m_MockDcp;
};

TestOven::TestOven() : m_DeviceProcessing(this)
{
    quint32 FctInstanceID;

    FctInstanceID = FM_STEPPER_MOTOR;
    m_DevFctModList.insert(CANObjectKeyLUT::m_OvenCoverMotorKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_Motor);

    FctInstanceID = FM_TEMP_CONTROL;
    m_DevFctModList.insert(CANObjectKeyLUT::m_OvenTempCtrlKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_TempControl);
}

void TestOven::init()
{
    m_Motor.Reset();
    m_TempControl.Reset();

    mp_Oven = new CDeviceOven(m_DeviceProcessing, m_DevFctModList, DEVICE_INSTANCE_ID_OVEN);

    if (!connect(&m_MockDcp, SIGNAL(Configure()),
                  mp_Oven, SIGNAL(Configure()))) {
        QFAIL("Cannot connect Configure signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Initialize()),
                 mp_Oven, SIGNAL(Initialize()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Open()),
                  mp_Oven, SIGNAL(CoverOpen()))) {
        QFAIL("Cannot connect Open signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Close()),
                  mp_Oven, SIGNAL(CoverClose()))) {
        QFAIL("Cannot connect Close signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Stop()),
                  mp_Oven, SIGNAL(Stop()))) {
        QFAIL("Cannot connect Stop signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Resume()),
                  mp_Oven, SIGNAL(Resume()))) {
        QFAIL("Cannot connect Resume signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Abort()),
                  mp_Oven, SIGNAL(Abort()))) {
        QFAIL("Cannot connect Abort signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Disable()),
                  mp_Oven, SIGNAL(Disable()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    // give instantiated state machine a chance to start and process events
    QCoreApplication::processEvents();

    // give some time for the thread to start
    THREAD_PROCESS();
}

void TestOven::cleanup()
{
    m_DeviceProcessing.EmitShutdown();

    QCoreApplication::processEvents();

    delete mp_Oven;
}

void TestOven::utTestOvenOpenClose()
{
    // Normal flow: Ref run -> Open -> Close

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );
    QVERIFY( mp_Oven->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Oven->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Oven->GetStates().contains("Working") );
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );

    // Open Oven lid
    m_MockDcp.EmitOpen();
    QVERIFY( mp_Oven->GetStates().contains("Opening") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("Open") );

    // Close Oven lid
    m_MockDcp.EmitClose();
    QVERIFY( mp_Oven->GetStates().contains("Closing") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
}

void TestOven::utTestOvenError_1()
{
    // Reopen when already in Open state
    // Ref Run -> Open -> Open

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_Oven->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Oven->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Oven->GetStates().contains("Working") );
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );

    // Open Oven lid
    m_MockDcp.EmitOpen();
    QVERIFY( mp_Oven->GetStates().contains("Opening") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("Open") );

    // Try to reopen
    m_MockDcp.EmitOpen();
    QVERIFY( mp_Oven->GetStates().contains("Open") );
}

void TestOven::utTestOvenError_2()
{
    // Exceptional Flow: Open before Initialization

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    // Emit open signal before initializing
    m_MockDcp.EmitOpen();

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );
}

void TestOven::utTestOvenInitFailed()
{
    // Error Scenario: Test Set State & Reference Run Failure scenario

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_Oven->GetStates().contains("StepperInitSetState") );

    // Set state failed
    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_FAILED);
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    // Reinitialisation command received from DCP
    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);

    // Reference Run failed
    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_FAILED, 0);
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    // Reinitialisation command received from DCP
    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    // Reference run success
    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Oven->GetStates().contains("Working") );
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );
}

void TestOven::utTestOvenStopResume()
{
    // Test Stop & Resume command flow

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_Oven->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Oven->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );

    // Open Oven lid
    m_MockDcp.EmitOpen();
    QVERIFY( mp_Oven->GetStates().contains("Opening") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    // Stop movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_Oven->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("StepperStopped") );

    // Resume movement
    m_MockDcp.EmitResume();
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 5000, 0);
    // Confirm Open
    QVERIFY( mp_Oven->GetStates().contains("Open") );

    // Close Oven lid
    m_MockDcp.EmitClose();
    QVERIFY( mp_Oven->GetStates().contains("Closing") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    // Stop movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_Oven->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("StepperStopped") );

    // Resume movement
    m_MockDcp.EmitResume();
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 5000, 0);
    // Confirm Closed
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
}

void TestOven::utTestOvenOpenAbortOpen()
{
    // Test Abort command flow
    // Ref Run -> Open -> Stop -> Abort -> Open

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_Oven->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Oven->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );

    // Open Oven lid
    m_MockDcp.EmitOpen();
    QVERIFY( mp_Oven->GetStates().contains("Opening") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    // Stop movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_Oven->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("StepperStopped") );

    // Abort
    m_MockDcp.EmitAbort();
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );
    QVERIFY( mp_Oven->GetStates().contains("Intermediate") );

    // Open Oven lid again
    m_MockDcp.EmitOpen();
    QVERIFY( mp_Oven->GetStates().contains("Opening") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );
}

void TestOven::utTestOvenOpenAbortClose()
{
    // Test Abort command flow
    // Ref Run -> Open -> Stop -> Abort -> Close

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_Oven->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Oven->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );

    // Open Oven lid
    m_MockDcp.EmitOpen();
    QVERIFY( mp_Oven->GetStates().contains("Opening") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    // Stop movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_Oven->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("StepperStopped") );

    // Abort
    m_MockDcp.EmitAbort();
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );
    QVERIFY( mp_Oven->GetStates().contains("Intermediate") );

    // Open Oven lid again
    m_MockDcp.EmitClose();
    QVERIFY( mp_Oven->GetStates().contains("Closing") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );
}

void TestOven::utTestOvenCloseAbortClose()
{
    // Test Abort command flow
    // Ref Run -> Open -> Close -> Stop -> Abort -> Close

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_Oven->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Oven->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );

    // Open Oven lid
    m_MockDcp.EmitOpen();
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Close Oven lid
    m_MockDcp.EmitClose();
    QVERIFY( mp_Oven->GetStates().contains("Closing") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    // Stop Movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_Oven->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("StepperStopped") );

    // Abort
    m_MockDcp.EmitAbort();
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );
    QVERIFY( mp_Oven->GetStates().contains("Intermediate") );

    // Close Oven lid again
    m_MockDcp.EmitClose();
    QVERIFY( mp_Oven->GetStates().contains("Closing") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );
}

void TestOven::utTestOvenCloseAbortOpen()
{
    // Test Abort command flow
    // Ref Run -> Open -> Close -> Stop -> Abort -> Open

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_Oven->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Oven->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );

    // Open Oven lid
    m_MockDcp.EmitOpen();
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Close Oven lid
    m_MockDcp.EmitClose();
    QVERIFY( mp_Oven->GetStates().contains("Closing") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );

    // Stop Movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_Oven->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("StepperStopped") );

    // Abort
    m_MockDcp.EmitAbort();
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );
    QVERIFY( mp_Oven->GetStates().contains("Intermediate") );

    // Open Oven lid again
    m_MockDcp.EmitOpen();
    QVERIFY( mp_Oven->GetStates().contains("Opening") );
    QVERIFY( mp_Oven->GetStates().contains("StepperMoving") );
}

void TestOven::utTestOvenStepperError()
{
    // Test Stepper Error

    QVERIFY( mp_Oven->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_Oven->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_Oven->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_Oven->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_Oven->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_Oven->GetStates().contains("Closed") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );

    // Open Oven lid
    m_MockDcp.EmitOpen();
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_Oven->GetStates().contains("Open") );
    QVERIFY( mp_Oven->GetStates().contains("StepperIdle") );
}

} // end namespace DeviceControl

QTEST_MAIN(DeviceControl::TestOven);

#include "TestOven.moc"

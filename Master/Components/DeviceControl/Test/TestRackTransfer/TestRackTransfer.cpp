#include <QtCore/QString>
#include <QtTest/QtTest>

#include "DeviceControl/Include/Devices/DeviceRackTransfer.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"


namespace DeviceControl {

enum FM_INSTANCE_ID {
    FM_STEPPER_MOTOR = 1
};

class TestRackTransfer : public QObject
{
    Q_OBJECT

public:
    TestRackTransfer();

private Q_SLOTS:
    void init();
    void cleanup();

    void utRackTransferOpenClose();
    void utRackTransferError_1();
    void utRackTransferError_2();
    void utRackTransferInitFailed();
    void utRackTransferStopResume();
    void utRackTransferOpenAbortOpen();
    void utRackTransferOpenAbortClose();
    void utRackTransferCloseAbortClose();
    void utRackTransferCloseAbortOpen();
    void utRackTransferStepperError();

private:
    // Device
    CDeviceRackTransfer *mp_RackTransfer;

    // Function Modules
    CStepperMotor m_Motor;

    // Device Processing
    DeviceProcessing m_DeviceProcessing;
    DeviceModuleList_t m_DevFctModList;

    // Device Command Processor
    CMockDcp m_MockDcp;
};

TestRackTransfer::TestRackTransfer() : m_DeviceProcessing(this)
{
    quint32 FctInstanceID;

    FctInstanceID = FM_STEPPER_MOTOR;
    m_DevFctModList.insert(CANObjectKeyLUT::m_RackTransferMotorKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_Motor);
}

void TestRackTransfer::init()
{
    m_Motor.Reset();
    mp_RackTransfer = new CDeviceRackTransfer(m_DeviceProcessing, m_DevFctModList, DEVICE_INSTANCE_ID_RACKTRANSFER);

    if (!connect(&m_MockDcp, SIGNAL(Configure()),
                  mp_RackTransfer, SIGNAL(Configure()))) {
        QFAIL("Cannot connect Configure signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Initialize()),
                  mp_RackTransfer, SIGNAL(Initialize()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Open()),
                  mp_RackTransfer, SIGNAL(Transfer()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Close()),
                  mp_RackTransfer, SIGNAL(Load()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Stop()),
                  mp_RackTransfer, SIGNAL(Stop()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Resume()),
                  mp_RackTransfer, SIGNAL(Resume()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Abort()),
                  mp_RackTransfer, SIGNAL(Abort()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    if (!connect(&m_MockDcp, SIGNAL(Disable()),
                  mp_RackTransfer, SIGNAL(Disable()))) {
        QFAIL("Cannot connect Initialize signal.");
    }

    // give instantiated state machine a chance to start and process events
    QCoreApplication::processEvents();

    THREAD_PROCESS();
}

void TestRackTransfer::cleanup()
{
    delete mp_RackTransfer;
}

void TestRackTransfer::utRackTransferOpenClose()
{
    // Normal flow: Ref run -> Open -> Close

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Working") );
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );

    // Open
    m_MockDcp.EmitOpen();
    QVERIFY( mp_RackTransfer->GetStates().contains("Opening") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Open") );

    // Close
    m_MockDcp.EmitClose();
    QVERIFY( mp_RackTransfer->GetStates().contains("Closing") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
}

void TestRackTransfer::utRackTransferError_1()
{
    // Reopen when already in Open state
    // Ref Run -> Open -> Open

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Working") );
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );

    // Open
    m_MockDcp.EmitOpen();
    QVERIFY( mp_RackTransfer->GetStates().contains("Opening") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Open") );

    // Try to reopen
    m_MockDcp.EmitOpen();
    QVERIFY( mp_RackTransfer->GetStates().contains("Open") );
}

void TestRackTransfer::utRackTransferError_2()
{
    // Exceptional Flow: Open before Initialization

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    // Emit open signal before initializing
    m_MockDcp.EmitOpen();

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );
}

void TestRackTransfer::utRackTransferInitFailed()
{
    // Error Scenario: Test Set State & Reference Run Failure scenario

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitSetState") );

    // Set state failed
    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_FAILED);
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    // Reinitialisation command received from DCP
    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);

    // Reference Run failed
    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_FAILED, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    // Reinitialisation command received from DCP
    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Reference run success
    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Working") );
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );
}

void TestRackTransfer::utRackTransferStopResume()
{
    // Test Stop & Resume command flow

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );

    // Open
    m_MockDcp.EmitOpen();
    QVERIFY( mp_RackTransfer->GetStates().contains("Opening") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    // Stop movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopped") );

    // Resume movement
    m_MockDcp.EmitResume();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 5000, 0);
    // Confirm Open
    QVERIFY( mp_RackTransfer->GetStates().contains("Open") );

    // Close
    m_MockDcp.EmitClose();
    QVERIFY( mp_RackTransfer->GetStates().contains("Closing") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    // Stop movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopped") );

    // Resume movement
    m_MockDcp.EmitResume();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 5000, 0);
    // Confirm Closed
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
}

void TestRackTransfer::utRackTransferOpenAbortOpen()
{
    // Test Abort command flow
    // Ref Run -> Open -> Stop -> Abort -> Open

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );

    // Open
    m_MockDcp.EmitOpen();
    QVERIFY( mp_RackTransfer->GetStates().contains("Opening") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    // Stop movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopped") );

    // Abort
    m_MockDcp.EmitAbort();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );
    QVERIFY( mp_RackTransfer->GetStates().contains("Intermediate") );

    // Open
    m_MockDcp.EmitOpen();
    QVERIFY( mp_RackTransfer->GetStates().contains("Opening") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );
}

void TestRackTransfer::utRackTransferOpenAbortClose()
{
    // Test Abort command flow
    // Ref Run -> Open -> Stop -> Abort -> Close

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );

    // Open
    m_MockDcp.EmitOpen();
    QVERIFY( mp_RackTransfer->GetStates().contains("Opening") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    // Stop movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopped") );

    // Abort
    m_MockDcp.EmitAbort();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );
    QVERIFY( mp_RackTransfer->GetStates().contains("Intermediate") );

    // Open again
    m_MockDcp.EmitClose();
    QVERIFY( mp_RackTransfer->GetStates().contains("Closing") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );
}

void TestRackTransfer::utRackTransferCloseAbortClose()
{
    // Test Abort command flow
    // Ref Run -> Open -> Close -> Stop -> Abort -> Close

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );

    // Open
    m_MockDcp.EmitOpen();
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Close
    m_MockDcp.EmitClose();
    QVERIFY( mp_RackTransfer->GetStates().contains("Closing") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    // Stop Movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopped") );

    // Abort
    m_MockDcp.EmitAbort();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );
    QVERIFY( mp_RackTransfer->GetStates().contains("Intermediate") );

    // Close again
    m_MockDcp.EmitClose();
    QVERIFY( mp_RackTransfer->GetStates().contains("Closing") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );
}

void TestRackTransfer::utRackTransferCloseAbortOpen()
{
    // Test Abort command flow
    // Ref Run -> Open -> Close -> Stop -> Abort -> Open

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );

    // Open
    m_MockDcp.EmitOpen();
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);

    // Close
    m_MockDcp.EmitClose();
    QVERIFY( mp_RackTransfer->GetStates().contains("Closing") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );

    // Stop Movement
    m_MockDcp.EmitStop();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopping") );

    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperStopped") );

    // Abort
    m_MockDcp.EmitAbort();
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );
    QVERIFY( mp_RackTransfer->GetStates().contains("Intermediate") );

    // Open again
    m_MockDcp.EmitOpen();
    QVERIFY( mp_RackTransfer->GetStates().contains("Opening") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperMoving") );
}

void TestRackTransfer::utRackTransferStepperError()
{
    // Test Stepper Error

    QVERIFY( mp_RackTransfer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QVERIFY( mp_RackTransfer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QVERIFY( mp_RackTransfer->GetStates().contains("Initializing") );

    // Do Reference run
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitSetState") );

    m_Motor.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperInitRefRun") );

    m_Motor.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Closed") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );


    // Open
    m_MockDcp.EmitOpen();
    m_Motor.EmitReportMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 0);
    QVERIFY( mp_RackTransfer->GetStates().contains("Open") );
    QVERIFY( mp_RackTransfer->GetStates().contains("StepperIdle") );
}

} // end namespace DeviceControl

QTEST_MAIN(DeviceControl::TestRackTransfer)

#include "TestRackTransfer.moc"

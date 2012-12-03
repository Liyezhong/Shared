#include <QTest>
#include <QSignalSpy>
#include <QDebug>
#include "DeviceControl/Include/Devices/DeviceHood.h"

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl {

enum FM_INSTANCE_ID {
    FM_DIGITAL_INPUT = 1
};

class TestHood : public QObject {
    Q_OBJECT
public:
    TestHood();
    ~TestHood();

private slots:
    void init();
    void cleanup();

    void utTestHood();
private: 
    // Device
    CDeviceHood *mp_Hood;

    // Function Modules
    CDigitalInput m_HoodSensor;

    // Device Processing
    DeviceProcessing m_DeviceProcessing;
    DeviceModuleList_t m_DevFctModList;

    // Device Command Processor
    CMockDcp m_MockDcp;
};

TestHood::TestHood() : mp_Hood(NULL)
{
    quint32 FctInstanceID;

    FctInstanceID = FM_DIGITAL_INPUT;
    m_DevFctModList.insert(CANObjectKeyLUT::m_HoodSensorKey, FctInstanceID);
    m_DeviceProcessing.InsertFuncModule(FctInstanceID, &m_HoodSensor);
}

TestHood::~TestHood()
{
}

void TestHood::init() 
{  
    m_HoodSensor.Reset();
    mp_Hood = new CDeviceHood(m_DeviceProcessing, m_DevFctModList, DEVICE_INSTANCE_ID_HOOD);

    if (!connect(&m_MockDcp, SIGNAL(Configure()), 
                  mp_Hood, SIGNAL(Configure()))) {
        QFAIL("Cannot connect Configure signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(Initialize()), 
                  mp_Hood, SIGNAL(Initialize()))) {
        QFAIL("Cannot connect Initialize signal.");
    }
    if (!connect(&m_MockDcp, SIGNAL(IsClosed()), 
                  mp_Hood, SIGNAL(IsClosed()))) {
        QFAIL("Cannot connect IsClosed signal.");
    }

    // give instantiated state machine a chance to start and process events
    QCoreApplication::processEvents();

    THREAD_PROCESS();
}

void TestHood::cleanup() 
{  
    delete mp_Hood;
}

void TestHood::utTestHood()
{
    QSignalSpy SpyNeedInitialize(mp_Hood, SIGNAL( NeedInitialize(ReturnCode_t) ));
    QSignalSpy SpyOpened(mp_Hood, SIGNAL( Opened() ));
    QSignalSpy SpyClosed(mp_Hood, SIGNAL( Closed() ));
    QSignalSpy SpyReportIsClosed(mp_Hood, SIGNAL( ReportIsClosed(ReturnCode_t, bool) ));

    m_MockDcp.EmitConfigure();
    QVERIFY(mp_Hood->GetStates().contains("Configured"));

    m_MockDcp.EmitInitialize();
    QVERIFY(mp_Hood->GetStates().contains("Checking"));

    // task IsClosed is not valid in this state
    m_MockDcp.EmitIsClosed();
    QCOMPARE( SpyReportIsClosed.count(), 1 );
    QCOMPARE( SpyReportIsClosed.at(0).at(0).value<ReturnCode_t>(), 
        DCL_ERR_INVALID_STATE );

    m_HoodSensor.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    QVERIFY(mp_Hood->GetStates().contains("Open"));
    QCOMPARE( SpyOpened.count(), 1 );

    // three signals triggered by reed relais
    m_HoodSensor.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY(mp_Hood->GetStates().contains("Closed"));
    QCOMPARE( SpyClosed.count(), 1 );

    m_HoodSensor.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QVERIFY(mp_Hood->GetStates().contains("Closed"));
    QCOMPARE( SpyClosed.count(), 1 );

    m_HoodSensor.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 1);
    QVERIFY(mp_Hood->GetStates().contains("Open"));
    QCOMPARE( SpyOpened.count(), 2 );

    QCOMPARE( SpyNeedInitialize.count(), 0 );
    QCOMPARE( SpyReportIsClosed.count(), 1 );

    // use IsClosed task
    m_MockDcp.EmitIsClosed();
    QCOMPARE( SpyReportIsClosed.count(), 2 );
    QCOMPARE( SpyReportIsClosed.at(1).at(0).value<ReturnCode_t>(), 
        DCL_ERR_FCT_CALL_SUCCESS );
    QCOMPARE( SpyReportIsClosed.at(1).at(1).value<bool>(), false );

    // error enforces Configured state
    m_HoodSensor.EmitReportActInputValue(1234, DCL_ERR_FCT_CALL_FAILED, 0);
    QCOMPARE( SpyNeedInitialize.count(), 1 );
    QVERIFY(mp_Hood->GetStates().contains("Configured"));
}


} // end namespace DeviceControl

QTEST_MAIN(DeviceControl::TestHood)

#include "TestHood.moc"

// vi: set ts=4 sw=4 et:

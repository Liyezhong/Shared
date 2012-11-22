#include <QTest>
#include <QSignalSpy>
#include "Drawer.h"
#include "MockFm.h"

#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

namespace DeviceControl {

class CMockDcp : public QObject
{
Q_OBJECT
public:
    CMockDcp(QObject *parent = 0) : QObject(parent) {}
    void EmitConfigure() { emit Configure(); }
    void EmitInitialize() { emit Initialize(); }
    void EmitReadRfid(LoaderRFIDChannel_t Channel)
        { emit ReadRfid( Channel ); }
Q_SIGNALS:
    void Configure();
    void Initialize();
    void ReadRfid(LoaderRFIDChannel_t);
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

private: 
    CDrawer *mp_Drawer;
    CMockDcp m_MockDcp;
    CMockFm m_MockFm;
};

TestDrawer::TestDrawer() : mp_Drawer(NULL)
{
}
TestDrawer::~TestDrawer()
{
}

void TestDrawer::init() 
{  
    m_MockFm.Reset();
    mp_Drawer = new CDrawer(&m_MockFm);

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

    QCoreApplication::processEvents();
}

void TestDrawer::cleanup() 
{  
    delete mp_Drawer;
}

void TestDrawer::utTest() 
{  
    QVERIFY( mp_Drawer->GetStates().contains("Start") );

    m_MockDcp.EmitConfigure();
    QCoreApplication::processEvents();

    // Request ReadRfid in wrong state    
    QSignalSpy nackSpy(mp_Drawer, SIGNAL( ReportReadRackRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32) ));
    m_MockDcp.EmitReadRfid(LOADER_ID_RFID_1);
    QCoreApplication::processEvents();
    QCOMPARE( nackSpy.count(), 1 );
    QCOMPARE( nackSpy.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_INVALID_STATE );
    m_MockDcp.EmitInitialize();
    QCoreApplication::processEvents();

    m_MockFm.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QCoreApplication::processEvents();
    m_MockFm.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCoreApplication::processEvents();
    m_MockFm.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCoreApplication::processEvents();
    m_MockFm.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCoreApplication::processEvents();
    QVERIFY( mp_Drawer->GetStates().contains("Working") );

    QSignalSpy ackSpy(mp_Drawer, SIGNAL( ReportReadRackRfid(ReturnCode_t, LoaderRFIDChannel_t, quint32, quint32) ));

    m_MockDcp.EmitReadRfid(LOADER_ID_RFID_1);
    QCoreApplication::processEvents();
    QCOMPARE( ackSpy.count(), 0 );

    m_MockFm.EmitReportStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QCoreApplication::processEvents();
    m_MockFm.EmitReportLoginAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QCoreApplication::processEvents();
    m_MockFm.EmitReportUID(1234, DCL_ERR_FCT_CALL_SUCCESS, 123456);
    QCoreApplication::processEvents();
    m_MockFm.EmitReportUserData(1234, DCL_ERR_FCT_CALL_SUCCESS, 0, 654321);
    QCoreApplication::processEvents();

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
    QCoreApplication::processEvents();
    QVERIFY( mp_Drawer->GetStates().contains("Configured") );

    m_MockDcp.EmitInitialize();
    QCoreApplication::processEvents();
    QVERIFY( mp_Drawer->GetStates().contains("StepperInitSetState") );

    m_MockFm.EmitReportSetStateAckn(1234, DCL_ERR_FCT_CALL_SUCCESS);
    QCoreApplication::processEvents();
    QVERIFY( mp_Drawer->GetStates().contains("StepperInitRefRun") );
    
    m_MockFm.EmitReportReferenceMovementAckn(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCoreApplication::processEvents();
    m_MockFm.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCoreApplication::processEvents();
    
    QSignalSpy initSpy(mp_Drawer, SIGNAL(ReportInitialize(ReturnCode_t)));

    m_MockFm.EmitReportActOutputValue(1234, DCL_ERR_FCT_CALL_SUCCESS, 0);
    QCoreApplication::processEvents();

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

    m_MockFm.SetNextError();
    m_MockDcp.EmitInitialize();
    QCoreApplication::processEvents();
    QVERIFY( mp_Drawer->GetStates().contains("Configured") );
    QCOMPARE( initSpy.count(), 1 );
    QCOMPARE( initSpy.at(0).at(0).value<ReturnCode_t>(), DCL_ERR_FCT_CALL_FAILED );
}


} // end namespace DeviceControl

QTEST_MAIN(DeviceControl::TestDrawer)

#include "TestDrawer.moc"

// vi: set ts=4 sw=4 et:

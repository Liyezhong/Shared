#include <QTest>
#include <QObject>
#include <QDebug>

#include "Interface/IDeviceProcessing.h"
#include "Global/Include/GlobalDefines.h"
#include <Global/Include/SystemPaths.h>
#include <Global/Include/Utils.h>

// Run exec for a maximum of TIMEOUT msecs
#define QCOREAPPLICATION_EXEC(TIMEOUT) \
{ \
    QTimer timer; \
    timer.setSingleShot(true); \
    timer.setInterval(TIMEOUT); \
    timer.start(); \
    connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit())); \
    QCoreApplication::exec(); \
}

namespace DeviceControl {

class TestIDeviceProcessing : public QObject {
    Q_OBJECT
public:
    TestIDeviceProcessing()
    {
        Global::SystemPaths::Instance().SetSettingsPath("../../../../../../Himalaya/HimalayaMain/Master/Components/Main/Build/Settings");
        p_IDeviceProcessing = new IDeviceProcessing;

        CONNECTSIGNALSLOT(p_IDeviceProcessing, ReportInitializationFinished(quint32, ReturnCode_t),
                          this, DevProcInitialisationAckn(quint32, ReturnCode_t));
        CONNECTSIGNALSLOT(p_IDeviceProcessing, ReportConfigurationFinished(quint32, ReturnCode_t),
                          this, DevProcConfigurationAckn(quint32, ReturnCode_t));
        CONNECTSIGNALSLOT(p_IDeviceProcessing, ReportStartNormalOperationMode(quint32, ReturnCode_t),
                          this, DevProcStartNormalOpModeAckn(quint32, ReturnCode_t));
        CONNECTSIGNALSLOT(p_IDeviceProcessing, ReportError(quint32, quint16, quint16, quint16, const QDateTime &),
                          this, ThrowError(quint32, quint16, quint16, quint16, const QDateTime &));
        CONNECTSIGNALSLOT(p_IDeviceProcessing, ReportDestroyFinished(), this, DevProcDestroyAckn());

    }
    ~TestIDeviceProcessing()
    {
        delete p_IDeviceProcessing;
    }

public slots:

    void DevProcInitialisationAckn(quint32 /*a*/, ReturnCode_t /*b*/)
    {
        qDebug() << "ReportInitializationFinished";
    }

    void DevProcConfigurationAckn(quint32 /*a*/, ReturnCode_t /*b*/)
    {
        qDebug() << "ReportConfigurationFinished";
    }

    void DevProcStartNormalOpModeAckn(quint32 /*a*/, ReturnCode_t /*b*/)
    {
        qDebug() << "ReportStartNormalOperationMode";
    }

    void ThrowError(quint32 /*a*/, quint16 /*b*/, quint16 /*c*/, quint16 /*d */, const QDateTime &/*time*/)
    {

    }

    void DevProcDestroyAckn()
    {
    }

private slots:
    void initTestCase();
    void caseGetSerialNumber();
    void caseDiagnosticService();
    void caseALSetPressureCtrl();
    void caseALTurnFan();
    void caseALVaccum();
    void caseALDraining();
    void caseALFilling();
    void caseRTLock();
    void casePerMainRelay();
    void caseGetRecentValues();

    void cleanupTestCase();

public:
    IDeviceProcessing *p_IDeviceProcessing;
};

void TestIDeviceProcessing::initTestCase()
{
    QVERIFY(p_IDeviceProcessing);
    QVERIFY(Global::SystemPaths::Instance().GetSettingsPath().length() != 0);
    QCOREAPPLICATION_EXEC(1000);
    //! Restart device control layer configuration
    QVERIFY(p_IDeviceProcessing->RestartConfigurationService() == DCL_ERR_FCT_CALL_SUCCESS);
    QCOREAPPLICATION_EXEC(10000);
}

void TestIDeviceProcessing::caseDiagnosticService()
{
    //! Start device control layer diagnstic service
    QVERIFY(p_IDeviceProcessing->StartDiagnosticService() ==  DCL_ERR_FCT_CALL_SUCCESS);
    //! Finisch device control layer diagnostic service
    QVERIFY(p_IDeviceProcessing->CloseDiagnosticService() ==  DCL_ERR_FCT_CALL_SUCCESS);
}

void TestIDeviceProcessing::caseGetSerialNumber()
{
    QString serialNo;
    IDeviceProcessing::GetSerialNumber(serialNo);

    if (serialNo.length())
        qDebug() << "  Serial number is " << serialNo;

    QVERIFY(serialNo.length() != 0);
}

void TestIDeviceProcessing::caseALSetPressureCtrl()
{
    QCOREAPPLICATION_EXEC(20000);
    QCOMPARE(p_IDeviceProcessing->ALSetPressureCtrlON(), DCL_ERR_FCT_CALL_SUCCESS);
    QCOREAPPLICATION_EXEC(1000);
    QCOMPARE(p_IDeviceProcessing->ALSetPressureCtrlOFF(), DCL_ERR_FCT_CALL_SUCCESS);
    QCOREAPPLICATION_EXEC(1000);
    QCOMPARE(p_IDeviceProcessing->ALReleasePressure(), DCL_ERR_FCT_CALL_SUCCESS);
}

void TestIDeviceProcessing::caseALTurnFan()
{
    QCOMPARE(p_IDeviceProcessing->ALTurnOnFan(), DCL_ERR_FCT_CALL_SUCCESS);
    QCOREAPPLICATION_EXEC(1000);
    QCOMPARE(p_IDeviceProcessing->ALTurnOnFan(), DCL_ERR_FCT_CALL_SUCCESS);
    QCOREAPPLICATION_EXEC(1000);
}

void TestIDeviceProcessing::caseALVaccum()
{
    QCOMPARE(p_IDeviceProcessing->ALVaccum(), DCL_ERR_FCT_CALL_SUCCESS);
    QCOREAPPLICATION_EXEC(1000);
}

void TestIDeviceProcessing::caseALDraining()
{
    QCOMPARE(p_IDeviceProcessing->ALDraining(100), DCL_ERR_FCT_CALL_SUCCESS);
}

void TestIDeviceProcessing::caseALFilling()
{
    QCOMPARE(p_IDeviceProcessing->ALFilling(100, false), DCL_ERR_FCT_CALL_SUCCESS);
    QCOMPARE(p_IDeviceProcessing->ALStopCmdExec(2), DCL_ERR_FCT_CALL_SUCCESS);
    QCOMPARE(p_IDeviceProcessing->ALFillingForService(2, false), DCL_ERR_FCT_CALL_SUCCESS);
}

void TestIDeviceProcessing::caseRTLock()
{
    QCOMPARE(p_IDeviceProcessing->RTLock(), DCL_ERR_FCT_CALL_SUCCESS);
    QCOMPARE(p_IDeviceProcessing->RTUnlock(), DCL_ERR_FCT_CALL_SUCCESS);
}

void TestIDeviceProcessing::casePerMainRelay()
{
    QCOMPARE(p_IDeviceProcessing->PerTurnOnMainRelay(), DCL_ERR_FCT_CALL_SUCCESS);
    QCOMPARE(p_IDeviceProcessing->PerTurnOffMainRelay(), DCL_ERR_FCT_CALL_SUCCESS);
}

void TestIDeviceProcessing::caseGetRecentValues()
{
    qreal value = p_IDeviceProcessing->ALGetRecentPressure();
    value = p_IDeviceProcessing->ALGetRecentTemperature(AL_LEVELSENSOR,0);
    value = p_IDeviceProcessing->ALGetTemperatureControlState(AL_LEVELSENSOR);
    value = p_IDeviceProcessing->RTGetRecentLockStatus();
    value = p_IDeviceProcessing->RTGetRecentTemperature(RT_BOTTOM,0);
    value = p_IDeviceProcessing->OvenGetHeatingStatus(OVEN_BOTTOM);
    value = p_IDeviceProcessing->OvenGetRecentLidStatus();
    value = p_IDeviceProcessing->OvenGetRecentTemperature(OVEN_BOTTOM,0);
    value = p_IDeviceProcessing->RVGetRecentTemperature(0);
    value = p_IDeviceProcessing->RVGetTemperatureControlState();
    value = p_IDeviceProcessing->IDGetSlaveCurrent(Slave_3);
    value = p_IDeviceProcessing->IDGetSlaveVoltage(Slave_3);
    value = p_IDeviceProcessing->IDGetRemoteAlarmStatus();
    value = p_IDeviceProcessing->IDGetLocalAlarmStatus();
    QCOMPARE(p_IDeviceProcessing->IDSetAlarm(1), DCL_ERR_FCT_CALL_SUCCESS);
}

void TestIDeviceProcessing::cleanupTestCase()
{
//
}

} // end namespace

QTEST_MAIN(DeviceControl::TestIDeviceProcessing)

#include "TestIDeviceProcessing.moc"




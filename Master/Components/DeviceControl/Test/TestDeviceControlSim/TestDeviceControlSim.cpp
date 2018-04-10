#include <QString>
#include <QtTest>
#include <

class TestDeviceControlSimTest : public QObject
{
    Q_OBJECT

public:
    TestDeviceControlSimTest();

private Q_SLOTS:
    void initTestCase();
    void utGetHwconfig();
    void cleanup();

private:
    DeviceControSim* m_pDeviceControlSim;
};

TestDeviceControlSimTest::TestDeviceControlSimTest()
{
}

void initTestCase()
{

}

void TestDeviceControlSimTest::utGetHwconfig()
{
    QVERIFY2(true, "Failure");
}

void TestDeviceControlSimTest::cleanup()
{

}

QTEST_APPLESS_MAIN(TestDeviceControlSimTest)

#include "tst_testdevicecontrolsimtest.moc"

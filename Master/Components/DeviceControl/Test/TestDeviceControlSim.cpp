#include <QString>
#include <QtTest>
#include "DeviceControl/Include/Simulation/DeviceControlSim.h"
#include <Global/Include/SystemPaths.h>

namespace DeviceControl {

class TestDeviceControlSim : public QObject
{
    Q_OBJECT

public:
    TestDeviceControlSim();

private Q_SLOTS:
    void initTestCase();
    void utGetHwconfig();
    void cleanupTestCase();

private:
    DeviceControlSim* m_pDeviceControlSim;
};

TestDeviceControlSim::TestDeviceControlSim()
{
}

void TestDeviceControlSim::utGetHwconfig()
{
//    hwconfigType* config = NULL;

    hwconfigType* config = m_pDeviceControlSim->GetDeviceConfig();

    std::for_each(config->parameter_master().retorts().retort().begin(), config->parameter_master().retorts().retort().end(), [](retortType& retort)
    {

        qDebug() << "Retort Name: " + QString::fromStdString(retort.name());
        std::for_each(retort.devices().device().begin(), retort.devices().device().end(), [](deviceType& device)
        {
           qDebug() << "Device Name: " + QString::fromStdString(device.name()) + " InstanceId: " + QString::fromStdString(device.id());
        });
    });

    QVERIFY2((*(*config->parameter_master().retorts().retort().begin()).devices().device().begin()).name() == "RotaryValveDevice", "Failure");


}

void TestDeviceControlSim::initTestCase()
{
    Global::SystemPaths::Instance().SetSettingsPath(QDir::currentPath() + "/../../../../../../Master/Components/Main/Build/Settings/");
    m_pDeviceControlSim = new DeviceControlSim(2);
}

void TestDeviceControlSim::cleanupTestCase()
{
    if(m_pDeviceControlSim != NULL)
        delete m_pDeviceControlSim;
}
}

QTEST_APPLESS_MAIN(DeviceControl::TestDeviceControlSim)


#include "TestDeviceControlSim.moc"

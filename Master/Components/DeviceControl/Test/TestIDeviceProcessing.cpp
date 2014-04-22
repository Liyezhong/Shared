#include <unistd.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#include <QTest>
#include <QObject>

#include "Interface/IDeviceProcessing.h"


namespace DeviceControl {

class TestIDeviceProcessing : public QObject {
    Q_OBJECT
public:
    TestIDeviceProcessing() : p_IDeviceProcessing(new IDeviceProcessing)
    {

    }
    ~TestIDeviceProcessing()
    {
        delete p_IDeviceProcessing;
    }

public slots:
    void initTestCase();
    void cleanupTestCase();

public:
    IDeviceProcessing *p_IDeviceProcessing;
};

void TestIDeviceProcessing::initTestCase()
{
    QCOMPARE(p_IDeviceProcessing->ALSetPressureCtrlON(), DCL_ERR_NOT_INITIALIZED);
}

void TestIDeviceProcessing::cleanupTestCase()
{

}

} // end namespace

QTEST_MAIN(DeviceControl::TestIDeviceProcessing)

#include "TestIDeviceProcessing.moc"




#include <QtCore>
#include <QCoreApplication>
#include <QTest>
#include <iostream>
#include <DeviceControl/Test/TestDeviceControlBase.h>
#include <DeviceControl/Test/TestDeviceControlGrappler.h>
#include <DeviceControl/Test/TestDeviceControlLoader.h>
#include <DeviceControl/Test/TestDeviceControlAgitation.h>
#include <DeviceControl/Test/TestDeviceControlOven.h>
#include <DeviceControl/Test/TestDeviceControlWater.h>
#include <DeviceControl/Test/TestDeviceControlCANNode.h>

/****************************************************************************/
 /*! \brief  Main function of the DeviceControlLayer unit test project.
 *
 *           This function executes the stuff usually is done by calling the QTEST_MAIN macro.
 *
 *           In opposite to the macro, here all unit test are executed one after another while the
 *           TestDeviceControlBase class is still alive. This class keeps the DeviceControlLayer and
 *           the ColoPiaSimulator running, wich is fundamental for all the tests.
 *
 *           The function creates the instances of the test classes and sets the
 *           response interface to them.
 *           The tests needs the ColoPiaSimulator for CAN communication simulation.
 *           ColoPiaSimulator will be started, and finally finished, by TestDeviceControlBase
 *
 *           The test are executed by calling the QTest::qExec(..) function with
 *           each test class as an argument.
 *
 *
 */
/****************************************************************************/
int main(int argc, char *argv[]) \
{ \
    int TestResult;
    QCoreApplication app(argc, argv);

    // test classes used by DeviceControlLayer unit test
    DeviceControl::TestDeviceControlBase       TestObjectDeviceControlBase;
    DeviceControl::TestDeviceControlGrappler   TestObjectDeviceControlGrappler;
    DeviceControl::TestDeviceControlLoader     TestObjectDeviceControlLoader;
    DeviceControl::TestDeviceControlAgitation  TestObjectDeviceControlAgitation;
    DeviceControl::TestDeviceControlOven       TestObjectDeviceControlOven;
    DeviceControl::TestDeviceControlWater      TestObjectDeviceControlWater;
    DeviceControl::TestDeviceControlCANNode    TestObjectDeviceControlCANNode;

    // The TestObjectDeviceControlBase creates and configures the DeviceControlLayer
    // ColopiaSimulater will be started as well
    TestResult = QTest::qExec(&TestObjectDeviceControlBase, argc, argv);

    // Forward the response class to all other test classes
    TestObjectDeviceControlGrappler.SetDeviceProcessingResponse(TestObjectDeviceControlBase.GetDeviceProcessingResponse());
    TestObjectDeviceControlLoader.SetDeviceProcessingResponse(TestObjectDeviceControlBase.GetDeviceProcessingResponse());
    TestObjectDeviceControlAgitation.SetDeviceProcessingResponse(TestObjectDeviceControlBase.GetDeviceProcessingResponse());
    TestObjectDeviceControlOven.SetDeviceProcessingResponse(TestObjectDeviceControlBase.GetDeviceProcessingResponse());
    TestObjectDeviceControlWater.SetDeviceProcessingResponse(TestObjectDeviceControlBase.GetDeviceProcessingResponse());
    TestObjectDeviceControlCANNode.SetDeviceProcessingResponse(TestObjectDeviceControlBase.GetDeviceProcessingResponse());

    //Execute the tests by calling the QTest::qExec function
    TestResult = QTest::qExec(&TestObjectDeviceControlGrappler, argc, argv);
    TestResult = QTest::qExec(&TestObjectDeviceControlLoader, argc, argv);
    TestResult = QTest::qExec(&TestObjectDeviceControlAgitation, argc, argv);
    TestResult = QTest::qExec(&TestObjectDeviceControlOven, argc, argv);
    TestResult = QTest::qExec(&TestObjectDeviceControlWater, argc, argv);
    TestResult = QTest::qExec(&TestObjectDeviceControlCANNode, argc, argv);

    // Shutdown the DeviceControlLayer and kill the ColoPiaSimulater
    TestObjectDeviceControlBase.CloseTest();

    return TestResult;
}

#ifndef TESTDEVICECONTROLLOADER_H
#define TESTDEVICECONTROLLOADER_H
#include <QObject>
#include <QProcess>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"

namespace DeviceControl {

class DeviceProcessingResponse;

/****************************************************************************/
/**
 * \brief Test class for Loader device.
 */
/****************************************************************************/
class TestDeviceControlLoader: public QObject
{
  Q_OBJECT

public:

    TestDeviceControlLoader();
    ~TestDeviceControlLoader();

    void SetDeviceProcessingResponse(DeviceProcessingResponse* pDeviceProcessingResponse) { m_pDeviceProcessingResponse = pDeviceProcessingResponse;};

public slots:
    void OnReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, LoaderPosition_t Position);
    void OnReportLoaderBlockingState(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, BlockState_t BlockingState);

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    ///< \warning: Each test case builds on the output of the previous one.
    ///< All tests shall be run exactly in the order as they are listed here:
    void utTestReferenceRuns(); ///< Test 1
    void utTestPositioning();   ///< Test 2
    void utTestBlocking();      ///< Test 2


private:

    DeviceProcessingResponse*  m_pDeviceProcessingResponse;
    ILoaderDevice* m_pILoaderDevice;
    ILoaderDevice* m_pIUnloaderDevice;

    ReturnCode_t m_HdlInfoReferenceRunLoader;
    ReturnCode_t m_HdlInfoReferenceRunUnloader;

    ReturnCode_t m_HdlInfoBlockStateLoader;
    ReturnCode_t m_HdlInfoBlockStateUnloader;

    BlockState_t  m_BlockingStateLoader;
    BlockState_t  m_BlockingStateUnloader;

};

} // end namespace ExternalProcessControl
#endif // TESTDEVICECONTROLLOADER_H

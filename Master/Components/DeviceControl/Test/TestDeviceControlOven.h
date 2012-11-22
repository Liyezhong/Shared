#ifndef TESTDEVICECONTROLOVEN_H
#define TESTDEVICECONTROLOVEN_H
#include <QObject>
#include <QProcess>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"

namespace DeviceControl {

class DeviceProcessingResponse;

/****************************************************************************/
/**
 * \brief Test class for Oven device.
 */
/****************************************************************************/
class TestDeviceControlOven: public QObject
{
  Q_OBJECT

public:

    TestDeviceControlOven();
    ~TestDeviceControlOven();

    void SetDeviceProcessingResponse(DeviceProcessingResponse* pDeviceProcessingResponse) { m_pDeviceProcessingResponse = pDeviceProcessingResponse;};

public slots:
    void OnOvenReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, OvenCoverPosition_t Position);


private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void utTestReferenceRuns(); ///< Test 1


private:

    DeviceProcessingResponse*  m_pDeviceProcessingResponse;
    IOvenDevice* m_pIOvenDevice;

    ReturnCode_t m_HdlInfoReferenceRunAckn;

};

} // end namespace DeviceControl

#endif // TESTDEVICECONTROLOVEN_H

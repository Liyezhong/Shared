#ifndef TESTDEVICECONTROLGRAPPLER_H
#define TESTDEVICECONTROLGRAPPLER_H
#include <QObject>
#include <QProcess>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
namespace DeviceControl {

class DeviceProcessingResponse;

/****************************************************************************/
/**
 * \brief Test class for Grappler device.
 */
/****************************************************************************/
class TestDeviceControlGrappler: public QObject
{
  Q_OBJECT

public:

    TestDeviceControlGrappler();
    ~TestDeviceControlGrappler();

    void SetDeviceProcessingResponse(DeviceProcessingResponse* pDeviceProcessingResponse) { m_pDeviceProcessingResponse = pDeviceProcessingResponse;};

public slots:
    void OnGrapplerReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo);
    void OnGrapplerPositioningAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo);

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    ///< \warning: Each test case builds on the output of the previous one.
    ///< All tests shall be run exactly in the order as they are listed here:
    void utTestReferenceRuns();       ///< Test 1
    void utTestGrapplerPositioningEmptyMove(); ///< Test 2
    void utTestGrapplerPositioningAttachToRack(); ///< Test 2
    void utTestGrapplerPositioningPullUpRack(); ///< Test 2
    void utTestGrapplerPositioningTransportRackTo(); ///< Test 2
    void utTestGrapplerPositioningLetdownRack();
    void utTestGrapplerPositioningDetachRack();
     void utTestGrapplerPositioningEmptyMove2(); ///< Test 2


private:

    DeviceProcessingResponse*  m_pDeviceProcessingResponse;

    IGrapplerDevice* m_pIGrappler1;   //!< left or rigth grappler
    IGrapplerDevice* m_pIGrappler2;   //!< the other grappler

    ReturnCode_t m_HdlInfoReferenceRunGrappler1;
    ReturnCode_t m_HdlInfoReferenceRunGrappler2;
    ReturnCode_t m_HdlInfoPositioningGrappler1;
    ReturnCode_t m_HdlInfoPositioningGrappler2;


};

} // end namespace ExternalProcessControl

#endif // TESTDEVICECONTROLGRAPPLER_H

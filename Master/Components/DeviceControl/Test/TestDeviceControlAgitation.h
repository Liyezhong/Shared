#ifndef TESTDEVICECONTROLAGITATION_H
#define TESTDEVICECONTROLAGITATION_H
#include <QObject>
#include <QProcess>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"

namespace DeviceControl {

class DeviceProcessingResponse;

/****************************************************************************/
/**
 * \brief Test class for Agitation device.
 */
/****************************************************************************/
class TestDeviceControlAgitation: public QObject
{
  Q_OBJECT

public:

    TestDeviceControlAgitation();
    ~TestDeviceControlAgitation();

    void SetDeviceProcessingResponse(DeviceProcessingResponse* pDeviceProcessingResponse) { m_pDeviceProcessingResponse = pDeviceProcessingResponse;};

public slots:
    void OnAgitationReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationPosition_t Position);
    void OnAgitationActSpeed(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationSpeed_t Speed);
    void OnAgitationActPosition(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationPosition_t Position);


private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void utTestReferenceRuns(); ///< Test 1
    void utTestSpeed1();        ///< Test 2
    void utTestStop();         ///< Test 3
    void utTestSpeed2();        ///< Test 4
    void utTestStop2();         ///< Test 5


private:

    DeviceProcessingResponse*  m_pDeviceProcessingResponse;
    IAgitationDevice* m_pIAgitationDevice;

    ReturnCode_t m_HdlInfoReferenceRunAckn;
    ReturnCode_t m_HdlInfoActSpeed;
    ReturnCode_t m_HdlInfoActPosition;


};

} // end namespace DeviceControl

#endif // TESTDEVICECONTROLAGITATION_H

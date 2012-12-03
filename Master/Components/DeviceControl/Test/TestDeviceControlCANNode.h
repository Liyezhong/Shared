#ifndef TESTDEVICECONTROLCANNODE_H
#define TESTDEVICECONTROLCANNODE_H
#include <QObject>
#include <QProcess>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"

namespace DeviceControl {

class DeviceProcessingResponse;

/****************************************************************************/
/**
 * \brief Test class for CANNode.
 */
/****************************************************************************/
class TestDeviceControlCANNode: public QObject
{
  Q_OBJECT

public:

    TestDeviceControlCANNode();
    ~TestDeviceControlCANNode();

    void SetDeviceProcessingResponse(DeviceProcessingResponse* pDeviceProcessingResponse) { m_pDeviceProcessingResponse = pDeviceProcessingResponse;};

public slots:
    void OnReportEndTestResult(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 TestResult, quint8 TestYear, quint8 TestMonth, quint8 TestDay);
    void OnReportHWInfo(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 VersionMajor, quint8 VersionMinor, quint8 Year, quint8 Month, quint8 Day);
    void OnReportSWInfo(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 Version, quint8 Year, quint8 Month, quint8 Day);
    void OnReportLifeCycleData(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 StepCounter, quint32 TripCounter, quint32 OperationTime);

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void utTestCANNodeAccess();
    void utTestCANNode();
    void utTestCANNodeFctModules();


private:

    DeviceProcessingResponse*  m_pDeviceProcessingResponse;
    CBaseModule* m_pICANNode;

    ReturnCode_t m_HdlInfoTestResult;

    ReturnCode_t m_HdlInfoHWInfo;
    quint8       m_HWVersionMajor;
    quint8       m_HWVersionMinor;

    ReturnCode_t m_HdlInfoSWInfo;
    quint16      m_SWVersion;

    ReturnCode_t m_HdlInfoLifeCyleData;

};

} // end namespace DeviceControl

#endif // TESTDEVICECONTROLCANNODE_H

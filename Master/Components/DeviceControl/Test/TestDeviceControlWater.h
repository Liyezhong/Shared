#ifndef TESTDEVICECONTROLWATER_H
#define TESTDEVICECONTROLWATER_H
#include <QObject>
#include <QProcess>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"

namespace DeviceControl {

class DeviceProcessingResponse;

/****************************************************************************/
/**
 * \brief Test class for Water device.
 */
/****************************************************************************/
class TestDeviceControlWater: public QObject
{
  Q_OBJECT

public:

    TestDeviceControlWater();
    ~TestDeviceControlWater();

    void SetDeviceProcessingResponse(DeviceProcessingResponse* pDeviceProcessingResponse) { m_pDeviceProcessingResponse = pDeviceProcessingResponse;};

public slots:
    void OnReportWaterValveStatus(DevInstanceID_t, ReturnCode_t, WaterValveID_t, quint8);
    void OnReportWaterLiquidLevel(DevInstanceID_t, ReturnCode_t, quint16);

private slots:

    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void utTestWaterValves();


private:

    DeviceProcessingResponse*  m_pDeviceProcessingResponse;
    IWaterDevice* m_pIWaterDevice;

    ReturnCode_t   m_HdlInfoLiquidLevel;
    ReturnCode_t   m_HdlInfoValveStatusAckn1;
    ReturnCode_t   m_HdlInfoValveStatusAckn2;
    ReturnCode_t   m_HdlInfoValveStatusAckn3;
    ReturnCode_t   m_HdlInfoValveStatusAckn4;
    ReturnCode_t   m_HdlInfoValveStatusAckn5;
    ReturnCode_t   m_HdlInfoValveStatusAckn6;
    quint8         m_ValveState;
    WaterValveID_t m_ValveID;

};

} // end namespace DeviceControl

#endif // TESTDEVICECONTROLWATER_H

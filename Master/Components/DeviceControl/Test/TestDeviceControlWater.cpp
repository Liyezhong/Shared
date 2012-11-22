#include <unistd.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#include <QTest>
#include <DeviceControl/Test/TestDeviceControlWater.h>
#include <DeviceControl/Test/DeviceControlResponse.h>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/Interface/Devices/IWaterDevice.h"

namespace DeviceControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestDeviceControlWater::TestDeviceControlWater()
{

}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestDeviceControlWater::~TestDeviceControlWater()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlWater::initTestCase()
{
    if(m_pDeviceProcessingResponse)
    {
        IDeviceProcessing* pIDeviceProcessing;

        pIDeviceProcessing = m_pDeviceProcessingResponse->GetIDeviceProcessing();
        if(pIDeviceProcessing != 0)
        {
            m_pIWaterDevice = pIDeviceProcessing->GetIWaterDevice();

            connect(m_pDeviceProcessingResponse, SIGNAL(ReportWaterLiquidLevel(DevInstanceID_t, ReturnCode_t, quint16)), this, SLOT(OnReportWaterLiquidLevel(DevInstanceID_t, ReturnCode_t, quint16)));
            connect(m_pDeviceProcessingResponse, SIGNAL(ReportWaterValveStatus(DevInstanceID_t, ReturnCode_t, WaterValveID_t, quint8)), this, SLOT(OnReportWaterValveStatus(DevInstanceID_t, ReturnCode_t, WaterValveID_t, quint8)));
        }
    }

    QVERIFY(m_pIWaterDevice != 0);
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlWater::init()
{

}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlWater::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlWater::cleanupTestCase()
{

}

void TestDeviceControlWater::utTestWaterValves()
{
    ReturnCode_t RetValSetValve;
    ReturnCode_t RetValReqLiquid;
    ReturnCode_t RetValReqValveStatus;

    RetValSetValve = m_pIWaterDevice->SetValveStatus(WATER_VALVE_ID_1, 1);
    QVERIFY(RetValSetValve == DCL_ERR_FCT_CALL_SUCCESS);
    RetValSetValve = m_pIWaterDevice->SetValveStatus(WATER_VALVE_ID_2, 1);
    QVERIFY(RetValSetValve == DCL_ERR_FCT_CALL_SUCCESS);
    RetValSetValve = m_pIWaterDevice->SetValveStatus(WATER_VALVE_ID_3, 1);
    QVERIFY(RetValSetValve == DCL_ERR_FCT_CALL_SUCCESS);
    RetValSetValve = m_pIWaterDevice->SetValveStatus(WATER_VALVE_ID_4, 1);
    QVERIFY(RetValSetValve == DCL_ERR_FCT_CALL_SUCCESS);
    RetValSetValve = m_pIWaterDevice->SetValveStatus(WATER_VALVE_ID_5, 1);
    QVERIFY(RetValSetValve == DCL_ERR_FCT_CALL_SUCCESS);
    RetValSetValve = m_pIWaterDevice->SetValveStatus(WATER_VALVE_ID_6, 1);
    QVERIFY(RetValSetValve == DCL_ERR_FCT_CALL_SUCCESS);


    //spend some time to proceed the request
    QTest::qWait(1000);

    RetValReqLiquid = m_pIWaterDevice->ReqLiquidLevel();
    QVERIFY(RetValReqLiquid == DCL_ERR_FCT_CALL_SUCCESS);
    QTest::qWait(200);

    QVERIFY(m_HdlInfoLiquidLevel == DCL_ERR_FCT_CALL_SUCCESS);

    m_HdlInfoValveStatusAckn2 = DCL_ERR_UNDEFINED;
    RetValReqValveStatus = m_pIWaterDevice->ReqValveStatus(WATER_VALVE_ID_2);
    QVERIFY(RetValReqValveStatus == DCL_ERR_FCT_CALL_SUCCESS);
    QTest::qWait(200);

    QVERIFY(m_HdlInfoValveStatusAckn2 == DCL_ERR_FCT_CALL_SUCCESS);
    QVERIFY(m_ValveState == 0x01);
    QVERIFY(m_ValveID == m_ValveID);

}

void TestDeviceControlWater::OnReportWaterValveStatus(DevInstanceID_t, ReturnCode_t HdlInfo, WaterValveID_t ValveID, quint8 ValveState)
{
    if(ValveID == WATER_VALVE_ID_1)
    {
        m_HdlInfoValveStatusAckn1 = HdlInfo;
    }
    if(ValveID == WATER_VALVE_ID_2)
    {
        m_HdlInfoValveStatusAckn2 = HdlInfo;
    }
    if(ValveID == WATER_VALVE_ID_3)
    {
        m_HdlInfoValveStatusAckn3 = HdlInfo;
    }
    if(ValveID == WATER_VALVE_ID_4)
    {
        m_HdlInfoValveStatusAckn4 = HdlInfo;
    }
    if(ValveID == WATER_VALVE_ID_5)
    {
        m_HdlInfoValveStatusAckn5 = HdlInfo;
    }
    if(ValveID == WATER_VALVE_ID_6)
    {
        m_HdlInfoValveStatusAckn6 = HdlInfo;
    }
    m_ValveState = ValveState;
}

void TestDeviceControlWater::OnReportWaterLiquidLevel(DevInstanceID_t, ReturnCode_t HdlInfo, quint16 LiquidLevel)
{
    Q_UNUSED(LiquidLevel);
    m_HdlInfoLiquidLevel = HdlInfo;
}


} // end namespace









#include <unistd.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#include <QTest>
#include <DeviceControl/Test/TestDeviceControlGrappler.h>
#include <DeviceControl/Test/DeviceControlResponse.h>
#include "DeviceControl/Include/Interface/Devices/IGrapplerDevice.h"

namespace DeviceControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestDeviceControlGrappler::TestDeviceControlGrappler() :
    m_pIGrappler1(0), m_pIGrappler2(0),
    m_HdlInfoReferenceRunGrappler1(DCL_ERR_FCT_CALL_FAILED), m_HdlInfoReferenceRunGrappler2(DCL_ERR_FCT_CALL_FAILED)
{

}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestDeviceControlGrappler::~TestDeviceControlGrappler()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlGrappler::initTestCase()
{


    m_pIGrappler1 = m_pDeviceProcessingResponse->GetIDeviceProcessing()->GetIGrapplerDevice(DEVICE_INSTANCE_ID_GRAPPLER_1);
    m_pIGrappler2 = m_pDeviceProcessingResponse->GetIDeviceProcessing()->GetIGrapplerDevice(DEVICE_INSTANCE_ID_GRAPPLER_2);

    connect(m_pDeviceProcessingResponse, SIGNAL(ReportGrapplerReferenceRunAckn(DevInstanceID_t, ReturnCode_t)), this, SLOT(OnGrapplerReferenceRunAckn(DevInstanceID_t, ReturnCode_t)));
    connect(m_pDeviceProcessingResponse, SIGNAL(ReportGrapplerPositioningAckn(DevInstanceID_t, ReturnCode_t)), this, SLOT(OnGrapplerPositioningAckn(DevInstanceID_t, ReturnCode_t)));

}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlGrappler::init()
{

}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlGrappler::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlGrappler::cleanupTestCase()
{

}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestDeviceControlGrappler::utTestReferenceRuns()
{
    if(m_pIGrappler1 && m_pIGrappler2)
    {
        ReturnCode_t RetCode;

        RetCode = m_pIGrappler1->ReqReferenceRun();
        QVERIFY(RetCode == DCL_ERR_FCT_CALL_SUCCESS);

        RetCode = m_pIGrappler2->ReqReferenceRun();
        QVERIFY(RetCode == DCL_ERR_FCT_CALL_SUCCESS);

        //spend some time to proceed the reference runs
        QTest::qWait(15000);

        QVERIFY(m_HdlInfoReferenceRunGrappler1 == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoReferenceRunGrappler2 == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlGrappler::OnGrapplerReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    if(InstanceID == DEVICE_INSTANCE_ID_GRAPPLER_1)
    {
        m_HdlInfoReferenceRunGrappler1 = HdlInfo;
    }
    else if(InstanceID == DEVICE_INSTANCE_ID_GRAPPLER_2)
    {
        m_HdlInfoReferenceRunGrappler2 = HdlInfo;
    }
}

void TestDeviceControlGrappler::utTestGrapplerPositioningEmptyMove()
{
    if(m_pDeviceProcessingResponse)
    {
        ReturnCode_t RetCode;
        Scheduler1::CStation Station1;
        Scheduler1::CStation Station2;

        Station1.SetColumn(11);
        Station1.SetRow(3);
        RetCode = m_pIGrappler1->ReqStationAction(STATION_ACTION_MOVE_EMTPY_TO, 3, 11);


        Station2.SetColumn(2);
        Station2.SetRow(9);
        RetCode = m_pIGrappler2->ReqStationAction(STATION_ACTION_MOVE_EMTPY_TO, 9, 2);

        //spend some time to proceed the reference runs
        QTest::qWait(10000);

        QVERIFY(m_HdlInfoPositioningGrappler1 == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoPositioningGrappler2 == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlGrappler::utTestGrapplerPositioningAttachToRack()
{
    if(m_pDeviceProcessingResponse)
    {
        ReturnCode_t RetCode;
        Scheduler1::CStation Station1;
        Scheduler1::CStation Station2;

        m_HdlInfoPositioningGrappler1 = DCL_ERR_FCT_CALL_FAILED;
        m_HdlInfoPositioningGrappler2 = DCL_ERR_FCT_CALL_FAILED;

        Station1.SetColumn(11);
        Station1.SetRow(3);
        RetCode = m_pIGrappler1->ReqStationAction(STATION_ACTION_ATTACH_TO_RACK, 3, 11);


        Station2.SetColumn(2);
        Station2.SetRow(9);
        RetCode = m_pIGrappler2->ReqStationAction(STATION_ACTION_ATTACH_TO_RACK, 2, 9);

        //spend some time to proceed the reference runs
        QTest::qWait(10000);

        QVERIFY(m_HdlInfoPositioningGrappler1 == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoPositioningGrappler2 == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlGrappler::utTestGrapplerPositioningPullUpRack()
{
    if(m_pDeviceProcessingResponse)
    {
        ReturnCode_t RetCode;
        Scheduler1::CStation Station1;
        Scheduler1::CStation Station2;

        m_HdlInfoPositioningGrappler1 = DCL_ERR_FCT_CALL_FAILED;
        m_HdlInfoPositioningGrappler2 = DCL_ERR_FCT_CALL_FAILED;

        Station1.SetColumn(11);
        Station1.SetRow(3);
        RetCode = m_pIGrappler1->ReqStationAction(STATION_ACTION_PULL_UP_RACK, 3, 11);


        Station2.SetColumn(2);
        Station2.SetRow(9);
        RetCode = m_pIGrappler2->ReqStationAction(STATION_ACTION_PULL_UP_RACK, 2, 9);

        //spend some time to proceed the reference runs
        QTest::qWait(10000);

        QVERIFY(m_HdlInfoPositioningGrappler1 == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoPositioningGrappler2 == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlGrappler::utTestGrapplerPositioningTransportRackTo()
{
    if(m_pDeviceProcessingResponse)
    {
        ReturnCode_t RetCode;
        Scheduler1::CStation Station1;
        Scheduler1::CStation Station2;

        m_HdlInfoPositioningGrappler1 = DCL_ERR_FCT_CALL_FAILED;
        m_HdlInfoPositioningGrappler2 = DCL_ERR_FCT_CALL_FAILED;

        Station1.SetColumn(14);
        Station1.SetRow(0);
        RetCode = m_pIGrappler1->ReqStationAction(STATION_ACTION_TRANSP_RACK_TO, 0, 14);


        Station2.SetColumn(1);
        Station2.SetRow(4);
        RetCode = m_pIGrappler2->ReqStationAction(STATION_ACTION_TRANSP_RACK_TO, 4, 1);

        //spend some time to proceed the reference runs
        QTest::qWait(10000);

        QVERIFY(m_HdlInfoPositioningGrappler1 == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoPositioningGrappler2 == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlGrappler::utTestGrapplerPositioningLetdownRack()
{
    if(m_pDeviceProcessingResponse)
    {
        ReturnCode_t RetCode;
        Scheduler1::CStation Station1;
        Scheduler1::CStation Station2;

        m_HdlInfoPositioningGrappler1 = DCL_ERR_FCT_CALL_FAILED;
        m_HdlInfoPositioningGrappler2 = DCL_ERR_FCT_CALL_FAILED;

        Station1.SetColumn(14);
        Station1.SetRow(0);
        RetCode = m_pIGrappler1->ReqStationAction(STATION_ACTION_LET_DOWN_RACK, 0, 14);


        Station2.SetColumn(1);
        Station2.SetRow(4);
        RetCode = m_pIGrappler2->ReqStationAction(STATION_ACTION_LET_DOWN_RACK, 4, 1);

        //spend some time to proceed the reference runs
        QTest::qWait(10000);

        QVERIFY(m_HdlInfoPositioningGrappler1 == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoPositioningGrappler2 == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlGrappler::utTestGrapplerPositioningEmptyMove2()
{
    if(m_pDeviceProcessingResponse)
    {
        ReturnCode_t RetCode;
        Scheduler1::CStation Station1;
        Scheduler1::CStation Station2;
        quint8 WaitCyle = 0;

        m_HdlInfoPositioningGrappler1 = DCL_ERR_FCT_CALL_FAILED;
        m_HdlInfoPositioningGrappler2 = DCL_ERR_FCT_CALL_FAILED;

        Station1.SetColumn(11);
        Station1.SetRow(3);
        RetCode = m_pIGrappler1->ReqStationAction(STATION_ACTION_MOVE_EMTPY_TO, 3, 11);


        Station2.SetColumn(3);
        Station2.SetRow(8);
        RetCode = m_pIGrappler2->ReqStationAction(STATION_ACTION_MOVE_EMTPY_TO, 8, 3);


        while((m_HdlInfoPositioningGrappler1 != DCL_ERR_FCT_CALL_SUCCESS) && (m_HdlInfoPositioningGrappler2 != DCL_ERR_FCT_CALL_SUCCESS))
        {
            //spend some time to proceed the reference runs
            QTest::qWait(1000);
            if(WaitCyle++ > 15)
            {
                break;
            }
        }

        QVERIFY(m_HdlInfoPositioningGrappler1 == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoPositioningGrappler2 == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlGrappler::utTestGrapplerPositioningDetachRack()
{
    if(m_pDeviceProcessingResponse)
    {
        ReturnCode_t RetCode;
        Scheduler1::CStation Station1;
        Scheduler1::CStation Station2;
        quint8 WaitCyle = 0;

        m_HdlInfoPositioningGrappler1 = DCL_ERR_FCT_CALL_FAILED;
        m_HdlInfoPositioningGrappler2 = DCL_ERR_FCT_CALL_FAILED;

        Station1.SetColumn(14);
        Station1.SetRow(0);
        RetCode = m_pIGrappler1->ReqStationAction(STATION_ACTION_DETACH_RACK, 0, 14);


        Station2.SetColumn(1);
        Station2.SetRow(4);
        RetCode = m_pIGrappler2->ReqStationAction(STATION_ACTION_DETACH_RACK, 4, 1);

        while((m_HdlInfoPositioningGrappler1 != DCL_ERR_FCT_CALL_SUCCESS) && (m_HdlInfoPositioningGrappler2 != DCL_ERR_FCT_CALL_SUCCESS))
        {
            //spend some time to proceed the reference runs
            QTest::qWait(1000);
            if(WaitCyle++ > 10)
            {
                break;
            }
        }

        QVERIFY(m_HdlInfoPositioningGrappler1 == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoPositioningGrappler2 == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlGrappler::OnGrapplerPositioningAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo)
{
    if(InstanceID == DEVICE_INSTANCE_ID_GRAPPLER_1)
    {
        m_HdlInfoPositioningGrappler1 = HdlInfo;
    }
    else if(InstanceID == DEVICE_INSTANCE_ID_GRAPPLER_2)
    {
        m_HdlInfoPositioningGrappler2 = HdlInfo;
    }
}
} // end namespace

//QTEST_MAIN(DeviceControl::TestDeviceControlGrappler)




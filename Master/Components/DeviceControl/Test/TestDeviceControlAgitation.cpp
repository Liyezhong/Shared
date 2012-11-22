#include <unistd.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#include <QTest>
#include <DeviceControl/Test/TestDeviceControlAgitation.h>
#include <DeviceControl/Test/DeviceControlResponse.h>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/Interface/Devices/IAgitationDevice.h"

namespace DeviceControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestDeviceControlAgitation::TestDeviceControlAgitation() :
    m_HdlInfoReferenceRunAckn(DCL_ERR_FCT_CALL_FAILED), m_HdlInfoActSpeed(DCL_ERR_FCT_CALL_FAILED), m_HdlInfoActPosition(DCL_ERR_FCT_CALL_FAILED)
{

}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestDeviceControlAgitation::~TestDeviceControlAgitation()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlAgitation::initTestCase()
{
    if(m_pDeviceProcessingResponse)
    {
        IDeviceProcessing* pIDeviceProcessing;

        pIDeviceProcessing = m_pDeviceProcessingResponse->GetIDeviceProcessing();
        if(pIDeviceProcessing != 0)
        {
            m_pIAgitationDevice = pIDeviceProcessing->GetIAgitationDevice();

            connect(m_pDeviceProcessingResponse, SIGNAL(ReportAgitationReferenceRunAckn(DevInstanceID_t, ReturnCode_t, AgitationPosition_t)), this, SLOT(OnAgitationReferenceRunAckn(DevInstanceID_t, ReturnCode_t, AgitationPosition_t)));
            connect(m_pDeviceProcessingResponse, SIGNAL(ReportAgitationActSpeed(DevInstanceID_t, ReturnCode_t, AgitationSpeed_t)), this, SLOT(OnAgitationActSpeed(DevInstanceID_t, ReturnCode_t, AgitationSpeed_t)));
            connect(m_pDeviceProcessingResponse, SIGNAL(ReportAgitationActPosition(DevInstanceID_t, ReturnCode_t, AgitationPosition_t)), this, SLOT(OnAgitationActPosition(DevInstanceID_t, ReturnCode_t, AgitationPosition_t)));
        }
    }

    QVERIFY(m_pIAgitationDevice != 0);
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlAgitation::init()
{

}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlAgitation::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlAgitation::cleanupTestCase()
{

}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestDeviceControlAgitation::utTestReferenceRuns()
{
    if(m_pIAgitationDevice != 0)
    {
        m_pIAgitationDevice->ReqReferenceRun();

        //spend some time to proceed the reference runs
        QTest::qWait(10000);

        QVERIFY(m_HdlInfoReferenceRunAckn == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlAgitation::OnAgitationReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationPosition_t Position)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(Position);

    m_HdlInfoReferenceRunAckn = HdlInfo;

}

void TestDeviceControlAgitation::OnAgitationActSpeed(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationSpeed_t Speed)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(Speed);

    m_HdlInfoActSpeed = HdlInfo;
}

void TestDeviceControlAgitation::OnAgitationActPosition(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, AgitationPosition_t Position)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(Position);

    m_HdlInfoActPosition = HdlInfo;
}


void TestDeviceControlAgitation::utTestSpeed1()
{
    if(m_pDeviceProcessingResponse)
    {
        ReturnCode_t RetCode;

        m_HdlInfoActSpeed = DCL_ERR_FCT_CALL_FAILED;

        RetCode = m_pIAgitationDevice->SetAgitationSpeed(AGITATION_SPEED_20);

        //spend some time to proceed the command
        QTest::qWait(5000);

        QVERIFY(m_HdlInfoActSpeed == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlAgitation::utTestStop()
{
    if(m_pDeviceProcessingResponse)
    {
        ReturnCode_t RetCode;

        m_HdlInfoActSpeed = DCL_ERR_FCT_CALL_FAILED;

        RetCode = m_pIAgitationDevice->SetAgitationSpeed(AGITATION_SPEED_STOP);

        //spend some time to proceed the command
        QTest::qWait(2000);

        QVERIFY(m_HdlInfoActSpeed == DCL_ERR_FCT_CALL_SUCCESS);


        RetCode = m_pIAgitationDevice->ReqActPosition();

        //spend some time to proceed the command
        QTest::qWait(1000);

        QVERIFY(m_HdlInfoActPosition == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlAgitation::utTestSpeed2()
{

}

void TestDeviceControlAgitation::utTestStop2()
{

}

} // end namespace







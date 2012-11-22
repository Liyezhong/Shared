#include <unistd.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <QTest>
#include <DeviceControl/Test/TestDeviceControlOven.h>
#include <DeviceControl/Test/DeviceControlResponse.h>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/Interface/Devices/IOvenDevice.h"

namespace DeviceControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestDeviceControlOven::TestDeviceControlOven() :
    m_HdlInfoReferenceRunAckn(DCL_ERR_FCT_CALL_FAILED)
{

}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestDeviceControlOven::~TestDeviceControlOven()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlOven::initTestCase()
{
    if(m_pDeviceProcessingResponse)
    {
        IDeviceProcessing* pIDeviceProcessing;

        pIDeviceProcessing = m_pDeviceProcessingResponse->GetIDeviceProcessing();
        if(pIDeviceProcessing != 0)
        {
            m_pIOvenDevice = pIDeviceProcessing->GetIOvenDevice();

            connect(m_pDeviceProcessingResponse, SIGNAL(ReportOvenReferenceRunAckn(DevInstanceID_t, ReturnCode_t, OvenCoverPosition_t)), this, SLOT(OnOvenReferenceRunAckn(DevInstanceID_t, ReturnCode_t, OvenCoverPosition_t)));
        }
    }

    QVERIFY(m_pIOvenDevice != 0);
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlOven::init()
{

}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlOven::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlOven::cleanupTestCase()
{

}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestDeviceControlOven::utTestReferenceRuns()
{
    if(m_pIOvenDevice != 0)
    {
        m_pIOvenDevice->ReqCoverReferenceRun();

        //spend some time to proceed the reference runs
        QTest::qWait(10000);

        QVERIFY(m_HdlInfoReferenceRunAckn == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlOven::OnOvenReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, OvenCoverPosition_t Position)
{
    Q_UNUSED(InstanceID);
    Q_UNUSED(Position);

    m_HdlInfoReferenceRunAckn = HdlInfo;

}

} // end namespace








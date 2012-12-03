#include <unistd.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#include <QTest>
#include <DeviceControl/Test/TestDeviceControlLoader.h>
#include <DeviceControl/Test/DeviceControlResponse.h>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/Interface/Devices/ILoaderDevice.h"

namespace DeviceControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestDeviceControlLoader::TestDeviceControlLoader() :
    m_HdlInfoReferenceRunLoader(DCL_ERR_FCT_CALL_FAILED), m_HdlInfoReferenceRunUnloader(DCL_ERR_FCT_CALL_FAILED),
    m_HdlInfoBlockStateLoader(DCL_ERR_FCT_CALL_FAILED), m_HdlInfoBlockStateUnloader(DCL_ERR_FCT_CALL_FAILED),
    m_BlockingStateLoader(BLOCKING_STATE_UNDEF), m_BlockingStateUnloader(BLOCKING_STATE_UNDEF)
{

}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestDeviceControlLoader::~TestDeviceControlLoader()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlLoader::initTestCase()
{
    if(m_pDeviceProcessingResponse)
    {
        IDeviceProcessing* pIDeviceProcessing;

        pIDeviceProcessing = m_pDeviceProcessingResponse->GetIDeviceProcessing();
        if(pIDeviceProcessing != 0)
        {
            m_pILoaderDevice = pIDeviceProcessing->GetILoaderDevice(DEVICE_INSTANCE_ID_LOADER);
            m_pIUnloaderDevice = pIDeviceProcessing->GetILoaderDevice(DEVICE_INSTANCE_ID_UNLOADER);

            connect(m_pDeviceProcessingResponse, SIGNAL(ReportLoaderReferenceRunAckn(DevInstanceID_t, ReturnCode_t, LoaderPosition_t)), this, SLOT(OnReferenceRunAckn(DevInstanceID_t, ReturnCode_t, LoaderPosition_t)));
            connect(m_pDeviceProcessingResponse, SIGNAL(ReportLoaderBlockingState(DevInstanceID_t, ReturnCode_t, BlockState_t)), this, SLOT(OnReportLoaderBlockingState(DevInstanceID_t, ReturnCode_t, BlockState_t)));
        }
    }

    QVERIFY(m_pILoaderDevice != 0);
    QVERIFY(m_pIUnloaderDevice != 0);
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlLoader::init()
{

}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlLoader::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlLoader::cleanupTestCase()
{

}

/****************************************************************************/
/**
 * \brief Test how state engine handles bad input.
 */
/****************************************************************************/
void TestDeviceControlLoader::utTestReferenceRuns()
{
    if((m_pILoaderDevice != 0) && (m_pIUnloaderDevice != 0))
    {
        m_pILoaderDevice->ReqReferenceRun();
        m_pIUnloaderDevice->ReqReferenceRun();

        //spend some time to proceed the reference runs
        QTest::qWait(10000);

        QVERIFY(m_HdlInfoReferenceRunLoader == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoReferenceRunUnloader == DCL_ERR_FCT_CALL_SUCCESS);
    }
}

void TestDeviceControlLoader::OnReferenceRunAckn(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, LoaderPosition_t Position)
{
    Q_UNUSED(Position);

    if(InstanceID == DEVICE_INSTANCE_ID_LOADER)
    {
        m_HdlInfoReferenceRunLoader = HdlInfo;
    }
    else if(InstanceID == DEVICE_INSTANCE_ID_UNLOADER)
    {
        m_HdlInfoReferenceRunUnloader = HdlInfo;
    }
}

void TestDeviceControlLoader::utTestPositioning()
{

}


void TestDeviceControlLoader::utTestBlocking()
{
    if((m_pILoaderDevice != 0) && (m_pIUnloaderDevice != 0))
    {
        ReturnCode_t RetVal;

        RetVal = m_pILoaderDevice->ReqBlockingState(BLOCKING_STATE_BLOCKED);
        QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

        RetVal = m_pIUnloaderDevice->ReqBlockingState(BLOCKING_STATE_BLOCKED);
        QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

        //spend some time to let it work, otherwise the next command will fail
        QTest::qWait(100);

        RetVal = m_pILoaderDevice->ReqActBlockingState();
        QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

        RetVal = m_pIUnloaderDevice->ReqActBlockingState();
        QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

        //spend some time to proceed the reference runs
        QTest::qWait(2000);

        QVERIFY(m_HdlInfoBlockStateLoader == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoBlockStateUnloader == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_BlockingStateLoader == BLOCKING_STATE_BLOCKED);
        QVERIFY(m_BlockingStateUnloader == BLOCKING_STATE_BLOCKED);

        //#################
        // unblock
        //#################
        m_BlockingStateLoader = BLOCKING_STATE_UNDEF;
        m_BlockingStateUnloader = BLOCKING_STATE_UNDEF;

        RetVal = m_pILoaderDevice->ReqBlockingState(BLOCKING_STATE_FREE);
        QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

        RetVal = m_pIUnloaderDevice->ReqBlockingState(BLOCKING_STATE_FREE);
        QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

        //spend some time to let it work, otherwise the next command will fail
        QTest::qWait(100);

        RetVal = m_pILoaderDevice->ReqActBlockingState();
        QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

        RetVal = m_pIUnloaderDevice->ReqActBlockingState();
        QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

        //spend some time to proceed the reference runs
        QTest::qWait(2000);

        QVERIFY(m_HdlInfoBlockStateLoader == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_HdlInfoBlockStateUnloader == DCL_ERR_FCT_CALL_SUCCESS);
        QVERIFY(m_BlockingStateLoader == BLOCKING_STATE_FREE);
        QVERIFY(m_BlockingStateUnloader == BLOCKING_STATE_FREE);


    }
}

void TestDeviceControlLoader::OnReportLoaderBlockingState(DevInstanceID_t InstanceID, ReturnCode_t HdlInfo, BlockState_t BlockingState)
{
    Q_UNUSED(BlockingState);

    if(InstanceID == DEVICE_INSTANCE_ID_LOADER)
    {
        m_HdlInfoBlockStateLoader = HdlInfo;
        m_BlockingStateLoader = BlockingState;
    }
    else if(InstanceID == DEVICE_INSTANCE_ID_UNLOADER)
    {
        m_HdlInfoBlockStateUnloader = HdlInfo;
        m_BlockingStateUnloader = BlockingState;
    }
}


} // end namespace

//QTEST_MAIN(DeviceControl::TestDeviceControlGrappler)





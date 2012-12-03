#include <unistd.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#include <QTest>
#include <DeviceControl/Test/TestDeviceControlCANNode.h>
#include <DeviceControl/Test/DeviceControlResponse.h>
#include "DeviceControl/Include/Interface/IDeviceProcessing.h"
#include "DeviceControl/Include/SlaveModules/BaseModule.h"

namespace DeviceControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestDeviceControlCANNode::TestDeviceControlCANNode()
{

}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestDeviceControlCANNode::~TestDeviceControlCANNode()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlCANNode::initTestCase()
{
    if(m_pDeviceProcessingResponse)
    {
        IDeviceProcessing* pIDeviceProcessing;

        pIDeviceProcessing = m_pDeviceProcessingResponse->GetIDeviceProcessing();
        if(pIDeviceProcessing != 0)
        {
            m_pICANNode = pIDeviceProcessing->GetINode(0);

            connect(m_pICANNode, SIGNAL(ReportEndTestResult(quint32 , ReturnCode_t , quint8, quint8, quint8, quint8)), this, SLOT(OnReportEndTestResult(quint32, ReturnCode_t, quint8, quint8, quint8, quint8)));
            connect(m_pICANNode, SIGNAL(ReportHWInfo(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint8)), this, SLOT(OnReportHWInfo(quint32, ReturnCode_t, quint8, quint8, quint8, quint8, quint8)));
            connect(m_pICANNode, SIGNAL(ReportSWInfo(quint32, ReturnCode_t, quint16, quint8, quint8, quint8)), this, SLOT(OnReportSWInfo(quint32, ReturnCode_t, quint16, quint8, quint8, quint8)));
        }
    }

    QVERIFY(m_pICANNode != 0);
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlCANNode::init()
{

}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlCANNode::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlCANNode::cleanupTestCase()
{

}

void TestDeviceControlCANNode::utTestCANNodeAccess()
{
    if(m_pDeviceProcessingResponse)
    {
        IDeviceProcessing* pIDeviceProcessing;

        pIDeviceProcessing = m_pDeviceProcessingResponse->GetIDeviceProcessing();
        if(pIDeviceProcessing != 0)
        {
            CBaseModule* pICANNodeKey;
            CBaseModule* pICANNodeInstance;
            CBaseModule* pICANNodeID;

            pICANNodeKey = pIDeviceProcessing->GetINodeFromKey("sa_board_controller1");
            pICANNodeInstance = pIDeviceProcessing->GetINodeFromInstanceID(0x00000006);
            QVERIFY(pICANNodeKey == pICANNodeInstance);

            pICANNodeID = pIDeviceProcessing->GetINodeFromID(0x0006);
            QVERIFY(pICANNodeID == pICANNodeKey);
        }
    }
}

void TestDeviceControlCANNode::utTestCANNode()
{
    ReturnCode_t RetVal;

    RetVal = m_pICANNode->ReqEndTestResult();
    QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

    //spend some time to proceed the request
    QTest::qWait(100);

    QVERIFY(m_HdlInfoTestResult == DCL_ERR_FCT_CALL_SUCCESS);

    RetVal = m_pICANNode->ReqHWInfo();
    QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

    //spend some time to proceed the request
    QTest::qWait(100);

    QVERIFY(m_HdlInfoHWInfo == DCL_ERR_FCT_CALL_SUCCESS);

    RetVal = m_pICANNode->ReqSWInfo();
    QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);

    //spend some time to proceed the request
    QTest::qWait(100);

    QVERIFY(m_HdlInfoSWInfo == DCL_ERR_FCT_CALL_SUCCESS);

}


void TestDeviceControlCANNode::OnReportEndTestResult(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 TestResult, quint8 TestYear, quint8 TestMonth, quint8 TestDay)
{
    Q_UNUSED(InstanceID);
    m_HdlInfoTestResult = HdlInfo;
    Q_UNUSED(TestResult);
    Q_UNUSED(TestYear);
    Q_UNUSED(TestMonth);
    Q_UNUSED(TestDay);
}

void TestDeviceControlCANNode::OnReportHWInfo(quint32 InstanceID, ReturnCode_t HdlInfo, quint8 VersionMajor, quint8 VersionMinor, quint8 Year, quint8 Month, quint8 Day)
{
    Q_UNUSED(InstanceID);
    m_HdlInfoHWInfo = HdlInfo;
    m_HWVersionMajor = VersionMajor;
    m_HWVersionMinor = VersionMinor;
    Q_UNUSED(Year);
    Q_UNUSED(Month);
    Q_UNUSED(Day);
}

void TestDeviceControlCANNode::OnReportSWInfo(quint32 InstanceID, ReturnCode_t HdlInfo, quint16 Version, quint8 Year, quint8 Month, quint8 Day)
{
    Q_UNUSED(InstanceID);
    m_HdlInfoSWInfo = HdlInfo;
    m_SWVersion = Version;
    Q_UNUSED(Year);
    Q_UNUSED(Month);
    Q_UNUSED(Day);

}

void TestDeviceControlCANNode::utTestCANNodeFctModules()
{
    ReturnCode_t RetVal;
    CFunctionModule* pIFctModBase;
    CStepperMotor* pICANStepperMotor;

    pIFctModBase = m_pICANNode->GetIFctModule("motor_z-axis_grap1");
    pICANStepperMotor = (CStepperMotor*) pIFctModBase;

    if(pICANStepperMotor)
    {
        connect(pICANStepperMotor, SIGNAL(ReportLifeCycleData(quint32, ReturnCode_t, quint32, quint32, quint32)), this, SLOT(OnReportLifeCycleData(quint32, ReturnCode_t, quint32, quint32, quint32)));

        RetVal = pICANStepperMotor->ReqLifeCycleData();
        QVERIFY(RetVal == DCL_ERR_FCT_CALL_SUCCESS);
        //spend some time to proceed the request
        QTest::qWait(1000);
    }

    QVERIFY(m_HdlInfoLifeCyleData == DCL_ERR_FCT_CALL_SUCCESS);
}

void TestDeviceControlCANNode::OnReportLifeCycleData(quint32 InstanceID, ReturnCode_t HdlInfo, quint32 StepCounter, quint32 TripCounter, quint32 OperationTime)
{
    Q_UNUSED(InstanceID);
    m_HdlInfoLifeCyleData = HdlInfo;

    Q_UNUSED(StepCounter);
    Q_UNUSED(TripCounter);
    Q_UNUSED(OperationTime);
}

} // end namespace










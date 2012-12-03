/****************************************************************************/
/*! \file TestDeviceControlBase.cpp
 *
 *  \brief Implementation file for class TestDeviceControl.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.02.2011
 *   $Author:  $ Norbert Wiedmann
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#include <unistd.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>


#include <QTest>
#include <DeviceControl/Test/TestDeviceControlBase.h>
#include <DeviceControl/Test/DeviceControlResponse.h>

namespace DeviceControl {

/****************************************************************************/
/**
 * \brief Constructor.
 */
/****************************************************************************/
TestDeviceControlBase::TestDeviceControlBase()
{
    m_pDeviceProcessingResponse = 0;
}

/****************************************************************************/
/**
 * \brief Destructor.
 */
/****************************************************************************/
TestDeviceControlBase::~TestDeviceControlBase()
{
}

/****************************************************************************/
/**
 * \brief Called before the first testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlBase::initTestCase()
{
    QProcess KillColoPiaProcess(this);

    //for the startup phase of this testproject, ensure there is no running ColoPiaSimulator process on the computer
    KillColoPiaProcess.start("killall ColoPiaSimulator");
    QTest::qWait(1000);
    KillColoPiaProcess.waitForStarted();


    // create the ColoPiaSimulator process
    m_pColoPiaProcess = new QProcess(this);

    // Start the process instance, set configuration directory
    m_pColoPiaProcess->start("../../../../../Tools/ColoPiaSimulator/ColoPiaSimulator colorado ../../../../../Colorado/ColoradoMain/Master/Components/Main/Build/Settings");

    //test at workstation starts at _dbg
    //m_pColoPiaProcess->start("../../../../../../Tools/ColoPiaSimulator/ColoPiaSimulator colorado ../../../../../../Colorado/ColoradoMain/Master/Components/Main/Build/Settings");

    // wait until started, function has a timout of 30sec
    if(m_pColoPiaProcess->waitForStarted())
    {
        m_pDeviceProcessingResponse = new DeviceProcessingResponse();
    }

    //ensure ColoPiaSimulator has started and the device processing interface class has been created.
    QVERIFY(m_pDeviceProcessingResponse != 0);

    RegisterMetaTypes();
}

/****************************************************************************/
/**
 * \brief Called before each testfunction is executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlBase::init()
{

}

/****************************************************************************/
/**
 * \brief Called after each testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlBase::cleanup()
{
}

/****************************************************************************/
/**
 * \brief Called after last testfunction was executed.
 *        (Qt Test framework standard function)
 */
/****************************************************************************/
void TestDeviceControlBase::cleanupTestCase()
{
}

/****************************************************************************/
/*!
 *  \brief   Register the metat types of the device control layer
 *
 *           The registration is need to ensure correct data transmission
 *           within the signal-slot-mechanism
 *
 ****************************************************************************/
void TestDeviceControlBase::RegisterMetaTypes()
{
    qRegisterMetaType<ReturnCode_t>("ReturnCode_t");
    qRegisterMetaType<DevInstanceID_t>("DevInstanceID_t");
    qRegisterMetaType<NodeState_t>("NodeState_t");
    qRegisterMetaType<BlockState_t>("BlockState_t");
    qRegisterMetaType<HeatedVesselID_t>("HeatedVesselID_t");
    qRegisterMetaType<LoaderPosition_t>("LoaderPosition_t");
    qRegisterMetaType<LoaderRFIDChannel_t>("LoaderRFIDChannel_t");
    qRegisterMetaType<AgitationSpeed_t>("AgitationSpeed_t");
    qRegisterMetaType<WaterValveID_t>("WaterValveID_t");
    qRegisterMetaType<OvenCoverPosition_t>("OvenCoverPosition_t");
    qRegisterMetaType<TempCtrlStatus_t>("TempCtrlStatus_t");
    qRegisterMetaType<TempCtrlOperatingMode_t>("TempCtrlOperatingMode_t");
    qRegisterMetaType<HeatedVesselID_t>("HeatedVesselID_t");
    qRegisterMetaType<RackAdapterPosition_t>("RackAdapterPosition_t");
    qRegisterMetaType<AgitationPosition_t>("AgitationPosition_t");
    qRegisterMetaType<AgitationSpeed_t>("AgitationSpeed_t");
}

void TestDeviceControlBase::utTestStartDCL()
{
    if(m_pDeviceProcessingResponse)
    {
        m_pDeviceProcessingResponse->Start();

        //spend some time to proceed the initialisation
        QTest::qWait(15000);

        QVERIFY(m_pDeviceProcessingResponse->m_bInitialisationFinished ==true);
    }
}

/****************************************************************************/
/**
 * \brief Test process startup with command parameters.
 */
/****************************************************************************/
void TestDeviceControlBase::utTestDCLConfiguration()
{
    if(m_pDeviceProcessingResponse)
    {
        m_pDeviceProcessingResponse->StartConfiguration();

        //spend some time to proceed the configuration
        QTest::qWait(10000);

        QVERIFY(m_pDeviceProcessingResponse->m_bConfigurationFinished == true);
    }
}

void TestDeviceControlBase::utTestSerialKey()
{
    QString SerialKey;

    SerialKey = m_pDeviceProcessingResponse->GetSerialNumber();
    QVERIFY(SerialKey == "holdrio4711");
}

void  TestDeviceControlBase::utTestCANNodes()
{

    QVERIFY(m_pDeviceProcessingResponse->RequestCANNodes() == true);

}

/****************************************************************************/
/*!
 *  \brief   Clean up the tests environmant
 *
 *           Deletes the IResponse intervace and kill ColoPia process
 *
 ****************************************************************************/
void  TestDeviceControlBase::CloseTest()
{
    if(m_pDeviceProcessingResponse)
    {
        delete m_pDeviceProcessingResponse;
        m_pDeviceProcessingResponse = 0;
    }

    if(m_pColoPiaProcess)
    {
        m_pColoPiaProcess->kill();
        //m_pColoPiaProcess->start("../../../../../Tools/ColoPiaSimulator/ColoPiaSimulator colorado ../../../../../Colorado/ColoradoMain/Master/Components/Main/Build/Settings");
        //delete m_pColoPiaProcess;
    }
}


} // end namespace StateMachines

//QTEST_MAIN(DeviceControl::TestDeviceControlBase)

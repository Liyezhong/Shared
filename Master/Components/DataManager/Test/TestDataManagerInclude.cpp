/****************************************************************************/
/*! \file TestDataManagerInclude.cpp
 *
 *  \brief Implementation file for class TestDataManagerInclude.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-05-27
 *  $Author:    $ Shuvasmita
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <QTest>
#include <DataManager/Include/DataContainerCollectionBase.h>
#include <DataManager/Include/DataManagerBase.h>
#include <Global/Include/SystemPaths.h>
#include "NetCommands/Include/CmdScheduledStations.h"
#include <Threads/Include/MasterThreadController.h>
#include <Threads/Include/CommandChannel.h>
#include <QDir>
#include <HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h>

class TestMasterThread : public Threads::MasterThreadController {
    Q_OBJECT
private:

public:

    Threads::CommandChannel  *m_CommandChannelScheduler;  ///< Command Channel for Scheduler
    Threads::CommandChannel *m_CommandChannelDeviceCommandProcessor; ///< Command Channel for DeviceCommandProcessor

    /****************************************************************************/
    /**
     * \brief Constructor
     */
    /****************************************************************************/
    TestMasterThread():
      MasterThreadController("HimalayaShutdown"),
        m_CommandChannelScheduler(NULL),m_CommandChannelDeviceCommandProcessor(NULL)
    {
    }

    /****************************************************************************/
    /**
     * \brief Destructor
     */
    /****************************************************************************/
    ~TestMasterThread()
    {
        delete m_CommandChannelScheduler;
        delete m_CommandChannelDeviceCommandProcessor;
    }
    /****************************************************************************/
    /**
     * \brief Start statemachine.
     */
    /****************************************************************************/
    void StartStatemachine(){}
    /****************************************************************************/
    /**
     * \brief Initiate the shutdown process.
     *
     * Initiate the shutdown process. Make some project specific tasks the call
     * \ref Shutdown to shut down software.
     *
     * \iparam Reboot
     */
    /****************************************************************************/
    void InitiateShutdown(bool Reboot = false){}
    /****************************************************************************/
    /**
     * \brief Create GUI controllers, which inturn starts GUI process .
     *
     * \note All the XML files are sent to GUI.
     */
    /****************************************************************************/
    void InitializeGUI(){}
    /****************************************************************************/
    /**
     * \brief Return the command channel object requested
     * \iparam CommandChannelSelector = Command channel to return
     * \return CommandChannel object
     */
    /****************************************************************************/
    Threads::CommandChannel & GetCommandChannel(const int CommandChannelSelector){}
    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     * \iparam PowerFailStage = Type of power fail stage
     *
     */
    /****************************************************************************/
    void OnPowerFail(const Global::PowerFailStages PowerFailStage) {}
    /****************************************************************************/
    /**
     * \brief This method is called when the base received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread.\n
     * We create all controllers used (the according method in derived classes
     * \ref CreateControllersAndThreads is also called), then initialize the controllers
     * and finally attach them to the corresponding threads and start them.\n
     * If something goes wrong, the master thread (and application) will be stopped.
     */
    /****************************************************************************/
    void OnGoReceived()
    {

    }

};

namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for Datamanger Include files
 */
/****************************************************************************/
class TestDataManagerInclude : public QObject {
    Q_OBJECT
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();
    /****************************************************************************/
    /**
     * \brief Test Data Conatiner collection base.
     */
    /****************************************************************************/
    void utTestDCCollectionBase();

    /****************************************************************************/
    /**
     * \brief Test Data Manager base.
     */
    /****************************************************************************/
    void utTestDatamanagerBase();

}; // end class TestXmlConfigFileStrings

/****************************************************************************/
void TestDataManagerInclude::initTestCase() {

}

/****************************************************************************/
void TestDataManagerInclude::init() {
}

/****************************************************************************/
void TestDataManagerInclude::cleanup() {
}

/****************************************************************************/
void TestDataManagerInclude::cleanupTestCase() {
}

/****************************************************************************/
void TestDataManagerInclude::utTestDCCollectionBase() {

    TestMasterThread *p_TestObj = new TestMasterThread();

    CDataContainerCollectionBase *p_CollectionBase = new CDataContainerCollectionBase(p_TestObj);
    p_CollectionBase->InitContainers();
    p_CollectionBase->DeinitContainers();

    delete p_CollectionBase;
    delete p_TestObj;

}
/****************************************************************************/
void TestDataManagerInclude::utTestDatamanagerBase()
{
    Global::SystemPaths::Instance().SetSettingsPath("../Settings");
    Global::SystemPaths::Instance().SetInstrumentSettingsPath("../Settings/Instrument");
    TestMasterThread *p_TestObj = new TestMasterThread();
    CDataReagentList *p_ReagentList = new CDataReagentList();
    CDataContainerCollectionBase *p_CollectionBase = new CDataContainerCollectionBase(p_TestObj);
    CDataManagerBase *p_DataMgrBase = new CDataManagerBase(p_TestObj);
    CDataManagerBase *p_DataManagerBase = new CDataManagerBase(p_TestObj, p_CollectionBase);
    p_DataManagerBase->SaveDataOnShutdown();
    QCOMPARE(p_CollectionBase->InitContainers(),true);
    p_DataManagerBase->InitDataContainer();
    p_DataManagerBase->m_IsInitialized = true;
    p_DataManagerBase->GetDataContainer();
    p_DataManagerBase->GetUserSettingsInterface();
    p_DataManagerBase->GetDeviceConfigurationInterface();
    p_DataManagerBase->GetRCConfigurationInterface();

    delete p_ReagentList;
    delete p_DataManagerBase;
    delete p_DataMgrBase;
    delete p_CollectionBase;
    delete p_TestObj;
}

} // end namespace DataLogging

QTEST_MAIN(DataManager::TestDataManagerInclude)

#include "TestDataManagerInclude.moc"

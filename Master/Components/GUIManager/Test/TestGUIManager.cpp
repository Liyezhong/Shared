/****************************************************************************/
/*! \file TestGUIMamager.cpp
 *
 *  \brief Implementation file for class TestGUIMamager.
 *
 *  \b Description:
 *         Checks the CGUIMamager class implementation
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-08-10
 *  $Author:    $ Raju
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
#include <QFile>
#include <GUIManager/Include/GUIManager.h>
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "Threads/Include/MasterThreadController.h"

//class to test the functionality of abstract class
class TestMasterThread : public Threads::MasterThreadController {
     Q_OBJECT
public:

    /****************************************************************************/
    /**
     * \brief Constructor
     */
    /****************************************************************************/
    TestMasterThread():MasterThreadController("ColoradoShutdown"){}

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
    void InitiateShutdown(bool Reboot = false) { Q_UNUSED(Reboot)}

    /****************************************************************************/
    /**
     * \brief Start statemachine.
     *
     * Start own statemachine and start to work.
     * Must be implemented in derived classes.
     */
    /****************************************************************************/
    void StartStatemachine() {}

    /****************************************************************************/
    /**
     * \brief Create GUI controllers, which inturn starts GUI process .
     *
     * \note All the XML files are sent to GUI.
     */
    /****************************************************************************/
    void InitializeGUI() {}

    /****************************************************************************/
    /**
     * \brief Return the command channel object requested
     * \iparam CommandChannelSelector = Command channel to return
     * \return CommandChannel object
     */
    /****************************************************************************/
    const Threads::CommandChannel & GetCommandChannel(const int CommandChannelSelector) { Q_UNUSED(CommandChannelSelector)}

    /****************************************************************************/
    /**
     * \brief This method is called when we received the \ref Go signal.
     *
     * This means that everything is fine and normal operation started.
     * We are running in our own thread now.\n
     * Do whatever you must do when normal operation started.\n
     * Has to be implemented in derived classes.
     */
    /****************************************************************************/
    void OnGoReceived() {}

    /****************************************************************************/
    /**
     * \brief This method is called when we received the \ref Stop signal.
     *
     * This means that normal operation will stop after processing this signal.
     * We are still running in our own thread.\n
     * Do whatever you must do before normal operation ends.\n
     * Has to be implemented in derived classes.
     * Should return within STOP_TIMEOUT, otherwise it can get interrupted by
     * CleanupAndDestroyObjects() which is called by main thread.
     */
    /****************************************************************************/
    void OnStopReceived() {}

    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     * \iparam PowerFailStage = Type of power fail stage
     *
     *   Must be implemented in derived classes.
     */
    /****************************************************************************/
    void OnPowerFail(const Global::PowerFailStages PowerFailStage) { Q_UNUSED(PowerFailStage)}

    /****************************************************************************/
    /**
     * \brief Cleanup and destroy used objects.
     *
     * Destroy all created objects.
     * <b>Remember, you are not running in your own thread, but in the thread that called
     * this method.</b>\n
     * Has to be implemented in derived classes.
     */
    /****************************************************************************/
    void CleanupAndDestroyObjects() {}

};


namespace GUIManager {

/****************************************************************************/
/**
 * \brief Test class for XmlConfigFileUserSettings class.
 */
/****************************************************************************/
class TestGUIManager : public QObject {
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
     * \brief Test the GUI manager.
     */
    /****************************************************************************/
    void utTestGUIManager();

}; // end class TestGUIMamager

/****************************************************************************/
void TestGUIManager::initTestCase() {

}

/****************************************************************************/
void TestGUIManager::init() {
}

/****************************************************************************/
void TestGUIManager::cleanup() {
}

/****************************************************************************/
void TestGUIManager::cleanupTestCase() {
}

/****************************************************************************/
void TestGUIManager::utTestGUIManager() {
    TestMasterThread MasterThread;
    CGUIManager Manager(MasterThread, "Test_*.qm");
    Manager.Initialize();
    QCOMPARE(Manager.SendLanguageFileToGUI("Hello"), false);
    QCOMPARE(Manager.UpdateSupportedGUILanguages(), false);

}

} // end namespace

QTEST_MAIN(GUIManager::TestGUIManager)

#include "TestGUIManager.moc"

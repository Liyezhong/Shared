/****************************************************************************/
/*! \file TestCmdInterface.cpp
 *
 *  \brief Implementation file for class TestCmdInterface.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-10-21
 *  $Author:    $ Shuvasmita.S
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

#include <QtTest/QTest>
#include <QDebug>
#include <QFile>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

#include "DataManager/CommandInterface/Include/CommandInterfaceBase.h"
#include "DataManager/CommandInterface/Include/UserSettingsCommandInterface.h"
#include "DataManager/Containers/ParameterSets/Commands/Include/CmdCancelParameterSet.h"

namespace DataManager {


/****************************************************************************/
/**
 * \brief TestMasterThreadController class.
 */
/****************************************************************************/
class TestMasterThreadController : public Threads::MasterThreadController {
    Q_OBJECT

private:
    Threads::CommandChannel  m_CommandChannelGui;                    ///< Command channel for Gui thread controller.

public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    TestMasterThreadController() :
        MasterThreadController("ColoradoShutdown"),
        m_CommandChannelGui(this, "Gui", Global::EVENTSOURCE_NONE)
    {
    }

public:

    /****************************************************************************/
    /**
     * \brief Power will fail shortly.
     * \iparam PowerFailStage = Type of power fail stage
     *
     *   Must be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void OnPowerFail(const Global::PowerFailStages PowerFailStage) { Q_UNUSED(PowerFailStage)}

    /****************************************************************************/
    /**
     * \brief Start statemachine.
     *
     * Start own statemachine and start to work.
     * Must be implemented in derived classes.
     */
    /****************************************************************************/
    virtual void StartStatemachine() {}

    /****************************************************************************/
    /**
     * \brief Initiate the shutdown process.
     *
     * Initiate the shutdown process. Make some project specific tasks the call
     * \ref Shutdown to shut down software.
     * Must be implemented in derived classes.
     *
     * \iparam Reboot
     */
    /****************************************************************************/
    virtual void InitiateShutdown(bool Reboot = false) { Q_UNUSED(Reboot)}

    /****************************************************************************/
    /**
     * \brief Create GUI controllers, which inturn starts GUI process .
     *
     * \note All the XML files are sent to GUI.
     */
    /****************************************************************************/
    virtual void InitializeGUI() {}

    /****************************************************************************/
    /**
     * \brief Return the command channel object requested
     * \iparam CommandChannelSelector = Command channel to return
     * \return CommandChannel object
     */
    /****************************************************************************/
    const Threads::CommandChannel & GetCommandChannel(const int CommandChannelSelector)
    {  Q_UNUSED(CommandChannelSelector)
                return m_CommandChannelRemoteCare; }

    /****************************************************************************/
    /**
     * \brief Starts the export controller thread
     *
     * \iparam ControllerNumber
     * \iparam BasicThreadController
     *
     ****************************************************************************/
    void StartSpecificThreadController(const quint32 ControllerNumber)
    {
        MasterThreadController::StartSpecificThreadController(ControllerNumber);
    }

};

/****************************************************************************/
/**
 * \brief Test class for command Interface class.
 */
/****************************************************************************/
class TestCmdInterface : public QObject {
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
     * \brief Test data User Settings Command Interface.
     */
    /****************************************************************************/
    void utTestUserSettingsCmdInterface();

    /****************************************************************************/
    /**
     * \brief Test data CmdInterfaceBase
     */
    /****************************************************************************/
    void utCmdInterfaceBase();


}; // end class TestDataModuleList

/****************************************************************************/
void TestCmdInterface::initTestCase() {
}

/****************************************************************************/
void TestCmdInterface::init() {
}

/****************************************************************************/
void TestCmdInterface::cleanup() {
}

/****************************************************************************/
void TestCmdInterface::cleanupTestCase() {
}

/****************************************************************************/
void TestCmdInterface::utTestUserSettingsCmdInterface() {

    CDataManagerBase *p_DataManager = NULL;
    CDataContainerCollectionBase *p_DataContainer = NULL;
    DataManager::TestMasterThreadController *p_MasterThreadController = new DataManager::TestMasterThreadController();
    CUserSettingsCommandInterface *p_UsersettingsCmdInterface = new CUserSettingsCommandInterface(p_DataManager,p_MasterThreadController, p_DataContainer );

    //p_UsersettingsCmdInterface->RegisterCommands();
    Threads::CommandChannel USChannel(p_MasterThreadController, "UserSettings", Global::EVENTSOURCE_NONE);
    MsgClasses::CmdChangeUserSettings *p_CmdChangeUserSettings = new MsgClasses::CmdChangeUserSettings();
    MsgClasses::CmdAlarmToneTest *p_CmdAlarmTest = new MsgClasses::CmdAlarmToneTest();

    p_UsersettingsCmdInterface->SettingsUpdateHandler((quint64)4,*p_CmdChangeUserSettings ,USChannel);
    p_UsersettingsCmdInterface->AlarmTestToneHandler((quint64)4, *p_CmdAlarmTest ,USChannel);

}
/****************************************************************************/
void TestCmdInterface::utCmdInterfaceBase() {

    CDataManagerBase *p_DataManager = NULL;
    DataManager::TestMasterThreadController *p_MasterThreadController = new DataManager::TestMasterThreadController();
    CDataContainerCollectionBase *p_DataContainer = NULL;
    MsgClasses::CmdCancelParamterSet *p_CmdCancelParaSets = new MsgClasses::CmdCancelParamterSet(1000, (quint32)3);
    CCommandInterfaceBase *p_CmdInterfaceBase = new CCommandInterfaceBase(p_DataManager ,p_MasterThreadController, p_DataContainer);
    Threads::CommandChannel CmdBaseChannel(p_MasterThreadController, "GUI", Global::EVENTSOURCE_NONE);
    p_CmdInterfaceBase->SendNackToGUIWithDefaultMsg((quint64)12,CmdBaseChannel,(quint32)23);
    p_CmdInterfaceBase->SendAckAndUpdateGUI((quint64)12,CmdBaseChannel ,Global::CommandShPtr_t(p_CmdCancelParaSets));

    CDataStationList* p_DSL_Verification = new CDataStationList();
    ListOfErrors_t ErrorList = p_DSL_Verification->GetErrorList();
    p_CmdInterfaceBase->SendNackToGUI((quint64)12, CmdBaseChannel ,ErrorList ,(quint32)23);
}

}

QTEST_MAIN(DataManager::TestCmdInterface)

#include "TestCmdInterface.moc"


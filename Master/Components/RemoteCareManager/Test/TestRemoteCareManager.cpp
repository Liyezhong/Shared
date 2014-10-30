#include <QtTest/QTest>
#include <QDebug>
#include <QFile>
#include <QProcess>


#include "RemoteCareManager/Include/RemoteCareManager.h"
#include "RemoteCareManager/Include/RCManagerEventCodes.h"
#include <Threads/Include/MasterThreadController.h>
#include <Threads/Include/PlatformThreadIDs.h>
#include <EventHandler/Include/StateHandler.h>

const QString RESOURCE_FILENAME = ":/Xml/RCConfiguration.xml"; ///< Resource file path

namespace DataManager {

/****************************************************************************/
/**
 * \brief CDataContainer definition
 */
/****************************************************************************/
class CDataContainerTemp: public DataManager::CDataContainerCollectionBase
{
    Q_OBJECT
private:
    /****************************************************************************/
    /**
     * \brief Deinitialise containers
     */
    /****************************************************************************/
    bool DeinitializeContainers()
    {
        return CDataContainerCollectionBase::DeinitContainers();
    }

public:
    /****************************************************************************/
    /**
     * \brief Constructor
     * \iparam p_ColoradoMasterThreadController
     */
    /****************************************************************************/
    CDataContainerTemp(Threads::MasterThreadController *p_ColoradoMasterThreadController):
        CDataContainerCollectionBase(p_ColoradoMasterThreadController)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor
     */
    /****************************************************************************/
    ~CDataContainerTemp()
    {
        try {
            (void)DeinitializeContainers();
        }
        catch(...)
        {

        }
    }
    /****************************************************************************/
    /**
     * \brief Initialize containers
     */
    /****************************************************************************/
    bool InitializeContainers()
    {
        if (!CDataContainerCollectionBase::InitContainers()) {
                return false;
        }
        m_IsInitialized = true;
        return true;
    }
    /****************************************************************************/
    /**
     * \brief Read RC Configuration
     */
    /****************************************************************************/
    bool ResetRCConfiguration()
    {
        QString FilenameRCConfigInterface = RESOURCE_FILENAME;
        RCConfigurationInterface->SetDataVerificationMode(false);
        if (!RCConfigurationInterface->Read(FilenameRCConfigInterface)) {
            return false;
        }

        RCConfigurationInterface->SetDataVerificationMode(true);
        if (!RCConfigurationInterface->VerifyData()) {
            return false;
        }

        return true;
    }
};

/****************************************************************************/
/**
 * \brief CDataManager definition
 */
/****************************************************************************/
class CColoradoDataManagerTemp : public CDataManagerBase
{
    Q_OBJECT
private:
public:
    /****************************************************************************/
    /**
     * \brief Constructor
     * \iparam p_ColoradoMasterThreadController
     */
    /****************************************************************************/
    CColoradoDataManagerTemp(Threads::MasterThreadController *p_ColoradoMasterThreadController):
        CDataManagerBase(p_ColoradoMasterThreadController)
    {
        DataManager::CDataContainerTemp *p_CollectionBase = new DataManager::CDataContainerTemp(p_ColoradoMasterThreadController);

        mp_DataContainerCollectionBase = p_CollectionBase;

        p_CollectionBase->InitializeContainers();
        p_CollectionBase->ResetRCConfiguration();

        m_IsInitialized = true;
    }
};

}
namespace RemoteCare {

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
        MasterThreadController("HimalayaShutdown"),
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
 * \brief Test class for HeartBeatThread class.
 */
/****************************************************************************/
class TestRemoteCareManager : public QObject {
    Q_OBJECT
    friend class RemoteCare::RemoteCareManager;

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
     * \brief Test data of RemoteCareManager.
     */
    /****************************************************************************/
    void utTestRemoteCareManager();
}; // end class TestDataModuleList

/****************************************************************************/
void TestRemoteCareManager::initTestCase() {
}

/****************************************************************************/
void TestRemoteCareManager::init() {
}

/****************************************************************************/
void TestRemoteCareManager::cleanup() {
}

/****************************************************************************/
void TestRemoteCareManager::cleanupTestCase() {
}

/****************************************************************************/
void TestRemoteCareManager::utTestRemoteCareManager()
{
    Global::SystemPaths::Instance().SetSettingsPath("../");

    RemoteCare::TestMasterThreadController *p_MasterThreadController = new RemoteCare::TestMasterThreadController();

    DataManager::CColoradoDataManagerTemp *p_DataManagerBase =
            new DataManager::CColoradoDataManagerTemp(p_MasterThreadController);

    RemoteCare::RemoteCareManager RCManager(*p_MasterThreadController, p_DataManagerBase->GetRCConfigurationInterface(), true);

    RCManager.Init();

//    p_MasterThreadController->StartSpecificThreadController(THREAD_ID_REMOTECARE, false);

//    p_MasterThreadController->SendCommand(
//                Global::CommandShPtr_t(new NetCommands::CmdRCSetTag(5000, RC_DATAITEM_SET_EVENT_PRIORITY,RDI_Analog, "1")),
//                p_MasterThreadController->GetCommandChannel(1));

    Global::tRefType Ref = p_MasterThreadController->GetNewCommandRef();
    Threads::CommandChannel RCChannel(p_MasterThreadController, "RemoteCare", Global::EVENTSOURCE_NONE);

    //check command subscription
    NetCommands::CmdRCSetTag CmdSubscription(5000, RC_DATAITEM_SET_SUBSCRIPTION, RDI_Analog, "0");

    RCManager.OnCmdRCSetTag(Ref, CmdSubscription, RCChannel);

    CmdSubscription.SetCommandData(RDI_Digital, "1");
    RCManager.OnCmdRCSetTag(Ref, CmdSubscription, RCChannel);

    CmdSubscription.SetCommandData(RDI_Analog, "1");
    RCManager.OnCmdRCSetTag(Ref, CmdSubscription, RCChannel);

    //test event priority data item
    NetCommands::CmdRCSetTag CmdPriority(5000, RC_DATAITEM_SET_EVENT_PRIORITY,RDI_Analog, "1");

    RCManager.OnCmdRCSetTag(Ref, CmdPriority, RCChannel);

    CmdPriority.SetCommandData(RDI_Analog, "2");
    RCManager.OnCmdRCSetTag(Ref, CmdPriority, RCChannel);

    CmdPriority.SetCommandData(RDI_Analog, "3");
    RCManager.OnCmdRCSetTag(Ref, CmdPriority, RCChannel);

    CmdPriority.SetCommandData(RDI_Analog, "4");
    RCManager.OnCmdRCSetTag(Ref, CmdPriority, RCChannel);

    CmdPriority.SetCommandData(RDI_Digital, "1");
    RCManager.OnCmdRCSetTag(Ref, CmdPriority, RCChannel);

    //test event class data item
    NetCommands::CmdRCSetTag CmdEventClass(5000, RC_DATAITEM_SET_EVENT_CLASS,RDI_Analog, "1");

    RCManager.OnCmdRCSetTag(Ref, CmdEventClass, RCChannel);

    CmdEventClass.SetCommandData(RDI_Analog, "2");
    RCManager.OnCmdRCSetTag(Ref, CmdEventClass, RCChannel);

    CmdEventClass.SetCommandData(RDI_Analog, "3");
    RCManager.OnCmdRCSetTag(Ref, CmdEventClass, RCChannel);

    CmdEventClass.SetCommandData(RDI_Analog, "4");
    RCManager.OnCmdRCSetTag(Ref, CmdEventClass, RCChannel);

    CmdEventClass.SetCommandData(RDI_Analog, "-1");
    RCManager.OnCmdRCSetTag(Ref, CmdEventClass, RCChannel);

    CmdEventClass.SetCommandData(RDI_Digital, "1");
    RCManager.OnCmdRCSetTag(Ref, CmdEventClass, RCChannel);

    //test request assset info data item
    NetCommands::CmdRCSetTag CmdRequestAsset(5000, RC_DATAITEM_REQUEST_ASSET_INFO,RDI_Analog, "1");

    RCManager.OnCmdRCSetTag(Ref, CmdRequestAsset, RCChannel);

    CmdRequestAsset.SetCommandData(RDI_Analog, "0");
    RCManager.OnCmdRCSetTag(Ref, CmdRequestAsset, RCChannel);

    CmdRequestAsset.SetCommandData(RDI_Analog, "2");
    RCManager.OnCmdRCSetTag(Ref, CmdRequestAsset, RCChannel);

    CmdRequestAsset.SetCommandData(RDI_Digital, "1");
    RCManager.OnCmdRCSetTag(Ref, CmdRequestAsset, RCChannel);

    //test number of logfiles data item
    NetCommands::CmdRCSetTag CmdLogFiles(5000, RC_DATAITEM_SET_LOG_NUMBER,RDI_Analog, "1");

    RCManager.OnCmdRCSetTag(Ref, CmdLogFiles, RCChannel);

    CmdLogFiles.SetCommandData(RDI_Analog, "2");
    RCManager.OnCmdRCSetTag(Ref, CmdLogFiles, RCChannel);

    CmdLogFiles.SetCommandData(RDI_Analog, "3");
    RCManager.OnCmdRCSetTag(Ref, CmdLogFiles, RCChannel);

    CmdLogFiles.SetCommandData(RDI_Analog, "4");
    RCManager.OnCmdRCSetTag(Ref, CmdLogFiles, RCChannel);

    CmdLogFiles.SetCommandData(RDI_Analog, "5");
    RCManager.OnCmdRCSetTag(Ref, CmdLogFiles, RCChannel);

    CmdLogFiles.SetCommandData(RDI_Digital, "1");
    RCManager.OnCmdRCSetTag(Ref, CmdLogFiles, RCChannel);

    //test request remote session data item
    NetCommands::CmdRCSetTag CmdRequestRemote(5000, RC_DATAITEM_REQUEST_REMOTE_SESSION,RDI_Analog, "1");

    RCManager.OnCmdRCSetTag(Ref, CmdRequestRemote, RCChannel);

    CmdRequestRemote.SetCommandData(RDI_Analog, "0");
    RCManager.OnCmdRCSetTag(Ref, CmdRequestRemote, RCChannel);

    CmdRequestRemote.SetCommandData(RDI_Analog, "2");
    RCManager.OnCmdRCSetTag(Ref, CmdRequestRemote, RCChannel);

    CmdRequestRemote.SetCommandData(RDI_Digital, "1");
    RCManager.OnCmdRCSetTag(Ref, CmdRequestRemote, RCChannel);

    //test SW update available data item
    NetCommands::CmdRCSetTag CmdSWAvailable(5000, RC_DATAITEM_SET_UPDATE_AVAILABLE,RDI_Analog, "1");

    RCManager.OnCmdRCSetTag(Ref, CmdSWAvailable, RCChannel);

    CmdSWAvailable.SetCommandData(RDI_Analog, "0");
    RCManager.OnCmdRCSetTag(Ref, CmdSWAvailable, RCChannel);

    CmdSWAvailable.SetCommandData(RDI_Analog, "2");
    RCManager.OnCmdRCSetTag(Ref, CmdSWAvailable, RCChannel);

    CmdSWAvailable.SetCommandData(RDI_Digital, "1");
    RCManager.OnCmdRCSetTag(Ref, CmdSWAvailable, RCChannel);

    //test SW update downloaded data item
    NetCommands::CmdRCSetTag CmdSWDownloadFinished(5000, RC_DATAITEM_SET_DOWNLOAD_FINISHED,RDI_Analog, "1");

    RCManager.OnCmdRCSetTag(Ref, CmdSWDownloadFinished, RCChannel);

    CmdSWDownloadFinished.SetCommandData(RDI_Analog, "0");
    RCManager.OnCmdRCSetTag(Ref, CmdSWDownloadFinished, RCChannel);

    CmdSWDownloadFinished.SetCommandData(RDI_Analog, "2");
    RCManager.OnCmdRCSetTag(Ref, CmdSWDownloadFinished, RCChannel);

    CmdSWDownloadFinished.SetCommandData(RDI_Digital, "1");
    RCManager.OnCmdRCSetTag(Ref, CmdSWDownloadFinished, RCChannel);

    //test outgoing commands to RCA
    CmdRCRequestRemoteSession CmdRemoteSessionAccepted(5000, RemoteSession_Accepted);
    RCManager.OnCmdRCRequestRemoteSession(Ref, CmdRemoteSessionAccepted, RCChannel);

    CmdRCRequestRemoteSession CmdRemoteSessionDenied(5000, RemoteSession_Denied);
    RCManager.OnCmdRCRequestRemoteSession(Ref, CmdRemoteSessionDenied, RCChannel);

    //test outgoing commands to RCA
    CmdRCSoftwareUpdate CmdRCSSWStartDownload(5000, SWUpdate_StartDownload);
    RCManager.OnCmdRCSoftwareUpdate(Ref, CmdRCSSWStartDownload, RCChannel);

    RCManager.SWUpdateStatus(true);
    RCManager.SWUpdateStatus(false);
    RCManager.SendNotifyReconnectToRemoteCare();

//    RCManager.ExternalProcessExited("RemoteCare", 0);
//    RCManager.ExternalProcessError(1);

    NetCommands::RCLogEventReportStruct EventReportData;
    NetCommands::CmdRCSetLogEvent CmdLogEvent(5000, EventReportData);

    EventReportData.EventCode = EVENT_RCMANAGER_RECEIVED_COMMAND;
    RCManager.OnCmdRCSetLogEventHandler(Ref, CmdLogEvent, RCChannel);

    QList<QString> StringList;
    StringList << "1" << "2";

    EventReportData.AltStringUsage = Global::GUI_MSG_BOX;
    NetCommands::CmdRCSetLogEvent CmdLogEvent2(5000, EventReportData);
    RCManager.OnCmdRCSetLogEventHandler(Ref, CmdLogEvent2, RCChannel);

    EventReportData.EventStringList = StringList;
    NetCommands::CmdRCSetLogEvent CmdLogEvent1(5000, EventReportData);
    RCManager.OnCmdRCSetLogEventHandler(Ref, CmdLogEvent1, RCChannel);

}

}
QTEST_MAIN(RemoteCare::TestRemoteCareManager)

#include "TestRemoteCareManager.moc"


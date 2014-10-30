/****************************************************************************/
/*! \file RemoteCareManager/Include/RemoteCareManager.h
 *
 *  \brief Definition file for class RemoteCareManager
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2014-03-13
 *  $Author:    $ Ramya GJ
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
#ifndef REMOTECARE_REMOTECAREMANAGER_H
#define REMOTECARE_REMOTECAREMANAGER_H


#include "DataManager/Include/DataManagerBase.h"
#include "DataManager/Containers/RCConfiguration/Include/RCConfiguration.h"
#include "DataManager/Containers/RCConfiguration/Include/RCConfigurationInterface.h"
#include "Threads/Include/MasterThreadController.h"

#include <NetCommands/Include/CmdRCSetLogEvent.h>

#include <NetCommands/Include/CmdRCRequestRemoteSession.h>
#include <NetCommands/Include/CmdRCSetTag.h>
#include <NetCommands/Include/CmdRCNotifyDataItem.h>
#include <NetCommands/Include/CmdRCSoftwareUpdate.h>

namespace DataLogging {
    class DayEventEntry;
}
namespace RemoteCare {

class TestRemoteCareManager;

/****************************************************************************/
/**
 * \brief This class manages the remote care commands  which comes from RCA and forwards it to
 *         ColoradoMasterThread.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class RemoteCareManager : public QObject
{
    Q_OBJECT
    friend class Threads::MasterThreadController;
    friend class RemoteCare::TestRemoteCareManager;
public:
    RemoteCareManager(Threads::MasterThreadController &MasterThreadControllerRef,
                      DataManager::CRCConfigurationInterface *RCConfigurationInterface,
                      bool remoteCareStatus);

    void Init();

    void SendCommandToRemoteCare(const Global::CommandShPtr_t &);

private:

    typedef void (RemoteCareManager::*FctTagName)(const Global::tRefType &,
                           const NetCommands::CmdRCSetTag &,
                           Threads::CommandChannel &);                   //! function pointer definition
    typedef QHash<QString, FctTagName> FctTagNameHash;                   //! hash map definition

    FctTagNameHash m_FctTagNameHash;                                     //! hash map list

    //Data Members
    Threads::MasterThreadController &m_MasterThreadControllerRef;        //!< Master thread controller.
    QString m_EventClass;                                                //!< The event class
    Global::EventLogLevel m_EventPriority;                               //!< The event priority
    bool m_RCAAvailable;                                                 //!< is remote care agent active
    bool m_IsConnectedToWeb;                                             //!< connected to web flag
    bool m_SubscriptionStatus;                                           //!< is remote care subscribed
    bool m_RCSoftwareAvailable;                                          //!< is remote care software update
    quint8 m_NumberOfLogFiles;                                           //!< total number of log files to be exported to RC
    DataManager::CRCConfigurationInterface *mp_RCConfigurationInterface; //!< RC Config interface.
    DataManager::CRCConfiguration *mp_RCConfiguration;                   //!< RC Config.

    bool m_RemoteCareStatus;                                             //! Remote care enable/disable.

    QHash<QString, Global::EventLogLevel> m_EventLogLevelEnumMap;        //!< Hash of Event Log Level to RemoteCare values


    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(RemoteCareManager)

    /****************************************************************************/
    /**
     * \brief This function registers all Remote care commands  which comes from RCA
     */
    /****************************************************************************/
    void RegisterCommands();

    /****************************************************************************/
    /**
     * \brief This function reads current settings form xml file
     */
    /****************************************************************************/
    void ReadCurrentSettingsState();

    /****************************************************************************/
    /*!
     *  \brief   Function to setup maps
     */
    void SetupMaps();

    /****************************************************************************/
    /**
     * \brief Used for the remote care events to log in the log file
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void OnCmdRCSetLogEventHandler(const Global::tRefType Ref, const NetCommands::CmdRCSetLogEvent &Cmd,
                               Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Used when set event priority data item is received from RCA
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void SetEventPriorityHandler(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                               Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Used when set event class data item is received from RCA
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void SetEventClassHandler(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                               Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Used when SetSubscription data item is received from RCA
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void SetSubscriptionHandler(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                                Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Used when RequestAssetInformation data item is received from RCA
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void RequestAssetInfoHandler(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                               Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Used when SetLogNumber data item is received from RCA
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void SetLogNumberHandler(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                               Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Called when RCA informs that a new SW is available or not
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void SetUpdateAvailable(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                            Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Called when RCA informs that a new SW is downloaded or not
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void SetDownloadFinished(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                                                Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Used when remote session is accepted/denied by user through GUI
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void OnCmdRCRequestRemoteSession(const Global::tRefType Ref, const RemoteCare::CmdRCRequestRemoteSession &Cmd,
                               Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Used when software update if requested from GUI
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void OnCmdRCSoftwareUpdate(const Global::tRefType Ref, const RemoteCare::CmdRCSoftwareUpdate &Cmd,
                               Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief comamnd hanlder for the incoming CmdRCSetTag Command
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void OnCmdRCSetTag(const Global::tRefType Ref, const NetCommands::CmdRCSetTag &Cmd,
                               Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Used when RequestRemoteSession data item is received from RCA
     *
     * \iparam Ref - Reference of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void RequestRemoteSessionHandler(const Global::tRefType &Ref, const NetCommands::CmdRCSetTag &Cmd,
                                     Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief updates the data item present in the container
     *
     * \iparam Cmd - Command class
     * \iparam WriteToDisk - whether the data to be written on disk or not
     *
     * \return void
     */
    /****************************************************************************/
    void UpdateContainerDataItem(const NetCommands::CmdRCSetTag &Cmd,
                                                         const bool &WriteToDisk=true);

    /****************************************************************************/
    /**
     * \brief updates the data item present in the container
     *
     * \iparam Name - data item name
     * \iparam Quality - data item Quality
     * \iparam Value - data item Value
     * \iparam WriteToDisk - whether the data to be written on disk or not
     *
     * \return Data Item type (analog, digital, string etc)
     */
    /****************************************************************************/
    RemoteCare::RCDataItemType_t UpdateContainerDataItem(const QString &Name,
                                 const RemoteCare::RCDataItemQuality_t &Quality, const QString &Value, const bool &WriteToDisk=true);

    /****************************************************************************/
    /**
     * \brief send the notify data item command to RCS
     *
     * \iparam Name - data item name
     * \iparam Quality - data item Quality
     * \iparam Value - data item Value
     * \iparam WriteToDisk - whether the data to be written on disk or not
     */
    /****************************************************************************/
    void SendNotifyDataItem(const QString &Name, const RemoteCare::RCDataItemQuality_t &Quality, const QString &Value
                            , const bool &WriteToDisk=true);

public slots:
    /****************************************************************************/
    /**
     * \brief Inform remote care to get again connected to server due to settings/data item change
     */
    /****************************************************************************/
    void SendNotifyReconnectToRemoteCare();
    /****************************************************************************/
    /**
     * \brief Inform remote care to set enable/disable
     * \iparam status remote status
     */
    /****************************************************************************/
    void SetRemoteCareStatus(bool status)
    {
        m_RemoteCareStatus = status;
    }

private slots:
    /************************************************************************************/
    /*!
     *  \brief This slot is called when Master SW is conencted to RCA
     */
    /************************************************************************************/
    void RCAConnected();
    /************************************************************************************/
    /*!
     *   \brief    This slot is called when ExternalProcess terminates.
     *
     *  \iparam name = internal name of the process
     *  \iparam code = process exit code
     */
    /************************************************************************************/
    void ExternalProcessExited(const QString &name, int code);
    /************************************************************************************/
    /*!
     *  \brief This slot is called when external process has stopped after several retries.
     */
    /************************************************************************************/
    void ExternalProcessStoppedForever();

    /****************************************************************************/
    /**
     * \brief Forward event to Remote Care
     *
     * \iparam   &TheEvent
     * \iparam   EventId64
     */
    /****************************************************************************/
    void ForwardEventToRemoteCare(const DataLogging::DayEventEntry &TheEvent, const quint64 EventId64);

    /****************************************************************************/
    /**
     * \brief Inform RCA when export is over
     *
     * \iparam   FileName = Part of the file name
     */
    /****************************************************************************/
    void RCExportFinished(const QString FileName);

    /****************************************************************************/
    /**
     * \brief Inform RCA SW update is success /failure
     *
     * \iparam   Status = true or false
     */
    /****************************************************************************/
    void SWUpdateStatus(bool Status);

signals:
    /****************************************************************************/
    /**
     * \brief This signal is emitted when a asset request is received from RCA.
     *          This is caught by MasterThreadController and the service export will
     *              be done to RemoteCare folder
     *
     *  \iparam LogDays Number of day log files needs ot be exported
     */
    /****************************************************************************/
    void RemoteCareExport(const quint8 &LogDays);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when new SW update is initiated by GUI and the
     *          package is downloaded to the predefind path
     */
    /****************************************************************************/
    void UpdateSoftwareFromRC();

    /****************************************************************************/
    /**
     * \brief This signal is to send any command to GUI channel
     *
     *  \iparam Cmd = command to be sent
     */
    /****************************************************************************/
    void SendRCCmdToGui(const Global::CommandShPtr_t &Cmd);
};
}

#endif // REMOTECARE_REMOTECAREMANAGER_H

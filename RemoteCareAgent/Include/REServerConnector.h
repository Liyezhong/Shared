/****************************************************************************/
/*! \file REServerConnector.h
 *
 *  \brief Definition file for class REServerConnector.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 29.04.2013
 *  $Author:    $ B. Stach
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

#ifndef RCAGENTNAMESPACE_RESERVERCONNECTOR_H
#define RCAGENTNAMESPACE_RESERVERCONNECTOR_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QThread>

#include "AeOSLocal.h"
#include "AeTypes.h"
#include "AeError.h"
#include "AeOS.h"
#include "AeInterface.h"

// commads
#include <Global/Include/Commands/Command.h>
#include <Global/Include/SharedPointer.h>
#include <Global/Include/RemoteCareTypes.h>

#include "RCConfigurationWrapper.h"

namespace RCAgentNamespace {

/// state of the AgentEmbedded
typedef enum {
    AGENT_NOT_STARTED = 1,  ///< AgentEmbedded has not been started yet
    AGENT_IS_INITIALIZED,   ///< AgentEmbedded is initialized
    AGENT_IS_RUNNING,       ///< AgentEmbedded entered its working loop and is running
    AGENT_GOING_DOWN,       ///< AgentEmbedded will exit its working loop as soon as current iteration is finished
    AGENT_RESTART,          ///< AgentEmbedded listen for restart
    AGENT_STOPPED           ///< AgentEmbedded exited its working loop
} AgentStates_t;

/*************************************************************************************/
/*!
 *  \brief  This class is a wrapper for the Agent Embedded C-library.
 *          It implements Agent Embedded required callbacks and accesses
 *          its API. Furthermore the connection will be established and
 *          commands from REServer can be received and forward.
 *
 *  \warning N1: AgentEmbedded is supposed to run in its own thread. Do not
 *               call its members directly, but only via signals.
 *               Exceptions are "Getter" and Initialization methods.
 *
 *  \warning N2: If Alarm's/Event's/etc timestamp is in the past, the Remote Enterprise
 *               Server seem to ignore incoming messages without any complaining.
 *               E.g. the message reception will be acknowledged, but
 *               corresponding Alarms/Events/Etc will not be shown. Which means
 *               if there is some kind of consistent TimeStamp failure on any
 *               device, the Remote Enterprise Server might stop showing all notifications
 *               sent by this faulty device.
 */
/*************************************************************************************/

class REServerConnector : public QObject
{
    Q_OBJECT

    /// test classes are all friends:
    friend class TestREServerConnector;

public:

    REServerConnector();
    virtual ~REServerConnector();

    bool Initialize();
    void Reconnect();

    AeInt32         GetExecTime();
    AgentStates_t   GetState();

signals:

    /****************************************************************************/
    /*!
     *  \brief   This signal is emitted to deliver status of the Controller's request.
     *
     *  \iparam   ref    = reference of the request
     *  \iparam   result = true/false
     */
    /*****************************************************************************/
    void SigRequestStatus(const Global::tRefType ref, const bool &result);

    /****************************************************************************/
    /*!
     *  \brief   This signal is emitted to deliver Server's command
     *           to the Controller.
     *
     *  \warning  Agent commands are (so far) created with no timeouts.
     *            If you need timeout (for example if command will be forwarded to
     *            Master), then you need to set the timeout on the Controller side.
     *
     *  \iparam   ptr = shared pointer to the command
     */
    /*****************************************************************************/
    void SigAgentIncomingCommand(const Global::CommandShPtr_t &ptr);

public slots:

    void Start();
    void Stop();

    void OnMasterInitializationFailed();

    void SubmitEventRequest(
                                const Global::tRefType  &ref,
                                const QString           &nameStr,
                                const QString           &messageStr,
                                const QString           &severity,
                                const QString           &timestamp
                            );

    bool SubmitEventWithoutAck(
                                const QString           &nameStr,
                                const QString           &messageStr,
                                const QString           &severity,
                                const QString           &timestamp
                            );

    void SubmitDataItemRequest(
                                const Global::tRefType                  &ref,
                                const QString                           &name,
                                const RemoteCare::RCDataItemType_t      &type,
                                const RemoteCare::RCDataItemQuality_t   &quality,
                                const QString                           &value,
                                const QString                           &timestamp
                               );

    void SubmitUploadRequest(
                                const Global::tRefType  &ref,
                                const QString           &filenameStr
                            );

private slots:
    void Work();

private:
    Q_DISABLE_COPY(REServerConnector)

    bool InitializeSettings();

    void Restart();
    void RegisterAllCallbacks();
    void CheckIncomingCommandQueue();
    void ChangeAgentState(const AgentStates_t &state);

    static bool CheckIfFileExists(const QString &filename);

    /// ==== Member functions for AgentEmbedded Callbacks: ============
    /// == They have to be static to be used as callbacks. ==========

    static AeBool OnRemoteSessionStartCallback(AeRemoteInterface *pInterface);
    static void OnRemoteSessionEndCallback(AeRemoteInterface *pInterface);

    static void OnWebErrorCallback(AeInt32 iServerId, AeError iError);
    static void OnDeviceRegisteredCallback(AeInt32 iDeviceId);
    static void QueueStatusCallback(AeDRMQueueStatus iNewStatus);
    static void OnSOAPMethodCallback(AeInt32 iDeviceId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus);
    static void OnSOAPMethodExCallback(AeInt32 iDeviceId,
                                       AeInt32 iServerId,
                                       AeHandle pMethod,
                                       AeDRMSOAPCommandId *pSOAPId,
                                       AeDRMSOAPCommandStatus *pStatus
                                       );
    static void OnCommandSetTagCallback(AeInt32 iDeviceId, AeDRMDataItem *pDataItem, AeDRMSOAPCommandStatus *pStatus);
    static void OnCommandSetTimeCallback(AeInt32 iDeviceId, AeTimeValue *pTime, AeInt32 *piTZOffset, AeDRMSOAPCommandStatus *pStatus);
    static void OnCommandRestartCallback(AeInt32 iDeviceId, AeBool bHard, AeDRMSOAPCommandStatus *pStatus);
    static void OnPingRateUpdateCallback(AeInt32 iServerId, AeDRMPingRate *pPingRate);

    // File download specific
    static AeBool OnFileDownloadBeginCallback(AeInt32 iDeviceId, AePointer *ppUserData);
    static AeBool OnFileDownloadDataCallback(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, AeInt32 iSize, AePointer pUserData);
    static void OnFileDownloadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData);

    static QList<Global::CommandShPtr_t>    incomingCommandQueue; ///< Queue for the incoming from the Remote Care Server commands
    static RCConfigurationWrapper           confWrapper;          ///< Wrapper to configuration of RemoteCare Agent
    static std::string                      downloadPath;         ///< Path where files will be downloaded to
    static std::ofstream                    ofs;                  ///< Output stream for download file

    static AgentStates_t                    stateAgent;           ///< Agent's state variable

    QThread                                 threadRCAClient;      ///< Internal thread which handles REServer requests

    AeInt32                                 deviceID;             ///< Device ID
    AeInt32                                 serverID;             ///< Server ID

    bool                                    isOnRun;              ///< Agent is running
};
} // end namespace

#endif // RCAGENTNAMESPACE_RESERVERCONNECTOR_H

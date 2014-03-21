/****************************************************************************/
/*! \file AxedaAgent.cpp
 *
 *  \brief Definition file for class AxedaAgent.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 04.07.2011
 *  $Author:    $ Y.Novak
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

#ifndef RCAGENTNAMESPACE_AXEDAAGENT_H
#define RCAGENTNAMESPACE_AXEDAAGENT_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QByteArray>

#include "AeOSLocal.h"
#include "AeTypes.h"
#include "AeError.h"
#include "AeOS.h"
#include "AeInterface.h"

#include <Global/Include/SharedPointer.h>
#include <RemoteCareAgent/Include/AgentController.h>
#include <RemoteCareAgent/Include/AgentCmdFactory.h>

namespace RCAgentNamespace {

/// ack status of Controller's request
const QString AGENT_REQUEST_OK = "ok";
/// nack status of Controller's request
const QString AGENT_REQUEST_FAILED = "failed";
/// default upload chunk size is 1MB
const AeInt32 AGENT_UPLOAD_CHUNK_SIZE = (AeInt32)1000000;
/// internally used Date and Time format (for conversions)
const QString DATEANDTIME_FORMAT = "dd.MM.yyyy hh:mm:ss";
/// prefix for commands instantiation
const QString AGENT_COMMAND_PREFIX = "RCAgentNamespace::CmdAxeda";
/// slash for path extention
const QString AGENT_SLASH_IN_PATH = "/"; ///< \todo: it is linux-specific at the moment!

class CmdAxedaSetTag;

/// Typedef for a shared pointer of AgentCmdFactory
typedef Global::SharedPointer<RCAgentNamespace::AgentCmdFactory>    AgentCmdFactoryShPtr_t;
/// Typedef for the AgentCmdFactoryShPtr_t factory hash
typedef QHash<QString, AgentCmdFactoryShPtr_t>                      AgentCmdFactoryHash_t;

/// struct used for uploading files to server
class AgentUploadContainer
{
public:
    AgentUploadContainer() {
        UploadSpecList = NULL;
        FileStat.pName = NULL;
        FileData = NULL;
        ContentsCounter = 0;
        IsUploaded = AeFalse;
    };
    ~AgentUploadContainer() {
        UploadSpecList = NULL;
        FileStat.pName = NULL;
        if (FileData != NULL) {
            delete FileData;
        }
    };
public:
    AeFileUploadSpec **UploadSpecList;  ///< File upload parameters (Axeda internal)
    AeFileStat         FileStat;        ///< File description parameters (Axeda internal)
    AeChar            *FileData;        ///< File contents
    qint64             ContentsCounter; ///< "Pointer" to the file's contents (aka from where the next chunk shall be read)
    AeBool             IsUploaded;      ///< True if file is already uploaded, false otherwise
};

/// struct which holds the RemoteCareAgent configuration
class AgentConfiguration
{
public:
    AgentConfiguration() :
            QueueSize((AeInt32)(AGENT_UPLOAD_CHUNK_SIZE*2)), ///< let the queue hold at least one full upload chunk
            bPersist(AeTrue),
            //ExecTime,
            bDebug(AeFalse),
            bSecure(AeTrue),
            CryptLevel(AeWebCryptoMedium),
            bAuthenticate(AeFalse),
            pCertFile(NULL),
            //PingRate,
            ServerType(AeDRMServerConfigPrimary),
            pOwner((AeChar*)"drm-data_source"),
            pServerUrl((AeChar*)"https://leica-sandbox.axeda.com:443/eMessage"),
            ServerId(0),
            pSerialNumber((AeChar*)"DemoDevice1"),
            pModelNumber((AeChar*)"DemoModel1"),
            DeviceType(AeDRMDeviceMaster),
            DeviceId(0),
            bUseProxy(AeFalse),
            ProxyProtocol(AeWebProxyProtoNone),
            pProxyHost((AeChar*)""),
            pProxyUser((AeChar*)""),
            pProxyPassword((AeChar*)""),
            ProxyPort(0),
            bAllowRemoteSession(AeFalse),
            bRemoteSessionSetup(AeTrue),
            pRemoteSessionName((AeChar*)"Himalaya"),
            pRemoteSessionType((AeChar*)"desktop"),
            pRemoteSessionHost((AeChar*)"127.0.0.1"),
            RemoteSessionPort((AeInt16)5900),
            bCompress(AeFalse),
            MaxChunkSize(AGENT_UPLOAD_CHUNK_SIZE)
     {
        ExecTime.iSec = (AeInt32)8;
        ExecTime.iMicroSec = (AeInt32)0;
        PingRate.iSec = (AeInt32)15;
        PingRate.iMicroSec = (AeInt32)0;
     };
    ~AgentConfiguration() {
        pCertFile = NULL;
        pOwner = NULL;
        pServerUrl = NULL;
        pSerialNumber = NULL;
        pModelNumber = NULL;
        pProxyHost = NULL;
        pProxyUser = NULL;
        pProxyPassword = NULL;
        pRemoteSessionName = NULL;
        pRemoteSessionType = NULL;
        pRemoteSessionHost = NULL;
    };
public:
    /// general Agent configuration parameters
    AeInt32               QueueSize;           ///< Outgoing message queue size (in bytes)
    AeBool				  bPersist;            ///< If persistent HTTP(S) connection to RC Server shall be used
    AeTimeValue           ExecTime;            ///< Execution time period for the AeDrmExecute function
    AeBool                bDebug;              ///< If debug messages shall be enabled
    /// Secure connection config parameters
    AeBool                bSecure;             ///< If secure HTTPS connection to RC Server shall be used
    AeWebCryptoLevel      CryptLevel;          ///< Encryption level (low, medium or high)
    AeBool                bAuthenticate;       ///< If server certificate shall be validated
    AeChar                *pCertFile;          ///< Link to file which contains certificate
    /// Remote Care Server parameters
    AeTimeValue           PingRate;            ///< How often the Server shall be pinged (in ms)
    AeDRMServerConfigType ServerType;          ///< RC Server type: primary/secondary/etc.
    AeChar                *pOwner;             ///< The name of target database
    AeChar                *pServerUrl;         ///< URL of the RC Server
    AeInt32               ServerId;            ///< The assigned server ID (output parameter, assigned by Agent)
    /// Local Device Identification
    AeChar				  *pSerialNumber;      ///< Unique serial number of the local device
    AeChar				  *pModelNumber;       ///< Local device's model number
    AeDRMDeviceType       DeviceType;          ///< Local device's type (master or managed)
    AeInt32               DeviceId;            ///< The assigned device ID (output parameter, assigned by Agent)
    /// Local network proxy parameters
    AeBool                bUseProxy;           ///< If Proxy Server shall be used
    AeWebProxyProtocol    ProxyProtocol;       ///< Communication protocol to use with the Proxy
    AeChar                *pProxyHost;         ///< Proxy Server IP (?)
    AeChar                *pProxyUser;         ///< User Name for Proxy Authentication
    AeChar                *pProxyPassword;     ///< Password for Proxy Authentication
    AeInt16               ProxyPort;           ///< Proxy Server Port
    /// Remote Session parameters
    AeBool                bAllowRemoteSession; ///< If Remote Session is allowed (at runtime)
    AeBool                bRemoteSessionSetup; ///< If Remote Session is configured
    AeChar                *pRemoteSessionName; ///< Arbitrary Remote Session name
    AeChar                *pRemoteSessionType; ///< Session type (e.g. desktop/telnet/browser)
    AeChar                *pRemoteSessionHost; ///< Local VNC Server's IP (e.g (AeChar*)"127.0.0.1")
    AeInt16               RemoteSessionPort;   ///< Local VNC Server's Port (e.g 5900)
    /// file upload parameters
    AeBool				  bCompress;           ///< If compression shall be used for uploads
    AeInt32               MaxChunkSize;        ///< Maximum size of the file upload data chunk (in bytes)
};

/// state of the AxedaAgent
typedef enum {
    AGENT_NOT_STARTED = 1,  ///< AxedaAgent has not been started yet
    AGENT_IS_RUNNING,       ///< AxedaAgent entered its working loop and is running
    AGENT_GOING_DOWN,       ///< AxedaAgent will exit its working loop as soon as current iteration is finished
    AGENT_STOPPED           ///< AxedaAgent exited its working loop
} AgentStates_t;

/****************************************************************************/
/**
 * \brief  This class is a wrapper for the Axeda Agent Embedded C-library.
 *         It implements all Axeda Agent Embedded callbacks and accesses
 *         its API.
 *
 * \warning N1: AxedaAgent is supposed to run in its own thread. Do not
 *              call its members directly, but only via signals.
 *              Exceptions are "Getter" and Initialization methods.
 *
 * \warning N2: If Alarm's/Event's/etc timestamp is in the past, the Axeda
 *              Server seem to ignore incoming messages without any complaining.
 *              E.g. the message reception will be acknowledged, but
 *              corresponding Alarms/Events/Etc will not be shown. Which means
 *              if there is some kind of consistent TimeStamp failure on any
 *              device, the Axeda Server might stop showing all notifications
 *              sent by this faulty device.
 *     \todo:   Check if there is a setting on the server which can override
 *              this behaviour.
 */
/****************************************************************************/
class AxedaAgent : public QObject
{
    Q_OBJECT

    /// test classes are all friends:
    //friend class ;

public:

    AxedaAgent();
    virtual ~AxedaAgent();
    bool Initialize();
    AeInt32 GetExecTime();
    AgentStates_t GetState();

signals:

    /****************************************************************************/
    /*!
    *  \brief   This signal is emitted when Axeda Agent exits its working loop.
    *
    *****************************************************************************/
    void SigAgentStopped();
    /****************************************************************************/
    /*!
    *  \brief   This signal is emitted to deliver status of the Controller's request.
    *
    *  \param   ref = reference of the request
    *  \param   status = ack/nack/failed/etc
    *
    *****************************************************************************/
    void SigRequestStatus(quint64 ref, const QString &status);
    /****************************************************************************/
    /*!
    *  \brief   This signal is emitted to deliver Server's command
    *           to the Controller.
    *
    *  \warning  Agent commands are (so far) created with no timeouts.
    *            If you need timeout (for example if command will be forwarded to
    *            Master), then you need to set the timeout on the Controller side.
    *
    *  \param   ptr = shared pointer to the command
    *
    *****************************************************************************/
    void SigAgentIncomingCommand(const Global::CommandShPtr_t &ptr);

public slots:

    void Stop();
    void SubmitAlarmRequest(quint64 ref, QByteArray name, QByteArray message, const QString &severity, QByteArray condition, const QString &time);
    void SubmitEventRequest(quint64 ref, QByteArray name, QByteArray message, const QString &severity, const QString &time);
    void SubmitDataItemRequest(quint64 ref, QByteArray name, AxedaDataItemType_t type, AxedaDataItemQuality_t quality, QByteArray value, const QString &time);
    void SubmitUploadRequest(quint64 ref, QByteArray filename);
    void SetRemoteSessionStatus(bool status);
    void Work();

private:

    bool ReadSettings();
    bool SetSettings();
    void RegisterAllCallbacks();
    void ConvertTime(QString time, AeTimeValue *timestamp);
    static AeBool ProcessUploadContainer(AgentUploadContainer *container, AeInt32 *chunksize);
    static AeBool CheckFileUploadStatus(qint64 readresult, qint64 filesize, qint64 startposition, AgentUploadContainer *container);
    static void DestroyContainer(AgentUploadContainer *container);
    static void SubmitSetTagCommand(const QString &name, AxedaDataItemType_t type, const QString &value);
    static bool CheckIfFileExists(const QString &filename);
    void CheckIncomingCommandQueue();

    /****************************************************************************/
    /*!
     *  \brief   Call this function to change Agent state.
     *
     *  \param   state = the state to put Agent into
     *
    *****************************************************************************/
    inline void ChangeAgentState(const AgentStates_t &state)
    {
        m_AgentState = state;
        if (m_AgentState == AGENT_STOPPED) {
            emit SigAgentStopped();
        }
    }

    /****************************************************************************/
    /**
     * \brief Register an Agent message for later instantiation.
     */
    /****************************************************************************/
    template<class TheClass>
    void RegisterAgentMessage()
    {
        // create functor
        AgentCmdFactoryShPtr_t Factory(new RCAgentNamespace::TemplateAgentCmdFactory<TheClass> ());
        // and register
        RegisterCmdFactory(TheClass::NAME, Factory);
    }

    void RegisterCmdFactory(const QString &cmdname, const AgentCmdFactoryShPtr_t &factory);
    static AgentCmdFactoryShPtr_t GetCmdFactory(const QString &cmdname);

    /// ==== Member functions for Axeda Agent Callbacks: ============
    /// == They have to be static to be used as callbacks. ==========

    static void OnWebErrorCallback(AeInt32 iServerId, AeError iError);
    static void OnDeviceRegisteredCallback(AeInt32 iDeviceId);
    static void QueueStatusCallback(AeDRMQueueStatus iNewStatus);
    static void OnRemoteSessionEndCallback(AeRemoteInterface *pInterface);
    static AeBool OnRemoteSessionStartCallback(AeRemoteInterface *pInterface);
    static void OnSOAPMethodCallback(AeInt32 iDeviceId, AeHandle pMethod, AeDRMSOAPCommandStatus *pStatus);
    static void OnSOAPMethodExCallback(AeInt32 iDeviceId, AeInt32 iServerId, AeHandle pMethod, AeDRMSOAPCommandId *pSOAPId, AeDRMSOAPCommandStatus *pStatus);
    static void OnCommandSetTagCallback(AeInt32 iDeviceId, AeDRMDataItem *pDataItem, AeDRMSOAPCommandStatus *pStatus);
    static AeBool OnFileDownloadBeginCallback(AeInt32 iDeviceId, AePointer *ppUserData);
    static AeBool OnFileDownloadDataCallback(AeInt32 iDeviceId, AeFileStat *pFile, AeChar *pData, AeInt32 iSize, AePointer pUserData);
    static void OnFileDownloadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData);
    static AeBool OnFileUploadBeginCallback(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, AePointer *ppUserData);
    static AeBool OnFileUploadBeginExCallback(AeInt32 iDeviceId, AeFileUploadSpec **ppUploads, AeFileUploadExecuteParam *pParam, AePointer *ppUserData);
    static AeBool OnFileUploadDataCallback(AeInt32 iDeviceId, AeFileStat **ppFile, AeChar **ppData, AeInt32 *piSize, AePointer pUserData);
    static void OnFileUploadEndCallback(AeInt32 iDeviceId, AeBool bOK, AePointer pUserData);
    static void OnFileTransferEventCallback(AeInt32 iDeviceId, AeFileTransferEvent iEvent, AePointer pUserData);
    static void OnCommandSetTimeCallback(AeInt32 iDeviceId, AeTimeValue *pTime, AeInt32 *piTZOffset, AeDRMSOAPCommandStatus *pStatus);
    static void OnCommandRestartCallback(AeInt32 iDeviceId, AeBool bHard, AeDRMSOAPCommandStatus *pStatus);
    static void OnPingRateUpdateCallback(AeInt32 iServerId, AeDRMPingRate *pPingRate);

private:

    static AgentConfiguration              m_AgentConfig;          ///< Agent's configuration settings
    static QList<AgentUploadContainer*>    m_UploadContainers;     ///< List of containers for file uploads
    static QList<Global::CommandShPtr_t>   m_IncomingCommandQueue; ///< Queue for the incoming from the Remote Care Server commands
    AgentStates_t                          m_AgentState;           ///< Agent's state variable

    static AgentCmdFactoryHash_t           m_CmdFactories;         ///< Specific command factories
};

} // end namespace

#endif // RCAGENTNAMESPACE_AXEDAAGENT_H

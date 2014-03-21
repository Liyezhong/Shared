/****************************************************************************/
/*! \file NetworkDevice.h
 *
 *  \brief Base class for project specific Network Device
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.11.2010
 *   $Author:  $ Y.Novak
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

#ifndef NETWORKBASE_NETWORKDEVICE_H
#define NETWORKBASE_NETWORKDEVICE_H


#include <QObject>
#include <QHash>
#include <QString>
#include <QDomElement>
#include <NetworkComponents/Include/MessageChecker.h>
#include <Global/Include/AbstractFactory.h>
#include <Global/Include/GlobalDefines.h>

namespace ExternalProcessControl {
    class TestExternalProcessDvc;
}

namespace NetworkBase {

class ProtocolTxCommand;

/// Period for the HeartBeat check between Network Devices
const int HEARTBEAT_SERVER_DELAY = 60000; // 60000 ms = 1 Minute

/// Period for the HeartBeat check between Server and Client
/// It needs to be longer than server's period to avoid accidental disconnect
const int HEARTBEAT_CLIENT_DELAY = 80000; // 80000 ms = 1 Minute 20 seconds

/// Path ending to find config files under Settings directory
const QString CMH_PATH_TO_SETTINGS = "/Communication";

/// Type of configuration document
const QString CMH_CONFIGFILE_TYPE = "netlayer_messages";

const QString CMH_CMDNAME             = "name";       ///< Command name tag
const QString CMH_REFERENCE           = "ref";        ///< Identifier of the reference attribute in a message
const QString CMH_NOFITEMS            = "nofitems";   ///< Identifier of number of dataitems in a message
const QString CMH_CMD_TAG_NAME        = "cmd";        ///< Identifier of the command tag in a message
const QString CMH_DATA_TAG_NAME       = "dataitems";  ///< Name of the tag incapsulating the payload section of protocol messages
const QString CMH_MSG_ON_TAG          = "<message>";  ///< Message opening tag
const QString CMH_MSG_OFF_TAG         = "</message>"; ///< Message closing tag

const QString CMH_AUTHENTICATION_REQ  = "<msg>Authenticate</msg>";  ///< Authentication request message
const QString CMH_AUTHENTICATION_CONF = "<msg>Welcome</msg>";       ///< Authentication confirmation message

const QString CMH_MSG_SENDING_OK      = "ok";         ///< Universal OK status for message sending
const QString CMH_MSG_SENDING_FAILED  = "failed";     ///< Universal NACK status for message sending
const QString CMH_MSG_SENDING_ACK     = "ack";        ///< Universal ACK status for message delivery
const QString CMH_MSG_SENDING_NACK    = "nack";       ///< Universal NACK status for message delivery
const QString CMH_MSG_SENDING_TIMEOUT = "timeout";    ///< Universal TIMEOUT status for message delivery

/// all existing Types of messages
typedef enum {
    NET_NETLAYER_MESSAGE = 0x10,    ///< 100% XML text message used internally by network layer
    NET_APPLICATION_MESSAGE         ///< Message with XML header and QByteArray payload used by application
} NetMessageType_t;

/// internally used Date and Time format (for conversions)
const QString DATEANDTIME_FORMAT = "dd.MM.yyyy hh:mm:ss";

/****************************************************************************/
/**
 * \brief This is a base class for a project specific NetworkDevice.
 *
 * Object, derived from this calss, shall implement a project specific
 * Network Device (e.g. Sepia in Himalaya).
 *
 */
/****************************************************************************/
class NetworkDevice : public QObject
{
    Q_OBJECT

    /// all test classes are friends:
    friend class ExternalProcessControl::TestExternalProcessDvc;
    friend class TestNetworkServerDevice;
    friend class TestNetworkClientDevice;
    friend class TestProtocolTxCommand;

    /// all base netlayer commands are friends
    friend class Ack;
    friend class DateAndTime;
    friend class HeartBeat;
    friend class HeartBeatClient;
    friend class HeartBeatServer;
    friend class ProtocolTxCommand;
    friend class ProtocolRxCommand;
    friend class SetDateAndTime;

public:

    NetworkDevice(MessageLoaderType_t ptype, const QString &path, QObject * pParent = 0);
    virtual ~NetworkDevice();

    virtual bool InitializeDevice();
    virtual void DisconnectPeer();
    void MessageSendingResult(Global::tRefType, const QString &);

    /****************************************************************************/
    /*!
     *  \brief Check if Base Class has a particular registered command instance.
     *
     *  \param[in]  Name - class name
     *
     *  \return  true if class registered, false otherwise
     */
    /****************************************************************************/
    template<class ClassT>
    inline bool CheckClassRegistration(const QString &Name)
    {
        return Global::AbstractFactory<ClassT>::CheckClassInstance(Name);
    }

signals:

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when protocol peer connects
     *
     *  \param    name = name of the connected peer
     *
     ****************************************************************************/
    void SigPeerConnected(const QString &name);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when protocol peer disconnects
     *
     *  \param    name = name of the disconnected peer
     *
     ****************************************************************************/
    void SigPeerDisconnected(const QString &name);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to send a message over connected
     *            TcpSocket.
     *
     *  \param    msg = message as String
     *
     ****************************************************************************/
    void SendMessage(const QString &msg);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to send a message over connected
     *            TcpSocket.
     *
     *  \param    type = message type
     *  \param    msg = message as byte array
     *
     ****************************************************************************/
    void SendMessage(quint8 type, const QByteArray &msg);
    /******************************************************************************/
    /*!
    *  \brief    This signal is emitted when parsing of incoming message failes.
    *
    *******************************************************************************/
    void MessageParsingFailed();
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to report a message sending result to
     *            upper layers.
     *
     *  \warning  Connect this signal to be informed about Acks/Nacks/Timeouts/etc.
     *
     *  \param    Ref = application message reference
     *  \param    status = message sending status (ok/failed/timeout/whatever)
     *
     ****************************************************************************/
     void SigMsgSendingResult(Global::tRefType Ref, const QString &status);
     /****************************************************************************/
     /*!
      *  \brief    This signal is emitted to report a HeartBeat problem
      *
      *  \param    error = error description (timeout/nack/etc.)
      *
      ****************************************************************************/
     void SigHeartBeatError(const QString &error);
     /****************************************************************************/
     /*!
      *  \brief    This signal is emitted to report a DateTime sync problem
      *
      *  \param    error = error description (timeout/nack/etc.)
      *
      ****************************************************************************/
     void SigDateTimeSyncError(const QString &error);

public slots:

    /****************************************************************************/
    /*!
     *  \brief    Implement this function to Start device operation
     *
     *  \return   True if device successfully started, false otherwise
     *
     ****************************************************************************/
    virtual bool StartDevice() = 0;
    /****************************************************************************/
    /*!
     *  \brief    Implement this function to stop and destroy device object
     *
     ****************************************************************************/
    virtual void DestroyDevice() = 0;

    virtual bool ProcessIncomingMessage(const QString &, const QDomDocument &);
    virtual bool ProcessIncomingMessage(const QString &, const QString &, const QByteArray &);
    virtual bool SendCommand(const QString &);
    virtual bool SendCommand(NetMessageType_t, const QByteArray &);
    virtual void PeerDisconnected(const QString &);
    virtual void PeerConnected(const QString &);
    void GetIncomingMsg(quint8, QByteArray &ba); ///< \warning: consciously non-const!
    void SendOutgoingCommand(const QString &cmdname, const QByteArray &bArray, Global::tRefType Ref);

protected slots:

    virtual void HandleHeartBeat();
    virtual void HandleHeartBeatTimeout();

protected:

    /****************************************************************************/
    /*!
     *  \brief  This method creates a new Incoming protocol command.
     *
     *
     *  \param[in]  cmdname - name of the command
     *  \param[in]  domD - incoming message as XML document
     *
     *  \return     pointer to the created command
     */
    /****************************************************************************/
    template <class TCommandClass, class TTemplateClass>
    TCommandClass* CreateNewIncomingCommand(const QString &cmdname, const QDomDocument &domD)
    {
        // create command processing object
        TCommandClass *pC = TTemplateClass::CreateInstance(cmdname);
        if (pC == NULL) {
          qDebug() << (QString)("T_ProtocolCommand (incoming) " + cmdname + " was NOT created !");
          return NULL;
        }
//        qDebug() << (QString)("T_ProtocolCommand created (incoming) : " + cmdname);
        // init the incoming command
        if (!pC->Initialize(cmdname, domD, this)) {
            qDebug() << (QString)("T_ProtocolCommand (incoming) " + cmdname + " cannot be initialized !");
            delete pC;
            return NULL;
        }
        return pC;
    }

    /****************************************************************************/
    /*!
     *  \brief  This method creates a new Incoming protocol command.
     *
     *
     *  \param[in]  cmdname - name of the command
     *  \param[in]  ref - the command reference
     *  \param[in]  bArr - incoming log file
     *
     *  \return     pointer to the created command
     */
    /****************************************************************************/
    template <class TCommandClass, class TTemplateClass>
    TCommandClass* CreateNewIncomingCommand(const QString &cmdname, const QString &ref, const QByteArray &bArr)
    {
        // create command processing object
        TCommandClass *pC = TTemplateClass::CreateInstance(cmdname);
        if (pC == NULL) {
          qDebug() << (QString)("T_ProtocolCommand (incoming) " + cmdname + " was NOT created !");
          return NULL;
        }
        qDebug() << (QString)("T_ProtocolCommand created (incoming) : " + cmdname);
        // init the incoming command
        if (!pC->Initialize(cmdname, ref, bArr, this)) {
            qDebug() << (QString)("T_ProtocolCommand (incoming) " + cmdname + " cannot be initialized !");
            delete pC;
            return NULL;
        }
        return pC;
    }

    /****************************************************************************/
    /*!
     *  \brief  This method creates a new outgoing protocol command.
     *
     *
     *  \param[in]  cmdname - name of the command
     *
     *  \return     pointer to the created command
     */
    /****************************************************************************/
    template <class TCommandClass, class TTemplateClass>
    TCommandClass* CreateNewOutgoingCommand(const QString &cmdname)
    {
        // create command processing object
        TCommandClass *pC = TTemplateClass::CreateInstance(cmdname);
        if (pC == NULL) {
            qDebug() << (QString)("T_ProtocolCommand (outgoing) " + cmdname + " was NOT created !");
            return NULL;
        }
        qDebug() << (QString)("T_ProtocolCommand created (outgoing) : " + cmdname);
        // fetch new reference
        quint32 ref = GetNewCmdReference();
        // init the outgoing command
        if (!pC->Initialize(cmdname, ref, this)) {
            qDebug() << (QString)("T_ProtocolCommand (outgoing) " + cmdname + " cannot be initialized !");
            delete pC;
            return NULL;
        }
        return pC;
    }

    /****************************************************************************/
    /*!
     *  \brief    Implement this function to initialize network connection
     *
     *  \return   True if network successfully initialized, false otherwise
     *
     ****************************************************************************/
    virtual bool InitializeNetwork() = 0;
    /****************************************************************************/
    virtual bool InitializeMessaging();
    ProtocolTxCommand* FetchRunningCommand(quint32 ref);
    void RegisterRunningCommand(quint32 ref, ProtocolTxCommand *cmd);
    void DeregisterRunningCommand(quint32 ref, ProtocolTxCommand *cmd);
    virtual void RestartHeartBeatTimer();
    virtual void ReportHeartBeatProblem(const QString &problem);
    virtual void ReportFailedDateTimeSync(const QString &problem);


private:

    NetworkDevice();                                            ///< Not implemented.
    NetworkDevice(const NetworkDevice &);                       ///< Not implemented.
    const NetworkDevice & operator = (const NetworkDevice &);   ///< Not implemented.

    quint32 GetNewCmdReference();
    void ParseNetLayerMessage(const QByteArray &ba);
    void ParseApplicationMessage(QByteArray &ba);
    QString ExtractCommandName(QDomDocument *domD);
    QString ExtractCommandReference(QDomDocument *domD);
    bool ExtractProtocolMessage(QByteArray *ba, QDomDocument *message);
    bool CheckIncomingMsgSchema(QDomDocument *msg, QString *CmdName);

private:

    /// message format checker
    MessageChecker       *m_myMessageChecker;
    /// the device can be server of client
    MessageLoaderType_t   m_myType;
    /// path to config files
    QString               m_myPath;
    /// command reference
    quint32               m_cmdRef;
    /// Hash of currently instantiated commands
    /// Format: unique command reference, pointer to the command object
    QHash<quint32, ProtocolTxCommand*> RunningCommands;
    /// period of the HeartBeat timer
    int                   m_HBDelay;
    /// Periodic HeartBeat timer
    QTimer                m_HeartBeatTimer;
};

} // end namespace NetworkBase

#endif // NETWORKBASE_NETWORKDEVICE_H

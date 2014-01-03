/****************************************************************************/
/*! \file MasterLinkController.h
 *
 *  \brief MasterLinkController definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 03.08.2011
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

#ifndef NETWORKBASE_MASTERLINKCONTROLLER_H
#define NETWORKBASE_MASTERLINKCONTROLLER_H

#include "NetworkComponents/Include/NetworkClientDevice.h"

namespace NetworkBase {

/// Type of network client to create
//const NetworkBase::NetworkClientType_t MY_CLIENT_TYPE = NetworkBase::NCE_TYPE_COLORADO_GUI;

/****************************************************************************/
/**
 * \brief This class is an interface and controller for the NetworkLayer.
 */
/****************************************************************************/
class MasterLinkController : public QObject
{
   Q_OBJECT

public:
    MasterLinkController(const QString &ip, const QString &port, const NetworkBase::NetworkClientType_t clientType);
    ~MasterLinkController();
    bool Initialize();

signals:
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to forward message to NetLayer
     *
     *  \param    name = command name
     *  \param    payload = message payload
     *  \param    Ref = application command reference
     *
     ****************************************************************************/
    void SigSendMessageToMaster(const QString &name, const QByteArray &payload, Global::tRefType Ref);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to forward message to Recipient
     *
     *  \param    name = command name
     *  \param    payload = message payload
     *
     ****************************************************************************/
    void SigForwardMsgToRecipient(const QString &name, const QByteArray &payload);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when new DateTime sync is received
     *            from Master.
     *
     *  \param    dt = the actual date and time
     *
     ****************************************************************************/
    void SigDateAndTime(const QDateTime &dt);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when protocol peer connects
     *
     *  \param    name = name of the connected peer
     *
     ****************************************************************************/
    void SigMasterConnected(const QString &name);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when protocol peer disconnects
     *
     *  \param    name = name of the disconnected peer
     *
     ****************************************************************************/
    void SigMasterDisconnected(const QString &name);
    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted to start timer , when working
     *            connection with Server fails
     *
     *
     ****************************************************************************/
     void StartConnectionLostTimer();

public slots:
    void Go();
    void Stop();
    bool SendMessageToMaster(const QString &, const QByteArray &, Global::tRefType Ref);
    void ForwardMsgToRecipient(const QString &cmdname, const QByteArray &barray);

private:
    NetworkBase::NetworkClientDevice *m_myNetClient;    ///< network layer
    QThread           m_NetworkThread;  ///< thread to move network communication to
    QString           m_IP;             ///< IP to connect to
    QString           m_Port;           ///< port to connect to
    NetworkBase::NetworkClientType_t m_clientType; ///< client type

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(MasterLinkController)
}; // class

}  // namespace

#endif

/****************************************************************************/
/*! \file NetworkClientDevice.h
 *
 *  \brief NetworkClientDevice definition.
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

#ifndef NETWORKBASE_NETWORKCLIENTDEVICE_H
#define NETWORKBASE_NETWORKCLIENTDEVICE_H

#include <NetworkComponents/Include/NetworkClient.h>
#include <NetworkComponents/Include/NetworkDevice.h>


namespace NetworkBase {

/// NetCLient possible status values
typedef enum {
    NETCLIENT_BUSY = 1,
    NETCLIENT_READY
} NetworkClientDeviceStatusType_t;

/****************************************************************************/
/**
 * \brief This class controls communication with NetworkServerDevice.
 *
 */
/****************************************************************************/
class NetworkClientDevice : public NetworkDevice
{
   Q_OBJECT

    /// all test classes are friends:
   friend class TestNetworkServerDevice;
   friend class TestNetworkClientDevice;

public:

    NetworkClientDevice(NetworkClientType_t ctype, const QString &ip, const QString &port, const QString &path, QObject *pParent = 0);
    virtual ~NetworkClientDevice();

    virtual bool InitializeDevice();
    virtual bool StartDevice();
    virtual void DestroyDevice();
    virtual void DisconnectPeer();
    virtual bool SetDateAndTime(const QString &, const QDateTime &);

public slots:

    virtual bool ProcessIncomingMessage(const QString &cmdname, const QString &ref, const QByteArray &bArr);
    virtual void HandleNetworkError(NetworkBase::NetworkClientErrorType_t);

signals:

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
     *  \brief    This signal is emitted to deliver incoming message to
     *            the upper layer.
     *
     *  \param    name = command's name
     *  \param    payload = command's payload
     *
     ****************************************************************************/
    void ForwardMessageToUpperLayer(const QString &name, const QByteArray &payload);

private:

    bool InitializeNetwork();

private:

    /// Network client for communication with peer
    NetworkClient      *m_myNetworkClient;
    /// Type of NetworkClient to instantiate
    NetworkClientType_t m_myClientType;
    /// server IP to connect to
    QString             m_myIp;
    /// server port to connect to
    QString             m_myPort;
};

} // end namespace NetworkBase

#endif // NETWORKBASE_NETWORKCLIENTDEVICE_H

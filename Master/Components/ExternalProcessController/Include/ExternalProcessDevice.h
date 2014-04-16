/****************************************************************************/
/*! \file ExternalProcessDevice.h
 *
 *  \brief ExternalProcessDevice definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.12.2010
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
#ifndef EXTERNALPROCESSCONTROL_EXTERNALPROCESSDEVICE_H
#define EXTERNALPROCESSCONTROL_EXTERNALPROCESSDEVICE_H

#include <NetworkComponents/Include/NetworkServerDevice.h>

namespace ExternalProcessControl {

class ExternalProcessController;

///< Login timeout for a process:
const int EP_LOGIN_TIMEOUT = 20000; // 20000 ms = 20 seconds

/****************************************************************************/
/**
 * \brief This class controls communication with External Process.
 *        ExternalProcessDevice is a TCP server. External GUI process is a
 *        TCP client.
 */
/****************************************************************************/
class ExternalProcessDevice : public NetworkBase::NetworkServerDevice
{
   Q_OBJECT

    /// all test classes are friends
    friend class TestExternalProcessDvc;

public:

    ExternalProcessDevice(NetworkBase::NetworkServerType_t server, const QString &client, \
                          const QString &path, ExternalProcessController *controller, QObject *pParent = 0);
    virtual ~ExternalProcessDevice();

    virtual bool InitializeDevice();
    virtual bool InitializeDeviceWithParameters(const QString &remoteLoginEnabled, int remoteLoginTimeout);
    virtual bool StartDevice();
    virtual void DestroyDevice();
    virtual bool ProcessIncomingMessage(const QString &cmdname, const QString &ref, const QByteArray &bArr);
    void DisconnectPeer();

signals:

    /****************************************************************************/
    /*!
     *  \brief    This signal is emitted when external process fails to login in time
     *
     ****************************************************************************/
    void SigLoginTimeout();

public slots:

    void PeerDisconnected(const QString &);
    void PeerConnected(const QString &);
    void HandleNetworkError(NetworkBase::DisconnectType_t , const QString &);
    void LoginTimedout();
    void StartLoginGuard();
    void StopLoginGuard();

private:

    ExternalProcessDevice();                                                    ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(ExternalProcessDevice)

    /// timer to guard the login timeout (upon powerup or process restart)
    QTimer  m_LoginTimer;
    /*! Remote login: Yes/No. If External Process is not started and managed by
        Master, remotelogin shall be set to "Yes". Master will just idly wait for connection then. */
    QString m_RemoteLoginEnabled;
    /// Timeout for remote login option, in seconds. Zero means infinite waiting.
    int     m_RemoteLoginTimeout;
    /// Pointer to the device's Controller:
    ExternalProcessController *m_myController;
};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_EXTERNALPROCESSDEVICE_H

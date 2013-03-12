/****************************************************************************/
/*! \file ExternalProcessDevice.cpp
 *
 *  \brief ExternalProcessDevice implementation.
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

#include <NetworkComponents/Include/ProtocolRxCommands/Ack.h>
#include <ExternalProcessController/Include/ExternalProcessDevice.h>
#include <ExternalProcessController/Include/ExternalProcessController.h>
#include <Global/Include/TranslatableString.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
* \brief Constructor.
*
* \param[in]   server = type of server to start
* \param[in]   client = client login string.
* \param[in]   path = path to ExternalProcess configuration files
* \param[in]   pController = pointer to the device's controller
* \param[in]   pParent = pointer to parent object
*
*****************************************************************************/
ExternalProcessDevice::ExternalProcessDevice(NetworkBase::NetworkServerType_t server, const QString &client, \
                                             const QString &path, ExternalProcessController *pController, QObject *pParent) :
        NetworkServerDevice::NetworkServerDevice(server, client, path, pParent),
        m_RemoteLoginEnabled("No"),
        m_RemoteLoginTimeout(-1),
        m_myController(pController),
        m_LoginTimer(this)
{
    this->setParent(pParent);
}

/****************************************************************************/
/*!
* \brief Destructor.
*
*
*****************************************************************************/
ExternalProcessDevice::~ExternalProcessDevice()
{
    m_myController = NULL;
}

/****************************************************************************/
/*!
 *  \brief    API function to call before ExternalProcess Device can be used
 *
 *      This function does everything to initialize the ExternalProcess Device
 *      with default parameters.
 *      It shall be called before ExternalProcess Device can be used.
 *
 *  \return   true if success, false if error
 *
 ****************************************************************************/
bool ExternalProcessDevice::InitializeDevice()
{
    return InitializeDeviceWithParameters((QString)"No", (int)0);
}

/****************************************************************************/
/*!
 *  \brief    API function to call before ExternalProcess Device can be used
 *
 *      This function does everything to initialize the ExternalProcess Device.
 *      It shall be called before ExternalProcess Device can be used.
 *
 *  \param  remoteLoginEnabled = "Yes" if device waits for connection from outside,
 *                                "No" if device starts its external process itself.
 *  \param  remoteLoginTimeout =  Timeout for login process, in seconds.
 *                                Zero means infinite wait.
 *
 *  \return   true if success, false if error
 *
 ****************************************************************************/
bool ExternalProcessDevice::InitializeDeviceWithParameters(const QString &remoteLoginEnabled, int remoteLoginTimeout)
{
    qDebug() << "ExternalProcessDevice: initializing ExternalProcess...";

    if (m_myController == NULL) {
        qDebug() << "ExternalProcessDevice: Device's Controller is NULL !";
        return false;
    }

    if (!NetworkServerDevice::InitializeDevice()) {
        qDebug() << "ExternalProcessDevice: cannot initialize Network Server Device.";
        return false;
    }

    m_RemoteLoginEnabled = remoteLoginEnabled;
    m_RemoteLoginTimeout = remoteLoginTimeout;

    // guard the login timeout: if ExternalProcess fails to connect before timeout - not good. :)
    try {
        CONNECTSIGNALSLOT(&m_LoginTimer, timeout(), this, LoginTimedout());
    } catch (...) {
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  This method processes an incomig from peer device message.
 *
 *  \param[in]  msgname - name of the message
 *  \param[in]  ref - command reference
 *  \param[in]  bArray - message payload
 *
 *  \return true if command executed successfully, false otherwise
 */
/****************************************************************************/
bool ExternalProcessDevice::ProcessIncomingMessage(const QString &msgname, const QString &ref, const QByteArray &bArray)
{
    qDebug() << "ExternalProcessDevice: " << msgname << " reference: " << ref << " received !\n";
    bool result = false;
    QString status = NetworkBase::CMH_MSG_SENDING_NACK;

    if (msgname.isEmpty() || ref.isEmpty()) {
        /// \todo log error
        qDebug() << "\nExternalProcessDevice: oops. missing input !\n";
        static_cast<void> (NetworkBase::Ack::SendAcknowledge(this, ref, msgname, status));
        return result;
    }
    if (m_myController != NULL) {
        if (m_myController->ForwardMsgToRecepient(bArray)) {
            result = true;
            status = NetworkBase::CMH_MSG_SENDING_ACK;
        }
    }
    static_cast<void> (NetworkBase::Ack::SendAcknowledge(this, ref, msgname, status));
    return result;
}

/****************************************************************************/
/*!
 *  \brief This slot handles errors reported by the TCPIP Server.
 *
 *  \param[in]  dtype  = the server error
 *  \param[in]  client = client name
 */
/****************************************************************************/
void ExternalProcessDevice::HandleNetworkError(NetworkBase::DisconnectType_t dtype, const QString &client)
{
    /// \todo do we make ExternalProcessDevice a logging object? or do we forward
    // errors further up? who reports them?
    NetworkServerDevice::HandleNetworkError(dtype, client);
}

/****************************************************************************/
/*!
 *  \brief  This method starts ExternalProcess Device operation.
 *
 *          This method shall be called after the successful
 *          InitializeExternalProcessDevice call.
 *
 *  \return  true if success, false otherwise
 */
/****************************************************************************/
bool ExternalProcessDevice::StartDevice()
{
    // start server
    if (!NetworkServerDevice::StartDevice()) {
        return false;
    }
    // start login timeout timer
    StartLoginGuard();

    return true;
}

/****************************************************************************/
/*!
 *  \brief Destroy ExternalProcess Device.
 *
 *        Device is unusable after calling this function.
 *        InitializeDevice shall be called to be able to use the device again.
 */
/****************************************************************************/
void ExternalProcessDevice::DestroyDevice()
{
    static_cast<void> (
    // cannot use return value here anyway
    this->disconnect()
    );
    StopLoginGuard();
    NetworkServerDevice::DestroyDevice();
}

/****************************************************************************/
/*!
 *  \brief  This method processes peer connection event.
 *
 *          All periodic ExternalProcess activities shall be started till peer
 *          disconnects again.
 *
 *  \param[in]  name = the name of the connected process
 */
/****************************************************************************/
void ExternalProcessDevice::PeerConnected(const QString &name)
{
    qDebug() << "ExternalProcessDevice: HeartBeat Timer started.";
    // stop timeout guard:
    StopLoginGuard();
    // inform whoever is interested in this event
    NetworkDevice::PeerConnected(name);
}

/****************************************************************************/
/*!
 *  \brief  This method processes peer disconnection event.
 *
 *          All periodic ExternalProcess activities shall be stopped till peer
 *          connects again.
 *
 *  \param[in]  name = the name of the disconnected process
 */
/****************************************************************************/
void ExternalProcessDevice::PeerDisconnected(const QString &name)
{
    // stop periodic DateAndTime sync:
    qDebug() << "ExternalProcessDevice: HeartBeat Timer stopped.";
    // inform whoever is interested in this event
    NetworkServerDevice::PeerDisconnected(name);
}

/****************************************************************************/
/*!
 *  \brief    This function disconnects Himalaya from ExternalProcess
 *
 *
 ****************************************************************************/
void ExternalProcessDevice::DisconnectPeer()
{
    // stop the login timer
    StopLoginGuard();
    NetworkServerDevice::DisconnectPeer();
}

/****************************************************************************/
/*!
 *  \brief  This slot shall be called when ExternalProcess process is started.
 *
 *          Internal timer will be started to guard the login timeout.
 */
/****************************************************************************/
void ExternalProcessDevice::StartLoginGuard()
{
    if (m_RemoteLoginEnabled == "No") {
        m_LoginTimer.start(EP_LOGIN_TIMEOUT);
    }
    else {
        if (m_RemoteLoginTimeout > 0) {
            // m_RemoteLoginTimeout is in seconds: convert it to milliseconds
            m_LoginTimer.start(m_RemoteLoginTimeout*1000);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  This slot shall be called to stop the powerup guard timer.
 *
 */
/****************************************************************************/
void ExternalProcessDevice::StopLoginGuard()
{
    m_LoginTimer.stop();
}

/****************************************************************************/
/*!
 *  \brief  This slot is called when ExternalProcess fails to login at powerup/restart.
 *
 */
/****************************************************************************/
void ExternalProcessDevice::LoginTimedout()
{
    /// \todo what if ExternalProcess suddenly connects after timeout?
    // shall we shut down the server here before emitting the signal?

    // stop the login timer
    StopLoginGuard();
    qDebug() << "ExternalProcessDevice: ExternalProcess login timedout !";
    emit SigLoginTimeout();
}

} // end namespace ExternalProcessControl

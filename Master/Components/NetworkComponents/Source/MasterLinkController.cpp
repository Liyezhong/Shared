/****************************************************************************/
/*! \file MasterLinkController.cpp
 *
 *  \brief MasterLinkController implementation.
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

#include <NetworkComponents/Include/MasterLinkController.h>
#include <Global/Include/Utils.h>
#include <NetworkComponents/Include/NetworkComponentEventCodes.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief    Constructor
 *
 *  \iparam    ip = server's ip
 *  \iparam    port = server's port
 *  \iparam    clientType
 *
 ****************************************************************************/
MasterLinkController::MasterLinkController(const QString &ip, const QString &port, const NetworkBase::NetworkClientType_t clientType) :
        m_myNetClient(NULL),
        m_IP(ip),
        m_Port(port),
        m_clientType(clientType)
{
}

/****************************************************************************/
/*!
 *  \brief    Destructor
 *
 ****************************************************************************/
MasterLinkController::~MasterLinkController()
{
    try {
        if (m_myNetClient != NULL) {
            delete m_myNetClient;
            m_myNetClient = NULL;
        }
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
/*!
 *  \brief   Initializes network layer for communication with Master
 *
 *  \return   true if init successful, false otherwise
 *
 ****************************************************************************/
bool MasterLinkController::Initialize()
{
    try {
        // parent shall be NULL if NetworkClientDevice need to run in a diferent thread!
        m_myNetClient = new NetworkBase::NetworkClientDevice(m_clientType, m_IP, m_Port, "", NULL);
    }
    CATCHALL_RETURN(false)

    if (!m_myNetClient->InitializeDevice()) {
        qDebug() << "MasterLinkController: Cannot initialize MasterClient !\n";
        m_myNetClient->deleteLater();
        Global::EventObject::Instance().RaiseEvent(EVENT_MLC_INIT_FAILED,
                                                   Global::tTranslatableStringList() << m_IP << FILE_LINE);
        return false;
    }

    // ========= Connect all internal SIGNAL/SLOT pairs: ===

    // the Network Layer will start as soon as thread emits "started" signal
    if(!QObject::connect(&m_NetworkThread, SIGNAL(started()), m_myNetClient, SLOT(StartDevice()))) {
        qDebug() << "MasterLinkController: cannot connect LinkDevice's StartDevice slot !\n";
        Global::EventObject::Instance().RaiseEvent(EVENT_MLC_CONNECT_FAILED,
                                                   Global::tTranslatableStringList() << m_IP << "started");
        return false;
    }
    // Signal which delivers an incoming from Master message
    if (!QObject::connect(m_myNetClient, SIGNAL(ForwardMessageToUpperLayer(const QString &, const QByteArray &)),
                          this, SLOT(ForwardMsgToRecipient(const QString &, const QByteArray &)))) {
        qDebug() << "MasterLinkController: Cannot connect ACK signal-slot pair !\n";
        Global::EventObject::Instance().RaiseEvent(EVENT_MLC_CONNECT_FAILED,
                                                   Global::tTranslatableStringList() << m_IP << "ForwardMessageToUpperLayer");
        return false;
    }
    // Signal which sends message to Master
    if (!QObject::connect(this, SIGNAL(SigSendMessageToMaster(const QString &, const QByteArray &, Global::tRefType)),
                          m_myNetClient, SLOT(SendOutgoingCommand(const QString &, const QByteArray &, Global::tRefType)))) {
        qDebug() << "MasterLinkController: Cannot connect message sending signal-slot pair !\n";
        Global::EventObject::Instance().RaiseEvent(EVENT_MLC_CONNECT_FAILED,
                                                   Global::tTranslatableStringList() << m_IP << "SigSendMessageToMaster");
        return false;
    }

    // ========= Connect all SIGNAL/SLOT pairs: ===
    // Signal/slot for DateAndTime reporting
    if (!connect(m_myNetClient, SIGNAL(SigDateAndTime(QDateTime)), this, SIGNAL(SigDateAndTime(QDateTime)))) {
        qDebug() << "MasterLinkController: cannot connect 'DateAndTime' signal";
        Global::EventObject::Instance().RaiseEvent(EVENT_MLC_CONNECT_FAILED,
                                                   Global::tTranslatableStringList() << m_IP << "SigDateAndTime");
        return false;
    }
    // Signal indicating a valid connection to Master
    if (!QObject::connect(m_myNetClient, SIGNAL(SigPeerConnected(const QString &)),
                          this, SIGNAL(SigMasterConnected(const QString &)))) {
        qDebug() << "MasterLinkController: Cannot connect LinkDevice's ConnectedToMaster signal !\n";
        Global::EventObject::Instance().RaiseEvent(EVENT_MLC_CONNECT_FAILED,
                                                   Global::tTranslatableStringList() << m_IP << "SigPeerConnected");
        return false;
    }
    // Signal indicating a disconnection from Master
    if (!QObject::connect(m_myNetClient, SIGNAL(SigPeerDisconnected(const QString &)),
                          this, SIGNAL(SigMasterDisconnected(const QString &)))) {
        qDebug() << "MasterLinkController: Cannot connect LinkDevice's DisconnectedFromMaster signal !\n";
        Global::EventObject::Instance().RaiseEvent(EVENT_MLC_CONNECT_FAILED,
                                                   Global::tTranslatableStringList() << m_IP << "SigPeerDisconnected");
        return false;
    }
    // Signal for forwarding stuff from Master
    if (!QObject::connect(m_myNetClient, SIGNAL(StartConnectionLostTimer()),
                          this, SIGNAL(StartConnectionLostTimer()))) {
        Global::EventObject::Instance().RaiseEvent(EVENT_MLC_CONNECT_FAILED,
                                                   Global::tTranslatableStringList() << m_IP << "StartConnectionLostTimer");
        return false;
    }

    // initialization done, move client to network thread
    m_myNetClient->moveToThread(&m_NetworkThread);

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Starts network layer after successful initialization.
 *
 *
 ****************************************************************************/
void MasterLinkController::Go()
{
    if (m_myNetClient == NULL) {
        return;
    }
    // start the network thread
    m_NetworkThread.start();
}

/****************************************************************************/
/*!
 *  \brief   Stop network layer.
 *
 *
 ****************************************************************************/
void MasterLinkController::Stop()
{
    // stop the network thread
    m_NetworkThread.quit();
    m_NetworkThread.wait();
}

/****************************************************************************/
/*!
 *  \brief    This function forwards message to the NetLayer
 *
 *  \iparam     msgname = name of the message
 *  \iparam     bArr = payload
 *  \iparam     Ref = application command reference for tracking
 *
 *  \return   true if message forwarded successfully, false otherwise
 *
 ****************************************************************************/
bool MasterLinkController::SendMessageToMaster(const QString &msgname, const QByteArray &bArr, Global::tRefType Ref)
{
    qDebug() << "MasterLinkController: sending message : " << msgname;

    // check signal connection
    if (receivers(SIGNAL(SigSendMessageToMaster(const QString &, const QByteArray &, Global::tRefType))) == 0) {
        qDebug() << "MasterLinkController: message emitting signal is not connected !";
        Global::EventObject::Instance().RaiseEvent(EVENT_MLC_SIGNAL_NOT_CONNECTED, Global::tTranslatableStringList() << m_IP);
        return false;
    }

    emit SigSendMessageToMaster(msgname, bArr, Ref);
    return true;
}

/****************************************************************************/
/*!
 *  \brief    \todo comment and implement
 *
 *  \iparam     msgname = name of the message
 *  \iparam     barray = payload
 */
/****************************************************************************/
void MasterLinkController::ForwardMsgToRecipient(const QString &msgname, const QByteArray &barray)
{
//    DEBUGWHEREAMI;
    emit SigForwardMsgToRecipient(msgname, barray);
}

} //namespace

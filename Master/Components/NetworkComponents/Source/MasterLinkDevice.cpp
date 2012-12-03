/****************************************************************************/
/*! \file MasterLinkDevice.cpp
 *
 *  \brief MasterLinkDevice implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-21
 *   $Author:  $ M.Scherer
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

#include "NetworkComponents/Include/MasterLinkDevice.h"
#include "Global/Include/Commands/CmdDateAndTime.h"

#include <QMetaType>

namespace NetworkBase {

const CreatorFunctorShPtr_t NullCreatorFunctor(NULL);   ///< NULL functor.

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CMasterLinkDevice::CMasterLinkDevice(QString ip, QString port, const NetworkBase::NetworkClientType_t clientType)
    : QObject()
{
    // register some metatypes
    qRegisterMetaType<Global::tRefType>("Global::tRefType");

    // Network Layer
    mp_NetworkObject = new MasterLinkController(ip, port, clientType);
    // register network acknowledges
    RegisterNetMessage<Global::AckOKNOK, CMasterLinkDevice>(&CMasterLinkDevice::OnAckOKNOK, this);
    /// \todo register other acknowledges
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CMasterLinkDevice::~CMasterLinkDevice()
{
    try {
        // Network Components
        mp_NetworkObject->Stop();
        delete mp_NetworkObject;
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief Initializes the network layers
 */
/****************************************************************************/
qint32 CMasterLinkDevice::NetworkInit()
{
    if (!mp_NetworkObject) {
        return -1;
    }
    if (!mp_NetworkObject->Initialize()) {
        return -1;
    }

    // Signal/slot for DateAndTime reporting
    if (!connect(mp_NetworkObject, SIGNAL(SigDateAndTime(QDateTime)), this, SIGNAL(SigDateAndTime(QDateTime)))) {
        qDebug() << "CMasterLinkDevice: cannot connect 'DateAndTime' signal";
        return -1;
    }
    // Signal indicating a valid connection to Master
    if (!QObject::connect(mp_NetworkObject, SIGNAL(SigMasterConnected(const QString &)),
                          this, SIGNAL(SigMasterConnected(const QString &)))) {
        qDebug() << "CMasterLinkDevice: Cannot connect LinkDevice's ConnectedToMaster signal !\n";
        return -1;
    }
    // Signal indicating a disconnection from Master
    if (!QObject::connect(mp_NetworkObject, SIGNAL(SigMasterDisconnected(const QString &)),
                          this, SIGNAL(SigMasterDisconnected(const QString &)))) {
        qDebug() << "CMasterLinkDevice: Cannot connect LinkDevice's DisconnectedFromMaster signal !\n";
        return -1;
    }
    // Signal for forwarding stuff from Master
    if (!QObject::connect(mp_NetworkObject, SIGNAL(SigForwardMsgToRecipient(QString,QByteArray)),
                          this, SLOT(ProcessNetMessage(QString,QByteArray)))) {
        qDebug() << "CMasterLinkDevice: Cannot connect LinkDevice's SigForwardMsgToRecipient signal !\n";
        return -1;
    }
    mp_NetworkObject->Go();
    return 0;
}

/****************************************************************************/
/*!
 *  \brief Deserialize net message and call appropriate processing function.
 *
 * Deserialize net message (command and acknowledge) and call appropriate processing function.
 *
 * \param[in]   msgname     SERIALIZERSTRING for command or acknowledge.
 * \param[in]   barray      Serialized message.
 */
/****************************************************************************/
void CMasterLinkDevice::ProcessNetMessage(const QString &msgname, const QByteArray &barray) {
    DEBUGWHEREAMI;
    if((msgname != Global::Command::SERIALIZERSTRING) &&
       (msgname != Global::Acknowledge::SERIALIZERSTRING))
    {
        /// \todo implement
        qDebug() << "This is an unknown message type";
        return;
    }

    // deserialize data
    // create data stream
    QDataStream DS(barray);
    // set version
    DS.setVersion((int)QDataStream::Qt_4_0);
    // read class name
    QString ClassName;
    DS >> ClassName;
    // read Ref
    Global::tRefType Ref = 0;
    DS >> Ref;
    qDebug() << ClassName << " " << Ref;
    // unblock reference if acknowledge
    if(msgname == Global::Acknowledge::SERIALIZERSTRING) {
        // mark reference as reusable
        m_RefManager.UnblockReference(Ref);
    }
    // OK, now get functor and execute
    CreatorFunctorShPtr_t Functor = GetCreatorFunctor(ClassName);
    if(Functor == NullCreatorFunctor) {
        /// \todo throw exception
        qDebug() << "No creator functor found for " << ClassName;
        return;
    }
    // if acknwoledge: remove from list of pending commands
    if(msgname == Global::Acknowledge::SERIALIZERSTRING) {
        // remove from list of pending commands
        RemoveFromPendingCommands(Ref);
    }
    // execute
    Functor.GetPointerToUserData()->CreateAndDeserialize(Ref, DS);
    // remove from list of receivers
    m_AckAndTOReceivers.remove(Ref);
}

/****************************************************************************/
/*!
 *  \brief Register a creator functor.
 *
 * Register a creator functor.
 *
 * \param[in]   Name        Name for functor.
 * \param[in]   Functor     The functor to register.
 */
/****************************************************************************/
void CMasterLinkDevice::RegisterCreatorFunctor(const QString &Name, const CreatorFunctorShPtr_t &Functor) {
    // check if already registered
    if(m_CreatorFunctors.contains(Name)) {
        /// \todo throw exception
        qDebug() << "Creator functor for " << Name << " already registered";
        return;
    }
    // everything OK
    static_cast<void>(
        // we DO NOT NEED the return value of insert
        m_CreatorFunctors.insert(Name, Functor)
    );
}

/****************************************************************************/
/*!
 *  \brief Get registered creator functor.
 *
 * Get registered creator functor for name CmdName. return NullCreatorFunctor
 * if no functor registered for this name.
 *
 * \param[in]   CmdName     Rame of registred functor.
 * \return                  Registered functor or NullCreatorFunctor if not found.
 */
/****************************************************************************/
CreatorFunctorShPtr_t CMasterLinkDevice::GetCreatorFunctor(const QString &CmdName) const {
    CreatorFunctorHash_t::const_iterator it = m_CreatorFunctors.find(CmdName);
    if(it == m_CreatorFunctors.constEnd()) {
        // functor not found
        // return NULL functor
        return NullCreatorFunctor;
    }
    // return functor
    return it.value();
}

/****************************************************************************/
/*!
 *  \brief Slot called if a command sent to master times out.
 *
 * Slot called if a command sent to master times out. It deletes the according
 * pending command descriptor and calls the timeout processing method.
 *
 * \param[in]   Ref             Reference of command which timed out.
 * \param[in]   CommandName     Name of command which timed out.
 */
/****************************************************************************/
void CMasterLinkDevice::OnCommandTimeoutSlot(Global::tRefType Ref, QString CommandName) {
    DEBUGWHEREAMI;
    qDebug() << "Ref = " << Ref << " Name = " << CommandName;
    // check if in
    if(m_PendingCommands.contains(Ref)) {
        // remove from list of pending commands
        RemoveFromPendingCommands(Ref);
        // get timeout receiver
        CreatorFunctorShPtr_t Rcv = m_AckAndTOReceivers.value(Ref, NULL);
        CommandCreatorFunctor<Global::AckOKNOK> *Command =
                dynamic_cast<CommandCreatorFunctor<Global::AckOKNOK> *>(Rcv.GetPointerToUserData());
        // a receiver must still be there!
        if(Rcv == NullCreatorFunctor) {
            /// \todo throw exception
            qDebug() << "No creator functor found for " << CommandName;
            return;
        }
        // execute
        Command->CopyAndCall(Ref, Global::AckOKNOK(false));
        /// \todo still emit this signal?
        emit SigCmdTimeout(Ref, CommandName);
    } else {
        // unknown command reference. maybe already processed?.
        /// \todo what now? debug message? log error?
        qDebug() << "Error unknown timeout received: Ref = " << Ref << " Name = " << CommandName;
    }
}

/****************************************************************************/
/*!
 *  \brief An acknowledge of type Global::AckOKNOK was received.
 *
 * An acknowledge of type Global::AckOKNOK was received.
 *
 * \param[in]   Ref     Acknowledge reference.
 * \param[in]   Ack     The acknowledge.
 */
/****************************************************************************/
void CMasterLinkDevice::OnAckOKNOK(Global::tRefType Ref, const Global::AckOKNOK &Ack) {
    DEBUGWHEREAMI;
    qDebug() << "Ref = " << Ref << " Ack = " << Ack.GetName();
    // get acknowledge receiver
    CreatorFunctorShPtr_t Rcv = m_AckAndTOReceivers.value(Ref, NULL);
    CommandCreatorFunctor<Global::AckOKNOK> *Command =
            dynamic_cast<CommandCreatorFunctor<Global::AckOKNOK> *>(Rcv.GetPointerToUserData());
    // a receiver must still be there!
    if(Rcv == NullCreatorFunctor) {
        /// \todo throw exception
        qDebug() << "No creator functor found for " << Ack.GetName();
        return;
    }
    // execute
    Command->CopyAndCall(Ref, Ack);
}

} // end namespace NetLayer

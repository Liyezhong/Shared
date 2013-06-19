/****************************************************************************/
/*! \file ProtocolTxCommand.cpp
 *
 *  \brief ProtocolTxCommand base class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 25.03.2011
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

#include <NetworkComponents/Include/NetworkDevice.h>
#include <NetworkComponents/Include/ProtocolTxCommand.h>
#include <NetworkComponents/Include/ProtocolTxCommands/HeartBeat.h>
#include <NetworkComponents/Include/ProtocolTxCommands/DateAndTime.h>
#include <Global/Include/TranslatableString.h>
#include <Global/Include/GlobalEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

namespace NetworkBase {

/// Static registration for command HeartBeat:
DEFINE_REG_CLASS(ProtocolTxCommand, HeartBeat);
/// Static registration for command DateAndTime:
DEFINE_REG_CLASS(ProtocolTxCommand, DateAndTime);

/****************************************************************************/
/*!
 * \brief Constructor.
 *
 */
/****************************************************************************/
ProtocolTxCommand::ProtocolTxCommand() :
        ProtocolCommand::ProtocolCommand(),
        m_myRef(0),
        m_myTimer(this),
        m_myAppRef(0)

{
    m_myPayloadArray.clear();
}


/****************************************************************************/
/*!
 * \brief Destructor.
 *
 */
/****************************************************************************/
ProtocolTxCommand::~ProtocolTxCommand()
{
    m_myDevice = NULL;
}

/****************************************************************************/
/*!
 * \brief This method must be called prior to first use of the class object.
 *
 *        Calling this method ensures that the linker will create all needed
 *        static members. Otherwise it will not. Seem to be a linker bug.
 *
 */
/****************************************************************************/
void ProtocolTxCommand::LinkerTamer()
{
     qDebug() << "ProtocolTxCommand: LinkerTamer called !";
}

/****************************************************************************/
/*!
 *  \brief   Call this function before using the Outgoing command
 *
 *           This function will prepare command for execution.
 *
 *  \warning Implement this function in the derived class!
 *
 *  \param   name = command name
 *  \param   ref  = unique command reference
 *  \param   wObj = pointer to command's working object
 *
 *  \return  TRUE if initialization succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool ProtocolTxCommand::Initialize(const QString &name, quint32 ref, QObject *wObj)
{
    if (wObj == NULL) {
        return false;
    }
    m_myName = name;
    m_myRef = ref;
    m_myDevice = (NetworkDevice*)wObj;

    try {
        CONNECTSIGNALSLOT(&m_myTimer, timeout(), this, HandleAckTimeout());
    } catch (...) {
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief   Call this function before using the Outgoing command
 *
 *           This function will prepare command for execution.
 *
 *  \warning Implement this function in the derived class!
 *
 *  \param   name = command name
 *  \param   ref  = unique command reference
 *  \param   appRef  = application command reference
 *  \param   bArr = payload as byte array
 *  \param   wObj = pointer to command's working object
 *
 *  \return  TRUE if initialization succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool ProtocolTxCommand::Initialize(const QString &name, quint32 ref, Global::tRefType appRef, const QByteArray &bArr, QObject *wObj)
{
    m_myAppRef = appRef;
    m_myPayloadArray = bArr;
    return Initialize(name, ref, wObj);
}

/****************************************************************************/
/*!
 *  \brief   This function handles acknowledge of this command
 *
 *  \param   status = if remote command was executed successfully or not
 *
 *  \warning always call this function in the derived class's function. it
 *           will take care of deregistering the command and stopping
 *           the ack timeout timer.
 *
 ****************************************************************************/
void ProtocolTxCommand::HandleAck(const QString &status)
{
    // ack received before timeout: all OK, destroy the command.
    m_myTimer.stop();
    if (m_myDevice != NULL) {
        // tell Device to deregister and destroy the command:
        m_myDevice->DeregisterRunningCommand(m_myRef, this);
        m_myDevice->MessageSendingResult(m_myAppRef, status);
    }
}

/****************************************************************************/
/*!
 *  \brief   This function stops and destroys the waiting command
 *
 *
 ****************************************************************************/
void ProtocolTxCommand::DismissCommand()
{
    // stop ack timer
    m_myTimer.stop();
    if (m_myDevice != NULL) {
        // tell Device to deregister and destroy the command:
        m_myDevice->DeregisterRunningCommand(m_myRef, this);
    }
}

/****************************************************************************/
/*!
 *  \brief   This function handles command's ack timeout
 *
 *
 ****************************************************************************/
void ProtocolTxCommand::HandleAckTimeout()
{
    // we arrived here because of the timeout: report error!
    qDebug() << "ProtocolTxCommand: Ack timedout !";
    if (m_myDevice != NULL) {
        // tell Device to deregister and destroy the command:
        m_myDevice->DeregisterRunningCommand(m_myRef, this);
        m_myDevice->MessageSendingResult(m_myAppRef, NetworkBase::CMH_MSG_SENDING_TIMEOUT);
    }
}

/****************************************************************************/
/*!
 *  \brief   Get function for command's reference
 *
 *  \return  actual command reference as QString
 *
 ****************************************************************************/
QString ProtocolTxCommand::GetReference()
{
    QString sref = QString::number(m_myRef, 10);
    return sref;
}

/****************************************************************************/
/*!
 *  \brief   Call this function to execute the command
 *
 *  \param   message = the message to send
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool ProtocolTxCommand::ExecuteWithParameter(const QString &message)
{
    // register command with Device:
    m_myDevice->RegisterRunningCommand(m_myRef, this);
    // send message to Recipient:
    bool result = m_myDevice->SendCommand(message);
    // set timer for Ack timeout!
    m_myTimer.start(NetworkBase::ACK_TIMEOUT);

    return result;
}

/****************************************************************************/
/*!
 *  \brief   Call this function to execute the command
 *
 *  \param   type = type of command
 *  \param   ba = outgoing message as byte array
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool ProtocolTxCommand::ExecuteWithParameters(NetMessageType_t type, const QByteArray &ba)
{
    // register command with Device:
    m_myDevice->RegisterRunningCommand(m_myRef, this);
    // send message to Recipient:
    bool result = m_myDevice->SendCommand(type, ba);
    // set timer for Ack timeout!
    m_myTimer.start(NetworkBase::ACK_TIMEOUT);

    return result;
}

/****************************************************************************/
/*!
 *  \brief   Call this function to execute the command
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool ProtocolTxCommand::Execute()
{
    // register command with Device:
    m_myDevice->RegisterRunningCommand(m_myRef, this);
    // construct the message:
    QString msg = "<message><cmd name=\"" + m_myName + "\" ref=\"" + QString::number(m_myRef, 10) + "\" /></message>";
    // prepend text header to the payload:
    m_myPayloadArray.prepend(msg.toUtf8());
    // send message to Recipient:
    bool result = m_myDevice->SendCommand(NetworkBase::NET_APPLICATION_MESSAGE, m_myPayloadArray);
    // set timer for Ack timeout!
    m_myTimer.start(NetworkBase::ACK_TIMEOUT);

    return result;
}

} // end namespace NetworkBase

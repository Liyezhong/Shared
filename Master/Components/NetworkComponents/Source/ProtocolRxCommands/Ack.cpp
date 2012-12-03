/****************************************************************************/
/*! \file Ack.cpp
 *
 *  \brief Acknowledge command implementation.
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

#include <NetworkComponents/Include/ProtocolRxCommands/Ack.h>
#include <NetworkComponents/Include/ProtocolTxCommand.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief   Constructor for Ack class
 *
 ****************************************************************************/
Ack::Ack() :
    ProtocolRxCommand::ProtocolRxCommand()
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor for Ack class
 *
 ****************************************************************************/
Ack::~Ack()
{
}

/****************************************************************************/
/*!
 *  \brief   Send acknowledge
 *
 *  \param[in]  pNd = pointer to the NetworkDevice
 *  \param[in]  Ref = protocol message reference
 *  \param[in]  CmdName = name of acknowledged command
 *  \param[in]  Status = status of command execution
 *
 *  \return  true if ack was sent, false otherwise
 *
 ****************************************************************************/
bool Ack::SendAcknowledge(NetworkDevice *pNd, const QString &Ref, const QString &CmdName, const QString &Status)
{
    if ((pNd == NULL) || Ref.isEmpty() || CmdName.isEmpty() ) {
        return false;
    }
    // Construct Ack:
    QString msg = "<message><cmd name=\"Ack\" ref=\"" + Ref +
                  "\" /><dataitems cmd=\"" + CmdName + "\" status=\"" + Status + "\" /></message>";

//    qDebug() << "Outgoing Ack::SendAcknowledge :  " << msg << "\n";

    return pNd->SendCommand(msg);
}

/****************************************************************************/
/*!
 *  \brief   Execute incomig Ack command
 *
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool Ack::Execute()
{
    qDebug() << (QString)"Ack: Execute Incoming called !";

    if (m_myDevice == NULL) {
        qDebug() << (QString)"Ack: ERROR -> m_myDevice = NULL !";
        return false;
    }

//    QString inack(m_myMessage.toByteArray(1));
//    qDebug() << "Incoming Ack :  " << inack << "\n";

    QDomElement emt = m_myMessage.documentElement();
    // fetch acked command name and reference:
    QString cmd = (emt.firstChildElement("dataitems")).attribute("cmd", "NULL");
    if ((cmd.isEmpty()) || (cmd == "NULL")) {
        // ack is not complete - do not know what to do with it
        qDebug() << (QString)"Ack: ERROR -> cmd or ref is empty !";
        return false;
    }

    // fetch the command we are supposed to ack:
    ProtocolTxCommand*  scmd = m_myDevice->FetchRunningCommand(m_myRef.toULong());
    if (scmd == NULL) {
        // no such command running - do not know what to do with it
        qDebug() << (QString)"Ack: ERROR -> cannot fetch running command !";
        return false;
    }

    // check if this is a correct command:
    if (cmd != scmd->GetName()) {
        // cmd name does not correspond to reference - do not know what to do with it
        qDebug() << (QString)"Ack: ERROR -> ackCmdName (" << cmd << ") != fetchedCmdName (" << scmd->GetName() << ") !";
        return false;
    }

    QString status = (emt.firstChildElement("dataitems")).attribute("status", "NULL");
    if ((status.isEmpty()) || (status == "NULL")) {
        // ack is not complete - do not know what to do with it
        qDebug() << (QString)"Ack: ERROR -> status empty !";
        return false;
    }

    // all OK, call corresponding Ack function:
    scmd->HandleAck(status);

    return true;
}

} // end namespace NetworkBase

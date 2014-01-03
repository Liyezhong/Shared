/****************************************************************************/
/*! \file HeartBeatClient.cpp
 *
 *  \brief HeartBeatClient command implementation.
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

#include <NetworkComponents/Include/ProtocolRxCommands/HeartBeatClient.h>
#include <NetworkComponents/Include/ProtocolTxCommands/HeartBeat.h>
#include <NetworkComponents/Include/ProtocolTxCommand.h>
#include <Global/Include/Utils.h>
#include <NetworkComponents/Include/NetworkComponentEventCodes.h>
#include <Global/Include/EventObject.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief   Constructor for HeartBeatClient class
 *
 ****************************************************************************/
HeartBeatClient::HeartBeatClient() :
                ProtocolRxCommand::ProtocolRxCommand()
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor for HeartBeatClient class
 *
 ****************************************************************************/
HeartBeatClient::~HeartBeatClient()
{
}

/****************************************************************************/
/*!
 *  \brief   Call this function to execute the command
 *
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool HeartBeatClient::Execute()
{
//    qDebug() << (QString)"HeartBeatClient: Server RECEIVER called !";

    if (m_myDevice == NULL) {
        Global::EventObject::Instance().RaiseEvent(EVENT_NL_NULL_POINTER,
                                                   Global::tTranslatableStringList() << "" << FILE_LINE);
        return false;
    }

    QDomElement emt = m_myMessage.documentElement();
    // fetch acked command name:
    QString nr = (emt.firstChildElement("dataitems")).attribute("nr", "NULL");
    if ((nr.isEmpty()) || (nr == "NULL")) {
        // command is not complete - do not know what to do with it
        Global::EventObject::Instance().RaiseEvent(EVENT_NL_COMMAND_NOT_COMPLETE,
                                                   Global::tTranslatableStringList() << nr << FILE_LINE);
        return false;
    }

    // fetch the command we are supposed to ack:
    ProtocolTxCommand*  scmd = m_myDevice->FetchRunningCommand(m_myRef.toULong());
    if (scmd == NULL) {
        // no such command running - do not know what to do with it
        Global::EventObject::Instance().RaiseEvent(EVENT_NL_COMMAND_NOT_RUNNING,
                                                   Global::tTranslatableStringList() << m_myRef << FILE_LINE);
        return false;
    }

    // check if this is a correct command:
    if ("HeartBeat" != scmd->GetName()) {
        // cmd name does not correspond to reference - do not know what to do with it
        Global::EventObject::Instance().RaiseEvent(EVENT_NL_COMMAND_INVALID_REFERENCE,
                                                   Global::tTranslatableStringList() << "HeartBeat" << FILE_LINE);
        return false;
    }

    QString status = NetworkBase::CMH_MSG_SENDING_OK;
    bool ok;
    quint16 receivedNumber = nr.toUShort(&ok, 10);
    quint16 sentNumber = ((HeartBeat*)scmd)->GetSentNumber();
    if (receivedNumber != (sentNumber+1)) {
        // HeartBeatClient number does not correspond to the (sent one + 1) - set Ack status to negative.
        QString mynr = QString::number((sentNumber+1), 10);
        qDebug() << (QString)("HeartBeatClient: wrong HB number received!\nExpected = " + mynr + "\nReceived = " + nr + "\n");
        status = NetworkBase::CMH_MSG_SENDING_NACK;
    }

    // call corresponding Ack function:
    scmd->HandleAck(status);

    return true;
}

} // end namespace NetworkBase

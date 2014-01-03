/****************************************************************************/
/*! \file HeartBeatServer.cpp
 *
 *  \brief HeartBeatServer command implementation.
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

#include <NetworkComponents/Include/ProtocolRxCommands/HeartBeatServer.h>
#include <Global/Include/Utils.h>
#include <NetworkComponents/Include/NetworkComponentEventCodes.h>
#include <Global/Include/EventObject.h>

namespace NetworkBase {


/****************************************************************************/
/*!
 *  \brief   Constructor for HeartBeatServer class
 *
 ****************************************************************************/
HeartBeatServer::HeartBeatServer() :
                ProtocolRxCommand::ProtocolRxCommand()
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor for HeartBeatServer class
 *
 ****************************************************************************/
HeartBeatServer::~HeartBeatServer()
{
    //qDebug() << (QString)("HeartBeatServer: incoming command destroyed.");
}

/****************************************************************************/
/*!
 *  \brief   Call this function to execute the command
 *
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool HeartBeatServer::Execute()
{
    qDebug() << (QString)"HeartBeatServer: Client RECEIVER called !";

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

    m_myDevice->RestartHeartBeatTimer();

    bool ok;
    quint16 newNumber = nr.toUShort(&ok, 10) + 1;
    // calculate HeartBeat number for reply :
    nr = QString::number(newNumber, 10);

    // construct message
    QString cmd = "<message><cmd name=\"HeartBeatClient\" ref=\"" + m_myRef +
                                    "\" /><dataitems nr=\"" + nr + "\" /></message>";
    // send message to Sepia:
    return m_myDevice->SendCommand(cmd);
}

} // end namespace NetworkBase

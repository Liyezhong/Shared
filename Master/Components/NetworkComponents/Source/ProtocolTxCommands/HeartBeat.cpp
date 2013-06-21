/****************************************************************************/
/*! \file HeartBeat.cpp
 *
 *  \brief HeartBeat command implementation.
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

#include <NetworkComponents/Include/ProtocolTxCommands/HeartBeat.h>

namespace NetworkBase {

quint16 HeartBeat::m_myHeartBeatNumber = 0;

/****************************************************************************/
/*!
 *  \brief   Constructor for HeartBeat class
 *
 ****************************************************************************/
HeartBeat::HeartBeat() :
                ProtocolTxCommand::ProtocolTxCommand(),
                m_myActualNumber(++HeartBeat::m_myHeartBeatNumber)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor for HeartBeat class
 *
 ****************************************************************************/
HeartBeat::~HeartBeat()
{
}

/****************************************************************************/
/*!
 *  \brief   Get the sent HeartBeat number
 *
 *
 *  \return  Number sent be the requester
 *
 ****************************************************************************/
quint16 HeartBeat::GetSentNumber()
{
    return m_myActualNumber;
}

/****************************************************************************/
/*!
 *  \brief   Call this function to execute the command
 *
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool HeartBeat::Execute()
{
    qDebug() << (QString)"HeartBeat: Execute called !";

    // get HeartBeat number:
    QString nr = QString::number(m_myActualNumber, 10);
    // construct Server HeartBeat message
    QString cmd = "<message><cmd name=\"HeartBeatServer\" ref=\"" +
                             QString::number(m_myRef, 10) + "\" /><dataitems nr=\"" + nr + "\" /></message>";

    return NetworkBase::ProtocolTxCommand::ExecuteWithParameter(cmd);
}

/****************************************************************************/
/*!
 *  \brief   This function handles acknowledge of this command
 *
 *  \param   status = if incoming HbNum is OK or not
 *
 ****************************************************************************/
void HeartBeat::HandleAck(const QString &status)
{
    m_myTimer.stop();
    if (m_myDevice == NULL) {
        return;
    }
    // ack received before timeout: check status
    if (status != NetworkBase::CMH_MSG_SENDING_OK) {
        // incoming HbNum is wrong: do something
        qDebug() << "HeartBeat: _ACK_ received with a wrong HBNumber !";
        m_myDevice->ReportHeartBeatProblem(NetworkBase::CMH_MSG_SENDING_FAILED);
    }
    // tell Device to deregister and destroy the command:
    m_myDevice->DeregisterRunningCommand(m_myRef, this);
}

/****************************************************************************/
/*!
 *  \brief   This function handles this command's reply timeout
 *
 *
 ****************************************************************************/
void HeartBeat::HandleAckTimeout()
{
    // we arrived here because of the timeout
    if (m_myDevice != NULL) {
        // report error:
        m_myDevice->ReportHeartBeatProblem(NetworkBase::CMH_MSG_SENDING_TIMEOUT);
        // tell Device to deregister and destroy the command:
        m_myDevice->DeregisterRunningCommand(m_myRef, this);
        /// \todo: the connection is closed here for now, but this action shall be moved to upper layer.
        m_myDevice->DisconnectPeer();
    }
}

} // end namespace NetworkBase

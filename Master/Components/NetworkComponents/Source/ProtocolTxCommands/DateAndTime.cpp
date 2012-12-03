/****************************************************************************/
/*! \file DateAndTime.cpp
 *
 *  \brief DateAndTime command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 23.08.2011
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

#include <NetworkComponents/Include/ProtocolTxCommands/DateAndTime.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
DateAndTime::DateAndTime() :
                ProtocolTxCommand::ProtocolTxCommand()
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 *
 ****************************************************************************/
DateAndTime::~DateAndTime()
{
}

/****************************************************************************/
/*!
 *  \brief   This function executes transmitting version of the command
 *
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool DateAndTime::Execute()
{
    qDebug() << (QString)"DateAndTime: ExecuteTransmitter called !";

    if (m_myDevice == NULL) {
        return false;
    }

    // Construct valid DateTime object:
    QDateTime qdt = Global::AdjustedTime::Instance().GetCurrentDateTime();
    // fetch date/time and construct message
    QString dt = qdt.toString(NetworkBase::DATEANDTIME_FORMAT);
    QString cmd = "<message><cmd name=\"SetDateAndTime\" ref=\"" +
                             QString::number(m_myRef, 10) + "\" /><dataitems dt=\"" + dt + "\" /></message>";
    // set the name to the name which will come with the ack:
    m_myName = "SetDateAndTime";

    qDebug() << "\nDateAndTime: sent date and time - " << dt << "\n";

    return NetworkBase::ProtocolTxCommand::ExecuteWithParameter(cmd);
}

/****************************************************************************/
/*!
 *  \brief   This function handles acknowledge of this command
 *
 *  \param   status = if remote command was executed successfully or not
 *
 ****************************************************************************/
void DateAndTime::HandleAck(const QString &status)
{
    qDebug() << "DateAndTime: _ACK_ called.";
    if (m_myDevice == NULL) {
        return;
    }
    // ack received before timeout: all OK, now check the status.
    if (status != NetworkBase::CMH_MSG_SENDING_OK) {
        qDebug() << "DateAndTime: negative _ACK_ received with status: " << status;
        m_myDevice->ReportFailedDateTimeSync(NetworkBase::CMH_MSG_SENDING_FAILED);
    }

    // tell Device to deregister and destroy the command:
    m_myDevice->DeregisterRunningCommand(m_myRef, this);
}

/****************************************************************************/
/*!
 *  \brief   This function handles command's reply timeout
 *
 *
 ****************************************************************************/
void DateAndTime::HandleAckTimeout()
{
    // we arrived here because of the timeout
    if (m_myDevice != NULL) {
        // report error:
        m_myDevice->ReportFailedDateTimeSync(NetworkBase::CMH_MSG_SENDING_TIMEOUT);
        // tell Device to deregister and destroy the command:
        m_myDevice->DeregisterRunningCommand(m_myRef, this);
    }
}

} // end namespace

/****************************************************************************/
/*! \file SetDateAndTime.cpp
 *
 *  \brief SetDateAndTime command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 04.08.2010
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

#include <NetworkComponents/Include/ProtocolRxCommands/SetDateAndTime.h>
#include <NetworkComponents/Include/ProtocolRxCommands/Ack.h>
#include <NetworkComponents/Include/NetworkClientDevice.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
SetDateAndTime::SetDateAndTime() :
                ProtocolRxCommand::ProtocolRxCommand(SetDateAndTimeAttr)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 *
 ****************************************************************************/
SetDateAndTime::~SetDateAndTime()
{
}

/****************************************************************************/
/*!
 *  \brief   This function executes receiving version of the command
 *
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool SetDateAndTime::Execute()
{
    qDebug() << (QString)"SetDateAndTime: ExecuteReceiver called !";

    if (m_myDevice == NULL) {
        return false;
    }

    Q_ASSERT(!m_myMessageDataTree.isEmpty());

    QString status = NetworkBase::CMH_MSG_SENDING_OK;
    // fetch date and time
    QDateTime qdt = QDateTime::fromString(m_myMessageDataTree[0].DataitemsValues[0], NetworkBase::DATEANDTIME_FORMAT);
    // it must be UTC
    qdt.setTimeSpec(Qt::UTC);
    // check if qdt is valid
    if (!qdt.isValid() || qdt.isNull()) {
        return NetworkBase::ProtocolRxCommand::CheckStatus(NetworkBase::CMH_MSG_SENDING_NACK);
    }
    // forward date and time to the application
    if (!((NetworkClientDevice*)m_myDevice)->SetDateAndTime(m_myRef, qdt)) {
        status = NetworkBase::CMH_MSG_SENDING_NACK;
    }

    return NetworkBase::Ack::SendAcknowledge(m_myDevice, m_myRef, m_myName, status);
}

} // end namespace

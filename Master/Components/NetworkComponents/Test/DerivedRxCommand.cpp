/****************************************************************************/
/*! \file DerivedRxCommand.cpp
 *
 *  \brief DerivedRxCommand command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 11.04.2011
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

#include <NetworkComponents/Test/DerivedRxCommand.h>
#include <NetworkComponents/Test/TestNetworkServerDevice.h>

namespace NetworkBase {

/****************************************************************************/
/*!
 *  \brief   Call this function to execute the command
 *
 *
 *  \return  TRUE if execution succeeded, FALSE otherwise
 *
 ****************************************************************************/
bool DerivedRxCommand::Execute()
{
    if (m_myDevice == NULL) {
        qDebug() << (QString)"DerivedRxCommand: my device is NULL !";
        return false;
    }

    qDebug() << (QString)"DerivedRxCommand: Execute called !";

    QString status = NetworkBase::CMH_MSG_SENDING_OK;
    // inform device about success:
    ((DerivedNetworkServerDevice*)m_myDevice)->CommandExecuted();
    return NetworkBase::ProtocolRxCommand::CheckStatus(status);
}

}

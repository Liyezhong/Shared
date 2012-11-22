/****************************************************************************/
/*! \file CmdSchedulerMessage.cpp
 *
 *  \brief Implementation file for class CmdSchedulerMessage.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-06-13
 *  $Author:    $ Michael Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <Global/Include/Commands/CmdSchedulerMessage.h>

namespace Global {

QString CmdSchedulerMessage::NAME           = "Global::CmdSchedulerMessage";
int     CmdSchedulerMessage::m_CmdTimeout   = 5000;

/****************************************************************************/
CmdSchedulerMessage::CmdSchedulerMessage()
    : Global::Command(m_CmdTimeout)
{
}

/****************************************************************************/
CmdSchedulerMessage::~CmdSchedulerMessage() {
}

} // end namespace Global


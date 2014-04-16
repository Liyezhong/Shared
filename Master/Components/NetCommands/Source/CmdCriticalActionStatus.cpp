/****************************************************************************/
/*! \file CmdCriticalActionStatus.cpp
 *
 *  \brief Implementation file for class CmdCriticalActionStatus.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-01
 *  $Author:    $ N.Kamath
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

#include <NetCommands/Include/CmdCriticalActionStatus.h>

namespace NetCommands {

QString CmdCriticalActionStatus::NAME           = "NetCommands::CmdCriticalActionStatus";

/****************************************************************************/
CmdCriticalActionStatus::CmdCriticalActionStatus(int TimeOut, CriticalActionStatus_t CriticalActionStatus) :
    Global::Command(TimeOut),
    m_CriticalActionStatus(CriticalActionStatus),
    m_EvaluateCommand(true)
{
}

/****************************************************************************/
CmdCriticalActionStatus::CmdCriticalActionStatus():
    Global::Command(0),
    m_CriticalActionStatus(NO_CRITICAL_ACTION),
    m_EvaluateCommand(true)
{

}

/****************************************************************************/
CmdCriticalActionStatus::~CmdCriticalActionStatus() {
}

} // end namespace NetCommands






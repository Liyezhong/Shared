/****************************************************************************/
/*! \file CmdUserAction.cpp
 *
 *  \brief Implementation file for class CmdUserAction.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-04-11
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

#include <NetCommands/Include/CmdUserAction.h>

namespace NetCommands {

QString CmdUserAction::NAME           = "NetCommands::CmdUserAction";

/****************************************************************************/
CmdUserAction::CmdUserAction(int TimeOut, quint32 CurrentUserActionID, bool UserActionCompleted) :
    Global::Command(TimeOut),
    m_UserActionID(CurrentUserActionID),
    m_UserActionCompleted(UserActionCompleted)
{
}

/****************************************************************************/
CmdUserAction::CmdUserAction():
    Global::Command(0),
    m_UserActionID(0),
    m_UserActionCompleted(true)
{

}

/****************************************************************************/
CmdUserAction::~CmdUserAction() {
}

} // end namespace NetCommands







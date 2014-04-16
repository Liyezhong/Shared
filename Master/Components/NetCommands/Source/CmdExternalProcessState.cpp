/****************************************************************************/
/*! \file CmdExternalProcessState.cpp
 *
 *  \brief Implementation file for class CmdExternalProcessState.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-11-05
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

#include <NetCommands/Include/CmdExternalProcessState.h>

namespace NetCommands {

QString CmdExternalProcessState::NAME           = "NetCommands::CmdExternalProcessState";

/****************************************************************************/
CmdExternalProcessState::CmdExternalProcessState(int TimeOut, ProcessType_t ProcessType, bool Connected, bool StoppedForEver) :
    Global::Command(TimeOut),
    m_ProcessType(ProcessType),
    m_Connected(Connected),
    m_StoppedForEver(StoppedForEver)
{
}

/****************************************************************************/
CmdExternalProcessState::CmdExternalProcessState():
    Global::Command(0)
{

}

/****************************************************************************/
CmdExternalProcessState::~CmdExternalProcessState() {
}

} // end namespace NetCommands




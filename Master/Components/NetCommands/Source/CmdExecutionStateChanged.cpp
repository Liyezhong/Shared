/****************************************************************************/
/*! \file CmdExecutionStateChanged.cpp
 *
 *  \brief Implementation file for class CmdExecutionStateChanged.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-08-13
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

#include <NetCommands/Include/CmdExecutionStateChanged.h>

namespace NetCommands {

QString CmdExecutionStateChanged::NAME           = "NetCommands::CmdExecutionStateChanged";

/****************************************************************************/
CmdExecutionStateChanged::CmdExecutionStateChanged(int TimeOut) :
    Global::Command(TimeOut)
{
}

/****************************************************************************/
CmdExecutionStateChanged::CmdExecutionStateChanged():
    Global::Command(0)
{

}

/****************************************************************************/
CmdExecutionStateChanged::~CmdExecutionStateChanged() {
}

} // end namespace NetCommands



/****************************************************************************/
/*! \file CmdCriticalActionCheck.cpp
 *
 *  \brief Implementation file for class CmdCriticalActionCheck.
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

#include <NetCommands/Include/CmdCriticalActionCheck.h>

namespace NetCommands {

QString CmdCriticalActionCheck::NAME           = "NetCommands::CmdCriticalActionCheck";

/****************************************************************************/
CmdCriticalActionCheck::CmdCriticalActionCheck(int TimeOut) :
    Global::Command(TimeOut)
{
}

/****************************************************************************/
CmdCriticalActionCheck::CmdCriticalActionCheck():
    Global::Command(0)
{

}

/****************************************************************************/
CmdCriticalActionCheck::~CmdCriticalActionCheck() {
}

} // end namespace NetCommands





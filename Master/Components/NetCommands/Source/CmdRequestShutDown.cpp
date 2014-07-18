/****************************************************************************/
/*! \file NetCommands/Source/CmdRequestShutDown.cpp
 *
 *  \brief Implementation file for class CmdRequestShutDown. This command
 *          is used to ask user to shutdown the system on system error.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2014-11-02
 *  $Author:    $ Ramya GJ
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

#include <NetCommands/Include/CmdRequestShutDown.h>

namespace NetCommands {

QString CmdRequestShutDown::NAME = "Global::CmdRequestShutDown";

/****************************************************************************/
CmdRequestShutDown::CmdRequestShutDown() :
    Command(NOTIMEOUT)
    ,m_ShutDown(false)
{
}

/****************************************************************************/
CmdRequestShutDown::CmdRequestShutDown(int TimeOut, bool ShutDown):
    Command(TimeOut)
    ,m_ShutDown(ShutDown)
{
}

/****************************************************************************/
CmdRequestShutDown::~CmdRequestShutDown() {
}

/****************************************************************************/
const CmdRequestShutDown & CmdRequestShutDown::operator = (const CmdRequestShutDown &rOther) {
    if(this != &rOther) {
        Command::operator =(rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void CmdRequestShutDown::CopyFrom(const CmdRequestShutDown &/*rOther*/) {
}

} // end namespace Global


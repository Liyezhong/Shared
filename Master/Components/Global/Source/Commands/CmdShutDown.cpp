/****************************************************************************/
/*! \file Global/Source/Commands/CmdShutDown.cpp
 *
 *  \brief Implementation file for class CmdShutDown.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-02-19
 *  $Author:    $ N.Kamath
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

#include <Global/Include/Commands/CmdShutDown.h>

namespace Global {

QString CmdShutDown::NAME = "Global::CmdShutDown";

/****************************************************************************/
CmdShutDown::CmdShutDown(bool Reboot) :
    Command(NOTIMEOUT),
    m_IsReboot(Reboot)
{
}

/****************************************************************************/
CmdShutDown::CmdShutDown(const CmdShutDown &rOther) :
    Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
CmdShutDown::~CmdShutDown() {
}

/****************************************************************************/
const CmdShutDown & CmdShutDown::operator = (const CmdShutDown &rOther) {
    if(this != &rOther) {
        Command::operator =(rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void CmdShutDown::CopyFrom(const CmdShutDown &/*rOther*/) {
}

} // end namespace Global


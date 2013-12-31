/****************************************************************************/
/*! \file Global/Source/Commands/CmdLCDPowerControl.cpp
 *
 *  \brief Implementation file for class CmdLCDPowerControl.
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

#include <Global/Include/Commands/CmdLCDPowerControl.h>

namespace Global {

QString CmdLCDPowerControl::NAME = "Global::CmdLCDPowerControl";
int     CmdLCDPowerControl::m_CmdTimeout   = 10000;
/****************************************************************************/
CmdLCDPowerControl::CmdLCDPowerControl(bool LCDActive) :
    Command(m_CmdTimeout),
    m_LCDOn(LCDActive)
{
}

/****************************************************************************/
CmdLCDPowerControl::CmdLCDPowerControl(const CmdLCDPowerControl &rOther) :
    Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
CmdLCDPowerControl::~CmdLCDPowerControl() {
}

/****************************************************************************/
const CmdLCDPowerControl & CmdLCDPowerControl::operator = (const CmdLCDPowerControl &rOther) {
    if(this != &rOther) {
        Command::operator =(rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void CmdLCDPowerControl::CopyFrom(const CmdLCDPowerControl &rOther) {
    m_LCDOn = rOther.m_LCDOn;
}

} // end namespace Global

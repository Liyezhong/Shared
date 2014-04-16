/****************************************************************************/
/*! \file CmdSoftSwitchLEDControl.cpp
 *
 *  \brief Implementation file for class CmdSoftSwitchLEDControl.
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

#include <GPIOManager/Commands/Include/CmdSoftSwitchLEDControl.h>

namespace GPIOManager {

QString CmdSoftSwitchLEDControl::NAME = "Global::CmdSoftSwitchLEDControl";
int     CmdSoftSwitchLEDControl::m_CmdTimeout   = 5000;

/****************************************************************************/
CmdSoftSwitchLEDControl::CmdSoftSwitchLEDControl(LEDControl_t LEDControl) :
    Command(m_CmdTimeout),
    m_LEDControl(LEDControl)
{
}

/****************************************************************************/
CmdSoftSwitchLEDControl::CmdSoftSwitchLEDControl(const CmdSoftSwitchLEDControl &rOther) :
    Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
CmdSoftSwitchLEDControl::~CmdSoftSwitchLEDControl() {
}

/****************************************************************************/
const CmdSoftSwitchLEDControl & CmdSoftSwitchLEDControl::operator = (const CmdSoftSwitchLEDControl &rOther) {
    if(this != &rOther) {
        Command::operator =(rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void CmdSoftSwitchLEDControl::CopyFrom(const CmdSoftSwitchLEDControl &rOther) {
    m_LEDControl = rOther.m_LEDControl;
}

} // end namespace GPIOManager


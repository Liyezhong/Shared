/****************************************************************************/
/*! \file Global/Source/Commands/Command.cpp
 *
 *  \brief Implementation file for class Command.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-01
 *  $Author:    $ J.Bugariu
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

#include <Global/Include/Commands/Command.h>

namespace Global {

const QString   Command::SERIALIZERSTRING   = "Command";
const int       Command::NOTIMEOUT          = 0;
const int       Command::MAXTIMEOUT    = 0x7FFFFFFF;

/****************************************************************************/
Command::Command(int Timeout)
    : m_Timeout(Timeout)
    , m_stateGuard(7, true)
{
}

/****************************************************************************/
Command::Command(const Command &rOther)
    : m_Timeout(0)
    , m_stateGuard(7, true)
{
    CopyFrom(rOther);
}

/****************************************************************************/
Command::~Command() {
}

/****************************************************************************/
const Command & Command::operator = (const Command &rOther) {
    if(this != &rOther) {
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
void Command::CopyFrom(const Command &rOther) {
    m_Timeout = rOther.m_Timeout;
    m_stateGuard = rOther.m_stateGuard;
}

bool Command::isStateAllowed(QString state)
{
    if (state == "NormalState")     // everything is allowed if we are error free
        return true;

    if ((state == "ErrorState") && (isErrorStateAllowed()))
        return true;

    if ((state == "BusyState") && (isBusyStateAllowed()))
        return true;

    if ((state == "IdleState") && (isIdleStateAllowed()))
        return true;

    if ((state == "InitState"))
        return true;

    if ((state == "InitFailedState") && isInitFailedStateAllowed())
         return true;

    if ((state == "SoftSwitchMonitorState") && (isSoftSwitchStateAllowed()))
        return true;

    return false;
}

} // end namespace Global

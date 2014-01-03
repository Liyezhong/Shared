/****************************************************************************/
/*! \file Global/Source/Commands/CmdDateAndTime.cpp
 *
 *  \brief CmdDateAndTime command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.03.2010
 *   $Author:  $ Y.Novak
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

#include <Global/Include/Commands/CmdDateAndTime.h>

#include <QDataStream>
#include <QDebug>

namespace Global {

QString CmdDateAndTime::NAME = "Global::CmdDateAndTime";

/****************************************************************************/
/**
 * \brief Default constructor.
 */
/****************************************************************************/
CmdDateAndTime::CmdDateAndTime() :
    Command(Command::NOTIMEOUT)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout     Timeout for command.
 * \iparam      dt          DateTime
 *
 ****************************************************************************/
CmdDateAndTime::CmdDateAndTime(int Timeout, const QDateTime &dt) :
    Command(Timeout),
    m_DateAndTime(dt)
{
    setBusyStateAllowed(false);
    setErrorStateAllowed(false);
}

/****************************************************************************/
/**
 * \brief Copy constructor.
 *
 * Copy constructor.
 *
 * \iparam   rOther  Instance to copy from.
 */
/****************************************************************************/
CmdDateAndTime::CmdDateAndTime(const CmdDateAndTime &rOther) :
    Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 *
 *
 ****************************************************************************/
CmdDateAndTime::~CmdDateAndTime()
{
}

/****************************************************************************/
/**
 * \brief Assignment operator.
 *
 * \iparam   rOther  Instance to copy from.
 * \return              Const reference to self.
 */
/****************************************************************************/
const CmdDateAndTime & CmdDateAndTime::operator = (const CmdDateAndTime &rOther) {
    if(this != &rOther) {
        Command::operator =(rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
/**
 * \brief Copy from other instance.
 *
 * Copy from other instance.
 *
 * \iparam   rOther  Instance to copy from.
 */
/****************************************************************************/
void CmdDateAndTime::CopyFrom(const CmdDateAndTime &rOther) {
    m_DateAndTime = rOther.m_DateAndTime;
}


/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 *
 ****************************************************************************/
QString CmdDateAndTime::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns date and time
 *
 *  \return  date and time as Qt object
 *
 ****************************************************************************/
QDateTime CmdDateAndTime::GetDateTime() const
{
    return m_DateAndTime;
}

} // end namespace Threads

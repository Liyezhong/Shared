/****************************************************************************/
/*! \file CmdAxedaRemoteSessionStatus.cpp
 *
 *  \brief CmdAxedaRemoteSessionStatus command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.09.2011
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

#include <RemoteCareAgent/Include/Commands/CmdAxedaRemoteSessionStatus.h>

#include <QDataStream>
#include <QDebug>

namespace RCAgentNamespace {

QString CmdAxedaRemoteSessionStatus::NAME = "RCAgentNamespace::CmdAxedaRemoteSessionStatus";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
CmdAxedaRemoteSessionStatus::CmdAxedaRemoteSessionStatus() :
        Command(Command::NOTIMEOUT),
        m_Status(false)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *
 ****************************************************************************/
CmdAxedaRemoteSessionStatus::CmdAxedaRemoteSessionStatus(int timeout) :
        Command(timeout),
        m_Status(false)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *  \param[in] status  = the status of remote sessions: true for enabled, false for disabled
 *
 ****************************************************************************/
CmdAxedaRemoteSessionStatus::CmdAxedaRemoteSessionStatus(int timeout, bool status) :
        Command(timeout),
        m_Status(status)
{
}

/****************************************************************************/
/*!
 *  \brief   Copy Constructor
 *
 *  \param[in] rOther = instance to copy
 *
 ****************************************************************************/
CmdAxedaRemoteSessionStatus::CmdAxedaRemoteSessionStatus(const CmdAxedaRemoteSessionStatus &rOther) :
        Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
/*!
 *  \brief   Assignment operator
 *
 *  \param[in] rOther = instance to assign
 *
 ****************************************************************************/
const CmdAxedaRemoteSessionStatus & CmdAxedaRemoteSessionStatus::operator = (const CmdAxedaRemoteSessionStatus &rOther)
{
    if (this != &rOther) {
        Command::operator = (rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 *
 *
 ****************************************************************************/
CmdAxedaRemoteSessionStatus::~CmdAxedaRemoteSessionStatus()
{
}

/****************************************************************************/
/*!
 *  \brief   Copy data from another class instance
 *
 *  \param[in] rOther = instance to copy from
 *
 ****************************************************************************/
void CmdAxedaRemoteSessionStatus::CopyFrom(const CmdAxedaRemoteSessionStatus &rOther)
{
    m_Status = rOther.m_Status;
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 *
 ****************************************************************************/
QString CmdAxedaRemoteSessionStatus::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get the status of remote sessions
 *
 *  \return  true for enabled, false for disabled
 *
 ****************************************************************************/
bool CmdAxedaRemoteSessionStatus::GetRemoteSessionStatus() const
{
    return m_Status;
}

} // end namespace

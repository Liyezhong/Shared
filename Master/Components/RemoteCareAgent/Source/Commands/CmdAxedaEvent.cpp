/****************************************************************************/
/*! \file CmdAxedaEvent.cpp
 *
 *  \brief CmdAxedaEvent command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.09.2011
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

#include <RemoteCareAgent/Include/Commands/CmdAxedaEvent.h>

#include <QDataStream>
#include <QDebug>

namespace RCAgentNamespace {

QString CmdAxedaEvent::NAME = "RCAgentNamespace::CmdAxedaEvent";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
CmdAxedaEvent::CmdAxedaEvent() :
        Command(Command::NOTIMEOUT),
        m_Name(""),
        m_Message(""),
        m_Severity(""),
        m_Timestamp("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *
 ****************************************************************************/
CmdAxedaEvent::CmdAxedaEvent(int timeout) :
        Command(timeout),
        m_Name(""),
        m_Message(""),
        m_Severity(""),
        m_Timestamp("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *  \param[in] name = event's name (Axeda parameter)
 *  \param[in] message = event's description text (Axeda parameter)
 *  \param[in] severity = event's severity (Axeda parameter)
 *  \param[in] timestamp = timestamp of the event occurrence (Axeda parameter)
 *
 ****************************************************************************/
CmdAxedaEvent::CmdAxedaEvent(int timeout, const QString &name, const QString &message, \
                               const QString &severity, const QString &timestamp) :
        Command(timeout),
        m_Name(name),
        m_Message(message),
        m_Severity(severity),
        m_Timestamp(timestamp)
{
}

/****************************************************************************/
/*!
 *  \brief   Copy Constructor
 *
 *  \param[in] rOther = instance to copy
 *
 ****************************************************************************/
CmdAxedaEvent::CmdAxedaEvent(const CmdAxedaEvent &rOther) :
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
const CmdAxedaEvent & CmdAxedaEvent::operator = (const CmdAxedaEvent &rOther)
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
CmdAxedaEvent::~CmdAxedaEvent()
{
}

/****************************************************************************/
/*!
 *  \brief   Copy data from another class instance
 *
 *  \param[in] rOther = instance to copy from
 *
 ****************************************************************************/
void CmdAxedaEvent::CopyFrom(const CmdAxedaEvent &rOther)
{
    m_Name = rOther.m_Name;
    m_Message = rOther.m_Message;
    m_Severity = rOther.m_Severity;
    m_Timestamp = rOther.m_Timestamp;
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 *
 ****************************************************************************/
QString CmdAxedaEvent::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get Event name
 *
 *  \return  Event name
 *
 ****************************************************************************/
QString CmdAxedaEvent::GetEventName() const
{
    return m_Name;
}

/****************************************************************************/
/*!
 *  \brief   Get Event description
 *
 *  \return  Event description
 *
 ****************************************************************************/
QString CmdAxedaEvent::GetEventMessage() const
{
    return m_Message;
}

/****************************************************************************/
/*!
 *  \brief   Get Event severity
 *
 *  \return  Event severity
 *
 ****************************************************************************/
QString CmdAxedaEvent::GetEventSeverity() const
{
    return m_Severity;
}

/****************************************************************************/
/*!
 *  \brief   Get Event timestamp
 *
 *  \return  Event timestamp
 *
 ****************************************************************************/
QString CmdAxedaEvent::GetEventTimestamp() const
{
    return m_Timestamp;
}

} // end namespace

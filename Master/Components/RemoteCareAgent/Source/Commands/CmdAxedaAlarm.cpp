/****************************************************************************/
/*! \file CmdAxedaAlarm.cpp
 *
 *  \brief CmdAxedaAlarm command implementation.
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

#include <RemoteCareAgent/Include/Commands/CmdAxedaAlarm.h>

#include <QDataStream>
#include <QDebug>

namespace RCAgentNamespace {

QString CmdAxedaAlarm::NAME = "RCAgentNamespace::CmdAxedaAlarm";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
CmdAxedaAlarm::CmdAxedaAlarm() :
        Command(Command::NOTIMEOUT),
        m_Name(""),
        m_Message(""),
        m_Severity(""),
        m_Condition(""),
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
CmdAxedaAlarm::CmdAxedaAlarm(int timeout) :
        Command(timeout),
        m_Name(""),
        m_Message(""),
        m_Severity(""),
        m_Condition(""),
        m_Timestamp("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *  \param[in] name = alarm's name (Axeda parameter)
 *  \param[in] message = alarm's description text (Axeda parameter)
 *  \param[in] severity = alarm's severity (Axeda parameter)
 *  \param[in] condition = alarm's condition (Axeda parameter)
 *  \param[in] timestamp = timestamp of the alarm occurrence (Axeda parameter)
 *
 ****************************************************************************/
CmdAxedaAlarm::CmdAxedaAlarm(int timeout, const QString &name, const QString &message, const QString &severity, \
                   const QString &condition, const QString &timestamp) :
        Command(timeout),
        m_Name(name),
        m_Message(message),
        m_Severity(severity),
        m_Condition(condition),
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
CmdAxedaAlarm::CmdAxedaAlarm(const CmdAxedaAlarm &rOther) :
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
const CmdAxedaAlarm & CmdAxedaAlarm::operator = (const CmdAxedaAlarm &rOther)
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
CmdAxedaAlarm::~CmdAxedaAlarm()
{
}

/****************************************************************************/
/*!
 *  \brief   Copy data from another class instance
 *
 *  \param[in] rOther = instance to copy from
 *
 ****************************************************************************/
void CmdAxedaAlarm::CopyFrom(const CmdAxedaAlarm &rOther)
{
    m_Name = rOther.m_Name;
    m_Message = rOther.m_Message;
    m_Severity = rOther.m_Severity;
    m_Condition = rOther.m_Condition;
    m_Timestamp = rOther.m_Timestamp;
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 *
 ****************************************************************************/
QString CmdAxedaAlarm::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get alarm name
 *
 *  \return  alarm name
 *
 ****************************************************************************/
QString CmdAxedaAlarm::GetAlarmName() const
{
    return m_Name;
}

/****************************************************************************/
/*!
 *  \brief   Get alarm description
 *
 *  \return  alarm description
 *
 ****************************************************************************/
QString CmdAxedaAlarm::GetAlarmMessage() const
{
    return m_Message;
}

/****************************************************************************/
/*!
 *  \brief   Get alarm severity
 *
 *  \return  alarm severity
 *
 ****************************************************************************/
QString CmdAxedaAlarm::GetAlarmSeverity() const
{
    return m_Severity;
}

/****************************************************************************/
/*!
 *  \brief   Get alarm condition
 *
 *  \return  alarm condition
 *
 ****************************************************************************/
QString CmdAxedaAlarm::GetAlarmCondition() const
{
    return m_Condition;
}

/****************************************************************************/
/*!
 *  \brief   Get alarm timestamp
 *
 *  \return  alarm timestamp
 *
 ****************************************************************************/
QString CmdAxedaAlarm::GetAlarmTimestamp() const
{
    return m_Timestamp;
}

} // end namespace

/****************************************************************************/
/*! \file CmdAxedaAlarm.h
 *
 *  \brief CmdAxedaAlarm command definition.
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

#ifndef RCAGENTNAMESPACE_CMDAXEDAALARM_H
#define RCAGENTNAMESPACE_CMDAXEDAALARM_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Command.h>

namespace RCAgentNamespace {

/****************************************************************************/
/*!
 *  \brief  This class implements an Alarm command for Axeda Agent.
 *
 ****************************************************************************/
class CmdAxedaAlarm : public Global::Command
{
    /// streaming operators are friends
    friend QDataStream & operator << (QDataStream &, const CmdAxedaAlarm &);
    friend QDataStream & operator >> (QDataStream &, CmdAxedaAlarm &);

public:

    static QString NAME; ///< name of this command

    CmdAxedaAlarm();
    CmdAxedaAlarm(int timeout);
    CmdAxedaAlarm(int timeout, const QString &name, const QString &message, const QString &severity, \
                               const QString &condition, const QString &timestamp);
    CmdAxedaAlarm(const CmdAxedaAlarm &rOther);
    const CmdAxedaAlarm & operator = (const CmdAxedaAlarm &rOther);
    ~CmdAxedaAlarm();

    // inherited function
    virtual QString GetName() const;
    // own functions
    QString GetAlarmName() const;
    QString GetAlarmMessage() const;
    QString GetAlarmSeverity() const;
    QString GetAlarmCondition() const;
    QString GetAlarmTimestamp() const;

private:

    void CopyFrom(const CmdAxedaAlarm &rOther);

private:

    QString    m_Name;      ///< alarm name
    QString    m_Message;   ///< alarm description
    QString    m_Severity;  ///< alarm severity
    QString    m_Condition; ///< alarm condition
    QString    m_Timestamp; ///< timestamp of the alarm occurence

}; // end class


/****************************************************************************/
/*!
 * \brief   Streaming operator to work with the QDataStream class.
 *     See QDataStream documentation: "Reading and writing other Qt classes"
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         Command to stream from.
 *
 * \return  Stream with the serialized command
 *
 ****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdAxedaAlarm &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // stream the CmdAxedaAlarm private members
    Stream << Cmd.m_Name;
    Stream << Cmd.m_Message;
    Stream << Cmd.m_Severity;
    Stream << Cmd.m_Condition;
    Stream << Cmd.m_Timestamp;
    return Stream;
}

/****************************************************************************/
/*!
 * \brief   Streaming operator to work with the QDataStream class.
 *     See QDataStream documentation: "Reading and writing other Qt classes"
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in,out]   Cmd         Command to stream into.
 *
 * \return  Stream
 *
 ****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdAxedaAlarm &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // stream the CmdAxedaAlarm private members
    Stream >> Cmd.m_Name;
    Stream >> Cmd.m_Message;
    Stream >> Cmd.m_Severity;
    Stream >> Cmd.m_Condition;
    Stream >> Cmd.m_Timestamp;
    return Stream;
}

} // end namespace

#endif

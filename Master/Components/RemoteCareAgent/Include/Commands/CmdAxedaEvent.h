/****************************************************************************/
/*! \file CmdAxedaEvent.h
 *
 *  \brief CmdAxedaEvent command definition.
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

#ifndef RCAGENTNAMESPACE_CMDAXEDAEVENT_H
#define RCAGENTNAMESPACE_CMDAXEDAEVENT_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Command.h>

namespace RCAgentNamespace {

/****************************************************************************/
/*!
 *  \brief  This class implements an Event command for Axeda Agent.
 *
 ****************************************************************************/
class CmdAxedaEvent : public Global::Command
{
    /// streaming operators are friends
    friend QDataStream & operator << (QDataStream &, const CmdAxedaEvent &);
    friend QDataStream & operator >> (QDataStream &, CmdAxedaEvent &);

public:

    static QString NAME; ///< name of this command

    CmdAxedaEvent();
    CmdAxedaEvent(int timeout);
    CmdAxedaEvent(int timeout, const QString &name, const QString &message, const QString &severity, const QString &timestamp);
    CmdAxedaEvent(const CmdAxedaEvent &rOther);
    const CmdAxedaEvent & operator = (const CmdAxedaEvent &rOther);
    ~CmdAxedaEvent();

    // inherited function
    virtual QString GetName() const;
    // own functions
    QString GetEventName() const;
    QString GetEventMessage() const;
    QString GetEventSeverity() const;
    QString GetEventTimestamp() const;

private:

    void CopyFrom(const CmdAxedaEvent &rOther);

private:

    QString    m_Name;      ///< event name
    QString    m_Message;   ///< event description
    QString    m_Severity;  ///< event severity
    QString    m_Timestamp; ///< timestamp of the event occurence

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
inline QDataStream & operator << (QDataStream &Stream, const CmdAxedaEvent &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // stream the CmdAxedaEvent private members
    Stream << Cmd.m_Name;
    Stream << Cmd.m_Message;
    Stream << Cmd.m_Severity;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdAxedaEvent &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // stream the CmdAxedaEvent private members
    Stream >> Cmd.m_Name;
    Stream >> Cmd.m_Message;
    Stream >> Cmd.m_Severity;
    Stream >> Cmd.m_Timestamp;
    return Stream;
}

} // end namespace

#endif

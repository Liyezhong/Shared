/****************************************************************************/
/*! \file CmdAxedaRemoteSessionStatus.h
 *
 *  \brief CmdAxedaRemoteSessionStatus command definition.
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

#ifndef RCAGENTNAMESPACE_CMDAXEDAREMOTESESSIONSTATUS_H
#define RCAGENTNAMESPACE_CMDAXEDAREMOTESESSIONSTATUS_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Command.h>

namespace RCAgentNamespace {

/****************************************************************************/
/*!
 *  \brief  This class implements a RemoteSessionStatus command for Axeda Agent.
 *
 ****************************************************************************/
class CmdAxedaRemoteSessionStatus : public Global::Command
{
    /// streaming operators are friends
    friend QDataStream & operator << (QDataStream &, const CmdAxedaRemoteSessionStatus &);
    friend QDataStream & operator >> (QDataStream &, CmdAxedaRemoteSessionStatus &);

public:

    static QString NAME; ///< name of this command

    CmdAxedaRemoteSessionStatus();
    CmdAxedaRemoteSessionStatus(int timeout);
    CmdAxedaRemoteSessionStatus(int timeout, bool status);
    CmdAxedaRemoteSessionStatus(const CmdAxedaRemoteSessionStatus &rOther);
    const CmdAxedaRemoteSessionStatus & operator = (const CmdAxedaRemoteSessionStatus &rOther);
    ~CmdAxedaRemoteSessionStatus();

    // inherited function
    virtual QString GetName() const;
    // own functions
    bool GetRemoteSessionStatus() const;

private:

    void CopyFrom(const CmdAxedaRemoteSessionStatus &rOther);

private:

    bool    m_Status; ///< current status of remote sessions: true for enabled, false for disabled

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
inline QDataStream & operator << (QDataStream &Stream, const CmdAxedaRemoteSessionStatus &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // stream the CmdAxedaRemoteSessionStatus private members
    Stream << (quint8&)Cmd.m_Status;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdAxedaRemoteSessionStatus &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // stream the CmdAxedaRemoteSessionStatus private members
    Stream >> (quint8&)Cmd.m_Status;
    return Stream;
}

} // end namespace

#endif

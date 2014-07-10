/****************************************************************************/
/*! \file NetCommands/Include/CmdRCRequestRemoteSession.h
 *
 *  \brief CmdRCRequestRemoteSession command definition.
 *
 *   $Version: $ 1.0
 *   $Date:    $ 2014-03-13
 *   $Author:  $ Ramya GJ
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

#ifndef NETCOMMANDS_CMDRCREQUESTREMOTESESSION_H
#define NETCOMMANDS_CMDRCREQUESTREMOTESESSION_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>


namespace RemoteCare {

/****************************************************************************/
/**
 * \brief remote session status type.
 */
/****************************************************************************/

const QString   RemoteSession_Invalid = "Invalid";        //!< session invalid
const QString   RemoteSession_Requested = "Requested";       //!< session is requested
const QString   RemoteSession_Ended = "Ended";        //!< session ended
const QString   RemoteSession_Accepted = "Accepted";  //!< session accepted
const QString   RemoteSession_Denied    = "Denied";      //!< session denied

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCRequestRemoteSession command.
 */
/****************************************************************************/
class CmdRCRequestRemoteSession : public Global::Command {
    friend QDataStream & operator << (const QDataStream &, const CmdRCRequestRemoteSession &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRCRequestRemoteSession &);
public:
    static QString NAME;    //!< Command name.
    CmdRCRequestRemoteSession(const int& Timeout, const QString& RequestType);
    CmdRCRequestRemoteSession();
    ~CmdRCRequestRemoteSession();
    virtual QString GetName() const;
    QString GetRequestType() const;
private:
    
    CmdRCRequestRemoteSession(const CmdRCRequestRemoteSession &);                       //!< Not implemented.
    const CmdRCRequestRemoteSession & operator = (const CmdRCRequestRemoteSession &);   //!< Not implemented.
private:

    QString m_RequestType;      //!< request type
}; // end class CmdRCRequestRemoteSession

/****************************************************************************/
/**
     * \brief Output Stream Operator which streams data
     *
     * \iparam   Stream      Stream to stream into.
     * \iparam   Cmd         The command to stream.
     * \return   Output Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (const QDataStream &Stream, const CmdRCRequestRemoteSession &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
    StreamRef << Cmd.m_RequestType;
    return StreamRef;
}

/****************************************************************************/
/**
     * \brief Input Stream Operator which streams data
     *
     * \iparam   Stream      Stream to stream from.
     * \iparam   Cmd         The command to stream.
     * \return   Input Stream.
     */
/****************************************************************************/
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRCRequestRemoteSession &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRCRequestRemoteSession& CmdRef = const_cast<CmdRCRequestRemoteSession &>(Cmd);

	// copy base class data
    CmdRef.CopyFromStream(StreamRef);
    StreamRef >> CmdRef.m_RequestType;
    return StreamRef;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCREQUESTREMOTESESSION_H

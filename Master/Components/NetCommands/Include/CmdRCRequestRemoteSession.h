/****************************************************************************/
/*! \file NetCommands/Include/CmdRCRequestRemoteSession.h
 *
 *  \brief CmdRCRequestRemoteSession command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 30.04.2013
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
 * \brief remote care data item quality.
 */
/****************************************************************************/

const QString   RemoteSession_Invalid = "Invalid";        ///< session invalid
const QString   RemoteSession_Requested = "Requested";       ///< session is requested
const QString   RemoteSession_Ended = "Ended";        ///< session ended
const QString   RemoteSession_Accepted = "Accepted";  ///< session accepted
const QString   RemoteSession_Denied    = "Denied";      ///< session denied

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCRequestRemoteSession command.
 */
/****************************************************************************/
class CmdRCRequestRemoteSession : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdRCRequestRemoteSession &);
    friend QDataStream & operator >> (QDataStream &, CmdRCRequestRemoteSession &);
public:
    static QString NAME;    ///< Command name.
    CmdRCRequestRemoteSession(int Timeout, QString RequestType);
    CmdRCRequestRemoteSession();
    ~CmdRCRequestRemoteSession();
    virtual QString GetName() const;
    QString GetRequestType() const;
private:
    
    CmdRCRequestRemoteSession(const CmdRCRequestRemoteSession &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRCRequestRemoteSession & operator = (const CmdRCRequestRemoteSession &);   ///< Not implemented.
private:

    QString m_RequestType;      ///< request type
}; // end class CmdRCRequestRemoteSession

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \iparam   Stream      Stream to stream into.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRCRequestRemoteSession &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    Stream << Cmd.m_RequestType;
    return Stream;
}

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \iparam   Stream      Stream to stream from.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdRCRequestRemoteSession &Cmd)
{
	// copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> Cmd.m_RequestType;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCREQUESTREMOTESESSION_H

/****************************************************************************/
/*! \file NetCommands/Include/CmdRCNotifyReconnection.h
 *
 *  \brief CmdRCNotifyReconnection command definition.
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

#ifndef NETCOMMANDS_CMDRCNOTIFYRECONNECTION_H
#define NETCOMMANDS_CMDRCNOTIFYRECONNECTION_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>


namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCNotifyReconnection command.
 */
/****************************************************************************/
class CmdRCNotifyReconnection : public Global::Command {
    friend QDataStream & operator << (const QDataStream &, const CmdRCNotifyReconnection &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRCNotifyReconnection &);
public:
    static QString NAME;    //!< Command name.
    CmdRCNotifyReconnection(const int& Timeout);
    CmdRCNotifyReconnection();
    ~CmdRCNotifyReconnection();
    virtual QString GetName() const;
private:
    
    CmdRCNotifyReconnection(const CmdRCNotifyReconnection &);                       //!< Not implemented.
    const CmdRCNotifyReconnection & operator = (const CmdRCNotifyReconnection &);   //!< Not implemented.
private:
}; // end class CmdRCNotifyReconnection

/****************************************************************************/
/**
     * \brief Output Stream Operator which streams data
     *
     * \iparam   Stream      Stream to stream into.
     * \iparam   Cmd         The command to stream.
     * \return   Output Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (const QDataStream &Stream, const CmdRCNotifyReconnection &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
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
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRCNotifyReconnection &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRCNotifyReconnection& CmdRef = const_cast<CmdRCNotifyReconnection &>(Cmd);

	// copy base class data
    CmdRef.CopyFromStream(StreamRef);

    return StreamRef;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCNOTIFYRECONNECTION_H

/****************************************************************************/
/*! \file NetCommands/Include/CmdRCNotifyReconnection.h
 *
 *  \brief CmdRCNotifyReconnection command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.07.2013
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
    friend QDataStream & operator << (QDataStream &, const CmdRCNotifyReconnection &);
    friend QDataStream & operator >> (QDataStream &, CmdRCNotifyReconnection &);
public:
    static QString NAME;    ///< Command name.
    CmdRCNotifyReconnection(int Timeout);
    CmdRCNotifyReconnection();
    ~CmdRCNotifyReconnection();
    virtual QString GetName() const;
private:
    
    CmdRCNotifyReconnection(const CmdRCNotifyReconnection &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRCNotifyReconnection & operator = (const CmdRCNotifyReconnection &);   ///< Not implemented.
private:
}; // end class CmdRCNotifyReconnection

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \iparam   Stream      Stream to stream into.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRCNotifyReconnection &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRCNotifyReconnection &Cmd)
{
	// copy base class data
    Cmd.CopyFromStream(Stream);
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCNOTIFYRECONNECTION_H

/****************************************************************************/
/*! \file NetCommands/Include/CmdRCNotifyShutdown.h
 *
 *  \brief CmdRCNotifyShutdown command definition.
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

#ifndef NETCOMMANDS_CMDRCNOTIFYSHUTDOWN_H
#define NETCOMMANDS_CMDRCNOTIFYSHUTDOWN_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>


namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCNotifyShutdown command.
 */
/****************************************************************************/
class CmdRCNotifyShutdown : public Global::Command {
    friend QDataStream & operator << (const QDataStream &, const CmdRCNotifyShutdown &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRCNotifyShutdown &);
public:
    static QString NAME;    //!< Command name.
    CmdRCNotifyShutdown(const int& Timeout);
	CmdRCNotifyShutdown();
    ~CmdRCNotifyShutdown();
    virtual QString GetName() const;
private:
    
    CmdRCNotifyShutdown(const CmdRCNotifyShutdown &);                       //!< Not implemented.
    const CmdRCNotifyShutdown & operator = (const CmdRCNotifyShutdown &);   //!< Not implemented.
private:
}; // end class CmdRCNotifyShutdown

/****************************************************************************/
/**
     * \brief Output Stream Operator which streams data
     *
     * \iparam   Stream      Stream to stream into.
     * \iparam   Cmd         The command to stream.
     * \return   Output Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (const QDataStream &Stream, const CmdRCNotifyShutdown &Cmd)
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
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRCNotifyShutdown &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRCNotifyShutdown& CmdRef = const_cast<CmdRCNotifyShutdown &>(Cmd);

	// copy base class data
    CmdRef.CopyFromStream(StreamRef);
    return StreamRef;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCNOTIFYSHUTDOWN_H

/****************************************************************************/
/*! \file NetCommands/Include/CmdRCNotifyShutdown.h
 *
 *  \brief CmdRCNotifyShutdown command definition.
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
    friend QDataStream & operator << (QDataStream &, const CmdRCNotifyShutdown &);
    friend QDataStream & operator >> (QDataStream &, CmdRCNotifyShutdown &);
public:
    static QString NAME;    ///< Command name.
    CmdRCNotifyShutdown(int Timeout);
	CmdRCNotifyShutdown();
    ~CmdRCNotifyShutdown();
    virtual QString GetName() const;
private:
    
    CmdRCNotifyShutdown(const CmdRCNotifyShutdown &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRCNotifyShutdown & operator = (const CmdRCNotifyShutdown &);   ///< Not implemented.
private:
}; // end class CmdRCNotifyShutdown

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \iparam   Stream      Stream to stream into.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRCNotifyShutdown &Cmd)
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRCNotifyShutdown &Cmd)
{
	// copy base class data
    Cmd.CopyFromStream(Stream);
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCNOTIFYSHUTDOWN_H

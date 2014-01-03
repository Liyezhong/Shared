/****************************************************************************/
/*! \file CmdDayRunLogRequest.h
 *
 *  \brief CmdDayRunLogRequest command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.03.2011
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

#ifndef NETCOMMANDS_CMDDAYRUNLOGREQUEST_H
#define NETCOMMANDS_CMDDAYRUNLOGREQUEST_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdDayRunLogRequest command.
 *
 */
/****************************************************************************/
class CmdDayRunLogRequest : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdDayRunLogRequest &);
    friend QDataStream & operator >> (QDataStream &, CmdDayRunLogRequest &);
public:
    static QString NAME;    ///< Command name.
    CmdDayRunLogRequest(int Timeout, int UserLevel);
    CmdDayRunLogRequest();
    ~CmdDayRunLogRequest();
    virtual QString GetName() const;
    int GetUserLevel() const;
private:
    int m_UserLevel;                ///< Store the user level
    CmdDayRunLogRequest(const CmdDayRunLogRequest &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdDayRunLogRequest & operator = (const CmdDayRunLogRequest &);   ///< Not implemented.
}; // end class CmdDayRunLogRequest

/****************************************************************************/
/*!
 * \brief Streaming operator.
 *
 * \oparam   Stream      Stream to stream into.
 * \iparam   Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDayRunLogRequest &Cmd)
{    
    // copy base class data
    Cmd.CopyToStream(Stream);
    Stream << Cmd.m_UserLevel;
    return Stream;
}

/****************************************************************************/
/*!
 * \brief Streaming operator.
 *
 * \oparam   Stream      Stream to stream from.
 * \iparam   Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdDayRunLogRequest &Cmd)
{    
    // copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> Cmd.m_UserLevel;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDDAYRUNLOGREQUEST_H

/****************************************************************************/
/*! \file CmdUserAuthenticationReply.h
 *
 *  \brief CmdUserAuthenticationReply command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 30.10.2012
 *   $Author:  $ Raju
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

#ifndef NETCOMMANDS_CMDUSERAUTHENTICATIONREPLY_H
#define NETCOMMANDS_CMDUSERAUTHENTICATIONREPLY_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a authenticated user levels.
 */
/****************************************************************************/
class CmdUserAuthenticationReply : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdUserAuthenticationReply &);
    friend QDataStream & operator >> (QDataStream &, CmdUserAuthenticationReply &);
public:
    static QString NAME;    ///< Command name.
    CmdUserAuthenticationReply(int Timeout, const Global::GuiUserLevel &UserLevel);
    CmdUserAuthenticationReply();
    ~CmdUserAuthenticationReply();
    virtual QString GetName() const;
    Global::GuiUserLevel GetUserAuthenticatedLevel() const;
private:
    
    CmdUserAuthenticationReply(const CmdUserAuthenticationReply &);                       ///< Not implemented.
    const CmdUserAuthenticationReply & operator = (const CmdUserAuthenticationReply &);   ///< Not implemented.
private:
    Global::GuiUserLevel  m_UserLevel;      ///< Type of the user authenticated
}; // end class CmdUserAuthenticationReply

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdUserAuthenticationReply &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << (qint32)Cmd.m_UserLevel;
    return Stream;
}

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream from.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdUserAuthenticationReply &Cmd)
{
    qint32 UserLevel;
    // copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> UserLevel;
    Cmd.m_UserLevel = (Global::GuiUserLevel)UserLevel;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDUSERAUTHENTICATIONREPLY_H

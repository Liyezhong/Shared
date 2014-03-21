/****************************************************************************/
/*! \file CmdChangeUserLevelReply.h
 *
 *  \brief CmdChangeUserLevelReply command definition.
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

#ifndef NETCOMMANDS_CmdChangeUserLevelReply_H
#define NETCOMMANDS_CmdChangeUserLevelReply_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a authenticated user levels.
 */
/****************************************************************************/
class CmdChangeUserLevelReply : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdChangeUserLevelReply &);
    friend QDataStream & operator >> (QDataStream &, CmdChangeUserLevelReply &);
public:
    static QString NAME;    ///< Command name.
    CmdChangeUserLevelReply(int Timeout, const Global::GuiUserLevel &UserLevel);
    CmdChangeUserLevelReply();
    ~CmdChangeUserLevelReply();
    virtual QString GetName() const;
    Global::GuiUserLevel GetUserAuthenticatedLevel() const;
private:
    
    CmdChangeUserLevelReply(const CmdChangeUserLevelReply &);                       ///< Not implemented.
    const CmdChangeUserLevelReply & operator = (const CmdChangeUserLevelReply &);   ///< Not implemented.
private:
    Global::GuiUserLevel  m_UserLevel;      ///< Type of the user authenticated
}; // end class CmdChangeUserLevelReply

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdChangeUserLevelReply &Cmd)
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
inline QDataStream & operator >> (QDataStream &Stream, CmdChangeUserLevelReply &Cmd)
{
    qint32 UserLevel;
    // copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> UserLevel;
    Cmd.m_UserLevel = (Global::GuiUserLevel)UserLevel;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CmdChangeUserLevelReply_H

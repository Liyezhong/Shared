/****************************************************************************/
/*! \file NetCommands/Include/CmdChangeUserLevel.h
 *
 *  \brief CmdChangeUserLevel command definition.
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

#ifndef NETCOMMANDS_CMDCHANGEUSERLEVEL_H
#define NETCOMMANDS_CMDCHANGEUSERLEVEL_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>


namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdChangeUserLevel command.
 */
/****************************************************************************/
class CmdChangeUserLevel : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdChangeUserLevel &);
    friend QDataStream & operator >> (QDataStream &, CmdChangeUserLevel &);
public:
    static QString NAME;    ///< Command name.
    CmdChangeUserLevel(int Timeout, const Global::GuiUserLevel &UserLevel, const QString &Password);
	CmdChangeUserLevel();
    ~CmdChangeUserLevel();
    virtual QString GetName() const;
    Global::GuiUserLevel GetUserLevel() const;
	QString GetPassword() const;
private:
    
    CmdChangeUserLevel(const CmdChangeUserLevel &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdChangeUserLevel & operator = (const CmdChangeUserLevel &);   ///< Not implemented.
private:
    Global::GuiUserLevel  m_UserLevel;      ///< Type of the user
    QString         m_Password;	   ///< password
}; // end class CmdChangeUserLevel

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdChangeUserLevel &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << (qint32)Cmd.m_UserLevel << Cmd.m_Password;
    return Stream;
}

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream from.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdChangeUserLevel &Cmd)
{
    qint32 UserLevel;
    // copy base class data
    Cmd.CopyFromStream(Stream);
    Stream >> UserLevel;
    Cmd.m_UserLevel = (Global::GuiUserLevel)UserLevel;
    // copy internal data
    Stream >> Cmd.m_Password;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDCHANGEUSERLEVEL_H

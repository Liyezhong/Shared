/****************************************************************************/
/*! \file NetCommands/Include/CmdChangeAdminPassword.h
 *
 *  \brief CmdChangeAdminPassword command definition.
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

#ifndef NETCOMMANDS_CMDCHANGEADMINPASSWORD_H
#define NETCOMMANDS_CMDCHANGEADMINPASSWORD_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdChangeAdminPassword command.
 */
/****************************************************************************/
class CmdChangeAdminPassword : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdChangeAdminPassword &);
    friend QDataStream & operator >> (QDataStream &, CmdChangeAdminPassword &);
public:
    static QString NAME;    ///< Command name.
    CmdChangeAdminPassword(int Timeout, const QString &CmdType, const QString &Password);
	CmdChangeAdminPassword();
    ~CmdChangeAdminPassword();
    virtual QString GetName() const;
    QString GetCommandType() const;
	QString GetPassword() const;
private:
    
    CmdChangeAdminPassword(const CmdChangeAdminPassword &);                       ///< Not implemented.
    const CmdChangeAdminPassword & operator = (const CmdChangeAdminPassword &);   ///< Not implemented.
private:
    QString      m_CmdType;        ///< Type of the command
	QString		 m_Password;	   ///< password
}; // end class CmdChangeAdminPassword

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdChangeAdminPassword &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_CmdType << Cmd.m_Password;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdChangeAdminPassword &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_CmdType >> Cmd.m_Password;
    return Stream;
}
} // end namespace NetCommands

#endif // NETCOMMANDS_CMDCHANGEADMINPASSWORD_H

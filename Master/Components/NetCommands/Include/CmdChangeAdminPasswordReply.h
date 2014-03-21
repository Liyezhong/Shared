/****************************************************************************/
/*! \file NetCommands/Include/CmdChangeAdminPasswordReply.h
 *
 *  \brief CmdChangeAdminPasswordReply command definition.
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

#ifndef NETCOMMANDS_CMDCHANGEADMINPASSWORDREPLY_H
#define NETCOMMANDS_CMDCHANGEADMINPASSWORDREPLY_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdChangeAdminPasswordReply command.
 */
/****************************************************************************/
class CmdChangeAdminPasswordReply : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdChangeAdminPasswordReply &);
    friend QDataStream & operator >> (QDataStream &, CmdChangeAdminPasswordReply &);
public:
    static QString NAME;    ///< Command name.
    CmdChangeAdminPasswordReply(int Timeout, const QString &CmdType);
	CmdChangeAdminPasswordReply();
    ~CmdChangeAdminPasswordReply();
    virtual QString GetName() const;
    QString GetCommandType() const;	
private:
    
    CmdChangeAdminPasswordReply(const CmdChangeAdminPasswordReply &);                       ///< Not implemented.
    const CmdChangeAdminPasswordReply & operator = (const CmdChangeAdminPasswordReply &);   ///< Not implemented.
private:
    QString      m_CmdType;        ///< Type of the command	
}; // end class CmdChangeAdminPasswordReply

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdChangeAdminPasswordReply &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_CmdType;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdChangeAdminPasswordReply &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_CmdType;
    return Stream;
}
} // end namespace NetCommands

#endif // NETCOMMANDS_CMDCHANGEADMINPASSWORDREPLY_H

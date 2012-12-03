/****************************************************************************/
/*! \file CmdDayRunLogReply.h
 *
 *  \brief CmdDayRunLogReply command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 06.11.2012
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

#ifndef NETCOMMANDS_CMDDAYRUNLOGREPLY_H
#define NETCOMMANDS_CMDDAYRUNLOGREPLY_H

#include <Global/Include/Commands/Command.h>
#include <QStringList>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdDayRunLogReply command.
 *
 */
/****************************************************************************/
class CmdDayRunLogReply : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdDayRunLogReply &);
    friend QDataStream & operator >> (QDataStream &, CmdDayRunLogReply &);
public:
    static QString NAME;    ///< Command name.
    CmdDayRunLogReply(int Timeout, const QStringList &FileList);
    CmdDayRunLogReply();
    ~CmdDayRunLogReply();
    virtual QString GetName() const;
    QStringList GetFileNames() const;

private:
    CmdDayRunLogReply(const CmdDayRunLogReply &);                       ///< Not implemented.
    const CmdDayRunLogReply & operator = (const CmdDayRunLogReply &);   ///< Not implemented.
private:
    QStringList     m_FileNames;      ///< List of daily run log file names.
}; // end class CmdDayRunLogReply

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDayRunLogReply &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_FileNames;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdDayRunLogReply &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_FileNames;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDDAYRUNLOGREPLY_H

/****************************************************************************/
/*! \file CmdExportDayRunLogReply.h
 *
 *  \brief CmdExportDayRunLogReply command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.12.2012
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

#ifndef NETCOMMANDS_CMDEXPORTDAYRUNLOGREPLY_H
#define NETCOMMANDS_CMDEXPORTDAYRUNLOGREPLY_H

#include <Global/Include/Commands/Command.h>
#include <QStringList>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdExportDayRunLogReply command.
 *
 */
/****************************************************************************/
class CmdExportDayRunLogReply : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdExportDayRunLogReply &);
    friend QDataStream & operator >> (QDataStream &, CmdExportDayRunLogReply &);
public:
    static QString NAME;    ///< Command name.
    CmdExportDayRunLogReply(int Timeout, const QStringList &FileList);
    CmdExportDayRunLogReply();
    ~CmdExportDayRunLogReply();
    virtual QString GetName() const;
    QStringList GetFileNames() const;

private:
    CmdExportDayRunLogReply(const CmdExportDayRunLogReply &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdExportDayRunLogReply & operator = (const CmdExportDayRunLogReply &);   ///< Not implemented.
private:
    QStringList     m_FileNames;      ///< List of daily run log file names.
}; // end class CmdExportDayRunLogReply

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdExportDayRunLogReply &Cmd)
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
     * \iparam       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdExportDayRunLogReply &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_FileNames;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDEXPORTDAYRUNLOGREPLY_H

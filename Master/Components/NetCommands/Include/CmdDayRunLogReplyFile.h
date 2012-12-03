/****************************************************************************/
/*! \file CmdDayRunLogReplyFile.h
 *
 *  \brief CmdDayRunLogReplyFile command definition.
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

#ifndef NETCOMMANDS_CMDDAYRUNLOGREPLYFILE_H
#define NETCOMMANDS_CMDDAYRUNLOGREPLYFILE_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdDayRunLogReplyFile command.
 *
 */
/****************************************************************************/
class CmdDayRunLogReplyFile : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdDayRunLogReplyFile &);
    friend QDataStream & operator >> (QDataStream &, CmdDayRunLogReplyFile &);
public:
    static QString NAME;    ///< Command name.
    CmdDayRunLogReplyFile(int Timeout, const QDataStream &FileData);
    CmdDayRunLogReplyFile();
    ~CmdDayRunLogReplyFile();
    virtual QString GetName() const;
    const QByteArray &GetFileData() const;
private:    
    CmdDayRunLogReplyFile(const CmdDayRunLogReplyFile &);                       ///< Not implemented.
    const CmdDayRunLogReplyFile & operator = (const CmdDayRunLogReplyFile &);   ///< Not implemented.
private:
    QByteArray     m_FileData;      ///< Requested file data
}; // end class CmdDayRunLogReplyFile

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDayRunLogReplyFile &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_FileData;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdDayRunLogReplyFile &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_FileData;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDDAYRUNLOGREPLYFILE_H

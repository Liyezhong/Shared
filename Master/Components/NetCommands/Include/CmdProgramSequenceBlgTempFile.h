/****************************************************************************/
/*! \file CmdProgramSequenceBlgTempFile.h
 *
 *  \brief CmdProgramSequenceBlgTempFile command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 28.11.2012
 *   $Author:  $ Ningu
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

#ifndef NETCOMMANDS_CMDPROGRAMSEQUENCEBLGTEMPFILE_H
#define NETCOMMANDS_CMDPROGRAMSEQUENCEBLGTEMPFILE_H

#include <Global/Include/Commands/Command.h>
#include <QStringList>
#include <QDataStream>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdProgramSequenceBlgTempFile command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdProgramSequenceBlgTempFile : public Global::Command {
        friend QDataStream & operator << (QDataStream &, const CmdProgramSequenceBlgTempFile &);
        friend QDataStream & operator >> (QDataStream &, CmdProgramSequenceBlgTempFile &);

public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdProgramSequenceBlgTempFile(int Timeout, const QDataStream &ProgramSequenceBlgTempFile);
    CmdProgramSequenceBlgTempFile();
    ~CmdProgramSequenceBlgTempFile();
    virtual QString GetName() const;
    QByteArray const& GetProgramSequenceBlgTempFile() const;

private:
    CmdProgramSequenceBlgTempFile(const CmdProgramSequenceBlgTempFile &);                       ///< Not implemented.
    const CmdProgramSequenceBlgTempFile & operator = (const CmdProgramSequenceBlgTempFile &);   ///< Not implemented.

private:
    QByteArray      m_ProgramSeqBlgTemp;       ///< The ProgramSequenceBlgTempFile.
}; // end class CmdProgramSequenceBlgTempFile

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdProgramSequenceBlgTempFile &Cmd) {
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_ProgramSeqBlgTemp;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdProgramSequenceBlgTempFile &Cmd) {

    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ProgramSeqBlgTemp;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDPROGRAMSEQUENCEBLGTEMPFILE_H

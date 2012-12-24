/****************************************************************************/
/*! \file CmdGenerateBathLayout.h
 *
 *  \brief CmdGenerateBathLayout command definition.
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

#ifndef NETCOMMANDS_CMDGENERATEBATHLAYOUT_H
#define NETCOMMANDS_CMDGENERATEBATHLAYOUT_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdGenerateBathLayout command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdGenerateBathLayout : public Global::Command {
public:
    static QString NAME;    ///< Command name.

    friend QDataStream & operator << (QDataStream &, const CmdGenerateBathLayout &);
    friend QDataStream & operator >> (QDataStream &, CmdGenerateBathLayout &);
    /****************************************************************************/
    CmdGenerateBathLayout(int Timeout, const QDataStream &ProgramSequenceBlgTempFile);
    CmdGenerateBathLayout();
    ~CmdGenerateBathLayout();
    virtual QString GetName() const;    
    QByteArray const& GetProgramSequenceBlgTempFile() const;
private:
    CmdGenerateBathLayout(const CmdGenerateBathLayout &);                       ///< Not implemented.
    const CmdGenerateBathLayout & operator = (const CmdGenerateBathLayout &);   ///< Not implemented.

private:
    QByteArray      m_ProgramSeqBlgTemp;       ///< The ProgramSequenceBlgTempFile.
}; // end class CmdGenerateBathLayout

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdGenerateBathLayout &Cmd) {
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
inline QDataStream & operator >> (QDataStream &Stream, CmdGenerateBathLayout &Cmd) {

    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_ProgramSeqBlgTemp;
    return Stream;
}

} // end namespace NetCommands

#endif

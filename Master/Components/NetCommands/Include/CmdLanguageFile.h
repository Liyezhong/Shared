/****************************************************************************/
/*! \file CmdLanguageFile.h
 *
 *  \brief CmdLanguageFile command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 28.08.2012
 *   $Author:  $ N.Kamath
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

#ifndef CmdLanguageFile_H
#define CmdLanguageFile_H

#include <Global/Include/Commands/Command.h>
#include <QByteArray>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdLanguageFile command.
 *
 * \todo implement
 */
/****************************************************************************/

class CmdLanguageFile : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdLanguageFile &);
    friend QDataStream & operator >> (QDataStream &, CmdLanguageFile &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdLanguageFile(int Timeout, const QDataStream &LanguageFileDataStream);
    CmdLanguageFile();
    ~CmdLanguageFile();
    virtual QString GetName() const;
    /****************************************************************************/
    /**
     * \brief Get the Language file data
     * \return Byte array.
     */
    /****************************************************************************/
    QByteArray const & GetLanguageData() const { return m_LanguageFileByteArray;}

private:
    CmdLanguageFile(const CmdLanguageFile &);                     ///< Not implemented.
    const CmdLanguageFile & operator = (const CmdLanguageFile &); ///< Not implemented.

    QByteArray m_LanguageFileByteArray; //!< Byte array for Language file
}; // end class CmdLanguageFile

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdLanguageFile &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream <<Cmd.m_LanguageFileByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdLanguageFile &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_LanguageFileByteArray;
    return Stream;
}

} // end namespace NetCommands



#endif // CMDLANGUAGEFILE_H

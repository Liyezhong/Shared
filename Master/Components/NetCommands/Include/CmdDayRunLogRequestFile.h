/****************************************************************************/
/*! \file CmdDayRunLogRequestFile.h
 *
 *  \brief CmdDayRunLogRequestFile command definition.
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

#ifndef NETCOMMANDS_CMDDAYRUNLOGREQUESTFILE_H
#define NETCOMMANDS_CMDDAYRUNLOGREQUESTFILE_H

#include <Global/Include/Commands/Command.h>

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdDayRunLogRequestFile command.
 *
 */
/****************************************************************************/
class CmdDayRunLogRequestFile : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdDayRunLogRequestFile &);
    friend QDataStream & operator >> (QDataStream &, CmdDayRunLogRequestFile &);
public:
    static QString NAME;    ///< Command name.
    CmdDayRunLogRequestFile(int Timeout, int UserLevel, const QString &FileName);
    CmdDayRunLogRequestFile();                                                    ///< Not implemented.
    ~CmdDayRunLogRequestFile();
    virtual QString GetName() const;
    QString GetFileName() const;
    int GetUserLevel() const;
private:
    int m_UserLevel;                ///< Store the user level
    CmdDayRunLogRequestFile(const CmdDayRunLogRequestFile &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdDayRunLogRequestFile & operator = (const CmdDayRunLogRequestFile &);   ///< Not implemented.
private:
    QString     m_FileName;      ///< file name of the Daily run log file.
}; // end class CmdDayRunLogRequestFile

/****************************************************************************/
/*!
 * \brief Streaming operator.
 *
 * \oparam   Stream      Stream to stream into.
 * \iparam   Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDayRunLogRequestFile &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_UserLevel;
    Stream << Cmd.m_FileName;
    return Stream;
}

/****************************************************************************/
/*!
 * \brief Streaming operator.
 *
 * \oparam   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdDayRunLogRequestFile &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_UserLevel;
    Stream >> Cmd.m_FileName;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDDAYRUNLOGREQUESTFILE_H

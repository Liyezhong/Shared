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
#include <Global/Include/GlobalDefines.h>
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
    CmdDayRunLogRequestFile(int Timeout, const QString &FileName, Global::GuiUserLevel CurrentUserRole);
    CmdDayRunLogRequestFile();                                                    ///< Not implemented.
    ~CmdDayRunLogRequestFile();
    virtual QString GetName() const;
    QString GetFileName() const;
    Global::GuiUserLevel GetCurrentUserRole() const;
private:
    CmdDayRunLogRequestFile(const CmdDayRunLogRequestFile &);                       ///< Not implemented.
    const CmdDayRunLogRequestFile & operator = (const CmdDayRunLogRequestFile &);   ///< Not implemented.
private:
    QString     m_FileName;      ///< file name of the Daily run log file.
    Global::GuiUserLevel m_CurrentUserRole;
}; // end class CmdDayRunLogRequestFile

/****************************************************************************/
/**
     * \brief Streaming operator.
     *
     * \param[in,out]   Stream      Stream to stream into.
     * \param[in]       Cmd         The command to stream.
     * \return                      Stream.
     */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdDayRunLogRequestFile &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_FileName;
    quint8 intTemp = Cmd.m_CurrentUserRole;
    Stream << intTemp;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdDayRunLogRequestFile &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_FileName;
    quint8 intTemp;
    Stream >> intTemp;
    Cmd.m_CurrentUserRole = (Global::GuiUserLevel)intTemp;
    return Stream;
}

} // end namespace NetCommands

#endif // NETCOMMANDS_CMDDAYRUNLOGREQUESTFILE_H

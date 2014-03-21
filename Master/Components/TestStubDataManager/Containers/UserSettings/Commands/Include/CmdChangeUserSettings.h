/****************************************************************************/
/*! \file CmdChangeUserSettings.h
 *
 *  \brief CmdChangeUserSettings command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.03.2011
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

#ifndef MSGCLASSES_CMDCHANGEUSERSETTINGS_H
#define MSGCLASSES_CMDCHANGEUSERSETTINGS_H

#include "Global/Include/Commands/Command.h"

namespace MsgClasses {

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdChangeUserSettings command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdChangeUserSettings : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdChangeUserSettings &);
    friend QDataStream & operator >> (QDataStream &, CmdChangeUserSettings &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdChangeUserSettings(int Timeout, const QDataStream &UserSettings);
    CmdChangeUserSettings();
    ~CmdChangeUserSettings();
    virtual QString GetName() const;
    QByteArray const& GetUserSettings() const;
private:
    CmdChangeUserSettings(const CmdChangeUserSettings &);                       ///< Not implemented.
    const CmdChangeUserSettings & operator = (const CmdChangeUserSettings &);   ///< Not implemented.
private:
    QByteArray      m_UserSettings;     ///< User settings.
}; // end class CmdChangeUserSettings

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdChangeUserSettings &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_UserSettings;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdChangeUserSettings &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_UserSettings;
    return Stream;
}
} // end namespace MsgClasses

#endif // MSGCLASSES_CMDCHANGEUSERSETTINGS_H

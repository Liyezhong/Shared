/****************************************************************************/
/*! \file CmdChangeUserSettings.cpp
 *
 *  \brief CmdChangeUserSettings command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.03.2010
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

#include "TestStubDataManager/Containers/UserSettings/Commands/Include/CmdChangeUserSettings.h"
#include <QDebug>
namespace MsgClasses {

QString CmdChangeUserSettings::NAME = "MsgClasses::CmdChangeUserSettings";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout         Timeout for command.
 * \param[in]   UserSettings    User settings
 */
/****************************************************************************/
CmdChangeUserSettings::CmdChangeUserSettings(int Timeout, const QDataStream &UserSettings) :
    Command(Timeout),
    m_UserSettings(UserSettings.device()->readAll())
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdChangeUserSettings::~CmdChangeUserSettings()
{
}

CmdChangeUserSettings::CmdChangeUserSettings():Command(0)
{

}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdChangeUserSettings::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns User Settings
 *
 *  \return  user settings as list of strings
 */
/****************************************************************************/
QByteArray const& CmdChangeUserSettings::GetUserSettings() const
{
    return m_UserSettings;
}

} // end namespace MsgClasses

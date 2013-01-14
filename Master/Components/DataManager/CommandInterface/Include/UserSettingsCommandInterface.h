/****************************************************************************/
/*! \file UserSettingsCommandInterfaceBase.h
 *
 *  \brief Command Interface definition
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-30
 *  $Author:    $ N.Kamath
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
#ifndef USERSETTINGSCOMMANDINTERFACE_H
#define USERSETTINGSCOMMANDINTERFACE_H

#include "CommandInterfaceBase.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdChangeUserSettings.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdAlarmToneTest.h"

namespace DataManager {
/****************************************************************************/
/**
 * \brief  Settings container command interface definition
 */
/****************************************************************************/
class CUserSettingsCommandInterface:public CCommandInterfaceBase
{
    Q_OBJECT
public:
    CUserSettingsCommandInterface(CDataManagerBase *p_DataManager, Threads::MasterThreadController *p_MasterThreadController,CDataContainerCollectionBase *p_DataContainer);

private:
    void RegisterCommands();
    void SettingsUpdateHandler(Global::tRefType, const MsgClasses::CmdChangeUserSettings &Cmd, Threads::CommandChannel &AckCommandChannel);
    void AlarmTestToneHandler(Global::tRefType Ref, const MsgClasses::CmdAlarmToneTest &Cmd, Threads::CommandChannel &AckCommandChannel);
signals:
    void UserSettingsChanged();
};

}// End of namespace DataManager
#endif // USERSETTINGSCOMMANDINTERFACE_H

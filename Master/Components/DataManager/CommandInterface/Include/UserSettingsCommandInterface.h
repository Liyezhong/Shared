/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/CommandInterface/Include/UserSettingsCommandInterface.h
 *
 *  \brief Command Interface definition
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-30
 *  $Author:    $ N.Kamath, Ramya GJ
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

#include "DataManager/CommandInterface/Include/CommandInterfaceBase.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdChangeUserSettings.h"
#include "DataManager/Containers/UserSettings/Commands/Include/CmdAlarmToneTest.h"
#include "Global/Include/EventTranslator.h"

namespace DataManager {
/****************************************************************************/
/**
 * \brief  Settings container command interface definition
 */
/****************************************************************************/
class CUserSettingsCommandInterface:public CCommandInterfaceBase
{
    Q_OBJECT
    friend class TestCmdInterface;
public:
    CUserSettingsCommandInterface(CDataManagerBase *p_DataManager, Threads::MasterThreadController *p_MasterThreadController,CDataContainerCollectionBase *p_DataContainer);

private:
    bool m_ConnToOtherProcess;//!< Flag for connection to other process
    void RegisterCommands();
    void SettingsUpdateHandler(Global::tRefType, const MsgClasses::CmdChangeUserSettings &Cmd, Threads::CommandChannel &AckCommandChannel);
    void AlarmTestToneHandler(Global::tRefType Ref, const MsgClasses::CmdAlarmToneTest &Cmd, Threads::CommandChannel &AckCommandChannel);
public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ConnectedToOtherProcess(bool Connected);

signals:
    /****************************************************************************/
    /**
     * \brief This signal is emitted when User modifies the Settings from GUI
     * \iparam LanguageChanged = true if language changed
     */
    /****************************************************************************/
    void UserSettingsChanged(const bool LanguageChanged);
    /****************************************************************************/
    /**
     * \brief This signal is emitted when User clicks on test tone button
     *
     * \iparam AlarmTypeFlag = Alarm Type( Error/Note)
     * \iparam Volume = Volume of tone
     * \iparam AlarmNumber = Tone Number
     */
    /****************************************************************************/
    void PlayTestTone(bool AlarmTypeFlag, quint8 Volume, quint8 AlarmNumber);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when User changes DI water type to Tap water and
     *              vice versa in Bathlayout screen.
     * \iparam TapWaterType = true if Water Station type is changed to Tap Water
     */
    /****************************************************************************/
    void UserSettingsWaterTypeChanged(const bool TapWaterType);
    /****************************************************************************/
    /**
     * \brief This signal is emitted when User resets the life time of ActiveCarbonFilter
     *
     */
    /****************************************************************************/
    void ResetActiveCarbonFilterLifeTime();
};

}// End of namespace DataManager
#endif // USERSETTINGSCOMMANDINTERFACE_H

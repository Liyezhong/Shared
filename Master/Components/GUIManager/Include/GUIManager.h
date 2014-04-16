/****************************************************************************/
/*! \file GUIManager.h
 *
 *  \brief Definition file for class GUIManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-11-07
 *  $Author:    $ Raju
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
#ifndef EXPORT_GUIManager_H
#define EXPORT_GUIManager_H

#include "ExternalProcessController/Include/ExternalProcessController.h"
#include "DataManager/Containers/ExportConfiguration/Commands/Include/CmdDataExport.h"
#include "Threads/Include/MasterThreadController.h"
#include "PasswordManager/Include/PasswordManager.h"
#include "DataManager/Helper/Include/XmlConfigFilePasswords.h"
#include <DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h>
#include <DataManager/Containers/UserSettings/Include/UserSettingsInterface.h>
#include <Global/Include/Commands/CmdDateAndTime.h>
#include <NetCommands/Include/CmdChangeUserLevel.h>
#include <NetCommands/Include/CmdChangeAdminPassword.h>
#include <NetCommands/Include/CmdGuiEventReport.h>
#include <NetCommands/Include/CmdStartServiceSW.h>
#include <NetCommands/Include/CmdLanguageFile.h>
#include <Global/Include/UITranslator.h>
#include <Threads/Include/CommandChannel.h>
#include <NetCommands/Include/CmdExecutionStateChanged.h>
#include <NetCommands/Include/CmdChangeUserLevelReply.h>
#include <NetCommands/Include/CmdChangeAdminPasswordReply.h>

namespace GUIManager {

const quint32 GUI_COMMANDS_TIMEOUT =   15000; //!< time out gui commands
#if defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_M_ARM)
const quint32 CONFIG_FILE_TIMER_INTERVAL    = 5000; //!< 10[s] for target
#else
const quint32 CONFIG_FILE_TIMER_INTERVAL    = 3000;  //!< 3[s] for desktop
#endif

const quint32 RAISE_EVENTREPORT_TIMER_INTERVAL    = 1000; //!< 2[s]

/****************************************************************************/
/**
 * \brief This is a Export Controller. It handles communication
 *        with Main and Export process.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CGUIManager: public QObject
{
    friend class TestGUIManager;

    Q_OBJECT
private:
    PasswordManager::CPasswordManager           m_PasswordManager;                      ///< The password manager.    
    Threads::MasterThreadController&            m_MasterThreadControllerRef;            ///< The master thread controller.
    DataManager::CDeviceConfigurationInterface* mp_DeviceConfigInterface;               ///< The device config interface.
    DataManager::CUserSettingsInterface*        mp_SettingsInterface;                   ///< The user settings interface
    QString                                     m_LanguageFileName;                     ///< current language file name.

protected:
    Threads::CommandChannel                     m_CommandChannelGui;                    ///< Command channel for Gui thread controller.

    Q_DISABLE_COPY(CGUIManager) ///< Disable copy constructor

    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    CGUIManager(); ///< not implemented

    /****************************************************************************/
    /**
     * \brief store settings interface
     * \iparam p_SettingsInterface = settings interface pointer.
     */
    /****************************************************************************/
    void SetUserConfigInterface(DataManager::CUserSettingsInterface* p_SettingsInterface)
    {
        mp_SettingsInterface = p_SettingsInterface;
    }

    /****************************************************************************/
    /**
     * \brief store device config interface
     * \iparam p_DeviceInterface = device config interface pointer.
     */
    /****************************************************************************/
    void SetDeviceConfigInterface(DataManager::CDeviceConfigurationInterface* p_DeviceInterface)
    {
        mp_DeviceConfigInterface = p_DeviceInterface;
    }


public:
    CGUIManager(Threads::MasterThreadController &MasterThreadControllerRef, QString LanguageFileName);

    virtual ~CGUIManager();

    /****************************************************************************/
    /**
     * \brief This function initializes and Send XML files to GUI when its connected to Main
     */
    /****************************************************************************/
    void Initialize();

    /****************************************************************************/
    /**
     * \brief This function registers all GUI commands  which comes from GUI
     */
    /****************************************************************************/
    void RegisterCommonCommands();

    /****************************************************************************/
    /**
     * \brief Send the language file to GUI
     *
     *  \iparam FileName - Name of the file
     *
     * \return On successful (True) or not (false)
     */
    /****************************************************************************/
    bool SendLanguageFileToGUI(QString FileName);

    /****************************************************************************/
    /**
     * \brief Update the GUI language files (qm files from translation directory)
     * \return updated status
     */
    /****************************************************************************/
    bool UpdateSupportedGUILanguages();

    /****************************************************************************/
    /**
     * \brief Set new date and time offset.
     *
     * \iparam   Ref                 Command reference.
     * \iparam   Cmd                 Command.
     * \iparam   AckCommandChannel   Command channel for acknowledges.
     */
    /****************************************************************************/
    void SetDateTime(Global::tRefType Ref, const Global::CmdDateAndTime &Cmd, Threads::CommandChannel &AckCommandChannel);

    /************************************************************************************/
    /*!
     *  \brief Change of the user level
     *  \iparam Ref = Command reference
     *  \iparam Cmd = The command
     *  \iparam AckCommandChannel = command channel
     */
    /************************************************************************************/
    void ChangeUserLevelHandler(Global::tRefType Ref, const NetCommands::CmdChangeUserLevel &Cmd,
                                Threads::CommandChannel &AckCommandChannel);

    /************************************************************************************/
    /*!
     *  \brief Change of the password for the Admin user
     *  \iparam Ref = Command reference
     *  \iparam Cmd = The command
     *  \iparam AckCommandChannel = command channel
     */
    /************************************************************************************/
    void ChangeAdminPasswordHandler(Global::tRefType Ref, const NetCommands::CmdChangeAdminPassword &Cmd,
                                    Threads::CommandChannel &AckCommandChannel);

    /****************************************************************************/
    /**
     * \brief Used for the gui events to log in the log file
     *
     * \iparam Ref - Refernce of the command argument
     * \iparam Cmd - Command class
     * \iparam AckCommandChannel - Channel class for the command
     *
     */
    /****************************************************************************/
    void GuiEventReportHandler(Global::tRefType Ref, const NetCommands::CmdGuiEventReport &Cmd,
                               Threads::CommandChannel &AckCommandChannel);


    /************************************************************************************/
    /*!
     *  \brief  Cmd Handler for CmdStartService SW
     *  \iparam Ref = Command reference
     *  \iparam Cmd = The command
     *  \iparam AckCommandChannel = command channel=>GUI
     */
    /************************************************************************************/
    void CmdStartServiceSWHandler(Global::tRefType Ref, const NetCommands::CmdStartServiceSW &Cmd, Threads::CommandChannel &AckCommandChannel);

    /************************************************************************************/
    /*!
     *  \brief Returns the command channel
     *
     *  \return Command channel
     */
    /************************************************************************************/
    Threads::CommandChannel& GetCommandChannel()
    {
        return m_CommandChannelGui;
    }

private slots:
    /************************************************************************************/
    /*!
     *  \brief This slot intiates the task to send Configuration files to GUI
     */
    /************************************************************************************/
    void InitiateSendingConfigurationFiles();

public slots:
    /************************************************************************************/
    /*!
     *  \brief This slot is called to make GUI to stabilize and shoots a timer
     *           to send Configuration files to GUI
     */
    /************************************************************************************/
    void WaitForGUIToStabilize();

    /****************************************************************************/
    /**
     * \brief Informs GUI to display/close wait dialog
     *
     * \iparam Display - true indicate display dialog, false indicates close dialog
     * \iparam WaitDialogText = Wait dialog text
     */
    /****************************************************************************/
    void ShowWaitDialog(bool Display, Global::WaitDialogText_t WaitDialogText);
    /****************************************************************************/
    /**
     * \brief Informs GUI to display/close wait dialog. Used when GUI is connected
     *        to Main Software
     */
    /****************************************************************************/
    void ShowWaitDialog();

    /****************************************************************************/
    /**
     * \brief Hides wait dialog
     */
    /****************************************************************************/
    void HideWaitDialog();

signals:
    /************************************************************************************/
    /*!
     *  \brief This signal emits whenever there is a change GUI time
     */
    /************************************************************************************/
    void TimeUpdated();

    /************************************************************************************/
    /*!
     *  \brief This signal sends the configuration files to GUI
     */
    /************************************************************************************/
    void StartSendingConfigurationFiles();
};
}

#endif // EXPORT_GUIManager_H

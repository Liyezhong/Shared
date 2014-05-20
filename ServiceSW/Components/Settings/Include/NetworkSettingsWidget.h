/****************************************************************************/
/*! \file NetworkSettingsWidget.h
 *
 *  \brief NetworkSettingsWidget definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-06-12
 *   $Author:  $ Soumya. D
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

#ifndef SETTINGS_NETWORKSETTINGSWIDGET_H
#define SETTINGS_NETWORKSETTINGSWIDGET_H

#include "../Include/PlatformServiceDefines.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include <KeyBoard/Include/KeyBoard.h>
#include <QRegExpValidator>
#include <MainMenu/Include/BaseTable.h>
#include <QStandardItemModel>

namespace Settings {

namespace Ui {
    class CNetworkSettingsWidget;
}

//!< Button Type -UserName, Password, IP Address or Port
typedef enum {
    USERNAME_BTN_CLICKED = 1,
    PASSWORD_BTN_CLICKED,
    IP_ADDRESS_BTN_CLICKED,
    PORT_BTN_CLICKED,
    INVALID
}ButtonType_t;

/****************************************************************************/
/**
 * \brief This class contains the proxy settings for remote care
 */
/****************************************************************************/
class CNetworkSettingsWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

public:
    explicit CNetworkSettingsWidget(QWidget *p_Parent = 0);
    ~CNetworkSettingsWidget();
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);
    void SetSaveButtonStatus();

    /****************************************************************************/
    /*!
     *  \brief  To add data item to the table
     *  \iparam TestName = Test name
     */
    /****************************************************************************/
    void AddItem(QString TestName);

protected:
    void changeEvent(QEvent *p_Event);

private:
    Ui::CNetworkSettingsWidget *mp_Ui;                      //!< User interface
    MainMenu::CMainWindow *mp_MainWindow;                   //!< Pointer to MainWindow  
    bool m_ProcessRunning;                                  //!< Process running state    
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    KeyBoard::CKeyBoard *mp_KeyBoardWidget;                 //!< Reference to Keyboard widget
    ButtonType_t m_ButtonType;                              //!< Button Type
    QString m_Password;                                     //!< String to hold Password
    MainMenu::CMessageDlg *mp_MessageDlg;                   //!< Message dialog object.
    QString m_IPAddress;                                    //!< To store the Ip address
    MainMenu::CBaseTable *mp_TableWidget;                   //!< Base Table widget
    QStandardItemModel m_Model;                             //!< Model for the table

    void ConnectKeyBoardSignalSlots();
    void DisconnectKeyBoardSignalSlots();    

signals:
    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted when the ServiceParameter is changed.
     *
     *  \iparam ServiceParameters = ServiceParameters reference.
     */
    /****************************************************************************/
    void SaveIPAddress(QString);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted when validation of key board is done.
     *
     *  \iparam Value = Flag value.
     */
    /****************************************************************************/
    void KeyBoardStringValidationComplete(const bool Value);

    /****************************************************************************/
    /*!
     *  \brief  This signal is emitted for firmware download
     */
    /****************************************************************************/
    void DownloadFirmware();

private slots:
    void OnProxyIPAddress();
    void OnSave();
    void OnOkClicked(QString EnteredText);
    void OnESCClicked();
    void KeyBoardValidateEnteredString(QString);
    void RetranslateUI();

public slots:
    void OnDownloadFirmware();
    void UpdateIpAddress(QString IpAddress);
    void SetInformwationText(QString Text, QString Color);
    void SetNetworkSettingsResult(PlatformService::NetworkSettings_t, bool);
    void reset();

};

} // end namespace Settings

#endif // SETTINGS_NETWORKSETTINGSWIDGET_H

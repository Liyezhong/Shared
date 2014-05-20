/****************************************************************************/
/*! \file USBKeyValidator.cpp
 *
 *  \brief This class provides implementation for Service/Manuacturing key
 *  authentication.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-04-09
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

#include "../Include/PlatformServiceEventCodes.h"
#include "ServiceKeyValidator/Include/USBKeyValidator.h"
#include <QApplication>

namespace ServiceKeyValidator {

const QString REGEXP_NUMERIC_VALIDATOR = "^[0-9]{1,4}$"; //!< Reg expression for the validator

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam StartUp = Startup object
 */
/****************************************************************************/
CUSBKeyValidator::CUSBKeyValidator(QString DeviceName) :
    m_DeviceName(DeviceName),
    m_EnteredPin(""),
    m_CurrentUser(""),
    m_SystemLogOff(false),
    m_ManufacturingKey("manufacturing")
{    
    mp_KeyBoardWidget = new KeyBoard::CKeyBoard(KeyBoard::SIZE_1, KeyBoard::QWERTY_KEYBOARD);
    mp_MessageDlg = new MainMenu::CMessageDlg(mp_KeyBoardWidget);
    mp_MessageDlg->setModal(true);
    mp_KeyBoardWidget->setModal(true);
    mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Core::CUSBKeyValidator", "Enter Pin", 0, QApplication::UnicodeUTF8));
    mp_KeyBoardWidget->SetPasswordMode(true);
    mp_KeyBoardWidget->SetValidation(true);
    mp_KeyBoardWidget->SetMinCharLength(4);
    mp_KeyBoardWidget->SetMaxCharLength(4);
    mp_KeyBoardWidget->DisplayNumericKeyBoard();
    mp_KeyBoardWidget->SetLineEditValidatorExpression(REGEXP_NUMERIC_VALIDATOR);
    mp_KeyBoardWidget->show();
    ConnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam StartUp = Startup object
 *  \iparam SystemLogOff = Sets SystemLogOff variable
 */
/****************************************************************************/
CUSBKeyValidator::CUSBKeyValidator(QString DeviceName, bool SystemLogOff) :
    m_DeviceName(DeviceName),
    m_EnteredPin(""),
    m_CurrentUser(""),
    m_SystemLogOff(SystemLogOff),
    m_ManufacturingKey("manufacturing")
{    
    mp_KeyBoardWidget = new KeyBoard::CKeyBoard(KeyBoard::SIZE_1, KeyBoard::QWERTY_KEYBOARD);
    mp_MessageDlg = new MainMenu::CMessageDlg(mp_KeyBoardWidget);
    mp_MessageDlg->setModal(true);
    mp_KeyBoardWidget->setModal(true);
    mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Core::CUSBKeyValidator", "Enter Pin", 0, QApplication::UnicodeUTF8));
    mp_KeyBoardWidget->SetPasswordMode(true);
    mp_KeyBoardWidget->SetValidation(true);
    mp_KeyBoardWidget->SetMinCharLength(4);
    mp_KeyBoardWidget->SetMaxCharLength(4);
    mp_KeyBoardWidget->DisplayNumericKeyBoard();
    mp_KeyBoardWidget->SetLineEditValidatorExpression(REGEXP_NUMERIC_VALIDATOR);
    mp_KeyBoardWidget->show();
    ConnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CUSBKeyValidator::~CUSBKeyValidator()
{
    try {
    }
    catch (...) {
        // To please Lint
    }
}

/****************************************************************************/
/*!
 *  \brief Slot called to set the device name
 *  \iparam DeviceName = Device name
 */
/****************************************************************************/
void CUSBKeyValidator::SetDeviceName(QString DeviceName) {
    m_DeviceName = DeviceName;
}

/****************************************************************************/
/*!
 *  \brief This function is called when OK is clicked
 *  \iparam EnteredString = Stores line edit string
 */
/****************************************************************************/
void CUSBKeyValidator::OnOkClicked(QString EnteredString)
{
    SetEnteredPin(EnteredString);

    PasswordManager::CServicePassword m_ServicePassword(m_EnteredPin, GetDeviceName());
    QString UserMode("");

    /* Code for debug purposes only. This section needs to be removed for release*/
    if(EnteredString.startsWith("1406"))
    {
        emit SetSoftwareMode(PlatformService::SERVICE_MODE, QString("1406"));
        DisconnectKeyBoardSignalSlots();
        return;
    }


    /* Code for debug purposes only. This section needs to be removed for release*/
    if(EnteredString.startsWith("1407"))
    {
        emit SetSoftwareMode(PlatformService::MANUFACTURING_MODE, QString("1407"));
        DisconnectKeyBoardSignalSlots();
        return;
    }

    QString MessageTitle, MessageInfo;
    /* Service Key authentication */
    if (m_ServicePassword.ValidateAuthentication()) {
        UserMode =  m_ServicePassword.ReadServiceID();
        if(UserMode.startsWith(m_ManufacturingKey)) {
            emit SetSoftwareMode(PlatformService::MANUFACTURING_MODE, UserMode);
        }
        else {
            emit SetSoftwareMode(PlatformService::SERVICE_MODE, UserMode);
        }
        DisconnectKeyBoardSignalSlots();
    }
    else {
        PasswordManager::ServiceUserAuthentication AuthenticationError = m_ServicePassword.GetAuthenticationError();

        switch (AuthenticationError) {
        case PasswordManager::NO_SERVICE_KEY_FILE:
            Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_NO_KEY_FILE);
            MessageTitle = QString("Service Key File");
            MessageInfo = QString("Service Key file is not present, Cannot boot into Service/Manufacturing Software.");
            break;
        case PasswordManager::NO_DEVICE_TAG:
            Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_DEVICE_NAME_NOT_FOUND);
            MessageTitle = QString("Device Name");
            MessageInfo = QString("Device name is not present, Cannot boot into Service/Manufacturing Software.");
            break;
        case PasswordManager::BASIC_TAG_NOT_MATCHING:
            Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_BASIC_TAG_NOT_MATCHING);
            MessageTitle = QString("Invalid Tag");
            MessageInfo = QString("Service user is not trained for the device.");
            break;
        case PasswordManager::DATE_EXPIRED:
            Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_KEY_DATE_EXPIRED);
            MessageTitle = QString("Service Key Expired");
            MessageInfo = QString("Date expired, Cannot boot into Service/Manufacturing Software.");
            break;
        default:
            Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_INVALID_PIN);
            MessageTitle = QString("Invalid Pin");
            MessageInfo = QString("Invalid Pin, Cannot boot into Service/Manufacturing Software.");
            break;
        }

        mp_KeyBoardWidget->setVisible(false);
        mp_MessageDlg->SetTitle(MessageTitle);
        mp_MessageDlg->SetText(MessageInfo);
        mp_MessageDlg->SetButtonText(1, QApplication::translate("Core::CUSBKeyValidator", "Ok", 0, QApplication::UnicodeUTF8));
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        mp_MessageDlg->Show();

        if(mp_MessageDlg->exec()) {
            mp_KeyBoardWidget->setVisible(true);
            QTimer::singleShot(100, this, SLOT(DisplayKeyBoard()));
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Slot called to display Keyboard
 */
/****************************************************************************/
void CUSBKeyValidator::DisplayKeyBoard()
{
    m_EnteredPin = QString("");
    mp_KeyBoardWidget->SetPasswordMode(true);
    mp_KeyBoardWidget->SetValidation(true);
    mp_KeyBoardWidget->SetMinCharLength(4);
    mp_KeyBoardWidget->SetMaxCharLength(4);
    mp_KeyBoardWidget->DisplayNumericKeyBoard();
    mp_KeyBoardWidget->SetLineEditValidatorExpression(REGEXP_NUMERIC_VALIDATOR);
    mp_KeyBoardWidget->show();
    //    ConnectKeyBoardSignalSlots();
}

void CUSBKeyValidator::HideKeyBoard()
{
    if (mp_KeyBoardWidget)
        mp_KeyBoardWidget->HideKeyBoard();
}

/****************************************************************************/
/*!
 *  \brief This function hides the keyboard when Esc is clicked
 */
/****************************************************************************/
void CUSBKeyValidator::OnESCClicked()
{
    // Disconnect signals and slots connected to keyboard.
    //    DisconnectKeyBoardSignalSlots();
    //    mp_KeyBoardWidget->hide();
    mp_KeyBoardWidget->SetPasswordMode(true);
    mp_KeyBoardWidget->SetValidation(true);
    mp_KeyBoardWidget->SetMinCharLength(4);
    mp_KeyBoardWidget->SetMaxCharLength(4);
    mp_KeyBoardWidget->DisplayNumericKeyBoard();
    mp_KeyBoardWidget->SetLineEditValidatorExpression(REGEXP_NUMERIC_VALIDATOR);
    mp_KeyBoardWidget->show();
    ConnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief Connects signals and slots of keyboard.
 */
/****************************************************************************/
void CUSBKeyValidator::ConnectKeyBoardSignalSlots()
{
    // Connect signals and slots to keyboard.
    CONNECTSIGNALSLOT(mp_KeyBoardWidget, OkButtonClicked(QString), this, OnOkClicked(QString));
    CONNECTSIGNALSLOT(mp_KeyBoardWidget, EscButtonClicked(), this, OnESCClicked());
}

/****************************************************************************/
/*!
 *  \brief Disconnects signals and slots of keyboard.
 */
/****************************************************************************/
void CUSBKeyValidator::DisconnectKeyBoardSignalSlots()
{
    // Disconnect signals and slots connected to keyboard.
    (void) disconnect(mp_KeyBoardWidget, SIGNAL(OkButtonClicked(QString)), this, SLOT(OnOkClicked(QString)));
    (void) disconnect(mp_KeyBoardWidget, SIGNAL(EscButtonClicked()), this, SLOT(OnESCClicked()));

}

}   // end of namespace ServiceKeyValidator


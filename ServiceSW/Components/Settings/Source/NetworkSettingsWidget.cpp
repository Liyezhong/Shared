/****************************************************************************/
/*! \file NetworkSettingsWidget.cpp
 *
 *  \brief Implementation file for class CNetworkSettingsWidget.
 *
 *  \b Description:
 *          This class implements widgets for displaying Network Settings and provides
 *          the user interfaces to enter network settings parameters for remote care.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-26
 *   $Author:  $ M.Scherer, C.Adaragunchi, Soumya. D
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
#include "Settings/Include/NetworkSettingsWidget.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/MessageDlg.h"
#include "ui_NetworkSettingsWidget.h"
#include <QCheckBox>
#include <QDebug>

namespace Settings {

const QString REGEXP_NUMERIC_VALIDATOR  = "^[0-9]{1,5}$"; //!< Reg expression for the validator
const QString IPADDRESS_INPUT_MASK      = "000.000.000.000; "; //!< Mask for the Ip address
const qint32 MIN_PROXY_IP_PORT          = 1; ///< Minimum value of IP Port
const qint32 MAX_PROXY_IP_PORT          = 65535; ///< Maximum value of IP Port
const qint32 MIN_IP_ADDRESS_NUMBER      = 0;    ///< Minimum value of IP Address Number
const qint32 MAX_IP_ADDRESS_NUMBER      = 255;  ///< Maximum value of IP Address Number
const qint32 MAX_CHAR_LENGTH            = 16;   ///< Maximum character length
const int SET_FIXED_TABLE_WIDTH         = 320;  ///< Set table width
const int SET_FIXED_TABLE_HEIGHT        = 170;  ///< Set table height
const int VISIBLE_TABLE_ROWS            = 3;    ///< Visible table rows

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CNetworkSettingsWidget::CNetworkSettingsWidget(QWidget *p_Parent)
    : MainMenu::CPanelFrame(p_Parent),
      mp_Ui(new Ui::CNetworkSettingsWidget),
      mp_MainWindow(NULL),
      m_ProcessRunning(false),
      m_CurrentUserRole(MainMenu::CMainWindow::Operator),
      mp_KeyBoardWidget(NULL),
      m_ButtonType(INVALID),
      m_Password(""),
      mp_MessageDlg(NULL)
{
    mp_Ui->setupUi(GetContentFrame());
    m_strDownloading = QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                               "Downloading...", 0, QApplication::UnicodeUTF8);
    m_strWaitTitle   = QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                               "Please Wait", 0, QApplication::UnicodeUTF8);
    SetPanelTitle(QApplication::translate("ServiceUpdates::CNetworkSettingsWidget", "Network", 0,
                                          QApplication::UnicodeUTF8));

    mp_KeyBoardWidget = new KeyBoard::CKeyBoard(KeyBoard::SIZE_1, KeyBoard::QWERTY_KEYBOARD);
    mp_KeyBoardWidget->setModal(true);

    mp_WaitDlg    = new MainMenu::CWaitDialog(this);
    mp_WaitDlg->setModal(true);

    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->resize(SET_FIXED_TABLE_WIDTH, SET_FIXED_TABLE_HEIGHT);
    mp_TableWidget->SetVisibleRows(VISIBLE_TABLE_ROWS);


    AddItem(QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                         "Host reachable", 0, QApplication::UnicodeUTF8));
    AddItem(QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                         "Service available", 0, QApplication::UnicodeUTF8));
    AddItem(QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                         "Access rights", 0, QApplication::UnicodeUTF8));

    mp_TableWidget->horizontalHeader()->resizeSection(0, 230);       // 0 => Index  450 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(1, 60);        // 1 => Index  50 => Size
    mp_TableWidget->horizontalHeader()->hide();
    mp_TableWidget->setModel(&m_Model);

    mp_TableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    mp_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


    mp_Ui->tableWidget->setMinimumSize(mp_TableWidget->width(), mp_TableWidget->height());
    mp_Ui->tableWidget->SetContent(mp_TableWidget);

    mp_Ui->firmwareCheckBoxLabel->setPixmap(QPixmap(QString::fromUtf8(":/Large/CheckBoxLarge/CheckBox-enabled-large.png")));

    CONNECTSIGNALSLOTGUI(mp_Ui->proxyIpAddressButton, clicked(), this, OnProxyIPAddress());
    CONNECTSIGNALSLOTGUI(mp_Ui->saveButton, clicked(), this, OnSave());
    CONNECTSIGNALSLOTGUI(mp_Ui->downloadFirmwareBtn, clicked(), this, OnDownloadFirmware());

    mp_MessageDlg = new MainMenu::CMessageDlg(this);
    mp_MessageDlg->setModal(true);
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CNetworkSettingsWidget::~CNetworkSettingsWidget()
{
    try {
        if (mp_WaitDlg) {
            delete mp_WaitDlg;
        }
        if (mp_MessageDlg) {
            delete mp_MessageDlg;
        }
        if(mp_KeyBoardWidget) {
            delete mp_KeyBoardWidget;
        }
        delete mp_Ui;
    }
    catch (...) {
        // to please Lint
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CNetworkSettingsWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
    case QEvent::LanguageChange:
        //Commented below line because it calls retranslateUi of ui_NetworkSettingsWidget.h
        // which sets the buttons(UserNameButton, Password, IPAddress and IP Port) text to default values.
       // mp_Ui->retranslateUi(this);      
        RetranslateUI();
        break;
    default:
        break;
    }
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *
 *  \iparam p_MainWindow = MainWindow object.
 */
/****************************************************************************/
void CNetworkSettingsWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;   
}

/****************************************************************************/
/*!
 *  \brief  To add data item to the table
 *  \iparam TestName = Test name
 */
/****************************************************************************/
void CNetworkSettingsWidget::AddItem(QString TestName)
{
    QList<QStandardItem *> ItemList;
    ItemList << new QStandardItem(TestName);

    QPixmap SetPixmap;
    QPixmap PixMap(QString(":/Large/CheckBoxLarge/CheckBox-enabled-large.png"));
    if (!PixMap.isNull())
        SetPixmap = (PixMap.scaled(QSize(45,45),Qt::KeepAspectRatio, Qt::FastTransformation));

    QStandardItem *item = new QStandardItem;
    item->setData(SetPixmap, (int) Qt::DecorationRole);
    ItemList << item;
    m_Model.appendRow(ItemList);
}

/****************************************************************************/
/*!
 *  \brief Updated Ip address
 *  \iparam IpAddress = Ip address
 */
/****************************************************************************/
void CNetworkSettingsWidget::UpdateIpAddress(QString IpAddress)
{
    mp_Ui->proxyIpAddressButton->setText(IpAddress);
    mp_Ui->proxyIpAddressButton->setEnabled(true);
    mp_Ui->saveButton->setEnabled(true);
}

/****************************************************************************/
/*!
 *  \brief Shows the on screen keyboard to Enter Proxy IP Address
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnProxyIPAddress()
{
    m_ButtonType = IP_ADDRESS_BTN_CLICKED;
    if (mp_KeyBoardWidget) {
        mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                                  "Enter Proxy IP Address", 0, QApplication::UnicodeUTF8));
        mp_KeyBoardWidget->SetPasswordMode(false);
        mp_KeyBoardWidget->SetLineEditContent(mp_Ui->proxyIpAddressButton->text());
        mp_KeyBoardWidget->SetValidation(true);
        mp_KeyBoardWidget->DisplayNumericKeyBoard();
        // enable the input mask so that user can easily fill the ip address
        mp_KeyBoardWidget->SetLineEditInputMask(IPADDRESS_INPUT_MASK);
        mp_KeyBoardWidget->show();
    }
    // Connect signals and slots to keyboard.
    ConnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief This slot is called when ESC button on Keyboard is pressed.
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnESCClicked()
{
    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Ok button on KeyBoard is pressed.
 *
 *  \iparam EnteredText = Entered String from keyboard.
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnOkClicked(QString EnteredText)
{    
    switch(m_ButtonType) {

    case IP_ADDRESS_BTN_CLICKED:
        if (m_IPAddress.length() == 0) {
           m_IPAddress = EnteredText;
        }
        mp_Ui->proxyIpAddressButton->setText(m_IPAddress);
        break;

    default:
        break;
    }
    // Disconnect signals and slots connected to keyboard.
    DisconnectKeyBoardSignalSlots();
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Save button is clicked.
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnSave()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IPADDRESS_UPDATED);
    emit SaveIPAddress(mp_Ui->proxyIpAddressButton->text());
}

/****************************************************************************/
/*!
 *  \brief Connects signals and slots of keyboard.
 */
/****************************************************************************/
void CNetworkSettingsWidget::ConnectKeyBoardSignalSlots()
{
    // Connect signals and slots to keyboard.
    CONNECTSIGNALSLOTGUI(mp_KeyBoardWidget, OkButtonClicked(QString), this, OnOkClicked(QString));
    CONNECTSIGNALSLOTGUI(mp_KeyBoardWidget, EscButtonClicked(), this, OnESCClicked());
    CONNECTSIGNALSLOTGUI(mp_KeyBoardWidget, ValidateEnteredString(QString),
                         this, KeyBoardValidateEnteredString(QString));
    CONNECTSIGNALSLOTGUI(this, KeyBoardStringValidationComplete(const bool), mp_KeyBoardWidget,
                      OnValidationComplete(const bool));
}

/****************************************************************************/
/*!
 *  \brief Disconnects signals and slots of keyboard.
 */
/****************************************************************************/
void CNetworkSettingsWidget::DisconnectKeyBoardSignalSlots()
{
    // Disconnect signals and slots connected to keyboard.
    (void) disconnect(mp_KeyBoardWidget, SIGNAL(OkButtonClicked(QString)),
                      this, SLOT(OnOkClicked(QString)));
    (void) disconnect(mp_KeyBoardWidget, SIGNAL(EscButtonClicked()),
                      this, SLOT(OnESCClicked()));
    (void) disconnect(mp_KeyBoardWidget, SIGNAL(ValidateEnteredString(QString)),
                      this, SLOT(KeyBoardValidateEnteredString(QString)));
    (void) disconnect(this, SIGNAL(KeyBoardStringValidationComplete(const bool)), mp_KeyBoardWidget,
                      SLOT(OnValidationComplete(const bool)));
}

/****************************************************************************/
/*!
 *  \brief Validates the entered string in the keyboard
 *
 *  \iparam Value = Entered string
 */
/****************************************************************************/
void CNetworkSettingsWidget::KeyBoardValidateEnteredString(QString Value)
{    
    QString EnteredString("");
    bool IsIPAddressValid = false;
    int IPAddressNotValidNumber = 0;
    EnteredString  = Value;
    m_IPAddress = "";
    if (mp_MessageDlg) {
        delete mp_MessageDlg;
        mp_MessageDlg = NULL;
    }
    mp_MessageDlg = new MainMenu::CMessageDlg(this);
    mp_MessageDlg->SetTitle(QApplication::translate("ServiceUpdates::CNetworkSettingsWidget", "Information Message",
                                                    0, QApplication::UnicodeUTF8));
    mp_MessageDlg->SetIcon(QMessageBox::Information);
    mp_MessageDlg->SetButtonText(1, QApplication::translate("ServiceUpdates::CNetworkSettingsWidget", "Ok",
                                                            0, QApplication::UnicodeUTF8));
    mp_MessageDlg->setModal(true);
    mp_MessageDlg->HideButtons();

    switch (m_ButtonType) {   

    case USERNAME_BTN_CLICKED:
        break;
    case PASSWORD_BTN_CLICKED:
        break;
    case IP_ADDRESS_BTN_CLICKED:
        // verify the proxy address
        for (qint32 SplitCount = 0; SplitCount < EnteredString.split(".").count(); SplitCount++) {
            // iterate the string
            QString AddressNumberString = (EnteredString.split(".").value(SplitCount));
            // remove the empty spaces if any
            if (AddressNumberString.trimmed().compare("") != 0) {
                bool Result = false;
                // convert the value to integer
                qint32 AddressNumber = AddressNumberString.trimmed().toInt(&Result, 10);
                m_IPAddress += AddressNumberString.trimmed();
                if (Result) {
                    // check the ip address number
                    if (AddressNumber >= MIN_IP_ADDRESS_NUMBER &&
                            AddressNumber <= MAX_IP_ADDRESS_NUMBER) {
                        // check how many zeros are there in the Ip address
                        if (AddressNumber == MIN_IP_ADDRESS_NUMBER) {
                            IPAddressNotValidNumber++;
                        }
                    }
                    else {
                        // make the IP number is not a valid number
                        IPAddressNotValidNumber = 4;
                        break;
                    }
                }
            }
            else {
                // increment the count if the Ip number is zero
                IPAddressNotValidNumber++;
                m_IPAddress += "000";
            }
            // append dot
            if ((SplitCount + 1) != EnteredString.split(".").length()) {
                m_IPAddress += ".";
            }
        }

        // first number should not be 0 (0.0.0.0)
        if (IPAddressNotValidNumber != 4) {
            IsIPAddressValid = true;
        }

        if (mp_KeyBoardWidget) {
            // enable the input mask so that user can easily fill the ip address
            mp_KeyBoardWidget->SetLineEditInputMask(IPADDRESS_INPUT_MASK);
        }

        if (!IsIPAddressValid) {
            mp_MessageDlg->SetText
                    (tr("%1 is not a valid IP address.Valid range is from 000.000.000.001 to 255.255.255.255")
                     .arg(Value));
            (void) mp_MessageDlg->exec();
            emit KeyBoardStringValidationComplete(false);
            return;
        }
        break;   
    case PORT_BTN_CLICKED:
        break;
    case INVALID:
        break;
    }
    emit KeyBoardStringValidationComplete(true);
}

/****************************************************************************/
/*!
 *  \brief Sets the Save button status
 *  \iparam p_ServiceGUIConnector = Data connector object
 */
/****************************************************************************/
void CNetworkSettingsWidget::SetSaveButtonStatus()
{    
    if (mp_MainWindow->GetSaMUserMode() == QString("Service")) {
        mp_Ui->proxyIpAddressButton->setEnabled(false);
        mp_Ui->saveButton->setEnabled(false);
        mp_Ui->downloadFirmwareBtn->setEnabled(false);
    }
    else if (mp_MainWindow->GetSaMUserMode() == QString("Manufacturing")) {
        mp_Ui->proxyIpAddressButton->setEnabled(true);
        mp_Ui->saveButton->setEnabled(true);
        mp_Ui->downloadFirmwareBtn->setEnabled(true);
    }
}


/****************************************************************************/
/*!
 *  \brief Called for firmware download
 */
/****************************************************************************/
void CNetworkSettingsWidget::OnDownloadFirmware()
{
    qDebug() << " On download firmware";
    mp_WaitDlg->SetDialogTitle(m_strWaitTitle);
    mp_WaitDlg->SetText(m_strDownloading);
    mp_WaitDlg->HideAbort();
    mp_WaitDlg->show();

    mp_Ui->downloadFirmwareBtn->setEnabled(false);
    emit DownloadFirmware();
}

/****************************************************************************/
/*!
 *  \brief Called for firmware download
 *  \iparam Text = Information text
 *  \iparam Color = color
 */
/****************************************************************************/
void CNetworkSettingsWidget::SetInformationText(QString Text, QString Color)
{
    QString TextFormat = tr("<font color='%1'>%2</font>");
    mp_Ui->instructionLabel->setText(TextFormat.arg(Color, Text));
}

/****************************************************************************/
/*!
 *  \brief Called for firmware download
 *  \iparam NtService = Network service
 *  \iparam Result = Result of the test
 */
/****************************************************************************/
void CNetworkSettingsWidget::SetNetworkSettingsResult(PlatformService::NetworkSettings_t NtService, bool Result)
{
    QPixmap PixMapPass(QString(":/Large/CheckBoxLarge/CheckBox-Checked_large_green.png"));
    QPixmap PixMapFail(QString(":/Large/CheckBoxLarge/CheckBox-Crossed_large_red.png"));
    QPixmap SetPixMap;

    mp_WaitDlg->close();

    switch (NtService) {
    case PlatformService::HOST_REACHABLE:
        if (Result) {
            if (!PixMapPass.isNull()) {
                SetPixMap = (PixMapPass.scaled(45,45,Qt::KeepAspectRatio));
            }
            (void) m_Model.setData(m_Model.index(0, 1), SetPixMap, (int) Qt::DecorationRole);

        } else {
            if (!PixMapFail.isNull()) {
                SetPixMap = (PixMapFail.scaled(45,45,Qt::KeepAspectRatio));
            }
            (void) m_Model.setData(m_Model.index(0, 1), SetPixMap, (int) Qt::DecorationRole);
        }
        break;
    case PlatformService::SERVICE_AVAILABLE:
        if (Result) {
            if (!PixMapPass.isNull()) {
                SetPixMap = (PixMapPass.scaled(45,45,Qt::KeepAspectRatio));
            }
            (void) m_Model.setData(m_Model.index(1, 1), SetPixMap, (int) Qt::DecorationRole);
        } else {
            if (!PixMapFail.isNull()) {
                SetPixMap = (PixMapFail.scaled(45,45,Qt::KeepAspectRatio));
            }
            (void) m_Model.setData(m_Model.index(1, 1), SetPixMap, (int) Qt::DecorationRole);
        }
        break;
    case PlatformService::ACCESS_RIGHTS:
        if (Result) {
            if (!PixMapPass.isNull()) {
                SetPixMap = (PixMapPass.scaled(45,45,Qt::KeepAspectRatio));
            }
            (void) m_Model.setData(m_Model.index(2, 1), SetPixMap, (int) Qt::DecorationRole);
        } else {
            if (!PixMapFail.isNull()) {
                SetPixMap = (PixMapFail.scaled(45,45,Qt::KeepAspectRatio));
            }
            (void) m_Model.setData(m_Model.index(2, 1), SetPixMap, (int) Qt::DecorationRole);
        }
        break;
    case PlatformService::DOWNLOAD_FIRMWARE:
        if (Result) {
            mp_Ui->firmwareCheckBoxLabel->setPixmap(PixMapPass);
        } else {
            mp_Ui->firmwareCheckBoxLabel->setPixmap(PixMapFail);
        }
        break;
    default:
        break;
    }
    mp_Ui->downloadFirmwareBtn->setEnabled(true);
}

/****************************************************************************/
/*!
 *  \brief To reset the check boxes.
 */
/****************************************************************************/
void CNetworkSettingsWidget::reset()
{
    QPixmap SetPixmap;
    QPixmap PixMap(QString(":/Large/CheckBoxLarge/CheckBox-enabled-large.png"));
    if (!PixMap.isNull()) {
        SetPixmap = (PixMap.scaled(45,45,Qt::KeepAspectRatio));
    }
    (void) m_Model.setData(m_Model.index(0, 1), SetPixmap, (int) Qt::DecorationRole);
    (void) m_Model.setData(m_Model.index(1, 1), SetPixmap, (int) Qt::DecorationRole);
    (void) m_Model.setData(m_Model.index(2, 1), SetPixmap, (int) Qt::DecorationRole);
    mp_Ui->firmwareCheckBoxLabel->setPixmap(QPixmap(QString::fromUtf8(":/Large/CheckBoxLarge/CheckBox-enabled-large.png")));
}


/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CNetworkSettingsWidget::RetranslateUI()
{
    SetPanelTitle(QApplication::translate("Settings::CNetworkSettingsWidget",
                                             "Network", 0, QApplication::UnicodeUTF8)); 
    mp_Ui->saveButton->setText(QApplication::translate("Settings::CNetworkSettingsWidget",
                                "Save", 0, QApplication::UnicodeUTF8));

    if (mp_KeyBoardWidget) {
        if (m_ButtonType == IP_ADDRESS_BTN_CLICKED) {
            mp_KeyBoardWidget->SetKeyBoardDialogTitle(QApplication::translate("Settings::CNetworkSettingsWidget",
                                                                              "Enter Proxy IP Address", 0, QApplication::UnicodeUTF8));
        }
    }

    m_Model.clear();
    AddItem(QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                         "Host reachable", 0, QApplication::UnicodeUTF8));
    AddItem(QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                         "Service available", 0, QApplication::UnicodeUTF8));
    AddItem(QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                         "Access rights", 0, QApplication::UnicodeUTF8));

    mp_TableWidget->horizontalHeader()->resizeSection(0, 230);       // 0 => Index  450 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(1, 60);        // 1 => Index  50 => Size

    m_strDownloading = QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                               "Downloading...", 0, QApplication::UnicodeUTF8);
    m_strWaitTitle   = QApplication::translate("ServiceUpdates::CNetworkSettingsWidget",
                                               "Please Wait", 0, QApplication::UnicodeUTF8);
}

} // end namespace Settings

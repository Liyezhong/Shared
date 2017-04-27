/****************************************************************************/
/*! \file ErrorMsgDlg.cpp
 *
 *  \brief Implementation of file for class CErrorMsgDlg.
 *
 *  \b Description:
 *          This class implements a base widget to display Error messages dialog.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-10-29
 *   $Author:  $ Bhanu Prasad H
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "MainMenu/Include/ErrorMsgDlg.h"
#include "ui_ErrorMsgDlg.h"
#include "Application/Include/LeicaStyle.h"


namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor *
 *  \iparam p_Parent = Parent widget
 *  \iparam p_MainWindow = main window object
 *  \iparam p_UserSettingsInterface = User Settings Interface object
*/
/****************************************************************************/
CErrorMsgDlg::CErrorMsgDlg(QWidget *p_Parent, QWidget *p_MainWindow,DataManager::CUserSettingsInterface *p_UserSettingsInterface) :
    MainMenu::CDialogFrame(p_Parent),
    mp_Ui(new Ui::CErrorMsgDlg),
    mp_SettingsInterface(p_UserSettingsInterface)
{
    mp_Ui->setupUi(GetContentFrame());
    mp_TextEdit = new QTextEdit(this);
    mp_TextEdit->setReadOnly(true);
    mp_TextEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    mp_Ui->scrollTable->SetContent(mp_TextEdit);
    setModal(true);
    mp_MainWidow = static_cast<MainMenu::CMainWindow *>(p_MainWindow);
    QPalette Palette = mp_TextEdit->palette();
    Palette.setColor(QPalette::Window, Qt::white);
    mp_TextEdit->setPalette(Palette);
    CONNECTSIGNALSLOTGUI(this, DialogLangaugeChanged(), this, RetranslateUI())

    if (!connect(mp_Ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()))) {
        qDebug() << "CErrorMsgDlg: cannot connect 'clicked' signal";
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CErrorMsgDlg::~CErrorMsgDlg()
{
    try {
        delete mp_TextEdit;
        delete mp_Ui;
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CErrorMsgDlg::changeEvent(QEvent *p_Event)
{
    QDialog::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            RetranslateUI();
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the Errormessages list to the Text Edit
 */
/****************************************************************************/
void CErrorMsgDlg::SetErrorMsgList()
{
    QStringList ErrorText;
    QList<MsgData>::Iterator ErrMsgIterator;
    //Iterating through the List
    for (ErrMsgIterator = m_ErrorMsgList.begin(); ErrMsgIterator!= m_ErrorMsgList.end();ErrMsgIterator++)
    {
        MsgData ErrorMsgStruct = *ErrMsgIterator;
        //Get the DateTime string as per the userSettings Date Format
        QString DateTimeStr = GetDateAndTime(ErrorMsgStruct.Time);
        quint64 ID =  (ErrorMsgStruct.ID & 0xffffffff00000000) >> 32;
        //Appending TimeStamp, EventIdand Event string to a String list
        ErrorText.append(DateTimeStr + ";  "+QString::number(ID) +";  "+
                         QString("%1").arg(ErrorMsgStruct.MsgString)+"\n");
    }
    //Sorting the string list,Latest will be put at the Top
    //Setting the Error Messages to Text Edit
    SetText(ErrorText);
}

/****************************************************************************/
/*!
 *  \brief Sets a small caption in the top of the dialog
 *
 *  \iparam Caption = Caption above the text view
 */
/****************************************************************************/
void CErrorMsgDlg::SetCaption(QString Caption)
{
    mp_Ui->captionLabel->setText(Caption);
}

/****************************************************************************/
/*!
 *  \brief Sets the content of the text view
 *
 *  \iparam MsgList = Message string list.
 */
/****************************************************************************/
void CErrorMsgDlg::SetText(QStringList MsgList)
{
    mp_TextEdit->clear();
    if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
        mp_TextEdit->setFontPointSize(8);
    }
    else {
        mp_TextEdit->setFontPointSize(11);
    }
    QStringList MessagesList;
    for (int Msg = MsgList.count()-1;Msg >=0;Msg--) {
        MessagesList.append(MsgList.at((Msg)));
    }
    mp_TextEdit->setText(MessagesList.join("\n"));
}

/****************************************************************************/
/*!
 *  \brief Popups Error Message dialog
 */
/****************************************************************************/
void CErrorMsgDlg::PopUp()
{
    //Setting Error messages List to the Text Edit
    SetErrorMsgList();
    //Setting Title and Caption label
    SetDialogTitle(QApplication::translate("MainMenu::CErrorMsgDlg", "Error Messages",
                                           Q_NULLPTR, -1));
    if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
        mp_Ui->captionLabel->hide();
        resize(mp_MainWidow->width() - 50, mp_MainWidow->height() - 40);
        move(50, 35);
    }
    else {
        SetCaption(QApplication::translate("MainMenu::CErrorMsgDlg", "Error Messages",
                                           Q_NULLPTR, -1));
        resize(mp_MainWidow->width() - 150, mp_MainWidow->height() - 75);
        move(80, 35);
    }
    show();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CErrorMsgDlg::RetranslateUI()
{
    SetDialogTitle(QApplication::translate("MainMenu::CErrorMsgDlg", "Error Messages",
                                           Q_NULLPTR, -1));
    SetCaption(QApplication::translate("MainMenu::CErrorMsgDlg", "Error Messages",
                                       Q_NULLPTR, -1));
}

/****************************************************************************/
/**
 * \brief Retriving the Event ID and Error Message from Error Message Hash.
 *
 * \iparam  ErrorIDStructList = Error message structure list
 *
 */
/****************************************************************************/
void CErrorMsgDlg::ErrorMsgList(QList <MsgData> ErrorIDStructList)
{
    //Assigning the Error message structure list
    m_ErrorMsgList = ErrorIDStructList;
}

/****************************************************************************/
/*!
 *  \brief Sending the Messages to the Status bar Manager.
 *
 *  \iparam DateTimeStr - Dateand time string in seconds.
 *
 *  \return Date and time string as per the UserSettings.
 */
/****************************************************************************/
QString CErrorMsgDlg::GetDateAndTime(QString DateTimeStr)
{
    QDateTime DateTime = QDateTime::fromString(DateTimeStr);
    QString Date;
    QString Time;
    if (mp_SettingsInterface && mp_SettingsInterface->GetUserSettings()) {
        //Getting the current Date Format from user settings
        Global::DateFormat DateFmt = mp_SettingsInterface->GetUserSettings()->GetDateFormat();
        //Getting the current Time Format from user settings
        //Global::TimeFormat TimeFmt = mp_SettingsInterface->GetUserSettings()->GetTimeFormat();
        if (DateFmt == Global::DATE_INTERNATIONAL) {
            Date = DateTime.date().toString("dd.MM.yyyy");
        }
        else if (DateFmt == Global::DATE_ISO) {
            Date = DateTime.date().toString(Qt::ISODate);
        }
        else {
            Date = DateTime.date().toString("MM/dd/yyyy");
        }

        Time = DateTime.time().toString("hh:mm");
        /*if (TimeFmt == Global::TIME_24) {
            Time = DateTime.time().toString("hh:mm");
        }
        else {
            Time = DateTime.time().toString("hh:mm a");
        }*/
    }
    //Concatenating the Date and Time Strings
    QString TimeStamp = Date + " " + Time;
    return TimeStamp;
}

} // end namespace MainMenu



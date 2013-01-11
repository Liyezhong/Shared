/****************************************************************************/
/*! \file WarningMsgDlg.cpp
 *
 *  \brief WarningMsgDlg implementation.
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
#include "MainMenu/Include/WarningMsgDlg.h"
#include "ui_WarningMsgDlg.h"

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 *  \iparam p_MainWindow = main window object
 *  \iparam p_UserSettingsInterface = User Settings Interface object
 */
/****************************************************************************/
CWarningMsgDlg::CWarningMsgDlg(QWidget *p_Parent, QWidget *p_MainWindow,DataManager::CUserSettingsInterface *p_UserSettingsInterface) : MainMenu::CDialogFrame(p_Parent),
    mp_Ui(new Ui::CWarningMsgDlg),
    mp_SettingsInterface(p_UserSettingsInterface)
{
    mp_Ui->setupUi(GetContentFrame());
    mp_TextEdit = new QTextEdit(mp_Ui->widget);
    mp_TextEdit->setReadOnly(true);
    mp_TextEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    mp_Ui->widget->SetContent(mp_TextEdit);
    mp_MainWidow = static_cast<MainMenu::CMainWindow *>(p_MainWindow);
    QPalette Palette = mp_TextEdit->palette();
    Palette.setColor(QPalette::Window, Qt::white);
    mp_TextEdit->setPalette(Palette);
    if (!connect(mp_Ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()))) {
        qDebug() << "CWarningMsgDlg: cannot connect 'clicked' signal";
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CWarningMsgDlg::~CWarningMsgDlg()
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

void CWarningMsgDlg::changeEvent(QEvent *p_Event)
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


//****************************************************************************/
/*!
 *  \brief Sets the Warningmessages list to the Text Edit
 */
/****************************************************************************/
void CWarningMsgDlg::SetWarningMsgList()
{
    QStringList WarningText;
    QList<MsgData>::Iterator WarnMsgIterator;
    //Iterating through the List
    for (WarnMsgIterator = m_WarnMsgList.begin(); WarnMsgIterator!= m_WarnMsgList.end();WarnMsgIterator++)
    {
        MsgData WarnMsgStruct = *WarnMsgIterator;
        //Get the DateTime string as per the userSettings Date Format
        QString DateTimeStr = GetDateAndTime(WarnMsgStruct.Time);
        quint32 ID =  (WarnMsgStruct.ID & 0xffffffff00000000) >> 32;
        //Appending TimeStamp, EventIdand Event string to a String list
        WarningText.append(DateTimeStr + ";  "+QString::number(ID) +";  "+QString(tr("%1").arg(WarnMsgStruct.MsgString))+"\n");
    }
    //Sorting the string list,Latest will be put at the Top
    //qSort(WarningText.begin(), WarningText.end(), qGreater<QString>());
    //Setting the warning Messages to Text Edit
    SetText(WarningText);

}

/****************************************************************************/
/*!
 *  \brief Sets a small caption in the top of the dialog
 *
 *  \iparam Caption = Caption above the text view
 */
/****************************************************************************/
void CWarningMsgDlg::SetCaption(QString Caption)
{
    mp_Ui->captionLabel->setText(Caption);
}

/****************************************************************************/
/*!
 *  \brief Sets the content of the text view
 *
 *  \iparam Text = Text file
 */
/****************************************************************************/
void CWarningMsgDlg::SetText(QStringList MsgList)
{
    mp_TextEdit->clear();
    mp_TextEdit->setFontPointSize(11);
    for (int Msg = MsgList.count()-1;Msg >=0;Msg--) {
        mp_TextEdit->append(tr("%1").arg(MsgList.at(Msg)));
    }
}

/****************************************************************************/
/*!
 *  \brief Popups Warning Message dialog
 */
/****************************************************************************/
void CWarningMsgDlg::PopUp()
{
    //Setting Warning messages List to the Text Edit
    SetWarningMsgList();
    //Setting Title and Caption label
    SetCaption(tr("Warning Messages"));
    SetDialogTitle(tr("Warning Messages"));
    mp_MainWidow->SetTabWidgetIndex();
    resize(mp_MainWidow->width()-150,mp_MainWidow->height()-75);
    move(80, 35);
    show();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CWarningMsgDlg::RetranslateUI()
{
   MainMenu::CDialogFrame::SetDialogTitle(QApplication::translate("MainMenu::CWarningMsgDlg", "Warning Messages", 0, QApplication::UnicodeUTF8));
}

/****************************************************************************/
/**
 * \brief Retriving the Event ID and Warning Message from Warning Message Hash
 * \iparam  WarningIdMsgHash = Hash with Warning Id and Error Msg data
 *
 */
/****************************************************************************/
void CWarningMsgDlg::WarningMsgList(QList <MsgData> WarnIDStructList)
{
    m_WarnMsgList = WarnIDStructList;
}
/****************************************************************************/
/*!
 *  \brief Sending the Messages to the Status bar Manager
 *  \iparam DateTimeStr - Dateand time string in seconds
 *  \return Date and time string as per the UserSettings
 */
/****************************************************************************/

QString CWarningMsgDlg::GetDateAndTime(QString DateTimeStr)
{
    QDateTime DateTime = QDateTime::fromString(DateTimeStr);
    QString Date;
    QString Time;
    if (mp_SettingsInterface && mp_SettingsInterface->GetUserSettings()) {
        //Getting the current Date Format from user settings
        Global::DateFormat DateFmt = mp_SettingsInterface->GetUserSettings()->GetDateFormat();
        //Getting the current Time Format from user settings
        Global::TimeFormat TimeFmt = mp_SettingsInterface->GetUserSettings()->GetTimeFormat();
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
    //Caoncatenating the Date and Time Strings
    QString TimeStamp = Date + " " + Time;
    return TimeStamp;
}

} // end namespace MainMenu



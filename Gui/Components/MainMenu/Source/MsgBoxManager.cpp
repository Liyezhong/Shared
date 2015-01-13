/****************************************************************************/
/*! \file MsgBoxManager.cpp
 *
 *  \brief Implementation of file for class CMsgBoxManager.
 *
 *  \b Description:
 *          This class manages the messages boxes for displaying Error/
 *          Warning/Information messages
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-10-18
 *   $Author:  $ N.Kamath
 *   \note MsgBoxManager manages the message boxes required to be displayed
 *         for CmdEventReport.
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
#include <QMetaType>
#include <QDebug>
#include <MainMenu/Include/MsgBoxManager.h>
#include <MainMenu/Include/StatusBarManager.h>
#include <MainMenu/Include/MessageDlg.h>
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Helper.h"
#include <QApplication>



namespace MainMenu {

const int MAX_MESSAGE_TEXT_LENGTH = 1024;       //!< Maximum length of the text to be

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = MainWindow is passed as parent
 *  \iparam p_SettingsInterface = SettingsInterface object
 */
/****************************************************************************/
CMsgBoxManager::CMsgBoxManager(QWidget *p_Parent, DataManager::CUserSettingsInterface *p_SettingsInterface):
    m_LastMsgBoxEventID(-1),
    mp_MessageDlg(NULL),
    mp_Parent(p_Parent),
    mp_SettingsInterface(p_SettingsInterface),
    m_bMsgWaiting(false)
{
    m_PopupTimer.setSingleShot(true);
    CONNECTSIGNALSLOTGUI(&m_PopupTimer, timeout(), this, ShowMsgBoxIfQueueNotEmpty());

    m_AutoQuitMsgBoxTimer.setSingleShot(true);
    CONNECTSIGNALSLOT(&m_AutoQuitMsgBoxTimer, timeout(), this, AutoQuitMessageBox());
}

/****************************************************************************/
/*!
 *  \brief Creating Message Box dialog
 *
 *  \iparam MsgDataStruct  = Type of event - info/warning/error/fatalerror
 */
/****************************************************************************/
void CMsgBoxManager::CreateMesgBox(MsgData MsgDataStruct)
{
    if (!mp_MessageDlg) {
        mp_MessageDlg = new MainMenu::CMessageDlg(mp_Parent);
        //Connect Message Box signals
        CONNECTSIGNALSLOTGUI(mp_MessageDlg, ButtonLeftClicked(), this,
                             ButtonLeftClicked());
        CONNECTSIGNALSLOTGUI(mp_MessageDlg, ButtonCenterClicked(), this,
                             ButtonCenterClicked());
        CONNECTSIGNALSLOTGUI(mp_MessageDlg, ButtonRightClicked(), this,
                             ButtonRightClicked());
        CONNECTSIGNALSLOTGUI(mp_MessageDlg, DialogLangaugeChanged(), this,
                             LanguageChanged());
        //Set Title
        switch (MsgDataStruct.EventType) {
        case Global::EVTTYPE_INFO:
            mp_MessageDlg->SetTitle(QApplication::translate("MainMenu::CMsgBoxManager", "Information Message",
                                                            0, QApplication::UnicodeUTF8));

            //Also set Msg Box Icon
            mp_MessageDlg->SetIcon(QMessageBox::Information);
            break;
        case Global::EVTTYPE_WARNING:
            mp_MessageDlg->SetTitle(QApplication::translate("MainMenu::CMsgBoxManager", "Warning Message",
                                                            0, QApplication::UnicodeUTF8));

            mp_MessageDlg->SetIcon(QMessageBox::Warning);
            break;
        case Global::EVTTYPE_FATAL_ERROR:
        case Global::EVTTYPE_ERROR:
            mp_MessageDlg->SetTitle(QApplication::translate("MainMenu::CMsgBoxManager", "Error Message",
                                                            0, QApplication::UnicodeUTF8));
            mp_MessageDlg->SetIcon(QMessageBox::Critical);
            break;
        default:
            mp_MessageDlg->SetTitle(QApplication::translate("MainMenu::CMsgBoxManager", "Error Message",
                                                            0, QApplication::UnicodeUTF8));
            mp_MessageDlg->SetIcon(QMessageBox::Critical);
            break;
        }
        //Set Button Text
        quint32 ButtonCount = Global::GetButtonCountFromButtonType(MsgDataStruct.BtnType);
        if (ButtonCount == 1 && MsgDataStruct.BtnType == Global::OK) {
            mp_MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideButtons();
        }
        else if (ButtonCount == 1 && MsgDataStruct.BtnType == Global::RETRY){
            mp_MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Retry", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideButtons();
        }
        else if (ButtonCount == 2) {
            SetMessageBoxType(MsgDataStruct);
        }
        else if (ButtonCount == 0) {
            mp_MessageDlg->HideAllButtons();
        }
        else {
            mp_MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Yes", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideButtons();
        }
        mp_MessageDlg->setModal(true);
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CMsgBoxManager::~CMsgBoxManager()
{
    try {
        delete mp_MessageDlg;
    }
    catch (...) {
        //to suppress Lint Error
    }
}

/****************************************************************************/
/*!
 *  \brief Manage the Event messages in Ques(Add/Remove)
 *
 *  \iparam DS - CmdEventReport DataStream
 *  \warning - This method can handle DS passed in CmdEventReport Only !!
 *  \iparam Ref -Cmd Reference of CmdEventReport
 */
/****************************************************************************/
void CMsgBoxManager::Manage(QDataStream &DS, Global::tRefType Ref)
{
    //Deserialize Event DataStream
    quint32 EventType;
    quint32 ButtonType;
    quint64 EventID;
    bool EventStatus;
    (void) DS.device()->reset();
    DS >> EventStatus >> EventType >> EventID;
    //check whether we need to display or remove the Msg Box
    if (EventStatus) {
        MsgData CurrentMsgData;
        DS >> CurrentMsgData.MsgString >> CurrentMsgData.Time >> ButtonType >> CurrentMsgData.StatusBarIcon >> CurrentMsgData.BtnEnableConditions >> CurrentMsgData.AutoQuitMsgBoxTime;
        CurrentMsgData.ID = EventID;
        CurrentMsgData.EventType = static_cast<Global::EventType>(EventType);
        CurrentMsgData.BtnType = static_cast<Global::GuiButtonType>(ButtonType);
        if (CurrentMsgData.BtnType != Global::INVISIBLE) {
            AddMsgBoxToQueue(Ref, CurrentMsgData);
        }
        if (CurrentMsgData.StatusBarIcon == true) {
            QByteArray *p_ByteArray = new QByteArray();
            p_ByteArray->clear();
            QDataStream XmlStream(p_ByteArray, QIODevice::ReadWrite);
            XmlStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
            XmlStream << CurrentMsgData.ID << CurrentMsgData.EventType << CurrentMsgData.MsgString << CurrentMsgData.Time;
            (void) XmlStream.device()->reset();
            //Adding the messages with Status as active and Status Icon is TRUE
            MainMenu::CStatusBarManager::GetInstance()->AddEventMessages(XmlStream);
            delete p_ByteArray;
        }
    }
    else {
        RemoveMsgBoxFromQueue(static_cast<Global::EventType>(EventType) , EventID);
        //Removing the messages with Status as Inactive
        MainMenu::CStatusBarManager::GetInstance()->RemoveEventMessages(static_cast<Global::EventType>(EventType) , EventID);
    }
}

void CMsgBoxManager::EnableOKButton()
{
  if (m_bMsgWaiting && mp_MessageDlg)
  {
      mp_MessageDlg->EnableButton(1, true);
      m_bMsgWaiting = false;
  }
}

/****************** **********************************************************/
/*!
 *  \brief This slot is called when the language is changed.
 *
 */
/****************************************************************************/
void CMsgBoxManager::LanguageChanged()
{
    quint32 ButtonCount = Global::GetButtonCountFromButtonType(m_CurrentMsgData.BtnType);
    if (ButtonCount == 1 && m_CurrentMsgData.BtnType == Global::OK && mp_MessageDlg) {
        mp_MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
        mp_MessageDlg->HideButtons();
    }
    else if (ButtonCount == 2) {
        SetMessageBoxType(m_CurrentMsgData);
    }
    else {
        if (mp_MessageDlg) {
            mp_MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideButtons();
        }
    }

    QDateTime DateTime = QDateTime::fromString(m_CurrentMsgData.Time);
    QString Date;
    QString Time;
    if (mp_SettingsInterface && mp_SettingsInterface->GetUserSettings()) {
        Global::DateFormat DateFmt = mp_SettingsInterface->GetUserSettings()->GetDateFormat();
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

        if (TimeFmt == Global::TIME_24) {
            Time = DateTime.time().toString("hh:mm");
        }
        else {
            Time = DateTime.time().toString("hh:mm a");
        }
    }
    // retrieve the original event ID.
    quint32 ID =  (m_CurrentMsgData.ID & 0xffffffff00000000) >> 32;
    if (mp_MessageDlg) {
        mp_MessageDlg->SetTitle(Date + " " + Time, QString::number(ID));
    }

}

/****************** **********************************************************/
/*!
 *  \brief Adds Message box to Priority Queue.
 *
 *  \iparam Ref = Reference type
 *  \iparam CurrentMsgData = Current message data
 */
/****************************************************************************/
void CMsgBoxManager::AddMsgBoxToQueue(Global::tRefType Ref, MsgData &CurrentMsgData)
{
    //Store reference in Hash
    m_EvenIDCmdRefHash.insert(CurrentMsgData.ID, Ref);
    //Add EventID to Priority Queue & EventId Hash
    m_PriorityQueue.Push(static_cast<int>(CurrentMsgData.EventType), CurrentMsgData.ID);
    m_EventIDMsgDataHash.insert(CurrentMsgData.ID, CurrentMsgData);
    //Msg Box will be poped up only after 1 seconds.
    //This is done so that, if GUI receives numerous events
    //at a time, then we prevent Msg Box creation overload
    if (!m_PopupTimer.isActive()) {
        m_PopupTimer.start(1000);
    }
}

/****************************************************************************/
/*!
 *  \brief Slot called when Left corner button on Msg Box is clicked
 *         Left corner button for now is assigned for Negative actions f
 */
/****************************************************************************/
void CMsgBoxManager::ButtonLeftClicked()
{
    //Send Info to  Main
    Global::tRefType CmdRef = m_EvenIDCmdRefHash.value(m_CurrentMsgData.ID);
    if (m_CurrentMsgData.BtnType == Global::OK_CANCEL) {
        emit EventReportAck(NetCommands::CANCEL_BUTTON, CmdRef, m_CurrentMsgData.ID);
    }
    else if (m_CurrentMsgData.BtnType == Global::CONTINUE_STOP) {
        emit EventReportAck(NetCommands::STOP_BUTTON, CmdRef, m_CurrentMsgData.ID);
    }
    else if (m_CurrentMsgData.BtnType == Global::YES_NO) {
        emit EventReportAck(NetCommands::NOT_SPECIFIED, CmdRef, m_CurrentMsgData.ID);
    }
    //RemoveDataFromContainers(m_CurrentMsgData.EventType, m_CurrentMsgData.ID);
    m_LastMsgBoxEventID  = -1;
    //ShowMsgBoxIfQueueNotEmpty();
}

/****************************************************************************/
/*!
 *  \brief Slot called when center button on Msg Box is clicked
 */
/****************************************************************************/
void CMsgBoxManager::ButtonCenterClicked()
{
    //FUTURE Release

}

/****************************************************************************/
/*!
 *  \brief Slot called when Right corner button on Msg Box is clicked
 *         Left corner button for now is assigned for Positive actions
 */
/****************************************************************************/
void CMsgBoxManager::ButtonRightClicked()
{
    //Send Info to  Main
    Global::tRefType CmdRef = m_EvenIDCmdRefHash.value(m_CurrentMsgData.ID);
    if (m_CurrentMsgData.BtnType == Global::OK_CANCEL) {
        emit EventReportAck(NetCommands::OK_BUTTON, CmdRef, m_CurrentMsgData.ID);
    }
    else if (m_CurrentMsgData.BtnType == Global::CONTINUE_STOP) {
        emit EventReportAck(NetCommands::CONTINUE_BUTTON, CmdRef, m_CurrentMsgData.ID);
    }
    else if (m_CurrentMsgData.BtnType == Global::YES_NO) {
        emit EventReportAck(NetCommands::YES_BUTTON, CmdRef, m_CurrentMsgData.ID);
    }
    else if(m_CurrentMsgData.BtnType == Global::OK){
        emit EventReportAck(NetCommands::OK_BUTTON, CmdRef, m_CurrentMsgData.ID);
    }
    else if(m_CurrentMsgData.BtnType == Global::RETRY){
        emit EventReportAck(NetCommands::RETRY_BUTTON, CmdRef, m_CurrentMsgData.ID);
    }
    //RemoveDataFromContainers(m_CurrentMsgData.EventType, m_CurrentMsgData.ID);
    m_LastMsgBoxEventID  = 0;
    //ShowMsgBoxIfQueueNotEmpty();
}

void CMsgBoxManager::AutoQuitMessageBox()
{
    Global::tRefType CmdRef = m_EvenIDCmdRefHash.value(m_CurrentMsgData.ID);
    emit EventReportAck(NetCommands::TIMEOUT, CmdRef, m_CurrentMsgData.ID);
    //RemoveDataFromContainers(m_CurrentMsgData.EventType, m_CurrentMsgData.ID);
    m_LastMsgBoxEventID  = 0;
    //ShowMsgBoxIfQueueNotEmpty();
}

/****************************************************************************/
/*!
 *  \brief Checks if Msg Box needs to be displayed
 */
/****************************************************************************/
void CMsgBoxManager::ShowMsgBoxIfQueueNotEmpty()
{
    if (mp_MessageDlg)
    {
        RemoveMsgBoxFromQueue(static_cast<Global::EventType>(m_CurrentMsgData.EventType) , m_LastMsgBoxEventID);
        //Removing the messages with Status as Inactive
        MainMenu::CStatusBarManager::GetInstance()->RemoveEventMessages(static_cast<Global::EventType>(m_CurrentMsgData.EventType) , m_LastMsgBoxEventID);

        delete mp_MessageDlg;
        mp_MessageDlg = NULL;
    }
        //If there are MesgBoxes to be displayed still,then display them
    if (!m_PriorityQueue.IsEmpty()) {
        //Now update Msg Data
        qint64 CurrentMsgBoxEventID = m_PriorityQueue.Top();
        //Keys in Multimap are in ascending order, hence last key is the
        //one with highest priority
        //if (CurrentMsgBoxEventID == m_LastMsgBoxEventID) {
            //If current msg box event ID is equal to the last key of the multimap
            //Return from the current method.
            //return;
        //}

        //update MsgDlg info
        m_CurrentMsgData = m_EventIDMsgDataHash.value(CurrentMsgBoxEventID);
        CreateMesgBox(m_CurrentMsgData);

        //Check for the text lenght to be displayed
        QString MessageString = m_CurrentMsgData.MsgString;
        if (MessageString.length() > MAX_MESSAGE_TEXT_LENGTH) {
            (void) MessageString.remove(MAX_MESSAGE_TEXT_LENGTH, MessageString.length() - MAX_MESSAGE_TEXT_LENGTH);
            MessageString.append(" ...");
        }
        // Now update MsgDialog
        mp_MessageDlg->SetText(MessageString);
        QDateTime DateTime = QDateTime::fromString(m_CurrentMsgData.Time);
        QString Date;
        QString Time;
        if (mp_SettingsInterface && mp_SettingsInterface->GetUserSettings()) {
            Global::DateFormat DateFmt = mp_SettingsInterface->GetUserSettings()->GetDateFormat();
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

            if (TimeFmt == Global::TIME_24) {
                Time = DateTime.time().toString("hh:mm");
            }
            else {
                Time = DateTime.time().toString("hh:mm a");
            }
        }
        // retrieve the original event ID.
        quint32 ID =  (m_CurrentMsgData.ID & 0xffffffff00000000) >> 32;
        mp_MessageDlg->SetTitle(Date + " " + Time, QString::number(ID));

        if (mp_Parent)
        {
            QRect scr = mp_Parent->geometry();
            mp_MessageDlg->move( scr.center() - mp_MessageDlg->rect().center());
        }

        if ("" != m_CurrentMsgData.AutoQuitMsgBoxTime)
        {
            int timeInSeconds = DataManager::Helper::ConvertTimeStringToSeconds(m_CurrentMsgData.AutoQuitMsgBoxTime);
            m_AutoQuitMsgBoxTimer.start(timeInSeconds*1000);
        }

        //disable "OK"
        if ("" != m_CurrentMsgData.BtnEnableConditions)
        {
            if (m_CurrentMsgData.BtnEnableConditions == "RT_LID_OPEN_CLOSE")
            {
                mp_MessageDlg->EnableButton(1, false);
                m_bMsgWaiting = true;
            }
        }


        //Show MsgDlg
        mp_MessageDlg->show();
        //Store Event Id currently being displayed
        m_LastMsgBoxEventID = CurrentMsgBoxEventID;
    }
}

/****************************************************************************/
/*!
 *  \brief  Removes the Msg Box from the Queue and then checks if Msg Boxes
 *          are available in queue to be displayed
 *  \iparam EventType = Info/warning/Error
 *  \iparam ID        = Event ID
 */
/****************************************************************************/
void CMsgBoxManager::RemoveMsgBoxFromQueue(Global::EventType EventType, quint64 ID)
{
    //Remove data from internal containers - hashes/priority queues etc
    RemoveDataFromContainers(EventType, ID);
    //Check if MsgBox with this Id is currently being shown
    if (ID == m_CurrentMsgData.ID && mp_MessageDlg) {
        mp_MessageDlg->hide();
        //ShowMsgBoxIfQueueNotEmpty();
    }
}

/****************************************************************************/
/*!
 *  \brief  Removes Event ID from the containers used internally by MsgBox
 *          Manager
 *  \iparam EventType = Info/warning/Error
 *  \iparam ID        = Event ID
 */
/****************************************************************************/
void CMsgBoxManager::RemoveDataFromContainers(Global::EventType EventType, quint64 ID)
{
    m_PriorityQueue.Remove(EventType, ID);
    m_EventIDMsgDataHash.remove(ID);
    m_EvenIDCmdRefHash.remove(ID);
}

/****************************************************************************/
/*!
 *  \brief  Sets message box type depending on button types.
 *  \iparam MessageBoxData = Info/warning/Error
 */
/****************************************************************************/
void CMsgBoxManager::SetMessageBoxType(MsgData MessageBoxData)
{
    if (mp_MessageDlg) {
        switch (MessageBoxData.BtnType) {
        case Global::YES_NO:
            //I am not adding the below lines(Which are repeated in every switch case) to a function
            //since Translation is a problem
            mp_MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Yes", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->SetButtonText(3, QApplication::translate("CMsgBoxManager", "No", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideCenterButton();
            break;
        case Global::CONTINUE_STOP:
            mp_MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Continue", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->SetButtonText(3, QApplication::translate("CMsgBoxManager","Stop", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideCenterButton();
            break;
        case Global::OK_CANCEL:
            mp_MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->SetButtonText(3, QApplication::translate("CMsgBoxManager","Cancel", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideCenterButton();
            break;
        case Global::WITHOUT_BUTTONS:
            mp_MessageDlg->HideAllButtons();
            break;
        default:
            mp_MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
            mp_MessageDlg->HideButtons();
            break;
        }
    }
}
} // End of namespace Core

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
    m_CurrentMsgBoxEventID(-1),
    mp_MessageDlg(NULL),
    mp_Parent(p_Parent),
    mp_SettingsInterface(p_SettingsInterface),
    m_bMsgWaiting(false)
{
    m_PopupTimer.setSingleShot(true);
    CONNECTSIGNALSLOTGUI(&m_PopupTimer, timeout(), this, ShowMsgBoxIfQueueNotEmpty());
}

/****************************************************************************/
/*!
 *  \brief Creating Message Box dialog
 *
 *  \iparam MsgDataStruct  = Type of event - info/warning/error/fatalerror
 */
/****************************************************************************/
MainMenu::CMessageDlg *  CMsgBoxManager::CreateMesgBox(MsgData MsgDataStruct)
{
    MainMenu::CMessageDlg *MsgDlg = new MainMenu::CMessageDlg(mp_Parent, MsgDataStruct.ID);
    //Connect Message Box signals
    CONNECTSIGNALSLOTGUI(MsgDlg, ButtonLeftClicked(quint64), this,
                         ButtonLeftClicked(quint64));
    CONNECTSIGNALSLOTGUI(MsgDlg, ButtonCenterClicked(quint64), this,
                         ButtonCenterClicked(quint64));
    CONNECTSIGNALSLOTGUI(MsgDlg, ButtonRightClicked(quint64), this,
                         ButtonRightClicked(quint64));
    CONNECTSIGNALSLOTGUI(MsgDlg, DialogLangaugeChanged(), this,
                         LanguageChanged());
    //Set Title
    switch (MsgDataStruct.EventType) {
    case Global::EVTTYPE_INFO:
        MsgDlg->SetTitle(QApplication::translate("MainMenu::CMsgBoxManager", "Information Message",
                                                        0, QApplication::UnicodeUTF8));

        //Also set Msg Box Icon
        MsgDlg->SetIcon(QMessageBox::Information);
        break;
    case Global::EVTTYPE_WARNING:
        MsgDlg->SetTitle(QApplication::translate("MainMenu::CMsgBoxManager", "Warning Message",
                                                        0, QApplication::UnicodeUTF8));

        MsgDlg->SetIcon(QMessageBox::Warning);
        break;
    case Global::EVTTYPE_FATAL_ERROR:
    case Global::EVTTYPE_ERROR:
        MsgDlg->SetTitle(QApplication::translate("MainMenu::CMsgBoxManager", "Error Message",
                                                        0, QApplication::UnicodeUTF8));
        MsgDlg->SetIcon(QMessageBox::Critical);
        break;
    default:
        MsgDlg->SetTitle(QApplication::translate("MainMenu::CMsgBoxManager", "Error Message",
                                                        0, QApplication::UnicodeUTF8));
        MsgDlg->SetIcon(QMessageBox::Critical);
        break;
    }
    //Set Button Text
    quint32 ButtonCount = Global::GetButtonCountFromButtonType(MsgDataStruct.BtnType);
    if (ButtonCount == 1 && MsgDataStruct.BtnType == Global::OK) {
        MsgDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
        MsgDlg->HideButtons();
    }
    else if (ButtonCount == 1 && MsgDataStruct.BtnType == Global::RETRY){
        MsgDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Retry", 0, QApplication::UnicodeUTF8));
        MsgDlg->HideButtons();
    }
    else if (ButtonCount == 2) {
        SetMessageBoxType(MsgDlg, MsgDataStruct);
    }
    else if (ButtonCount == 0) {
        MsgDlg->HideAllButtons();
    }
    else {
        MsgDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Yes", 0, QApplication::UnicodeUTF8));
        MsgDlg->HideButtons();
    }
    MsgDlg->setModal(false);


    //Check for the text lenght to be displayed
    QString MessageString = MsgDataStruct.MsgString;
    if (MessageString.length() > MAX_MESSAGE_TEXT_LENGTH) {
        (void) MessageString.remove(MAX_MESSAGE_TEXT_LENGTH, MessageString.length() - MAX_MESSAGE_TEXT_LENGTH);
        MessageString.append(" ...");
    }
    // Now update MsgDialog
    MsgDlg->SetText(MessageString);
    QDateTime DateTime = QDateTime::fromString(MsgDataStruct.Time);
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
    quint32 ID =  (MsgDataStruct.ID & 0xffffffff00000000) >> 32;
    MsgDlg->SetTitle(Date + " " + Time, QString::number(ID));

    //disable "OK"
    if ("" != MsgDataStruct.BtnEnableConditions)
    {
        if (m_CurrentMsgData.BtnEnableConditions == "RT_LID_OPEN_CLOSE")
        {
            MsgDlg->EnableButton(1, false);
            m_bMsgWaiting = true;
        }
    }
    return MsgDlg;

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
        QHashIterator<quint64, MainMenu::CMessageDlg *> i(m_MsgDlgEventIDHash);
        while(i.hasNext())
        {
            delete i.value();
        }
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
        RemoveMsgBoxFromQueue(EventID);
        //Removing the messages with Status as Inactive
        MainMenu::CStatusBarManager::GetInstance()->RemoveEventMessages(static_cast<Global::EventType>(EventType) , EventID);
    }
}

void CMsgBoxManager::EnableOKButton()
{
  if (m_bMsgWaiting && mp_MessageDlg)
  {
      QHashIterator<quint64, MainMenu::CMessageDlg *> i(m_MsgDlgEventIDHash);
      while(i.hasNext())
      {
          if(i.value())
          {
              i.value()->EnableButton(1, true);
          }
      }
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
    QHashIterator<quint64, MsgData> i(m_EventIDMsgDataHash);
    while(i.hasNext())
    {
        MsgData data = i.value();
        quint64 EventID  = i.key();
        if(!m_MsgDlgEventIDHash.contains(EventID) || m_MsgDlgEventIDHash.value(EventID) == NULL)
        {
            continue;
        }
        MainMenu::CMessageDlg *MessageDlg = m_MsgDlgEventIDHash.value(EventID);
        quint32 ButtonCount = Global::GetButtonCountFromButtonType(data.BtnType);
        if (ButtonCount == 1 && m_CurrentMsgData.BtnType == Global::OK && MessageDlg) {
            MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
            MessageDlg->HideButtons();
        }
        else if (ButtonCount == 2) {
            SetMessageBoxType(MessageDlg,data);
        }
        else {
            if (MessageDlg) {
                MessageDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
                MessageDlg->HideButtons();
            }
        }

        QDateTime DateTime = QDateTime::fromString(data.Time);
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
        quint32 ID =  (data.ID & 0xffffffff00000000) >> 32;
        if (MessageDlg) {
            MessageDlg->SetTitle(Date + " " + Time, QString::number(ID));
        }
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
    RemoveDataFromContainers(CurrentMsgData.ID);
    m_EvenIDCmdRefHash.insert(CurrentMsgData.ID, Ref);
    //Add EventID to Priority Queue & EventId Hash
    m_PriorityQueue.Push(static_cast<int>(CurrentMsgData.EventType), CurrentMsgData.ID);
    m_EventIDMsgDataHash.insert(CurrentMsgData.ID, CurrentMsgData);
    //Msg Box will be poped up only after 1 seconds.
    //This is done so that, if GUI receives numerous events
    //at a time, then we prevent Msg Box creation overload
//    if(!mp_MessageDlg || !mp_MessageDlg->isVisible()){
//        if (!m_PopupTimer.isActive()) {
//            m_PopupTimer.start(1000);
//        }
//    }
//    ShowMsgBoxIfQueueNotEmpty();
    m_PopupTimer.start(10);
}

/****************************************************************************/
/*!
 *  \brief Slot called when Left corner button on Msg Box is clicked
 *         Left corner button for now is assigned for Negative actions f
 *  \param ID message box id
 */
/****************************************************************************/
void CMsgBoxManager::ButtonLeftClicked(quint64 ID)
{
    //Send Info to  Main
    if(m_EvenIDCmdRefHash.contains(ID)){
        Global::tRefType CmdRef = m_EvenIDCmdRefHash.value(ID);
        MsgData msg = m_EventIDMsgDataHash.value(ID);
        if (msg.BtnType == Global::OK_CANCEL) {
            emit EventReportAck(NetCommands::CANCEL_BUTTON, CmdRef, ID);
        }
        else if (msg.BtnType == Global::CONTINUE_STOP) {
            emit EventReportAck(NetCommands::STOP_BUTTON, CmdRef, ID);
        }
        else if (msg.BtnType == Global::YES_NO) {
            emit EventReportAck(NetCommands::NOT_SPECIFIED, CmdRef, ID);
        }
        RemoveDataFromContainers(ID);
        //    m_CurrentMsgBoxEventID  = -1;
        //    ShowNextMsgBoxInQueue();
    }
}

/****************************************************************************/
/*!
 *  \brief Slot called when center button on Msg Box is clicked
 *  \param ID message box id
 */
/****************************************************************************/
void CMsgBoxManager::ButtonCenterClicked(quint64 ID)
{
    Q_UNUSED(ID)
    //FUTURE Release

}

/****************************************************************************/
/*!
 *  \brief Slot called when Right corner button on Msg Box is clicked
 *         Left corner button for now is assigned for Positive actions
 *  \param ID message box id
 */
/****************************************************************************/
void CMsgBoxManager::ButtonRightClicked(quint64 ID)
{
    //Send Info to  Main
    if(m_EvenIDCmdRefHash.contains(ID)){
        Global::tRefType CmdRef = m_EvenIDCmdRefHash.value(ID);
        MsgData msg = m_EventIDMsgDataHash.value(ID);
        if (msg.BtnType == Global::OK_CANCEL) {
            emit EventReportAck(NetCommands::OK_BUTTON, CmdRef, ID);
        }
        else if (msg.BtnType == Global::CONTINUE_STOP) {
            emit EventReportAck(NetCommands::CONTINUE_BUTTON, CmdRef, ID);
        }
        else if (msg.BtnType == Global::YES_NO) {
            emit EventReportAck(NetCommands::YES_BUTTON, CmdRef, ID);
        }
        else if(msg.BtnType == Global::OK){
            emit EventReportAck(NetCommands::OK_BUTTON, CmdRef, ID);
        }
        else if(msg.BtnType == Global::RETRY){
            emit EventReportAck(NetCommands::RETRY_BUTTON, CmdRef, ID);
        }
        RemoveDataFromContainers(ID);
//        m_CurrentMsgBoxEventID  = 0;
//        ShowNextMsgBoxInQueue();
    }
}

/****************************************************************************/
/*!
 *  \brief Slot called when Right corner button on Msg Box is clicked
 *         Left corner button for now is assigned for Positive actions
 *  \param ID quint64 messagebox id
 */
void CMsgBoxManager::AutoQuitMessageBox(quint64 ID)
{
    Global::tRefType CmdRef = m_EvenIDCmdRefHash.value(ID);
    emit EventReportAck(NetCommands::TIMEOUT, CmdRef, ID);
    RemoveDataFromContainers(ID);
    //ShowNextMsgBoxInQueue();
}

void CMsgBoxManager::ShowNextMsgBoxInQueue()
{
    if(! m_PopupTimer.isActive()){
        m_PopupTimer.start(100);
    }
}

/****************************************************************************/
/*!
 *  \brief Checks if Msg Box needs to be displayed
 */
/****************************************************************************/
void CMsgBoxManager::ShowMsgBoxIfQueueNotEmpty()
{
    //If there are MesgBoxes to be displayed still,then display them
    if (!m_PriorityQueue.IsEmpty()) {
        //Now update Msg Data
        qint64 CurrentMsgBoxEventID = m_PriorityQueue.Top();
        //update MsgDlg info
        MsgData MData = m_EventIDMsgDataHash.value(CurrentMsgBoxEventID);

         MainMenu::CMessageDlg * MessDlg = NULL;
         MessDlg = CreateMesgBox(MData);

         if(MessDlg)
         {
             m_MsgDlgEventIDHash.insert(CurrentMsgBoxEventID, MessDlg);
         }
         else
         {
             return;
         }
        if (mp_Parent && MessDlg)
        {
            QRect scr = mp_Parent->geometry();
            MessDlg->move( scr.center() - MessDlg->rect().center());
        }

        if ("" != MData.AutoQuitMsgBoxTime)
        {
            int timeInSeconds = DataManager::Helper::ConvertTimeStringToSeconds(MData.AutoQuitMsgBoxTime);
            CONNECTSIGNALSLOT(MessDlg,MsgBoxTimeout(quint64), this, AutoQuitMessageBox(quint64));
            MessDlg->StartTimer(timeInSeconds * 1000);
        }

        //Show MsgDlg
        MessDlg->Show();
        //Store Event Id currently being displayed
//        m_CurrentMsgBoxEventID = CurrentMsgBoxEventID;
    }
}

/****************************************************************************/
/*!
 *  \brief  Removes the Msg Box from the Queue and then checks if Msg Boxes
 *          are available in queue to be displayed
 *  \iparam ID        = Event ID
 */
/****************************************************************************/
void CMsgBoxManager::RemoveMsgBoxFromQueue(quint64 ID)
{
    //Remove data from internal containers - hashes/priority queues etc
    RemoveDataFromContainers(ID);
    //Check if MsgBox with this Id is currently being shown
//    if (ID == m_CurrentMsgData.ID && mp_MessageDlg) {
//        mp_MessageDlg->hide();
//        ShowNextMsgBoxInQueue();
//    }
}

/****************************************************************************/
/*!
 *  \brief  Removes Event ID from the containers used internally by MsgBox
 *          Manager
 *  \iparam ID        = Event ID
 */
/****************************************************************************/
void CMsgBoxManager::RemoveDataFromContainers(quint64 ID)
{
    MsgData msg = m_EventIDMsgDataHash.value(ID);
    m_PriorityQueue.Remove(msg.EventType, ID);
    m_EventIDMsgDataHash.remove(ID);
    m_EvenIDCmdRefHash.remove(ID);
    MainMenu::CMessageDlg *MsgDlg = NULL;
    MsgDlg = m_MsgDlgEventIDHash.value(ID);
    if(MsgDlg)
    {
        MsgDlg->hide();
        m_MsgDlgEventIDHash.remove(ID);
        delete MsgDlg;
    }
}

/****************************************************************************/
/*!
 *  \brief  Sets message box type depending on button types.
 *  \iparam MessageBoxData = Info/warning/Error
 *  \iparam MsgDlg messge dialog
 */
/****************************************************************************/
void CMsgBoxManager::SetMessageBoxType(MainMenu::CMessageDlg *MsgDlg, MsgData MessageBoxData)
{
    if (MsgDlg) {
        switch (MessageBoxData.BtnType) {
        case Global::YES_NO:
            //I am not adding the below lines(Which are repeated in every switch case) to a function
            //since Translation is a problem
            MsgDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Yes", 0, QApplication::UnicodeUTF8));
            MsgDlg->SetButtonText(3, QApplication::translate("CMsgBoxManager", "No", 0, QApplication::UnicodeUTF8));
            MsgDlg->HideCenterButton();
            break;
        case Global::CONTINUE_STOP:
            MsgDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Continue", 0, QApplication::UnicodeUTF8));
            MsgDlg->SetButtonText(3, QApplication::translate("CMsgBoxManager","Stop", 0, QApplication::UnicodeUTF8));
            MsgDlg->HideCenterButton();
            break;
        case Global::OK_CANCEL:
            MsgDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
            MsgDlg->SetButtonText(3, QApplication::translate("CMsgBoxManager","Cancel", 0, QApplication::UnicodeUTF8));
            MsgDlg->HideCenterButton();
            break;
        case Global::WITHOUT_BUTTONS:
            MsgDlg->HideAllButtons();
            break;
        default:
            MsgDlg->SetButtonText(1, QApplication::translate("CMsgBoxManager","Ok", 0, QApplication::UnicodeUTF8));
            MsgDlg->HideButtons();
            break;
        }
    }
}
} // End of namespace Core

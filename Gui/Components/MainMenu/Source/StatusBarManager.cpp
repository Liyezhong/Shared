/****************************************************************************/
/*! \file StatusBarManager.cpp
 *
 *  \brief StatusBarManager Implementation
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-11-02
 *   $Author:  $ Bhanu Prasad H
 *   \note StatusBarManager manages the Statusbar icons.
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

#include <MainMenu/Include/StatusBarManager.h>
#include <MainMenu/Include/MessageDlg.h>
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"


namespace MainMenu {

StatusBarManager* StatusBarManager::mp_StatusBarMgrInstance = NULL; //!< The instance


/****************************************************************************/
/*!
 *  \brief Constructor
 *
 */
/****************************************************************************/

StatusBarManager::StatusBarManager() :QWidget(NULL)
{

}

/****************************************************************************/
/**
 * \brief Argumented constructor.
 */
/****************************************************************************/
StatusBarManager::StatusBarManager(MainMenu::CMainWindow *p_MainWindow):mp_MainWindow(p_MainWindow)
{
    mp_ErrorMsgDlg = new MainMenu::CErrorMsgDlg(this, mp_MainWindow);
    mp_WarningMsgDlg = new MainMenu::CWarningMsgDlg(this,mp_MainWindow);
    CONNECTSIGNALSIGNAL(mp_MainWindow,ShowErrorMsgDlg(),this,ShowErrorPopup());
    CONNECTSIGNALSIGNAL(mp_MainWindow,ShowWarningMsgDlg(),this,ShowWarningPopup());
    CONNECTSIGNALSLOT(this,ShowErrorPopup(),mp_ErrorMsgDlg,PopUp());
    CONNECTSIGNALSLOT(this,ShowWarningPopup(),mp_WarningMsgDlg,PopUp());
}

/****************************************************************************/
/**
 * \brief Adding event messages to the List
 * \iparam EventMsgStream = EventMessage data in the data stream format
 *
 */
/****************************************************************************/
void StatusBarManager::AddEventMessages(QDataStream &EventMsgStream)
{
    quint32 EventType;
    quint32 ButtonType;
    quint64 EventId;
    bool EventStatus;
    bool StatusBarIcon;
    EventMsgStruct Msg;
    EventMsgStream >> EventStatus >> EventType >> EventId;
    qDebug() << "Event Message Stream" << EventStatus << EventType << EventId;
    EventMsgStream >> Msg.EventString >> Msg.Time >> ButtonType >> StatusBarIcon;
    Msg.ID = EventId;
    Msg.EventType = static_cast<Global::EventType>(EventType);
    if (Msg.EventType == Global::EVTTYPE_WARNING) {
        m_WarningIDMsgHash.insert(Msg.ID,Msg);
        //Updating the Event Message data into a list
        WarningMsgList(m_WarningIDMsgHash);
        qDebug() << "warning ID hash"<<m_WarningIDMsgHash.count();
        if (m_WarningIDMsgHash.count() > 0) {
            (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::Warning);
        }
        qDebug() << "Added in Warning hash" ;
    }
    else if(Msg.EventType == Global::EVTTYPE_ERROR) {
        m_ErrorIDMsgHash.insert(Msg.ID,Msg);
        qDebug() << "Error ID hash"<<m_ErrorIDMsgHash.count();
        if (m_ErrorIDMsgHash.count() > 0) {
            (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::Error);
        }
        //Updating the Event Message data into a list
        ErrorMsgList(m_ErrorIDMsgHash);
        qDebug() << "Added in Error hash" ;
    }
    qDebug() << "Event ID in message box is" << EventId;
}

/****************************************************************************/
/**
 * \brief Removing event messages From the List
 * \iparam  EventType = Event type
 * \iparam  ID = Event Id
 */
/****************************************************************************/
void StatusBarManager::RemoveEventMessages(Global::EventType EventType, quint64 EventID)
{
    if (m_WarningIDMsgHash.contains(EventID) || m_ErrorIDMsgHash.contains(EventID)) {
        if (EventType == Global::EVTTYPE_WARNING) {
            m_WarningIDMsgHash.remove(EventID);
            //Updating the Event Message data into a list
            WarningMsgList(m_WarningIDMsgHash);
            if(m_WarningIDMsgHash.count() <= 0)
            {
                /// \todo: Unsetting the icon is not completed
                (void)mp_MainWindow->UnsetStatusIcons(MainMenu::CMainWindow::Warning);
            }
        }
        else if(EventType == Global::EVTTYPE_ERROR) {
            m_ErrorIDMsgHash.remove(EventID);
            //Updating the Event Message data into a list
            ErrorMsgList(m_ErrorIDMsgHash);
            if(m_ErrorIDMsgHash.count()<= 0)
            {
                /// \todo: Unsetting the icon is not completed
                (void)mp_MainWindow->UnsetStatusIcons(MainMenu::CMainWindow::Error);
            }
        }
        qDebug() << "Event MEssages removal" << EventID;
    }
    else {
        qDebug() << "Event ID is not present in the Event message Hash" ;
    }
}
/****************************************************************************/
/**
 * \brief Retriving the Event ID and Error Message from Error Message Hash
 * \iparam  ErrorIdMsgHash = Hash with Error Id and Error Msg data
 *
 */
/****************************************************************************/
void StatusBarManager::ErrorMsgList(QHash <quint64, EventMsgStruct> ErrorIdMsgHash)
{
    QHash<quint64, EventMsgStruct>::const_iterator ErrorList = ErrorIdMsgHash.constBegin();
    EventMsgStruct EventMsg;
    QStringList ErrMsgList;
    QStringList EventIDList;
    QStringList TimeList;
    for (ErrorList = ErrorIdMsgHash.constBegin(); ErrorList != ErrorIdMsgHash.constEnd(); ++ErrorList) {
        EventMsg = ErrorList.value();
        EventIDList.append(QString::number(EventMsg.ID));
        ErrMsgList.append(EventMsg.EventString);
        TimeList.append(EventMsg.Time);
    }
    //Setting the EventID,Event messages list to the Model
    mp_ErrorMsgDlg->SetErrorMsgList(EventIDList,ErrMsgList,TimeList);
}
/****************************************************************************/
/**
 * \brief Retriving the Event ID and Warning Message from Warning Message Hash
 * \iparam  WarningIdMsgHash = Hash with Warning Id and Error Msg data
 *
 */
/****************************************************************************/
void StatusBarManager::WarningMsgList(QHash <quint64, EventMsgStruct> WarningIdMsgHash)
{
    QHash<quint64, EventMsgStruct>::const_iterator WarningList = WarningIdMsgHash.constBegin();
    EventMsgStruct EventMsg;
    QStringList WarnMsgList;
    QStringList EventIDList;
    QStringList TimeList;
    for (WarningList = WarningIdMsgHash.constBegin(); WarningList != WarningIdMsgHash.constEnd(); ++WarningList) {
        EventMsg = WarningList.value();
        EventIDList.append(QString::number(EventMsg.ID));//at(count) = QString(EventMsg.ID);
        WarnMsgList.append(EventMsg.EventString);
        TimeList.append(EventMsg.Time);
    }
    //Setting the EventID,Event messages list to the Model
    mp_WarningMsgDlg->SetWarningMsgList(EventIDList,WarnMsgList,TimeList);
}

} // end namespace EventHandling


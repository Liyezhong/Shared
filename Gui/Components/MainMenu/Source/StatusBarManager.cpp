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
StatusBarManager::StatusBarManager(MainMenu::CMainWindow *p_MainWindow,DataManager::CUserSettingsInterface *p_UsrSettingsInterface):mp_MainWindow(p_MainWindow),mp_UsrSettingsInterface(p_UsrSettingsInterface)
{
    mp_ErrorMsgDlg = new MainMenu::CErrorMsgDlg(this, mp_MainWindow,mp_UsrSettingsInterface);
    mp_WarningMsgDlg = new MainMenu::CWarningMsgDlg(this,mp_MainWindow,mp_UsrSettingsInterface);
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
    MsgData Msg;
    EventMsgStream >> Msg.ID >> EventType >> Msg.MsgString >> Msg.Time;
    Msg.EventType = static_cast<Global::EventType>(EventType);
    if (Msg.EventType == Global::EVTTYPE_WARNING) {
        m_WarningMsgList.append(Msg);
        qDebug() << "warning ID hash"<<m_WarningMsgList.count();
        if (m_WarningMsgList.count() > 0) {
            //Displaying the message icon when the list count is morethan 0
            (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::Warning);
        }
        //We have to display only 10 latest entries so restricting the count to 10
        if (m_WarningMsgList.count() >10) {
            m_WarningMsgList.removeAt(0);
        }
        //Updating the Event Message data into a list
        mp_WarningMsgDlg->WarningMsgList(m_WarningMsgList);
        qDebug() << "Added in Warning hash" ;
    }
    else if(Msg.EventType == Global::EVTTYPE_ERROR) {
        m_ErrorMsgList.append(Msg);
        qDebug() << "Error ID hash"<<m_ErrorMsgList.count();
        if (m_ErrorMsgList.count() > 0) {
            //Displaying the message icon when the list count is morethan 0
            (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::Error);
        }
        //We have to display only 10 latest entries so restricting the count to 10
        if (m_ErrorMsgList.count() >10) {
            m_ErrorMsgList.removeAt(0);
        }
        //Updating the Event Message data into a list
        mp_ErrorMsgDlg->ErrorMsgList(m_ErrorMsgList);
        qDebug() << "Added in Error hash" ;
    }
    qDebug() << "Event ID in message box is" << Msg.ID;
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

    QList<MsgData>::Iterator ErrMsgIterator;
    QList<MsgData>::Iterator WarnMsgIterator;
    //Check for the Error type Message
    if (EventType == Global::EVTTYPE_ERROR) {
        int MsgCount=0;
        //Iterate Through the List
        for (ErrMsgIterator = m_ErrorMsgList.begin(); ErrMsgIterator!= m_ErrorMsgList.end();++ErrMsgIterator)
        {
            MsgData EventData = *ErrMsgIterator;
            if (EventData.ID == EventID) {
                //Remove the Element from the list which contain the EventID to be removed
                m_ErrorMsgList.removeAt(MsgCount);
                break;
            }
            MsgCount++;
        }
        //Set the Error messages List to the  Error messages dialog
        mp_ErrorMsgDlg->ErrorMsgList(m_ErrorMsgList);
        if (m_ErrorMsgList.count() <= 0) {
            //Unsetting the Icon if the Error messages list count is 0
            (void)mp_MainWindow->UnsetStatusIcons(MainMenu::CMainWindow::Error);
        }
    }
    //Check for the Warning type Message
    else if (EventType == Global::EVTTYPE_WARNING) {
        int MsgCount=0;
        //Iterate Through the List
        for (WarnMsgIterator = m_WarningMsgList.begin(); WarnMsgIterator!= m_WarningMsgList.end();++WarnMsgIterator)
        {
            MsgData EventData = *WarnMsgIterator;
            if (EventData.ID == EventID) {
                //Remove the Element from the list which contain the EventID to be removed
                m_WarningMsgList.removeAt(MsgCount);
                break;
            }
            MsgCount++;
        }
        //Set the Error messages List to the  Error messages dialog
        mp_WarningMsgDlg->WarningMsgList(m_WarningMsgList);
        if (m_WarningMsgList.count() <= 0) {
            //Unsetting the Icon if the Warningmessages list count is 0
            (void)mp_MainWindow->UnsetStatusIcons(MainMenu::CMainWindow::Warning);
        }
    }
    else
    {
        qDebug() << "Event ID is not present in the Event message List" ;
    }
}

} // end namespace EventHandling


/****************************************************************************/
/*! \file StatusBarManager.cpp
 *
 *  \brief Implementation of file for class CStatusBarManager.
 *
 *  \b Description:
 *          This class Manages the displaying of statusbarIcons for
 *          Warning/Error/RemoteCare/Process icons in the application statusbar .
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

CStatusBarManager* CStatusBarManager::mp_StatusBarMgrInstance = NULL; //!< The instance


/****************************************************************************/
/*!
 *  \brief Constructor
 *
 */
/****************************************************************************/

CStatusBarManager::CStatusBarManager() :QWidget(NULL)
{

}
/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CStatusBarManager::~CStatusBarManager()
{

    try {
        delete mp_ErrorMsgDlg;
        delete mp_WarningMsgDlg;
        delete mp_MainWindow;
    }
    catch (...) {
        // to please Lint.
    }
}

/****************************************************************************/
/**
 * \brief Argumented constructor.
 *
 * \iparam p_MainWindow = MainWindow object
 * \iparam p_UsrSettingsInterface = UserSettingsInterface object
 *
 */
/****************************************************************************/
CStatusBarManager::CStatusBarManager(MainMenu::CMainWindow *p_MainWindow,
                                     DataManager::CUserSettingsInterface *p_UsrSettingsInterface):
    mp_MainWindow(p_MainWindow),
    mp_UsrSettingsInterface(p_UsrSettingsInterface)
{
    mp_ErrorMsgDlg = new MainMenu::CErrorMsgDlg(this, mp_MainWindow,mp_UsrSettingsInterface);
    mp_WarningMsgDlg = new MainMenu::CWarningMsgDlg(this,mp_MainWindow,mp_UsrSettingsInterface);
    CONNECTSIGNALSLOTGUI(mp_MainWindow, ShowErrorMsgDlg(), mp_ErrorMsgDlg,PopUp());
    CONNECTSIGNALSLOTGUI(mp_MainWindow, ShowWarningMsgDlg(), mp_WarningMsgDlg,PopUp());
}

/****************************************************************************/
/**
 * \brief Adding event messages to the List
 *
 * \iparam EventMsgStream = EventMessage data in the data stream format
 *
 */
/****************************************************************************/
void CStatusBarManager::AddEventMessages(QDataStream &EventMsgStream)
{
    QList<MsgData>::Iterator ErrMsgIterator;
    QList<MsgData>::Iterator WarnMsgIterator;
    quint32 EventType;
    MsgData Msg;
    EventMsgStream >> Msg.ID >> EventType >> Msg.MsgString >> Msg.Time;
    Msg.EventType = static_cast<Global::EventType>(EventType);
    quint64 ID =  (Msg.ID & 0xffffffff00000000) >> 32;
    if (Msg.EventType == Global::EVTTYPE_WARNING) {
        if (m_WarningEventIDList.contains(ID) == false) {
            m_WarningMsgList.append(Msg);
            m_WarningEventIDList.append(ID);
        }
        else {
            int MsgCount=0;
            //Iterate Through the List
            for (WarnMsgIterator = m_WarningMsgList.begin(); WarnMsgIterator!= m_WarningMsgList.end();++WarnMsgIterator) {
                MsgData EventData = *WarnMsgIterator;
                quint64 CurrentEventID =  (EventData.ID & 0xffffffff00000000) >> 32;
                if (CurrentEventID == ID) {
                    //Remove the Element from the list which contain the EventID to be removed
                    m_WarningMsgList.removeAt(MsgCount);
                    (void) m_WarningEventIDList.removeOne(EventData.ID);
                    break;
                }
                MsgCount++;
            }
            // Add the latest warning event to the WarningEventList
            m_WarningMsgList.append(Msg);
            m_WarningEventIDList.append(ID);
        }
        qDebug() << "warning ID hash"<<m_WarningMsgList.count();
        if (m_WarningMsgList.count() > 0) {
            //Displaying the message icon when the list count is morethan 0
            (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::Warning);
        }
        //We have to display only 10 latest entries so restricting the count to 10
        if (m_WarningMsgList.count() > 20) {
            m_WarningMsgList.removeAt(0);
            m_WarningEventIDList.removeAt(0);
        }
        //Updating the Event Message data into a list
        mp_WarningMsgDlg->WarningMsgList(m_WarningMsgList);
        qDebug() << "Added in Warning hash" ;
    }
    else if (Msg.EventType == Global::EVTTYPE_ERROR) {
        int MsgCount=0;
        if (m_ErrorEventIDList.contains(ID) == false) {
            m_ErrorMsgList.append(Msg);
            m_ErrorEventIDList.append(ID);
        }
        else {
            //Iterate Through the List
            for (ErrMsgIterator = m_ErrorMsgList.begin(); ErrMsgIterator!= m_ErrorMsgList.end();++ErrMsgIterator) {
                MsgData EventData = *ErrMsgIterator;
                quint64 CurrentEventID =  (EventData.ID & 0xffffffff00000000) >> 32;
                if (CurrentEventID == ID) {
                    //Remove the Element from the list which contain the EventID to be removed
                    m_ErrorMsgList.removeAt(MsgCount);
                    (void) m_ErrorEventIDList.removeOne(EventData.ID);
                    break;
                }
                MsgCount++;
            }
            // Add the latest error event to the ErrorEventList
            m_ErrorMsgList.append(Msg);
            m_ErrorEventIDList.append(ID);
        }
        qDebug() << "Error ID hash"<< m_ErrorMsgList.count();
        if (m_ErrorMsgList.count() > 0) {
            //Displaying the message icon when the list count is morethan 0
            (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::Error);
        }
        //We have to display only 10 latest entries so restricting the count to 10
        if (m_ErrorMsgList.count() > 20) {
            m_ErrorMsgList.removeAt(0);
            m_ErrorEventIDList.removeAt(0);
        }
        //Updating the Event Message data into a list
        mp_ErrorMsgDlg->ErrorMsgList(m_ErrorMsgList);
    }
    qDebug() << "Event ID in message box is" << Msg.ID;
}

/****************************************************************************/
/**
 * \brief Removing event messages From the List.
 *
 * \iparam  EventType = Event type
 * \iparam  EventID = Event Id
 */
/****************************************************************************/
void CStatusBarManager::RemoveEventMessages(Global::EventType EventType, quint64 EventID)
{
    QList<MsgData>::Iterator ErrMsgIterator;
    QList<MsgData>::Iterator WarnMsgIterator;
    //Check for the Error type Message
    if (EventType == Global::EVTTYPE_ERROR) {
        int MsgCount=0;
        //Iterate Through the List
        for (ErrMsgIterator = m_ErrorMsgList.begin(); ErrMsgIterator!= m_ErrorMsgList.end();++ErrMsgIterator) {
            MsgData EventData = *ErrMsgIterator;
            if (EventData.ID == EventID) {
                //Remove the Element from the list which contain the EventID to be removed
                m_ErrorMsgList.removeAt(MsgCount);
                (void) m_ErrorEventIDList.removeOne(EventData.ID);
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
        for (WarnMsgIterator = m_WarningMsgList.begin(); WarnMsgIterator!= m_WarningMsgList.end();++WarnMsgIterator) {
            MsgData EventData = *WarnMsgIterator;
            if (EventData.ID == EventID) {
                //Remove the Element from the list which contain the EventID to be removed
                m_WarningMsgList.removeAt(MsgCount);
                (void) m_WarningEventIDList.removeOne(EventData.ID);
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
}

/****************************************************************************/
/**
 * \brief Set the Process state icon.
 *
 * \iparam  ProcessState = True or False
 */
/****************************************************************************/
void CStatusBarManager::SetProcessState(bool &ProcessState)
{
    if (ProcessState) {
        (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::ProcessRunning);
    }
    else {
        (void)mp_MainWindow->UnsetStatusIcons(MainMenu::CMainWindow::ProcessRunning);
    }
}
/****************************************************************************/
/**
 * \brief Set the RemoteCare state icon.
 *
 * \iparam  RemoteCareState = True or False
 */
/****************************************************************************/
void CStatusBarManager::SetRemoteCareState(bool &RemoteCareState)
{
    if (RemoteCareState) {
        (void)mp_MainWindow->SetStatusIcons(MainMenu::CMainWindow::RemoteCare);
    }
    else {
        (void)mp_MainWindow->UnsetStatusIcons(MainMenu::CMainWindow::RemoteCare);
    }
}

/****************************************************************************/
/*!
 *  \brief This slots Sets/Unsets the remote care icon on Status Bar.
 *
 *  \iparam DisableStatus = True or False
 *
 */
/****************************************************************************/
void CStatusBarManager::OnDisableRemoteCareEvents(const bool DisableStatus)
{
    bool SetIcon = !DisableStatus;
    SetRemoteCareState(SetIcon);
}

} // end namespace EventHandling


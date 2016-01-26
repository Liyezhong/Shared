/****************************************************************************/
/*! \file MsgBoxManager.h
 *
 *  \brief Header file for class CMsgBoxManager.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-10-18
 *   $Author:  $ N.Kamath
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
#ifndef MSGBOXMANAGER_H
#define MSGBOXMANAGER_H
// QT Headers
#include <QDataStream>
#include <QHash>
#include <QTimer>
#include <QDateTime>
//Project headers
#include <Global/Include/PriorityQueue.h>
#include <Global/Include/GlobalDefines.h>
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "NetCommands/Include/CmdAcknEventReport.h"


namespace MainMenu {
    class CMessageDlg;
    class StatusBarManager;
}


namespace MainMenu {

/****************************************************************************/
/**
 *  \brief Structure for Message box manager.
 */
/****************************************************************************/
struct MsgData{
    bool EventStatus;  //!< True to close dialog
    Global::EventType EventType; //!< EventType
    Global::GuiButtonType BtnType; //!< GUI button type
    QString Time; //!< Time at which event occured
    quint64 ID; //!< Event ID
    QString MsgString;  //!< Event String
    bool StatusBarIcon; //!< True to store event and display when status bar icon is clicked
    QString BtnEnableConditions; //!< button enable conditions: RT_LID_OPEN_CLOSE
    QString AutoQuitMsgBoxTime; //< timeout for the message if no user responsing
};

/****************************************************************************/
/**
 *  \brief Message Box manager managing Message box displayed for
 *         CmdEventReport
 */
/****************************************************************************/
class CMsgBoxManager : public QObject {
    Q_OBJECT
    friend class  CTestMainMenu;
public:
    CMsgBoxManager(QWidget *p_Parent, DataManager::CUserSettingsInterface *p_SettingsInterface);
    ~CMsgBoxManager();
    void Manage(QDataStream &DS, Global::tRefType Ref);
    void SetMessageBoxType(MainMenu::CMessageDlg *MsgDlg, MsgData MessageBoxData);
    //!< Data Structure containing Data Sent from EventReport
    void EnableOKButton();
    void SetRTLidStatusOpen();
private:
    //Data Members
    Global::PriorityQueue <quint64> m_PriorityQueue;  //!< Priority Queue consisting of EventID's
    qint64 m_CurrentMsgBoxEventID;                   //!< If a Event Msg box is displayed , the EventID associated with it.
    MainMenu::CMessageDlg *mp_MessageDlg;             //!< The msg dialog
    QHash<quint64,MainMenu::CMessageDlg*> m_MsgDlgEventIDHash; //!< hash of event id to message dlg.
    QWidget *mp_Parent;                               //!< Parent for Msg Dlg
    DataManager::CUserSettingsInterface *mp_SettingsInterface;  //!< UserSettings Interface

    //!< we use this hash since we store only event id in queue
    QHash <quint64, MsgData> m_EventIDMsgDataHash; //!< Hash used to get MsgData Struct .
    MsgData m_CurrentMsgData;                      //!< Data Structure containing Data Sent from EventReport
    MsgData m_DisplayedMsgData;                    //!< The data currently displayed in Mesg Box

    /*! Hash table for storing EventId  associated with Command reference.
     * This  will be used when we user clicks a button on the Mesg box, result of which
     * is to inform main about user action.We will send the CmdRef through Ack/Nack.This
     * will enable event Handler to identify the Event ID
     */
    QHash<quint64, Global::tRefType> m_EvenIDCmdRefHash; //!< Hash of EventID(Key) and CmdRef(value)
    QTimer m_PopupTimer;    //!< When this timer times out, MsgBox will be poped out if queue not empty.
    bool m_bMsgWaiting;    //!<it has a "OK" button, but it is disabled.
    //----------------End of Members------------------------//

    //Methods
    Q_DISABLE_COPY(CMsgBoxManager) //!< disable copy construction and assignment operation

    MainMenu::CMessageDlg *  CreateMesgBox(MsgData MsgDataStruct);
    void RemoveMsgBoxFromQueue(quint64 ID);
    void RemoveDataFromContainers(quint64 ID);
    void AddMsgBoxToQueue(Global::tRefType Ref, MsgData &CurrentMsgData);

    /****************************************************************************/
    /*! \brief Sends the message data to Status bar manager.
     *
     *  \iparam EventMsgStruct = Event Message structure
     */
     /****************************************************************************/
    void SendMsgsToStatusBar(MsgData EventMsgStruct);


private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     *  \param ID message box id
     */
    /****************************************************************************/
    void ButtonLeftClicked(quint64 ID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     *  \param ID message box id
     */
    /****************************************************************************/
    void ButtonCenterClicked(quint64 ID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     *  \param ID message box id
     */
    /****************************************************************************/
    void ButtonRightClicked(quint64 ID);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ShowMsgBoxIfQueueNotEmpty();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ShowNextMsgBoxInQueue();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void LanguageChanged();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     *  \param ID quint64 messagebox id
     */
    /****************************************************************************/
    void AutoQuitMessageBox(quint64 ID);

signals:
    /****************************************************************************/
    /*! \brief This Signal is emitted when a button on the mesg
     *         box is clicked. The CmdRef is the one which was
     *         generated when CmdEventReport was sent to GUI
     *         for this particular MsgBox.
     *
     *  \iparam ClickedButton = Clicked button type.
     *  \iparam CmdRef = Command reference
     *  \iparam EventKey = Event key
     */
     /****************************************************************************/
    void EventReportAck(NetCommands::ClickedButton_t ClickedButton, Global::tRefType CmdRef, quint64 EventKey);
};

}

#endif // MSGBOXMANAGER_H

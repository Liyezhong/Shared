/****************************************************************************/
/*! \file StatusBarManager.h
 *
 *  \brief Definition file for class StatsuBarManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-11-02
 *  $Author:    $ Bhanu Prasad
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 * \warning This object lives in HimalayaMaster Thread
 */
/****************************************************************************/

#ifndef MAINMENU_STATUSBARMANAGER_H
#define MAINMENU_STATUSBARMANAGER_H
#include <QDataStream>
#include <QHash>
#include <QTimer>

//Project headers
#include <Global/Include/PriorityQueue.h>
#include <Global/Include/GlobalDefines.h>
#include "MainMenu/Include/ErrorMsgDlg.h"
#include "MainMenu/Include/WarningMsgDlg.h"
#include "MainMenu/Include/MainWindow.h"
#include <QDateTime>
#include <QObject>
#include <QDebug>
#include <MainMenu/Include/MsgBoxManager.h>

//namespace StatusBar {
//    class CErrorMsgDlg;
//    class CWarningMsgDlg;
//}

namespace MainMenu {

class StatusBarManager : public QWidget {
    Q_OBJECT
private:
    static StatusBarManager *mp_StatusBarMgrInstance; //!< Static instance
    StatusBarManager(const StatusBarManager &);   ///< Not implemented.
    CErrorMsgDlg *mp_ErrorMsgDlg; //!< Error Dialog instance
    CWarningMsgDlg *mp_WarningMsgDlg;//!< Warning Dialog Instance
    MainMenu::CMainWindow *mp_MainWindow;           //!< Main window of the GUI
    DataManager::CUserSettingsInterface *mp_UsrSettingsInterface; //!< UserSettings interface pointer
    //MainMenu::MsgData EventMsgStruct; //!< Event Message Structure
    //QHash <quint64, MsgData> m_ErrorIDMsgHash; //!< Hash for Error Messages
    //QHash <quint64, MsgData> m_WarningIDMsgHash; //!< Hash for Warning Messages
    QList <MsgData> m_ErrorMsgList; //!< List for Error Messages Struct
    QList <MsgData> m_WarningMsgList; //!< List for Warning Messages Struct
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    StatusBarManager();


public:


    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~StatusBarManager() {
    }
    /****************************************************************************/
    /**
     * \brief Argumented constructor.
     */
    /****************************************************************************/
    StatusBarManager(MainMenu::CMainWindow *p_Window,DataManager::CUserSettingsInterface *p_UsrSettingsInterface);

    /****************************************************************************/
    /**
     * \brief Get pointer to StatusBarManager.
     *
     * \return      pointer to instance.
     */
    /****************************************************************************/
    static inline StatusBarManager *GetInstance() {
            return mp_StatusBarMgrInstance;
    }

    /****************************************************************************/
    /**
     * \brief Adding event messages to the List
     * \iparam EventMsgStream = EventMessage data in the data stream format
     *
     */
    /****************************************************************************/
    void AddEventMessages(QDataStream &EventMsgStream);
    /****************************************************************************/
    /**
     * \brief Removing event messages From the List
     * \iparam  EventType = Event type
     * \iparam  ID = Event Id
     */
    /****************************************************************************/
    void RemoveEventMessages(Global::EventType EventType, quint64 ID);
    /****************************************************************************/
    /**
     * \brief Get pointer to StatusBarManager. This method is called for the first time
     *        to get the pointer to StatusBarManager.
     * \iparam p_MainWindow = Pointer to the mainwindow
     * \return      pointer to StatusBarManager instance.
     */
    /****************************************************************************/
    static StatusBarManager *CreateInstance(MainMenu::CMainWindow *p_MainWindow,DataManager::CUserSettingsInterface *p_UserSettingsInterface) {

        if (!mp_StatusBarMgrInstance) {
            mp_StatusBarMgrInstance = new StatusBarManager(p_MainWindow,p_UserSettingsInterface);
            return mp_StatusBarMgrInstance;
        }
        else
            return mp_StatusBarMgrInstance;
    }
signals:
    void ShowErrorPopup();
    void ShowWarningPopup();

}; // end class StatusBarManager

} // end namespace MainMenu

#endif //MAINMENU_STATUSBARMANAGER_H

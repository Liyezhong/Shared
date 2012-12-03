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
 * \warning This object lives in ColoradoMaster Thread
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
    struct EventMsgStruct{
        Global::EventType EventType; //!< EventType
        QString Time; //!< Time at which event occured
        quint64 ID; //!< Event ID
        QString EventString;  //!< Event String
    };
    QHash <quint64, EventMsgStruct> m_ErrorIDMsgHash; //!< Hash for Error Messages
    QHash <quint64, EventMsgStruct> m_WarningIDMsgHash; //!< Hash for Warning Messages
    CErrorMsgDlg *mp_ErrorMsgDlg; //!< Error Dialog instance
    CWarningMsgDlg *mp_WarningMsgDlg;//!< Warning Dialog Instance
    MainMenu::CMainWindow *mp_MainWindow;           //!< Main window of the GUI
    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    StatusBarManager();

    /****************************************************************************/
    /**
     * \brief Retriving the Event ID and Error Message from Error Message Hash
     * \iparam  ErrorIdMsgHash = Hash with Error Id and Error Msg data
     *
     */
    /****************************************************************************/
    void ErrorMsgList(QHash <quint64, EventMsgStruct> ErrorIdMsgHash);
    /****************************************************************************/
    /**
     * \brief Retriving the Event ID and Warning Message from Warning Message Hash
     * \iparam  WarningIdMsgHash = Hash with Warning Id and Error Msg data
     *
     */
    /****************************************************************************/
    void WarningMsgList(QHash <quint64, EventMsgStruct> WarningIdMsgHash);//!< List object for Error Messages

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
    StatusBarManager(MainMenu::CMainWindow *p_Window);

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
    static StatusBarManager *CreateInstance(MainMenu::CMainWindow *p_MainWindow) {

        if (!mp_StatusBarMgrInstance) {
            mp_StatusBarMgrInstance = new StatusBarManager(p_MainWindow);
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

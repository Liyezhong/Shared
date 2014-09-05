/****************************************************************************/
/*! \file StatusBarManager.h
 *
 *  \brief Header file for class CStatusBarManager.
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
#include <MainMenu/Include/MsgBoxManager.h>


namespace MainMenu {

/****************************************************************************/
/**
 *  \brief Status Bar manager managing status bar icons.
 */
/****************************************************************************/
class CStatusBarManager : public QWidget {

    Q_OBJECT
    friend class  CTestMainMenu;

private:
    static CStatusBarManager *mp_StatusBarMgrInstance;   //!< Static instance
    CErrorMsgDlg *mp_ErrorMsgDlg;                       //!< Error Dialog instance
    CWarningMsgDlg *mp_WarningMsgDlg;                   //!< Warning Dialog Instance
    MainMenu::CMainWindow *mp_MainWindow;               //!< Main window of the GUI
    DataManager::CUserSettingsInterface *mp_UsrSettingsInterface; //!< UserSettings interface pointer
    QList <MsgData> m_ErrorMsgList;                     //!< List for Error Messages Struct
    QList <MsgData> m_WarningMsgList;                   //!< List for Warning Messages Struct
    QList <quint64> m_ErrorEventIDList;                 //!< List containing Error Event ID's
    QList <quint64> m_WarningEventIDList;               //!< List containing Warning Event ID's


    /****************************************************************************/
    /**
     * \brief Constructor.
     */
    /****************************************************************************/
    CStatusBarManager();
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CStatusBarManager)


public:

    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CStatusBarManager();

    /****************************************************************************/
    /**
     * \brief Argumented constructor.
     */
    /****************************************************************************/
    CStatusBarManager(MainMenu::CMainWindow *p_Window,DataManager::CUserSettingsInterface *p_UsrSettingsInterface);

    /****************************************************************************/
    /**
     * \brief Get pointer to StatusBarManager.
     *
     * \return      pointer to instance.
     */
    /****************************************************************************/
    static inline CStatusBarManager *GetInstance() {
            return mp_StatusBarMgrInstance;
    }

    /****************************************************************************/
    /**
     * \brief Adding event messages to the List
     *
     * \iparam EventMsgStream = EventMessage data in the data stream format
     */
    /****************************************************************************/
    void AddEventMessages(QDataStream &EventMsgStream);

    /****************************************************************************/
    /**
     * \brief Removing event messages From the List
     *
     * \iparam  EventType = Event type
     * \iparam  EventID = Event Id
     */
    /****************************************************************************/
    void RemoveEventMessages(Global::EventType EventType, quint64 EventID);

    /****************************************************************************/
    /**
     * \brief Get pointer to StatusBarManager. This method is called for the first time
     *        to get the pointer to StatusBarManager.
     *
     * \iparam p_MainWindow = Pointer to the mainwindow
     *  \iparam p_UserSettingsInterface = UserSettingsInterface object
     *
     * \return Pointer to StatusBarManager instance.
     */
    /****************************************************************************/
    static CStatusBarManager *CreateInstance(MainMenu::CMainWindow *p_MainWindow,
                                            DataManager::CUserSettingsInterface *p_UserSettingsInterface) {

        if (!mp_StatusBarMgrInstance) {
            mp_StatusBarMgrInstance = new CStatusBarManager(p_MainWindow,p_UserSettingsInterface);
            return mp_StatusBarMgrInstance;
        }
        else
            return mp_StatusBarMgrInstance;
    }

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetProcessState
     */
    /****************************************************************************/
    void SetProcessState(bool &ProcessState);
    /*************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot SetRemoteCareState
     */
    /****************************************************************************/
    void SetRemoteCareState(bool &RemoteCareState);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot OnDisableRemoteCareEvents
     */
    /****************************************************************************/
    void OnDisableRemoteCareEvents(const bool DisableStatus);

signals:
    /****************************************************************************/
    /**
     * \brief This signal is emitted whenever error event message is sent from Main.
     *
     */
    /****************************************************************************/
    void ShowErrorPopup();

    /****************************************************************************/
    /**
     * \brief This signal is emitted whenever warning event message is sent from Main.
     *
     */
    /****************************************************************************/
    void ShowWarningPopup();

}; // end class StatusBarManager

} // end namespace MainMenu

#endif //MAINMENU_STATUSBARMANAGER_H

/****************************************************************************/
/*! \file ErrorMsgDlg.h
 *
 *  \brief ErrorMsgDlg definition.
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

#ifndef MAINMENU_ERRORMSGDLG_H
#define MAINMENU_ERRORMSGDLG_H

#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include <QTextEdit>
#include "MainMenu/Include/MsgBoxManager.h"
#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"



namespace MainMenu {

namespace Ui {
    class CErrorMsgDlg;
}

/****************************************************************************/
/**
 * \brief Content panel for dialogs displaying text files.
 */
/****************************************************************************/
class CErrorMsgDlg : public MainMenu::CDialogFrame
{
    Q_OBJECT

public:
    explicit CErrorMsgDlg(QWidget *p_Parent = NULL , QWidget *p_MainWindow = NULL,DataManager::CUserSettingsInterface *p_UserSettingsInterface=NULL);
    virtual ~CErrorMsgDlg();
    void SetCaption(QString Caption);
    void SetText(QStringList Text);
    void SetErrorMsgList();
    void RetranslateUI();
    QString GetDateAndTime(QString);

    /****************************************************************************/
    /**
     * \brief Retriving the Event ID and Error Message from Error Message Hash
     * \iparam  ErrorIdMsgHash = Hash with Error Id and Error Msg data
     *
     */
    /****************************************************************************/
    void ErrorMsgList(QList <MsgData> ErrorIDStructList);
private:
    Ui::CErrorMsgDlg *mp_Ui; //!< User interface
    QTextEdit *mp_TextEdit; //!< Widget displaying a text file
    MainMenu::CMainWindow *mp_MainWidow;    //!< Pointer to  main window
    QList <MsgData> m_ErrorMsgList;   //!< List for Error messages
    DataManager::CUserSettingsInterface *mp_SettingsInterface;  //!< UserSettings Interface


private slots:
    void PopUp();

protected:
    void changeEvent(QEvent *p_Event);
};

} // end namespace MainMenu

#endif // MAINMENU_ERRORMSGDLG_H

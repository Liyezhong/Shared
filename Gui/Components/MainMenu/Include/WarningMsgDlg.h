/****************************************************************************/
/*! \file WarningMsgDlg.h
 *
 *  \brief Header file for class CWarningMsgDlg.
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

#ifndef MAINMENU_WARNINGMSGDLG_H
#define MAINMENU_WARNINGMSGDLG_H

#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/DialogFrame.h"
#include "MainMenu/Include/MainWindow.h"
#include <QTextEdit>
#include "MainMenu/Include/MsgBoxManager.h"

//lint -e435

namespace MainMenu {

namespace Ui {
    class CWarningMsgDlg;
}

/****************************************************************************/
/**
 * \brief Content panel for dialogs displaying text files.
 */
/****************************************************************************/
class CWarningMsgDlg : public MainMenu::CDialogFrame
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    explicit CWarningMsgDlg(QWidget *p_Parent = NULL , QWidget *p_MainWindow = NULL,
                            DataManager::CUserSettingsInterface *p_UserSettingsInterface=NULL);
    virtual ~CWarningMsgDlg();
    void SetCaption(QString Caption);
    void SetText(QStringList Text);
    void SetWarningMsgList();

    QString GetDateAndTime(QString);

    void WarningMsgList(QList <MsgData> ErrorIDStructList);

private:
    Ui::CWarningMsgDlg *mp_Ui;      //!< User interface
    QTextEdit *mp_TextEdit;         //!< Widget displaying a text file
    MainMenu::CMainWindow *mp_MainWidow;    //!< Pointer to  main window
    QList <MsgData> m_WarnMsgList;  //!< List for warning messages
    DataManager::CUserSettingsInterface *mp_SettingsInterface;  //!< UserSettings Interface
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CWarningMsgDlg)

private slots:
    void PopUp();
    void RetranslateUI();
protected:
    void changeEvent(QEvent *p_Event);
};

} // end namespace MainMenu

#endif // MAINMENU_WARNINGMSGDLG_H

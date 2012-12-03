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
    explicit CErrorMsgDlg(QWidget *p_Parent = NULL , QWidget *p_MainWindow = NULL);
    virtual ~CErrorMsgDlg();
    void SetCaption(QString Caption);
    void SetText(QStringList Text);
    void SetErrorMsgList(QStringList,QStringList,QStringList);
    void RetranslateUI();

private:
    Ui::CErrorMsgDlg *mp_Ui; //!< User interface
    QTextEdit *mp_TextEdit; //!< Widget displaying a text file
    MainMenu::CMainWindow *mp_MainWidow;    //!< Pointer to  main window


private slots:
    void PopUp();

protected:
    void changeEvent(QEvent *p_Event);
};

} // end namespace MainMenu

#endif // MAINMENU_ERRORMSGDLG_H

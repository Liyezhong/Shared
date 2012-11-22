/****************************************************************************/
/*! \file MessageDlg.h
 *
 *  \brief MessageDlg definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-28
 *   $Author:  $ M.Scherer
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

#ifndef MAINMENU_MESSAGEDLG_H
#define MAINMENU_MESSAGEDLG_H

#include "MainMenu/Include/DialogFrame.h"
#include <QMessageBox>

namespace MainMenu {

namespace Ui {
    class CMessageDlg;
}

/****************************************************************************/
/**
 * \brief This class displays a information, warning or error message box.
 */
/****************************************************************************/
class CMessageDlg : public CDialogFrame
{
    Q_OBJECT

private:
    Ui::CMessageDlg *mp_Ui; //!< User interface

public:
    //! Enum describes the message severity
    typedef enum {
        Information,
        Warning,
        Critical
    } Severity_t;

    explicit CMessageDlg(QWidget *p_Parent = NULL);
    virtual ~CMessageDlg();
    void SetTitle(const QString &Title);
    void SetText(const QString &Text);
    void SetIcon(QMessageBox::Icon);
    void SetButtonText(qint32 ButtonNumber, QString ButtonText);
    void HideCenterButton();
    void HideButtons();

public slots:
    void Show();

protected:
    void changeEvent(QEvent *p_Event);
};

} // end namespace MainMenu

#endif // MAINMENU_MESSAGEDLG_H

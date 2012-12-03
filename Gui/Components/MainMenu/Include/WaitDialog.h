/****************************************************************************/
/*! \file WaitDialog.h
 *
 *  \brief WaitDialog definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-22
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

#ifndef MAINMENU_WAITDIALOG_H
#define MAINMENU_WAITDIALOG_H

#include "MainMenu/Include/DialogFrame.h"
#include <QTimer>

namespace MainMenu {

namespace Ui {
    class CWaitDialog;
}

/****************************************************************************/
/**
 * \brief This dialog is started, when the user has to wait for the result of
 *        a background process.
 */
/****************************************************************************/
class CWaitDialog : public MainMenu::CDialogFrame
{
    Q_OBJECT

public:
    explicit CWaitDialog(QWidget *p_Parent = 0);
    virtual ~CWaitDialog();
    void SetText(QString Text);
    void SetTimeout(qint32 MilliSeconds);
    void HideAbort();
    void show();

public slots:
    void done(int Result);

protected:
    bool eventFilter(QObject *p_Object, QEvent *p_Event);
    void changeEvent(QEvent *p_Event);

private:
    Ui::CWaitDialog *mp_Ui;     //!< User interface
    bool m_AbortButtonVisible;  //!< Is the abort button visible?
    bool m_DialogLock;          //!< Prevents closing the dialog
    bool m_DialogHide;          //!< Should the dialog be closed?
    QTimer m_Timer;             //!< Timeout timer

private slots:
    void LockDialog();
    void UnlockDialog();

signals:
    void Timeout();
};

} // end namespace MainMenu

#endif // MAINMENU_WAITDIALOG_H

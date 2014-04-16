/****************************************************************************/
/*! \file WaitDialog.cpp
 *
 *  \brief Implementation of file for class CWaitDialog.
 *
 *  \b Description:
 *          This class implements a base widget for displaying the wait dialogs.
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/WaitDialog.h"
#include "ui_WaitDialog.h"
#include <QDebug>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CWaitDialog::CWaitDialog(QWidget *p_Parent) : MainMenu::CDialogFrame(p_Parent), mp_Ui(new Ui::CWaitDialog)
{
    mp_Ui->setupUi(GetContentFrame());
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    m_AbortButtonVisible = true;
    m_DialogLock = false;
    m_DialogHide = false;
    m_Timer.setSingleShot(true);
    m_IsBlgProcessStarted = false;
    setModal(true);

    CONNECTSIGNALSLOTGUI(mp_Ui->abortButton, clicked(), this, AbortWaitDialog());
    CONNECTSIGNALSLOTGUI(&m_Timer, timeout(), this, reject());
    CONNECTSIGNALSIGNALGUI(&m_Timer, timeout(), this, Timeout());    

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CWaitDialog::~CWaitDialog()
{
    try {
        delete mp_Ui;
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief Sets the text displayed in the wait dialog
 *
 *  \iparam Text = Label text
 */
/****************************************************************************/
void CWaitDialog::SetText(QString Text)
{
    mp_Ui->contentLabel->setText(QString("%1").arg(Text));
}

/****************************************************************************/
/*!
 *  \brief Activates a timeout timer
 *
 *  \iparam Milliseconds = Timeout in milliseconds
 */
/****************************************************************************/
void CWaitDialog::SetTimeout(qint32 Milliseconds)
{
    m_Timer.start(Milliseconds);
}

/****************************************************************************/
/*!
 *  \brief Abort wait dialog
 */
/****************************************************************************/
void CWaitDialog::AbortWaitDialog()
{
    if (m_IsBlgProcessStarted) {
        m_IsBlgProcessStarted = false;
        //Send command to main to abort blg process and park the robot arms
        emit AbortBlgProcess(true);
        reject();
    }
    else {
        m_IsBlgProcessStarted = false;
        reject();
    }
}

/****************************************************************************/
/*!
 *  \brief Abort Bathlayout generating process
 */
/****************************************************************************/
void CWaitDialog::BlgProcessProgress(bool IsBlgProcessStarted)
{
    m_IsBlgProcessStarted = IsBlgProcessStarted;
}

/****************************************************************************/
/*!
 *  \brief Hides the abort button of the dialog
 */
/****************************************************************************/
void CWaitDialog::HideAbort()
{
    mp_Ui->abortButton->hide();
    m_AbortButtonVisible = false;
}

/****************************************************************************/
/*!
 *  \brief Overrides the show function of QDialog
 *
 *      If the abort button of this message box is enabled, this method will
 *      immediately show the dialog. If this is not the case, the dialog will
 *      be shown after a time of 500 ms.
 */
/****************************************************************************/
void CWaitDialog::show()
{
    if(m_AbortButtonVisible == true) {
        CDialogFrame::show();
    }
    else {
        m_DialogLock = false;
        m_DialogHide = false;
        qApp->installEventFilter(this);
        LockDialog();
    }
}

/****************************************************************************/
/*!
 *  \brief Overrides the done function of QDialog
 *
 *      Only If the message box is not locked, the dialog will be closed
 *      immediately.
 *
 *  \iparam Result = Dialog result code
 */
/****************************************************************************/
void CWaitDialog::done(int Result)
{
    m_DialogHide = true;
    m_Timer.stop();
    if (m_DialogLock == false) {
        CDialogFrame::done(Result);
    }
}

/****************************************************************************/
/*!
 *  \brief Filters all mouse events
 *
 *      There can be a small period of time in which the wait dialog is not
 *      displayed. This is required so that the wait dialog does not have to
 *      be displayed everytime network communication takes place.
 *
 *  \iparam p_Object = Object that is watched
 *  \iparam p_Event = Current event
 *
 *  \return True if an event should be filtered
 */
/****************************************************************************/
bool CWaitDialog::eventFilter(QObject *p_Object, QEvent *p_Event)
{
    if (p_Event->type() == QEvent::MouseButtonPress || p_Event->type() == QEvent::MouseButtonRelease) {
        return true;
    }
    else {
        // standard event processing
        return QObject::eventFilter(p_Object, p_Event);
    }
}

/****************************************************************************/
/*!
 *  \brief Blocks the dialog for closing
 *
 *      This function helps to assure that the dialog is displayed for at
 *      least 500 ms. This needed avoid the flickering of the dialog, when
 *      it is displayed only for a short period of time.
 */
/****************************************************************************/
void CWaitDialog::LockDialog()
{
    qApp->removeEventFilter(this);
    if (m_DialogHide == false) {
        CDialogFrame::show();
        for (int Count =0;Count < 5 ;Count++) {
          QCoreApplication::processEvents();
        }
        m_DialogLock = true;
        QTimer::singleShot(500, this, SLOT(UnlockDialog()));
    }
}

/****************************************************************************/
/*!
 *  \brief Unblocks the dialog for closing
 */
/****************************************************************************/
void CWaitDialog::UnlockDialog()
{
    m_DialogLock = false;
    if (m_DialogHide == true) {
        accept();
    }
}

} // end namespace MainMenu

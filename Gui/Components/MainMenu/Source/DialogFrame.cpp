/****************************************************************************/
/*! \file DialogFrame.cpp
 *
 *  \brief Implementation of file for class CDialogFrame.
 *
 *  \b Description:
 *          This class implements a base widget to display Dialogs.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-08
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

#include "MainMenu/Include/DialogFrame.h"
#include "ui_DialogFrame.h"
#include <QMainWindow>
#include <QShowEvent>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CDialogFrame::CDialogFrame(QWidget *p_Parent, QMainWindow *pMainWindow) : QDialog(p_Parent, Qt::FramelessWindowHint),
    mp_DialogUi(new Ui::CDialogFrame), mp_MainWindow(pMainWindow)
{
    mp_DialogUi->setupUi(this);
    mp_DialogUi->widget->SetTitleCenter();
    (void) connect(mp_DialogUi->widget, SIGNAL(LanguageChanged()), this, SIGNAL(DialogLangaugeChanged()));
    setAttribute(Qt::WA_TranslucentBackground);
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDialogFrame::~CDialogFrame()
{
    try {
        delete mp_DialogUi;
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CDialogFrame::changeEvent(QEvent *p_Event)
{
    QDialog::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_DialogUi->retranslateUi(this);
            break;
        default:
            break;
    }
}


void CDialogFrame::showEvent(QShowEvent *p_Event)
{
    QDialog::showEvent(p_Event);
    if (mp_MainWindow && (p_Event->type() == QShowEvent::Show))
    {
        QRect scr = mp_MainWindow->geometry();
        this->move( scr.center() - this->rect().center());
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the title of the dialog
 *
 *  \iparam Title = Title of the dialog
 */
/****************************************************************************/
void CDialogFrame::SetDialogTitle(QString Title)
{
    mp_DialogUi->widget->SetPanelTitle(QString("%1").arg(Title));
    mp_DialogUi->widget->SetTitleCenter();
}

/****************************************************************************/
/*!
 *  \brief Sets two titles, one at left corner and other at right corner
 *
 *  \iparam TitleLeftCorner  = Title at left corner
 *  \iparam TitleRightCorner = Title at right corner
 */
/****************************************************************************/
void CDialogFrame::SetDialogTitle(QString TitleLeftCorner, QString TitleRightCorner)
{
    mp_DialogUi->widget->SetPanelTitle(TitleLeftCorner, TitleRightCorner);
}

/****************************************************************************/
/*!
 *  \brief Sets the content of the dialog
 *
 *  \iparam pContent = Content layout of the dialog
 */
/****************************************************************************/
void CDialogFrame::SetContent(QLayout *pContent)
{
    mp_DialogUi->widget->SetContent(pContent);
}

/****************************************************************************/
/*!
 *  \brief Returns the content frame of the dialog
 *
 *  \return Content frame of the dialog
 */
/****************************************************************************/
QWidget *CDialogFrame::GetContentFrame()
{
    return mp_DialogUi->widget->GetContentFrame();
}

/****************************************************************************/
/*!
 *  \brief Chop dialog title's text as per the maximum title string length.
 *
 *  \iparam StringLength = Maximum string lenght of dialog title.
 *  \iparam TitleText = String to be set
 *
 */
/****************************************************************************/
void CDialogFrame::SetMaxStringDialogTitle(qint32 StringLength, QString TitleText)
{
    mp_DialogUi->widget->SetMaxStringPanelTitle(StringLength, TitleText);
}

/****************************************************************************/
/*!
 *  \brief Specify the MainWindow for position itself in the center of this MainWindow.
 *
 *  \iparam StringLength = Maximum string lenght of dialog title.
 *  \iparam TitleText = String to be set
 *
 */
/****************************************************************************/
void CDialogFrame::SetMainWindow(QMainWindow *pMainWindow)
{
    mp_MainWindow = pMainWindow;
}

} // end namespace MainMenu

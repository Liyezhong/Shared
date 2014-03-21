/****************************************************************************/
/*! \file DialogFrame.cpp
 *
 *  \brief DialogFrame implementation.
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

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CDialogFrame::CDialogFrame(QWidget *p_Parent) : QDialog(p_Parent, Qt::FramelessWindowHint), mp_DialogUi(new Ui::CDialogFrame)
{
    mp_DialogUi->setupUi(this);
    mp_DialogUi->widget->SetTitleCenter();

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
/****************************************************************************/
/*!
 *  \brief Sets the title of the dialog
 *
 *  \iparam Title = Title of the dialog
 */
/****************************************************************************/
void CDialogFrame::SetDialogTitle(QString Title)
{
    mp_DialogUi->widget->SetPanelTitle(tr("%1").arg(Title));
}

/****************************************************************************/
/*!
 *  \brief Sets two titles, one at left corner and other at right corner
 *
 *  \iparam TitleLeft  = Title at left corner
 *  \iparam TitleRight = Title at right corner
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

} // end namespace MainMenu

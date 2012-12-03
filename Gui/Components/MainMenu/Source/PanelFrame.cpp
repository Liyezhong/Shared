/****************************************************************************/
/*! \file PanelFrame.cpp
 *
 *  \brief PanelFrame implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-17
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

#include "MainMenu/Include/PanelFrame.h"
#include "Application/Include/LeicaStyle.h"
#include "ui_PanelFrame.h"
#include <QDebug>
#include <QPainter>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CPanelFrame::CPanelFrame(QWidget *p_Parent) : QWidget(p_Parent), mp_FrameUi(new Ui::CPanelFrame)
{
    mp_FrameUi->setupUi(this);
    m_IsDialog = false;
    bool StyleSize = dynamic_cast<Application::CLeicaStyle *>(qApp->style())->GetStyleSize();

    if (StyleSize == true) {
        mp_FrameUi->verticalLayout->setContentsMargins(10, 10, 10, 10);
        mp_FrameUi->panelTitle->setMinimumHeight(36);
    }

    QPalette Palette = mp_FrameUi->panelTitle->palette();
    Palette.setBrush(QPalette::WindowText, QColor(Qt::white));
    mp_FrameUi->panelTitle->setPalette(Palette);
    mp_FrameUi->panelTitleAdditional->setPalette(Palette);
    mp_FrameUi->panelTitleAdditional->hide();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CPanelFrame::~CPanelFrame()
{
    try {
        delete mp_FrameUi;
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

void CPanelFrame::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_FrameUi->retranslateUi(this);
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the title of the panel
 *
 *  \iparam Title = Title of the panel
 */
/****************************************************************************/
void CPanelFrame::SetPanelTitle(QString Title)
{
    mp_FrameUi->panelTitle->setText(tr("%1").arg(Title));
    mp_FrameUi->panelTitleAdditional->hide();
    mp_FrameUi->horizontalSpacer->changeSize(0, 0);
    mp_FrameUi->horizontalSpacer->invalidate();
}

/****************************************************************************/
/*!
 *  \brief Sets two titles, one at left corner and other at right corner
 *
 *  \iparam TitleLeft  = Title at left corner
 *  \iparam TitleRight = Title at right corner
 */
/****************************************************************************/
void CPanelFrame::SetPanelTitle(QString TitleLeft, QString TitleRight)
{
    mp_FrameUi->panelTitle->setText(tr("%1").arg(TitleLeft));
    mp_FrameUi->panelTitleAdditional->setText(tr("%1").arg(TitleRight));
    mp_FrameUi->panelTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mp_FrameUi->panelTitleAdditional->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
    mp_FrameUi->panelTitleAdditional->show();
}

/****************************************************************************/
/*!
 *  \brief Sets the content of the panel
 *
 *  \iparam pContent = Content layout of the panel
 */
/****************************************************************************/
void CPanelFrame::SetContent(QLayout *pContent)
{
    //lint -esym(429, pContent)
    mp_FrameUi->panelWidget->setLayout(pContent);
}


/****************************************************************************/
/*!
 *  \brief Returns the content frame of the panel
 *
 *  \return Content frame of the panel
 */
/****************************************************************************/
QWidget *CPanelFrame::GetContentFrame()
{
    return mp_FrameUi->panelWidget;
}

/****************************************************************************/
/*!
 *  \brief Centers the text in the title bar
 */
/****************************************************************************/
void CPanelFrame::SetTitleCenter()
{
    mp_FrameUi->panelTitle->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    m_IsDialog = true;
}

/****************************************************************************/
/*!
 *  \brief Draws the background image of the panel
 */
/****************************************************************************/
void CPanelFrame::paintEvent(QPaintEvent *)
{
    QPainter Painter(this);
    QPixmap Target(size());
    QPixmap Source;

    Target.fill(Qt::transparent);

    if (m_IsDialog == false) {
        Source = QPixmap(QString(":/%1/Panel.png").arg(Application::CLeicaStyle::GetStyleSizeString()));
    }
    else {
        Source = QPixmap(QString(":/%1/Popup/Popup.png").arg(Application::CLeicaStyle::GetStyleSizeString()));
    }

    if (Application::CLeicaStyle::GetStyleSize() == false) {
        Application::CLeicaStyle::BorderPixmap(&Target, &Source, 18, 32, 20, 21);
    }
    else {
        Application::CLeicaStyle::BorderPixmap(&Target, &Source, 29, 46, 29, 29);
    }

    Painter.drawPixmap(0, 0, Target);
}

} // end namespace MainMenu

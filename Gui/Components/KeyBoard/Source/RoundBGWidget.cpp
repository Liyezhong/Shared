/****************************************************************************/
/*! \file RoundBGWidget.cpp
 *
 *  \brief Implementation of background widget with rounded corners.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-10-19
 *   $Author:  $ N.Kamath
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


#include "KeyBoard/Include/RoundBGWidget.h"
#include "Application/Include/LeicaStyle.h"
#include <QPaintEvent>
#include <QDebug>

namespace KeyBoard {


/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CRoundWidget::CRoundWidget(QWidget *p_Parent) :
    QWidget(p_Parent)
{
    mp_Target = NULL;
}
/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CRoundWidget::~CRoundWidget()
{
    try {
        delete mp_Target;
    }
    catch(...) {
        // to please Lint.
    }
}

/****************************************************************************/
/*!
 *  \brief Load the Speller background.
 */
/****************************************************************************/
void CRoundWidget::Init()
{
    mp_Target = new QPixmap(size());
    m_Pixmap.load(":/Large/SpellerButtons/Speller_BG.png");
    mp_Target->fill(Qt::transparent);
    Application::CLeicaStyle::BorderPixmap(mp_Target, &m_Pixmap, 20, 33, 22, 23);
}

/****************************************************************************/
/*!
 *  \brief Draws the background widget
 */
/****************************************************************************/
void CRoundWidget::paintEvent(QPaintEvent *p_PaintEvent)
{
    p_PaintEvent->accept();
    QPainter Painter(this);
    //Painter.save();
    if (mp_Target) {
        Painter.drawPixmap(0, 0, *mp_Target);
    }
    //Painter.restore();
}

}

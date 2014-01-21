/****************************************************************************/
/*! \file WaitIndicator.cpp
 *
 *  \brief Implementation of file for class CWaitIndicator.
 *
 *  \b Description:
 *          This class implements a base widget for displaying the wait
 *          indicators.
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

#include "MainMenu/Include/WaitIndicator.h"
#include <QDebug>
#include <QPainter>
#include <math.h>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CWaitIndicator::CWaitIndicator(QWidget *p_Parent) : QWidget(p_Parent)
{
    m_Counter = 0;
    setMinimumSize(48, 48);

    if (!connect(&m_Timer, SIGNAL(timeout()), this, SLOT(MoveIndicator()))) {
        qDebug() << "CWaitIndicator: cannot connect 'timeout' signal";
    }

    m_Timer.start(100);
}

/****************************************************************************/
/*!
 *  \brief Painting routine
 */
/****************************************************************************/
void CWaitIndicator::paintEvent(QPaintEvent *)
{
    QPainter Painter(this);
    Painter.setRenderHint(QPainter::Antialiasing);
    Painter.setPen(QPen(Qt::NoPen));
    for (qint32 i = 0; i < 7; i++) {
        if (m_Counter % 7 == i) {
            Painter.setBrush(QBrush(QColor(218, 32, 29)));
        }
        else {
            Painter.setBrush(QBrush(QColor(165, 165, 169)));
        }
        Painter.drawEllipse(width() / 2 + (qint32)(18.0 * cos(2.0 * M_PI * i / 7.0)) - 6,
            height() / 2 + (qint32)(18.0 * sin(2.0 * M_PI * i / 7.0)) - 6, 12, 12);
    }
}

/****************************************************************************/
/*!
 *  \brief Timed method moving the indicator to the next position
 */
/****************************************************************************/
void CWaitIndicator::MoveIndicator()
{
    if (isVisible() == true) {
        m_Counter++;
        update();
    }
}

} // end namespace MainMenu

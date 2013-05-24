/****************************************************************************/
/*! \file ScrollWheel.cpp
 *
 *  \brief ScrollWheel implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-20
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

#include "kineticscroller/include/QtScroller"
#include "kineticscroller/include/QtScrollPrepareEvent"
#include "Application/Include/LeicaStyle.h"
#include "MainMenu/Include/ScrollWheel.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CScrollWheel::CScrollWheel(QWidget *p_Parent) : QWidget(p_Parent, Qt::FramelessWindowHint), m_ThreeDigitMode(false)
{
    QFont Font = font();
    Font.setPointSize(46);
    setFont(Font);
    //chose a multiple of m_ItemHeight, if multiple was not chosen ,the item position was
    //getting an offset of about 10 pixels in the begining.
    m_ScrollableHeight = 98000;


    m_Offset = QPoint(0, 0);
    m_BackgroundPixmap = QPixmap(QString(":/%1/Digits/Digit_Background.png").arg(Application::CLeicaStyle::GetProjectNameString())).copy(2, 43, 86, 188);
    m_SelectedItemPixmap = QPixmap(QString(":/%1/Digits/Digit_Cover.png").arg(Application::CLeicaStyle::GetProjectNameString())).copy(2, 43, 86, 188);

    m_ItemAlignment = Qt::AlignCenter;
    m_ItemHeight = 70;
    m_NonContinuous = false;

    if (QtScroller::grabGesture(this, QtScroller::LeftMouseButtonGesture) < Qt::CustomGesture) {
        qDebug() << "CScrollWheel: invalid gesture type";
    }

    setMinimumSize(86, 188);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
}

/****************************************************************************/
/*!
 *  \brief Returns the current position of the wheel
 *
 *  \return Wheel position
 */
/****************************************************************************/
QPoint CScrollWheel::scrollOffset() const
{
    return m_Offset;
}

/****************************************************************************/
/*!
 *  \brief Sets the current position of the wheel
 *
 *  \iparam Offset = Wheel position
 */
/****************************************************************************/
void CScrollWheel::setScrollOffset(const QPoint &Offset)
{
    if (m_Items.count() > 0)
    {
        m_Offset.setY(Offset.y() % (m_ItemHeight * m_Items.count()));
        if (m_Offset.y() < 0) {
            m_Offset.setY(m_Offset.y() + (m_ItemHeight * m_Items.count()));
        }
    }
    update();
}

/****************************************************************************/
/*!
 *  \brief Drawing routine of the wheel
 *
 *  \iparam p_PaintEvent = Properties of the paint area
 */
/****************************************************************************/
void CScrollWheel::paintEvent(QPaintEvent *p_PaintEvent)
{
    if (!m_Items.count()) {
        return;
    }

    QPainter Painter(this);
    //draw the scaled BG image
    Painter.fillRect(p_PaintEvent->rect(), QColor(39, 47, 58));
    Painter.drawPixmap(0, 0, m_BackgroundPixmap);
    Painter.setPen(Qt::white);

    qint32 Items = height() / m_ItemHeight;
    qint32 Start = m_Offset.y() / m_ItemHeight - Items / 2 - 1;
    qint32 End = Start + height() / m_ItemHeight + Items / 2 + 2;
    qint32 YPos = Start * m_ItemHeight - m_Offset.y() + height() / 2 + 2;

    for (qint32 i = Start; i < End; i++, YPos += m_ItemHeight) {
        qint32 Index = (i % m_Items.count() + m_Items.count()) % m_Items.count();
        if (m_Items[Index] != "")
        {
            Painter.drawText(0, YPos - m_ItemHeight / 2, width(), m_ItemHeight, m_ItemAlignment, m_Items[Index]);

            if(!m_ItemPixmaps[Index].isNull()) {
                Painter.drawPixmap(11, YPos + 24, m_ItemPixmaps[Index]);
            }
        }
        else
            if(!m_ItemPixmaps[Index].isNull()) {
                if (m_ThreeDigitMode)
                    Painter.drawPixmap(30, YPos - m_ItemHeight / 2, m_ItemPixmaps[Index]);
                else
                    Painter.drawPixmap(9, YPos - m_ItemHeight / 2, m_ItemPixmaps[Index]);
            }

    }

    // draw the scaled cover
    Painter.drawPixmap(0, 0, m_SelectedItemPixmap);
}

/****************************************************************************/
/*!
 *  \brief Handles the kinetic scroll events
 *
 *  \iparam p_Event = Scroll events
 *
 *  \return Success of the event handler
 */
/****************************************************************************/
bool CScrollWheel::event(QEvent *p_Event)
{
    switch ((qint32)p_Event->type()) {
        case QtScrollPrepareEvent::ScrollPrepare:
        {
            QtScroller *p_Scroller = QtScroller::scroller(this);
            if (p_Scroller != NULL) {
                p_Scroller->setSnapPositionsY(m_ScrollableHeight % m_ItemHeight, m_ItemHeight);
            }
            QtScrollPrepareEvent *p_ScrollEvent = static_cast<QtScrollPrepareEvent *>(p_Event);
            p_ScrollEvent->setViewportSize(QSizeF(size()));
            p_ScrollEvent->setContentPosRange(QRectF(0.0, 0.0, 0.0, m_ScrollableHeight));
            if (m_NonContinuous) {
                p_ScrollEvent->setContentPos(QPointF(0.0, scrollOffset().y()));
            }
            else {
                p_ScrollEvent->setContentPos(QPointF(0.0, scrollOffset().y() + (m_ScrollableHeight/2)));
            }
            p_ScrollEvent->accept();
            return true;
        }
        case QtScrollEvent::Scroll:
        {
            QtScrollEvent *p_ScrollEvent = static_cast<QtScrollEvent *>(p_Event);
            qint32 Overshoot = (qint32)p_ScrollEvent->overshootDistance().y();
            qint32 YPos = (qint32)(p_ScrollEvent->contentPos().y());
            if (!m_NonContinuous) {
                setScrollOffset(QPoint(0, YPos - (m_ScrollableHeight/2)));
            }
            else {
                //Overshoot can be positive or negative value
                //used to give rubber band effect
                setScrollOffset(QPoint(0, YPos + Overshoot));
            }
            p_ScrollEvent->accept();
            return true;
        }
        default:
            return QWidget::event(p_Event);
    }
}

/****************************************************************************/
/*!
 *  \brief Adds an item to the scroll wheel
 *
 *  \iparam Text = Item text
 *  \iparam Data = Data assigned to the item
 *  \iparam Pixmap = Pixmap drawn below text
 */
/****************************************************************************/
void CScrollWheel::AddItem(const QString &Text, const QVariant &Data, const QPixmap &Pixmap)
{
    m_Items.append(Text);
    m_ItemPixmaps.append(Pixmap);
    m_ItemData.append(Data);
    update();
}

/****************************************************************************/
/*!
 *  \brief Clear the scroll wheel from all content
 */
/****************************************************************************/
void CScrollWheel::ClearItems()
{
    m_Items.clear();
    m_ItemPixmaps.clear();
    m_ItemData.clear();
    update();
}

/****************************************************************************/
/*!
 *  \brief Sets the alignment of the scroll wheel content
 *
 *  \iparam Alignement = Alignment flags
 */
/****************************************************************************/
void CScrollWheel::SetItemAlignment(Qt::Alignment Alignement)
{
    m_ItemAlignment = Alignement;
    update();
}

/****************************************************************************/
/*!
 *  \brief Sets the current item of the wheel
 *
 *  \iparam Index = Item index
 */
/****************************************************************************/
void CScrollWheel::SetCurrentItem(qint32 Index)
{
    m_Offset.setY(Index * m_ItemHeight);
    update();
}

/****************************************************************************/
/*!
 *  \brief Sets the current item of the wheel
 *
 *  \iparam Data = Item data
 */
/****************************************************************************/
void CScrollWheel::SetCurrentData(const QVariant &Data)
{
    if (m_ItemData.contains(Data)) {
        qint32 Index = m_ItemData.indexOf(Data);
        SetCurrentItem(Index);
    }
}

/****************************************************************************/
/*!
 *  \brief Returns the data of the currently selected wheel item
 *
 *  \return Item data
 */
/****************************************************************************/
QVariant CScrollWheel::GetCurrentData() const
{
    qint32 Index = m_Offset.y() / m_ItemHeight;
    return m_ItemData[Index];
}

/****************************************************************************/
/*!
 *  \brief Sets the scroller to non continuous mode
 */
/****************************************************************************/
void CScrollWheel::SetNonContinuous()
{
    m_NonContinuous = true;
    m_ScrollableHeight = (m_Items.count() - 1) * m_ItemHeight;
    //Dummy data to hide roll over values
    QPixmap Pixmap;
    for(qint32 I = 0; I < 4; I++) {
        m_ItemData.append("");
        m_Items.append("");
        m_ItemPixmaps.append(Pixmap);
    }
    update();
}

/****************************************************************************/
/*!
 *  \brief Sets the scroller pixmap based on whether the scroller should display
 *         three digits or two digits.
 */
/****************************************************************************/
void CScrollWheel::SetThreeDigitMode(bool Mode)
{
    m_ThreeDigitMode = Mode;
    if (Mode) {
        m_BackgroundPixmap = QPixmap(QString(":/%1/Digits/Digit_Background_3_digits.png").arg(Application::CLeicaStyle::GetProjectNameString())).copy(2, 43, 126, 188);
        m_SelectedItemPixmap = QPixmap(QString(":/%1/Digits/Digit_Cover_3_digits.png").arg(Application::CLeicaStyle::GetProjectNameString())).copy(2, 43, 126, 188);
    }
    else {
        m_BackgroundPixmap = QPixmap(QString(":/%1/Digits/Digit_Background.png").arg(Application::CLeicaStyle::GetProjectNameString())).copy(2, 43, 86, 188);
        m_SelectedItemPixmap = QPixmap(QString(":/%1/Digits/Digit_Cover.png").arg(Application::CLeicaStyle::GetProjectNameString())).copy(2, 43, 86, 188);
    }
}

} // end namespace MainMenu

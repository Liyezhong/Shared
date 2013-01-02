/****************************************************************************/
/*! \file ContentScroller.cpp
 *
 *  \brief ContentScroller implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-31
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

#include "MainMenu/Include/ContentScroller.h"
#include "ui_ContentScroller.h"
#include <QDebug>
#include <QScrollBar>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CContentScroller::CContentScroller(QWidget *p_Parent) : QWidget(p_Parent), mp_Ui(new Ui::CContentScroller)
{
    mp_Ui->setupUi(this);
    mp_Content = NULL;

    QPalette Palette = mp_Ui->lineUp->palette();
    Palette.setColor(QPalette::WindowText, QColor(177, 175, 181));
    mp_Ui->lineUp->setPalette(Palette);
    mp_Ui->lineDown->setPalette(Palette);

    m_ButtonGroup.addButton(mp_Ui->btnScrollUp, 0);
    m_ButtonGroup.addButton(mp_Ui->btnScrollDown, 1);
    m_ButtonGroup.addButton(mp_Ui->btnScrollBegin, 2);
    m_ButtonGroup.addButton(mp_Ui->btnScrollEnd, 3);

    //Set initial state of buttons
    m_IconScrollBegin.addPixmap(QPixmap(":/Large/Icons/Scroll_Indicator/Scroll_up_end_active.png"));
    m_IconScrollUp.addPixmap(QPixmap(":/Large/Icons/Scroll_Indicator/Scroll_up_active.png"));
    m_IconScrollDown.addPixmap(QPixmap(":/Large/Icons/Scroll_Indicator/Scroll_down_active.png"));
    m_IconScrollEnd.addPixmap(QPixmap(":/Large/Icons/Scroll_Indicator/Scroll_down_end_active.png"));

    m_IconScrollBegin.addPixmap(QPixmap(":/Large/Icons/Scroll_Indicator/Scroll_up_end_passive.png"), QIcon::Disabled);
    m_IconScrollUp.addPixmap(QPixmap(":/Large/Icons/Scroll_Indicator/Scroll_up_passive.png"), QIcon::Disabled);
    m_IconScrollDown.addPixmap(QPixmap(":/Large/Icons/Scroll_Indicator/Scroll_down_passive.png"), QIcon::Disabled);
    m_IconScrollEnd.addPixmap(QPixmap(":/Large/Icons/Scroll_Indicator/Scroll_down_end_passive.png"), QIcon::Disabled);

    mp_Ui->btnScrollBegin->setIcon(m_IconScrollBegin);
    mp_Ui->btnScrollUp->setIcon(m_IconScrollUp);
    mp_Ui->btnScrollDown->setIcon(m_IconScrollDown);
    mp_Ui->btnScrollEnd->setIcon(m_IconScrollEnd);
 }

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CContentScroller::~CContentScroller()
{
    try {
        delete mp_Ui;
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

void CContentScroller::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the content of the scroll frame
 *
 *  \iparam p_Content = Content widget
 */
/****************************************************************************/
void CContentScroller::SetContent(QAbstractScrollArea *p_Content)
{
    mp_Content = p_Content;
    mp_Ui->verticalLayout->insertWidget(1, mp_Content);

    m_Animation.setTargetObject(mp_Content->verticalScrollBar());
    m_Animation.setDuration(300);

    mp_Content->setAutoFillBackground(true);
    mp_Content->setFrameStyle((int)QFrame::NoFrame);
    mp_Content->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    if (!connect(&m_ButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(ScrollContent(int)))) {
        qDebug() << "CContentScroller: cannot connect 'buttonClicked' signal";
    }

    if (!connect(&m_Animation, SIGNAL(valueChanged(const QVariant &)), this, SLOT(ScrollStep(const QVariant &)))) {
        qDebug() << "CContentScroller: cannot connect 'finished' signal";
    }

    if (!connect(&m_Animation, SIGNAL(finished()), this, SLOT(UpdateVerticalScrollBar()))) {
        qDebug() << "CContentScroller: cannot connect 'finished' signal";
    }

    if (!connect(mp_Content->verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(ScrollRangeChanged()))) {
        qDebug() << "CContentScroller: cannot connect 'finished' signal";
    }
}

/****************************************************************************/
/*!
 *  \brief Removes the end to end arrows and the small lines
 */
/****************************************************************************/
void CContentScroller::SimpleArrows()
{
    mp_Ui->btnScrollBegin->hide();
    mp_Ui->btnScrollEnd->hide();
    mp_Ui->lineUp->hide();
    mp_Ui->lineDown->hide();
}

/****************************************************************************/
/*!
 *  \brief Computes the look of the arrows when the widget is activated.
 */
/****************************************************************************/
void CContentScroller::showEvent(QShowEvent *)
{
    ScrollRangeChanged();
}

/****************************************************************************/
/*!
 *  \brief This method is peridically called by the scroll animation.
 *
 *  \iparam Value = Target value for the scroll bar
 */
/****************************************************************************/
void CContentScroller::ScrollStep(const QVariant &Value)
{
    if (m_AnimationStep % 2) {
        mp_Content->verticalScrollBar()->setValue(Value.toInt());
    }
    m_AnimationStep++;
}

/****************************************************************************/
/*!
 *  \brief Handles the scroll procedure initiated by the arrow buttons
 *
 *  \iparam Direction = Scroll direction (up=0, down=1, begin=2, end=3)
 */
/****************************************************************************/
void CContentScroller::ScrollContent(int Direction)
{
    qint32 NewPosition = 0;
    if (mp_Content != NULL) {
        switch (Direction) {
            case 0:
                NewPosition = mp_Content->verticalScrollBar()->value() - mp_Content->viewport()->height();
                if (NewPosition < mp_Content->verticalScrollBar()->minimum()) {
                    NewPosition = mp_Content->verticalScrollBar()->minimum();
                }
                break;
            case 1:
                NewPosition = mp_Content->verticalScrollBar()->value() + mp_Content->viewport()->height();
                if (NewPosition > mp_Content->verticalScrollBar()->maximum()) {
                    NewPosition = mp_Content->verticalScrollBar()->maximum();
                }
                break;
            case 2:
                NewPosition = mp_Content->verticalScrollBar()->minimum();
                break;
            case 3:
                NewPosition = mp_Content->verticalScrollBar()->maximum();
                break;
        }
        UpdateArrows(NewPosition);
        emit Scrolled();
        m_AnimationStep = 0;
        m_Animation.setStartValue(mp_Content->verticalScrollBar()->value());
        m_Animation.setEndValue(NewPosition);
        m_Animation.start();
    }
}

void CContentScroller::UpdateArrows(qint32 NewPosition)
{
    if (mp_Content != NULL) {
        if (NewPosition <= mp_Content->verticalScrollBar()->minimum()) {
            mp_Ui->btnScrollUp->setEnabled(false);
            mp_Ui->btnScrollBegin->setEnabled(false);
        }
        else {
            mp_Ui->btnScrollUp->setEnabled(true);
            mp_Ui->btnScrollBegin->setEnabled(true);
        }

        if (NewPosition >= mp_Content->verticalScrollBar()->maximum()) {
            mp_Ui->btnScrollDown->setEnabled(false);
            mp_Ui->btnScrollEnd->setEnabled(false);
        }
        else {
            mp_Ui->btnScrollDown->setEnabled(true);
            mp_Ui->btnScrollEnd->setEnabled(true);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Updates the vertical Scrollbar
 */
/****************************************************************************/
void CContentScroller::UpdateVerticalScrollBar()
{
    if (mp_Content != NULL) {
        mp_Content->verticalScrollBar()->setValue(m_Animation.endValue().toInt());
    }
}

/****************************************************************************/
/*!
 *  \brief Moves the scroller to first page.
 */
/****************************************************************************/
void CContentScroller::Reset()
{
    mp_Content->verticalScrollBar()->setValue(0);
    UpdateArrows(0);
}

/****************************************************************************/
/*!
 *  \brief This should be called, when the range of the scrollbar changes.
 */
/****************************************************************************/
void CContentScroller::ScrollRangeChanged()
{
    if (mp_Content != NULL) {
        UpdateArrows(mp_Content->verticalScrollBar()->value());
    }
}

} // end namespace MainMenu

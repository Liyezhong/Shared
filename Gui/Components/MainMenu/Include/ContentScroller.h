/****************************************************************************/
/*! \file ContentScroller.h
 *
 *  \brief ContentScroller definition.
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

#ifndef MAINMENU_CONTENTSCROLLER_H
#define MAINMENU_CONTENTSCROLLER_H

#include <QAbstractScrollArea>
#include <QButtonGroup>
#include <QIcon>
#include <QPropertyAnimation>
#include <QWidget>
#include <QScrollBar>

namespace MainMenu {

namespace Ui {
    class CContentScroller;
}

/****************************************************************************/
/**
 * \brief Container widget for scrollable GUI elements.
 */
/****************************************************************************/
class CContentScroller : public QWidget
{
    Q_OBJECT

public:
    explicit CContentScroller(QWidget *p_Parent = 0);
    virtual ~CContentScroller();
    void SetContent(QAbstractScrollArea *p_Content);
    void SimpleArrows();
    void Reset();

private:
    void showEvent(QShowEvent *);
    void UpdateArrows(qint32 NewPosition);

    Ui::CContentScroller *mp_Ui;        //!< User interface
    QAbstractScrollArea *mp_Content;    //!< The content widget of the scroller
    QPropertyAnimation m_Animation;     //!< Animation for smooth scrolling
    QButtonGroup m_ButtonGroup;         //!< Grouping the up and down buttons
    qint32 m_AnimationStep;             //!< Count the steps of the animations
    QIcon m_IconScrollBegin;
    QIcon m_IconScrollUp;
    QIcon m_IconScrollDown;
    QIcon m_IconScrollEnd;

protected:
    void changeEvent(QEvent *p_Event);

private slots:
    void UpdateVerticalScrollBar();
   // void ScrollContent(int Direction);
    void ScrollStep(const QVariant &Value);
    void ScrollRangeChanged();

public slots:
    void ScrollContent(int Direction);

signals:
    void Scrolled();
};

} // end namespace MainMenu

#endif // MAINMENU_CONTENTSCROLLER_H

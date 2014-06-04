/****************************************************************************/
/*! \file ContentScroller.h
 *
 *  \brief Header file for class CContentScroller.
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

//lint -e524

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
    friend class  CTestMainMenu;

public:
    explicit CContentScroller(QWidget *p_Parent = 0);
    virtual ~CContentScroller();
    void SetContent(QAbstractScrollArea *p_Content);
    void SimpleArrows();
    void Reset();
    void SetNintyPercentScroll(bool Status) {
        m_ScrollNintyPercent = Status;
    }

private:
    void showEvent(QShowEvent *);
    void UpdateArrows(qint32 NewPosition);

    Ui::CContentScroller *mp_Ui;        //!< User interface
    QAbstractScrollArea *mp_Content;    //!< The content widget of the scroller
    QPropertyAnimation m_Animation;     //!< Animation for smooth scrolling
    QButtonGroup m_ButtonGroup;         //!< Grouping the up and down buttons
    qint32 m_AnimationStep;             //!< Count the steps of the animations
    QIcon m_IconScrollBegin;            //!< Icon for scroll begin
    QIcon m_IconScrollUp;               //!< Icon for scroll up
    QIcon m_IconScrollDown;             //!< Icon for scroll down
    QIcon m_IconScrollEnd;              //!< Icon for scroll end
    bool m_ScrollNintyPercent;          //!< flag to check wether 90% is covered
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CContentScroller)

protected:
    void changeEvent(QEvent *p_Event);

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void UpdateVerticalScrollBar();
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ScrollStep(const QVariant &Value);
    

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ScrollContent(int Direction);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ScrollRangeChanged();

signals:

    /****************************************************************************/
    /*!
     *  \brief This signal is emitted when the table is scrolled.
     *
     */
    /****************************************************************************/
    void Scrolled();
};

} // end namespace MainMenu

#endif // MAINMENU_CONTENTSCROLLER_H

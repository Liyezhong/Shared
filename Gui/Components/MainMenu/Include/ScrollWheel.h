/****************************************************************************/
/*! \file ScrollWheel.h
 *
 *  \brief Header file for class CScrollWheel.
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

#ifndef MAINMENU_SCROLLWHEEL_H
#define MAINMENU_SCROLLWHEEL_H

#include <QWidget>
#include <QVariant>

//lint -e653
namespace MainMenu {

/****************************************************************************/
/**
 * \brief Scrollable wheel used for the input of numerical data.
 */
/****************************************************************************/
class CScrollWheel : public QWidget
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    CScrollWheel(QWidget *p_Parent = NULL);

    void AddItem(const QString &Text, const QVariant &Data = QVariant(), const QPixmap &Pixmap = QPixmap());
    void ClearItems();
    void SetItemAlignment(Qt::Alignment Alignment);
    void SetCurrentItem(qint32 Index);
    void SetCurrentData(const QVariant &Data);
    QVariant GetCurrentData() const;
    void SetNonContinuous();
    void SetThreeDigitMode(bool Mode);
    void InitScrollWheel(QString Type);

protected:
    virtual QPoint scrollOffset() const;
    virtual void setScrollOffset(const QPoint &Offset);

    void paintEvent(QPaintEvent *p_PaintEvent);
    bool event(QEvent *p_Event);

private:
    QPoint m_Offset;                //!< The current offste inside the scroll view
    qint32 m_ItemHeight;            //!< The height of a single item
    QStringList m_Items;            //!< The text of the items
    QList<QVariant> m_ItemData;     //!< The content assigned to the items
    QList<QPixmap> m_ItemPixmaps;   //!< Small pixmaps in the lower left item corner
    QPixmap m_BackgroundPixmap;     //!< Background image
    QPixmap m_SelectedItemPixmap;   //!< Image of the slected item
    Qt::Alignment m_ItemAlignment;  //!< Alignment direction of the wheels
    qint32 m_ScrollableHeight;      //!< Maximum scrollable height for non continuous scroller.
    bool m_NonContinuous;           //!< If true scroller is non continuous
    QString m_ScrollType;           //!< Scroll wheel type for Sepia
	bool m_ThreeDigitMode;
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CScrollWheel)
};

} // end namespace MainMenu

#endif // MAINMENU_SCROLLWHEEL_H

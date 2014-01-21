/****************************************************************************/
/*! \file RoundBGWidget.h
 *
 *  \brief Background widget for keyboard Keys
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-11-02
 *   $Author:  $ N.Kamath
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef ROUNDBGWIDGET_H
#define ROUNDBGWIDGET_H

#include <QWidget>
#include <QPainter>

namespace KeyBoard {
/****************************************************************************/
/**
 * \brief Tray which holds all the keys on the keyboard
 */
/****************************************************************************/
class CRoundWidget : public QWidget
{
Q_OBJECT
private:
    QPixmap m_Pixmap;   //!< Speller background pixmap
    QPixmap *mp_Target; //!< Speller background pixmap stretched
    void paintEvent(QPaintEvent *p_PaintEvent);
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CRoundWidget)

public:
    explicit CRoundWidget(QWidget *p_Parent = 0);
    ~CRoundWidget();
    void Init();
};

}//End of namespace KeyBoard
#endif // CROUNDWIDGET_H

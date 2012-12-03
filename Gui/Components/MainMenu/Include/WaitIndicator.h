/****************************************************************************/
/*! \file WaitIndicator.h
 *
 *  \brief WaitIndicator definition.
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

#ifndef MAINMENU_WAITINDICATOR_H
#define MAINMENU_WAITINDICATOR_H

#include <QTimer>
#include <QWidget>

namespace MainMenu {

/****************************************************************************/
/**
 * \brief This class holds a small animation indicating that the application
 *        is busy.
 */
/****************************************************************************/
class CWaitIndicator : public QWidget
{
    Q_OBJECT

public:
    explicit CWaitIndicator(QWidget *p_Parent = 0);

private:
    void paintEvent(QPaintEvent *);
    QTimer m_Timer;     //!< Timer responsible for the repaint
    qint32 m_Counter;   //!< Current position of he wheel

private slots:
    void MoveIndicator();
};

} // end namespace MainMenu

#endif // MAINMENU_WAITINDICATOR_H

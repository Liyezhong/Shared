/****************************************************************************/
/*! \file ScrollTable.h
 *
 *  \brief Header file for class CScrollTable.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-29
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

#ifndef MAINMENU_SCROLLTABLE_H
#define MAINMENU_SCROLLTABLE_H

#include <QPixmap>
#include <QCache>
#include "MainMenu/Include/ContentScroller.h"

//lint -e1540

namespace MainMenu {

/****************************************************************************/
/**
 * \brief This is widget is a container for scrollable tables.
 */
/****************************************************************************/
class CScrollTable : public CContentScroller
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    explicit CScrollTable(QWidget *p_Parent = 0);
    virtual ~CScrollTable();

private:
    void paintEvent(QPaintEvent *);
    QCache <qint64, QPixmap> m_PixmapCache;     //!< Pixmap cache object
    QPixmap m_PixmapTableGrid;                  //!< Pixmap object for Table grid
    QPixmap *mp_PixmapTarget;                   //!< Pixmap object for Table
    qint64 m_CacheKey;                          //!< Cachec key value
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CScrollTable)

};

} // end namespace MainMenu

#endif // MAINMENU_SCROLLTABLE_H

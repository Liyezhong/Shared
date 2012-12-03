/****************************************************************************/
/*! \file ScrollTable.h
 *
 *  \brief ScrollTable definition.
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

namespace MainMenu {

/****************************************************************************/
/**
 * \brief This is widget is a container for scrollable tables.
 */
/****************************************************************************/
class CScrollTable : public CContentScroller
{
    Q_OBJECT

public:
    explicit CScrollTable(QWidget *p_Parent = 0);

private:
    void paintEvent(QPaintEvent *);
    QPixmap m_PixmapTableGrid;
    QPixmap m_PixmapTarget;
    QCache <qint64, QPixmap> m_PixmapCache;
    qint64 m_CacheKey;

};

} // end namespace MainMenu

#endif // MAINMENU_SCROLLTABLE_H

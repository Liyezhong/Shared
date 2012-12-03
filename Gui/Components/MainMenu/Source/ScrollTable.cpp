/****************************************************************************/
/*! \file ScrollTable.cpp
 *
 *  \brief ScrollTable implementation.
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

#include "MainMenu/Include/ScrollTable.h"
#include "Application/Include/LeicaStyle.h"
#include <QLayout>
#include <QPainter>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CScrollTable::CScrollTable(QWidget *p_Parent) : CContentScroller(p_Parent), m_PixmapCache(1000),m_CacheKey(0)
{
    layout()->setMargin(11);
    layout()->setSpacing(1);
    m_PixmapTableGrid.load(":/Large/Grid/Grid-BG.png");  
}

/****************************************************************************/
/*!
 *  \brief Draws the background image of the table
 */
/****************************************************************************/
void CScrollTable::paintEvent(QPaintEvent *)
{
    QPainter Painter(this);
    QPixmap Target(size());
    if (!m_PixmapCache.contains(m_CacheKey)) {
        Target.fill(Qt::transparent);
        Application::CLeicaStyle::BorderPixmap(&Target, &m_PixmapTableGrid, 24, 71, 24, 43);
        m_PixmapTarget = Target;
        m_CacheKey = m_PixmapTarget.cacheKey();
        m_PixmapCache.insert(m_CacheKey, &m_PixmapTarget);
        Painter.drawPixmap(0, 0, m_PixmapTarget);
    }
    else {
         Painter.drawPixmap(0, 0, *m_PixmapCache.object(m_CacheKey));
    }

}

} // end namespace MainMenu

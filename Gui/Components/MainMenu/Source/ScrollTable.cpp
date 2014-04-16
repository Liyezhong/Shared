/****************************************************************************/
/*! \file ScrollTable.cpp
 *
 *  \brief Implementation of file for class CScrollTable.
 *
 *  \b Description:
 *          This class implements a base widget for displaying Table view
 *          scroller.
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
CScrollTable::CScrollTable(QWidget *p_Parent) : CContentScroller(p_Parent),
    m_PixmapCache(1000),
    mp_PixmapTarget(NULL),
    m_CacheKey(0)
{
    layout()->setMargin(11);
    layout()->setSpacing(1);
    (void)     m_PixmapTableGrid.load(QString(":/%1/Grid/Grid-BG.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
    mp_PixmapTarget = new QPixmap();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 *
 */
/****************************************************************************/
CScrollTable::~CScrollTable() {
    try {
        //delete mp_PixmapTarget; QCache deletes this. ignore valgrind output
    }
    catch (...) {}
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
        *mp_PixmapTarget = Target;
        if (mp_PixmapTarget) {
            m_CacheKey = mp_PixmapTarget->cacheKey();
            (void) m_PixmapCache.insert(m_CacheKey, mp_PixmapTarget);
            Painter.drawPixmap(0, 0, *mp_PixmapTarget);
        }
    }
    else {
         Painter.drawPixmap(0, 0, *m_PixmapCache.object(m_CacheKey));
    }

}

} // end namespace MainMenu

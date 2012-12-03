/****************************************************************************/
/*! \file BaseTable.cpp
 *
 *  \brief BaseTable implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-07-08
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

#include "MainMenu/Include/BaseTable.h"
#include <QDebug>
#include <QDropEvent>
#include <QHeaderView>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CBaseTable::CBaseTable(QWidget *p_Parent) : QTableView(p_Parent)
{
    QPalette Palette = palette();
    Palette.setColor(QPalette::ButtonText, Qt::black);
    Palette.setColor(QPalette::HighlightedText, Qt::white);
    Palette.setColor(QPalette::Window, Qt::white);
    setPalette(Palette);

    setFocusPolicy(Qt::NoFocus);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    horizontalHeader()->setClickable(false);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->hide();
    verticalHeader()->setDefaultSectionSize(40);
    verticalHeader()->setMinimumSectionSize(40);
    verticalHeader()->hide();
}

/****************************************************************************/
/*!
 *  \brief Sets the height of the table in rows
 *
 *  \iparam Rows = Number of rows
 */
/****************************************************************************/
void CBaseTable::SetVisibleRows(qint32 Rows)
{
    setFixedHeight(28 + 40 * Rows);
}

} // end namespace MainMenu

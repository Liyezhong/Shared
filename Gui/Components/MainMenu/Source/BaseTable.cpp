/****************************************************************************/
/*! \file BaseTable.cpp
 *
 *  \brief Implementation of file for class CBaseTable.
 *
 *  \b Description:
 *          This class implements a base widget to display Table view.
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

    horizontalHeader()->setSectionsClickable (false);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
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
 *  \iparam Rows = Number of rows.
 *  \iparam HeaderHeight = Default value is 28 require to decide height of the Table.
 *  \iparam RowHeight = Default value is 40. Multiplied With Number of
 *   Rows decide height of the Table.
 *
 */
/****************************************************************************/
void CBaseTable::SetVisibleRows(qint32 Rows , qint32 HeaderHeight ,qint32 RowHeight)
{
    setFixedHeight(HeaderHeight + RowHeight * Rows);
}

/****************************************************************************/
/*!
 *  \brief Ignores mouse movement.
 *
 *  \iparam p_Event = event object of the mouse event class.
 */
/****************************************************************************/
void CBaseTable::mouseMoveEvent(QMouseEvent *p_Event)
{
    p_Event->ignore();
}

} // end namespace MainMenu

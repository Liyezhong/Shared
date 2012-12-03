/****************************************************************************/
/*! \file BaseTable.h
 *
 *  \brief BaseTable definition.
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

#ifndef MAINMENU_BASETABLE_H
#define MAINMENU_BASETABLE_H

#include <QTableView>

namespace MainMenu {

/****************************************************************************/
/**
 * \brief This is the base class for all table widget displayed in the GUI.
 */
/****************************************************************************/
class CBaseTable : public QTableView
{
    Q_OBJECT

public:
    explicit CBaseTable(QWidget *p_Parent = 0);
    void SetVisibleRows(qint32 Rows);

};

} // end namespace MainMenu

#endif // MAINMENU_BASETABLE_H

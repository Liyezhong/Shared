/****************************************************************************/
/*! \file EventMsgModel.h
 *
 *  \brief EventMsgModel definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-08-25
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

#ifndef MAINMENU_EVENTMSGMODEL_H
#define MAINMENU_EVENTMSGMODEL_H

#include "MainMenu/Include/TableMoving.h"
#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>

namespace MainMenu {

/****************************************************************************/
/**
 * \brief This is the data model for the program tables
 */
/****************************************************************************/
class CEventMsgModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CEventMsgModel(QObject *p_Parent = NULL);
    void SetEventMsgData(QStringList,QStringList,QStringList);
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &Index, int Role) const;
    Qt::ItemFlags flags(const QModelIndex &Index) const;
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role) const;
    void SetVisibleRowCount(int RowCount);

private:
    qint32 m_Columns;   //!< Number of table columns
    qint32 m_VisibleRowCount; //!< Number of rows visible in the table
    QStringList m_EventIdList; //!< Event Id's List
    QStringList m_EventMsgList; //!< Event Messages List
    QStringList m_TimeStampList; //!< Time Stamp List
private slots:
    void UpdateEventMsgList();

};

} // end namespace MainMenu

#endif //MAINMENU_EVENTMSGMODEL_H

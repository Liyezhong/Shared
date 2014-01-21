/****************************************************************************/
/*! \file EventMsgModel.h
 *
 *  \brief Header file for class CEventMsgModel.
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
    friend class  CTestMainMenu;

public:
    /****************************************************************************/
    /*!
     *  \brief Constructor
     *
     *  \iparam p_Parent = Parent object
     */
    /****************************************************************************/
    CEventMsgModel(QObject *p_Parent = NULL);

    /****************************************************************************/
    /*!
     *  \brief Sets event message data
     *
     */
    /****************************************************************************/
    void SetEventMsgData(QStringList,QStringList,QStringList);

    /****************************************************************************/
    /*!
     *  \brief Returns the number of rows in the table
     *
     *  \return Row count
     */
    /****************************************************************************/
    int rowCount(const QModelIndex &) const;

    /****************************************************************************/
    /*!
     *  \brief Returns the number of columns in the table
     *
     *  \return Columns count
     */
    /****************************************************************************/
    int columnCount(const QModelIndex &) const;

    /****************************************************************************/
    /*!
     *  \brief Returns the data items displayed in the table
     *
     *  \iparam Index = Index of a table cell
     *  \iparam Role = Display role to be returned
     *
     *  \return Data item
     */
    /****************************************************************************/
    QVariant data(const QModelIndex &Index, int Role) const;

    /****************************************************************************/
    /*!
     *  \brief Returns item model flags of a cell
     *
     *  \iparam Index = Index of a table cell
     *
     *  \return Flags of the cell
     */
    /****************************************************************************/
    Qt::ItemFlags flags(const QModelIndex &Index) const;

    /****************************************************************************/
    /*!
     *  \brief Returns the caption of the column headers
     *
     *  \iparam Section = Row or column number
     *  \iparam Orientation = Horizontal or vertical header
     *  \iparam Role = Display role to be returned
     *
     *  \return Header data
     */
    /****************************************************************************/
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role) const;

    /****************************************************************************/
    /*!
     *  \brief Sets the number of rows visible in the table
     *
     *  \iparam RowCount
     */
    /****************************************************************************/
    void SetVisibleRowCount(int RowCount);

private:
    qint32 m_Columns;       //!< Number of table columns
    qint32 m_VisibleRowCount; //!< Number of rows visible in the table
    QStringList m_EventIdList; //!< Event Id's List
    QStringList m_EventMsgList; //!< Event Messages List
    QStringList m_TimeStampList; //!< Time Stamp List
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CEventMsgModel)

private slots:
    /****************************************************************************/
    /*!
     *  \brief Updates event message list.
     *
     */
    /****************************************************************************/
    void UpdateEventMsgList();

};

} // end namespace MainMenu

#endif //MAINMENU_EVENTMSGMODEL_H

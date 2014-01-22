/****************************************************************************/
/*! \file ProgramSequenceModel.h
 *
 *  \brief Include file for class CFileSelectionModel.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-06-17
 *   $Author:  $ Raju
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
#ifndef MAINMENU_FILESELECTIONMODEL_H
#define MAINMENU_FILESELECTIONMODEL_H

#include "MainMenu/Include/MessageDlg.h"
#include <QAbstractTableModel>

namespace MainMenu {


/****************************************************************************/
/**
 * \brief This is the data model to display file names in a table with selection
 */
/****************************************************************************/
class CFileSelectionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    CFileSelectionModel(QObject *p_Parent = NULL);
    ~CFileSelectionModel();
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &Index, int Role) const;
    Qt::ItemFlags flags(const QModelIndex &Index) const;
    QVariant headerData(int Section, Qt::Orientation Orientation, int Role) const;
    bool setData(const QModelIndex &Index, const QVariant &Value, int Role);
    void SetVisibleRowCount(int RowCount);
    void SetNumberOfColumns(qint32 Columns);
    void SetFileList(QStringList FileList);
    void SetMaxItemSelection(qint32 MaxSelValue);
    void SetFileNameSeperator(QString FileNameSeperator);
    void SetPartOfFileName(qint32 PartOfFileName);
    void SetSelectableFileNames(QStringList SelectableFileNameList);
    void SetSelectOneFileName(QStringList SelectOneFileNameList);
    QStringList GetSelectedFiles();

private:
    qint32 m_Columns;                                 //!< Number of table columns
    qint32 m_VisibleRowCount;                         //!< Number of rows visible in the table
    bool m_MaximumSelection;                          //!< Maximum selection of the files
    qint32 m_MaxItemsSelectable;                      //!< Maximum items can be selectable
    QString m_Seperator;                              //!< Separtor
    qint32 m_PartOfFileName;                          //!< Part of the file name
    QStringList m_SelectableList;                     //!< List of file names
    QStringList m_SelectOneFileNameFromList;          //!< Optional file name list
    QStringList m_FileList;                           //!< Store file names
    QList<bool> m_CheckedValueList;                   //!< Store the check box values
    QStringList m_TypeOfSelectedList;                 //!< Store the type of selected file list

    void UpdateDataList(const QModelIndex &Index);
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CFileSelectionModel)


};

} // end namespace FileSelectionModel

#endif // MAINMENU_FILESELECTIONMODEL_H

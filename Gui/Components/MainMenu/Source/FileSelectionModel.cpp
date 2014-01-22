/****************************************************************************/
/*! \file FileSelectionModel.cpp
 *
 *  \brief Implementation file for class CFileSelectionModel.
 *
 *  \b Description:
 *          This class implements the FileSelectionModel required for FileSelection
 *          Widget to display list of files to import the data.
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "MainMenu/Include/FileSelectionModel.h"
#include "MainMenu/Include/FileSelection.h"
#include <QDebug>
#include <QPainter>
#include <QPixmap>

namespace MainMenu {

const int VISIBLE_ROW_COUNT = 7;                //!< Number of rows visible in a table

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CFileSelectionModel::CFileSelectionModel(QObject *p_Parent) :
    QAbstractTableModel(p_Parent),
    m_Columns(0),
    m_VisibleRowCount(VISIBLE_ROW_COUNT),
    m_MaximumSelection(false),
    m_MaxItemsSelectable(3),
    m_Seperator("_"),
    m_PartOfFileName(1)
{
    m_FileList.clear();
    m_SelectableList.clear();
    m_SelectOneFileNameFromList.clear();
    m_TypeOfSelectedList.clear();
    m_CheckedValueList.clear();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CFileSelectionModel::~CFileSelectionModel()
{

}

/****************************************************************************/
/*!
 *  \brief Sets the number of columns in the table.
 *
 *  \iparam Columns = Table columns
 */
/****************************************************************************/
void CFileSelectionModel::SetNumberOfColumns(qint32 Columns)
{
     m_Columns = Columns;
}

/****************************************************************************/
/*!
 *  \brief Sets the number of rows visible in the table
 *
 *  \iparam RowCount
 */
/****************************************************************************/
void CFileSelectionModel::SetVisibleRowCount(int RowCount) {
    beginResetModel();
    m_VisibleRowCount = RowCount;
    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief Returns the number of rows in the table
 *
 *  \return Row count
 */
/****************************************************************************/
int CFileSelectionModel::rowCount(const QModelIndex &) const
{
    int Count = m_FileList.count();
    return ((Count < m_VisibleRowCount) ? m_VisibleRowCount : Count);
}

/****************************************************************************/
/*!
 *  \brief Returns the number of columns in the table
 *
 *  \return Columns count
 */
/****************************************************************************/
int CFileSelectionModel::columnCount(const QModelIndex &) const
{
    return m_Columns;
}

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
QVariant CFileSelectionModel::data(const QModelIndex &Index, int Role) const
{
    if (m_FileList.count() > Index.row()) {
        if (Role == (int)Qt::DisplayRole) {
            switch (Index.column()) {
            case 1:
                qDebug() << Index.row();
                return m_FileList.value(Index.row());
            }
        }
        else if (Role == (int)Qt::CheckStateRole) {
            if (Index.column() == 0) {
                if (m_CheckedValueList.value(Index.row())) {
                    return (int)Qt::Checked;
                }
                else {
                    return (int)Qt::Unchecked;
                }
            }
        }
    }
    else {
        if (Role == (int)Qt::BackgroundRole) {
            QPalette Palette;
            return QVariant(Palette.color(QPalette::Window));
        }
    }
    return QVariant();
}

/****************************************************************************/
/*!
 *  \brief Returns item model flags of a cell
 *
 *  \iparam Index = Index of a table cell
 *
 *  \return Flags of the cell
 */
/****************************************************************************/
Qt::ItemFlags CFileSelectionModel::flags(const QModelIndex &Index) const
{
    if (m_FileList.value(Index.row()).split(m_Seperator).count() > 1) {
        if (m_TypeOfSelectedList.contains(m_FileList.value(Index.row()).split(m_Seperator).value(m_PartOfFileName))
                && !m_CheckedValueList.value(Index.row())) {
            return Qt::NoItemFlags;
        }
    }
    if (m_CheckedValueList.value(Index.row()) || !m_MaximumSelection) {
        if (Index.column() == 0) {
            return QAbstractItemModel::flags(Index) | Qt::ItemIsUserCheckable;
        }
        else {
            return QAbstractItemModel::flags(Index);
        }
    }
    return Qt::NoItemFlags;
}

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
QVariant CFileSelectionModel::headerData(int Section, Qt::Orientation Orientation, int Role) const
{
    if (Role == (int)Qt::DisplayRole && Orientation == Qt::Horizontal) {
        switch (Section) {
        case 0:
            return tr("Use");
        case 1:
            return tr("File Name");
        }
    }
    return QVariant();
}

/****************************************************************************/
/*!
 *  \brief Sets the Filelist
 *
 *  \iparam  FileList - List of file names
 */
/****************************************************************************/
void CFileSelectionModel::SetFileList(QStringList FileList)
{
    m_FileList = FileList;
    for (qint32 Counter = 0; Counter < m_FileList.count(); Counter++) {
        m_CheckedValueList.append(false);
    }
    beginResetModel();
    endResetModel();
}

/****************************************************************************/
/*!
 *  \brief Sets the maximum items selection from the table
 *
 *  \iparam  MaxSelValue - Maximum selection value
 */
/****************************************************************************/
void CFileSelectionModel::SetMaxItemSelection(qint32 MaxSelValue)
{
    m_MaxItemsSelectable = MaxSelValue;
}

/****************************************************************************/
/*!
 *  \brief Sets the sepeartor which is used to split file name
 *         For example if the file name is "DeviceName_Type_SerialNumber_TimeStamp
 *         then "_" is called as sepeartor
 *
 *  \iparam  FileNameSeperator - Seperator which is used for file name
 */
/****************************************************************************/
void CFileSelectionModel::SetFileNameSeperator(QString FileNameSeperator)
{
    m_Seperator = FileNameSeperator;
}

/****************************************************************************/
/*!
 *  \brief Sets the part of the file name
 *         For example if the file name is "DeviceName_Type_SerialNumber_TimeStamp
 *         then "Type" is called as second part of the file name (Here the numbering
 *         started from one. In C++ array starts from zero)
 *
 *  \iparam  PartOfFileName - Part of the file name
 */
/****************************************************************************/
void CFileSelectionModel::SetPartOfFileName(qint32 PartOfFileName)
{
    m_PartOfFileName = PartOfFileName;
}

/****************************************************************************/
/*!
 *  \brief Sets selectable file names in a list
 *
 *  \iparam  SelectableFileNameList - List of file names
 */
/****************************************************************************/
void CFileSelectionModel::SetSelectableFileNames(QStringList SelectableFileNameList)
{
    m_SelectableList = SelectableFileNameList;
}

/****************************************************************************/
/*!
 *  \brief Sets selectable file names from list. But either one of the file
 *         can be selected from the list
 *
 *  \iparam  SelectOneFileNameList - List of file names
 */
/****************************************************************************/
void CFileSelectionModel::SetSelectOneFileName(QStringList SelectOneFileNameList)
{
    m_SelectOneFileNameFromList = SelectOneFileNameList;
}

/****************************************************************************/
/*!
*  \brief Sets the content of table items
*
*  \iparam Index = Index of a table cell
*  \iparam Value = The new value
*  \iparam Role = Display role to be modified
*
*  \return Changed something (true) or not (false)
*/
/****************************************************************************/
bool CFileSelectionModel::setData(const QModelIndex &Index, const QVariant &Value, int Role)
{
    if (m_FileList.count() > Index.row()) {
        if (Role == (int)Qt::CheckStateRole) {
            if (Index.column() == 0) {
                m_CheckedValueList[Index.row()] = Value.toBool();
                m_TypeOfSelectedList.clear();
                UpdateDataList(Index);
                beginResetModel();
                endResetModel();
                return true;
            }
        }
    }
    return false;
}

/****************************************************************************/
/*!
*  \brief Updates the data in the list to disable rest of the rows if it reaches
*         maximum selection
*
*  \iparam Index = Index of a table cell
*
*/
/****************************************************************************/
void CFileSelectionModel::UpdateDataList(const QModelIndex &Index)
{
    qint32 SelectionCounter = 0;
    for (qint32 Counter = 0; Counter < m_CheckedValueList.count(); Counter++) {
        if (m_CheckedValueList.value(Counter)) {
            // check the type of the file by splitting the file name and add it into the
            // selected list
            if (m_FileList.value(Index.row()).split(m_Seperator).count() > m_PartOfFileName) {
                QString TypeOfFileName(m_FileList.value(Counter).split(m_Seperator).value(m_PartOfFileName));
                if (m_SelectableList.contains(TypeOfFileName)) {
                    if (m_SelectOneFileNameFromList.contains(TypeOfFileName)) {
                        m_TypeOfSelectedList.append(m_SelectOneFileNameFromList);
                    }
                    else {
                        m_TypeOfSelectedList.append(TypeOfFileName);
                    }
                }
            }

            SelectionCounter++;
            // break the loop if it reaches maximum item to select
            if(SelectionCounter == m_MaxItemsSelectable) {
                m_MaximumSelection = true;
                break;
            }
            else {
                m_MaximumSelection = false;
            }
        }
    }
}

/****************************************************************************/
/*!
*  \brief Get the list of files which are selected by user
*
*  \return list of files
*
*/
/****************************************************************************/
QStringList CFileSelectionModel::GetSelectedFiles()
{
    QStringList FileList;
    for (qint32 Counter = 0; Counter < m_CheckedValueList.count(); Counter++) {
        if (m_CheckedValueList.value(Counter)) {
            FileList.append(m_FileList.value(Counter));
        }
    }
    return FileList;
}

} // end namespace FileSelectionModel

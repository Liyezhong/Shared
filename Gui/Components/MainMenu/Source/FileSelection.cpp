/****************************************************************************/
/*! \file FileSelection.cpp
 *
 *  \brief CFileSelection implementation.
 *
 *  \b Description:
 *          This class
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
#include "MainMenu/Include/FileSelection.h"
#include "ui_FileSelection.h"
#include <Global/Include/SystemPaths.h>

namespace MainMenu {

const int VISIBLE_ROW_COUNT = 8;                    //!< Number of rows visible in a table

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_DataConnector = Global data container
 *  \iparam p_MainWindow = MainWindow pointer
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CFileSelection::CFileSelection(QWidget *p_Parent) :
    MainMenu::CDialogFrame(p_Parent),
    mp_Ui(new Ui::CFileSelection)

{
    SetDialogTitle(QApplication::translate("MainMenu::CFileSelection", "Import Files",
                                           0, QApplication::UnicodeUTF8));
    mp_Ui->setupUi(GetContentFrame());

    mp_TableWidget = new MainMenu::CBaseTable;
    // fix the height of the Table widget so that it can display upto 8 r0ws
    mp_TableWidget->SetVisibleRows(8);
    m_Model.SetNumberOfColumns(2);
    m_Model.SetVisibleRowCount(8);

    mp_TableWidget->setModel(&m_Model);
    mp_TableWidget->horizontalHeader()->show();
    mp_TableWidget->selectRow(0);
    mp_Ui->scrollTable->SetContent(mp_TableWidget);
    //mp_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mp_TableWidget->horizontalHeader()->resizeSection(0, 90);

    mp_Ui->btnOk->setEnabled(false);

    CONNECTSIGNALSLOTGUI(mp_Ui->btnCancel, clicked(), this, CancelButtonClicked());
    CONNECTSIGNALSLOTGUI(mp_TableWidget, clicked(QModelIndex), this, ItemActivated(QModelIndex));
    CONNECTSIGNALSLOTGUI(mp_Ui->btnOk, clicked(), this, OKButtonClicked());
    CONNECTSIGNALSLOTGUI(this, DialogLangaugeChanged(), this, RetranslateUI());

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CFileSelection::~CFileSelection()
{
    try {
        delete mp_TableWidget;
        delete mp_Ui;
    }
    catch (...) {
        // to please Lint.
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CFileSelection::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
    case QEvent::LanguageChange:
        mp_Ui->retranslateUi(this);
        RetranslateUI();
        break;
    default:
        break;
    }
}


/****************************************************************************/
/*!
 *  \brief Slot for the item activation
 *
 *  \iparam Index = Index of the row
 */
/****************************************************************************/
void CFileSelection::ItemActivated(QModelIndex Index)
{
    //m_Model.SetIndex(Index.row());
    mp_TableWidget->selectRow(Index.row());
    if (m_Model.GetSelectedFiles().count() > 0) {
        mp_Ui->btnOk->setEnabled(true);
    }
    else {
        mp_Ui->btnOk->setEnabled(false);
    }
}

/****************************************************************************/
/**
 * \brief Set the panel size of the file selection window
 *
 * \param[in] Width = width of the panel
 * \param[in] Height = Height of the panel
 */
/****************************************************************************/
void CFileSelection::SetFileSelectionPanelSize(const qint32 &Width, const qint32 &Height)
{
    this->move(120, 35);
    // if the Main screen resolution is 800X600 then the panel size will be 550X400
    this->resize(Width - 250, Height - 200);
}


/****************************************************************************/
/*!
 *  \brief Sets the row focus
 *
 *  \iparam CurrentRow = Current selected row.
 */
/****************************************************************************/
void CFileSelection::SetRowFocus(qint32 CurrentRow)
{
    mp_TableWidget->selectRow(CurrentRow);    
}

/****************************************************************************/
/*!
 *  \brief Sets the Filelist
 *
 *  \iparam  FileList List of file names
 */
/****************************************************************************/
void CFileSelection::SetFileList(QStringList FileList)
{
    m_Model.SetFileList(FileList);
}


/****************************************************************************/
/*!
 *  \brief Sets the required data in model
 *
 *  \iparam  MaxItemsSelectable - List of file names
 *  \iparam  Sepeartor - Sepeartor of the file name
 *  \iparam  PartOfFileName - Part of the file name
 *  \iparam  SelectableFileList - Selectable part of the file names from the list
 *  \iparam  OptionalSelList - Optional selectable part of the file names from the list
 */
/****************************************************************************/
void CFileSelection::SetData(qint32 MaxItemsSelectable, QString Sepeartor, qint32 PartOfFileName,
                             QStringList SelectableFileList, QStringList OptionalSelList)
{
    m_Model.SetMaxItemSelection(MaxItemsSelectable);
    m_Model.SetFileNameSeperator(Sepeartor);
    m_Model.SetPartOfFileName(PartOfFileName);
    m_Model.SetSelectableFileNames(SelectableFileList);
    m_Model.SetSelectOneFileName(OptionalSelList);
}

/****************************************************************************/
/*!
 *  \brief Slot executed when the OK button is clicked
 *
 */
/****************************************************************************/
void CFileSelection::OKButtonClicked()
{
    QStringList ListOfFiles = m_Model.GetSelectedFiles();

    if(ListOfFiles.count() > 0) {
        accept();
        emit SelectedFileList(ListOfFiles);        
    }
}


/****************************************************************************/
/*!
 *  \brief Slot executed when the Cancel button is clicked
 *
 */
/****************************************************************************/
void CFileSelection::CancelButtonClicked()
{
    reject();
    emit SelectedFileList(QStringList());
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CFileSelection::RetranslateUI()
{
    MainMenu::CDialogFrame::SetDialogTitle(QApplication::translate("MainMenu::CFileSelection", "Import Files",
                                                                   0, QApplication::UnicodeUTF8));
}


/****************************************************************************/
} // end namespace MainMenu

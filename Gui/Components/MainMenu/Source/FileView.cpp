/****************************************************************************/
/*! \file FileView.cpp
 *
 *  \brief Implementation of file for class CFileView.
 *
 *  \b Description:
 *          This class implements a base widget which displays the Day run log
 *      file content.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-29, 2012-11-07
 *   $Author:  $ M.Scherer, Raju
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/FileView.h"
#include "ui_FileView.h"
#include <QDebug>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CFileView::CFileView(QWidget *p_Parent) : QWidget(p_Parent), mp_Ui(new Ui::CFileView),
    m_SelectableRows(0),
    m_PreviousIndex(0)
{
    mp_Ui->setupUi(this);

    mp_TableWidget = new MainMenu::CBaseTable;
    // fix the height of the Table widget so that it can display upto 8 r0ws
    mp_TableWidget->SetVisibleRows(8);

    mp_TableWidget->setModel(&m_Model);
    // Set horizontal header item
    m_Model.setHorizontalHeaderItem(0, new QStandardItem("   " + tr("File Name")));
    mp_TableWidget->horizontalHeader()->show();
    mp_TableWidget->selectRow(0);
    mp_Ui->widget->SetContent(mp_TableWidget);
    mp_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mp_TableWidget->horizontalHeader()->resizeSection(0, 100);

    mp_Dialog = new MainMenu::CTextDialog(this);
    mp_Dialog->SetNintyPercentScroll(true);

    // add empty items
    AddEmptyRows();

    CONNECTSIGNALSLOTGUI(mp_Ui->pushButton, clicked(), this, OpenButtonClicked());
    CONNECTSIGNALSLOTGUI(mp_TableWidget, pressed(const QModelIndex&), this, ItemActivated(const QModelIndex&));
    CONNECTSIGNALSLOTGUI(mp_Dialog, LanguageChanged(), this, RetranslateUI());

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CFileView::~CFileView()
{
    try {
        // delete all the items in the table
        for (qint32 RowCount = 0; RowCount < m_Model.rowCount(); RowCount++) {
            delete m_Model.takeItem(RowCount);
        }
        delete mp_Dialog;
        delete mp_TableWidget;
        delete mp_Ui;
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CFileView::changeEvent(QEvent *p_Event)
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
 *  \brief  Function to set Dialog caption
 *
 *  \iparam Caption String
 */
/****************************************************************************/
void CFileView::SetCaption(QString Caption)
{
    mp_Dialog->SetCaption(Caption);
}

/****************************************************************************/
/*!
 *  \brief Slot for the clicked event
 */
/****************************************************************************/
void CFileView::OpenButtonClicked()
{
    // emit the signal
    emit GetFileContent(m_Model.itemFromIndex(mp_TableWidget->currentIndex())->text().trimmed());
}

/****************************************************************************/
/*!
 *  \brief Slot for the item activation
 *
 *  \iparam Index = Index of the row
 */
/****************************************************************************/
void CFileView::ItemActivated(const QModelIndex &Index)
{
    if (Index.row() > (m_SelectableRows - 1)) {
        mp_TableWidget->selectRow(m_PreviousIndex);
    }
    else {
        m_PreviousIndex = mp_TableWidget->currentIndex().row();
    }
}

/****************************************************************************/
/*!
 *  \brief Day run log file content
 *
 *  \iparam DataStream = File stream
 */
/****************************************************************************/
void CFileView::DayRunLogFileContent(const QDataStream &DataStream)
{
    (void)DataStream.device()->reset();
    // read all the data into string
    QString Text = QString::fromUtf8(DataStream.device()->readAll());

    // tr function is used for translator
    mp_Dialog->SetDialogTitle(QApplication::translate("MainMenu::CFileView", "Event Viewer",
                                                      0, QApplication::UnicodeUTF8));
    mp_Dialog->SetCaption(m_Model.itemFromIndex(mp_TableWidget->currentIndex())->text() + " ");
    mp_Dialog->setModal(true);
    mp_Dialog->show();
    mp_Dialog->EnableTable(false);
    int Counter = 0;
    mp_Dialog->SetPlainText(Text.mid(Counter, 5000));
    Counter += 5000;
    // for the large file to load the text it takes more time
    // so if the text is loaded by parts then hanging issue can be solved.
    do {
        QCoreApplication::processEvents();
        if (Counter == 5000) {
            emit ShowDialog();
        }
        mp_Dialog->AppendText(Text.mid(Counter, 10000));
        Counter += 10000;

    } while(Counter < Text.count());
    mp_Dialog->EnableTable(true);
    // hide the wait dialog
    emit HideDialog();


}

/****************************************************************************/
/*!
 *  \brief Display the day run log files in the list
 *
 *  \iparam FileNames = List of file names
 */
/****************************************************************************/
void CFileView::DayRunLogFileNames(const QStringList &FileNames)
{   
    m_SelectableRows = FileNames.count();
    // check the number of rows
    if (m_Model.rowCount() > 0) {
        // delete all the items in the table
        for (qint32 RowCount = 0; RowCount < m_Model.rowCount(); RowCount++) {
            delete m_Model.takeItem(RowCount);
        }
        //clear the table
        m_Model.clear();
        // Set horizontal header item
        m_Model.setHorizontalHeaderItem(0, new QStandardItem("   " + tr("File Name")));
    }

    // display all the file names
    for (qint32 FileCount = 0; FileCount < FileNames.count(); FileCount++) {
        // append the row in tables
        m_Model.appendRow(new QStandardItem("   " + FileNames.value(FileCount)));
    }
    // add empty rows if required
    AddEmptyRows();

    // check file name count
    if (FileNames.count() != 0) {
        if (m_Model.rowCount() > 1) {
            mp_TableWidget->selectRow(0);
        }
        // enable the button if the items are available in the table
        mp_Ui->pushButton->setEnabled(true);
    }
    else {
        mp_Ui->pushButton->setEnabled(false);
    }
}

/****************************************************************************/
/**
 * \brief Set the panel size of the Eventviewer window
 *
 * \param[in] Width = width of the panel
 * \param[in] Height = Height of the panel
 */
/****************************************************************************/
void CFileView::SetEventViewerPanelSize(const qint32 &Width, const qint32 &Height)
{
    mp_Dialog->move(90, 40);
    // if the Main screen resolution is 800X600 then the panel size will be 700X550
    mp_Dialog->resize(Width - 100, Height - 50);
}

/****************************************************************************/
/**
 * \brief Disable the "Open" button in the widget
 */
/****************************************************************************/
void CFileView::DisableButton()
{
    mp_Ui->pushButton->setEnabled(false);
}

/****************************************************************************/
/**
 * \brief Add empty rows in the model
 */
/****************************************************************************/
void CFileView::AddEmptyRows()
{
    if (m_Model.rowCount() < 9) {
        // display all the file names
        for (qint32 RowCount = m_Model.rowCount(); RowCount < 8; RowCount++) {
            QStandardItem *p_Item = new QStandardItem();
            p_Item->setSelectable(false);
            // append the row in tables
            m_Model.appendRow(p_Item);
            QPalette Palette;
            (void) m_Model.setData(m_Model.index(RowCount, 0), Palette.color(QPalette::Window), (int) Qt::BackgroundColorRole);
        }
    }
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CFileView::RetranslateUI()
{
    m_Model.setHorizontalHeaderItem(0, new QStandardItem(QApplication::translate("MainMenu::CFileView", "File Name",
                                                                                   0, QApplication::UnicodeUTF8)));
    mp_Dialog->SetDialogTitle(QApplication::translate("MainMenu::CFileView", "Event Viewer",
                                                      0, QApplication::UnicodeUTF8));
    if (mp_TableWidget->currentIndex().row() >= 0) {
        mp_Dialog->SetCaption(m_Model.itemFromIndex(mp_TableWidget->currentIndex())->text() + " ");
    }
}

} // end namespace MainMenu


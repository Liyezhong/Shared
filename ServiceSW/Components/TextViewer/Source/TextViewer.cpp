/****************************************************************************/
/*! \file TextViewer.cpp
 *
 *  \brief Text Viewer implementation.
 *
 *  \b Description:
 *          This class list all files from a given directory in a tabular
 *          format.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-01-16
 *   $Author:  $ Soumya. D
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

#include "TextViewer/Include/TextViewer.h"
#include "ui_TextViewer.h"
#include "Global/Include/SystemPaths.h"

namespace TextViewer {

const int FIXED_TABLEWIDGET_SIZE = 350;         ///< Table widget width and height

/****************************************************************************/
/*!
 *  \brief Constructor 
 *  \iparam FileType = File type
 *  \iparam FilePath = File Path
 *  \iparam p_Parent    = Parent widget
 */
/****************************************************************************/
CTextViewer::CTextViewer(QString FileType, QString FilePath, QWidget *p_Parent)
    : QWidget(p_Parent),
      mp_Ui(new Ui::CTextViewer),
      m_FileType(FileType),
      m_FilePath(FilePath)

{
    mp_Ui->setupUi(this);
    m_CurrentIndex = 0;
    m_FileName =  "";
    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->resize(FIXED_TABLEWIDGET_SIZE, FIXED_TABLEWIDGET_SIZE);

    QDir Directory(m_FilePath);
    Directory.setFilter(QDir::Files);
    QFileInfoList List = Directory.entryInfoList();
    for (int i = 0; i < List.size(); i++) {
        QFileInfo fileInfo = List.at(i);
        QString FileName = fileInfo.fileName();
        if (m_FileType.startsWith("Leica_ST_")) {
            if ((FileName.startsWith(m_FileType)) && (!FileName.startsWith("Leica_ST_Service"))
                    && (FileName.endsWith("log"))) {
                AddItem(fileInfo.fileName());
            }
        } else if ((FileName.startsWith(m_FileType)) && (FileName.endsWith("log"))) {
            AddItem(fileInfo.fileName());
        }
    }

    mp_TableWidget->setModel(&m_Model);    

    mp_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mp_Ui->widget->setMinimumSize(mp_TableWidget->width(), mp_TableWidget->height());
    mp_Ui->widget->SetContent(mp_TableWidget);

    mp_MessageDlg = new MainMenu::CMessageDlg(this);
    mp_MessageDlg->setModal(true);

    QObject::connect(mp_TableWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(SelectionChanged(QModelIndex)));
    QObject::connect(mp_Ui->showDetailsBtn, SIGNAL(clicked()), this, SLOT(ExecDialog()));

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CTextViewer::~CTextViewer()
{
    try {                        
        delete mp_MessageDlg;
        delete mp_TableWidget;
        delete mp_Ui;
    }
    catch (...) {
        // to please Lint
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CTextViewer::changeEvent(QEvent *p_Event)
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
 *  \brief  To add data item to the table
 *  \iparam FileName = File name
 */
/****************************************************************************/
void CTextViewer::AddItem(QString FileName)
{
    QList<QStandardItem *> ItemList;
    ItemList << new QStandardItem(FileName);

    m_Model.appendRow(ItemList);
}

/****************************************************************************/
/*!
 *  \brief To get the index of selected row from the table
 *  \iparam Index = Index of the row
 */
/****************************************************************************/
void CTextViewer::SelectionChanged(QModelIndex Index)
{
    m_CurrentIndex = Index.row();
    m_FileName = Index.data((int)Qt::DisplayRole);
}

/****************************************************************************/
/*!
 *  \brief  Pop's up a dialog for service log.
 */
/****************************************************************************/
void CTextViewer::ExecDialog()
{    
    if (m_FileName.toString().isEmpty()) {
        mp_MessageDlg->SetTitle(QApplication::translate("LogViewer::CSystemLogViewer",
                                              "Warning", 0, QApplication::UnicodeUTF8));
        mp_MessageDlg->SetButtonText(1, QApplication::translate("LogViewer::CSystemLogViewer",
                                                      "Ok", 0, QApplication::UnicodeUTF8));
        mp_MessageDlg->HideButtons();
        mp_MessageDlg->SetText(QApplication::translate("LogViewer::CSystemLogViewer",
                                             "Please Select a log file to continue.", 0,
                                                       QApplication::UnicodeUTF8));
        mp_MessageDlg->SetIcon(QMessageBox::Warning);
        mp_MessageDlg->show();
    } else {
        emit DisplayFileContents(m_FileName.toString(), m_FilePath);
    }
}

/****************************************************************************/
/*!
 *  \brief  Slot called for updating log file
 */
/****************************************************************************/
void CTextViewer::UpdateFileTableEntries()
{
    m_Model.clear();
    QDir Directory(m_FilePath);
    Directory.setFilter(QDir::Files);
    QFileInfoList List = Directory.entryInfoList();
    for (int i = 0; i < List.size(); i++) {
        QFileInfo fileInfo = List.at(i);
        QString FileName = fileInfo.fileName();
        if (FileName.startsWith(m_FileType)) {
            AddItem(fileInfo.fileName());
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Slot called for clearing FilePath variable
 */
/****************************************************************************/
void CTextViewer::ResetLogFilePath()
{
    m_FileName.clear();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CTextViewer::RetranslateUI()
{
    mp_MessageDlg->SetTitle(QApplication::translate("LogViewer::CLogViewer",
                                                    "Warning", 0, QApplication::UnicodeUTF8));

    mp_MessageDlg->SetText(QApplication::translate("LogViewer::CLogViewer",
                           "Please Select a file to continue.", 0, QApplication::UnicodeUTF8));


}

} // end namespace TextViewer

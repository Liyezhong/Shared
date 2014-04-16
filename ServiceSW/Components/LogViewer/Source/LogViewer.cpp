/****************************************************************************/
/*! \file LogViewer.cpp
 *
 *  \brief Log Viewer implementation.
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

#include "LogViewer/Include/LogViewer.h"
#include "ui_LogViewer.h"
#include "Global/Include/SystemPaths.h"

namespace LogViewer {

const int FIXED_TABLEWIDGET_SIZE = 350;         ///< Table widget width and height

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataConnector = Service data connector object
 *  \iparam LogFileName = File name
 *  \iparam LogFilePath = File Path
 *  \iparam p_Parent    = Parent widget
 */
/****************************************************************************/
CLogViewer::CLogViewer(QString LogFileName, QString LogFilePath, QWidget *p_Parent)
    : QWidget(p_Parent),
      mp_Ui(new Ui::CLogViewer),
      m_LogFileType(LogFileName),
      m_LogFilePath(LogFilePath)

{
    mp_Ui->setupUi(this);
    m_CurrentIndex = 0;
    m_LogFileName =  "";
    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->resize(FIXED_TABLEWIDGET_SIZE, FIXED_TABLEWIDGET_SIZE);

    QDir Directory(m_LogFilePath);
    Directory.setFilter(QDir::Files);
    QFileInfoList List = Directory.entryInfoList();
    for (int i = 0; i < List.size(); i++) {
        QFileInfo fileInfo = List.at(i);
        QString FileName = fileInfo.fileName();
        if (FileName.startsWith(m_LogFileType)) {
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
CLogViewer::~CLogViewer()
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
void CLogViewer::changeEvent(QEvent *p_Event)
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
 *  \iparam LogFileName = System log file name
 */
/****************************************************************************/
void CLogViewer::AddItem(QString LogFileName)
{
    QList<QStandardItem *> ItemList;
    ItemList << new QStandardItem(LogFileName);

    m_Model.appendRow(ItemList);
}

/****************************************************************************/
/*!
 *  \brief To get the index of selected row from the table
 *  \iparam Index = Index of the row
 */
/****************************************************************************/
void CLogViewer::SelectionChanged(QModelIndex Index)
{
    m_CurrentIndex = Index.row();
    m_LogFileName = Index.data((int)Qt::DisplayRole);
}

/****************************************************************************/
/*!
 *  \brief  Pop's up a dialog for service log.
 */
/****************************************************************************/
void CLogViewer::ExecDialog()
{    
    if (m_LogFileName.toString().isEmpty()) {
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
        emit DisplayLogFileContents(m_LogFileName.toString(), m_LogFilePath);
    }
}

/****************************************************************************/
/*!
 *  \brief  Slot called for updating log file
 */
/****************************************************************************/
void CLogViewer::UpdateLogFileTableEntries()
{
    m_Model.clear();
    QDir Directory(m_LogFilePath);
    Directory.setFilter(QDir::Files);
    QFileInfoList List = Directory.entryInfoList();
    for (int i = 0; i < List.size(); i++) {
        QFileInfo fileInfo = List.at(i);
        QString FileName = fileInfo.fileName();
        if (FileName.startsWith(m_LogFileType)) {
            AddItem(fileInfo.fileName());
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  Slot called for clearing LogFilePath variable
 */
/****************************************************************************/
void CLogViewer::ResetLogFilePath()
{
    m_LogFileName.clear();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CLogViewer::RetranslateUI()
{
    mp_MessageDlg->SetTitle(QApplication::translate("LogViewer::CSystemLogViewer",
                                                    "Warning", 0, QApplication::UnicodeUTF8));

    mp_MessageDlg->SetText(QApplication::translate("LogViewer::CSystemLogViewer",
                           "Please Select a log file to continue.", 0, QApplication::UnicodeUTF8));


}

} // end namespace LogViewer

/****************************************************************************/
/*! \file DataManagement.cpp
 *
 *  \brief DataManagement implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-22
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

#include "MainMenu/Include/DataManagement.h"
#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "ui_DataManagement.h"
#include <QDebug>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CDataManagement::CDataManagement(QWidget *p_Parent) : QWidget(p_Parent), mp_Ui(new Ui::CDataManagement)
{
    mp_Ui->setupUi(this);

    mp_Dialog = new MainMenu::CWaitDialog(this);

    if (!connect(mp_Ui->exportButton, SIGNAL(clicked()), this, SLOT(ExportDialog()))) {
        qDebug() << "CDataManagement: cannot connect 'clicked' signal";
    }
    if (!connect(mp_Ui->importButton, SIGNAL(clicked()), this, SLOT(ImportDialog()))) {
        qDebug() << "CDataManagement: cannot connect 'clicked' signal";
    }
    if (!connect(mp_Ui->serviceExportButton, SIGNAL(clicked()), this, SLOT(ServiceExportDialog()))) {
        qDebug() << "CDataManagement: cannot connect 'clicked' signal";
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDataManagement::~CDataManagement()
{
    try {
        delete mp_Dialog;
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

void CDataManagement::changeEvent(QEvent *p_Event)
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

void CDataManagement::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event);
    ResetButtons();
}
/****************************************************************************/
/*!
 *  \brief Starts the export procedure and the wait dialog
 */
/****************************************************************************/
void CDataManagement::ExportDialog()
{
    QStringList Type;
    Type << "User";

    emit ExecSending("DataExport", Type);

//    mp_Dialog->SetDialogTitle(tr("User Export"));
//    mp_Dialog->SetText(tr("Exporting user data ..."));

//    mp_Dialog->show();
}

/****************************************************************************/
/*!
 *  \brief Starts the import procedure and the wait dialog
 */
/****************************************************************************/
void CDataManagement::ImportDialog()
{
    QStringList Type;
//    Type << "all";

    emit ExecSending("DataImport", Type);
//    mp_Dialog->SetDialogTitle(tr("Import"));
//    mp_Dialog->SetText(tr("Importing data ..."));

//    mp_Dialog->show();
}

/****************************************************************************/
/*!
 *  \brief Starts the service export procedure and the wait dialog
 */
/****************************************************************************/
void CDataManagement::ServiceExportDialog()
{
    QStringList Type;
    Type << "Service";
    emit ExecSending("DataExport", Type);
//    mp_Dialog->SetDialogTitle(tr("Service Export"));
//    mp_Dialog->SetText(tr("Exporting service data ..."));

//    mp_Dialog->show();

}

/****************************************************************************/
/*!
 *  \brief This slot is called when User Role changes
 */
/****************************************************************************/
void CDataManagement::OnUserRoleChanged()
{
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CDataManagement::OnProcessStateChanged()
{
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Enables/Disables the apply button based on the user role/process
 *         running status
 */
/****************************************************************************/
void CDataManagement::ResetButtons()
{
    m_CurrentUserRole = MainMenu::CMainWindow::GetCurrentUserRole();
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if (!m_ProcessRunning) {
        mp_Ui->exportButton->setEnabled(true);
        mp_Ui->serviceExportButton->setEnabled(true);
        if ((m_CurrentUserRole == MainMenu::CMainWindow::Admin ||
             m_CurrentUserRole == MainMenu::CMainWindow::Service)) {
            mp_Ui->importButton->setEnabled(true);
            mp_Ui->remoteSWUpdateButton->setEnabled(true);
            mp_Ui->softwareUpdateButton->setEnabled(true);
        }
        else {
            mp_Ui->importButton->setEnabled(false);
            mp_Ui->remoteSWUpdateButton->setEnabled(false);
            mp_Ui->softwareUpdateButton->setEnabled(false);
        }
    }
    else {
        mp_Ui->exportButton->setEnabled(false);
        mp_Ui->importButton->setEnabled(false);
        mp_Ui->serviceExportButton->setEnabled(false);
        mp_Ui->remoteSWUpdateButton->setEnabled(false);
        mp_Ui->softwareUpdateButton->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 */
/****************************************************************************/
void CDataManagement::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;
    CONNECTSIGNALSLOT(mp_MainWindow, UserRoleChanged(), this, OnUserRoleChanged());
    CONNECTSIGNALSLOT(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CDataManagement::RetranslateUI()
{
   mp_Dialog->SetDialogTitle(QApplication::translate("MainMenu::CDataManagement", "Import", 0, QApplication::UnicodeUTF8));
   mp_Dialog->SetDialogTitle(QApplication::translate("MainMenu::CDataManagement", "Export", 0, QApplication::UnicodeUTF8));
   mp_Dialog->SetText(QApplication::translate("MainMenu::CDataManagement", "Importing data ...", 0, QApplication::UnicodeUTF8));
   mp_Dialog->SetText(QApplication::translate("MainMenu::CDataManagement", "Exporting data ...", 0, QApplication::UnicodeUTF8));
}

} // end namespace MainMenu

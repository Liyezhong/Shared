/****************************************************************************/
/*! \file DataManagement.cpp
 *
 *  \brief CDataManagement class definition file for Import/Export
 *
 *  \b Description:
 *          This class implements functions to Import/Export of data.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-05-08
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

#include "../Include/PlatformServiceEventCodes.h"
#include "Settings/Include/DataManagement.h"
#include "MainMenu/Include/FileSelection.h"
#include "ui_DataManagement.h"
#include <QDebug>

namespace Settings {


/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataConnector = Service data connector object
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CDataManagement::CDataManagement(QWidget *p_Parent) : QWidget(p_Parent),
    mp_Ui(new Ui::CDataManagement),
    mp_MainWindow(NULL),
    mp_WaitDialog(NULL),
    mp_MessageDlg(NULL)
{   
    mp_Ui->setupUi(this);
    mp_WaitDialog = new MainMenu::CWaitDialog(this);
    mp_WaitDialog->setModal(true);

    CONNECTSIGNALSLOTGUI(mp_Ui->exportBtn, clicked(), this, ExportFiles());
    CONNECTSIGNALSLOTGUI(mp_Ui->importBtn, clicked(), this, ImportFiles());

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDataManagement::~CDataManagement()
{
    try {
//        delete mp_MainWindow;
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
void CDataManagement::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            this->RetranslateUI();
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Slot called when Export button is pressed
 */
/****************************************************************************/
void CDataManagement::ExportFiles()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_GUI_SETTINGS_DATAMANAGEMENT_EXPORT);

    emit ExecSending("Export", "Service");
    if (mp_WaitDialog) {
        delete mp_WaitDialog;
        mp_WaitDialog = NULL;
    }
    mp_WaitDialog = new MainMenu::CWaitDialog(this);
    mp_WaitDialog->setModal(true);
    mp_WaitDialog->SetDialogTitle(QApplication::translate("ServiceUpdates::CDataManagement","Service Export",
                                                          0, QApplication::UnicodeUTF8));
    mp_WaitDialog->SetText(QApplication::translate("ServiceUpdates::CDataManagement","Exporting service data...",
                                                   0, QApplication::UnicodeUTF8));
    mp_WaitDialog->show();
    mp_WaitDialog->setModal(false);
    mp_WaitDialog->HideAbort();
}

/****************************************************************************/
/*!
 *  \brief Slot called when Import button is pressed
 */
/****************************************************************************/
void CDataManagement::ImportFiles()
{    
    Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_GUI_SETTINGS_DATAMANAGEMENT_IMPORT);
    emit ExecSending("Import", "Service");

    if (mp_WaitDialog) {
        delete mp_WaitDialog;
        mp_WaitDialog = NULL;
    }

    mp_WaitDialog = new MainMenu::CWaitDialog(this);
    mp_WaitDialog->setModal(true);
    mp_WaitDialog->SetDialogTitle(QApplication::translate("ServiceUpdates::CDataManagement", "Service Import",
                                                          0, QApplication::UnicodeUTF8));
    mp_WaitDialog->SetText(QApplication::translate("ServiceUpdates::CDataManagement", "Importing service data...",
                                                   0, QApplication::UnicodeUTF8));
    mp_WaitDialog->show();
    mp_WaitDialog->setModal(false);
    mp_WaitDialog->HideAbort();
}

/****************************************************************************/
/*!
 *  \brief Slot called when Import/Export is completed
 *  \iparam ExitCode = Exit code for Import/Export
 *  \iparam IsImport = True or false
 */
/****************************************************************************/
void CDataManagement::ImportExportFinished(int ExitCode, bool IsImport)
{
    QString DialogTitle("Data Import/Export failed.");
    QString MessageInfo("Data Import/Export failed.");

    (void) mp_WaitDialog->close();
    mp_MessageDlg = new MainMenu::CMessageDlg(this);
    mp_MessageDlg->setModal(true);
    mp_MessageDlg->SetIcon(QMessageBox::Critical);

    switch(ExitCode) {
    case EVENT_SERVICE_EXPORT_FAILED:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_FAILED);
        DialogTitle = QString("Data Export");
        MessageInfo = QString("Export of data failed.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORT_FAILED:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_FAILED);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Import of data failed.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORT_NO_FILES_TO_IMPORT:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_NO_FILES_TO_IMPORT);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import failed: No files to import.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORTEXPORT_IMPORT_NO_USB:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORTEXPORT_IMPORT_NO_USB);
        qDebug() << EVENT_SERVICE_IMPORTEXPORT_IMPORT_NO_USB;
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import failed: No USB connected.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORTEXPORT_EXPORT_NO_USB:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORTEXPORT_EXPORT_NO_USB);
        DialogTitle = QString("Data Export");
        MessageInfo = QString("Data Export failed: No USB connected.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_EXPORT_DIRECTORY_CREATION_FAILED:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_DIRECTORY_CREATION_FAILED);
        DialogTitle = QString("Data Export");
        MessageInfo = QString("Data Export failed: Export directory creation failed.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_EXPORT_FILES_NOT_COPIED:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_FILES_NOT_COPIED);
        DialogTitle = QString("Data Export");
        MessageInfo = QString("Data Export failed: Cannot copy files.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_EXPORT_UNABLE_TO_CREATE_FILE_TEMP_EXPORTCONFIGURATION:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_UNABLE_TO_CREATE_FILE_TEMP_EXPORTCONFIGURATION);
        DialogTitle = QString("Data Export");
        MessageInfo = QString("Data Export failed: Unable to create Temp export configuration file.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORT_UPDATE_ROLLBACK_FAILED:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_UPDATE_ROLLBACK_FAILED);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import failed: update rollback files failed.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORT_TAMPERED_ARCHIVE_FILE:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_TAMPERED_ARCHIVE_FILE);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import failed.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORT_DEVICE_NAME_NOT_MATCHING:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_DEVICE_NAME_NOT_MATCHING);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import failed: Device name is not matching.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORT_ARCHIVE_FILE_FORMAT_NOT_PROPER:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_ARCHIVE_FILE_FORMAT_NOT_PROPER);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import failed: Invalid archive file format.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORT_TYPEOFIMPORTNOTVALID:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_TYPEOFIMPORTNOTVALID);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import failed: Invalid type of Import.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORT_REQUIRED_FILES_NOT_AVAILABLE:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_REQUIRED_FILES_NOT_AVAILABLE);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import failed: Import files are not available.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_IMPORT_ROLLBACK_FAILED:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_ROLLBACK_FAILED);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import failed: Rollback failed.");
        mp_MessageDlg->SetIcon(QMessageBox::Critical);
        break;
    case EVENT_SERVICE_EXPORT_SUCCESS:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_SUCCESS);
        DialogTitle = QString("Data Export");
        MessageInfo = QString("Data Export completed successfully.");
        mp_MessageDlg->SetIcon(QMessageBox::Information);
        break;
    case Global::EVENT_EXPORT_SUCCESS:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_EXPORT_SUCCESS);
        DialogTitle = QString("Data Export");
        MessageInfo = QString("Data Export completed successfully.");
        mp_MessageDlg->SetIcon(QMessageBox::Information);
        break;
    case EVENT_SERVICE_IMPORT_SUCCESS:
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_IMPORT_SUCCESS);
        DialogTitle = QString("Data Import");
        MessageInfo = QString("Data Import completed successfully.");
        mp_MessageDlg->SetIcon(QMessageBox::Information);
        break;
    default:
        if (IsImport) {
            if (ExitCode != 0) {
                DialogTitle = QString("Data Import");
                MessageInfo = QString("Data Import Failed.");
                mp_MessageDlg->SetIcon(QMessageBox::Critical);
            } else {
                DialogTitle = QString("Data Import");
                MessageInfo = QString("Data Import completed successfully.");
                mp_MessageDlg->SetIcon(QMessageBox::Information);
            }
        }
        break;
    }

    mp_MessageDlg->SetTitle(DialogTitle);
    mp_MessageDlg->SetButtonText(1, QApplication::translate("ServiceUpdates::CDataManagement", "Ok",
                                                            0, QApplication::UnicodeUTF8));
    mp_MessageDlg->HideButtons();
    mp_MessageDlg->SetText(MessageInfo);

    mp_MessageDlg->show();
}

/****************************************************************************/
/*!
 *  \brief Slot called when multiple archive files exist.
 *  \iparam FileList = List of archive file names
 */
/****************************************************************************/
void CDataManagement::FileSelectionForImport(QStringList FileList)
{
    (void) mp_WaitDialog->close();
    MainMenu::CFileSelection FileSelection;

    CONNECTSIGNALSIGNALGUI(&FileSelection, SelectedFileList(QStringList), this, SelectedImportFileList(QStringList));

    FileSelection.SetData(1, "_", 1, QStringList() << "Service", QStringList() << "Service");
    FileSelection.SetFileList(FileList);
    FileSelection.setModal(true);
    FileSelection.SetFileSelectionPanelSize(600, 400);
    (void) FileSelection.exec();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CDataManagement::RetranslateUI()
{
    mp_WaitDialog->SetDialogTitle(QApplication::translate("ServiceUpdates::CDataManagement",
                                                          "Service Export", 0, QApplication::UnicodeUTF8));

    mp_WaitDialog->SetText(QApplication::translate("ServiceUpdates::CDataManagement",
                                                   "Exporting service data...", 0, QApplication::UnicodeUTF8));

    mp_WaitDialog->SetDialogTitle(QApplication::translate("ServiceUpdates::CDataManagement",
                                                          "Service Import", 0, QApplication::UnicodeUTF8));

    mp_WaitDialog->SetText(QApplication::translate("ServiceUpdates::CDataManagement",
                                                   "Importing service data...", 0, QApplication::UnicodeUTF8));

}

} // end namespace Settings

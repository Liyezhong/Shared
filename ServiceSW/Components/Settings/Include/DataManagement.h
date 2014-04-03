/****************************************************************************/
/*! \file DataManagement.h
 *
 *  \brief DataManagement definition file (Import and Export)
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

#ifndef SETTINGS_DATAMANAGEMENT_H
#define SETTINGS_DATAMANAGEMENT_H


#include <QDir>
#include <QProcess>
#include <QWidget>
#include <MainMenu/Include/MessageDlg.h>
#include <MainMenu/Include/WaitDialog.h>
#include <MainMenu/Include/MainWindow.h>

namespace Settings {

namespace Ui {
    class CDataManagement;
}
/****************************************************************************/
/**
 * \brief This widget implements import and export of data
 */
/****************************************************************************/
class CDataManagement : public QWidget
{
    Q_OBJECT

public:
    explicit CDataManagement(QWidget *p_Parent = 0);
    ~CDataManagement();

protected:
    void changeEvent(QEvent *p_Event);

private:
    Ui::CDataManagement *mp_Ui;                     //!< User Interface
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow
    MainMenu::CWaitDialog *mp_WaitDialog;           //!< Pointer to WaitDialog
    MainMenu::CMessageDlg *mp_MessageDlg;           //!< Pointer to Message Dialog    

signals:
    /****************************************************************************/
    /**
     * \brief Sends a data import/export request to the service master
     * \iparam Name = Message command name
     * \iparam Type = Import/Export type
     */
    /****************************************************************************/
     void ExecSending(const QString Name, const QString Type);

     /****************************************************************************/
     /**
      * \brief Signal sends list of archived files exist
      * \iparam FileList = List of archive file names
      */
     /****************************************************************************/
     void SelectedImportFileList(QStringList FileList);

private slots:
    void RetranslateUI();

public slots:
     void ExportFiles();
     void ImportFiles();
     void ImportExportFinished(int ExitCode, bool IsImport);
     void FileSelectionForImport(QStringList FileList);

};

} // end namespace Settings

#endif //SETTINGS_DATAMANAGEMENT_H

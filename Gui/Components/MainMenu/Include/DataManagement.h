/****************************************************************************/
/*! \file DataManagement.h
 *
 *  \brief Header file for class CDataManagement.
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

#ifndef MAINMENU_DATAMANAGEMENT_H
#define MAINMENU_DATAMANAGEMENT_H

#include "MainMenu/Include/WaitDialog.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/ServiceExportDlg.h"
#include <QWidget>

namespace MainMenu {

namespace Ui {
    class CDataManagement;
}

/****************************************************************************/
/**
 * \brief This widget contains two buttons starting the device data import
 *        and export process.
 */
/****************************************************************************/
class CDataManagement : public QWidget
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    explicit CDataManagement(QWidget *p_Parent = 0);
    virtual ~CDataManagement();
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);

signals:

    /****************************************************************************/
    /**
     * \brief Sends a data import/export request to the master
     *
     * \iparam Name = Message command name
     * \iparam ParamList = Message parameters
     */
    /****************************************************************************/
     void ExecSending(const QString Name, const QStringList &ParamList);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when softwre update button is clicked.
     *
     *  \iparam SWUpdateUsingUSB = True if SW Update is done using USB else False.
     *
     */
    /****************************************************************************/
    void UpdateSoftware(bool SWUpdateUsingUSB);

    /****************************************************************************/
    /**
     * \brief This signal is emitted when RC softwre update button is clicked.
     */
    /****************************************************************************/
    void UpdateSoftwareFromRC();


private:
    Ui::CDataManagement *mp_Ui;         //!< User interface
    MainMenu::CWaitDialog *mp_Dialog;   //!< Dialog content
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow
    //Flags
    bool m_ProcessRunning;                  //!< Process running state
    //UI related
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //!< Current user role
    MainMenu::CServiceExportDlg *mp_ServiceExportDlg;         //!< Service Export dialog
    void ResetButtons();
    void RetranslateUI();
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CDataManagement)

protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *p_Event);

private slots:
    void ExportDialog();
    void ImportDialog();
    void ServiceExportDialog();
    void OnProcessStateChanged();
    void OnUserRoleChanged();
    void OnRemoteSWUpdate();
    void OnSoftwareUpdate();
    void OnServiceExportFileSelection(int NoOfFiles);

public slots:
    void SetRemoteSWButtonState(bool Status);
};

} // end namespace MainMenu

#endif // MAINMENU_DATAMANAGEMENT_H

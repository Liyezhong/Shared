/****************************************************************************/
/*! \file DataManagement.h
 *
 *  \brief DataManagement definition.
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

private:
    Ui::CDataManagement *mp_Ui;         //!< User interface
    MainMenu::CWaitDialog *mp_Dialog;   //!< Dialog content
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow
    //Flags
    bool m_ProcessRunning;                  //!< Process running state
    //UI related
    MainMenu::CMainWindow::UserRole_t m_CurrentUserRole;    //! < Current user role
    void ResetButtons();
    void RetranslateUI();

protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *p_Event);

private slots:
    void ExportDialog();
    void ImportDialog();
    void ServiceExportDialog();
    void OnProcessStateChanged();
    void OnUserRoleChanged();

};

} // end namespace MainMenu

#endif // MAINMENU_DATAMANAGEMENT_H

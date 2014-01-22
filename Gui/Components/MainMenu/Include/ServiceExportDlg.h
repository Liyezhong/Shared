/****************************************************************************/
/*! \file ServiceExportDlg.h
 *
 *  \brief Include file for class CServiceExportDlg.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-07-04
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
#ifndef MAINMENU_SERVICEEXPORTDLG_H
#define MAINMENU_SERVICEEXPORTDLG_H

#include "MainMenu/Include/MainWindow.h"
#include <QWidget>

namespace MainMenu {

namespace Ui {
    class CServiceExportDlg;
}

/****************************************************************************/
/**
 * \brief Dialog for number of log file selection
 */
/****************************************************************************/
class CServiceExportDlg : public CDialogFrame
{
    Q_OBJECT
    friend class  CTestMainMenu;

public:
    explicit CServiceExportDlg(QWidget *p_Parent = 0);
    virtual ~CServiceExportDlg();

private:
    Ui::CServiceExportDlg *mp_Ui;                //!< Service Export dialog UI object    
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CServiceExportDlg)

private slots:
    void OkClicked();
    void RetranslateUI();

signals:
    /****************************************************************************/
    /*!
     *  \brief Signal will be emitted with number of files
     *
     *  \iparam NoOfFiles - No of files to export
     *
     */
    /****************************************************************************/
    void ExportNoOfFiles(int NoOfFiles);

};
} // end namespace MainMenu
#endif // MAINMENU_SERVICEEXPORTDLG_H

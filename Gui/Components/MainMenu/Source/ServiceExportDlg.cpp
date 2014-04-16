/****************************************************************************/
/*! \file ServiceExportDlg.cpp
 *
 *  \brief ServiceExportDlg implementation.
 *
 *  \b Description:
 *          This class allows service user to select number of log
 *          event files for export.
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

#include "Global/Include/Utils.h"
#include "Global/Include/Exception.h"
#include "MainMenu/Include/ServiceExportDlg.h"
#include "ui_ServiceExportDlg.h"

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CServiceExportDlg::CServiceExportDlg(QWidget *p_Parent) :
    MainMenu::CDialogFrame(p_Parent),
    mp_Ui(new Ui::CServiceExportDlg)

{
    SetDialogTitle(QApplication::translate("MainMenu::CServiceExport", "Service Export",
                                           0, QApplication::UnicodeUTF8));
    mp_Ui->setupUi(GetContentFrame());
    mp_Ui->radioButton_5files->setChecked(true);
    CONNECTSIGNALSLOTGUI(mp_Ui->btnOk, clicked(), this, OkClicked());
    CONNECTSIGNALSLOTGUI(this, DialogLangaugeChanged(), this, RetranslateUI());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 *
 */
/****************************************************************************/
CServiceExportDlg::~CServiceExportDlg()
{
    try {
        delete mp_Ui;
    }
    catch (...) {

    }

}

/****************************************************************************/
/*!
 *  \brief This slot is called when Ok button is clicked.
 *
 */
/****************************************************************************/
void CServiceExportDlg::OkClicked()
{
    accept();
    // by default 5 files readio button is selected, so need not require to check
    int NoOfFiles = 5;

    if (mp_Ui->radioButton_10files->isChecked()) {
        NoOfFiles = 10;
    }
    if (mp_Ui->radioButton_15files->isChecked()) {
        NoOfFiles = 15;
    }
    if (mp_Ui->radioButton_30files->isChecked()) {
        NoOfFiles = 30;
    }
    emit ExportNoOfFiles(NoOfFiles);
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CServiceExportDlg::RetranslateUI()
{
    SetDialogTitle(QApplication::translate("MainMenu::CServiceExport", "Service Export",
                                           0, QApplication::UnicodeUTF8));
}
}

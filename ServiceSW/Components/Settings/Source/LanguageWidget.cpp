/****************************************************************************/
/*! \file LanguageWidget.cpp
 *
 *  \brief Implementation file for class CLanguageWidget.
 *
 *  \b Description:
 *          This class implements the widget to display list of languages in the table
 *          and provides user interface to change the language.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-29
 *   $Author:  $ M.Scherer, C.Adaragunchi
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
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "Settings/Include/LanguageWidget.h"
#include "ui_LanguageWidget.h"
#include <QLocale>

namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CLanguageWidget::CLanguageWidget(QWidget *p_Parent) :
    MainMenu::CPanelFrame(p_Parent),
    mp_Ui(new Ui::CLanguageWidget),
    mp_MainWindow(NULL),    
    m_ProcessRunning(false),
    m_SelectedLanguage("")
{
    mp_Ui->setupUi(GetContentFrame());
    SetPanelTitle(QApplication::translate("ServiceUpdates::CLanguageWidget", "Language",
                                          0, QApplication::UnicodeUTF8));
    mp_TableWidget = new MainMenu::CBaseTable;    
    mp_TableWidget->setFixedSize(250, 350);
    mp_TableWidget->setContentsMargins(0, 0, 0, 0);
    m_LanguageModel.SetVisibleRowCount(5);

    mp_TableWidget->setModel(&m_LanguageModel);
    mp_Ui->tableWidget->SetContent(mp_TableWidget);    
    PopulateLanguageList();
    m_LanguageList.append("Deutsch");
    m_LanguageList.append("U.S English");

    mp_MessageDlg = new MainMenu::CMessageDlg(this);
    mp_MessageDlg->setModal(true);

    CONNECTSIGNALSLOTGUI(mp_TableWidget, clicked(QModelIndex), this, SelectionChanged(QModelIndex));
    CONNECTSIGNALSLOTGUI(mp_Ui->btnApply, clicked(), this, OnBtnApplyClicked());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CLanguageWidget::~CLanguageWidget()
{
    try {
        delete mp_MessageDlg;
        delete mp_TableWidget;
        delete mp_Ui;
    }
    catch (...) {
        // to please Lint.
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CLanguageWidget::changeEvent(QEvent *p_Event)
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
 *  \brief Sets the LangauageList to the Langauge table.
 */
/****************************************************************************/
void CLanguageWidget::SetLanguages()
{
    m_LanguageModel.SetLanguageList(m_LanguageList);

    if (m_SelectedLanguage.isEmpty()) {
        if (mp_MainWindow->GetSaMUserMode() == QString("Manufacturing")) {
            m_SelectedLanguage = m_LanguageList.value(0);
        }
        else if (mp_MainWindow->GetSaMUserMode() == QString("Service")) {
            m_SelectedLanguage = m_LanguageList.value(1);
        }
    }

    if (m_SelectedLanguage == m_LanguageList.value(1))
        mp_TableWidget->selectRow(1);
    else
        mp_TableWidget->selectRow(0);

    InitDialog();
}

/****************************************************************************/
/*!
 *  \brief Fills the language selection table
 */
/****************************************************************************/
void CLanguageWidget::PopulateLanguageList()
{
    mp_TableWidget->horizontalHeader()->show();
    mp_TableWidget->SetVisibleRows(5);
    m_LanguageModel.SetVisibleRowCount(5);
    mp_TableWidget->horizontalHeader()->resizeSection(0, 245);
}

/****************************************************************************/
/*!
 *  \brief Called when the apply button is clicked
 */
/****************************************************************************/
void CLanguageWidget::OnBtnApplyClicked()
{
    if (m_SelectedLanguage == m_LanguageList.value(0)) {
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_GUI_SETTINGS_LANGUAGE_CHANGE, Global::tTranslatableStringList() << "Deutsch");
        emit SetLanguage(PlatformService::DEUTSCH);
    } else {
        Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_GUI_SETTINGS_LANGUAGE_CHANGE, Global::tTranslatableStringList() << "U.S English");
        emit SetLanguage(PlatformService::US_ENGLISH);
    }
}

/****************************************************************************/
/*!
 *  \brief Called when the selection of the table is changed
 *
 *  \iparam Index = Currently selected cell
 */
/****************************************************************************/
void CLanguageWidget::SelectionChanged(QModelIndex Index)
{
    int SelectedIndex = Index.row();
    if (SelectedIndex < m_LanguageList.size()) {
        m_SelectedLanguage = m_LanguageList.at(SelectedIndex);
        if (SelectedIndex + 1 <= m_LanguageList.count())  {
            m_SelectedLanguage = (m_LanguageList.at(SelectedIndex));
            ResetButtons();
        }
    }
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CLanguageWidget::OnProcessStateChanged()
{
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Enables/Disables the apply button based on the user role/process
 *         running status
 */
/****************************************************************************/
void CLanguageWidget::ResetButtons()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if (!m_ProcessRunning) {
        //Edit Mode
        mp_Ui->btnApply->setEnabled(true);
    } else {
        mp_Ui->btnApply->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief Called when Language widget is show
 */
/****************************************************************************/
void CLanguageWidget::showEvent(QShowEvent *p_Event)
{
    Q_UNUSED(p_Event);    
    InitDialog();
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Initialises the dialog with the language list
 */
/****************************************************************************/
void CLanguageWidget::InitDialog()
{    
    if (m_LanguageList.count() <= 0) {
        mp_Ui->btnApply->setEnabled(false);
    }
    else {
        mp_Ui->btnApply->setEnabled(true);
    }
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *
 *  \iparam p_MainWindow = Mainwindow object.
 *  \iparam p_DataConnector = DataConnector object.
 */
/****************************************************************************/
void CLanguageWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{    
    mp_MainWindow = p_MainWindow;       
    SetLanguages();
    CONNECTSIGNALSLOTGUI(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CLanguageWidget::RetranslateUI()
{
    MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("Settings::CLanguageWidget", "Language",
                                                                     0, QApplication::UnicodeUTF8));
    (void) m_LanguageModel.setHeaderData(0,Qt::Horizontal,QApplication::translate("Settings::CLanguageModel",
                                                        "Language", 0, QApplication::UnicodeUTF8),0);
    mp_MessageDlg->SetTitle(QApplication::translate("ServiceUpdates::CLanguageWidget",
                                                    "Language files", 0, QApplication::UnicodeUTF8));
    mp_MessageDlg->SetText(QApplication::translate("ServiceUpdates::CLanguageWidget",
                                                   "Translation files are missing.", 0, QApplication::UnicodeUTF8));
    mp_MessageDlg->SetButtonText(1, QApplication::translate("ServiceUpdates::CLanguageWidget",
                                                            "Ok", 0, QApplication::UnicodeUTF8));

}

} // end namespace Settings

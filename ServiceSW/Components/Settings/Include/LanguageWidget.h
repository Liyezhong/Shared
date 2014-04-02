/****************************************************************************/
/*! \file LanguageWidget.h
 *
 *  \brief LanguageWidget definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-06-12
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

#ifndef SETTINGS_LANGUAGEWIDGET_H
#define SETTINGS_LANGUAGEWIDGET_H

#include "../Include/PlatformServiceDefines.h"
#include "MainMenu/Include/MainWindow.h"
#include "MainMenu/Include/BaseTable.h"
#include "MainMenu/Include/PanelFrame.h"
#include "MainMenu/Include/MessageDlg.h"
#include "Settings/Include/LanguageModel.h"
#include <QTranslator>
#include <Global/Include/SystemPaths.h>

namespace Settings {

namespace Ui {
    class CLanguageWidget;
}

/****************************************************************************/
/**
 * \brief This class contains the language selection widget
 */
/****************************************************************************/
class CLanguageWidget : public MainMenu::CPanelFrame
{
    Q_OBJECT

public:
    explicit CLanguageWidget(QWidget *p_Parent = NULL);
    ~CLanguageWidget();
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);
    void SetLanguages();

protected:
    void changeEvent(QEvent *p_Event);
    void showEvent(QShowEvent *);

private:
    Ui::CLanguageWidget *mp_Ui;                     //!< User interface
    MainMenu::CBaseTable *mp_TableWidget;           //!< Language table
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow    
    bool m_ProcessRunning;                          //!< Process running state
    ServiceUpdates::CLanguageModel m_LanguageModel; //!< Language Model object
    QStringList m_LanguageList;                     //!< List of Languages
    QString m_SelectedLanguage;                     //!< Currently selected language
    QHash<QString,QString> m_LanguageNativeNames;   //!< Hash of Languages and thier native names
    QStringList m_LanguageNativeNameList;           //!< List of native language names
    QTranslator m_Translator;                       //!< Language translator
    MainMenu::CMessageDlg *mp_MessageDlg;           //!< Information dialog

    void PopulateLanguageList();
    void ResetButtons();
    void InitDialog();

signals:
    /****************************************************************************/
    /*!
     *  \brief Signal emitted for language setting
     */
    /****************************************************************************/
    void SetLanguage(PlatformService::Languages_t);

private slots:
    void OnBtnApplyClicked();
    void OnProcessStateChanged();
    void SelectionChanged(QModelIndex Index);
    void RetranslateUI();

};

} // end namespace Settings

#endif // SETTINGS_LANGUAGEWIDGET_H

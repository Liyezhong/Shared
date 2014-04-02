/****************************************************************************/
/*! \file DateTimeWidget.h
 *
 *  \brief DateTimeWidget definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-21
 *   $Author:  $ M.Scherer, C.Adaragunchi, Soumya. D
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

#ifndef SETTINGS_DATETIMEWIDGET_H
#define SETTINGS_DATETIMEWIDGET_H

#include "../Include/PlatformServiceDefines.h"
#include "MainMenu/Include/MainWindow.h"
#include "Settings/Include/DateTime.h"
#include "MainMenu/Include/PanelFrame.h"
#include <QShowEvent>

namespace Settings {

/****************************************************************************/
/**
 * \brief Frame for the date and time widget
 */
/****************************************************************************/
class CDateTimeWidget : public MainMenu::CPanelFrame {
    Q_OBJECT

public:
    explicit CDateTimeWidget(QWidget *p_Parent = 0);
    virtual ~CDateTimeWidget();    
    void SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow);
    Settings::CDateTime *GetContent();
    void SetButtonStatus();

protected:
    void showEvent(QShowEvent *p_Event);
    void changeEvent(QEvent *p_Event);

private:
    Settings::CDateTime *mp_DateTime;               //!< Content of this widget
    MainMenu::CMainWindow *mp_MainWindow;           //!< Pointer to MainWindow

private slots:
    void RetranslateUI();
};

} // end namespace Settings

#endif // SETTINGS_DATETIMEWIDGET_H

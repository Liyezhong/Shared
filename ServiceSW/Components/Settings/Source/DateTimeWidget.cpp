/****************************************************************************/
/*! \file DateTimeWidget.cpp
 *
 *  \brief Implementation file for class CDateTimeWidget.
 *
 *  \b Description:
 *          This class implements widget for displaying Date and Time
 *          and provides the user interfaces to change data and time.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-21
 *   $Author:  $ M.Scherer,C.Adaragunchi
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
#include "Settings/Include/DateTimeWidget.h"
#include <QDateTime>
#include <QApplication>
#include <QEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

namespace Settings {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent object
 */
/****************************************************************************/
CDateTimeWidget::CDateTimeWidget(QWidget *p_Parent) :
    MainMenu::CPanelFrame(p_Parent),
    mp_MainWindow(NULL)
{
    mp_DateTime = new Settings::CDateTime(GetContentFrame());
    SetPanelTitle(QApplication::translate("ServiceUpdates::CDateTimeWidget", "Date/Time",
                                          0, QApplication::UnicodeUTF8));
    mp_DateTime->RefreshDateTime(Global::TIME_24);

    QVBoxLayout *mp_VLayOut= new QVBoxLayout(GetContentFrame());
    mp_VLayOut->setContentsMargins(0,0,0,0);
    mp_VLayOut->addWidget(mp_DateTime);
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDateTimeWidget::~CDateTimeWidget()
{
    try {
        delete mp_DateTime;
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

void CDateTimeWidget::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            RetranslateUI();
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief  Returns the content of this panel
 *
 *  \return A pointer to the content
 */
/****************************************************************************/
Settings::CDateTime *CDateTimeWidget::GetContent()
{
    return mp_DateTime;
}

/****************************************************************************/
/*!
 *  \brief Updates the widget content everytime it is displayed
 *
 *  \iparam p_Event = Show event
 */
/****************************************************************************/
void CDateTimeWidget::showEvent(QShowEvent *p_Event) {
    if ((mp_DateTime != NULL) && (p_Event != NULL) &&
            !p_Event->spontaneous()) {     
        mp_DateTime->ResetButtons();
        SetButtonStatus();
    }
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *
 *  \iparam p_MainWindow = MainWindow object.
 *  \iparam p_ServiceGUIConnector = Data connector object
 */
/****************************************************************************/
void CDateTimeWidget::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;    
    if (mp_DateTime) {
        mp_DateTime->SetPtrToMainWindow(mp_MainWindow);
    }   
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CDateTimeWidget::RetranslateUI()
{
   MainMenu::CPanelFrame::SetPanelTitle(QApplication::translate("ServiceUpdates::CDateTimeWidget",
                                                                "Date/Time", 0, QApplication::UnicodeUTF8));
}

/****************************************************************************/
/*!
 *  \brief To set the Apply button status
 *  \iparam p_ServiceGUIConnector = ServiceGUIConnector Object
 */
/****************************************************************************/
void CDateTimeWidget::SetButtonStatus()
{
    if (mp_MainWindow->GetSaMUserMode() == QString("Service")) {
        mp_DateTime->SetApplyButtonStatus(PlatformService::SERVICE_MODE);
    } else if (mp_MainWindow->GetSaMUserMode() == QString("Manufacturing")) {
        mp_DateTime->SetApplyButtonStatus(PlatformService::MANUFACTURING_MODE);
    }
}

} // end namespace Settings

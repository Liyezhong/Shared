/****************************************************************************/
/*! \file DateTime.cpp
 *
 *  \brief Implementation of file for class CDateTime.
 *
 *  \b Description:
 *          This class implements a base widget to display Data/Time menu.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-21
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

#include <Global/Include/AdjustedTime.h>
#include "Application/Include/LeicaStyle.h"
#include "MainMenu/Include/DateTime.h"
#include "ui_DateTime.h"
#include <QDebug>
#include <QPainter>
//#include "Core/Include/GlobalHelper.h"
#include "MainMenu/Include/MessageDlg.h"
namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CDateTime::CDateTime(QWidget *p_Parent) : QWidget(p_Parent), mp_Ui(new Ui::CDateTime)
{
    mp_Ui->setupUi(this);

    mp_DayWheel = new MainMenu::CScrollWheel();
    mp_MonthWheel = new MainMenu::CScrollWheel();
    mp_YearWheel = new MainMenu::CScrollWheel();
    mp_HourWheel = new MainMenu::CScrollWheel();
    mp_MinWheel = new MainMenu::CScrollWheel();

    // Day
    for (int i = 1; i <= 31; i++) {
        mp_DayWheel->AddItem(QString("%1").arg(i, 2, 10, QChar('0')), i);
    }

    // Month
    for (int i = 1; i <= 12; i++) {
        mp_MonthWheel->AddItem(QString("%1").arg(i, 2, 10, QChar('0')), i);
    }

    // Year
    for (int i = 2000; i <= 2099; i++) {
        mp_YearWheel->AddItem(QString::number(i).right(QString::number(i).length() - 2), i);
    }

    // Minute
    for (int i = 0; i < 60; i++) {
        mp_MinWheel->AddItem(QString("%1").arg(i, 2, 10, QChar('0')), i);
    }
    mp_Ui->scrollPanelDate->Init(3);
    mp_Ui->scrollPanelDate->SetTitle(QApplication::translate("MainMenu::CDateTime", "Date", Q_NULLPTR, -1));
    mp_Ui->scrollPanelDate->AddScrollWheel(mp_DayWheel, 0);
    mp_Ui->scrollPanelDate->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Day", Q_NULLPTR, -1), 0);
    mp_Ui->scrollPanelDate->AddSeparator(MainMenu::CWheelPanel::FULLSTOP, 0);
    mp_Ui->scrollPanelDate->AddScrollWheel(mp_MonthWheel, 1);
    mp_Ui->scrollPanelDate->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Month", Q_NULLPTR, -1), 1);
    mp_Ui->scrollPanelDate->AddSeparator(MainMenu::CWheelPanel::FULLSTOP, 1);
    mp_Ui->scrollPanelDate->AddScrollWheel(mp_YearWheel, 2);
    mp_Ui->scrollPanelDate->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Year", Q_NULLPTR, -1), 2);

    mp_Ui->scrollPanelTime->Init(2);
    mp_Ui->scrollPanelTime->SetTitle(QApplication::translate("MainMenu::CDateTime", "Time", Q_NULLPTR, -1));
    mp_Ui->scrollPanelTime->AddScrollWheel(mp_HourWheel, 0);
    mp_Ui->scrollPanelTime->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Hour", Q_NULLPTR, -1), 0);
    mp_Ui->scrollPanelTime->AddSeparator(MainMenu::CWheelPanel::COLON, 0);
    mp_Ui->scrollPanelTime->AddScrollWheel(mp_MinWheel, 1);
    mp_Ui->scrollPanelTime->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Minute", Q_NULLPTR, -1), 1);

    if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
        mp_DayWheel->InitScrollWheel("Large");
        mp_MonthWheel->InitScrollWheel("Large");
        mp_MonthWheel->InitScrollWheel("Large");
        mp_YearWheel->InitScrollWheel("Large");
        mp_HourWheel->InitScrollWheel("Large");
        mp_MinWheel->InitScrollWheel("Large");
        mp_Ui->scrollPanelDate->SetLayoutContentsMargin(15, 20, 13, 25);
        mp_Ui->scrollPanelDate->SetVerticalSpacing(5);        
        mp_Ui->scrollPanelTime->SetLayoutContentsMargin(15, 20, 13, 25);
        mp_Ui->scrollPanelTime->SetVerticalSpacing(5);
    }

    if (!connect(mp_Ui->pushButton, SIGNAL(clicked()), this, SLOT(CollectData()))) {
        qDebug() << "CDateTime: cannot connect 'clicked' signal";
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDateTime::~CDateTime()
{
    try {
        delete mp_DayWheel;
        delete mp_MonthWheel;
        delete mp_YearWheel;
        delete mp_HourWheel;
        delete mp_MinWheel;
        delete mp_Ui;
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/

void CDateTime::changeEvent(QEvent *p_Event)
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
 *  \brief Returns the date and time displayed in the widget
 *
 *  \return Date and time displayed in the widget
 */
/****************************************************************************/
QDateTime CDateTime::GetDateTime()
{
    CollectData(false);
    return m_DateTime;
}

/****************************************************************************/
/*!
 *  \brief Sets the date and time displayed by the scroll wheels
 *
 *  \iparam TimeFormat = 12 or 24 hour format
 */
/****************************************************************************/
void CDateTime::RefreshDateTime(Global::TimeFormat TimeFormat)
{
    // get adjusted time
    QDateTime DateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();

    // Hour
    mp_HourWheel->ClearItems();
    for (int i = 0; i < 24; i++) {
        if (TimeFormat == Global::TIME_24) {
            mp_HourWheel->AddItem(QString("%1").arg(i, 2, 10, QChar('0')), i);
        }
        else {
            QPixmap Pixmap = (i < 12) ? 
                            QPixmap(QString(":/%1/Digits/Digit_AM.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())) :
                            QPixmap(QString(":/%1/Digits/Digit_PM.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())) ;
            qint32 Hour = (i % 12 == 0) ? 12 : i % 12;
            mp_HourWheel->AddItem(QString("%1").arg(Hour, 2, 10, QChar('0')), i, Pixmap);
        }
    }

    mp_DayWheel->SetCurrentData(DateTime.date().day());
    mp_MonthWheel->SetCurrentData(DateTime.date().month());
    mp_YearWheel->SetCurrentData(DateTime.date().year());

    mp_HourWheel->SetCurrentData(DateTime.time().hour());
    mp_MinWheel->SetCurrentData(DateTime.time().minute());
}

/****************************************************************************/
/*!
 *  \brief Reads data from the scroll wheels
 *
 *  \iparam Send = True if data has to be save else False.
 */
/****************************************************************************/
void CDateTime::CollectData(bool Send)
{    
    // make it UTC
    m_DateTime.setTimeSpec(Qt::UTC);

    m_DateTime.setDate(QDate(mp_YearWheel->GetCurrentData().toInt(),
        mp_MonthWheel->GetCurrentData().toInt(), mp_DayWheel->GetCurrentData().toInt()));
    m_DateTime.setTime(QTime(mp_HourWheel->GetCurrentData().toInt(), mp_MinWheel->GetCurrentData().toInt()));
    if (!m_DateTime.isValid())
    {
        MainMenu::CMessageDlg* pMessageDlg = new MainMenu::CMessageDlg(this);
        pMessageDlg->SetTitle(QApplication::translate("MainMenu::CDateTime", "Information",
                                                     Q_NULLPTR, -1));
        pMessageDlg->SetIcon(QMessageBox::Information);
        pMessageDlg->SetButtonText(1, QApplication::translate("MainMenu::CDateTime", "Ok",
                                                               Q_NULLPTR, -1));
        pMessageDlg->HideButtons();
        pMessageDlg->SetText(QApplication::translate("MainMenu::CDateTime", "The selected date is invalid. Please select another one.",
                                                         Q_NULLPTR, -1));
        if (mp_MainWindow)
        {
            QRect scr = mp_MainWindow->geometry();
            this->move( scr.center() - this->rect().center());
        }
        (void) pMessageDlg->exec();
        delete pMessageDlg;
        return;
    }

    if (Send == true) {
        emit ShowWaitDialog(QApplication::translate("MainMenu::CDateTime", "Saving settings ...",
                                                 Q_NULLPTR, -1));
        emit ApplyData(m_DateTime);
    }
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CDateTime::RetranslateUI()
{
    mp_Ui->scrollPanelDate->SetTitle(QApplication::translate("MainMenu::CDateTime", "Date", Q_NULLPTR, -1));
    mp_Ui->scrollPanelDate->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Day", Q_NULLPTR, -1),0);
    mp_Ui->scrollPanelDate->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Month", Q_NULLPTR, -1),1);
    mp_Ui->scrollPanelDate->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Year", Q_NULLPTR, -1),2);

    mp_Ui->scrollPanelTime->SetTitle(QApplication::translate("MainMenu::CDateTime", "Time", Q_NULLPTR, -1));
    mp_Ui->scrollPanelTime->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Hour", Q_NULLPTR, -1),0);
    mp_Ui->scrollPanelTime->SetSubtitle(QApplication::translate("MainMenu::CDateTime", "Minute", Q_NULLPTR, -1),1);
}

/****************************************************************************/
/*!
 *  \brief This slot is called when Process state changes
 */
/****************************************************************************/
void CDateTime::OnProcessStateChanged()
{
    ResetButtons();
}

/****************************************************************************/
/*!
 *  \brief Enables/Disables the apply button based on the user role/process
 *         running status
 */
/****************************************************************************/
void CDateTime::ResetButtons()
{
    m_ProcessRunning = MainMenu::CMainWindow::GetProcessRunningStatus();
    if (!m_ProcessRunning) {
        //if (Core::CGlobalHelper::CheckIfCanEdit() == false) {
        //    mp_Ui->pushButton->setEnabled(false);
        //}
       // else {
            //Edit Mode
            mp_Ui->pushButton->setEnabled(true);
       // }
    }
    else {
        mp_Ui->pushButton->setEnabled(false);
    }
}

/****************************************************************************/
/*!
 *  \brief Used to set pointer to mainwindow, used to retreive user role and
 *         process state changed.
 *
 *  \iparam p_MainWindow = MainWindow pointer.
 */
/****************************************************************************/
void CDateTime::SetPtrToMainWindow(MainMenu::CMainWindow *p_MainWindow)
{
    mp_MainWindow = p_MainWindow;
    CONNECTSIGNALSLOTGUI(mp_MainWindow, ProcessStateChanged(), this, OnProcessStateChanged());
}

} // end namespace MainMenu

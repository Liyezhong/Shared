/****************************************************************************/
/*! \file MainWindow.cpp
 *
 *  \brief MainWindow implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-05-17
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

#include "MainMenu/Include/MainWindow.h"
#include "ui_MainWindow.h"
#include "Global/Include/AdjustedTime.h"
#include "Users/Include/UserPrivilegeWidget.h"
#include "Application/Include/LeicaStyle.h"
#include <QDate>
#include <QDebug>

namespace MainMenu {

CMainWindow::UserRole_t CMainWindow::m_CurrentUserRole = CMainWindow::Operator;
bool CMainWindow::m_ProcessRunning = false;
bool StyleSize = dynamic_cast<Application::CLeicaStyle *>(qApp->style())->GetStyleSize();

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CMainWindow::CMainWindow(QWidget *p_Parent) :
    QMainWindow(p_Parent),
    mp_Ui(new Ui::CMainWindow),
    mTimeRefreshTimer(this)
{
    mp_Ui->setupUi(this);
    m_StatusLabel1Free = true;
    m_StatusLabel2Free = true;
    m_StatusLabel3Free = true;
    m_ProcessRunning = false;
    m_RemoteService = false;
    m_Error = false;
    m_Warning = false;
    m_WarningErrorFlag = false;
    m_CurrentDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
    // init time refresh timer
    if(!QObject::connect(&mTimeRefreshTimer, SIGNAL(timeout()), this, SLOT(RefreshDateTime()))) {
        /// \todo error. throw exception?
        qDebug() << "Can not connect mTimeRefreshTimer::timeout() with this::RefreshDateTime()";
    }
    mTimeRefreshTimer.setSingleShot(false);
    mTimeRefreshTimer.setInterval(500); /// \todo get some reasonable interval (0.5 - 3 seconds)
    mTimeRefreshTimer.start();

    QPalette Palette = mp_Ui->caption->palette();
    Palette.setBrush(QPalette::Window, QBrush(QPixmap(QString(":/%1/StatusArea.png").arg(Application::CLeicaStyle::GetStyleSizeString()))));
    Palette.setColor(QPalette::WindowText, Qt::white);
    mp_Ui->caption->setPalette(Palette);
    QObject::connect(mp_Ui->TabWidget,SIGNAL(currentChanged(int)),this,SLOT(OnCurrentTabChanged(int)));
    mp_Ui->TabWidget->clear();
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CMainWindow::~CMainWindow()
{
    try {
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

void CMainWindow::changeEvent(QEvent *p_Event)
{
    QMainWindow::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            emit onChangeEvent();
            break;
        default:
            break;
    }
}
/****************************************************************************/
/*!
 *  \brief Adds a menu widget to a tab widget
 *
 *  \iparam p_MenuGroup = Menu group
 *  \iparam Label = Label in tab bar
 *
 *  \return The index in the tab bar
 */
/****************************************************************************/
void CMainWindow::AddMenuGroup(QWidget *p_MenuGroup, QString Label)
{
    qint32 Index = mp_Ui->TabWidget->addTab(p_MenuGroup, Label);
    if (Index < 0) {
        qDebug() << "CMainWindow: tab index is out of range";
    }
}

/****************************************************************************/
/*!
 *  \brief Adds a menu widget to a tab widget and an icon to the tab bar
 *
 *  \iparam p_MenuGroup = Menu group
 *  \iparam Pixmap = Icon in tab bar
 *
 *  \return The index in the tab bar
 */
/****************************************************************************/
void CMainWindow::AddMenuGroup(QWidget *p_MenuGroup, QPixmap Pixmap)
{
    QTransform Transform;
    Transform.rotate(90.0);
    QString m_SepiaTabs = "Small";

    qint32 Index = mp_Ui->TabWidget->addTab(p_MenuGroup, QIcon(Pixmap.transformed(Transform)), "");
    if(Application::CLeicaStyle::GetStyleSizeString()== m_SepiaTabs){
        mp_Ui->TabWidget->setIconSize(QSize(180,250));
    }
    if (Index < 0) {
        qDebug() << "CMainWindow: tab index is out of range";
    }
}

/****************************************************************************/
/*!
 *  \brief Returns the item currently active in the tab widget
 *
 *  \return The active widget
 */
/****************************************************************************/
QWidget *CMainWindow::GetCurrentGroup()
{
    return (mp_Ui->TabWidget->currentWidget());
}

/****************************************************************************/
/*!
 *  \brief Sets the date and the time in the title bar
 */
/****************************************************************************/
void CMainWindow::RefreshDateTime()
{
    // get adjusted time
    m_CurrentDateTime = Global::AdjustedTime::Instance().GetCurrentDateTime();
    if (m_DateFormat == Global::DATE_INTERNATIONAL) {
        mp_Ui->labelDate->setText(m_CurrentDateTime.date().toString("dd.MM.yyyy"));
    }
    else if (m_DateFormat == Global::DATE_ISO) {
        mp_Ui->labelDate->setText(m_CurrentDateTime.date().toString(Qt::ISODate));
    }
    else {
        mp_Ui->labelDate->setText(m_CurrentDateTime.date().toString("MM/dd/yyyy"));
    }

    if (m_TimeFormat == Global::TIME_24) {
        mp_Ui->labelTime->setText(m_CurrentDateTime.time().toString("hh:mm"));
    }
    else {
        mp_Ui->labelTime->setText(m_CurrentDateTime.time().toString("hh:mm a"));
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the orientation of the tabs
 *
 *  \iparam Vertical = Vertical orientation or not
 */
/****************************************************************************/
void CMainWindow::SetVerticalOrientation(bool Vertical)
{
    if (Vertical == true) {
        mp_Ui->TabWidget->setTabPosition(QTabWidget::West);
    }
    else {
        mp_Ui->TabWidget->setTabPosition(QTabWidget::North);
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the small user role icon in the status bar
 *
 *  \iparam UserRole = the intended user role
 */
/****************************************************************************/
void CMainWindow::SetUserIcon(MainMenu::CMainWindow::UserRole_t UserRole)
{
    switch (UserRole) {
        case Admin:
            mp_Ui->labelUser->setPixmap(QPixmap(QString(":/%1/Icons/User_Status/User_Admin.png").arg(Application::CLeicaStyle::GetStyleSizeString())));
            break;
        case Operator:
            mp_Ui->labelUser->setPixmap(QPixmap(QString(":/%1/Icons/User_Status/User_Operator.png").arg(Application::CLeicaStyle::GetStyleSizeString())));
            break;
        case Service:
            mp_Ui->labelUser->setPixmap(QPixmap(QString(":/%1/Icons/User_Status/User_Service.png").arg(Application::CLeicaStyle::GetStyleSizeString())));
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the status icons in the status bar
 *
 *  \iparam Status
 */
/****************************************************************************/
bool CMainWindow::SetStatusIcons(Status_t Status)
{
    bool result =false;
    m_Status = Status;
    QLabel *p_Label = 0;
    qint32 Val = static_cast<qint32>(m_Status & (Error | Warning));
    if (m_WarningErrorFlag && Val) {
        goto Else;
    }
    if (m_StatusLabel1Free) {
        p_Label = mp_Ui->statusLabel1;
         m_StatusLabel1Free = false;
        //Back up label pointer. Used when all the labels are occupied.
        //warning and error share label.
        if(Val) {
            mp_Label = p_Label;
            m_WarningErrorFlag = true;
        }
    }
    else if (m_StatusLabel2Free) {
        p_Label = mp_Ui->statusLabel2;
        m_StatusLabel2Free = false;
        if(Val) {
            mp_Label = p_Label;
            m_WarningErrorFlag = true;
        }
    }
    else if (m_StatusLabel3Free) {
        p_Label = mp_Ui->statusLabel3;
        m_StatusLabel3Free = false;
        if(Val) {
            mp_Label = p_Label;
            m_WarningErrorFlag = true;
        }

    }
    else {
        Else:p_Label = mp_Label;
    }

    switch (m_Status) {
        case ProcessRunning:
            if (!m_ProcessRunning) {
                p_Label->setPixmap(QPixmap(QString(":/%1/Icons/Status_Bar/Status_small.png").arg(Application::CLeicaStyle::GetStyleSizeString())));
                p_Label->show();
                mp_ProcessRunningLabel = p_Label;
                m_ProcessRunning = true;
                //Inform widgets process is running.
                emit ProcessStateChanged();
                result = true;
            }
            break;
        case RemoteCare:
            if (!m_RemoteService) {
                p_Label->setPixmap(QPixmap(QString(":/%1/Icons/Status_Bar/RemoteCare_small.png").arg(Application::CLeicaStyle::GetStyleSizeString())));
                p_Label->show();
                mp_RemoteLabel = p_Label;
                m_RemoteService = true;
                result = true;
            }
            break;
        case Error:
            if (!m_Error) {
                p_Label->setPixmap(QPixmap(QString(":/%1/Icons/Status_Bar/Error_small.png").arg(Application::CLeicaStyle::GetStyleSizeString())));
                p_Label->show();
                mp_ErrorLabel = p_Label;
                m_Error= true;
                m_Warning = false;
                result = true;
            }
            break;
        case Warning:
            if (!m_Warning) {
                p_Label->setPixmap(QPixmap(QString(":/%1/Icons/Status_Bar/Warning_small.png").arg(Application::CLeicaStyle::GetStyleSizeString())));
                p_Label->show();
                mp_WarningLabel = p_Label;
                m_Warning = true;
                m_Error = false;
                result = true;
            }
            break;
        default:
            result = false;
            break;
    }
    return result;
}

/****************************************************************************/
/*!
 *  \brief Unsets the status icons in the status bar
 *
 *  \iparam Status
 */
/****************************************************************************/
bool CMainWindow::UnsetStatusIcons(Status_t Status)
{
    bool result = true;
    switch (Status) {
        case ProcessRunning:
            if (m_ProcessRunning) {
                if (mp_ProcessRunningLabel == mp_Ui->statusLabel1) {
                    mp_Ui->statusLabel1->hide();
                    m_ProcessRunning = false;
                    m_StatusLabel1Free = true;
                }
                else if (mp_ProcessRunningLabel == mp_Ui->statusLabel2) {
                    mp_Ui->statusLabel2->hide();
                    m_ProcessRunning = false;
                    m_StatusLabel2Free = true;
                }
                else {
                    mp_Ui->statusLabel3->hide();
                    m_ProcessRunning = false;
                    m_StatusLabel3Free = true;
                }
                //Inform widgets that process is not running anymore
                emit ProcessStateChanged();
                result = true;
            }
        break;
        case RemoteCare:
            if (m_RemoteService) {
                if (mp_RemoteLabel == mp_Ui->statusLabel1) {
                    mp_Ui->statusLabel1->hide();
                    m_RemoteService = false;
                    m_StatusLabel1Free = true;
                }
                else if (mp_RemoteLabel == mp_Ui->statusLabel2) {
                    mp_Ui->statusLabel2->hide();
                    m_RemoteService= false;
                    m_StatusLabel2Free = true;
                }
                else {
                    mp_Ui->statusLabel3->hide();
                    m_RemoteService = false;
                    m_StatusLabel3Free = true;
                }
                result = true;
            }
        break;
        case Warning:
            if (m_Warning) {
                if (mp_WarningLabel == mp_Ui->statusLabel1) {
                    mp_Ui->statusLabel1->hide();
                    m_Warning = false;
                    m_StatusLabel1Free = true;
                }
                else if (mp_WarningLabel == mp_Ui->statusLabel2) {
                    mp_Ui->statusLabel2->hide();
                    m_Warning = false;
                    m_StatusLabel2Free = true;
                }
                else {
                    mp_Ui->statusLabel3->hide();
                    m_Warning = false;
                    m_StatusLabel3Free = true;
                }
                m_WarningErrorFlag = false;
                result = true;
            }
        break;
        case Error:
            if (m_Error) {
                if (mp_ErrorLabel == mp_Ui->statusLabel1) {
                    mp_Ui->statusLabel1->hide();
                    m_Error = false;
                    m_StatusLabel1Free = true;
                }
                else if (mp_ErrorLabel == mp_Ui->statusLabel2) {
                    mp_Ui->statusLabel2->hide();
                    m_Error = false;
                    m_StatusLabel2Free = true;
                }
                else {
                    mp_Ui->statusLabel3->hide();
                    m_Error = false;
                    m_StatusLabel3Free = true;
                }
                m_WarningErrorFlag = false;
                result = true;
            }
        break;
        default:
            result = false;
        break;
    }
    return result;
}

/****************************************************************************/
/*!
 *  \brief  Shows the Status screen
 */
/****************************************************************************/
void CMainWindow::SetTabWidgetIndex()
{
    mp_Ui->TabWidget->setCurrentIndex(0);
}

/****************************************************************************/
/*!
 *  \brief This Slot is called when the Tab is changed.
 */
/****************************************************************************/
void CMainWindow::OnCurrentTabChanged(int CurrentIndex)
{
    emit CurrentTabChanged(CurrentIndex);
}

/****************************************************************************/
/*!
 *  \brief  Enables or disables the tabs
 */
/****************************************************************************/
void CMainWindow::SetTabEnabled(bool Status)
{
    mp_Ui->TabWidget->setTabEnabled(0, Status);
    mp_Ui->TabWidget->setTabEnabled(2, Status);
    mp_Ui->TabWidget->setTabEnabled(3, Status);
    mp_Ui->TabWidget->setTabEnabled(4, Status);
    mp_Ui->TabWidget->setTabEnabled(5, Status);

}

} // end namespace MainMenu

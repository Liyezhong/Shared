/****************************************************************************/
/*! \file MainWindow.cpp
 *
 *  \brief Implementation of file for class CMainWindow.
 *
 *  \b Description:
 *          This class implements a base widget which displays the application
 *      main window with different menu tabs.
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
//#include "Users/Include/UserPrivilegeWidget.h"
#include "Application/Include/LeicaStyle.h"
#include <QDate>
#include <QDebug>

namespace MainMenu {

CMainWindow::UserRole_t CMainWindow::m_CurrentUserRole = CMainWindow::Operator; //!< Operator type
bool CMainWindow::m_ProcessRunning = false;      //!< Process running state

//bool StyleSize = dynamic_cast<Application::CLeicaStyle *>(qApp->style())->GetStyleSize();   //!< Application type Colorado/Sepia

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CMainWindow::CMainWindow(QWidget *p_Parent) :QMainWindow(p_Parent), mp_Ui(new Ui::CMainWindow),
    mTimeRefreshTimer(this),
    mp_ProcPixmap(NULL),
    mp_RemotePixMap(NULL)
{
    mp_Ui->setupUi(this);
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
    mTimeRefreshTimer.setInterval(3000); /// \todo get some reasonable interval (0.5 - 3 seconds)
    mTimeRefreshTimer.start();

    QPalette Palette = mp_Ui->caption->palette();
    Palette.setBrush(QPalette::Window, QBrush(QPixmap(QString(":/%1/StatusBar/StatusBar_Background.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()))));
    Palette.setColor(QPalette::WindowText, Qt::white);
    mp_Ui->caption->setPalette(Palette);
    (void) QObject::connect(mp_Ui->TabWidget, SIGNAL(currentChanged(int)), this, SLOT(OnCurrentTabChanged(int)));

    mp_Ui->TabWidget->clear();
    installEventFilter(this);
    mp_Ui->statusLabelErr->installEventFilter(this);
    mp_Ui->statusLabelWarn->installEventFilter(this);
    mp_Ui->statusLabelWarn->setEnabled(false);
    mp_Ui->statusLabelErr->setEnabled(false);
    (void) UnsetStatusIcons(RemoteCare);
    if (Application::CLeicaStyle::GetCurrentDeviceType() == Application::DEVICE_SEPIA) {
        QFont Font("Sans");
        Font.setPixelSize(20);
        mp_Ui->labelTime->setFont(Font);
        mp_Ui->labelDate->setFont(Font);
    }
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
        delete mp_ProcPixmap;
        delete mp_RemotePixMap;
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
            /// Don't call the mp_Ui->retranslateui function
            /// because there are no translations are required
            /// in main window. If any translation required then
            /// use below function
            RetranslateUI();
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
    (void) Transform.rotate(90.0);
    QString m_SepiaTabs = "Small";

    qint32 Index = mp_Ui->TabWidget->addTab(p_MenuGroup, QIcon(Pixmap.transformed(Transform)), "");
    if (Application::CLeicaStyle::GetDeviceImagesPath() == m_SepiaTabs) {
       // mp_Ui->TabWidget->setIconSize(QSize(180,250));
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
            mp_Ui->labelUser->setPixmap(QPixmap(QString(":/%1/Icons/User_Status/User_Admin.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
            break;
        case Operator:
            mp_Ui->labelUser->setPixmap(QPixmap(QString(":/%1/Icons/User_Status/User_Operator.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
            break;
        case Service:
            mp_Ui->labelUser->setPixmap(QPixmap(QString(":/%1/Icons/User_Status/User_Service.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
            break;
    }    
}


/****************************************************************************/
/*!
 *  \brief Sets the small user mode icon in the status bar
 *
 *  \iparam Mode = the intended user mode (Service/Manuacturing)
 */
/****************************************************************************/
void CMainWindow::SetUserMode(QString Mode)
{
    if (Mode == QString("SERVICE")) {
        SetSaMUserMode("Service");
        mp_Ui->labelUser->setPixmap(QPixmap(QString(":/%1/Icons/User_Status/User_Service.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
    }
    else if (Mode == QString("MANUFACTURING")) {
        SetSaMUserMode("Manufacturing");
        mp_Ui->labelUser->setPixmap(QPixmap(QString(":/%1/Icons/User_Status/User_Manufaturing.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
    }
}

/****************************************************************************/
/*!
 *  \brief This function clears the TabWidget
 */
/****************************************************************************/
void CMainWindow::Clear()
{
    mp_Ui->TabWidget->clear();
}

/****************************************************************************/
/*!
 *  \brief Sets the status icons in the status bar
 *
 *  \iparam Status = Status of the
 *
 *  \return True if icon is suceessfully set else False.
 */
/****************************************************************************/
bool CMainWindow::SetStatusIcons(Status_t Status)
{
    bool result =false;
    m_Status = Status;
    QLabel *p_Label = 0;
    switch (m_Status) {
    case ProcessRunning:
        if (!m_ProcessRunning) {
            if (m_RemoteService) {
                p_Label = mp_Ui->statusLabel2;
                p_Label->setPixmap(*mp_RemotePixMap);
                p_Label->show();
            }
            p_Label = mp_Ui->statusLabel1;
            delete mp_ProcPixmap;
            mp_ProcPixmap = new QPixmap(QString(":/%1/Icons/Status_Bar/Status_small.png").arg(Application::CLeicaStyle::GetDeviceImagesPath()));
            p_Label->setPixmap(*mp_ProcPixmap);
            p_Label->show();
            m_ProcessRunning = true;
            //Inform widgets process is running.
            emit ProcessStateChanged();
            result = true;
        }

        break;
    case RemoteCare:
        if (!m_RemoteService) {
            p_Label = mp_Ui->statusLabel2;
            delete mp_RemotePixMap;
            mp_RemotePixMap = new QPixmap(QString(":/%1/Icons/Status_Bar/RemoteCare_small.png").
                                          arg(Application::CLeicaStyle::GetDeviceImagesPath()));
            p_Label->setPixmap(*mp_RemotePixMap);
            p_Label->show();
            m_RemoteService = true;
            result = true;
        }
        break;
    case Error:
        p_Label = mp_Ui->statusLabelErr;
        p_Label->setEnabled(true);
        p_Label->setPixmap(QPixmap(QString(":/%1/Icons/Status_Bar/Error_small.png").
                                   arg(Application::CLeicaStyle::GetDeviceImagesPath())));
        p_Label->show();
        m_Error= true;
        result = true;
        break;
    case Warning:
        p_Label = mp_Ui->statusLabelWarn;
        p_Label->setEnabled(true);
        p_Label->setPixmap(QPixmap(QString(":/%1/Icons/Status_Bar/Warning_small.png").
                                   arg(Application::CLeicaStyle::GetDeviceImagesPath())));
        p_Label->show();
        m_Warning = true;
        result = true;
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
 *
 *  \return True if icon is unset successfully else False.
 */
/****************************************************************************/
bool CMainWindow::UnsetStatusIcons(Status_t Status)
{
    bool result = true;
    switch (Status) {
    case ProcessRunning:
        if (m_ProcessRunning) {
            mp_Ui->statusLabel1->hide();
            if(mp_ProcPixmap) {
                mp_ProcPixmap->fill(Qt::transparent);
                mp_Ui->statusLabel1->setPixmap(*mp_ProcPixmap);
                // check whether remote service is running, if it is then set the first label to remote care
                if (m_RemoteService) {
                    if(mp_RemotePixMap) {
                        mp_RemotePixMap->fill(Qt::transparent);
                        mp_Ui->statusLabel2->setPixmap(*mp_RemotePixMap);
                        mp_Ui->statusLabel2->show();
                        delete mp_RemotePixMap;
                        mp_RemotePixMap = new QPixmap(QString(":/%1/Icons/Status_Bar/RemoteCare_small.png").
                                                      arg(Application::CLeicaStyle::GetDeviceImagesPath()));
                        mp_Ui->statusLabel1->setPixmap(*mp_RemotePixMap);
                    }
                }

                mp_Ui->statusLabel1->show();
            }
            //todo:after process icon can be loaded, move the following 3 lines into the above block {}
            m_ProcessRunning = false;
            emit ProcessStateChanged();
            result = true;
        }
        break;
    case RemoteCare:
        if (m_RemoteService) {
            //mp_Ui->statusLabel2->hide();
            if(mp_RemotePixMap) {
                mp_RemotePixMap->fill(Qt::transparent);
                mp_Ui->statusLabel2->setPixmap(*mp_RemotePixMap);
                if (!m_ProcessRunning) {
                    mp_Ui->statusLabel1->setPixmap(*mp_RemotePixMap);
                    mp_Ui->statusLabel1->show();
                }
                mp_Ui->statusLabel2->show();
                m_RemoteService= false;
                result = true;
            }
        }
        break;
    case Warning:
        mp_Ui->statusLabelWarn->setPixmap(QPixmap());
        mp_Ui->statusLabelWarn->setEnabled(false);
        result = true;
        break;
    case Error:
        mp_Ui->statusLabelErr->setPixmap(QPixmap());
        mp_Ui->statusLabelErr->setEnabled(false);
        result = true;
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
 *  \brief  Shows the Status screen
 */
/****************************************************************************/
void CMainWindow::SetTabWidgetIndex(int Index)
{
    mp_Ui->TabWidget->setCurrentIndex(Index);
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
 *  \brief  Enables or disables the tabs.
 *
 *  \iparam Status = True if Tab is enabled else False.
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

void CMainWindow::SetTabEnabled(int Index, bool Status)
{
     mp_Ui->TabWidget->setTabEnabled(Index, Status);
}

/****************************************************************************/
/*!
 *  \brief  Method will be called,when an event triggered on the object
 *
 *  \iparam Obj = object of the widget
 *  \iparam p_Event = Event object
 *
 *  \return true when event is triggered
 */
/****************************************************************************/
bool CMainWindow::eventFilter(QObject *Obj, QEvent *p_Event)
{    
    if (p_Event->type() == QEvent::MouseButtonPress ||            
            p_Event->type() == QEvent::MouseButtonDblClick ||
            p_Event->type() == QEvent::MouseMove) {

        if (Obj == static_cast<QObject*>(mp_Ui->statusLabelErr)) {
            qDebug() << "mp_Ui->statusLabelErr->isHidden()" << mp_Ui->statusLabelErr->isHidden()<<mp_Ui->statusLabelErr->isVisible();
            if(mp_Ui->statusLabelErr->isEnabled()) {
                emit ShowErrorMsgDlg();
                qDebug()<< "Clicked on Error status label";
            }
            return true;
        }
        else if(Obj == static_cast<QObject*>(mp_Ui->statusLabelWarn)) {
            qDebug() <<"mp_Ui->statusLabelWarn->isHidden()" << mp_Ui->statusLabelWarn->isHidden()<<mp_Ui->statusLabelWarn->isVisible();
            if (mp_Ui->statusLabelWarn->isEnabled()) {
                emit ShowWarningMsgDlg();
                qDebug()<< "Clicked on Warning status label";
            }
            return true;
        }
    }
    ResetWindowStatusTimer();
    return QMainWindow::eventFilter(Obj, p_Event);    
}

/****************************************************************************/
/*!
 *  \brief Slot called to reset window status timer
 */
/****************************************************************************/
void CMainWindow::ResetWindowStatusTimer()
{
    emit OnWindowActivated();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CMainWindow::RetranslateUI()
{
}

} // end namespace MainMenu

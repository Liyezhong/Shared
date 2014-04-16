/****************************************************************************/
/*! \file TestMainMenu.cpp
 *
 *  \brief Unit test for MainMenu.
 *
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-04-16
 *  $Author:    $ C.Adaragunchi
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

#include <QtTest>
#include <QDebug>
#include <MainMenu/Include/BaseTable.h>
#include <MainMenu/Include/ContentScroller.h>
#include <MainMenu/Include/DataManagement.h>
#include <MainMenu/Include/DateTime.h>
#include <MainMenu/Include/DialogFrame.h>
#include <MainMenu/Include/ErrorMsgDlg.h>
#include <MainMenu/Include/FileView.h>
#include <MainMenu/Include/MainWindow.h>
#include <MainMenu/Include/MenuGroup.h>
#include <MainMenu/Include/MessageDlg.h>
#include <MainMenu/Include/MsgBoxManager.h>
#include <MainMenu/Include/PanelFrame.h>
#include <MainMenu/Include/ScrollTable.h>
#include <MainMenu/Include/ScrollWheel.h>
#include <MainMenu/Include/SemiTransparentOverlay.h>
#include <MainMenu/Include/SliderControl.h>
#include <MainMenu/Include/StatusBarManager.h>
#include <MainMenu/Include/TableMoving.h>
#include <MainMenu/Include/TextDialog.h>
#include <MainMenu/Include/TextPanel.h>
#include <MainMenu/Include/WaitDialog.h>
#include <MainMenu/Include/WaitIndicator.h>
#include <MainMenu/Include/WarningMsgDlg.h>
#include <MainMenu/Include/WheelPanel.h>
#include <kineticscroller/include/QtScroller>
#include <QObject>
#include <QWidget>

namespace MainMenu {

/****************************************************************************/
/**
 * \brief Test class for MainMenu class.
 */
/****************************************************************************/
class CTestMainMenu : public QObject {
    Q_OBJECT
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();

    /****************************************************************************/
    /**
     * \brief Test MainMenu components
     */
    /****************************************************************************/
    void utTestMainMenu();

}; // end class TestPrograms

/****************************************************************************/
void CTestMainMenu::initTestCase() {
}

/****************************************************************************/
void CTestMainMenu::init() {
}

/****************************************************************************/
void CTestMainMenu::cleanup() {
}

/****************************************************************************/
void CTestMainMenu::cleanupTestCase() {
}

/****************************************************************************/
void CTestMainMenu::utTestMainMenu()
{
    QWidget *p_ParentWidget = new QWidget();

    MainMenu::CBaseTable *p_BaseTable = new MainMenu::CBaseTable(p_ParentWidget);
    MainMenu::CScrollTable *p_ScrollTable = new MainMenu::CScrollTable(p_ParentWidget);
    MainMenu::CContentScroller *p_ContentScroller = new MainMenu::CContentScroller(p_ParentWidget);
    MainMenu::CDataManagement *p_DataManagement = new MainMenu::CDataManagement(p_ParentWidget);
    MainMenu::CDateTime *p_DateTime = new MainMenu::CDateTime(p_ParentWidget);
    MainMenu::CDialogFrame *p_DialogFrame = new MainMenu::CDialogFrame(p_ParentWidget);
    MainMenu::CErrorMsgDlg *p_ErrorMessageDlg = new MainMenu::CErrorMsgDlg(p_ParentWidget);
    MainMenu::CFileView *p_FileView = new MainMenu::CFileView(p_ParentWidget);
    MainMenu::CMainWindow *p_MainWindow = new MainMenu::CMainWindow(p_ParentWidget);
    MainMenu::CPanelFrame *p_PanelFrame = new MainMenu::CPanelFrame(p_ParentWidget);
    MainMenu::CSliderControl *p_SlideControl = new MainMenu::CSliderControl(p_ParentWidget);
    MainMenu::CTextDialog *p_TextDialog = new MainMenu::CTextDialog(p_ParentWidget);
    MainMenu::CTextPanel *p_TextPanel = new MainMenu::CTextPanel(p_ParentWidget);
    MainMenu::CWaitDialog *p_WaitDialog = new MainMenu::CWaitDialog(p_ParentWidget);

    p_BaseTable->SetVisibleRows(8);
    p_ScrollTable->ScrollContent(0);
    p_ScrollTable->ScrollRangeChanged();

    p_ContentScroller->SimpleArrows();
    p_ContentScroller->ScrollContent(1);
    p_ContentScroller->UpdateArrows(20);
    p_ContentScroller->ScrollRangeChanged();
    p_ContentScroller->Reset();
    p_ContentScroller->UpdateVerticalScrollBar();

    p_DataManagement->ExportDialog();
    p_DataManagement->ImportDialog();
    p_DataManagement->ServiceExportDialog();
    p_DataManagement->OnRemoteSWUpdate();
    p_DataManagement->OnUserRoleChanged();
    p_DataManagement->OnProcessStateChanged();
    p_DataManagement->OnSoftwareUpdate();
    p_DataManagement->ResetButtons();
    p_DataManagement->SetPtrToMainWindow(p_MainWindow);

    p_DateTime->OnProcessStateChanged();
    p_DateTime->ResetButtons();
    p_DateTime->SetPtrToMainWindow(p_MainWindow);

    p_DialogFrame->SetDialogTitle("Test MainMenu");

    QStringList ErrorStringList;
    ErrorStringList.append("Error1");
    ErrorStringList.append("Error2");
    ErrorStringList.append("Error3");
    p_ErrorMessageDlg->SetCaption("Error Message");
    p_ErrorMessageDlg->SetText(ErrorStringList);
    p_ErrorMessageDlg->SetErrorMsgList();

    p_FileView->AddEmptyRows();
    p_FileView->SetEventViewerPanelSize(40, 60);
    p_FileView->DisableButton();

    p_MainWindow->SetUserIcon(MainMenu::CMainWindow::Operator);
    p_MainWindow->SetUserIcon(MainMenu::CMainWindow::Admin);
    p_MainWindow->SetUserIcon(MainMenu::CMainWindow::Service);
    p_MainWindow->SetStatusIcons(MainMenu::CMainWindow::ProcessRunning);
    p_MainWindow->SetStatusIcons(MainMenu::CMainWindow::RemoteCare);
    p_MainWindow->SetStatusIcons(MainMenu::CMainWindow::Warning);
    p_MainWindow->SetStatusIcons(MainMenu::CMainWindow::Error);
    p_MainWindow->SetVerticalOrientation(true);
    p_MainWindow->SetTabWidgetIndex();
    p_MainWindow->OnCurrentTabChanged(2);

    p_PanelFrame->SetPanelTitle("Test MainMenu");

    p_SlideControl->SetDisabled(true);
    p_SlideControl->OnValueChanged(100);
    p_SlideControl->SetPosition(MainMenu::CSliderControl::PosLeft);
    p_SlideControl->SetPosition(MainMenu::CSliderControl::PosRight);

    p_TextDialog->SetCaption("Test MainMenu");
    p_TextDialog->SetText("Text Dialog");

    p_TextPanel->AddCaption("Text Panel");
    p_WaitDialog->SetText("Wait Dialog");
    p_WaitDialog->SetTimeout(20000);
    p_WaitDialog->HideAbort();

    MainMenu::CMsgBoxManager *p_MesgBoxManager = new MainMenu::CMsgBoxManager(NULL, NULL);
    QByteArray eventReportByteArray;
    QDataStream eventDataStream(&eventReportByteArray, QIODevice::ReadWrite);
    // Stream order
    // EventStatus, EventType, EventID, EventString,  Time, btn Type,StatusBar Icon
    eventDataStream << true << Global::EVTTYPE_ERROR << 123456
                    << "Test error message" << "02/08/2014" << Global::OK
           << false << "RT_LID_OPEN_CLOSE"
           << "25s";

    Global::RefManager<Global::tRefType>  refMgr;
    Global::tRefType ref = refMgr.GetNewRef();
    p_MesgBoxManager->Manage(eventDataStream, ref);

}

} // end namespace MainMenu


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTEST_DISABLE_KEYPAD_NAVIGATION
    MainMenu::CTestMainMenu tc;
    QTest::qExec(&tc, argc, argv);
    app.exec();
}


#include "TestMainMenu.moc"


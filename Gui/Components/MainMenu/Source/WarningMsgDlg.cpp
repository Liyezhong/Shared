/****************************************************************************/
/*! \file WarningMsgDlg.cpp
 *
 *  \brief WarningMsgDlg implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-10-29
 *   $Author:  $ Bhanu Prasad H
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
#include "MainMenu/Include/WarningMsgDlg.h"
#include "ui_WarningMsgDlg.h"

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 *  \iparam p_MainWindow = main window object
 */
/****************************************************************************/
CWarningMsgDlg::CWarningMsgDlg(QWidget *p_Parent, QWidget *p_MainWindow) : MainMenu::CDialogFrame(p_Parent),
    mp_Ui(new Ui::CWarningMsgDlg)
{
    mp_Ui->setupUi(GetContentFrame());
    mp_TextEdit = new QTextEdit(mp_Ui->widget);
    mp_TextEdit->setReadOnly(true);
    mp_TextEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    mp_Ui->widget->SetContent(mp_TextEdit);
    mp_MainWidow = static_cast<MainMenu::CMainWindow *>(p_MainWindow);
    QPalette Palette = mp_TextEdit->palette();
    Palette.setColor(QPalette::Window, Qt::white);
    mp_Ui->captionLabel->setText(tr("Warning Messages"));
    SetDialogTitle(tr("Warning Messages"));
    mp_TextEdit->setPalette(Palette);
    if (!connect(mp_Ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()))) {
        qDebug() << "CWarningMsgDlg: cannot connect 'clicked' signal";
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CWarningMsgDlg::~CWarningMsgDlg()
{
    try {
        delete mp_TextEdit;
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

void CWarningMsgDlg::changeEvent(QEvent *p_Event)
{
    QDialog::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            RetranslateUI();
            break;
        default:
            break;
    }
}


//****************************************************************************/
/*!
 *  \brief Sets the Warningmessages list to the Text Edit
 *  \param[in] EventIdList   Event Id's List
 *  \param[in] WarningMsgList  Warning message List
 *  \param[in] TimeStampList   Time stamp List
 */
/****************************************************************************/
void CWarningMsgDlg::SetWarningMsgList(QStringList EventIdList,QStringList EventMessage,QStringList TimeStamp)
{
    QStringList WarningText;
    for (int EventID = 0;EventID < EventIdList.count();EventID++) {
        WarningText.append(TimeStamp.at(EventID) + "\t" + EventMessage.at(EventID) + "\n");
    }
    qDebug() << "Warning messages" << WarningText;
    SetText(WarningText);
}

/****************************************************************************/
/*!
 *  \brief Sets a small caption in the top of the dialog
 *
 *  \iparam Caption = Caption above the text view
 */
/****************************************************************************/
void CWarningMsgDlg::SetCaption(QString Caption)
{
    mp_Ui->captionLabel->setText(tr("%1").arg(Caption));
}

/****************************************************************************/
/*!
 *  \brief Sets the content of the text view
 *
 *  \iparam Text = Text file
 */
/****************************************************************************/
void CWarningMsgDlg::SetText(QStringList MsgList)
{
    mp_TextEdit->clear();
    mp_TextEdit->setFontPointSize(11);
    for (int Msg = 0;Msg <MsgList.count();Msg++) {
        mp_TextEdit->append(tr("%1").arg(MsgList.at(Msg)));
    }
}

/****************************************************************************/
/*!
 *  \brief Popups Warning Message dialog
 */
/****************************************************************************/
void CWarningMsgDlg::PopUp()
{
    mp_MainWidow->SetTabWidgetIndex();
    resize(mp_MainWidow->width()-150,mp_MainWidow->height()-75);
    move(80, 35);
    show();
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CWarningMsgDlg::RetranslateUI()
{
   MainMenu::CDialogFrame::SetDialogTitle(QApplication::translate("MainMenu::CWarningMsgDlg", "Warning Messages", 0, QApplication::UnicodeUTF8));
}

} // end namespace MainMenu



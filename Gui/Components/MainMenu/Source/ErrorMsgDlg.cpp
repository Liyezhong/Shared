/****************************************************************************/
/*! \file ErrorMsgDlg.cpp
 *
 *  \brief ErrorMsgDlg implementation.
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
#include "MainMenu/Include/ErrorMsgDlg.h"
#include "ui_ErrorMsgDlg.h"

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CErrorMsgDlg::CErrorMsgDlg(QWidget *p_Parent, QWidget *p_MainWindow) : MainMenu::CDialogFrame(p_Parent),
    mp_Ui(new Ui::CErrorMsgDlg)
{
    mp_Ui->setupUi(GetContentFrame());

    mp_TextEdit = new QTextEdit(mp_Ui->widget);
    mp_TextEdit->setReadOnly(true);
    mp_TextEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    mp_Ui->widget->SetContent(mp_TextEdit);
    mp_MainWidow = static_cast<MainMenu::CMainWindow *>(p_MainWindow);
    QPalette Palette = mp_TextEdit->palette();
    Palette.setColor(QPalette::Window, Qt::white);
    mp_Ui->captionLabel->setText(tr("Error Messages"));
    SetDialogTitle(tr("Error Messages"));
    mp_TextEdit->setPalette(Palette);

    if (!connect(mp_Ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()))) {
        qDebug() << "CErrorMsgDlg: cannot connect 'clicked' signal";
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CErrorMsgDlg::~CErrorMsgDlg()
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

void CErrorMsgDlg::changeEvent(QEvent *p_Event)
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
 *  \brief Sets the Errormessages list to the Text Edit
 *  \param[in] EventIdList   Event Id's List
 *  \param[in] ErrorMsgList  Error message List
 *  \param[in] TimeStampList   Time stamp List
 */
/****************************************************************************/
void CErrorMsgDlg::SetErrorMsgList(QStringList EventIdList,QStringList EventMessage,QStringList TimeStamp)
{
    QStringList ErrorText;
    for (int EventID = 0;EventID < EventIdList.count();EventID++) {
        ErrorText.append(TimeStamp.at(EventID) + ";  "+EventIdList.at(EventID) +";  "+EventMessage.at(EventID)+"\n");
       // SetText(ErrorText);
    }
    qDebug() << "Error messages" << ErrorText;
    SetText(ErrorText);
}

/****************************************************************************/
/*!
 *  \brief Sets a small caption in the top of the dialog
 *
 *  \iparam Caption = Caption above the text view
 */
/****************************************************************************/
void CErrorMsgDlg::SetCaption(QString Caption)
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
void CErrorMsgDlg::SetText(QStringList MsgList)
{
    mp_TextEdit->clear();
    mp_TextEdit->setFontPointSize(11);
    for (int Msg = 0;Msg <MsgList.count();Msg++) {
        mp_TextEdit->append(tr("%1").arg(MsgList.at(Msg)));
    }
}

/****************************************************************************/
/*!
 *  \brief Popups Error Message dialog
 */
/****************************************************************************/
void CErrorMsgDlg::PopUp()
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
void CErrorMsgDlg::RetranslateUI()
{
   MainMenu::CDialogFrame::SetDialogTitle(QApplication::translate("MainMenu::CErrorMsgDlg", "Error Messages", 0, QApplication::UnicodeUTF8));
}

} // end namespace MainMenu



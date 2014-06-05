/****************************************************************************/
/*! \file MessageDlg.cpp
 *
 *  \brief Implementation of file for class CMessageDlg.
 *
 *  \b Description:
 *          This class implements a base widget for displaying the message
 *          dialogs.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-09-28
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "MainMenu/Include/MessageDlg.h"
#include "Application/Include/LeicaStyle.h"
#include "ui_MessageDlg.h"
#include <QPushButton>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CMessageDlg::CMessageDlg(QWidget *p_Parent) : CDialogFrame(p_Parent),
    mp_Ui(new Ui::CMessageDlg)
{
    mp_Ui->setupUi(GetContentFrame());
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    CONNECTSIGNALSLOTGUI(mp_Ui->Button1, clicked(), this, OnButtonRightClicked());
    CONNECTSIGNALSLOTGUI(mp_Ui->Button2, clicked(), this, OnButtonCenterClicked());
    CONNECTSIGNALSLOTGUI(mp_Ui->Button3, clicked(), this, OnButtonLeftClicked());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CMessageDlg::~CMessageDlg()
{
    delete mp_Ui;
}


/****************************************************************************/
/*!
 *  \brief Sets the title of the message box
 *
 *  \iparam Title = Title string
 */
/****************************************************************************/
void CMessageDlg::SetTitle(const QString &Title)
{
    SetDialogTitle(QString("%1").arg(Title));
}

/****************************************************************************/
/*!
 *  \brief Sets two titles, one at left corner and other at right corner
 *
 *  \iparam TitleLeft  = Title at left corner
 *  \iparam TitleRight = Title at right corner
 */
/****************************************************************************/
void CMessageDlg::SetTitle(const QString &TitleLeft, const QString &TitleRight)
{
    SetDialogTitle(QString("%1").arg(TitleLeft), QString("%1").arg(TitleRight));
}

/****************************************************************************/
/*!
 *  \brief Sets the main text of the message box
 *
 *  \iparam Text = Text string
 */
/****************************************************************************/
void CMessageDlg::SetText(const QString &Text)
{
    mp_Ui->labelMessage->setTextFormat(Qt::RichText);
    mp_Ui->labelMessage->setText(QString("%1").arg(Text));
}

/****************************************************************************/
/*!
 *  \brief Sets the icon of the message box
 *
 *  \iparam Icon = Icon type
 */
/****************************************************************************/
void CMessageDlg::SetIcon(QMessageBox::Icon Icon)
{
    switch (Icon) {
        case QMessageBox::Information:
            mp_Ui->labelIcon->setPixmap(QPixmap(QString(":/%1/LAS-MessageBox-Icons/Icons_50x50/155_MB_information.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
            break;
        case QMessageBox::Warning:
            mp_Ui->labelIcon->setPixmap(QPixmap(QString(":/%1/LAS-MessageBox-Icons/Icons_50x50/154_MB_warning.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
            break;
        case QMessageBox::Critical:
            mp_Ui->labelIcon->setPixmap(QPixmap(QString(":/%1/LAS-MessageBox-Icons/Icons_50x50/151_MB_error.png").arg(Application::CLeicaStyle::GetDeviceImagesPath())));
            break;
        default:
            mp_Ui->labelIcon->setPixmap(QPixmap());
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets the icon of the message box
 *
 *  \iparam ButtonNumber = 1- for right corner Button, 2 - Center Button
 *                         3 - Left Corner Button
 *  \iparam ButtonText = Text to be displayed on the button
 */
/****************************************************************************/
void CMessageDlg::SetButtonText(qint32 ButtonNumber, QString ButtonText)
{
    switch(ButtonNumber){
    case 1:
        mp_Ui->Button1->setText(QString("%1").arg(ButtonText));
        mp_Ui->Button1->show();
        break;
    case 2:
        mp_Ui->Button2->setText(QString("%1").arg(ButtonText));
        mp_Ui->Button2->show();
        break;
    case 3:
        mp_Ui->Button3->setText(QString("%1").arg(ButtonText));
        mp_Ui->Button3->show();
        break;
    }
}

void CMessageDlg::EnableButton(qint32 ButtonNumber, bool IsEnable)
{
    switch(ButtonNumber){
    case 1:
        mp_Ui->Button1->setEnabled(IsEnable);
        break;
    case 2:
        mp_Ui->Button2->setEnabled(IsEnable);
        break;
    case 3:
        mp_Ui->Button3->setEnabled(IsEnable);
        break;
    }
}

/****************************************************************************/
/*!
 *  \brief Function to hide center button
 */
/****************************************************************************/
void CMessageDlg::HideCenterButton()
{
    mp_Ui->Button2->hide();
}

/****************************************************************************/
/*!
 *  \brief Function to hide the center and left corner Button
 */
/****************************************************************************/
void CMessageDlg::HideButtons()
{
    mp_Ui->Button2->hide();
    mp_Ui->Button3->hide();
}
/****************************************************************************/
/*!
 *  \brief Function to hide the center and Right corner Button
 */
/****************************************************************************/
void CMessageDlg::HideButtonsOneAndTwo()
{
    mp_Ui->Button1->hide();
    mp_Ui->Button2->hide();
}

/****************************************************************************/
/*!
 *  \brief Function to hide aal the buttons
 */
/****************************************************************************/
void CMessageDlg::HideAllButtons()
{
    mp_Ui->Button1->hide();
    mp_Ui->Button2->hide();
    mp_Ui->Button3->hide();
}

/****************************************************************************/
/*!
 *  \brief Slot for the show() functions of the dialog
 */
/****************************************************************************/
void CMessageDlg::Show()
{
    show();
}

} // end namespace MainMenu

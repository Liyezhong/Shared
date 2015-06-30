/****************************************************************************/
/*! \file TextDialog.cpp
 *
 *  \brief Implementation of file for class CTextDialog.
 *
 *  \b Description:
 *          This class implements a base widget for displaying Text dialogs.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-22
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

#include "MainMenu/Include/TextDialog.h"
#include "ui_TextDialog.h"
#include <QDebug>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CTextDialog::CTextDialog(QWidget *p_Parent) : MainMenu::CDialogFrame(p_Parent), mp_Ui(new Ui::CTextDialog)
{
    mp_Ui->setupUi(GetContentFrame());

    mp_TextEdit = new QTextEdit(mp_Ui->widget);
    mp_TextEdit->setReadOnly(true);
    mp_TextEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    mp_Ui->widget->SetContent(mp_TextEdit);

    QPalette Palette = mp_TextEdit->palette();
    Palette.setColor(QPalette::Window, Qt::white);
    mp_TextEdit->setPalette(Palette);

    if (!connect(mp_Ui->closeButton, SIGNAL(clicked()), this, SLOT(CloseDailog()))) {
        qDebug() << "CTextDialog: cannot connect 'clicked' signal";
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CTextDialog::~CTextDialog()
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

void CTextDialog::changeEvent(QEvent *p_Event)
{
    QDialog::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            emit LanguageChanged();
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Sets a small caption in the top of the dialog
 *
 *  \iparam Caption = Caption above the text view
 */
/****************************************************************************/
void CTextDialog::SetCaption(QString Caption)
{
    mp_Ui->captionLabel->setText(QString("%1").arg(Caption));
}

/****************************************************************************/
/*!
 *  \brief Sets the content of the text view
 *
 *  \iparam Text = Text file
 */
/****************************************************************************/
void CTextDialog::SetText(QString Text)
{
    mp_TextEdit->setText(QString("%1").arg(Text));
}

/****************************************************************************/
/*!
 *  \brief Sets the content of the text view as a normal text
 *         It loads the text faster than the "setText" function
 *
 *  \iparam Text = Text file
 */
/****************************************************************************/
void CTextDialog::SetPlainText(QString Text)
{
    mp_TextEdit->setPlainText(QString("%1").arg(Text));
}

/****************************************************************************/
/*!
 *  \brief Append the text
 *
 *  \iparam Text = Text file
 */
/****************************************************************************/
void CTextDialog::AppendText(QString Text)
{
    mp_TextEdit->append(Text);
}

/****************************************************************************/
/*!
 *  \brief Close all the dialogs
 *
 */
/****************************************************************************/
void CTextDialog::CloseDailog()
{
    accept();
    /// delete all the data from the text edit
    /// otherwise if large file loaded into memory then
    /// this dialog will be hidden but the text data remains in memory
    /// so if the text made as empty then we can release memory
//    mp_TextEdit->clear();
    mp_TextEdit->setText("");
}

/****************************************************************************/
/*!
 *  \brief Sets ninty percent scroll flag status.
 *
 *  \iparam Status = True/False
 *
 */
/****************************************************************************/
void CTextDialog::SetNintyPercentScroll(bool Status)
{
    mp_Ui->widget->SetNintyPercentScroll(Status);
}

void CTextDialog::EnableTable(bool enable)
{
    mp_Ui->widget->setEnabled(enable);
}

} // end namespace MainMenu

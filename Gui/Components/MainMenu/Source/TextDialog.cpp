/****************************************************************************/
/*! \file TextDialog.cpp
 *
 *  \brief TextDialog implementation.
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

    if (!connect(mp_Ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()))) {
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
    mp_Ui->captionLabel->setText(tr("%1").arg(Caption));
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
    mp_TextEdit->setText(tr("%1").arg(Text));
}

} // end namespace MainMenu

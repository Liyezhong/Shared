/****************************************************************************/
/*! \file FileView.cpp
 *
 *  \brief FileView implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-29
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
#include "MainMenu/Include/FileView.h"
#include "ui_FileView.h"
#include <QDebug>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CFileView::CFileView(QWidget *p_Parent) : QWidget(p_Parent), mp_Ui(new Ui::CFileView)
{
    mp_Ui->setupUi(this);

    mp_TableWidget = new MainMenu::CBaseTable;
    //TODO: remove this later
    for (qint32 i = 0; i < 20; i++) {
        m_Model.appendRow(new QStandardItem("Test " + QString::number(i)));
    }
    mp_TableWidget->setModel(&m_Model);
    mp_TableWidget->selectRow(0);
    mp_Ui->widget->SetContent(mp_TableWidget);
    mp_TableWidget->horizontalHeader()->resizeSection(0, 100);
    mp_Dialog = new MainMenu::CTextDialog(this);

    if (!connect(mp_Ui->pushButton, SIGNAL(clicked()), this, SLOT(ExecDialog()))) {
        qDebug() << "CFileView: cannot connect 'clicked' signal";
    }
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CFileView::~CFileView()
{
    try {
        delete mp_Dialog;
        delete mp_TableWidget;
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

void CFileView::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
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
 *  \brief  Function to set Dialog caption
 *  \iparam Caption String
 */
/****************************************************************************/
void CFileView::SetCaption(QString Caption)
{
    mp_Dialog->SetCaption(Caption);
}

/****************************************************************************/
/*!
 *  \brief Shows the logfile text dialog
 */
/****************************************************************************/
void CFileView::ExecDialog()
{
    //TODO: remove this later
    QString Text;
    for (qint32 i = 0; i < 100; i++) {
        for (qint32 j = 0; j < i; j++) {
            Text.append(m_Model.itemFromIndex(mp_TableWidget->currentIndex())->text() + " ");
        }
        Text.append("\n");
    }
    mp_Dialog->SetText(tr("%1").arg(Text));
    mp_Dialog->SetDialogTitle(tr("%1").arg(m_Model.itemFromIndex(mp_TableWidget->currentIndex())->text()));
    mp_Dialog->resize(458, 458);
    mp_Dialog->show();
}

} // end namespace MainMenu

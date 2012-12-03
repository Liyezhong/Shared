/****************************************************************************/
/*! \file MenuGroup.cpp
 *
 *  \brief MenuGroup implementation.
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

#include "MainMenu/Include/MenuGroup.h"
#include "ui_MenuGroup.h"
#include <QPushButton>
#include <QDebug>
#include <iostream>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CMenuGroup::CMenuGroup(QWidget *p_Parent) : QWidget(p_Parent), mp_Ui(new Ui::CMenuGroup)
{
    mp_Ui->setupUi(this);
    mp_Ui->menuWidget->SetPanelTitle(tr("Menu"));

    m_ItemCount = 0;

    mp_ButtonGroup = new QButtonGroup();

    if (!connect(mp_ButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(ButtonPressed(int)))) {
        qDebug() << "CMenuGroup: cannot connect 'buttonClicked' signal";
    }

    mp_ButtonLayout = new QVBoxLayout();
    mp_ButtonLayout->addStretch(1);
    mp_Ui->menuWidget->SetContent(mp_ButtonLayout);

    mp_ContentStack = new QStackedWidget();
    mp_ContentLayout = new QVBoxLayout();
    mp_ContentLayout->addWidget(mp_ContentStack);
    mp_Ui->contentWidget->SetContent(mp_ContentLayout);
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CMenuGroup::~CMenuGroup()
{
    try {
        delete mp_ContentStack;
        delete mp_ContentLayout;

        delete mp_ButtonGroup;
        delete mp_ButtonLayout;

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

void CMenuGroup::changeEvent(QEvent *p_Event)
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
 *  \brief Adds a panel to a menu group
 *
 *  \iparam Title = Title of the panel
 *  \iparam p_Content = The content widget
 */
/****************************************************************************/
void CMenuGroup::AddPanel(QString Title, QWidget *p_Content)
{
    AddButton(Title);
    m_ItemCount = mp_ContentStack->addWidget(p_Content);
    m_ItemCount++;

    ButtonPressed(0);
}

/****************************************************************************/
/*!
 *  \brief Returns the currently selected panel
 *
 *  \return Current panel widget
 */
/****************************************************************************/
QWidget *CMenuGroup::GetCurrentPanel()
{
    return (mp_ContentStack->currentWidget());
}

/****************************************************************************/
/*!
 *  \brief Adds a button to a menu group
 *
 *  \iparam Text = Text of the Bbutton
 */
/****************************************************************************/
void CMenuGroup::AddButton(QString Text)
{
    //lint -esym(429, p_Button)
    QPushButton *p_Button = new QPushButton(tr("%1").arg(Text));
    p_Button->setFocusPolicy(Qt::NoFocus);
    p_Button->setCheckable(true);

    mp_ButtonGroup->addButton(p_Button, m_ItemCount);

    mp_ButtonLayout->setStretch(m_ItemCount, 0);
    mp_ButtonLayout->insertWidget(m_ItemCount, p_Button);
    mp_ButtonLayout->addStretch(1);
}

/****************************************************************************/
/*!
 *  \brief Slot called when a button is pressed
 *
 *  \iparam Number = Button number
 */
/****************************************************************************/
void CMenuGroup::ButtonPressed(int Number)
{
    mp_ButtonGroup->button(Number)->setChecked(true);

    mp_Ui->contentWidget->SetPanelTitle(tr("%1").arg(mp_ButtonGroup->button(Number)->text()));
    mp_ContentStack->setCurrentIndex(Number);
}

} // end namespace MainMenu

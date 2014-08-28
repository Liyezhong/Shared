/****************************************************************************/
/*! \file MenuGroup.cpp
 *
 *  \brief Implementation of file for class CMenuGroup.
 *
 *  \b Description:
 *          This class implements a base widget for the menu group widgets.
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
    m_ButtonNumber = 0;

    mp_ButtonGroup = new QButtonGroup();

    if (!connect(mp_ButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(ButtonPressed(int)))) {
        qDebug() << "CMenuGroup: cannot connect 'buttonClicked' signal";
    }

    if (!connect(mp_ButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(ShowWidget(int)))) {
        qDebug() << "CMenuGroup: cannot connect 'buttonClicked' signal";
    }

    mp_Widget = new QWidget();
    mp_Widget->setMinimumSize(438, 349);
    mp_Ui->horizontalLayout->addWidget(mp_Widget);

    mp_VBoxLayout = new QVBoxLayout();

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

        delete mp_VBoxLayout;
        delete mp_ButtonGroup;
        delete mp_ButtonLayout;

        delete mp_Widget;
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
            mp_Ui->menuWidget->SetPanelTitle(tr("Menu"));
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
 *  \brief Adds a panel to a menu group
 *
 *  \iparam Title = Title of the panel
 *  \iparam p_Content = The content widget
 */
/****************************************************************************/
void CMenuGroup::AddSettingsPanel(QString Title, QWidget *p_Content)
{
    AddButton(Title);

    mp_VBoxLayout->setContentsMargins(0, 0, 0, 0);
    mp_VBoxLayout->addWidget(p_Content);
    mp_Widget->setLayout(mp_VBoxLayout);
    mp_Ui->contentWidget->setVisible(false);

    m_ItemCount++;
    m_CurrentTabIndex = 5;
    int indexLastButton = mp_ButtonGroup->buttons().count()-1;
    mp_ButtonGroup->button(indexLastButton)->setChecked(true);
    ShowWidget(indexLastButton);

   // ButtonPressed(indexLastButton);
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
 *  \brief This function deletes all widgets and buttons from button group
 */
/****************************************************************************/
void CMenuGroup::Clear()
{
    QList<QAbstractButton*> ButtonList = mp_ButtonGroup->buttons();
    foreach (QAbstractButton* pButton, ButtonList) {
        mp_ButtonGroup->removeButton(pButton);
        mp_ButtonLayout->removeWidget(pButton);
        delete pButton;
    }

    while(m_ItemCount) {
        mp_ContentStack->removeWidget(mp_ContentStack->widget(0));
        m_ItemCount--;
    }

    mp_VBoxLayout->removeWidget(mp_VBoxLayout->widget());
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
    QPushButton *p_Button = new QPushButton(QString("%1").arg(Text));
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

    mp_Ui->contentWidget->setVisible(true);
    mp_Widget->setVisible(false); 

    mp_Ui->contentWidget->SetPanelTitle(QString("%1").arg(mp_ButtonGroup->button(Number)->text()));
    mp_ContentStack->setCurrentIndex(Number);

    emit PanelChanged();
}

/****************************************************************************/
/*!
 *  \brief Slot called when a tab index is getting changed
 *
 *  \iparam TabIndex = Tab Index
 */
/****************************************************************************/
void CMenuGroup::SetCurrentTabIndex(int TabIndex)
{
    m_CurrentTabIndex = TabIndex;
}

/****************************************************************************/
/*!
 *  \brief Slot called when a button is clicked from ButtonGroup
 *
 *  \iparam ButtonClicked = Button Index
 */
/****************************************************************************/
void CMenuGroup::ShowWidget(int ButtonClicked)
{
    m_ButtonNumber =  ButtonClicked;
    if(m_CurrentTabIndex == 5 && m_ButtonNumber == (mp_ButtonGroup->buttons().count() - 1))
    {
        mp_Ui->contentWidget->setVisible(false);
        mp_Widget->setVisible(true);
    }
}

} // end namespace MainMenu

/****************************************************************************/
/*! \file TextPanel.cpp
 *
 *  \brief TextPanel implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2011-06-28
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

#include "MainMenu/Include/TextPanel.h"
#include <iostream>

namespace MainMenu {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CTextPanel::CTextPanel(QWidget *p_Parent) : CContentScroller(p_Parent)
{
    mp_Layout = new QGridLayout();
    mp_Layout->setRowStretch(0, 1);
    mp_Layout->setColumnStretch(0, 2);
    mp_Layout->setColumnStretch(1, 2);
    mp_Layout->setColumnStretch(2, 1);

    mp_Widget = new QWidget();
    mp_Widget->setLayout(mp_Layout);

    mp_ScrollArea = new QScrollArea();
    mp_ScrollArea->setWidgetResizable(true);
    mp_ScrollArea->setWidget(mp_Widget);

    QPalette Palette = palette();
    Palette.setColor(QPalette::Window, QColor(226, 227, 228));
    setPalette(Palette);

    SetContent(mp_ScrollArea);
    SimpleArrows();

    m_LineCounter = 0;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CTextPanel::~CTextPanel()
{
    try {
        QMap<QString, QLabel *>::const_iterator i;

        i = m_Description.constBegin();
        while (i != m_Description.constEnd()) {
            delete i.value();
            ++i;
        }

        i = m_Value.constBegin();
        while (i != m_Value.constEnd()) {
            delete i.value();
            ++i;
        }

        while (!m_Caption.isEmpty()) {
            delete m_Caption.takeFirst();
        }

        delete mp_Layout;
        delete mp_Widget;
        delete mp_ScrollArea;
    }
    catch (...) {}
}

/****************************************************************************/
/*!
 *  \brief Adds an empty line
 */
/****************************************************************************/
void CTextPanel::AddNewLine()
{
    mp_Layout->setRowMinimumHeight(m_LineCounter, 19);

    mp_Layout->setRowStretch(m_LineCounter, 0);
    mp_Layout->setRowStretch(m_LineCounter + 1, 1);

    m_LineCounter++;
}

/****************************************************************************/
/*!
 *  \brief Adds a paragraph caption
 *
 *  \iparam Caption = Text of the caption
 */
/****************************************************************************/
void CTextPanel::AddCaption(QString Caption)
{
    //lint -esym(429, pCaption)
    QLabel *p_Caption = new QLabel(Caption);
    QFont Font;
    Font.setBold(true);
    p_Caption->setAlignment(Qt::AlignHCenter);
    p_Caption->setFont(Font);

    mp_Layout->addWidget(p_Caption, m_LineCounter, 0, 1, 3);
    mp_Layout->setRowStretch(m_LineCounter, 0);
    mp_Layout->setRowStretch(m_LineCounter + 1, 1);

    m_Caption << p_Caption;

    m_LineCounter++;
}

/****************************************************************************/
/*!
 *  \brief Adds a text item
 *
 *  \iparam Identifier = ID used to access a text element
 *  \iparam Description = Text displayed on the left side of the screen
 *  \iparam Value = Item value
 */
/****************************************************************************/
void CTextPanel::AddTextItem(QString Identifier, QString Description, QString Value)
{
    QLabel *p_Description = new QLabel(Description);
    QLabel *p_Value = new QLabel(Value);

    mp_Layout->addWidget(p_Description, m_LineCounter, 0);
    mp_Layout->addWidget(p_Value, m_LineCounter, 1);
    mp_Layout->setRowStretch(m_LineCounter, 0);
    mp_Layout->setRowStretch(m_LineCounter + 1, 1);

    m_Description[Identifier] = p_Description;
    m_Value[Identifier] = p_Value;

    m_LineCounter++;
}

/****************************************************************************/
/*!
 *  \brief Updates the value of a text item
 *
 *  \iparam Identifier = ID used to access a text element
 *  \iparam Value = Item value
 */
/****************************************************************************/
void CTextPanel::UpdateTextItem(QString Identifier, QString Value)
{
    m_Value[Identifier]->setText(tr("%1").arg(Value));
}

} // end namespace MainMenu

/****************************************************************************/
/*! \file TextPanel.h
 *
 *  \brief TextPanel definition.
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

#ifndef MAINMENU_TEXTPANEL_H
#define MAINMENU_TEXTPANEL_H

#include "MainMenu/Include/ContentScroller.h"
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QScrollArea>

namespace MainMenu {

/****************************************************************************/
/**
 * \brief This is a base class for all panels displaying text items.
 */
/****************************************************************************/
class CTextPanel : public CContentScroller
{
    Q_OBJECT

public:
    explicit CTextPanel(QWidget *p_Parent = 0);
    virtual ~CTextPanel();

protected:
    void AddNewLine();
    void AddCaption(QString Caption);
    void AddTextItem(QString Identifier, QString Description, QString Value);
    void UpdateTextItem(QString Identifier, QString Value);

    QGridLayout *mp_Layout; //!< Layout of the text items
    qint32 m_LineCounter;   //!< Number of lines displayed on the screen

private:
    QWidget *mp_Widget;                     //!< Widget holding the content
    QScrollArea *mp_ScrollArea;             //!< The scroll area embedding the main widget
    QList<QLabel *> m_Caption;              //!< List of captions
    QMap<QString, QLabel *> m_Description;  //!< The descriptions displayed on the screen
    QMap<QString, QLabel *> m_Value;        //!< The values displayed on the right side of the screen
};

} // end namespace MainMenu

#endif // MAINMENU_TEXTPANEL_H

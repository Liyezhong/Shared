/****************************************************************************/
/*! \file MenuGroup.h
 *
 *  \brief MenuGroup definition.
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

#ifndef MAINMENU_MENUGROUP_H
#define MAINMENU_MENUGROUP_H

#include <QPushButton>
#include <QSignalMapper>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace MainMenu {

namespace Ui {
    class CMenuGroup;
}

/****************************************************************************/
/**
 * \brief This class contains menu buttons and all panels that are accessible
 *        through the menu.
 */
/****************************************************************************/
class CMenuGroup : public QWidget
{
    Q_OBJECT

public:
    explicit CMenuGroup(QWidget *p_Parent = 0);
    virtual ~CMenuGroup();

    void AddPanel (QString Title, QWidget *p_Content);
    QWidget *GetCurrentPanel();

private:
    void AddButton (QString Text);

    Ui::CMenuGroup *mp_Ui;              //!< User interface
    QVBoxLayout *mp_ContentLayout;      //!< Layout content panel
    QStackedWidget *mp_ContentStack;    //!< Stack holding the content screens
    QVBoxLayout *mp_ButtonLayout;       //!< Layout of the menu buttons
    QButtonGroup *mp_ButtonGroup;       //!< Group containing the menu buttons
    int m_ItemCount;                    //!< Number of items grouped by this screen

protected:
    void changeEvent(QEvent *p_Event);

private slots:
    void ButtonPressed(int Number);
};

} // end namespace MainMenu

#endif // MAINMENU_MENUGROUP_H

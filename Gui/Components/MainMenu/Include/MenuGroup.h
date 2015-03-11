/****************************************************************************/
/*! \file MenuGroup.h
 *
 *  \brief Header file for class CMenuGroup.
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

//lint -e429

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
    friend class  CTestMainMenu;

public:
    explicit CMenuGroup(QWidget *p_Parent = 0);
    virtual ~CMenuGroup();

    void AddPanel (QString Title, QWidget *p_Content);
    void AddSettingsPanel (QString Title, QWidget *p_Content);
    QWidget *GetCurrentPanel();

    void SetEnableButton(int Index, bool Enable, bool NextFlag = false);

    void Clear();
private:
    void AddButton (QString Text);

    Ui::CMenuGroup *mp_Ui;              //!< User interface
    QVBoxLayout *mp_ContentLayout;      //!< Layout content panel
    QStackedWidget *mp_ContentStack;    //!< Stack holding the content screens
    QVBoxLayout *mp_ButtonLayout;       //!< Layout of the menu buttons
    QButtonGroup *mp_ButtonGroup;       //!< Group containing the menu buttons
    int m_ItemCount;                    //!< Number of items grouped by this screen

    QWidget *mp_Widget;
    int m_ButtonNumber;
    int m_CurrentTabIndex;

    QVBoxLayout *mp_VBoxLayout;         //!< Vertical box layout object
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CMenuGroup)

protected:
    void changeEvent(QEvent *p_Event);

signals:
    void PanelChanged();

private slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ButtonPressed(int Number);

public slots:
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void SetCurrentTabIndex(int TabIndex);
    /****************************************************************************/
    /*!
     *  \brief  Definition/Declaration of slot UpdateReagentList
     */
    /****************************************************************************/
    void ShowWidget(int);
};

} // end namespace MainMenu

#endif // MAINMENU_MENUGROUP_H

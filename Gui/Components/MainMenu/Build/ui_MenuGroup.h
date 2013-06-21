/********************************************************************************
** Form generated from reading UI file 'MenuGroup.ui'
**
** Created: Sun Jan 6 10:16:49 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MENUGROUP_H
#define UI_MENUGROUP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "MainMenu/Include/PanelFrame.h"

namespace MainMenu {

class Ui_CMenuGroup
{
public:
    QHBoxLayout *horizontalLayout;
    CPanelFrame *menuWidget;
    CPanelFrame *contentWidget;

    void setupUi(QWidget *MainMenu__CMenuGroup)
    {
        if (MainMenu__CMenuGroup->objectName().isEmpty())
            MainMenu__CMenuGroup->setObjectName(QString::fromUtf8("MainMenu__CMenuGroup"));
        MainMenu__CMenuGroup->resize(608, 349);
        horizontalLayout = new QHBoxLayout(MainMenu__CMenuGroup);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        menuWidget = new CPanelFrame(MainMenu__CMenuGroup);
        menuWidget->setObjectName(QString::fromUtf8("menuWidget"));

        horizontalLayout->addWidget(menuWidget);

        contentWidget = new CPanelFrame(MainMenu__CMenuGroup);
        contentWidget->setObjectName(QString::fromUtf8("contentWidget"));

        horizontalLayout->addWidget(contentWidget);

        horizontalLayout->setStretch(1, 1);

        retranslateUi(MainMenu__CMenuGroup);

        QMetaObject::connectSlotsByName(MainMenu__CMenuGroup);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CMenuGroup)
    {
        MainMenu__CMenuGroup->setWindowTitle(QApplication::translate("MainMenu::CMenuGroup", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CMenuGroup: public Ui_CMenuGroup {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_MENUGROUP_H

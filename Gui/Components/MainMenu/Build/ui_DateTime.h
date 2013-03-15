/********************************************************************************
** Form generated from reading UI file 'DateTime.ui'
**
** Created: Fri Mar 15 03:54:29 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATETIME_H
#define UI_DATETIME_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "MainMenu/Include/WheelPanel.h"

namespace MainMenu {

class Ui_CDateTime
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    MainMenu::CWheelPanel *scrollPanelDate;
    QSpacerItem *horizontalSpacer_2;
    MainMenu::CWheelPanel *scrollPanelTime;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *pushButton;

    void setupUi(QWidget *MainMenu__CDateTime)
    {
        if (MainMenu__CDateTime->objectName().isEmpty())
            MainMenu__CDateTime->setObjectName(QString::fromUtf8("MainMenu__CDateTime"));
        MainMenu__CDateTime->resize(111, 87);
        verticalLayout = new QVBoxLayout(MainMenu__CDateTime);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 27, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        scrollPanelDate = new MainMenu::CWheelPanel(MainMenu__CDateTime);
        scrollPanelDate->setObjectName(QString::fromUtf8("scrollPanelDate"));

        horizontalLayout_2->addWidget(scrollPanelDate);

        horizontalSpacer_2 = new QSpacerItem(38, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        scrollPanelTime = new MainMenu::CWheelPanel(MainMenu__CDateTime);
        scrollPanelTime->setObjectName(QString::fromUtf8("scrollPanelTime"));

        horizontalLayout_2->addWidget(scrollPanelTime);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer_3 = new QSpacerItem(20, 28, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_4 = new QSpacerItem(418, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        pushButton = new QPushButton(MainMenu__CDateTime);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(MainMenu__CDateTime);

        QMetaObject::connectSlotsByName(MainMenu__CDateTime);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CDateTime)
    {
        MainMenu__CDateTime->setWindowTitle(QApplication::translate("MainMenu::CDateTime", "Form", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainMenu::CDateTime", "Apply", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CDateTime: public Ui_CDateTime {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_DATETIME_H

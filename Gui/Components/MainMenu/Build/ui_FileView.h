/********************************************************************************
** Form generated from reading UI file 'FileView.ui'
**
** Created: Sun Jan 6 10:16:49 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILEVIEW_H
#define UI_FILEVIEW_H

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
#include "MainMenu/Include/ScrollTable.h"

namespace MainMenu {

class Ui_CFileView
{
public:
    QVBoxLayout *verticalLayout;
    MainMenu::CScrollTable *widget;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;

    void setupUi(QWidget *MainMenu__CFileView)
    {
        if (MainMenu__CFileView->objectName().isEmpty())
            MainMenu__CFileView->setObjectName(QString::fromUtf8("MainMenu__CFileView"));
        MainMenu__CFileView->resize(120, 75);
        verticalLayout = new QVBoxLayout(MainMenu__CFileView);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new MainMenu::CScrollTable(MainMenu__CFileView);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMinimumSize(QSize(0, 0));

        verticalLayout->addWidget(widget);

        verticalSpacer = new QSpacerItem(20, 200, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(MainMenu__CFileView);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(MainMenu__CFileView);

        QMetaObject::connectSlotsByName(MainMenu__CFileView);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CFileView)
    {
        MainMenu__CFileView->setWindowTitle(QApplication::translate("MainMenu::CFileView", "Form", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainMenu::CFileView", "Open", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CFileView: public Ui_CFileView {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_FILEVIEW_H

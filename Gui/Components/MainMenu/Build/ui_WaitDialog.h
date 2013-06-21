/********************************************************************************
** Form generated from reading UI file 'WaitDialog.ui'
**
** Created: Sun Jan 6 10:16:49 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAITDIALOG_H
#define UI_WAITDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include "MainMenu/Include/WaitIndicator.h"

namespace MainMenu {

class Ui_CWaitDialog
{
public:
    QHBoxLayout *horizontalLayout;
    MainMenu::CWaitIndicator *widget;
    QSpacerItem *horizontalSpacer;
    QLabel *contentLabel;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *abortButton;

    void setupUi(QWidget *MainMenu__CWaitDialog)
    {
        if (MainMenu__CWaitDialog->objectName().isEmpty())
            MainMenu__CWaitDialog->setObjectName(QString::fromUtf8("MainMenu__CWaitDialog"));
        MainMenu__CWaitDialog->resize(196, 45);
        horizontalLayout = new QHBoxLayout(MainMenu__CWaitDialog);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        widget = new MainMenu::CWaitIndicator(MainMenu__CWaitDialog);
        widget->setObjectName(QString::fromUtf8("widget"));

        horizontalLayout->addWidget(widget);

        horizontalSpacer = new QSpacerItem(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        contentLabel = new QLabel(MainMenu__CWaitDialog);
        contentLabel->setObjectName(QString::fromUtf8("contentLabel"));

        horizontalLayout->addWidget(contentLabel);

        horizontalSpacer_2 = new QSpacerItem(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        abortButton = new QPushButton(MainMenu__CWaitDialog);
        abortButton->setObjectName(QString::fromUtf8("abortButton"));
        abortButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(abortButton);


        retranslateUi(MainMenu__CWaitDialog);

        QMetaObject::connectSlotsByName(MainMenu__CWaitDialog);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CWaitDialog)
    {
        MainMenu__CWaitDialog->setWindowTitle(QApplication::translate("MainMenu::CWaitDialog", "Form", 0, QApplication::UnicodeUTF8));
        contentLabel->setText(QApplication::translate("MainMenu::CWaitDialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        abortButton->setText(QApplication::translate("MainMenu::CWaitDialog", "Abort", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CWaitDialog: public Ui_CWaitDialog {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_WAITDIALOG_H

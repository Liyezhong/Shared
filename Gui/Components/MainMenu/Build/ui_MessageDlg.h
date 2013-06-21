/********************************************************************************
** Form generated from reading UI file 'MessageDlg.ui'
**
** Created: Sun Jan 6 10:16:49 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEDLG_H
#define UI_MESSAGEDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace MainMenu {

class Ui_CMessageDlg
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *labelIcon;
    QLabel *labelMessage;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *Button3;
    QSpacerItem *horizontalSpacer;
    QPushButton *Button2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *Button1;

    void setupUi(QWidget *MainMenu__CMessageDlg)
    {
        if (MainMenu__CMessageDlg->objectName().isEmpty())
            MainMenu__CMessageDlg->setObjectName(QString::fromUtf8("MainMenu__CMessageDlg"));
        MainMenu__CMessageDlg->resize(341, 75);
        verticalLayout = new QVBoxLayout(MainMenu__CMessageDlg);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        labelIcon = new QLabel(MainMenu__CMessageDlg);
        labelIcon->setObjectName(QString::fromUtf8("labelIcon"));

        horizontalLayout->addWidget(labelIcon);

        labelMessage = new QLabel(MainMenu__CMessageDlg);
        labelMessage->setObjectName(QString::fromUtf8("labelMessage"));
        labelMessage->setMaximumSize(QSize(500, 16777215));
        labelMessage->setWordWrap(true);

        horizontalLayout->addWidget(labelMessage);

        horizontalLayout->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        Button3 = new QPushButton(MainMenu__CMessageDlg);
        Button3->setObjectName(QString::fromUtf8("Button3"));

        horizontalLayout_2->addWidget(Button3);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        Button2 = new QPushButton(MainMenu__CMessageDlg);
        Button2->setObjectName(QString::fromUtf8("Button2"));
        Button2->setEnabled(true);

        horizontalLayout_2->addWidget(Button2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        Button1 = new QPushButton(MainMenu__CMessageDlg);
        Button1->setObjectName(QString::fromUtf8("Button1"));

        horizontalLayout_2->addWidget(Button1);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(MainMenu__CMessageDlg);

        QMetaObject::connectSlotsByName(MainMenu__CMessageDlg);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CMessageDlg)
    {
        Button3->setText(QApplication::translate("MainMenu::CMessageDlg", "PushButton", 0, QApplication::UnicodeUTF8));
        Button2->setText(QApplication::translate("MainMenu::CMessageDlg", "PushButton", 0, QApplication::UnicodeUTF8));
        Button1->setText(QApplication::translate("MainMenu::CMessageDlg", "PushButton", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MainMenu__CMessageDlg);
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CMessageDlg: public Ui_CMessageDlg {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_MESSAGEDLG_H

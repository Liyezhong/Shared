/********************************************************************************
** Form generated from reading UI file 'TableMoving.ui'
**
** Created: Fri Mar 15 03:54:29 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TABLEMOVING_H
#define UI_TABLEMOVING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

namespace MainMenu {

class Ui_CTableMoving
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *beginButton;
    QPushButton *upButton;
    QPushButton *downButton;
    QPushButton *endButton;

    void setupUi(QGroupBox *MainMenu__CTableMoving)
    {
        if (MainMenu__CTableMoving->objectName().isEmpty())
            MainMenu__CTableMoving->setObjectName(QString::fromUtf8("MainMenu__CTableMoving"));
        MainMenu__CTableMoving->resize(125, 306);
        verticalLayout = new QVBoxLayout(MainMenu__CTableMoving);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        beginButton = new QPushButton(MainMenu__CTableMoving);
        beginButton->setObjectName(QString::fromUtf8("beginButton"));
        beginButton->setMinimumSize(QSize(0, 62));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/HimalayaImages/Icons/Move/Icon_Move_Up_End.png"), QSize(), QIcon::Normal, QIcon::Off);
        beginButton->setIcon(icon);
        beginButton->setIconSize(QSize(71, 62));

        verticalLayout->addWidget(beginButton);

        upButton = new QPushButton(MainMenu__CTableMoving);
        upButton->setObjectName(QString::fromUtf8("upButton"));
        upButton->setMinimumSize(QSize(0, 62));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/HimalayaImages/Icons/Move/Icon_Move_Up.png"), QSize(), QIcon::Normal, QIcon::Off);
        upButton->setIcon(icon1);
        upButton->setIconSize(QSize(71, 62));

        verticalLayout->addWidget(upButton);

        downButton = new QPushButton(MainMenu__CTableMoving);
        downButton->setObjectName(QString::fromUtf8("downButton"));
        downButton->setMinimumSize(QSize(0, 62));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/HimalayaImages/Icons/Move/Icon_Move_Down.png"), QSize(), QIcon::Normal, QIcon::Off);
        downButton->setIcon(icon2);
        downButton->setIconSize(QSize(71, 62));

        verticalLayout->addWidget(downButton);

        endButton = new QPushButton(MainMenu__CTableMoving);
        endButton->setObjectName(QString::fromUtf8("endButton"));
        endButton->setMinimumSize(QSize(0, 62));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/HimalayaImages/Icons/Move/Icon_Move_Down_End.png"), QSize(), QIcon::Normal, QIcon::Off);
        endButton->setIcon(icon3);
        endButton->setIconSize(QSize(71, 62));

        verticalLayout->addWidget(endButton);


        retranslateUi(MainMenu__CTableMoving);

        QMetaObject::connectSlotsByName(MainMenu__CTableMoving);
    } // setupUi

    void retranslateUi(QGroupBox *MainMenu__CTableMoving)
    {
        MainMenu__CTableMoving->setTitle(QApplication::translate("MainMenu::CTableMoving", "Move", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CTableMoving: public Ui_CTableMoving {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_TABLEMOVING_H

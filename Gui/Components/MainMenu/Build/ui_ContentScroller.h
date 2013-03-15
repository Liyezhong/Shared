/********************************************************************************
** Form generated from reading UI file 'ContentScroller.ui'
**
** Created: Fri Mar 15 03:54:29 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTENTSCROLLER_H
#define UI_CONTENTSCROLLER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace MainMenu {

class Ui_CContentScroller
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnScrollUp;
    QFrame *lineUp;
    QPushButton *btnScrollBegin;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnScrollDown;
    QFrame *lineDown;
    QPushButton *btnScrollEnd;

    void setupUi(QWidget *MainMenu__CContentScroller)
    {
        if (MainMenu__CContentScroller->objectName().isEmpty())
            MainMenu__CContentScroller->setObjectName(QString::fromUtf8("MainMenu__CContentScroller"));
        MainMenu__CContentScroller->resize(117, 66);
        verticalLayout = new QVBoxLayout(MainMenu__CContentScroller);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        btnScrollUp = new QPushButton(MainMenu__CContentScroller);
        btnScrollUp->setObjectName(QString::fromUtf8("btnScrollUp"));
        btnScrollUp->setMinimumSize(QSize(0, 31));
        btnScrollUp->setFocusPolicy(Qt::NoFocus);
        btnScrollUp->setIconSize(QSize(26, 17));
        btnScrollUp->setFlat(true);

        horizontalLayout_2->addWidget(btnScrollUp);

        lineUp = new QFrame(MainMenu__CContentScroller);
        lineUp->setObjectName(QString::fromUtf8("lineUp"));
        lineUp->setFrameShadow(QFrame::Plain);
        lineUp->setFrameShape(QFrame::VLine);

        horizontalLayout_2->addWidget(lineUp);

        btnScrollBegin = new QPushButton(MainMenu__CContentScroller);
        btnScrollBegin->setObjectName(QString::fromUtf8("btnScrollBegin"));
        btnScrollBegin->setMinimumSize(QSize(0, 31));
        btnScrollBegin->setFocusPolicy(Qt::NoFocus);
        btnScrollBegin->setIconSize(QSize(26, 17));
        btnScrollBegin->setFlat(true);

        horizontalLayout_2->addWidget(btnScrollBegin);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        btnScrollDown = new QPushButton(MainMenu__CContentScroller);
        btnScrollDown->setObjectName(QString::fromUtf8("btnScrollDown"));
        btnScrollDown->setMinimumSize(QSize(0, 31));
        btnScrollDown->setFocusPolicy(Qt::NoFocus);
        btnScrollDown->setIconSize(QSize(26, 17));
        btnScrollDown->setFlat(true);

        horizontalLayout->addWidget(btnScrollDown);

        lineDown = new QFrame(MainMenu__CContentScroller);
        lineDown->setObjectName(QString::fromUtf8("lineDown"));
        lineDown->setFrameShadow(QFrame::Plain);
        lineDown->setFrameShape(QFrame::VLine);

        horizontalLayout->addWidget(lineDown);

        btnScrollEnd = new QPushButton(MainMenu__CContentScroller);
        btnScrollEnd->setObjectName(QString::fromUtf8("btnScrollEnd"));
        btnScrollEnd->setMinimumSize(QSize(0, 31));
        btnScrollEnd->setFocusPolicy(Qt::NoFocus);
        btnScrollEnd->setIconSize(QSize(26, 17));
        btnScrollEnd->setFlat(true);

        horizontalLayout->addWidget(btnScrollEnd);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(MainMenu__CContentScroller);

        QMetaObject::connectSlotsByName(MainMenu__CContentScroller);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CContentScroller)
    {
        MainMenu__CContentScroller->setWindowTitle(QApplication::translate("MainMenu::CContentScroller", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CContentScroller: public Ui_CContentScroller {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_CONTENTSCROLLER_H

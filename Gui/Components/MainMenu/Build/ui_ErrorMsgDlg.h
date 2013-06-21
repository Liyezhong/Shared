/********************************************************************************
** Form generated from reading UI file 'ErrorMsgDlg.ui'
**
** Created: Sun Jan 6 10:16:49 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ERRORMSGDLG_H
#define UI_ERRORMSGDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include "MainMenu/Include/ScrollTable.h"

namespace MainMenu {

class Ui_CErrorMsgDlg
{
public:
    QGridLayout *gridLayout;
    QLabel *captionLabel;
    MainMenu::CScrollTable *widget;
    QSpacerItem *horizontalSpacer;
    QPushButton *closeButton;

    void setupUi(QWidget *MainMenu__CErrorMsgDlg)
    {
        if (MainMenu__CErrorMsgDlg->objectName().isEmpty())
            MainMenu__CErrorMsgDlg->setObjectName(QString::fromUtf8("MainMenu__CErrorMsgDlg"));
        MainMenu__CErrorMsgDlg->resize(168, 74);
        gridLayout = new QGridLayout(MainMenu__CErrorMsgDlg);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        captionLabel = new QLabel(MainMenu__CErrorMsgDlg);
        captionLabel->setObjectName(QString::fromUtf8("captionLabel"));
        QFont font;
        font.setPointSize(11);
        captionLabel->setFont(font);

        gridLayout->addWidget(captionLabel, 0, 0, 1, 2);

        widget = new MainMenu::CScrollTable(MainMenu__CErrorMsgDlg);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout->addWidget(widget, 1, 0, 1, 3);

        horizontalSpacer = new QSpacerItem(1, 24, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 0, 1, 1);

        closeButton = new QPushButton(MainMenu__CErrorMsgDlg);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        closeButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(closeButton, 2, 2, 1, 1);

        gridLayout->setRowStretch(1, 1);

        retranslateUi(MainMenu__CErrorMsgDlg);

        QMetaObject::connectSlotsByName(MainMenu__CErrorMsgDlg);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CErrorMsgDlg)
    {
        MainMenu__CErrorMsgDlg->setWindowTitle(QApplication::translate("MainMenu::CErrorMsgDlg", "Form", 0, QApplication::UnicodeUTF8));
        captionLabel->setText(QApplication::translate("MainMenu::CErrorMsgDlg", "TextLabel", 0, QApplication::UnicodeUTF8));
        closeButton->setText(QApplication::translate("MainMenu::CErrorMsgDlg", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CErrorMsgDlg: public Ui_CErrorMsgDlg {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_ERRORMSGDLG_H

/********************************************************************************
** Form generated from reading UI file 'WarningMsgDlg.ui'
**
** Created: Fri Mar 15 03:54:29 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WARNINGMSGDLG_H
#define UI_WARNINGMSGDLG_H

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

class Ui_CWarningMsgDlg
{
public:
    QGridLayout *gridLayout;
    QLabel *captionLabel;
    MainMenu::CScrollTable *widget;
    QSpacerItem *horizontalSpacer;
    QPushButton *closeButton;

    void setupUi(QWidget *MainMenu__CWarningMsgDlg)
    {
        if (MainMenu__CWarningMsgDlg->objectName().isEmpty())
            MainMenu__CWarningMsgDlg->setObjectName(QString::fromUtf8("MainMenu__CWarningMsgDlg"));
        MainMenu__CWarningMsgDlg->resize(168, 74);
        gridLayout = new QGridLayout(MainMenu__CWarningMsgDlg);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        captionLabel = new QLabel(MainMenu__CWarningMsgDlg);
        captionLabel->setObjectName(QString::fromUtf8("captionLabel"));
        QFont font;
        font.setPointSize(11);
        captionLabel->setFont(font);

        gridLayout->addWidget(captionLabel, 0, 0, 1, 2);

        widget = new MainMenu::CScrollTable(MainMenu__CWarningMsgDlg);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout->addWidget(widget, 1, 0, 1, 3);

        horizontalSpacer = new QSpacerItem(1, 24, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 0, 1, 1);

        closeButton = new QPushButton(MainMenu__CWarningMsgDlg);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        closeButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(closeButton, 2, 2, 1, 1);

        gridLayout->setRowStretch(1, 1);

        retranslateUi(MainMenu__CWarningMsgDlg);

        QMetaObject::connectSlotsByName(MainMenu__CWarningMsgDlg);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CWarningMsgDlg)
    {
        MainMenu__CWarningMsgDlg->setWindowTitle(QApplication::translate("MainMenu::CWarningMsgDlg", "Form", 0, QApplication::UnicodeUTF8));
        captionLabel->setText(QApplication::translate("MainMenu::CWarningMsgDlg", "TextLabel", 0, QApplication::UnicodeUTF8));
        closeButton->setText(QApplication::translate("MainMenu::CWarningMsgDlg", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CWarningMsgDlg: public Ui_CWarningMsgDlg {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_WARNINGMSGDLG_H

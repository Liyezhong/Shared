/********************************************************************************
** Form generated from reading UI file 'DialogFrame.ui'
**
** Created: Sun Jan 6 10:16:49 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGFRAME_H
#define UI_DIALOGFRAME_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include "MainMenu/Include/PanelFrame.h"

namespace MainMenu {

class Ui_CDialogFrame
{
public:
    QVBoxLayout *verticalLayout;
    MainMenu::CPanelFrame *widget;

    void setupUi(QDialog *MainMenu__CDialogFrame)
    {
        if (MainMenu__CDialogFrame->objectName().isEmpty())
            MainMenu__CDialogFrame->setObjectName(QString::fromUtf8("MainMenu__CDialogFrame"));
        MainMenu__CDialogFrame->resize(16, 16);
        verticalLayout = new QVBoxLayout(MainMenu__CDialogFrame);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new MainMenu::CPanelFrame(MainMenu__CDialogFrame);
        widget->setObjectName(QString::fromUtf8("widget"));

        verticalLayout->addWidget(widget);


        retranslateUi(MainMenu__CDialogFrame);

        QMetaObject::connectSlotsByName(MainMenu__CDialogFrame);
    } // setupUi

    void retranslateUi(QDialog *MainMenu__CDialogFrame)
    {
        MainMenu__CDialogFrame->setWindowTitle(QApplication::translate("MainMenu::CDialogFrame", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CDialogFrame: public Ui_CDialogFrame {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_DIALOGFRAME_H

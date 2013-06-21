/********************************************************************************
** Form generated from reading UI file 'PanelFrame.ui'
**
** Created: Sun Jan 6 10:16:49 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PANELFRAME_H
#define UI_PANELFRAME_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace MainMenu {

class Ui_CPanelFrame
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *panelTitle;
    QSpacerItem *horizontalSpacer;
    QLabel *panelTitleAdditional;
    QWidget *panelWidget;

    void setupUi(QWidget *MainMenu__CPanelFrame)
    {
        if (MainMenu__CPanelFrame->objectName().isEmpty())
            MainMenu__CPanelFrame->setObjectName(QString::fromUtf8("MainMenu__CPanelFrame"));
        MainMenu__CPanelFrame->resize(108, 39);
        verticalLayout = new QVBoxLayout(MainMenu__CPanelFrame);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(4, 3, 6, 7);
        widget = new QWidget(MainMenu__CPanelFrame);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(14, 0, 14, 0);
        panelTitle = new QLabel(widget);
        panelTitle->setObjectName(QString::fromUtf8("panelTitle"));
        panelTitle->setMinimumSize(QSize(0, 29));

        horizontalLayout->addWidget(panelTitle);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        panelTitleAdditional = new QLabel(widget);
        panelTitleAdditional->setObjectName(QString::fromUtf8("panelTitleAdditional"));
        panelTitleAdditional->setEnabled(true);

        horizontalLayout->addWidget(panelTitleAdditional);


        verticalLayout->addWidget(widget);

        panelWidget = new QWidget(MainMenu__CPanelFrame);
        panelWidget->setObjectName(QString::fromUtf8("panelWidget"));

        verticalLayout->addWidget(panelWidget);

        verticalLayout->setStretch(1, 1);

        retranslateUi(MainMenu__CPanelFrame);

        QMetaObject::connectSlotsByName(MainMenu__CPanelFrame);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CPanelFrame)
    {
        MainMenu__CPanelFrame->setWindowTitle(QApplication::translate("MainMenu::CPanelFrame", "Form", 0, QApplication::UnicodeUTF8));
        panelTitle->setText(QString());
        panelTitleAdditional->setText(QString());
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CPanelFrame: public Ui_CPanelFrame {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_PANELFRAME_H

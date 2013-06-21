/********************************************************************************
** Form generated from reading UI file 'DataManagement.ui'
**
** Created: Sun Jan 6 10:16:49 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATAMANAGEMENT_H
#define UI_DATAMANAGEMENT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

namespace MainMenu {

class Ui_CDataManagement
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *exportButton;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *serviceExportButton;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_9;
    QPushButton *importButton;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *horizontalSpacer_10;
    QPushButton *softwareUpdateButton;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *remoteSWUpdateButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *MainMenu__CDataManagement)
    {
        if (MainMenu__CDataManagement->objectName().isEmpty())
            MainMenu__CDataManagement->setObjectName(QString::fromUtf8("MainMenu__CDataManagement"));
        MainMenu__CDataManagement->resize(205, 337);
        gridLayout = new QGridLayout(MainMenu__CDataManagement);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalSpacer_7 = new QSpacerItem(11, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_7, 0, 0, 1, 1);

        exportButton = new QPushButton(MainMenu__CDataManagement);
        exportButton->setObjectName(QString::fromUtf8("exportButton"));
        exportButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(exportButton, 0, 1, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(12, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_6, 0, 2, 1, 1);

        horizontalSpacer_8 = new QSpacerItem(11, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_8, 1, 0, 1, 1);

        serviceExportButton = new QPushButton(MainMenu__CDataManagement);
        serviceExportButton->setObjectName(QString::fromUtf8("serviceExportButton"));

        gridLayout->addWidget(serviceExportButton, 1, 1, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(12, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_5, 1, 2, 1, 1);

        horizontalSpacer_9 = new QSpacerItem(11, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_9, 2, 0, 1, 1);

        importButton = new QPushButton(MainMenu__CDataManagement);
        importButton->setObjectName(QString::fromUtf8("importButton"));
        importButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(importButton, 2, 1, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(12, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 2, 2, 1, 1);

        horizontalSpacer_10 = new QSpacerItem(11, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_10, 3, 0, 1, 1);

        softwareUpdateButton = new QPushButton(MainMenu__CDataManagement);
        softwareUpdateButton->setObjectName(QString::fromUtf8("softwareUpdateButton"));

        gridLayout->addWidget(softwareUpdateButton, 3, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(12, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 3, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(44, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 4, 0, 1, 1);

        remoteSWUpdateButton = new QPushButton(MainMenu__CDataManagement);
        remoteSWUpdateButton->setObjectName(QString::fromUtf8("remoteSWUpdateButton"));

        gridLayout->addWidget(remoteSWUpdateButton, 4, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(44, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 4, 2, 1, 1);


        retranslateUi(MainMenu__CDataManagement);

        QMetaObject::connectSlotsByName(MainMenu__CDataManagement);
    } // setupUi

    void retranslateUi(QWidget *MainMenu__CDataManagement)
    {
        MainMenu__CDataManagement->setWindowTitle(QApplication::translate("MainMenu::CDataManagement", "Form", 0, QApplication::UnicodeUTF8));
        exportButton->setText(QApplication::translate("MainMenu::CDataManagement", "User Export", 0, QApplication::UnicodeUTF8));
        serviceExportButton->setText(QApplication::translate("MainMenu::CDataManagement", "Service Export", 0, QApplication::UnicodeUTF8));
        importButton->setText(QApplication::translate("MainMenu::CDataManagement", "Import", 0, QApplication::UnicodeUTF8));
        softwareUpdateButton->setText(QApplication::translate("MainMenu::CDataManagement", "SW Update", 0, QApplication::UnicodeUTF8));
        remoteSWUpdateButton->setText(QApplication::translate("MainMenu::CDataManagement", "Remote SW Update", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CDataManagement: public Ui_CDataManagement {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_DATAMANAGEMENT_H

/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created: Fri Mar 15 03:54:29 2013
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace MainMenu {

class Ui_CMainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QWidget *caption;
    QHBoxLayout *horizontalLayout;
    QLabel *labelDate;
    QSpacerItem *horizontalSpacer_3;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_3;
    QLabel *statusLabelErr;
    QLabel *statusLabelWarn;
    QSpacerItem *horizontalSpacer_23;
    QLabel *labelTime;
    QSpacerItem *horizontalSpacer;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *statusLabel2;
    QLabel *statusLabel1;
    QLabel *labelUser;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QTabWidget *TabWidget;
    QWidget *tab_2;
    QWidget *tab;

    void setupUi(QMainWindow *MainMenu__CMainWindow)
    {
        if (MainMenu__CMainWindow->objectName().isEmpty())
            MainMenu__CMainWindow->setObjectName(QString::fromUtf8("MainMenu__CMainWindow"));
        MainMenu__CMainWindow->resize(800, 106);
        centralWidget = new QWidget(MainMenu__CMainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        caption = new QWidget(centralWidget);
        caption->setObjectName(QString::fromUtf8("caption"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(caption->sizePolicy().hasHeightForWidth());
        caption->setSizePolicy(sizePolicy);
        caption->setMinimumSize(QSize(0, 33));
        caption->setAutoFillBackground(true);
        horizontalLayout = new QHBoxLayout(caption);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(13, 0, 13, 0);
        labelDate = new QLabel(caption);
        labelDate->setObjectName(QString::fromUtf8("labelDate"));
        sizePolicy.setHeightForWidth(labelDate->sizePolicy().hasHeightForWidth());
        labelDate->setSizePolicy(sizePolicy);
        labelDate->setMinimumSize(QSize(0, 0));
        labelDate->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(labelDate);

        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        frame = new QFrame(caption);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Plain);
        horizontalLayout_3 = new QHBoxLayout(frame);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        statusLabelErr = new QLabel(frame);
        statusLabelErr->setObjectName(QString::fromUtf8("statusLabelErr"));
        sizePolicy.setHeightForWidth(statusLabelErr->sizePolicy().hasHeightForWidth());
        statusLabelErr->setSizePolicy(sizePolicy);
        statusLabelErr->setMinimumSize(QSize(48, 33));
        statusLabelErr->setMaximumSize(QSize(48, 16777215));
        statusLabelErr->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(statusLabelErr);

        statusLabelWarn = new QLabel(frame);
        statusLabelWarn->setObjectName(QString::fromUtf8("statusLabelWarn"));
        sizePolicy.setHeightForWidth(statusLabelWarn->sizePolicy().hasHeightForWidth());
        statusLabelWarn->setSizePolicy(sizePolicy);
        statusLabelWarn->setMinimumSize(QSize(48, 33));
        statusLabelWarn->setMaximumSize(QSize(48, 16777215));
        statusLabelWarn->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(statusLabelWarn);


        horizontalLayout->addWidget(frame);

        horizontalSpacer_23 = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_23);

        labelTime = new QLabel(caption);
        labelTime->setObjectName(QString::fromUtf8("labelTime"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelTime->sizePolicy().hasHeightForWidth());
        labelTime->setSizePolicy(sizePolicy1);
        labelTime->setMinimumSize(QSize(0, 0));
        labelTime->setMaximumSize(QSize(115, 16777215));
        QFont font;
        font.setPointSize(16);
        labelTime->setFont(font);
        labelTime->setLayoutDirection(Qt::LeftToRight);
        labelTime->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(labelTime);

        horizontalSpacer = new QSpacerItem(168, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        frame_2 = new QFrame(caption);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        sizePolicy.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy);
        frame_2->setFrameShape(QFrame::NoFrame);
        frame_2->setFrameShadow(QFrame::Plain);
        horizontalLayout_5 = new QHBoxLayout(frame_2);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        statusLabel2 = new QLabel(frame_2);
        statusLabel2->setObjectName(QString::fromUtf8("statusLabel2"));
        sizePolicy.setHeightForWidth(statusLabel2->sizePolicy().hasHeightForWidth());
        statusLabel2->setSizePolicy(sizePolicy);
        statusLabel2->setMinimumSize(QSize(48, 33));
        statusLabel2->setMaximumSize(QSize(48, 16777215));
        statusLabel2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_5->addWidget(statusLabel2);

        statusLabel1 = new QLabel(frame_2);
        statusLabel1->setObjectName(QString::fromUtf8("statusLabel1"));
        sizePolicy.setHeightForWidth(statusLabel1->sizePolicy().hasHeightForWidth());
        statusLabel1->setSizePolicy(sizePolicy);
        statusLabel1->setMinimumSize(QSize(48, 33));
        statusLabel1->setMaximumSize(QSize(48, 16777215));
        statusLabel1->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_5->addWidget(statusLabel1);

        labelUser = new QLabel(frame_2);
        labelUser->setObjectName(QString::fromUtf8("labelUser"));
        labelUser->setPixmap(QPixmap(QString::fromUtf8(":/Icons/User_Status/User_Service.png")));
        labelUser->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_5->addWidget(labelUser);


        horizontalLayout->addWidget(frame_2);


        verticalLayout->addWidget(caption);

        widget = new QWidget(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(6, 7, 10, 12);
        TabWidget = new QTabWidget(widget);
        TabWidget->setObjectName(QString::fromUtf8("TabWidget"));
        TabWidget->setFocusPolicy(Qt::NoFocus);
        TabWidget->setIconSize(QSize(64, 64));
        TabWidget->setUsesScrollButtons(false);
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        TabWidget->addTab(tab_2, QString());
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        TabWidget->addTab(tab, QString());

        horizontalLayout_2->addWidget(TabWidget);


        verticalLayout->addWidget(widget);

        MainMenu__CMainWindow->setCentralWidget(centralWidget);
        widget->raise();
        caption->raise();

        retranslateUi(MainMenu__CMainWindow);

        TabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainMenu__CMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainMenu__CMainWindow)
    {
        MainMenu__CMainWindow->setWindowTitle(QApplication::translate("MainMenu::CMainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        labelDate->setText(QApplication::translate("MainMenu::CMainWindow", "1970-01-01", 0, QApplication::UnicodeUTF8));
        statusLabelErr->setText(QString());
        statusLabelWarn->setText(QString());
        labelTime->setText(QApplication::translate("MainMenu::CMainWindow", "00:00", 0, QApplication::UnicodeUTF8));
        statusLabel2->setText(QString());
        statusLabel1->setText(QString());
        labelUser->setText(QString());
        TabWidget->setTabText(TabWidget->indexOf(tab_2), QApplication::translate("MainMenu::CMainWindow", "Tab 2", 0, QApplication::UnicodeUTF8));
        TabWidget->setTabText(TabWidget->indexOf(tab), QApplication::translate("MainMenu::CMainWindow", "Tab 1", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

} // namespace MainMenu

namespace MainMenu {
namespace Ui {
    class CMainWindow: public Ui_CMainWindow {};
} // namespace Ui
} // namespace MainMenu

#endif // UI_MAINWINDOW_H

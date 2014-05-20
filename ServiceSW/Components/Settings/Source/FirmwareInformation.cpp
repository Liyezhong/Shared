/****************************************************************************/
/*! \file FirmwareInformation.cpp
 *
 *  \brief FirmwareInformation implementation file
 *
 *  \b Description:
 *          This class implements widget to diaplay firmware information.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-05-08
 *   $Author:  $ Soumya. D
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "../Include/PlatformServiceEventCodes.h"
#include "Settings/Include/FirmwareInformation.h"
#include "ui_FirmwareInformation.h"

namespace Settings {

const int TABLE_FIXED_WIDTH = 380;          ///< Table widget width
const int TABLE_FIXED_HEIGHT = 500;         ///< Table widget height

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam p_DataConnector = service data connector object
 *  \iparam p_Parent = Parent widget
 */
/****************************************************************************/
CFirmwareInformation::CFirmwareInformation(QWidget *p_Parent) : QWidget(p_Parent),
    mp_Ui(new Ui::CFirmwareInformation),
    mp_ModuleList(NULL)
{
    mp_Ui->setupUi(this);
    mp_TableWidget = new MainMenu::CBaseTable;
    mp_TableWidget->resize(TABLE_FIXED_WIDTH, TABLE_FIXED_HEIGHT);

    m_Model.setHorizontalHeaderLabels(QStringList() << QApplication::translate("ServiceUpdates::CFirmwareInformation",
                                                       "Slave", 0, QApplication::UnicodeUTF8)
                                                    << QApplication::translate("ServiceUpdates::CDataManagement",
                                                       "Software Version Info", 0, QApplication::UnicodeUTF8));
    mp_TableWidget->horizontalHeader()->show();

    mp_TableWidget->setModel(&m_Model);
    mp_TableWidget->horizontalHeader()->resizeSection(0, 180);   // 0 => Index  90 => Size
    mp_TableWidget->horizontalHeader()->resizeSection(1, 300);   // 1 => Index  300 => Size

    mp_TableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    mp_TableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mp_Ui->widget->SetContent(mp_TableWidget);

    CONNECTSIGNALSLOTGUI(mp_Ui->refreshBtn, clicked(), this, OnRefreshButtonClicked());
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CFirmwareInformation::~CFirmwareInformation()
{
    try {        
        delete mp_TableWidget;
        delete mp_Ui;
    }
    catch (...) {
        // to please Lint
    }
}

/****************************************************************************/
/*!
 *  \brief Event handler for change events
 *
 *  \iparam p_Event = Change event
 */
/****************************************************************************/
void CFirmwareInformation::changeEvent(QEvent *p_Event)
{
    QWidget::changeEvent(p_Event);
    switch (p_Event->type()) {
        case QEvent::LanguageChange:
            mp_Ui->retranslateUi(this);
            this->RetranslateUI();
            break;
        default:
            break;
    }
}

/****************************************************************************/
/*!
 *  \brief Slot for updating the GUI
 */
/****************************************************************************/
void CFirmwareInformation::updateGUI(DataManager::CModuleDataList *p_ModuleList)
{
    if (p_ModuleList != NULL)
    {
        mp_ModuleList = p_ModuleList;
        for (int i = 0; i < p_ModuleList->GetNumberofModules(); i++)
        {
            DataManager::CModule* p_Module = p_ModuleList->GetModule(i);

            for (int j = 0; j < p_Module->GetNumberofSubModules(); j++)
            {
                DataManager::CSubModule* p_SubModule = p_Module->GetSubModuleInfo(j);

                QString SubModuleName = p_SubModule->GetSubModuleName();
                QString SubModuleType = p_SubModule->GetSubModuleType();

                if ((SubModuleName.startsWith("asb")) && (SubModuleType.startsWith("Board"))) {
                    DataManager::Parameter_t *Parameter;
                    Parameter = p_SubModule->GetParameterInfo("SoftwareVersion");
                    AddItem(SubModuleName, Parameter->ParameterValue);
                }
            }
        }
    }
    else
    {
        qDebug()<<"Data container Module List is NULL!!!";
    }
}

/****************************************************************************/
/*!
 *  \brief Slot called for updating the table list
 */
/****************************************************************************/
void CFirmwareInformation::OnRefreshButtonClicked()
{
    Global::EventObject::Instance().RaiseEvent(EVENT_SERVICE_GUI_SETTINGS_FIRMWARE_INFO_REQUESTED);
    if (mp_ModuleList)
    {
        for (int i = 0; i < mp_ModuleList->GetNumberofModules(); i++)
        {
            DataManager::CModule* p_Module = mp_ModuleList->GetModule(i);

            for (int j = 0; j < p_Module->GetNumberofSubModules(); j++)
            {
                DataManager::CSubModule* p_SubModule = p_Module->GetSubModuleInfo(j);

                QString SubModuleName = p_SubModule->GetSubModuleName();

                if (SubModuleName.startsWith("asb")) {
                    DataManager::Parameter_t *Parameter;
                    Parameter = p_SubModule->GetParameterInfo("SoftwareVersion");
                    AddItem(SubModuleName, Parameter->ParameterValue);
                }
            }
        }
    }
}

/****************************************************************************/
/*!
 *  \brief  To add data item to the table
 *  \iparam Slave = ASB name
 *  \iparam SoftwareVersionInfo = Software version Info
 */
/****************************************************************************/
void CFirmwareInformation::AddItem(QString Slave, QString SoftwareVersionInfo)
{
    QList<QStandardItem *> ItemList;
    ItemList << new QStandardItem(Slave);
    ItemList << new QStandardItem(SoftwareVersionInfo);

    m_Model.appendRow(ItemList);
}

/****************************************************************************/
/*!
 *  \brief Translates the strings in UI to the selected language
 */
/****************************************************************************/
void CFirmwareInformation::RetranslateUI()
{
    m_Model.setHorizontalHeaderLabels(QStringList() << QApplication::translate("ServiceUpdates::CFirmwareInformation",
                                                       "Slave", 0, QApplication::UnicodeUTF8)
                                                    << QApplication::translate("ServiceUpdates::CDataManagement",
                                                       "Software Version Info", 0, QApplication::UnicodeUTF8));
}

} // end namespace ServiceUpdates

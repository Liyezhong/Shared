/****************************************************************************/
/*! \file FirmwareInformation.h
 *
 *  \brief  FirmwareInformation definition file
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

#ifndef SETTINGS_FIRMWAREUPDATE_H
#define SETTINGS_FIRMWAREUPDATE_H

#include <QWidget>
#include "MainMenu/Include/BaseTable.h"
#include <QStandardItemModel>
#include "Global/Include/Utils.h"
#include "DataManager/Containers/InstrumentHistory/Include/ModuleDataList.h"
#include "DataManager/Containers/InstrumentHistory/Include/Module.h"

namespace Settings {

namespace Ui {
    class CFirmwareInformation;
}

/****************************************************************************/
/**
 * \brief This widget displays firmware update information
 */
/****************************************************************************/
class CFirmwareInformation : public QWidget
{
    Q_OBJECT

public:
    explicit CFirmwareInformation(QWidget *p_Parent = 0);
    ~CFirmwareInformation();

    /****************************************************************************/
    /*!
     *  \brief  To add data item to the table
     *  \iparam Slave = ASB name
     *  \iparam SoftwareVersionInfo = Software version Info
     */
    /****************************************************************************/
    void AddItem(QString Slave, QString SoftwareVersionInfo);

protected:
    void changeEvent(QEvent *p_Event);

private:
    Ui::CFirmwareInformation *mp_Ui;                //!< User Interface    
    MainMenu::CBaseTable *mp_TableWidget;           //!< Base table for widget
    DataManager::CModuleDataList *mp_ModuleList;    //!< Module list data object
    QStandardItemModel m_Model;                     //!< Model for table

private slots:
    void RetranslateUI();

public slots:
      void updateGUI(DataManager::CModuleDataList *p_ModuleList);
      void OnRefreshButtonClicked();

};

} // end namespace Settings

#endif //SETTINGS_FIRMWAREUPDATE_H

/****************************************************************************/
/*! \file Master/Components/DataManager/Include/DataManagerBase.h
 *
 *  \brief Definition file for class CDataManagerBase.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-11-19
 *  $Author:    $ Michael Thiel, Ramya GJ
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

#ifndef DATAMANAGER_DATAMANAGERBASE_H
#define DATAMANAGER_DATAMANAGERBASE_H

#include <QObject>
#include <QString>
#include <QIODevice>
#include "../Include/DataContainerCollectionBase.h"


namespace DataManager {

class CUserSettingsCommandInterface;

static const quint32 INIT_OK = 0; //!< Return code

//lint -sem(DataManager::CDataManagerBase::DeinitDataContainer,cleanup)

/****************************************************************************/
/**
 * \brief CDataManagerBase definition
 */
/****************************************************************************/
class CDataManagerBase : public QObject
{
    Q_OBJECT
    friend class TestDataManagerInclude;
protected:
    bool m_IsInitialized;           //!< status of initialization
    CDataContainerCollectionBase *mp_DataContainerCollectionBase; //!< pointer to DataContainerCollection base
    Threads::MasterThreadController *mp_MasterThreadController; //!< This is passed to DataContainer
    bool DeinitDataContainer();

public:
    /*lint -e18 */
    CUserSettingsCommandInterface *mp_SettingsCommandInterface; //!< Handles commands related to User Settings
    CDeviceConfigurationInterface *mp_DeviceConfigurationInterface; //!< Handles commands related to device configuration
    quint32 InitDataContainer();

    CDataManagerBase(Threads::MasterThreadController *p_MasterThreadController);
    CDataManagerBase(Threads::MasterThreadController *p_MasterThreadController, CDataContainerCollectionBase *containerCollection);
    virtual ~CDataManagerBase();

    //function to save data during shutdown
    virtual void SaveDataOnShutdown();

    /****************************************************************************/
    /*!
     *  \brief get the data container
     *
     *  \return pointer of data collection base
     */
    /****************************************************************************/
    virtual const CDataContainerCollectionBase* GetDataContainer() { return mp_DataContainerCollectionBase; }
    virtual CUserSettingsInterface* GetUserSettingsInterface();
    virtual CDeviceConfigurationInterface* GetDeviceConfigurationInterface();
    CRCConfigurationInterface* GetRCConfigurationInterface();

    /****************************************************************************/
    /*!
     *  \brief get the status of initialisation
     *
     *  \return true or false
     */
    /****************************************************************************/
    bool IsInitialized() const { return m_IsInitialized; }

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CDataManagerBase)

};
}// namespace DataManager

#endif // DATAMANAGER_DATAMANAGERBASE_H



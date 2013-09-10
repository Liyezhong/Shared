/****************************************************************************/
/*! \file DataManagerBase.h
 *
 *  \brief Definition file for class CDataManagerBase.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-11-19
 *  $Author:    $ Michael Thiel
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
class CDataManagerBase : public QObject
{
    Q_OBJECT
protected:
    bool m_IsInitialized;
    CDataContainerCollectionBase *mp_DataContainerCollection;
    Threads::MasterThreadController *mp_MasterThreadController; //!< This is passed to DataContainer
    bool DeinitDataContainer();

public:
    CUserSettingsCommandInterface *mp_SettingsCommandInterface; //!< Handles commands related to User Settings
    CDeviceConfigurationInterface *mp_DeviceConfigurationInterface; //!< Handles commands related to device configuration
    quint32 InitDataContainer();

    CDataManagerBase(Threads::MasterThreadController *p_MasterThreadController);
    CDataManagerBase(Threads::MasterThreadController *p_MasterThreadController, CDataContainerCollectionBase *containerCollection);
    virtual ~CDataManagerBase();

    virtual const CDataContainerCollectionBase* GetDataContainer() { return mp_DataContainerCollection; }
    virtual CUserSettingsInterface* GetUserSettingsInterface();
    virtual CDeviceConfigurationInterface* GetDeviceConfigurationInterface();

    /****************************************************************************/
    /*!
     *  \brief get the status of initialisation
     *
     *  \return true or false
     */
    /****************************************************************************/
    bool IsInitialized() const { return m_IsInitialized; }

};
}// namespace DataManager

#endif // DATAMANAGER_DATAMANAGERBASE_H



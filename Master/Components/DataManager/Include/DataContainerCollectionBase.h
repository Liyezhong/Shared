/****************************************************************************/
/*! \file Master/Components/DataManager/Include/DataContainerCollectionBase.h
 *
 *  \brief Definition file for class CDataContainerCollectionBase.
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

#ifndef DATAMANAGER_DATACONTAINERCOLLECTIONBASE_H
#define DATAMANAGER_DATACONTAINERCOLLECTIONBASE_H

#include <QString>
#include <QIODevice>

#include "DataManager/Containers/UserSettings/Include/UserSettingsInterface.h"
#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h"
#include "DataManager/Containers/RCConfiguration/Include/RCConfigurationInterface.h"

namespace Threads {
    class MasterThreadController;
}

//lint -sem(DataManager::CDataContainerCollectionBase::DeinitContainers,cleanup)

namespace DataManager {

/****************************************************************************/
/**
 * \brief CDataContainerCollectionBase definition
 */
/****************************************************************************/
class CDataContainerCollectionBase: public QObject
{
    Q_OBJECT
    friend class TestDataManagerInclude;
private:
    bool m_IsDeInitialised; //!< status of deinitialisation

    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CDataContainerCollectionBase)

protected:
    bool m_IsInitialized;   //!< status of initialisation
    bool InitContainers();
    bool DeinitContainers();

public:
    CDataContainerCollectionBase(Threads::MasterThreadController *p_MasterThreadController);
    virtual ~CDataContainerCollectionBase();

    Threads::MasterThreadController *mp_MasterThreardController; //!< We use it for  broadcasting command

    // prefix mp_ left because members are intentially accessible from outside (public)
    CUserSettingsInterface* SettingsInterface; //!< Provides interface to read the Settings info from xml
    CDeviceConfigurationInterface* DeviceConfigurationInterface; //!< Container for Device configuration
	CRCConfigurationInterface *RCConfigurationInterface; //!< Provides interface to read the RCConfiguration info from xml
    
};

} // namespace DataManager

#endif // DATAMANAGER_DATACONTAINERCOLLECTIONBASE_H


/****************************************************************************/
/*! \file Master/Components/DataManager/Source/DataContainerCollectionBase.cpp
 *
 *  \brief Implementation file for class CDataContainerCollectionBase.
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

#include <QDebug>
#include  "../Include/DataContainerCollectionBase.h"
#include <Threads/Include/MasterThreadController.h>

#include "DataManager/Containers/UserSettings/Include/UserSettingsVerifier.h"
#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationVerifier.h"
#include "DataManager/Containers/RCConfiguration/Include/RCConfigurationInterface.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_MasterThreadController = pointer to master thread controller
 */
/****************************************************************************/
CDataContainerCollectionBase::CDataContainerCollectionBase(Threads::MasterThreadController *p_MasterThreadController) :
    m_IsDeInitialised(false), m_IsInitialized(false),
    mp_MasterThreardController(p_MasterThreadController),
    SettingsInterface(NULL),
    DeviceConfigurationInterface(NULL),
    RCConfigurationInterface(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDataContainerCollectionBase::~CDataContainerCollectionBase()
{
    try {
        if (!m_IsDeInitialised && !DeinitContainers()) {
            qDebug() << "CDataContainer::Destructor / DeinitContainers failed";
        }
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
/*!
 *  \brief Intialise the data containers
 *
 *  \return Success or failure
 */
/****************************************************************************/
bool CDataContainerCollectionBase::InitContainers()
{
    //! m_IsInitialized shall be set to true in derived class implementations of InitContainers.
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::InitContainers was already called";
        return false;
    }

    SettingsInterface = new CUserSettingsInterface();
    DeviceConfigurationInterface = new CDeviceConfigurationInterface();
    RCConfigurationInterface = new CRCConfigurationInterface();
	
    //! \todo create other platform containers here.
   return true;
}

/****************************************************************************/
/*!
 *  \brief De initialise the data containers
 *
 *  \return Success or failure
 */
/****************************************************************************/
bool CDataContainerCollectionBase::DeinitContainers()
{
    delete SettingsInterface;
    delete DeviceConfigurationInterface;
    delete RCConfigurationInterface;
	mp_MasterThreardController = NULL;
    m_IsDeInitialised = true;
    return true;
}


}// namespace DataManager

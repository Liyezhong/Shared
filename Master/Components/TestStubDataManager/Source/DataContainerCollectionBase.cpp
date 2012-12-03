/****************************************************************************/
/*! \file DataContainerCollectionBase.cpp
 *
 *  \brief Implementation file for class CDataContainerCollectionBase.
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

#include <QDebug>
#include  "../Include/DataContainerCollectionBase.h"
#include <Threads/Include/MasterThreadController.h>

#include "TestStubDataManager/Containers/UserSettings/Include/UserSettingsVerifier.h"
#include "TestStubDataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationVerifier.h"

namespace DataManager {

CDataContainerCollectionBase::CDataContainerCollectionBase(Threads::MasterThreadController *p_MasterThreadController) :
    m_IsInitialized(false),
    mp_MasterThreardController(p_MasterThreadController),
    SettingsInterface(NULL),
    DeviceConfigurationInterface(NULL)
{
    if (!InitContainers()) {
        qDebug() << "CDataContainer::Constructor / InitContainers failed";
    }
}

CDataContainerCollectionBase::~CDataContainerCollectionBase()
{
    if (!DeinitContainers()) {
        qDebug() << "CDataContainer::Destructor / DeinitContainers failed";
    }
}

bool CDataContainerCollectionBase::InitContainers()
{
    //! m_IsInitialized shall be set to true in derived class implementations of InitContainers.
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::InitContainers was already called";
        Q_ASSERT(false);
        return false;
    }

    SettingsInterface = new CUserSettingsInterface();
    if (!ResetDCUserSettings()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCSettings failed!";
        return false;
    }


    DeviceConfigurationInterface = new CDeviceConfigurationInterface();
    if (!ResetDCDeviceConfiguration()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCDeviceConfiguration failed.";
        return false;
    }
    //! \todo create other platform containers here.
   return true;
}

bool CDataContainerCollectionBase::DeinitContainers()
{
    if (m_IsInitialized != true) {
        // nothing to do
        return true;
    }
    delete SettingsInterface;
    delete DeviceConfigurationInterface;
    return true;
}



bool CDataContainerCollectionBase::ResetDCUserSettings()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCUserSettings was already called";
        return false;
    }

    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataUserSettingVerifier = NULL;
    if (p_DataUserSettingVerifier == NULL) {
        p_DataUserSettingVerifier = new CUserSettingsVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    SettingsInterface->AddVerifier(p_DataUserSettingVerifier);

    return true;
}


bool CDataContainerCollectionBase::ResetDCDeviceConfiguration()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCDeviceConfiguration was already called";
        return false;
    }

    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataDeviceConfigurationVerifier = NULL;
    if (p_DataDeviceConfigurationVerifier == NULL) {
        p_DataDeviceConfigurationVerifier = new CDeviceConfigurationVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    DeviceConfigurationInterface->AddVerifier(p_DataDeviceConfigurationVerifier);

    return true;
}


}// namespace DataManager

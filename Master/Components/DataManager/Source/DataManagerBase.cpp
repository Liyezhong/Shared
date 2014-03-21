/****************************************************************************/
/*! \file DataManagerBase.cpp
 *
 *  \brief Implementation file for class CDataManagerBase.
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
#include <Global/Include/SystemPaths.h>
#include "../Include/DataManagerBase.h"
#include "DataManager/CommandInterface/Include/UserSettingsCommandInterface.h"
#include <Threads/Include/MasterThreadController.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
namespace DataManager {

CDataManagerBase::CDataManagerBase(Threads::MasterThreadController *p_MasterThreadController)
    : QObject(0)
    , m_IsInitialized(false)
    , mp_DataContainerCollection(NULL)
    , mp_MasterThreadController(p_MasterThreadController)
    , mp_SettingsCommandInterface(NULL)
    , mp_DeviceConfigurationInterface(NULL)
{

}

CDataManagerBase::CDataManagerBase(Threads::MasterThreadController *p_MasterThreadController, CDataContainerCollectionBase *containerCollection)
    : QObject(0)
    , m_IsInitialized(false)
    , mp_DataContainerCollection(containerCollection)
    , mp_MasterThreadController(p_MasterThreadController)
{

}

CDataManagerBase::~CDataManagerBase()
{
    if (!DeinitDataContainer()) {
        qDebug() << "CDataManagerBase::Destructor / DeinitDataContainer failed";
    }
}

//! DataManager InitDataContainer would have been called first, DataContainer is created and initialized there.
quint32 CDataManagerBase::InitDataContainer()
{
    //quint32 ReturnCode = INIT_OK;
    if (m_IsInitialized == true) {
        qDebug() << "CDataManagerBase::InitDataContainer was already called";
        return EVENT_DM_INITIALIZATION_ERROR;
    }

    QString FilenamePUserSettings = Global::SystemPaths::Instance().GetSettingsPath() + "/UserSettings.xml";
    mp_DataContainerCollection->SettingsInterface->SetDataVerificationMode(false);
    if (!mp_DataContainerCollection->SettingsInterface->Read(FilenamePUserSettings)) {
        return EVENT_DM_SETTINGS_XML_READ_FAILED;
    }

    QString FilenameDeviceConfiguration = Global::SystemPaths::Instance().GetSettingsPath() + "/DeviceConfiguration.xml";
    mp_DataContainerCollection->DeviceConfigurationInterface->SetDataVerificationMode(false);
    if (!mp_DataContainerCollection->DeviceConfigurationInterface->Read(FilenameDeviceConfiguration)) {
        return EVENT_DM_DEVICE_CONFIG_XML_READ_FAILED;
    }

    // when all containers are loaded, activate verification mode and verify their initial content
    mp_DataContainerCollection->SettingsInterface->SetDataVerificationMode(true);
    if (!mp_DataContainerCollection->SettingsInterface->VerifyData()) {
        qDebug() << "CDataManagerBase::InitDataContainer failed, because User Settings Verification failed.";
        return EVENT_DM_SETTINGS_VERIFICATION_FAILED;
    }
    mp_DataContainerCollection->DeviceConfigurationInterface->SetDataVerificationMode(true);

    if (!mp_DataContainerCollection->DeviceConfigurationInterface->VerifyData()) {
        qDebug() << "CDataManagerBase::InitDataContainer failed, because mp_DataContainerCollectionBase->DeviceConfigurationInterface->VerifyData failed.";
        return EVENT_DM_DEVICE_CONFIG_VERIFICATION_FAILED;
    }

    //Command interface to handle Settings command
    mp_SettingsCommandInterface = new CUserSettingsCommandInterface(this , mp_MasterThreadController, mp_DataContainerCollection);

    return INIT_OK;
}

bool CDataManagerBase::DeinitDataContainer()
{
    if (m_IsInitialized != true) {
        // nothing to do
        return true;
    }
    delete mp_SettingsCommandInterface;
    return true;
}

CUserSettingsInterface *CDataManagerBase::GetUserSettingsInterface()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManagerBase::GetUserSettingsInterface failed. Not initialized!";
        return NULL;
    }
    return mp_DataContainerCollection->SettingsInterface;
}

CDeviceConfigurationInterface *CDataManagerBase::GetDeviceConfigurationInterface()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManagerBase::GetDeviceConfigurationInterface failed. Not initialized!";
        return NULL;
    }
    return mp_DataContainerCollection->DeviceConfigurationInterface;
}

} // namespace DataManager

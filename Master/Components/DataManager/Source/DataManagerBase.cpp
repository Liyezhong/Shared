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

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_MasterThreadController = pointer to master thread controller
 */
/****************************************************************************/
CDataManagerBase::CDataManagerBase(Threads::MasterThreadController *p_MasterThreadController)
    : QObject(0)
    , m_IsInitialized(false)
    , mp_DataContainerCollectionBase(NULL)
    , mp_MasterThreadController(p_MasterThreadController)
    , mp_SettingsCommandInterface(NULL)
    , mp_DeviceConfigurationInterface(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief Constructor
 *
 *  \iparam p_MasterThreadController = pointer to master thread controller
 *  \iparam p_ContainerCollection = poiner to CDataContainerCollectionBase
 */
/****************************************************************************/
CDataManagerBase::CDataManagerBase(Threads::MasterThreadController *p_MasterThreadController, CDataContainerCollectionBase *p_ContainerCollection)
    : QObject(0)
    , m_IsInitialized(false)
    , mp_DataContainerCollectionBase(p_ContainerCollection)
    , mp_MasterThreadController(p_MasterThreadController)
    , mp_SettingsCommandInterface(NULL)
    , mp_DeviceConfigurationInterface(NULL)

{

}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDataManagerBase::~CDataManagerBase()
{
    try {
        (void)(DeinitDataContainer());
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
//! DataManager InitDataContainer would have been called first, DataContainer is created and initialized there.
quint32 CDataManagerBase::InitDataContainer()
{
    //quint32 ReturnCode = INIT_OK;
    if (m_IsInitialized == true) {
        qDebug() << "CDataManagerBase::InitDataContainer was already called";
        return EVENT_DM_INITIALIZATION_ERROR;
    }

    QString FilenamePUserSettings = Global::SystemPaths::Instance().GetSettingsPath() + "/UserSettings.xml";
    mp_DataContainerCollectionBase->SettingsInterface->SetDataVerificationMode(false);
    if (!mp_DataContainerCollectionBase->SettingsInterface->Read(FilenamePUserSettings)) {
        qDebug() << "CDataManager::InitDataContainer failed, because reading UserSettings failed with filename" << FilenamePUserSettings;
        return EVENT_DM_SETTINGS_XML_READ_FAILED;
    }

    QString FilenameDeviceConfiguration = Global::SystemPaths::Instance().GetSettingsPath() + "/DeviceConfiguration.xml";
    mp_DataContainerCollectionBase->DeviceConfigurationInterface->SetDataVerificationMode(false);
    if (!mp_DataContainerCollectionBase->DeviceConfigurationInterface->Read(FilenameDeviceConfiguration)) {
        qDebug() << "CDataManager::InitDataContainer failed, because reading DeviceConfiguration failed with filename" << FilenameDeviceConfiguration;
        return EVENT_DM_DEVICE_CONFIG_XML_READ_FAILED;
    }

    QString FilenameRCConfigInterface = Global::SystemPaths::Instance().GetSettingsPath() + "/RCConfiguration.xml";
    mp_DataContainerCollectionBase->RCConfigurationInterface->SetDataVerificationMode(false);
    if (!mp_DataContainerCollectionBase->RCConfigurationInterface->Read(FilenameRCConfigInterface)) {
        qDebug() << "CDataManager::InitDataContainer failed, because reading RCConfiguration failed with filename" << FilenameRCConfigInterface;
        return EVENT_DM_RCCONFIG_XML_READ_FAILED;
    }
    // when all containers are loaded, activate verification mode and verify their initial content
    mp_DataContainerCollectionBase->SettingsInterface->SetDataVerificationMode(true);
    if (!mp_DataContainerCollectionBase->SettingsInterface->VerifyData()) {
        qDebug() << "CDataManagerBase::InitDataContainer failed, because User Settings Verification failed.";
        return EVENT_DM_SETTINGS_VERIFICATION_FAILED;
    }
    mp_DataContainerCollectionBase->DeviceConfigurationInterface->SetDataVerificationMode(true);

    if (!mp_DataContainerCollectionBase->DeviceConfigurationInterface->VerifyData()) {
        qDebug() << "CDataManagerBase::InitDataContainer failed, because mp_DataContainerCollection->DeviceConfigurationInterface->VerifyData failed.";
        return EVENT_DM_DEVICE_CONFIG_VERIFICATION_FAILED;
    }

    mp_DataContainerCollectionBase->RCConfigurationInterface->SetDataVerificationMode(true);
    if (!mp_DataContainerCollectionBase->RCConfigurationInterface->VerifyData()) {
        qDebug() << "CDataManagerBase::InitDataContainer failed, because RCConfgurationInterface->VerifyData failed.";
        return EVENT_DM_RCCONFIG_VERIFICATION_FAILED;
    }
    //Command interface to handle Settings command
    /*lint -e64 */
    mp_SettingsCommandInterface = new CUserSettingsCommandInterface(this , mp_MasterThreadController, mp_DataContainerCollectionBase);
    return INIT_OK;
}

/****************************************************************************/
/*!
 *  \brief De initialise the data containers
 *
 *  \return Success or failure
 */
/****************************************************************************/
bool CDataManagerBase::DeinitDataContainer()
{
    if (m_IsInitialized != true) {
        // nothing to do
        return true;
    }

    mp_DeviceConfigurationInterface = NULL;
    mp_MasterThreadController = NULL;
    mp_DataContainerCollectionBase = NULL;
    /*lint -e1404 */
    delete mp_SettingsCommandInterface;
    return true;
}

/****************************************************************************/
/*!
 *  \brief get user seing inerface pointer
 *
 *  \return pointer to user settings
 */
/****************************************************************************/
CUserSettingsInterface *CDataManagerBase::GetUserSettingsInterface()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManagerBase::GetUserSettingsInterface failed. Not initialized!";
        return NULL;
    }

    if(mp_DataContainerCollectionBase) {
        return mp_DataContainerCollectionBase->SettingsInterface;
    }
    else {
        return NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief get copy of device cofiguration
 *
 *  \return pointer to device configuration
 */
/****************************************************************************/
CDeviceConfigurationInterface *CDataManagerBase::GetDeviceConfigurationInterface()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManagerBase::GetDeviceConfigurationInterface failed. Not initialized!";
        return NULL;
    }

    if(mp_DataContainerCollectionBase) {
        return mp_DataContainerCollectionBase->DeviceConfigurationInterface;
    }
    else {
        return NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief get rc configutation inerface pointer
 *
 *  \return pointer to rc configutation
 */
/****************************************************************************/
CRCConfigurationInterface *CDataManagerBase::GetRCConfigurationInterface()
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManagerBase::GetRCConfigurationInterface failed. Not initialized!";
        return NULL;
    }

    if(mp_DataContainerCollectionBase) {
        return mp_DataContainerCollectionBase->RCConfigurationInterface;
    }
    else {
        return NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief saves all the files modification before shutting down
 */
/****************************************************************************/
void CDataManagerBase::SaveDataOnShutdown()
{
    if (m_IsInitialized == true) {
        // save the data of all read-write XML files, leave the read-only xml file DeviceConfiguration
        if(!mp_DataContainerCollectionBase->SettingsInterface->Write(mp_DataContainerCollectionBase->SettingsInterface->GetFilename())){
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_SHUTDOWN_XML_WRITE_FAILED,
                                                       Global::tTranslatableStringList() << "UserSettings.xml",
                                                       true);
        }

        // save the data of all read-write XML files
        if(!mp_DataContainerCollectionBase->RCConfigurationInterface->Write(mp_DataContainerCollectionBase->RCConfigurationInterface->GetFilename())){
            Global::EventObject::Instance().RaiseEvent(EVENT_DM_SHUTDOWN_XML_WRITE_FAILED,
                                                       Global::tTranslatableStringList() << "RCConfiguration.xml",
                                                       true);
        }

    }
}
} // namespace DataManager

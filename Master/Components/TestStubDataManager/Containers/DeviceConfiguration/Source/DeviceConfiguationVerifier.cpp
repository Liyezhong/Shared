/****************************************************************************/
/*! \file DeviceConfigurationVerifier.cpp
 *
 *  \brief DeviceConfigurationVerifier class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-09-04
 *   $Author:  $ Ningu
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
#include <QFile>

#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationVerifier.h"
#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfiguration.h"



namespace DataManager {

const int MIN_SERIAL_CHAR_COUNT             = 1;        ///< Minimum character width of device serial number
const int MAX_SERIAL_CHAR_COUNT             = 32;       ///< Maximum character width of device serial number
const int MIN_STAINER_DEVNAME_CHAR_COUNT    = 1;        ///< Minimum character width of device name
const int MAX_STAINER_DEVNAME_CHAR_COUNT    = 32;       ///< Maximum character width of device name
const int MIN_CSLIPPER_DEVNAME_CHAR_COUNT   = 1;        ///< Minimum character width of coverslipper name
const int MAX_CSLIPPER_DEVNAME_CHAR_COUNT   = 32;       ///< Maximum character width of coverslipper name
const bool MODULE_AVAILABILITY_YES          = true;     ///< Module availability
const bool MODULE_AVAILABILITY_NO           = false;    ///< Module Non-Availability
/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CDeviceConfigurationVerifier::CDeviceConfigurationVerifier() : mp_DeviceConfigInterface(NULL)
{

}

/****************************************************************************/
/*!
 *  \brief  Verifies Device Configuration
 *
 *  \iparam p_DeviceConfigInterface = Device Configuration interface class object
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDeviceConfigurationVerifier::VerifyData(CDataContainerBase* p_DeviceConfigInterface)
{
    // assign pointer to member variable
    mp_DeviceConfigInterface = static_cast<CDeviceConfigurationInterface*>(p_DeviceConfigInterface);

    // check content of Device Configuration
    CDeviceConfiguration* DeviceConfig = mp_DeviceConfigInterface->GetDeviceConfiguration();


    // check the language
    QStringList LanguageList;
    QString Languages;
    LanguageList.clear();
    LanguageList = DeviceConfig->GetLanguageList();
    if(!LanguageList.isEmpty()) {
        for (qint32 I = 0; I < LanguageList.count(); I++) {
           Languages = LanguageList.at(I);
           //Need to implement a method to verify the language list
        }
         //need to return false once the language list verification method is freezed
    }

    // check the error for device name
    if (!((DeviceConfig->GetValue("DeviceName").count() >= MIN_STAINER_DEVNAME_CHAR_COUNT) && (DeviceConfig->GetValue("DeviceName").count() <= MAX_STAINER_DEVNAME_CHAR_COUNT))) {
         qDebug() << "Stainer device name width is not proper";
        return false;
    }

    // check the error for serial number
    if (!((DeviceConfig->GetValue("SerialNumber").count() >= MIN_SERIAL_CHAR_COUNT) && (DeviceConfig->GetValue("SerialNumber").count() <= MAX_SERIAL_CHAR_COUNT))) {
         qDebug() << "Stainer serial number width is not proper";
        return false;
    }

    // todo: generalize the following checks
    if (DeviceConfig->GetValue("DeviceName") == "ST 8020")
    {
        // check the error for Coverslipper device name
        if (!((DeviceConfig->GetValue("CoverslipperName").count() >= MIN_CSLIPPER_DEVNAME_CHAR_COUNT) && (DeviceConfig->GetValue("CoverslipperName").count() <= MAX_CSLIPPER_DEVNAME_CHAR_COUNT))) {
             qDebug() << "Coverslipper device name width is not proper";
            return false;
        }

        switch(DeviceConfig->GetBoolValue("WorkstationMode")) {
            case MODULE_AVAILABILITY_YES:
            case MODULE_AVAILABILITY_NO:
            break;
        default:
            qDebug() << "Workstation mode data is not proper";
           return false;
        }

        switch(DeviceConfig->GetBoolValue("HeatedCuevettes")) {
            case MODULE_AVAILABILITY_YES:
            case MODULE_AVAILABILITY_NO:
            break;
        default:
            qDebug() << "Heated cuevettes mode is not proper";
           return false;
        }

        switch(DeviceConfig->GetBoolValue("SlideIdCamera")) {
            case MODULE_AVAILABILITY_YES:
            case MODULE_AVAILABILITY_NO:
            break;
        default:
            qDebug() << "Camera slide id availability data is not proper";
           return false;
        }
    }

    if (DeviceConfig->GetValue("DeviceName") == "CV 8020")
    {
        // check the error for Stainer device name
        if (!((DeviceConfig->GetValue("StainerName").count() >= MIN_CSLIPPER_DEVNAME_CHAR_COUNT) && (DeviceConfig->GetValue("StainerName").count() <= MAX_CSLIPPER_DEVNAME_CHAR_COUNT))) {
             qDebug() << "Stainer device name width is not proper";
            return false;
        }

        switch(DeviceConfig->GetBoolValue("WorkstationMode")) {
            case MODULE_AVAILABILITY_YES:
            case MODULE_AVAILABILITY_NO:
            break;
        default:
            qDebug() << "Workstation mode data is not proper";
           return false;
        }
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief  Get the synchronous object
 *
 *  \iparam p_ReadWriteLock = Lock for the threads
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CDeviceConfigurationVerifier::GetSyncObject(QReadWriteLock* p_ReadWriteLock)
{
    p_ReadWriteLock = NULL;
    return true;
}

/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorHash_t& CDeviceConfigurationVerifier::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorsHash;
}

/****************************************************************************/
/*!
 *  \brief  Resets the last error which is done by verifier
 */
/****************************************************************************/
void CDeviceConfigurationVerifier::ResetLastErrors()
{
    m_ErrorsHash.clear();
}

/****************************************************************************/
/*!
 *  \brief  Returns whether verifier is a local verifier or not
 *
 *  \return bool - On successful (True) otherwise (False)
 */
/****************************************************************************/
bool CDeviceConfigurationVerifier::IsLocalVerifier()
{
    return true;
}

}  // namespace DataManager



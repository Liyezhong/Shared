/****************************************************************************/
/*! \file TestStubDataManager/Containers/DeviceConfiguration/Include/DeviceConfigurationInterface.h
 *
 *  \brief DeviceConfigurationInterface definition.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-04
 *  $Author:    $ Ningu
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
#ifndef DATAMANAGER_DEVICECONFIGURATIONINTERFACE_H
#define DATAMANAGER_DEVICECONFIGURATIONINTERFACE_H

#include <QString>
#include <QReadWriteLock>
#include <QLocale>
#include <QXmlStreamReader>

#include "TestStubDataManager/Containers/DeviceConfiguration/Include/DeviceConfiguration.h"
#include "TestStubDataManager/Containers/ContainerBase/Include/VerifierInterface.h"
#include "TestStubDataManager/Containers/ContainerBase/Include/DataContainerBase.h"

namespace DataManager {

/****************************************************************************/
/**
 * \brief Class for reading / writing XML based configuration file for Device Configuration.
 *
 * <b>This class isl thread safe.</b>
 */
/****************************************************************************/
class CDeviceConfigurationInterface : public CDataContainerBase
{
private:
    QReadWriteLock*             mp_ReadWriteLock;       ///< Synchronisation object.
    bool                        m_DataVerificationMode; ///< Store the Data verification mode flag
    CDeviceConfiguration*       mp_DeviceConfig;        ///< Store the Device Configurationl settings
    QString                     m_FileName;             ///< Store the file name    
    ErrorHash_t                 m_ErrorHash;            //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

    /****************************************************************************/


    bool SerializeContent(QIODevice& p_Device, bool CompleteData);
    bool DeserializeContent(QIODevice& p_Device, bool CompleteData);

protected:
public:    
    CDeviceConfigurationInterface();
    CDeviceConfigurationInterface(const CDeviceConfigurationInterface &);

    ~CDeviceConfigurationInterface();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CDeviceConfigurationInterface& DCInterface);
    friend QDataStream& operator >>(QDataStream& InDataStream, CDeviceConfigurationInterface& DCInterface);

    CDeviceConfigurationInterface & operator = (const CDeviceConfigurationInterface &);

    void SetDefaultAttributes();

    /****************************************************************************/
    /*!
     *  \brief Get the container type
     *
     *  \return True or False
     */
    /****************************************************************************/
    DataContainerType_t GetDataContainerType() { return DEVICECONFIGURATION; }

    /****************************************************************************/
    /*!
     *  \brief Get the Data verification mode flag
     *
     *  \return True or False
     */
    /****************************************************************************/
    bool GetDataVerificationMode() { return m_DataVerificationMode; }

    /****************************************************************************/
    /*!
     *  \brief Set the Data verification mode flag value
     *
     *  \iparam Value = flag value
     */
    /****************************************************************************/
    void SetDataVerificationMode(const bool Value) { m_DataVerificationMode = Value; }

    bool Read(QString FileName);

    bool UpdateDeviceConfiguration(const CDeviceConfiguration* p_DeviceConfig);

    CDeviceConfiguration* GetDeviceConfiguration(bool CopyConfiguration);

    /****************************************************************************/
    /*!
     *  \brief Get the Device Configuration pointer
     *
     *  \return mp_DeviceConfig pointer
     */
    /****************************************************************************/
    CDeviceConfiguration* GetDeviceConfiguration()
    {
        if (mp_DeviceConfig) {
            return mp_DeviceConfig;
        }
        else {
            return NULL;
        }
    }

    /****************************************************************************/
    /*!
     *  \brief Get's the version of the Device Configuration File
     *
     *  \return Version Number
     */
    /****************************************************************************/
    QString GetVersion()
    {
        // check whether Device Configuration exists or not
        QString Version = "";
        if (mp_DeviceConfig) {
            Version = mp_DeviceConfig->GetVersion();
        }
        return Version;
    }
    /****************************************************************************/
    /*!
     *  \brief Get's the Language of the Device Configuration
     *
     *  \return Language string
     */
    /****************************************************************************/
    QStringList GetLanguageList()
    {
        // check whether Device Configuration exists or not        
        if (mp_DeviceConfig) {
            return mp_DeviceConfig->GetLanguageList();
        }
    }

    /****************************************************************************/
    /*!
     *  \brief Get's the fileName of the Device Configuration
     *
     *  \return File name String
     */
    /****************************************************************************/
    QString GetFilename() {return m_FileName;}


}; // end class CDeviceConfigurationInterface

} // end namespace DataManager


#endif // DEVICECONFIGURATIONINTERFACE_H

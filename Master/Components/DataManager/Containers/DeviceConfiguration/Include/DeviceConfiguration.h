/****************************************************************************/
/*! \file DeviceConfiguration.h
 *
 *  \brief Definition file for class CDeviceConfiguration.
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

#ifndef DATAMANAGER_DEVICECONFIGURATION_H
#define DATAMANAGER_DEVICECONFIGURATION_H

#include <QString>
#include <QReadWriteLock>
#include <QLocale>
#include <QXmlStreamReader>

#include "Global/Include/GlobalDefines.h"
#include "Global/Include/GlobalEventCodes.h"
#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Helper/Include/Types.h"
#include "DataManager/Helper/Include/Helper.h"
#include "DataManager/Containers/ContainerBase/Include/VerifierInterface.h"


namespace DataManager {

/****************************************************************************/
/**
 * \brief Class for reading / writing XML based configuration file for Device Configuration.
 *
 * <b>This class is not thread safe.</b>
 */
/****************************************************************************/
class CDeviceConfiguration {
    friend class CDeviceConfigurationInterface;
private:
    QHash<QString, QString> m_ValueList;
    QString             m_Version;                      ///< Store the version number of the file    
//    QString             m_StainerDeviceName;            ///< Store stainer device name.
//    QString             m_StainerSerialNumber;          ///< Store stainer serial number.
//    QString             m_CoverSlipperDeviceName;       ///< Store coverslipper device name
    QStringList         m_LanguageList;                 ///< Store Language list
//    bool                m_WorkStation;                  ///< Defines if the device is a workstation
//    bool                m_HeatedCuevettesAvailable;     ///< Defines whether the heated cuvettes is available
//    bool                m_CameraSlideIdAvailable;       ///< Defines whether the BarCode Camera is available
    ErrorHash_t         m_ErrorHash;                     //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

    /****************************************************************************/

    bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);
    bool ReadCompleteData(QXmlStreamReader& XmlStreamReader);

    /****************************************************************************/
    /*!
     *  \brief Set the version number of the Xml config file
     *
     *  \iparam Version = version number
     */
    /****l************************************************************************/
    void SetVersion(const QString Version)
    {
       m_Version = Version;
    }

protected:
public:

    CDeviceConfiguration();
    CDeviceConfiguration(const CDeviceConfiguration &);

    ~CDeviceConfiguration();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CDeviceConfiguration& DeviceConfig);
    friend QDataStream& operator >>(QDataStream& InDataStream, CDeviceConfiguration& DeviceConfig);
    CDeviceConfiguration & operator = (const CDeviceConfiguration & DeviceConfig);

    void SetDefaultAttributes();

    /****************************************************************************/
    /*!
     *  \brief Get the version number
     *
     *  \return version number
     */
    /****************************************************************************/
    QString GetVersion() const
    {
       return m_Version;
    }    
    /****************************************************************************/
    /*!
     *  \brief Retrieves the language list
     *
     *  \return Language list
     */
    /****************************************************************************/
    QStringList GetLanguageList() const
    {
        return m_LanguageList;
    }
    /****************************************************************************/
    /*!
     *  \brief Sets the list of Languages
     *
     *  \iparam LanguageList = List of Station IDs
     */
    /****************************************************************************/
    void SetLanguageList(const QStringList LanguageList)
    {
        m_LanguageList = LanguageList;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the configuration value related to key
     *
     *  \return Value
     */
    /****************************************************************************/
    void SetValue(QString key, QString value)
    {
        m_ValueList.insert(key.toUpper(), value);
    }

    /****************************************************************************/
    /*!
     *  \brief Get the configuration value related to key
     *
     *  \return Value
     */
    /****************************************************************************/
    void SetValue(QString key, bool value)
    {
        m_ValueList.insert(key.toUpper(), value ? "true" : "false");
    }

    /****************************************************************************/
    /*!
     *  \brief Get the configuration value related to key
     *
     *  \return Value
     */
    /****************************************************************************/
    QString GetValue(QString key) const
    {
        return m_ValueList.value(key.toUpper(), "");
    }

    /****************************************************************************/
    /*!
     *  \brief Get the configuration value related to key
     *
     *  \return Value
     */
    /****************************************************************************/
    bool GetBoolValue(QString key) const
    {
        return (m_ValueList.value(key.toUpper(), "").toUpper() == "YES") ? true : false;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the Attribute value related to key
     *
     *  \return Value
     */
    /****************************************************************************/
    quint32 GetAttributeValue(QString key) const
    {
        QString BoolVal = m_ValueList.value(key.toUpper(), "").toUpper();
        if (BoolVal == "NO") {
            return 0;
        }
        else if (BoolVal == "YES") {
            return 1;
        }
        else {
            return 2;
        }
    }
    /****************************************************************************/

//    /*!
//     *  \brief Get the stainer device name
//     *
//     *  \return Stainer device name
//     */
//    /****************************************************************************/
//    QString GetStainerDeviceName() const
//    {
//       return m_StainerDeviceName;
//    }
//    /****************************************************************************/
//    /*!
//     *  \brief Set the stainer device name
//     *
//     *  \iparam StainerDeviceName = stainer device name
//     */
//    /****************************************************************************/
//    void SetStainerDeviceName(const QString StainerDeviceName)
//    {
//        m_StainerDeviceName = StainerDeviceName;
//    }
//    /****************************************************************************/
//    /*!
//     *  \brief Get the stainer serial number
//     *
//     *  \return Stainer serial number
//     */
//    /****************************************************************************/
//    QString GetStainerSerialNumber() const
//    {
//       return m_StainerSerialNumber;
//    }

//    /****************************************************************************/
//    /*!
//     *  \brief Set the stainer serial number
//     *
//     *  \iparam StainerSerialNumber = stainer serial number
//     */
//    /****************************************************************************/
//    void SetStainerSerialNumber(const QString StainerSerialNumber)
//    {
//        m_StainerSerialNumber = StainerSerialNumber;
//    }
//    /****************************************************************************/
//    /*!
//     *  \brief Get the coverslipper device name
//     *
//     *  \return coverslipper device name
//     */
//    /****************************************************************************/
//    QString GetCoverSlipperDeviceName() const
//    {
//       return m_CoverSlipperDeviceName;
//    }

//    /****************************************************************************/
//    /*!
//     *  \brief Set coverslipper device name
//     *
//     *  \iparam CoverSlipperDeviceName = coverslipper device name
//     */
//    /****************************************************************************/
//    void SetCoverSlipperDeviceName(const QString CoverSlipperDeviceName)
//    {
//        m_CoverSlipperDeviceName = CoverSlipperDeviceName;
//    }

//    /****************************************************************************/
//    /*!
//     *  \brief Get the device type flag(workstation = 0 or standalone = 1)
//     *
//     *  \return returns device type flag
//     */
//    /****************************************************************************/
//    bool GetWorkStation() const
//    {
//       return m_WorkStation;
//    }

//    /****************************************************************************/
//    /*!
//     *  \brief Set the device type flag (workstation = 0 or standalone = 1)
//     *
//     *  \iparam WorkStation = device type flag
//     */
//    /****************************************************************************/
//    void SetWorkStation(bool WorkStation)
//    {

//        m_WorkStation = WorkStation;
//    }
//    /****************************************************************************/
//    /*!
//     *  \brief Get the heated cuvettes availability flag
//     *
//     *  \return returns heated cuvettes availability flag
//     */
//    /****************************************************************************/
//    bool GetHeatedCuevettesAvailable() const
//    {
//       return m_HeatedCuevettesAvailable;
//    }

//    /****************************************************************************/
//    /*!
//     *  \brief Set the heated cuvettes availability flag
//     *
//     *  \iparam HeatedCuevettesAvailable = heated cuvettes availability flag
//     */
//    /****************************************************************************/
//    void SetHeatedCuevettesAvailable(bool HeatedCuevettesAvailable)
//    {

//        m_HeatedCuevettesAvailable = HeatedCuevettesAvailable;
//    }
//    /****************************************************************************/
//    /*!
//     *  \brief Get the BarCode Camera availability flag
//     *
//     *  \return returns BarCode Camera availability flag
//     */
//    /****************************************************************************/
//    bool GetCameraSlideIdAvailable() const
//    {
//       return m_CameraSlideIdAvailable;
//    }

//    /****************************************************************************/
//    /*!
//     *  \brief Set the BarCode Camera availability flag
//     *
//     *  \iparam CameraSlideIdAvailable = BarCode Camera availability flag
//     */
//    /****************************************************************************/
//    void SetCameraSlideIdAvailable(bool CameraSlideIdAvailable)
//    {

//        m_CameraSlideIdAvailable = CameraSlideIdAvailable;
//    }

}; // end class CDeviceConfiguration

} // end namespace DataManager

#endif // DATAMANAGEMENT_CDeviceConfiguration_H

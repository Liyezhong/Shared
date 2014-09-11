/****************************************************************************/
/*! \file Components/DataManager/Containers/DeviceConfiguration/Include/DeviceConfiguration.h
 *
 *  \brief Definition file for class CDeviceConfiguration.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-09-04
 *  $Author:    $ Ningu, Ramya GJ
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
    QMap<QString, QString> m_ValueList;                 ///< Store the values
    QString             m_Version;                      ///< Store the version number of the file
    QStringList         m_LanguageList;                 ///< Store Language list
    ErrorMap_t         m_ErrorMap;                     //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator

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
    void CopyFromOther(const CDeviceConfiguration &DeviceConfiguration);

    ~CDeviceConfiguration();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CDeviceConfiguration& DeviceConfig);
    friend QDataStream& operator >>(QDataStream& InDataStream, CDeviceConfiguration& DeviceConfig);
    CDeviceConfiguration & operator = (const CDeviceConfiguration & DeviceConfig);

    void SetDefaultAttributes();
    QMap<QString ,QString> GetValueList() const;

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
     *  \iparam key
     *  \iparam value
     *
     *  \return void
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
     *  \iparam key
     *  \iparam value
     *
     *  \return void
     */
    /****************************************************************************/
    void SetValue(QString key, bool value)
    {
        m_ValueList.insert(key.toUpper(), value ? "Yes" : "No");
    }

    /****************************************************************************/
    /*!
     *  \brief Get the configuration value related to key
     *
     *  \iparam key
     *
     *  \return string value
     */
    /****************************************************************************/
    QString GetValue(QString key) const
    {

        QString sn = m_ValueList.value(key.toUpper(), "");
        if(key.toUpper().compare("SERIALNUMBER") == 0){
            sn.remove(QRegExp("\\W"));
        }
        else if(key.toUpper().compare("DEVICENAME") == 0){
            sn.remove(QRegExp("\\s"));
        }

        return sn;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the serial number value
     *
     *  \return string of serial number.
     */
    /****************************************************************************/
    QString GetSerialNumber() const
    {
        return m_ValueList.value("SERIALNUMBER");
    }

    /****************************************************************************/
    /*!
     *  \brief Get the configuration value related to key
     *
     *  \iparam key
     *
     *  \return bool value
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
     *  \iparam key = The attribute
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
    /*!
     *  \brief Get the stainer device name
     *
     *  \iparam key
     *
     *  \return Stainer device name
     */
    /****************************************************************************/
    QString GetStainerDeviceName(QString key) const
    {
        QString DeviceName = m_ValueList.value(key, "");
        return DeviceName;
    }

}; // end class CDeviceConfiguration

} // end namespace DataManager

#endif // DATAMANAGEMENT_CDeviceConfiguration_H

/****************************************************************************/
/*! \file Components/DataManager/Containers/UserSettings/Include/UserSettings.h
 *
 *  \brief Definition file for class CUserSettings.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-04-23
 *  $Author:    $ Raju123, Ramya GJ
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

#ifndef DATAMANAGER_CUSERSETTINGS_H
#define DATAMANAGER_CUSERSETTINGS_H

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

//lint -sem(AddCorrectionModuleInfo,custodial(1))

namespace DataManager {

//!< Structure for Correction Module list.
typedef struct {
    QString ModuleId;  //!< name of the Module for Correction
    QString ModuleLength40; //!< Module value for Correction
    QString ModuleLength50; //!< Module value for Correction
    QString ModuleLength55; //!< Module value for Correction
    QString ModuleLength60; //!< Module value for Correction
} CorrectionModule_t;  //!< structure variable or Module
class CUserSettings;
typedef QList<QString> ListOfModuleNames_t;       //!< QList of ModuleNames.
typedef QHash<QString, CorrectionModule_t> ListOfCorrectionModules_t;    //!< QHash for List of Modules.

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function ERROR_TONE_PERIODIC_TIME
 *
 *  \return from ERROR_TONE_PERIODIC_TIME
 */
/****************************************************************************/
const QString ERROR_TONE_PERIODIC_TIME("ErrorTone_PeriodicTime");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function WARNING_TONE_PERIODIC
 *
 *  \return from WARNING_TONE_PERIODIC
 */
/****************************************************************************/
const QString WARNING_TONE_PERIODIC("WarningTone_Periodic");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function WARNING_TONE_PERIODIC_TIME
 *
 *  \return from WARNING_TONE_PERIODIC_TIME
 */
/****************************************************************************/
const QString WARNING_TONE_PERIODIC_TIME("WarningTone_PeriodicTime");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function INFORMATION_TONE_NUMBER
 *
 *  \return from INFORMATION_TONE_NUMBER
 */
/****************************************************************************/
const QString INFORMATION_TONE_NUMBER("InformationTone_Number");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function INFORMATION_TONE_LEVEL
 *
 *  \return from INFORMATION_TONE_LEVEL
 */
/****************************************************************************/
const QString INFORMATION_TONE_LEVEL("InformationTone_Level");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function INFORMATION_TONE_PERIODIC
 *
 *  \return from INFORMATION_TONE_PERIODIC
 */
/****************************************************************************/
const QString INFORMATION_TONE_PERIODIC("InformationTone_Periodic");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function INFORMATION_TONE_PERIODIC_TIME
 *
 *  \return from INFORMATION_TONE_PERIODIC_TIME
 */
/****************************************************************************/
const QString INFORMATION_TONE_PERIODIC_TIME("InformationTone_PeriodicTime");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function PARAFFIN_BATH_TEMPERATURE
 *
 *  \return from PARAFFIN_BATH_TEMPERATURE
 */
/****************************************************************************/
const QString PARAFFIN_BATH_TEMPERATURE("ParaffinBath_Temperature");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function PV_FREQUENCY
 *
 *  \return from PV_FREQUENCY
 */
/****************************************************************************/
const QString PV_FREQUENCY("PV_Frequency");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function RMS_PROCESSING_MODE
 *
 *  \return from RMS_PROCESSING_MODE
 */
/****************************************************************************/
const QString RMS_PROCESSING_MODE("RMS_ProcessingMode");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function RMS_CLEANING_MODE
 *
 *  \return from RMS_CLEANING_MODE
 */
/****************************************************************************/
const QString RMS_CLEANING_MODE("RMS_CleaningMode");

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function PERIODIC_ON
 *
 *  \return from PERIODIC_ON
 */
/****************************************************************************/
const QString PERIODIC_ON("on");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function PERIODIC_OFF
 *
 *  \return from PERIODIC_OFF
 */
/****************************************************************************/
const QString PERIODIC_OFF("off");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function RMS_CASSETTES_STRING
 *
 *  \return from RMS_CASSETTES_STRING
 */
/****************************************************************************/
const QString RMS_CASSETTES_STRING("cassettes");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function RMS_CYCLES_STRING
 *
 *  \return from RMS_CYCLES_STRING
 */
/****************************************************************************/
const QString RMS_CYCLES_STRING("cycles");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function RMS_DAYS_STRING
 *
 *  \return from RMS_DAYS_STRING
 */
/****************************************************************************/
const QString RMS_DAYS_STRING("days");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function RMS_OFF_STRING
 *
 *  \return from RMS_OFF_STRING
 */
/****************************************************************************/
const QString RMS_OFF_STRING("off");

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function CASSETTE_COUNT
 *
 *  \return from CASSETTE_COUNT
 */
/****************************************************************************/
const QString CASSETTE_COUNT("CassetteCount_Number");//need the format with "_"
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function REAGENTID_OF_LASTSTEP
 *
 *  \return from REAGENTID_OF_LASTSTEP
 */
/****************************************************************************/
const QString REAGENTID_OF_LASTSTEP("ReagentIdOfLastStep_ID");

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function SERVICE_OPERATION_HOURS
 *
 *  \return from SERVICE_OPERATION_HOURS
 */
/****************************************************************************/
const QString SERVICE_OPERATION_HOURS("Service_OperationHours");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function SERVICE_OPERATION_LAST_RESET_DATE
 *
 *  \return from SERVICE_OPERATION_LAST_RESET_DATE
 */
/****************************************************************************/
const QString SERVICE_OPERATION_LAST_RESET_DATE("Service_OperationLastResetDate");

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function SERVICE_ACTIVE_CARBON_HOURS
 *
 *  \return from SERVICE_ACTIVE_CARBON_HOURS
 */
/****************************************************************************/
const QString SERVICE_ACTIVE_CARBON_HOURS("Service_ActiveCarbonHours");
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function SERVICE_ACTIVE_CARBON_LAST_RESET_DATE
 *
 *  \return from SERVICE_ACTIVE_CARBON_LAST_RESET_DATE
 */
/****************************************************************************/
const QString SERVICE_ACTIVE_CARBON_LAST_RESET_DATE("Service_ActiveCarbonLastResetDate");

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function SERVICE_USE_EXHAUST_SYSTEM
 *
 *  \return from SERVICE_USE_EXHAUST_SYSTEM
 */
/****************************************************************************/
const QString SERVICE_USE_EXHAUST_SYSTEM("Service_UseExhaustSystem");

/****************************************************************************/
/**
 * \brief Class for reading / writing XML based configuration file for user settings.
 *
 * <b>This class is not thread safe.</b>
 */
/****************************************************************************/
class CUserSettings {
    friend class CUserSettingsInterface;
private:
    int                         m_Version;              ///< Store the version number of the file
    QLocale::Language           m_Language;             ///< Language.
    Global::DateFormat          m_DateFormat;           ///< Format for date.
    Global::TimeFormat          m_TimeFormat;           ///< Format for time.
    Global::TemperatureFormat   m_TemperatureFormat;    ///< Temperature format.
    int                         m_SoundNumberError;     ///< Sound number for errors.
    int                         m_SoundLevelError;      ///< Sound level for errors.
    int                         m_SoundNumberWarning;   ///< Sound number for warnings.
    int                         m_SoundLevelWarning;    ///< Sound level for warnings.
    ErrorMap_t                  m_ErrorMap;             //!< Event List for GUI and for logging purpose. This member is not copied when using copy constructor/Assignment operator
    Global::OnOffState          m_RemoteCare;           ///< True(On) if RemoteCare software is used else False(Off).
    Global::OnOffState          m_DirectConnection;     ///< True(On) if the device is directly connected esle False(Off).
    QString                     m_ProxyUserName;        ///< ProxyUserName
    QString                     m_ProxyPassword;        ///< ProxyPassword
    QString                     m_ProxyIPAddress;       ///< ProxyIPAddress
    int                         m_ProxyIPPort;          ///< ProxyIPPort
    QMap<QString, QString>      m_ValueList; //!< User Settings Map
    /****************************************************************************/

    virtual bool SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData);
    virtual bool DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData);

    /****************************************************************************/
    /*!
     *  \brief Set the version number of the Xml config file
     *
     *  \iparam Value = version number
     */
    /****************************************************************************/
    void SetVersion(int Value)
    {
        m_Version = Value;
    }

protected:
    bool ReadSoundSettings(QXmlStreamReader& XmlStreamReader);
    bool ReadLocalization(QXmlStreamReader& XmlStreamReader);
    bool ReadNetworkSettings(QXmlStreamReader& XmlStreamReader);
    bool ReadCorrectionSettings(QXmlStreamReader& XmlStreamReader);
public:

    CUserSettings();
    CUserSettings(const CUserSettings &);
    void CopyFromOther(const CUserSettings &UserSettings);

    virtual ~CUserSettings();

    friend QDataStream& operator <<(QDataStream& OutDataStream, const CUserSettings& UserSettings);
    friend QDataStream& operator >>(QDataStream& InDataStream, CUserSettings& UserSettings);
    CUserSettings & operator = (const CUserSettings &);
    ListOfCorrectionModules_t             m_ListOfCorrectionModules;      //!< Module Information in QHash
    ListOfModuleNames_t m_ModuleNames;    //!< List of Module Name;

    void SetDefaultAttributes();

    ErrorMap_t &GetErrors();
    QMap<QString ,QString> GetValueList() const;

    bool GetSoundPeriodicWarning(void) const;
    int GetSoundPeriodicTimeWarning(void) const;


    /****************************************************************************/
    /*!
     *  \brief Get the version number
     *
     *  \return version number
     */
    /****************************************************************************/
    int GetVersion() const
    {
        return m_Version;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the laguage name
     *
     *  \return language name
     */
    /****************************************************************************/
    QLocale::Language GetLanguage() const
    {
        return m_Language;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the language
     *
     *  \iparam TheLanguage = language name
     */
    /****************************************************************************/
    void SetLanguage(QLocale::Language TheLanguage)
    {
        m_Language = TheLanguage;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the Date format
     *
     *  \return Date format
     */
    /****************************************************************************/
    Global::DateFormat GetDateFormat() const
    {
        return m_DateFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Date format
     *
     *  \iparam TheDateFormat = date format
     */
    /****************************************************************************/
    void SetDateFormat(Global::DateFormat TheDateFormat)
    {
        m_DateFormat = TheDateFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the time format
     *
     *  \return time format
     */
    /****************************************************************************/
    Global::TimeFormat GetTimeFormat() const
    {
        return m_TimeFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the time format
     *
     *  \iparam TheTimeFormat = time format
     */
    /****************************************************************************/
    void SetTimeFormat(Global::TimeFormat TheTimeFormat)
    {
        m_TimeFormat = TheTimeFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the temperature format
     *
     *  \return temperature format
     */
    /****************************************************************************/
    Global::TemperatureFormat GetTemperatureFormat() const
    {
        return m_TemperatureFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the temperature format
     *
     *  \iparam TheFormat = temperature format
     */
    /****************************************************************************/
    void SetTemperatureFormat(Global::TemperatureFormat TheFormat)
    {
        m_TemperatureFormat = TheFormat;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the sound number error
     *
     *  \return sound number error
     */
    /****************************************************************************/
    int GetSoundNumberError() const
    {
        return m_SoundNumberError;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the sound number error
     *
     *  \iparam SoundNumberError = Error of the sound number
     */
    /****************************************************************************/
    void SetSoundNumberError(int SoundNumberError)
    {
        m_SoundNumberError = SoundNumberError;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the sound number warning
     *
     *  \return sound number warning
     */
    /****************************************************************************/
    int GetSoundNumberWarning() const
    {
        return m_SoundNumberWarning;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the sound number warning
     *
     *  \iparam SoundNumberWarning = warning number of the sound
     */
    /****************************************************************************/
    void SetSoundNumberWarning(int SoundNumberWarning)
    {
        m_SoundNumberWarning = SoundNumberWarning;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the sound level error
     *
     *  \return sound level error value
     */
    /****************************************************************************/
    int GetSoundLevelError() const
    {
        return m_SoundLevelError;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the sound level error
     *
     *  \iparam SoundLevel = level of the sound error
     */
    /****************************************************************************/
    void SetSoundLevelError(int SoundLevel)
    {
        m_SoundLevelError = SoundLevel;
    }

    /****************************************************************************/
    /*!
     *  \brief Get the sound level warning
     *
     *  \return sound level warning value
     */
    /****************************************************************************/
    int GetSoundLevelWarning() const
    {
        return m_SoundLevelWarning;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the sound level warning
     *
     *  \iparam SoundLevel = volume of the sound level
     */
    /****************************************************************************/
    void SetSoundLevelWarning(int SoundLevel)
    {
        m_SoundLevelWarning = SoundLevel;
    }

    QString GetValue(QString key) const;

    /****************************************************************************/
    /*!
     *  \brief Set Value (e.g. RMS_STATE)
     *
     *  \iparam key = unique key e.g. RMS_STATE
     *  \iparam value = value associated with Key
     */
    /****************************************************************************/
    void SetValue(QString key, QString value)
    {
        if (!m_ValueList.contains(key.toUpper()))
        {
            qDebug() << "UserSettings::SetValue, key not found" << key;
        }
        m_ValueList.insert(key.toUpper(), value);
    }

    /****************************************************************************/
    /*!
     *  \brief Set Value (e.g. RMS_STATE)
     *
     *  \iparam key = unique key e.g. RMS_STATE
     *  \iparam value = value associated with Key
     */
    /****************************************************************************/
    void SetValue(QString key, qint32 value)
    {
        SetValue(key, QString::number(value));
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the use of Remote Care Software to operate the device.
     *
     *  \return RemoteCare value.
     */
    /****************************************************************************/
    Global::OnOffState GetRemoteCare() const
    {
        return m_RemoteCare;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the use of Remote Care Software to operate the device.
     *
     *  \iparam RemoteCareOnOffState = On if Remote Care Software is used to operate the
     *          device else Off.
     */
    /****************************************************************************/
    void SetRemoteCare(Global::OnOffState RemoteCareOnOffState)
    {
        m_RemoteCare = RemoteCareOnOffState;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the DirectConnection value.
     *
     *  \return DirectConnection value.
     */
    /****************************************************************************/
    Global::OnOffState GetDirectConnection() const
    {
        return m_DirectConnection;
    }
    /****************************************************************************/
    /*!
     *  \brief Set the Device Direct Connection.
     *
     *  \iparam DeviceConnected = On if Device is directly connected else Off.
     */
    /****************************************************************************/
    void SetDirectConnection( Global::OnOffState DeviceConnected)
    {
        m_DirectConnection = DeviceConnected;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Network Settings Proxy UserName.
     *
     *  \return Proxy UserName.
     */
    /****************************************************************************/
    QString GetProxyUserName() const
    {
        return m_ProxyUserName;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Proxy UserName
     *
     *  \iparam ProxyUserName = Proxy UserName
     */
    /****************************************************************************/
    void SetProxyUserName(QString ProxyUserName)
    {
        m_ProxyUserName = ProxyUserName;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Network Settings Proxy Password.
     *
     *  \return Proxy Password.
     */
    /****************************************************************************/
    QString GetProxyPassword() const
    {
        return m_ProxyPassword;
    }

    /****************************************************************************/
    /*!
     *  \brief Set Proxy Password.
     *
     *  \iparam ProxyPassword = Proxy Password.
     */
    /****************************************************************************/
    void SetProxyPassword(QString ProxyPassword)
    {
        m_ProxyPassword = ProxyPassword;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Network Settings Proxy IP Address.
     *
     *  \return Proxy IP Address.
     */
    /****************************************************************************/
    QString GetProxyIPAddress() const
    {
        return m_ProxyIPAddress;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Proxy IP Address.
     *
     *  \iparam ProxyIPAddress = Proxy IP Address
     */
    /****************************************************************************/
    void SetProxyIPAddress(QString ProxyIPAddress)
    {
        m_ProxyIPAddress = ProxyIPAddress;
    }

    /****************************************************************************/
    /*!
     *  \brief Gets the Network Settings Proxy IP Port.
     *
     *  \return Proxy IP Port.
     */
    /****************************************************************************/
    int GetProxyIPPort() const
    {
        return m_ProxyIPPort;
    }

    /****************************************************************************/
    /*!
     *  \brief Set the Proxy IP Port number.
     *
     *  \iparam ProxyIPPort = Proxy IP Port number
     */
    /****************************************************************************/
    void SetProxyIPPort(int ProxyIPPort)
    {
        m_ProxyIPPort = ProxyIPPort;
    }
    /****************************************************************************/
    /*!
     *  \brief  To set Corrections's Module Info
     *  \iparam Id
     *  \iparam Length40
     *  \iparam Length50
     *  \iparam Length55
     *  \iparam Length60
     */
    /****************************************************************************/
    void AddCorrectionModuleInfo(const QString Id, const QString Length40, const QString Length50, const QString Length55, const QString Length60)
    {
        CorrectionModule_t StructModule;

        StructModule.ModuleId = Id;
        StructModule.ModuleLength40 = Length40;
        StructModule.ModuleLength50 = Length50;
        StructModule.ModuleLength55 = Length55;
        StructModule.ModuleLength60 = Length60;

        m_ModuleNames.append(StructModule.ModuleId);
        m_ListOfCorrectionModules.insert(StructModule.ModuleId, StructModule);
    }


    /****************************************************************************/
    /*!
     *  \brief  To Update Corrections's Module Info
     *  \iparam Id
     *  \iparam Correction
     *
     * \return Result
     */
    /****************************************************************************/
    bool UpdateCorrcetionModuleInfo(const QString Id, const CorrectionModule_t &Correction)
    {

        if (m_ListOfCorrectionModules.contains(Id)) {

            m_ListOfCorrectionModules.remove(Id);
            m_ListOfCorrectionModules.insert(Id, Correction);
            return true;
        }
        return false;
    }
    /****************************************************************************/
    /*!
     *  \brief Returns the Module Information
     *  \iparam ModuleId
     *  \iparam Mod
     *  \return Module Struct
     */
    /****************************************************************************/
    bool GetCorrectionModuleInfo(QString ModuleId, CorrectionModule_t &Mod) const
    {
        if (m_ListOfCorrectionModules.contains(ModuleId)) {
            Mod = m_ListOfCorrectionModules.value(ModuleId);
            return true;
        }
        return false;

    }
    /****************************************************************************/
    /*!
     *  \brief Returns the Module Information based on Index
     *  \iparam Index
     *  \iparam Module
     *  \return Module Struct
     */
    /****************************************************************************/
    bool GetCorrectionModuleInfo(const unsigned int Index,CorrectionModule_t &Module) const
    {
        QString ModuleId = m_ModuleNames.at(Index);
        if (m_ListOfCorrectionModules.contains(ModuleId))
        {
            Module = m_ListOfCorrectionModules.value(ModuleId);
            return true;
        }
        return false;

    }
    /****************************************************************************/
    /*!
     *  \brief Returns number of Modules in the list
     *  \return number of Modules in the list
     */
    /****************************************************************************/
    int GetNumberOfCorrectionModules() const { return m_ListOfCorrectionModules.count(); }
    /****************************************************************************/
    /*!
     *  \brief Returns the count of value list
     *  \return value list count
     */
    /****************************************************************************/
    int GetValueListCount() const { return m_ValueList.count(); }
    /****************************************************************************/
    /*!
     *  \brief  Deletes all the Correction module in the list
     */
    /****************************************************************************/
    void DeleteAllCorrectionModules();


}; // end class CUserSettings

} // end namespace DataManager

#endif // DATAMANAGEMENT_CUserSettings_H

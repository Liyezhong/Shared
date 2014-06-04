/****************************************************************************/
/*! \file Platform/Master/Components/DataManager/Containers/UserSettings/Source/UserSettings.cpp
 *
 *  \brief Implementation file for class CUserSettings.
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
//lint -sem(DataManager::CUserSettings::CopyFromOther,initializer)
#include <QReadLocker>
#include <QWriteLocker>
#include <QDebug>
#include <QFile>

#include "DataManager/Containers/UserSettings/Include/UserSettings.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"
//lint -e641
//lint -e1536


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CUserSettings::CUserSettings() :
    m_Version(0),
    m_Language(QLocale::German),
    m_DateFormat(Global::DATE_UNDEFINED),
    m_TimeFormat(Global::TIME_UNDEFINED),
    m_TemperatureFormat(Global::TEMP_FORMAT_UNDEFINED),
    m_SoundNumberError(0),
    m_SoundLevelError(0),
    m_SoundNumberWarning(0),
    m_SoundLevelWarning(0),
    m_RemoteCare(Global::ONOFFSTATE_UNDEFINED),
    m_DirectConnection(Global::ONOFFSTATE_UNDEFINED),
    m_ProxyUserName(""),
    m_ProxyPassword(""),
    m_ProxyIPAddress("000.000.000.001"),
    m_ProxyIPPort(1)
{
    // set default values
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam UserSettings = Instance of the CUserSettings class
 *
 *  \return
 */
/****************************************************************************/
CUserSettings::CUserSettings(const CUserSettings& UserSettings)
{
   CopyFromOther(UserSettings);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \note  Method for internal use only
 *
 *  \iparam UserSettings = Instance of the CStation class
 *
 *  \return
 */
/****************************************************************************/
void CUserSettings::CopyFromOther(const CUserSettings &UserSettings)
{
    CUserSettings &OtherStation = const_cast<CUserSettings &>(UserSettings);
    m_Version  = OtherStation.GetVersion();
    m_Language     = OtherStation.GetLanguage();
    m_DateFormat = OtherStation.GetDateFormat();
    m_TimeFormat = OtherStation.GetTimeFormat();
    m_TemperatureFormat = OtherStation.GetTemperatureFormat();
    m_SoundNumberError = OtherStation.GetSoundNumberError();
    m_SoundLevelError = OtherStation.GetSoundLevelError();
    m_SoundNumberWarning = OtherStation.GetSoundNumberWarning();
    m_SoundLevelWarning = OtherStation.GetSoundLevelWarning();
    m_RemoteCare = OtherStation.GetRemoteCare();
    m_DirectConnection = OtherStation.GetDirectConnection();
    m_ProxyUserName = OtherStation.GetProxyUserName();
    m_ProxyPassword = OtherStation.GetProxyPassword();
    m_ProxyIPAddress = OtherStation.GetProxyIPAddress();
    m_ProxyIPPort = OtherStation.GetProxyIPPort();
    m_ValueList = OtherStation.GetValueList();
    int Count = const_cast<CUserSettings&>(UserSettings).GetNumberOfCorrectionModules();

    this->DeleteAllCorrectionModules();

    for(int i=0; i<Count; i++)
    {
       CorrectionModule_t StructModule;
        if(OtherStation.GetCorrectionModuleInfo(i ,StructModule)) {

            m_ModuleNames.append(StructModule.ModuleId);
            m_ListOfCorrectionModules.insert(StructModule.ModuleId, StructModule);
        }
    }
}
/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CUserSettings::~CUserSettings()
{
    try {
        (void)DeleteAllCorrectionModules(); // to avoid lint-534
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
/*!
 *  \brief Set the default values of the local variables
 */
/****************************************************************************/
void CUserSettings::SetDefaultAttributes()
{
    m_Version               = 1;
    m_Language              = QLocale::English;
    m_DateFormat            = Global::DATE_INTERNATIONAL;
    m_TimeFormat            = Global::TIME_24;
    m_TemperatureFormat     = Global::TEMP_FORMAT_CELSIUS;
    m_SoundNumberError      = 1;
    m_SoundLevelError       = 6;
    m_SoundNumberWarning    = 1;
    m_SoundLevelWarning     = 6;
    m_RemoteCare            = Global::ONOFFSTATE_ON;
    m_DirectConnection      = Global::ONOFFSTATE_OFF;
    m_ProxyUserName         = "Himalaya";
    m_ProxyPassword         = "Himalaya";
    m_ProxyIPAddress        = "000.000.000.001";
    m_ProxyIPPort           = 1;
}



/****************************************************************************/
/*!
 *  \brief Writes from the CUserSettings object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the QXmlStreamWriter
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserSettings::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{

    // write the localization details
    XmlStreamWriter.writeStartElement("Localization");
    XmlStreamWriter.writeAttribute("Language", Global::LanguageToString(GetLanguage()));
    XmlStreamWriter.writeAttribute("DateFormat", Global::DateFormatToString(GetDateFormat()));
    XmlStreamWriter.writeAttribute("TimeFormat", Global::TimeFormatToString(GetTimeFormat()));
    XmlStreamWriter.writeAttribute("TemperatureFormat", Global::TemperatureFormatToString(GetTemperatureFormat()));
    XmlStreamWriter.writeEndElement();
    // write sound related details
    XmlStreamWriter.writeStartElement("Sound");
    // write errortone
    XmlStreamWriter.writeStartElement("ErrorTone");
    XmlStreamWriter.writeAttribute("Number", QString::number(GetSoundNumberError(), 10));
    XmlStreamWriter.writeAttribute("Level", QString::number(GetSoundLevelError(), 10));
    XmlStreamWriter.writeEndElement();
    // write warningtone
    XmlStreamWriter.writeStartElement("WarningTone");
    XmlStreamWriter.writeAttribute("Number", QString::number(GetSoundNumberWarning(), 10));
    XmlStreamWriter.writeAttribute("Level", QString::number(GetSoundLevelWarning(), 10));
    XmlStreamWriter.writeEndElement();
    // write sound end
    XmlStreamWriter.writeEndElement();

    if(m_ModuleNames.count() != 0) {

        XmlStreamWriter.writeStartElement("Correction");
        // write Correction settings related details
        QHash<QString, CorrectionModule_t>::const_iterator j;
        for(j = m_ListOfCorrectionModules.constBegin(); j != m_ListOfCorrectionModules.constEnd(); ++j)
        {
            XmlStreamWriter.writeStartElement("Module");
            CorrectionModule_t StructModule;
            StructModule = j.value();
            XmlStreamWriter.writeAttribute("ID", StructModule.ModuleId);
            XmlStreamWriter.writeAttribute("Length40", StructModule.ModuleLength40);
            XmlStreamWriter.writeAttribute("Length50", StructModule.ModuleLength50);
            XmlStreamWriter.writeAttribute("Length55", StructModule.ModuleLength55);
            XmlStreamWriter.writeAttribute("Length60", StructModule.ModuleLength60);
            XmlStreamWriter.writeEndElement();
        }
        XmlStreamWriter.writeEndElement();
    }
    //write network settings realted details
    XmlStreamWriter.writeStartElement("Network");
    XmlStreamWriter.writeAttribute("RemoteCare", Global::OnOffStateToString(GetRemoteCare()));
    XmlStreamWriter.writeAttribute("DirectConnection", Global::OnOffStateToString(GetDirectConnection()));
    XmlStreamWriter.writeAttribute("ProxyUserName", GetProxyUserName());
    XmlStreamWriter.writeAttribute("ProxyPassword", GetProxyPassword());
    XmlStreamWriter.writeAttribute("ProxyIPAddress", GetProxyIPAddress());
    XmlStreamWriter.writeAttribute("ProxyIPPort", QString::number(GetProxyIPPort(), 10));
    //write network end element
    XmlStreamWriter.writeEndElement();

    QMapIterator<QString, QString> i(m_ValueList);
    while (i.hasNext())
    {
        i.next();
        QStringList stringList = i.key().split("_",  QString::SkipEmptyParts);
        if (stringList.count() == 2)
        {
            if (CompleteData) {
                XmlStreamWriter.writeStartElement(stringList[0]);
                XmlStreamWriter.writeAttribute(stringList[1], i.value());
                XmlStreamWriter.writeEndElement();
            }
            else if (!stringList.contains("CLASSTEMPORARYDATA")) {
                XmlStreamWriter.writeStartElement(stringList[0]);
                XmlStreamWriter.writeAttribute(stringList[1], i.value());
                XmlStreamWriter.writeEndElement();
            }
        }
        else
        {
            XmlStreamWriter.writeAttribute(i.key(), i.value());
        }
    }

    if (CompleteData) {
        // do nothing
    }
    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CUserSettings object.
 *
 *  \iparam XmlStreamReader = Instance of the QXmlStreamReader.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserSettings::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    while ((!XmlStreamReader.atEnd()) &&
           ((XmlStreamReader.name() != "ClassTemporaryData") || (XmlStreamReader.tokenType() != QXmlStreamReader::StartElement)))
    {
        XmlStreamReader.readNextStartElement();

        if (XmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
        {
            if (XmlStreamReader.name() == "Localization")
            {
                if (!ReadLocalization(XmlStreamReader)) {
                    qDebug() << "CUserSettings::DeserializeContent: Read localization is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "Sound")
            {
                if (!ReadSoundSettings(XmlStreamReader)) {
                    qDebug() << "CUserSettings::DeserializeContent: Read sound settings is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "Correction")
            {
                DeleteAllCorrectionModules();
                if (!ReadCorrectionSettings(XmlStreamReader)) {
                    qDebug() << "CUserSettings::DeserializeContent: Read correction settings is failed";
                    return false;
                }
            }
            else if (XmlStreamReader.name() == "Network")
            {
                if (!ReadNetworkSettings(XmlStreamReader)) {
                    qDebug() << "CUserSettings::DeserializeContent: Read NetworkSettings is failed";
                    return false;
                }
            }

            else if (!XmlStreamReader.atEnd() && !XmlStreamReader.hasError())
            {
                QXmlStreamAttributes attributes = XmlStreamReader.attributes();
                foreach (QXmlStreamAttribute attribute, attributes)
                {
                    m_ValueList.insert(XmlStreamReader.name().toString().toUpper() + QString("_") + attribute.name().toString().toUpper(), attribute.value().toString());
                }
            }
        }
    }

    if (CompleteData) {
        // do nothing
    }
    return true;
}
/****************************************************************************/
/*!
 *  \brief Reads the Correction settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserSettings::ReadCorrectionSettings(QXmlStreamReader& XmlStreamReader)
{

    while ((XmlStreamReader.tokenType() != QXmlStreamReader::EndElement) || (XmlStreamReader.name() != "Correction"))
    {
        XmlStreamReader.readNextStartElement();
        if (XmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
        {
            //            qDebug() << "CUserSettings::DeserializeContent, checking" << XmlStreamReader.name();

            if (XmlStreamReader.name() == "Module") {
                if (!XmlStreamReader.attributes().hasAttribute("ID")) {
                    qDebug() <<  " attribute <ID> is missing => abort reading";
                    return false;
                }
                QString ModuleId = XmlStreamReader.attributes().value("ID").toString();

                if (!XmlStreamReader.attributes().hasAttribute("Length40")) {
                    qDebug() <<  " attribute <Length40> is missing => abort reading";
                    return false;
                }
                QString ModuleLength40 = XmlStreamReader.attributes().value("Length40").toString();

                if (!XmlStreamReader.attributes().hasAttribute("Length50")) {
                    qDebug() <<  " attribute <Length50> is missing => abort reading";
                    return false;
                }
                QString ModuleLength50 = XmlStreamReader.attributes().value("Length50").toString();

                if (!XmlStreamReader.attributes().hasAttribute("Length55")) {
                    qDebug() <<  " attribute <Length55> is missing => abort reading";
                    return false;
                }
                QString ModuleLength55 = XmlStreamReader.attributes().value("Length55").toString();

                if (!XmlStreamReader.attributes().hasAttribute("Length60")) {
                    qDebug() <<  " attribute <Length60> is missing => abort reading";
                    return false;
                }
                QString ModuleLength60 = XmlStreamReader.attributes().value("Length60").toString();

                AddCorrectionModuleInfo(ModuleId, ModuleLength40, ModuleLength50, ModuleLength55, ModuleLength60);
            }


        }
    }

    return true;
}
/****************************************************************************/
/*!
 *  \brief Reads the localization settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserSettings::ReadLocalization(QXmlStreamReader& XmlStreamReader)
{
    // read language
    if (!XmlStreamReader.attributes().hasAttribute("Language")) {
        qDebug() << "CUserSettings::ReadLocalization:### attribute <Language> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "Language", true);
        return false;
    }
    SetLanguage(Global::StringToLanguage(XmlStreamReader.attributes().value("Language").toString()));
    // read dateformat
    if (!XmlStreamReader.attributes().hasAttribute("DateFormat")) {
        qDebug() << "CUserSettings::ReadLocalization:### attribute <DateFormat> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "DateFormat", true);
        return false;
    }
    QString DateFormatStr = XmlStreamReader.attributes().value("DateFormat").toString();
    Global::DateFormat DF = Global::StringToDateFormat(DateFormatStr, false);
    if(DF == Global::DATE_UNDEFINED) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_DATEFORMAT, Global::tTranslatableStringList() << DF, true);
    }
    SetDateFormat(DF);

    // read timeformat
    if (!XmlStreamReader.attributes().hasAttribute("TimeFormat")) {
        qDebug() << "CUserSettings::ReadLocalization:### attribute <TimeFormat> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "TimeFormat", true);
        return false;
    }
    QString TimeFormatStr = XmlStreamReader.attributes().value("TimeFormat").toString();
    Global::TimeFormat TF = Global::StringToTimeFormat(TimeFormatStr, false);
    if(TF == Global::TIME_UNDEFINED) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_TIMEFORMAT, Global::tTranslatableStringList() << TF, true);
    }
    SetTimeFormat(TF);

    // read temperatureformat
    if (!XmlStreamReader.attributes().hasAttribute("TemperatureFormat")) {
        qDebug() << "CUserSettings::ReadLocalization:### attribute <TemperatureFormat> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "TemperatureFormat", true);
        return false;
    }
    QString TempFormatStr = XmlStreamReader.attributes().value("TemperatureFormat").toString();
    Global::TemperatureFormat TempF = Global::StringToTemperatureFormat(TempFormatStr, false);
    if(TempF == Global::TEMP_FORMAT_UNDEFINED) {
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_INVALID_TEMPFORMAT, Global::tTranslatableStringList() <<TempF, true);
    }

    SetTemperatureFormat(TempF);

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the sound settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserSettings::ReadSoundSettings(QXmlStreamReader& XmlStreamReader)
{
    while ((XmlStreamReader.tokenType() != QXmlStreamReader::EndElement) || (XmlStreamReader.name() != "Sound"))
    {
        XmlStreamReader.readNextStartElement();
        if (XmlStreamReader.tokenType() == QXmlStreamReader::StartElement)
        {
            //            qDebug() << "CUserSettings::DeserializeContent, checking" << XmlStreamReader.name();

            if (XmlStreamReader.name() == "ErrorTone")
            {
                // read number
                if (!XmlStreamReader.attributes().hasAttribute("Number")) {
                    qDebug() << "CUserSettings::ReadSoundSettings:### attribute <Number> is missing => abort reading";
                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ErrorTone Number", true);
                    return false;
                }
                SetSoundNumberError(XmlStreamReader.attributes().value("Number").toString().toInt());

                // read level attribute
                if (!XmlStreamReader.attributes().hasAttribute("Level")) {
                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ErrorTone Level", true);
                    return false;
                }
                SetSoundLevelError(XmlStreamReader.attributes().value("Level").toString().toInt());
            }
            if (XmlStreamReader.name() == "WarningTone")
            {
                // read number
                if (!XmlStreamReader.attributes().hasAttribute("Number")) {
                    qDebug() << "CUserSettings::ReadSoundSettings:### attribute <Number> is missing => abort reading";
                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "WarningTone Number", true);
                    return false;
                }
                SetSoundNumberWarning(XmlStreamReader.attributes().value("Number").toString().toInt());

                // read level attribute
                if (!XmlStreamReader.attributes().hasAttribute("Level")) {
                    qDebug() << "CUserSettings::ReadSoundSettings:### attribute <Level> is missing => abort reading";
                    Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "WarningTone Level", true);
                    return false;
                }
                SetSoundLevelWarning(XmlStreamReader.attributes().value("Level").toString().toInt());
            }
        }
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads the Network settings from the file.
 *
 *  \iparam XmlStreamReader = Xmlfile reader pointer
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserSettings::ReadNetworkSettings(QXmlStreamReader& XmlStreamReader)
{
    if (!XmlStreamReader.attributes().hasAttribute("RemoteCare")) {
        qDebug() << "CUserSettings::ReadNetworkSettings:### attribute <RemoteCare> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "RemoteCare", true);
        return false;
    }
    SetRemoteCare(Global::StringToOnOffState(XmlStreamReader.attributes().value("RemoteCare").toString(), false));

    if (!XmlStreamReader.attributes().hasAttribute("DirectConnection")) {
        qDebug() << "CUserSettings::ReadNetworkSettings:### attribute <DirectConnection> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "DirectConnection", true);
        return false;
    }
    SetDirectConnection(Global::StringToOnOffState(XmlStreamReader.attributes().value("DirectConnection").toString(), false));

    if (!XmlStreamReader.attributes().hasAttribute("ProxyUserName")) {
        qDebug() << "CUserSettings::ReadNetworkSettings:### attribute <ProxyUserName> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ProxyUserName", true);
        return false;
    }
    SetProxyUserName(XmlStreamReader.attributes().value("ProxyUserName").toString());

    if (!XmlStreamReader.attributes().hasAttribute("ProxyPassword")) {
        qDebug() << "CUserSettings::ReadNetworkSettings:### attribute <ProxyPassword> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ProxyPassword", true);
        return false;
    }
    SetProxyPassword(XmlStreamReader.attributes().value("ProxyPassword").toString());

    if (!XmlStreamReader.attributes().hasAttribute("ProxyIPAddress")) {
        qDebug() << "CUserSettings::ReadNetworkSettings:### attribute <ProxyIPAddress> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ProxyIPAddress", true);
        return false;
    }
    SetProxyIPAddress(XmlStreamReader.attributes().value("ProxyIPAddress").toString());

    if (!XmlStreamReader.attributes().hasAttribute("ProxyIPPort")) {
        qDebug() << "CUserSettings::ReadNetworkSettings:### attribute <ProxyIPPort> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "ProxyIPPort", true);
        return false;
    }
    SetProxyIPPort(XmlStreamReader.attributes().value("ProxyIPPort").toString().toInt());
    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam UserSettings = CUserSettings class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CUserSettings& UserSettings)
{
    // remove the constant and store it in a local variable
    CUserSettings* p_TempUserSettings = const_cast<CUserSettings*>(&UserSettings);
    QXmlStreamWriter XmlStreamWriter; ///< Writer for the XML

    // set the IO device
    QIODevice* IODevice = OutDataStream.device();

    XmlStreamWriter.setDevice(IODevice);
    XmlStreamWriter.setAutoFormatting(true);
    // start the XML Document
    XmlStreamWriter.writeStartDocument();
    // write the documnet type declaration
    XmlStreamWriter.writeDTD("<!DOCTYPE Settings>");
    XmlStreamWriter.writeStartElement("Settings");
    if (!p_TempUserSettings->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CUserSettings::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        //THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
        const_cast<CUserSettings &>(UserSettings).m_ErrorMap.insert(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "UserSettings");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "UserSettings", true);
    }

    // write enddocument
    XmlStreamWriter.writeEndDocument();
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam UserSettings = CUserSettings class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CUserSettings& UserSettings)
{
    QXmlStreamReader XmlStreamReader; ///< Reader for the XML
    // store the IO device
    QIODevice* IODevice = InDataStream.device();

    XmlStreamReader.setDevice(IODevice);

    // deserialize the content of the xml
    if (!UserSettings.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CUserSettings::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        //THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
        UserSettings.m_ErrorMap.insert(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "UserSettings");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "UserSettings", true);
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam UserSettings = CUserSettings class object
 *
 *  \return CUserSettings Class Object
 */
/****************************************************************************/
CUserSettings& CUserSettings::operator=(const CUserSettings& UserSettings)
{
    // make sure not same object
    if (this != &UserSettings)
    {
        CopyFromOther(UserSettings);
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Get value of a particular setting from the value list
 *
 *  \iparam key  = unique associated with the setting
 *
 *  \return Value
 */
/****************************************************************************/
QString CUserSettings::GetValue(QString key) const
{
    if (m_ValueList.contains(key.toUpper()))
    {
        return m_ValueList.value(key.toUpper(), "");
    }
    else
    {
        qDebug() << "UserSettings::GetValue, key not found" << key;
        return "";
    }
}
/****************************************************************************/
/*!
 *  \brief Returns the Vlaue list
 *
 *  \return ValueList
 */
/****************************************************************************/
QMap<QString ,QString> CUserSettings::GetValueList() const
{
    return m_ValueList;
}
/****************************************************************************/
/*!
 *  \brief  Gets the last errors which is done by verifier
 *
 *  \return QStringList - List of the errors occured
 */
/****************************************************************************/
ErrorMap_t& CUserSettings::GetErrors()
{
    // return the last error which is occured in the verifier
    return m_ErrorMap;
}
/****************************************************************************/
/*!
 *  \brief  Deletes all the modules in the list
 */
/****************************************************************************/
void CUserSettings::DeleteAllCorrectionModules()
{
    m_ListOfCorrectionModules.clear();
    m_ModuleNames.clear();

}
} // end namespace DataManager

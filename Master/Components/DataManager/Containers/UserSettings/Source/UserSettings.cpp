/****************************************************************************/
/*! \file UserSettings.h
 *
 *  \brief Implementation file for class CUserSettings.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2010-08-23, 2012-04-23
 *  $Author:    $ J.Bugariu, Raju
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

#include <QReadLocker>
#include <QWriteLocker>
#include <QDebug>
#include <QFile>

#include "DataManager/Containers/UserSettings/Include/UserSettings.h"



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
    m_SoundLevelWarning(0)
//    m_AgitationSpeed(0),
//    m_OvenStartMode(Global::OVENSTART_UNDEFINED),
//    m_OvenTemp(0),
//    m_RMSState(Global::ONOFFSTATE_UNDEFINED),
//    m_WaterType(Global::WATER_TYPE_UNDEFINED),
//    m_LeicaAgitationSpeed(0),
//    m_LeicaOvenTemp(0),
//    m_LoaderReagent1(""),
//    m_LoaderReagent2(""),
//    m_LoaderReagent3(""),
//    m_LoaderReagent4(""),
//    m_LoaderReagent5("")
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
    // remove the constant using type cast
    CUserSettings* p_TempUserSettings = const_cast<CUserSettings*>(&UserSettings);
    // do a deep copy of the data
    *this = *p_TempUserSettings;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CUserSettings::~CUserSettings()
{
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
//    m_AgitationSpeed        = 100;
//    m_OvenStartMode         = Global::OVENSTART_BEFORE_PROGRAM;
//    m_OvenTemp              = 70;
//    m_RMSState              = Global::ONOFFSTATE_ON;
//    m_WaterType             = Global::WATER_TYPE_TAP;
//    m_LeicaAgitationSpeed   = 1;
//    m_LeicaOvenTemp         = 45;
    //    // clear all the loader reagents
    //    m_LoaderReagents.clear();
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
    QString StringValue; ///< to store the version number

    // write version number
    (void) StringValue.setNum(GetVersion());  //to suppress lint-534
    XmlStreamWriter.writeAttribute("Version", StringValue);

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
    qDebug()<<"Serialize Value List"<<m_ValueList;
    QHashIterator<QString, QString> i(m_ValueList);
    while (i.hasNext())
    {
        i.next();

        QStringList stringList = i.key().split("_",  QString::SkipEmptyParts);
        if (stringList.count() == 2)
        {
            XmlStreamWriter.writeStartElement(stringList[0]);
            XmlStreamWriter.writeAttribute(stringList[1], i.value());
            XmlStreamWriter.writeEndElement();
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
                qDebug()<<GetDateFormat()<<GetTimeFormat()<<GetTemperatureFormat();
            }
            else if (XmlStreamReader.name() == "Sound")
            {
                if (!ReadSoundSettings(XmlStreamReader)) {
                    qDebug() << "CUserSettings::DeserializeContent: Read sound settings is failed";
                    return false;
                }
            }
            else
            {
                QXmlStreamAttributes attributes = XmlStreamReader.attributes();
                foreach (QXmlStreamAttribute attribute, attributes)
                {
                    qDebug() << "CUserSettings::DeserializeContent, inserting" << XmlStreamReader.name().toString().toUpper() + QString("_") + attribute.name().toString().toUpper() << attribute.value().toString();
                    m_ValueList.insert(XmlStreamReader.name().toString().toUpper() + QString("_") + attribute.name().toString().toUpper(), attribute.value().toString());
                }
            }
        }
    }

    if (CompleteData) {
        // do nothing
    }
    XmlStreamReader.device()->reset();
    qDebug()<<"UserSettings Class";
    qDebug()<<"UserSettings"<<XmlStreamReader.device()->readAll();
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
        return false;
    }    
    SetLanguage(Global::StringToLanguage(XmlStreamReader.attributes().value("Language").toString()));

    // read dateformat
    if (!XmlStreamReader.attributes().hasAttribute("DateFormat")) {
        qDebug() << "CUserSettings::ReadLocalization:### attribute <DateFormat> is missing => abort reading";
        return false;
    }
    QString DateFormatStr = XmlStreamReader.attributes().value("DateFormat").toString();
    Global::DateFormat DF = Global::StringToDateFormat(DateFormatStr, false);
    if(DF == Global::DATE_UNDEFINED) {
        // wrong format. throw exception.
        //LOGANDTHROWARG(EVENT_DATAMANAGEMENT_ERROR_NO_VALID_DATEFORMAT, DateFormatStr);
    }
    SetDateFormat(DF);

    // read timeformat
    if (!XmlStreamReader.attributes().hasAttribute("TimeFormat")) {
        qDebug() << "CUserSettings::ReadLocalization:### attribute <TimeFormat> is missing => abort reading";
        return false;
    }
    QString TimeFormatStr = XmlStreamReader.attributes().value("TimeFormat").toString();
    Global::TimeFormat TF = Global::StringToTimeFormat(TimeFormatStr, false);
    if(TF == Global::TIME_UNDEFINED) {
        // wrong format. throw exception.
        //LOGANDTHROWARG(EVENT_DATAMANAGEMENT_ERROR_NO_VALID_TIMEFORMAT, TimeFormatStr);
    }
    SetTimeFormat(TF);

    // read temperatureformat
    if (!XmlStreamReader.attributes().hasAttribute("TemperatureFormat")) {
        qDebug() << "CUserSettings::ReadLocalization:### attribute <TemperatureFormat> is missing => abort reading";
        return false;
    }
    QString TempFormatStr = XmlStreamReader.attributes().value("TemperatureFormat").toString();
    Global::TemperatureFormat TempF = Global::StringToTemperatureFormat(TempFormatStr, false);
    if(TempF == Global::TEMP_FORMAT_UNDEFINED) {
        // wrong format. throw exception.
        //LOGANDTHROWARG(EVENT_DATAMANAGEMENT_ERROR_NO_VALID_TEMPFORMAT, TempFormatStr);
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
            qDebug() << "CUserSettings::DeserializeContent, checking" << XmlStreamReader.name();

            if (XmlStreamReader.name() == "ErrorTone")
            {
                // read number
                if (!XmlStreamReader.attributes().hasAttribute("Number")) {
                    qDebug() << "CUserSettings::ReadSoundSettings:### attribute <Number> is missing => abort reading";
                    return false;
                }
                SetSoundNumberError(XmlStreamReader.attributes().value("Number").toString().toInt());

                // read level attribute
                if (!XmlStreamReader.attributes().hasAttribute("Level")) {
                    qDebug() << "CUserSettings::ReadSoundSettings:### attribute <Level> is missing => abort reading";
                    return false;
                }
                SetSoundLevelError(XmlStreamReader.attributes().value("Level").toString().toInt());
            }
            if (XmlStreamReader.name() == "WarningTone")
            {
                // read number
                if (!XmlStreamReader.attributes().hasAttribute("Number")) {
                    qDebug() << "CUserSettings::ReadSoundSettings:### attribute <Number> is missing => abort reading";
                    return false;
                }
                SetSoundNumberWarning(XmlStreamReader.attributes().value("Number").toString().toInt());

                // read level attribute
                if (!XmlStreamReader.attributes().hasAttribute("Level")) {
                    qDebug() << "CUserSettings::ReadSoundSettings:### attribute <Level> is missing => abort reading";
                    return false;
                }
                SetSoundLevelWarning(XmlStreamReader.attributes().value("Level").toString().toInt());
            }
        }
    }

    return true;
}

///****************************************************************************/
///*!
// *  \brief Reads the loader reagents from the file.
// *
// *  \iparam XmlStreamReader = Xmlfile reader pointer
// *
// *  \return On successful return True or False
// */
///****************************************************************************/
//bool CUserSettings::ReadLoaderReagents(QXmlStreamReader& XmlStreamReader)
//{
//    // Look for node <Loader>
//    if (!Helper::ReadNode(XmlStreamReader, "Loader")) {
//        qDebug() << "CUserSettings::DeserializeContent: abort reading. Node not found: Loader";
//        return false;
//    }

//    //    // clear all the reagents from the list
//    //    m_LoaderReagents.clear();

//    if (!XmlStreamReader.attributes().hasAttribute("Reagent1")) {
//        qDebug() << "CUserSettings::DeserializeContent: abort reading. Attribute not found: Reagent1";
//        return false;
//    }
//    SetLoaderReagent1(XmlStreamReader.attributes().value("Reagent1").toString());

//    if (!XmlStreamReader.attributes().hasAttribute("Reagent2")) {
//        qDebug() << "CUserSettings::DeserializeContent: abort reading. Attribute not found: Reagent2";
//        return false;
//    }
//    SetLoaderReagent2(XmlStreamReader.attributes().value("Reagent2").toString());

//    if (!XmlStreamReader.attributes().hasAttribute("Reagent3")) {
//        qDebug() << "CUserSettings::DeserializeContent: abort reading. Attribute not found: Reagent3";
//        return false;
//    }
//    SetLoaderReagent3(XmlStreamReader.attributes().value("Reagent3").toString());

//    if (!XmlStreamReader.attributes().hasAttribute("Reagent4")) {
//        qDebug() << "CUserSettings::DeserializeContent: abort reading. Attribute not found: Reagent4";
//        return false;
//    }
//    SetLoaderReagent4(XmlStreamReader.attributes().value("Reagent4").toString());

//    if (!XmlStreamReader.attributes().hasAttribute("Reagent5")) {
//        qDebug() << "CUserSettings::DeserializeContent: abort reading. Attribute not found: Reagent5";
//        return false;
//    }
//    SetLoaderReagent5(XmlStreamReader.attributes().value("Reagent5").toString());
//    return true;
//}


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
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    // write enddocument
    XmlStreamWriter.writeEndDocument();
    qDebug()<<XmlStreamWriter.device()->reset();
    qDebug()<<"Serialize User Settings"<< XmlStreamWriter.device()->readAll();
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
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    XmlStreamReader.device()->reset();
    qDebug()<<">> Stream OPERATOR "<<XmlStreamReader.device()->readAll();
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
        // create the byte array
        QByteArray* p_TempByteArray = new QByteArray();
        // create the data stream to write into a file
        QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        p_TempByteArray->clear();
        // write the data into data stream from source
        DataStream << UserSettings;
        // reset the IO device pointer to starting position
        (void)DataStream.device()->reset(); //to avoid lint 534
        // read the data from data stream to destination object
        DataStream >> *this;

        delete p_TempByteArray;
    }
    return *this;
}

QString CUserSettings::GetValue(QString key)
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

} // end namespace DataManager

/****************************************************************************/
/*! \file DataManager/Helper/Source/XmlConfigFile.cpp
 *
 *  \brief Implementation file for class XmlConfigFile.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-02
 *  $Author:    $ J.Bugariu
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

#include <DataManager/Helper/Include/XmlConfigFile.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>



namespace DataManager {

/****************************************************************************/
XmlConfigFile::XmlConfigFile() : m_FileName("") {
}

/****************************************************************************/
XmlConfigFile::~XmlConfigFile() {
}

/****************************************************************************/
void XmlConfigFile::InitStreamReader(QXmlStreamReader &rReader, QFile &rFile, const QString &FileName) {
    // check if we can open file
    rFile.close();
    rFile.setFileName(FileName);

    if(!rFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        THROWARG(Global::EVENT_GLOBAL_ERROR_FILE_OPEN, FileName);
    }
    m_FileName = FileName;
    // set stream reader device
    rReader.setDevice(&rFile);
}

/****************************************************************************/
void XmlConfigFile::InitStreamReader(QXmlStreamReader &rReader, QIODevice &rDevice) {
    // check if we can open device
    rDevice.close();
    if(!rDevice.open(QIODevice::Text | QIODevice::ReadOnly)) {
        THROW(Global::EVENT_GLOBAL_ERROR_FILE_OPEN);
    }
    // set stream reader device
    rReader.setDevice(&rDevice);
}

/****************************************************************************/
void XmlConfigFile::InitStreamWriter(QXmlStreamWriter &rWriter, QFile &rFile, const QString &FileName) {
    rFile.close();
    rFile.setFileName(FileName);

    if(!rFile.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate)) {
        THROWARG(Global::EVENT_GLOBAL_ERROR_FILE_CREATE, FileName);
    }
    m_FileName = FileName;
    // set stream writer device
    rWriter.setDevice(&rFile);
    rWriter.setCodec("UTF-8");
    rWriter.setAutoFormatting(true);
    rWriter.setAutoFormattingIndent(4);
}

/****************************************************************************/
void XmlConfigFile::InitStreamWriter(QXmlStreamWriter &rWriter, QIODevice &rDevice) {
    rDevice.close();
    if(!rDevice.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate)) {
        THROW(Global::EVENT_GLOBAL_ERROR_FILE_CREATE);
    }
    // set stream writer device
    rWriter.setDevice(&rDevice);
    rWriter.setCodec("UTF-8");
    rWriter.setAutoFormatting(true);
    rWriter.setAutoFormattingIndent(4);
}

/****************************************************************************/
QString XmlConfigFile::ReadAttributeString(QXmlStreamReader &rReader, const QString &AttributeName) {
    QXmlStreamAttributes Attr = rReader.attributes();
    if(!Attr.hasAttribute(AttributeName)) {
        // attribute not found. throw exception
        THROWARGS(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND,
                  Global::tTranslatableStringList() << AttributeName << m_FileName);
    }
    // return attribute as string
    return Attr.value(AttributeName).toString();
}

/****************************************************************************/
quint32 XmlConfigFile::ReadAttributequint32(QXmlStreamReader &rReader, const QString &AttributeName) {
    QString ValueStr = ReadAttributeString(rReader, AttributeName);
    bool OK = false;
    quint32 Value = ValueStr.toULongLong(&OK, 10);
    if(!OK) {
        // not a valid number
        THROWARGS(EVENT_DM_ERROR_NO_VALID_NUMBER,
                  Global::tTranslatableStringList() << ValueStr << m_FileName);
    }
    return Value;
}

/****************************************************************************/
QLocale::Language XmlConfigFile::ReadAttributeLanguage(QXmlStreamReader &rReader, const QString &AttributeName) {
    QString LangName = ReadAttributeString(rReader, AttributeName);
    QLocale::Language TheLanguage = Global::StringToLanguage(LangName);
    if(TheLanguage == QLocale::C) {
        // wrong value. throw exception
        THROWARGS(EVENT_DM_ERROR_NOT_SUPPORTED_LANGUAGE,
                  Global::tTranslatableStringList() << LangName << m_FileName);
    }
    return TheLanguage;
}

/****************************************************************************/
Global::OnOffState XmlConfigFile::ReadAttributeOnOff(QXmlStreamReader &rReader, const QString &AttributeName) {
    QString ValueStr = ReadAttributeString(rReader, AttributeName);
    Global::OnOffState Value = Global::StringToOnOffState(ValueStr, false);
    if(Value == Global::ONOFFSTATE_UNDEFINED) {
        // wrong value. throw exception
        THROWARGS(EVENT_DM_INVALID_RMS_ONOFFSTATE,
                  Global::tTranslatableStringList() << ValueStr << m_FileName);
    }
    return Value;
}

/****************************************************************************/
void XmlConfigFile::ReadStartElement(QXmlStreamReader &rReader, const QString &ElementName) {
    if(!rReader.readNextStartElement()) {
        THROWARGS(EVENT_DM_ERROR_READING_XML_STARTELEMENT,
                  Global::tTranslatableStringList() << ElementName << m_FileName);
    }
    if (rReader.name() != ElementName) {
        THROWARGS(EVENT_DM_ERROR_UNEXPECTED_XML_STARTELEMENT,
                  Global::tTranslatableStringList() << rReader.name().toString() << ElementName << m_FileName);
    }
}

/****************************************************************************/
QString XmlConfigFile::ReadFormatVersion(QXmlStreamReader &rReader, const QString &RootTagName) {
    // read start element
    ReadStartElement(rReader, RootTagName);
    return ReadAttributeString(rReader, "version");
}

/****************************************************************************/
void XmlConfigFile::WriteFormatVersion(QXmlStreamWriter &rWriter,
                                       const QString &RootTagName, const QString &Version) {
    // write start
    rWriter.writeStartElement(RootTagName);
    // write version
    rWriter.writeAttribute("version", Version);
}

} // end namespace DataManager

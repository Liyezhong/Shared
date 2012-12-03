/****************************************************************************/
/*! \file XmlConfigFilePasswords.cpp
 *
 *  \brief Implementation file for class XmlConfigFilePasswords.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-08-17
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

#include <TestStubDataManager/Helper/Include/XmlConfigFilePasswords.h>
#include <TestStubDataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>


namespace DataManager {

/****************************************************************************/
XmlConfigFilePasswords::XmlConfigFilePasswords(const QString &ExpectedSerialNumber) :
    m_ExpectedSerialNumber(ExpectedSerialNumber)
{
}

/****************************************************************************/
void XmlConfigFilePasswords::ReadPassword_V1(QXmlStreamReader &rReader, PasswordManager::CPasswordManager &rPasswordManager) {
    QString Name = ReadAttributeString(rReader, "name");
    QString Hash = ReadAttributeString(rReader, "hash");
    // set password
    rPasswordManager.SetPasswordHash(Name, Hash);
    // read until end of element
    rReader.skipCurrentElement();
}

/****************************************************************************/
void XmlConfigFilePasswords::ReadPasswords_V1(QXmlStreamReader &rReader, PasswordManager::CPasswordManager &rPasswordManager) {
    // read serial number
    ReadStartElement(rReader, "serialnumber");
    QString SerialNumber = rReader.readElementText();
    if(SerialNumber != m_ExpectedSerialNumber) {
        // serial number does not fit
        THROW(EVENT_DATAMANAGER_ERROR_PASSWORD_SERIAL_NUMBER);
    }
    while(rReader.readNextStartElement()) {
        if(rReader.name() == "password") {
            // Found a password. read it.
            ReadPassword_V1(rReader, rPasswordManager);
        } else {
            THROWARG(EVENT_DATAMANAGER_ERROR_UNEXPECTED_XML_STARTELEMENT, rReader.name().toString());
        }
    }
}

/****************************************************************************/
void XmlConfigFilePasswords::ReadPasswords(const QString &FileName, PasswordManager::CPasswordManager &rPasswordManager) {
    // reset data
    rPasswordManager.Clear();
    try {
        // init stream reader
        QXmlStreamReader Reader;
        QFile File;
        InitStreamReader(Reader, File, FileName);
        // now read format version
        QString Version = ReadFormatVersion(Reader, "passwords");
        if(Version == "1") {
            // read project file in version 1
            ReadPasswords_V1(Reader, rPasswordManager);
        } else {
            // not a supported version
            THROWARG(EVENT_DATAMANAGER_ERROR_UNSUPPORTED_VERSION, Version);
        }
    } catch(...) {
        // reset data
        rPasswordManager.Clear();
        throw;
    }
}

/****************************************************************************/
void XmlConfigFilePasswords::WritePasswords(const QString &FileName, const PasswordManager::CPasswordManager &ThePasswordManager, const QString &SerialNumber) {
    // init stream writer
    QXmlStreamWriter Writer;
    QFile File;
    InitStreamWriter(Writer, File, FileName);

    // write start document
    Writer.writeStartDocument("1.0");
    // write root element and version
    WriteFormatVersion(Writer, "passwords", "1");
    // write serial number
    Writer.writeTextElement("serialnumber", SerialNumber);
    PasswordManager::tPasswordIterator it = ThePasswordManager.Iterator();
    while(it.hasNext()) {
        static_cast<void>(
            // we DO NOT NEED the return value of next
            it.next()
        );
        // write one password entry
        Writer.writeStartElement("password");
        Writer.writeAttribute("name", it.key());
        Writer.writeAttribute("hash", it.value());
        Writer.writeEndElement();
    }
    // write root element end
    Writer.writeEndElement();
    // write enddocument
    Writer.writeEndDocument();
}

} // end namespace DataManager

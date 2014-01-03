/****************************************************************************/
/*! \file DataManager/Helper/Source/XmlConfigFileStrings.cpp
 *
 *  \brief Implementation file for class XmlConfigFileStrings.
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

#include <DataManager/Helper/Include/XmlConfigFileStrings.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

#include <QStringList>

namespace DataManager {

/****************************************************************************/
void XmlConfigFileStrings::ResetData() {
    m_Data.clear();
}

/****************************************************************************/
void XmlConfigFileStrings::ReadStringEntry_V1(QXmlStreamReader &rReader, Global::tLanguageData &rLanguageData) {
    quint32 ID   = ReadAttributequint32(rReader, "id");
    QStringList TextList;
    TextList << ReadAttributeString(rReader, "text1");
    QString Text2 = ReadAttributeString(rReader, "text2").simplified();
    if (Text2.isEmpty()) {
        Text2 = TextList.at(0);
    }
    TextList << Text2;

    rLanguageData.insert(ID, TextList);
    // read until end of element
    rReader.skipCurrentElement();
}

/****************************************************************************/
void XmlConfigFileStrings::ReadLanguage_V1(QXmlStreamReader &rReader, QSet<QLocale::Language> &rLanguageList) {
    // extract and check name
    QLocale::Language TheLanguage = ReadAttributeLanguage(rReader, "name");
    // check if we must read language data
    if(rLanguageList.contains(TheLanguage)) {
        // remove from list
        rLanguageList.remove(TheLanguage);
        // this language is contained in list of desired languages
        Global::tLanguageData LanguageData;
        // extract string entries
        while(rReader.readNextStartElement()) {
            if(rReader.name() == "string") {
                // Found a string. Read it.
                ReadStringEntry_V1(rReader, LanguageData);
            } else {
                THROWARGS(EVENT_DM_ERROR_UNEXPECTED_XML_STARTELEMENT,
                          Global::tTranslatableStringList() << rReader.name().toString() << "string" << m_FileName);
            }
        }
        // append language data to result
        m_Data.insert(TheLanguage, LanguageData);
    } else {
        // we must not read language data for this language. Skip it.
        rReader.skipCurrentElement();
    }
}

/****************************************************************************/
void XmlConfigFileStrings::ReadStrings_V1(QXmlStreamReader &rReader, QSet<QLocale::Language> &rLanguageList) {
    while(rReader.readNextStartElement()) {
        if(rReader.name() == "language") {
            // Found a language.
            ReadLanguage_V1(rReader, rLanguageList);
        } else {
            THROWARGS(EVENT_DM_ERROR_UNEXPECTED_XML_STARTELEMENT,
                      Global::tTranslatableStringList() << rReader.name().toString() << "language" << m_FileName);
        }
    }
}

/****************************************************************************/
void XmlConfigFileStrings::ReadStrings(const QString &FileName, QSet<QLocale::Language> &rLanguageList) {
    // reset data
    ResetData();

    // init stream reader
    QXmlStreamReader rReader;
    QFile File;
    m_FileName = FileName;
    InitStreamReader(rReader, File, FileName);
    // now read format version
    QString Version = ReadFormatVersion(rReader, "strings");
    if(Version == "1") {
        // read project file in version 1
        ReadStrings_V1(rReader, rLanguageList);
    }
    else {
        ResetData();
        // not a supported version
        THROWARGS(EVENT_DM_ERROR_UNSUPPORTED_VERSION,
                  Global::tTranslatableStringList() << Version << m_FileName);
    }

}

} // end namespace DataManager

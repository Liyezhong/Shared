/****************************************************************************/
/*! \file XmlConfigFileTimeOffset.cpp
 *
 *  \brief Implementation file for class XmlConfigFileTimeOffset.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-01-25
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

#include <DataManagement/Include/XmlConfigFileTimeOffset.h>
#include <DataManagement/Include/DataManagementEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>

namespace DataManager {

/****************************************************************************/
XmlConfigFileTimeOffset::XmlConfigFileTimeOffset() {
}

/****************************************************************************/
void XmlConfigFileTimeOffset::ReadTimeOffset_V1(QXmlStreamReader &rReader, int &rTimeOffset) {
    ReadStartElement(rReader, "offset");
    rTimeOffset = rReader.readElementText().toInt();
}

/****************************************************************************/
void XmlConfigFileTimeOffset::ReadTimeOffset(const QString &FileName, int &rTimeOffset) {
    // reset data
    rTimeOffset = 0;

    try {
        // init stream reader
        QXmlStreamReader Reader;
        QFile File;
        InitStreamReader(Reader, File, FileName);
        // now read format version
        QString Version = ReadFormatVersion(Reader, "timeoffset");
        if(Version == "1") {
            // read project file in version 1
            ReadTimeOffset_V1(Reader, rTimeOffset);
        } else {
            // not a supported version
            THROWARG(EVENT_DATAMANAGER_ERROR_UNSUPPORTED_VERSION, Version);
        }
    } catch(...) {
        // reset data
        rTimeOffset = 0;
        throw;
    }
}

/****************************************************************************/
void XmlConfigFileTimeOffset::WriteTimeOffset(const QString &FileName, int TimeOffset) {
    // init stream writer
    QXmlStreamWriter Writer;
    QFile File;
    InitStreamWriter(Writer, File, FileName);

    // write start document
    Writer.writeStartDocument("1.0");
    // write root element and version
    WriteFormatVersion(Writer, "timeoffset", "1");
    // write offset
    Writer.writeTextElement("offset", QString::number(TimeOffset, 10));
    // write root element end
    Writer.writeEndElement();
    // write enddocument
    Writer.writeEndDocument();
}

} // end namespace DataManager

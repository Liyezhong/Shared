/****************************************************************************/
/*! \file DataManager/Helper/Source/XmlConfigFileTimeOffset.cpp
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

#include <DataManager/Helper/Include/XmlConfigFileTimeOffset.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>
#include <Global/Include/EventObject.h>
#include <Global/Include/SystemPaths.h>

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

    // init stream reader
    QXmlStreamReader Reader;
    QFile File;
    InitStreamReader(Reader, File, FileName);
    // now read format version
    QString Version = ReadFormatVersion(Reader, "timeoffset");
    if(Version == "1") {
        // read project file in version 1
        ReadTimeOffset_V1(Reader, rTimeOffset);
    }
    else {
        rTimeOffset = 0;
        // not a supported version
        THROWARGS(EVENT_DM_ERROR_UNSUPPORTED_VERSION,
                  Global::tTranslatableStringList() << Version << m_FileName);
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

    (void)File.flush();
    (void)fsync(File.handle());
    File.close();

    const QString MD5sumGenerator = QString("%1%2 %3").arg(Global::SystemPaths::Instance().GetScriptsPath()).
            arg(QString("/EBox-Utils.sh update_md5sum_for_file_in_settings")).arg(FileName);
    (void)system(MD5sumGenerator.toStdString().c_str());
}

} // end namespace DataManager

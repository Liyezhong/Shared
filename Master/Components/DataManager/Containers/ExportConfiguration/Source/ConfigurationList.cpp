/****************************************************************************/
/*! \file ConfigurationList.cpp
 *
 *  \brief Implementation file for class CConfigurationList.
 *         Reads the configurationList tag data from the XML file
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-07-25
 *  $Author:    $ Raju
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

#include <QDebug>
#include <QString>
#include <QBuffer>
#include <QXmlStreamReader>
#include <iostream>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Containers/ExportConfiguration/Include/ConfigurationList.h"


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 *
 */
/****************************************************************************/
CConfigurationList::CConfigurationList() :
    m_GroupFlag(true),
    m_GroupFileName(""),
    m_Encryption(true),
    m_Compressed(true),
    m_Certified(true),
    m_PackageType("native"),
    m_TagName("")
{
    m_FileList.clear();
}


/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ExportConfiguration = Instance of the CConfigurationList class
 *
 *  \return
 */
/****************************************************************************/
CConfigurationList::CConfigurationList(const CConfigurationList& ExportConfiguration)
{
    CConfigurationList* p_TempExportConfiguration = const_cast<CConfigurationList*>(&ExportConfiguration);

    *this = *p_TempExportConfiguration;
}

/****************************************************************************/
/*!
 *  \brief Writes from the CConfigurationList object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the QXmlStreamWriter
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CConfigurationList::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    // write the main tag to store all the details
    // write the start element
    XmlStreamWriter.writeStartElement("List");
    // write the encryption flag
    XmlStreamWriter.writeAttribute("Encryption", (m_Encryption == true ? "active" : "inactive"));
    // write the certified flag
    XmlStreamWriter.writeAttribute("Certified", (m_Certified == true ? "active" : "inactive"));
    // write the compressed flag
    XmlStreamWriter.writeAttribute("Compressed", (m_Compressed == true ? "active" : "inactive"));
    // write the package type
    XmlStreamWriter.writeAttribute("PackageType", m_PackageType);
    // write the file list and path of the file
    for (int FileCount = 0; FileCount < m_FileList.count(); FileCount++) {
        XmlStreamWriter.writeStartElement("File");
        XmlStreamWriter.writeAttribute("Path", m_FileList.value(FileCount));
        // write the end element
        XmlStreamWriter.writeEndElement();
    }
    // check the group flag value
    if (m_GroupFlag) {
        // write the <Group> tag in the xml element
        XmlStreamWriter.writeStartElement("Group");
        // write the file name in the xml
        XmlStreamWriter.writeAttribute("File", m_GroupFileName);
        // end the </Group> element
        XmlStreamWriter.writeEndElement();
    }

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // Do Nothing as the m_ExportConfigurationType is derived from m_ID
    }
    //======NODE=END====Temporary Data Variables=========================

    XmlStreamWriter.writeEndElement(); //list

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XmlStreamReader to CConfigurationList object.
 *
 *  \iparam XmlStreamReader = Instance of the XmlStreamReader.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CConfigurationList::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    bool Result = true;
    // read the node
    if (!Helper::ReadNode(XmlStreamReader, "List")) {
        return false;
    }
    // read the encryption attribute
    if (!XmlStreamReader.attributes().hasAttribute("Encryption")) {
        qDebug() << "### attribute <Encryption> is missing => abort reading";
        return false;
    }
    SetEncryptionFlag((XmlStreamReader.attributes().value("Encryption").compare("active") == 0 ? true : false));

    // read the certified attribute
    if (!XmlStreamReader.attributes().hasAttribute("Certified")) {
        qDebug() << "### attribute <Certified> is missing => abort reading";
        return false;
    }
    SetCertifiedFlag((XmlStreamReader.attributes().value("Certified").compare("active") == 0 ? true : false));

    // read the compressed attribute
    if (!XmlStreamReader.attributes().hasAttribute("Compressed")) {
        qDebug() << "### attribute <Compressed> is missing => abort reading";
        return false;
    }
    SetCompressedFlag((XmlStreamReader.attributes().value("Compressed").compare("active") == 0 ? true : false));

    // read the package type attribute
    if (!XmlStreamReader.attributes().hasAttribute("PackageType")) {
        qDebug() << "### attribute <PackageType> is missing => abort reading";
        return false;
    }
    SetPackageType(XmlStreamReader.attributes().value("PackageType").toString());

    // read the file list from the xml
    Result = ReadFileList(XmlStreamReader);

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // Do Nothing as the m_ExportConfigurationType is derived from m_ID
    }
    //======NODE=END====Temporary Data Variables=========================

    return Result;
}


/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ExportConfiguration = CConfigurationList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CConfigurationList& ExportConfiguration)
{
    CConfigurationList* p_TempExportConfiguration = const_cast<CConfigurationList*>(&ExportConfiguration);

    QXmlStreamWriter XmlStreamWriter;
    // set the device to streamer to write the file
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempExportConfiguration->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CConfigurationList::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the QDataStream
 *  \iparam ExportConfiguration = CConfigurationList class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CConfigurationList& ExportConfiguration)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

    if (!ExportConfiguration.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CConfigurationList::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceExportConfiguration = CConfigurationList class object
 *
 *  \return CConfigurationList Class Object
 */
/****************************************************************************/
CConfigurationList& CConfigurationList::operator=(const CConfigurationList& SourceExportConfiguration)
{
    // make sure not same object
    if (this != &SourceExportConfiguration)
    {
        QByteArray TempByteArray;
        CConfigurationList* p_TempExportConfiguration = const_cast<CConfigurationList*>(&SourceExportConfiguration);
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        DataStream << *p_TempExportConfiguration;
        (void)DataStream.device()->reset(); //to avoid lint-534
        DataStream >> *this;
    }
    return *this;
}


/****************************************************************************/
/*!
 *  \brief Reads the list of files from the xml file
 *
 *  \iparam XmlStreamReader =  Instance of the XmlStreamReader.
 *
 *  \return Successful (true) or not (false)
 */
/****************************************************************************/
bool CConfigurationList::ReadFileList(QXmlStreamReader& XmlStreamReader)
{
    // set the group list flag value
    SetGroupListFlag(false);
    // Look for node <File>
    while(!XmlStreamReader.atEnd()) {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement()) {
            // read the file tag value
            if (XmlStreamReader.name().compare("File") == 0) {
                if(XmlStreamReader.attributes().hasAttribute("Path")) {
                    m_FileList.append(XmlStreamReader.attributes().value("Path").toString());
                }
                else {
                    return false;
                }
            }
            // read the group tag value
            else if (XmlStreamReader.name().compare("Group") == 0) {
                if(XmlStreamReader.attributes().hasAttribute("File")) {
                    // set the group list flag value
                    SetGroupListFlag(true);
                    SetGroupFileName(XmlStreamReader.attributes().value("File").toString());
                }
                else {
                    return false;
                }
            }
            else {
                qDebug() << "### unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        }
        // check the end element
        if (XmlStreamReader.isEndElement()) {
            // if the element is </List> then retun the control
            if (XmlStreamReader.name().compare("List") == 0) {
                return true;
            }
        }
    }
    return true;
}

}  // namespace DataManager



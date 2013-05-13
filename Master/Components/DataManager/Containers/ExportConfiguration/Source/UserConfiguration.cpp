/****************************************************************************/
/*! \file UserConfiguration.cpp
 *
 *  \brief Implementation file for class CUserConfiguration.
 *         This class reads the "User" tag information from
 *         "ExportConfiguration.xml" file
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
#include "DataManager/Containers/ExportConfiguration/Include/UserConfiguration.h"
#include <Global/Include/EventObject.h>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 *
 */
/****************************************************************************/
CUserConfiguration::CUserConfiguration()
{
    // for user config there is no Group configuration
    m_UserConfigList.SetGroupListFlag(false);
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ExportConfiguration = Instance of the CUserConfiguration class
 *
 *  \return
 */
/****************************************************************************/
CUserConfiguration::CUserConfiguration(const CUserConfiguration& ExportConfiguration)
{
    CUserConfiguration* p_TempExportConfiguration = const_cast<CUserConfiguration*>(&ExportConfiguration);
    // for user config there is no Group configuration
    m_UserConfigList.SetGroupListFlag(false);
    *this = *p_TempExportConfiguration;
}

/****************************************************************************/
/*!
 *  \brief Writes from the CUserConfiguration object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the QXmlStreamWriter
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserConfiguration::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    bool Result = true;
    // start the element
    XmlStreamWriter.writeStartElement("UserConfig");
    // serialize the content
    if (!m_UserConfigList.SerializeContent(XmlStreamWriter, CompleteData)) {
        /// \todo need to write error handling code
        return false;
    }
    // start the element
    XmlStreamWriter.writeStartElement("UserReport");
    // serialize the content
    if (!m_UserReportList.SerializeContent(XmlStreamWriter, CompleteData)) {
        /// \todo need to write error handling code
        return false;
    }
    // write the end element
    XmlStreamWriter.writeEndElement();
    // write the end element
    XmlStreamWriter.writeEndElement();

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // Do Nothing as the m_ExportConfigurationType is derived from m_ID
    }
    //======NODE=END====Temporary Data Variables=========================

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XmlStreamReader to CUserConfiguration object.
 *
 *  \iparam XmlStreamReader = Instance of the XmlStreamReader.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CUserConfiguration::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{

    (void)XmlStreamReader.readElementText(QXmlStreamReader::IncludeChildElements); //lint -e534

    // read the <UserConfig> node
    if (!Helper::ReadNode(XmlStreamReader, "UserConfig")) {
        return false;
    }

    // read the file lists
    if (!m_UserConfigList.DeserializeContent(XmlStreamReader, CompleteData)) {
        return false;
    }

    // read the <UserReport> node
    if (!Helper::ReadNode(XmlStreamReader, "UserReport")) {
        return false;
    }

    // read the file lists
    if (!m_UserReportList.DeserializeContent(XmlStreamReader, CompleteData)) {
        return false;
    }

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // Do Nothing as the m_ExportConfigurationType is derived from m_ID
    }
    //======NODE=END====Temporary Data Variables=========================

    return true;
}



/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ExportConfiguration = CUserConfiguration class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CUserConfiguration& ExportConfiguration)
{
    CUserConfiguration* p_TempExportConfiguration = const_cast<CUserConfiguration*>(&ExportConfiguration);

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempExportConfiguration->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CUserConfiguration::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the QDataStream
 *  \iparam ExportConfiguration = CUserConfiguration class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CUserConfiguration& ExportConfiguration)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

    if (!ExportConfiguration.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CUserConfiguration::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceExportConfiguration = CUserConfiguration class object
 *
 *  \return CUserConfiguration Class Object
 */
/****************************************************************************/
CUserConfiguration& CUserConfiguration::operator=(const CUserConfiguration& SourceExportConfiguration)
{
    // make sure not same object
    if (this != &SourceExportConfiguration)
    {
        QByteArray TempByteArray;
        CUserConfiguration* p_TempExportConfiguration = const_cast<CUserConfiguration*>(&SourceExportConfiguration);
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        TempByteArray.clear();
        DataStream << *p_TempExportConfiguration;
        (void)DataStream.device()->reset(); //to avoid lint-534
        DataStream >> *this;
    }
    return *this;
}

}  // namespace DataManager


/****************************************************************************/
/*! \file ServiceConfiguration.cpp
 *
 *  \brief Implementation file for class CServiceConfiguration.
 *         This class reads the "Service" tag information from
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

#include "DataManager/Containers/ExportConfiguration/Include/ServiceConfiguration.h"
#include <Global/Include/EventObject.h>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Default Constructor
 *
 */
/****************************************************************************/
CServiceConfiguration::CServiceConfiguration()
{
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ExportConfiguration = Instance of the CServiceConfiguration class
 *
 *  \return
 */
/****************************************************************************/
CServiceConfiguration::CServiceConfiguration(const CServiceConfiguration& ExportConfiguration)
{
    CServiceConfiguration* p_TempExportConfiguration = const_cast<CServiceConfiguration*>(&ExportConfiguration);
    *this = *p_TempExportConfiguration;
}

/****************************************************************************/
/*!
 *  \brief Writes from the CServiceConfiguration object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the QXmlStreamWriter
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CServiceConfiguration::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    bool Result = true;
    // write the <ServiceConfig> tag
    XmlStreamWriter.writeStartElement("ServiceConfig");

    // serialize the service configuration
    if (!m_ServiceConfigList.SerializeContent(XmlStreamWriter, CompleteData)) {
        /// \todo need to write error handling code
        return false;
    }

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData) {
        // Do Nothing as the m_ExportConfigurationType is derived from m_ID
    }
    //======NODE=END====Temporary Data Variables=========================

    XmlStreamWriter.writeEndElement(); //ServiceConfig

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XmlStreamReader to CServiceConfiguration object.
 *
 *  \iparam XmlStreamReader = Instance of the XmlStreamReader.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CServiceConfiguration::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    // read the <ServiceConfig> node
    if(!Helper::ReadNode(XmlStreamReader, "ServiceConfig")) {
        return false;
    }

    // deserialize the service configuration - read the file lists
    if (!m_ServiceConfigList.DeserializeContent(XmlStreamReader, CompleteData)) {
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
 *  \iparam ExportConfiguration = CServiceConfiguration class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CServiceConfiguration& ExportConfiguration)
{
    CServiceConfiguration* p_TempExportConfiguration = const_cast<CServiceConfiguration*>(&ExportConfiguration);

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempExportConfiguration->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CServiceConfiguration::Operator Streaming (SerializeContent) failed.";
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
 *  \iparam ExportConfiguration = CServiceConfiguration class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CServiceConfiguration& ExportConfiguration)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

    if (!ExportConfiguration.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CServiceConfiguration::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceExportConfiguration = CServiceConfiguration class object
 *
 *  \return CServiceConfiguration Class Object
 */
/****************************************************************************/
CServiceConfiguration& CServiceConfiguration::operator=(const CServiceConfiguration& SourceExportConfiguration)
{
    // make sure not same object
    if (this != &SourceExportConfiguration)
    {
        QByteArray TempByteArray;
        CServiceConfiguration* p_TempExportConfiguration = const_cast<CServiceConfiguration*>(&SourceExportConfiguration);
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


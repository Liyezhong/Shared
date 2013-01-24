/****************************************************************************/
/*! \file DeviceConfiguration.cpp
 *
 *  \brief DeviceConfiguration class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-09-04
 *   $Author:  $ Ningu
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

#include "DataManager/Containers/DeviceConfiguration/Include/DeviceConfiguration.h"
#include "DataManager/Helper/Include/DataManagerEventCodes.h"
#include "Global/Include/EventObject.h"


namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Constructor
 */
/****************************************************************************/
CDeviceConfiguration::CDeviceConfiguration() :
    m_Version("")
//    m_StainerDeviceName("ST 8200"),
//    m_StainerSerialNumber(""),
//    m_CoverSlipperDeviceName("CV 8020"),
//    m_WorkStation(false),
//    m_HeatedCuevettesAvailable(false),
//    m_CameraSlideIdAvailable(false)

{
    // set default values
    m_LanguageList.clear();
    SetDefaultAttributes();
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam DeviceConfig = Instance of the CDeviceConfiguration class
 *
 *  \return
 */
/****************************************************************************/
CDeviceConfiguration::CDeviceConfiguration(const CDeviceConfiguration& DeviceConfig)
{
    // remove the constant using type cast
    CDeviceConfiguration* p_TempDeviceConfig = const_cast<CDeviceConfiguration*>(&DeviceConfig);
    // do a deep copy of the data
    *this = *p_TempDeviceConfig;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CDeviceConfiguration::~CDeviceConfiguration()
{
}

/****************************************************************************/
/*!
 *  \brief Set the default values of the local variables
 */
/****************************************************************************/
void CDeviceConfiguration::SetDefaultAttributes()
{
    m_Version                       = "1";   
//    m_StainerDeviceName             = "ST 8200";
//    m_StainerSerialNumber           = "12345678";
//    m_CoverSlipperDeviceName        = "CV 8020";
//    m_WorkStation                   = false;
//    m_HeatedCuevettesAvailable      = false;
//    m_CameraSlideIdAvailable        = false;
    /*m_LanguageList.append("English");
    m_LanguageList.append("German");
    m_LanguageList.append("French");
    m_LanguageList.append("Italian");
    m_LanguageList.append("Spanish");
    m_LanguageList.append("Portuguese");*/

}

/****************************************************************************/
/*!
 *  \brief Writes from the CDeviceConfiguration object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the QXmlStreamWriter
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDeviceConfiguration::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    // write the document type declaration
    XmlStreamWriter.writeStartElement("Device");
    XmlStreamWriter.writeAttribute("Version", GetVersion());

    QHashIterator<QString, QString> i(m_ValueList);
    while (i.hasNext())
    {
        i.next();
        XmlStreamWriter.writeAttribute(i.key(), i.value());
    }
    XmlStreamWriter.writeEndElement();

    if (CompleteData) {
        // write Language list
        XmlStreamWriter.writeStartElement("LanguageList");
        XmlStreamWriter.writeAttribute("Languages", m_LanguageList.join(","));
        XmlStreamWriter.writeEndElement();
    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CDeviceConfiguration object.
 *
 *  \iparam XmlStreamReader = Instance of the QXmlStreamReader.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDeviceConfiguration::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{            
    while ((XmlStreamReader.name() != "Device") && (!XmlStreamReader.atEnd()))
    {
        XmlStreamReader.readNextStartElement();
    }

    if (XmlStreamReader.name() != "Device")
    {
        qDebug() << "CDeviceConfiguration::DeserializeContent, DeviceConfiguration not found";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "Device", true);
        return false;
    }

    // Read attribute Version
    if (!XmlStreamReader.attributes().hasAttribute("Version")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "Version", true);
        return false;
    }
    SetVersion(XmlStreamReader.attributes().value("Version").toString());
    // Read attribute SLIDEIDCAMERA
    if (!XmlStreamReader.attributes().hasAttribute("SLIDEIDCAMERA")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "SLIDEIDCAMERA", true);
        return false;
    }
    // Read attribute WORKSTATIONMODE
    if (!XmlStreamReader.attributes().hasAttribute("WORKSTATIONMODE")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "WORKSTATIONMODE", true);
        return false;
    }
    // Read attribute DEVICENAME
    if (!XmlStreamReader.attributes().hasAttribute("DEVICENAME")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "DEVICENAME", true);
        return false;
    }
    // Read attribute SERIALNUMBER
    if (!XmlStreamReader.attributes().hasAttribute("SERIALNUMBER")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "SERIALNUMBER", true);
        return false;
    }
    // Read attribute COVERSLIPPERNAME
    if (!XmlStreamReader.attributes().hasAttribute("COVERSLIPPERNAME")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "COVERSLIPPERNAME", true);
        return false;
    }
    // Read attribute HEATEDCUEVETTES
    if (!XmlStreamReader.attributes().hasAttribute("HEATEDCUEVETTES")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "HEATEDCUEVETTES", true);
        return false;
    }
    if (!XmlStreamReader.atEnd() && !XmlStreamReader.hasError())
    {
        QXmlStreamAttributes attributes = XmlStreamReader.attributes();
        foreach (QXmlStreamAttribute attribute, attributes)
        {
            m_ValueList.insert(attribute.name().toString().toUpper(), attribute.value().toString());
        }
    }

    if(CompleteData) {
        // read complete data from the xml
        if (!ReadCompleteData(XmlStreamReader)) {
            return false;
        }
    }

    return true;
}
/****************************************************************************/
/*!
 *  \brief Reads the complete information from the xml stream.
 *
 *  \iparam XmlStreamReader  = XML stream to read from
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDeviceConfiguration::ReadCompleteData(QXmlStreamReader& XmlStreamReader)
{
//    // read Language list
//    if (!Helper::ReadNode(XmlStreamReader, "LanguageList")) {
//        qDebug() << "CDeviceConfiguration::ReadCompleteData: abort reading. Node not found: LanguageList";
//        return false;
//    }

    while ((XmlStreamReader.name() != "LanguageList") && (!XmlStreamReader.atEnd()))
    {
        XmlStreamReader.readNextStartElement();
    }

    if (!XmlStreamReader.attributes().hasAttribute("Languages")) {
        qDebug()<<"### attribute <Languages> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "Languages", true);
        return false;
    }
    //retrieve station id list
    QStringList LanguageList = XmlStreamReader.attributes().value("Languages").toString().split(",",  QString::SkipEmptyParts);
    SetLanguageList(LanguageList);
    return true;
}
/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam DeviceConfig = CDeviceConfiguration class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CDeviceConfiguration& DeviceConfig)
{
    // remove the constant and store it in a local variable
    CDeviceConfiguration* p_TempDeviceConfig = const_cast<CDeviceConfiguration*>(&DeviceConfig);
    QXmlStreamWriter XmlStreamWriter; ///< Writer for the XML

    // set the IO device
    QIODevice* IODevice = OutDataStream.device();

    XmlStreamWriter.setDevice(IODevice);

    // start the XML Document
    XmlStreamWriter.writeStartDocument();

    if (!p_TempDeviceConfig->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CDeviceConfiguration::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        //THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
        const_cast<CDeviceConfiguration &>(DeviceConfig).m_ErrorHash.insert(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration", true);
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
 *  \iparam DeviceConfig = CDeviceConfiguration class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >>(QDataStream& InDataStream, CDeviceConfiguration& DeviceConfig)
{
    QXmlStreamReader XmlStreamReader; ///< Reader for the XML
    // store the IO device
    QIODevice* IODevice = InDataStream.device();

    XmlStreamReader.setDevice(IODevice);

    // deserialize the content of the xml
    if (!DeviceConfig.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CDeviceConfiguration::Operator Streaming (DeSerializeContent) failed.";
        DeviceConfig.m_ErrorHash.insert(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration", true);
        // throws an exception
        //THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam DeviceConfig = CDeviceConfiguration class object
 *
 *  \return CDeviceConfiguration Class Object
 */
/****************************************************************************/
CDeviceConfiguration& CDeviceConfiguration::operator=(const CDeviceConfiguration& DeviceConfig)
{
    // make sure not same object
    if (this != &DeviceConfig)
    {
        // create the byte array
        QByteArray* p_TempByteArray = new QByteArray();
        // create the data stream to write into a file
        QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
        (void)DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0)); //to avoid lint-534
        p_TempByteArray->clear();
        // write the data into data stream from source
        DataStream << DeviceConfig;
        // reset the IO device pointer to starting position
        (void)DataStream.device()->reset(); //to avoid lint 534
        // read the data from data stream to destination object
        DataStream >> *this;

        delete p_TempByteArray;
    }
    return *this;
}

} // end namespace DataManager

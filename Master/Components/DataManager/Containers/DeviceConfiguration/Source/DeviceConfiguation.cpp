/****************************************************************************/
/*! \file Components/DataManager/Containers/DeviceConfiguration/Source/DeviceConfiguation.cpp
 *
 *  \brief DeviceConfiguration class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-09-04
 *   $Author:  $ Ningu123, Ramya GJ
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
    CopyFromOther(DeviceConfig);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \note  Method for internal use only
 *
 *  \iparam DeviceConfiguration = Instance of the DeviceConfiguration class
 *
 *  \return
 */
/****************************************************************************/
void CDeviceConfiguration::CopyFromOther(const CDeviceConfiguration &DeviceConfiguration)
{
    CDeviceConfiguration &OtherDeviceConfig = const_cast<CDeviceConfiguration &>(DeviceConfiguration);
    m_Version  = OtherDeviceConfig.GetVersion();
    m_LanguageList     = OtherDeviceConfig.GetLanguageList();
    m_ValueList = OtherDeviceConfig.GetValueList();

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
    m_Version = "1";
}
/****************************************************************************/
/*!
 *  \brief Returns the Value list
 *
 *  \return ValueList
 */
/****************************************************************************/
QMap<QString ,QString> CDeviceConfiguration::GetValueList() const
{
    return m_ValueList;
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
//    XmlStreamWriter.writeAttribute("Version", GetVersion());

    QMapIterator<QString, QString> i(m_ValueList);
    while (i.hasNext())
    {
        (void)i.next();
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
        (void)XmlStreamReader.readNextStartElement();
    }

    if (XmlStreamReader.name() != "Device")
    {
        qDebug() << "CDeviceConfiguration::DeserializeContent, DeviceConfiguration not found";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "Device", true);
        return false;
    }

    // Read attribute Version
    if (!XmlStreamReader.attributes().hasAttribute("VERSION")) {
        qDebug() << "### attribute <Version> is missing => abort reading";
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_ERROR_XML_ATTRIBUTE_NOT_FOUND, Global::tTranslatableStringList() << "Version", true);
        return false;
    }
    SetVersion(XmlStreamReader.attributes().value("VERSION").toString());

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
    while ((XmlStreamReader.name() != "LanguageList") && (!XmlStreamReader.atEnd()))
    {
        (void)XmlStreamReader.readNextStartElement();
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
        const_cast<CDeviceConfiguration &>(DeviceConfig).m_ErrorMap.insert(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMOUT_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration", true);
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
        DeviceConfig.m_ErrorMap.insert(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration");
        Global::EventObject::Instance().RaiseEvent(EVENT_DM_STREAMIN_FAILED, Global::tTranslatableStringList() << "DeviceConfiguration", true);
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
        CopyFromOther(DeviceConfig);
    }
    return *this;
}

} // end namespace DataManager

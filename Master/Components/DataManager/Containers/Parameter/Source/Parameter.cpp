/****************************************************************************/
/*! \file Parameter.cpp
 *
 *  \brief Parameter implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-03-01
 *   $Author:  $ Thirumalesha R
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
#include <QBuffer>
#include <QXmlStreamReader>
#include <iostream>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"
#include "DataManager/Containers/Parameter/Include/Parameter.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CParameter::CParameter() :
    m_ParameterKey("")
{
    m_ParameterKeyList.clear();
}

/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam ParameterKey
 */
/****************************************************************************/
CParameter::CParameter(QString ParameterKey) :
    m_ParameterKey(ParameterKey)
{
    m_ParameterKeyList.clear();
}
/****************************************************************************/
/*!
 *  \brief Constructor
 *  \iparam ParameterKey = Parameter Key
 *  \iparam ParameterDefaultValue = Parameter DefaultValue
 *  \iparam ParameterOffset = Parameter Offset;
 */
/****************************************************************************/
CParameter::CParameter(QString ParameterKey,int ParameterDefaultValue,int ParameterOffset) :
    m_ParameterKey(ParameterKey),m_ParameterDefaultValue(ParameterDefaultValue),m_ParameterOffset(ParameterOffset)
{
    m_ParameterKeyList.clear();
}
/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam Parameter = Instance of the CParameter class
 */
/****************************************************************************/
CParameter::CParameter(const CParameter& Parameter)
{
    CParameter* p_TempParameter = const_cast<CParameter*>(&Parameter);
    *this = *p_TempParameter;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CParameter::~CParameter()
{
}

/****************************************************************************/
/*!
 *  \brief Writes from the CParameter object to a IODevice.
 *
 *  \iparam XmlStreamWriter = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CParameter::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    QString StringValue;

    bool Result = true;

    XmlStreamWriter.writeStartElement("Parameter");
    XmlStreamWriter.writeAttribute("Key", GetParameterKey());

    (void)StringValue.setNum(GetParameterDefaultValue()); //to suppress lint-534
    StringValue.append("%");
    XmlStreamWriter.writeAttribute("DefaultValue", StringValue);

    (void)StringValue.setNum(GetParameterOffset()); //to suppress lint-534
    StringValue.append("%");
    XmlStreamWriter.writeAttribute("OffSet", StringValue );

    XmlStreamWriter.writeEndElement(); // Parameter

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the IODevice to CParameter object.
 *
 *  \iparam XmlStreamReader = Instance of the IODevice might be Buffer or File.
 *  \iparam CompleteData = bool type if true gives Complete data of object
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CParameter::DeserializeContent(QXmlStreamReader& XmlStreamReader,bool CompleteData)
{
    // Key
    if (!XmlStreamReader.attributes().hasAttribute("Key")) {
        qDebug() << "### attribute <Key> is missing => abort reading";
        return false;
    }
    SetParameterKey(XmlStreamReader.attributes().value("Key").toString());

    // Counter
    if (!XmlStreamReader.attributes().hasAttribute("DefaultValue")) {
        qDebug() << "### attribute <Counter> is missing => abort reading";
        return false;
    }
    SetParameterDefaultValue(XmlStreamReader.attributes().value("DefaultValue").toString().toInt());

    // Counter
    if (!XmlStreamReader.attributes().hasAttribute("Offset")) {
        qDebug() << "### attribute <Counter> is missing => abort reading";
        return false;
    }
    SetParameterOffset(XmlStreamReader.attributes().value("Offset").toString().toInt());

    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam Parameter = CParameter class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator << (QDataStream& OutDataStream, const CParameter& Parameter)
{
    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    CParameter* p_TempParameter = const_cast<CParameter*>(&Parameter);
    if (!p_TempParameter->SerializeContent(XmlStreamWriter, true)) {
        qDebug() << "CParameter::Operator Streaming (SerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }

    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam Parameter = CDeviceParameter class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >> (QDataStream& InDataStream, CParameter& Parameter)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

    if (!Helper::ReadNode(XmlStreamReader, QString("Parameter"))) {
        qDebug() << "CDeviceParameter::Operator Streaming (DeSerializeContent) Node not found: Parameter";
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    if (!Parameter.DeserializeContent(XmlStreamReader, true)) {
        qDebug() << "CDeviceParameter::Operator Streaming (DeSerializeContent) failed.";
        // throws an exception
        THROWARG(Global::EVENT_GLOBAL_UNKNOWN_STRING_ID, Global::tTranslatableStringList() << FILE_LINE);
    }
    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceParameter = CParameter class object
 *
 *  \return CParameter Class Object
 */
/****************************************************************************/
CParameter& CParameter::operator = (const CParameter& SourceParameter)
{
    // make sure not same object
    if (this != &SourceParameter)
    {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        TempByteArray.clear();
        DataStream << SourceParameter;
        (void)DataStream.device()->reset();
        DataStream >> *this;
    }
    return *this;
}

}//End of namespace DataManager

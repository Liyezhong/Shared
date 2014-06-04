/****************************************************************************/
/*! \file RCDataItem.cpp
 *
 *  \brief RCDataItem class implementation.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-07-08
 *  $Author:    $ Ramya GJ
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
//lint -sem(DataManager::CRCDataItem::CopyFromOther,initializer)
#include <QDebug>
#include <QString>
#include <QBuffer>
#include <QXmlStreamReader>

#include <Global/Include/Exception.h>
#include <Global/Include/Utils.h>
#include <DataManager/Containers/RCConfiguration/Include/RCDataItem.h>
#include <Global/Include/EventObject.h>
#include <DataManager/Helper/Include/DataManagerEventCodes.h>
#include "DataManager/Helper/Include/Helper.h"

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief Default Constructor for CRCDataItem class.
*/
/****************************************************************************/
CRCDataItem::CRCDataItem() :
    m_Name(""),
    m_Type(RemoteCare::RDI_Undefined),
    m_Quality(RemoteCare::RDI_DataUncertain),
    m_Value("")
{
}

/****************************************************************************/
/*!
 *  \brief Parameterized constructor.
 *
 *  \iparam Name = data item name
 */
/****************************************************************************/
CRCDataItem::CRCDataItem(const QString Name) :
    m_Name(Name),
    m_Type(RemoteCare::RDI_Undefined),
    m_Quality(RemoteCare::RDI_DataUncertain),
    m_Value("")
{
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam RCDataItem = Instance of the CRCDataItem class
 *
 *  \return
 */
/****************************************************************************/
CRCDataItem::CRCDataItem(const CRCDataItem& RCDataItem)
{
    CopyFromOther(RCDataItem);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
.*  \note  Method for internal use only
 *
 *  \iparam RCDataItem = Instance of the CRCDataItem class
 *
 *  \return
 */
/****************************************************************************/
void CRCDataItem::CopyFromOther(const CRCDataItem &RCDataItem)
{
    CRCDataItem &OtherRCData = const_cast<CRCDataItem &>(RCDataItem);
    m_Name = OtherRCData.m_Name;
    m_Type = OtherRCData.m_Type;
    m_Quality = OtherRCData.m_Quality;
    m_Value = OtherRCData.m_Value;

}
/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CRCDataItem::~CRCDataItem()
{
}

/****************************************************************************/
/*!
 *  \brief Writes to the CRCDataItem object file from a XmlStreamWriter.
 *
 *  \iparam XmlStreamWriter = Instance of XmlStreamWriter Class
 *  \iparam CompleteData = Complete class Data , true or false
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCDataItem::SerializeContent(QXmlStreamWriter& XmlStreamWriter, bool CompleteData)
{
    bool Result = true;

    XmlStreamWriter.writeStartElement("DataItem"); //Data Item

    XmlStreamWriter.writeAttribute("Name", GetName());    // Name

    int IndexOfRDIType = (int)GetType();
    if ((IndexOfRDIType < 0) || (IndexOfRDIType >= NUMBER_OF_RDI_TYPE))
    {
        IndexOfRDIType = (int)RemoteCare::RDI_TypeInvalid;
        Result = false;
    }
    XmlStreamWriter.writeAttribute("Type", NameOfRDIType[IndexOfRDIType]);

    int IndexOfRDIQuality = (int)GetQuality();
    if ((IndexOfRDIQuality < 0) || (IndexOfRDIQuality >= NUMBER_OF_RDI_QUALITY))
    {
        IndexOfRDIQuality = (int)RemoteCare::RDI_DataInvalid;
        Result = false;
    }
    XmlStreamWriter.writeAttribute("Quality", NameOfRDIQuality[IndexOfRDIQuality]);

    XmlStreamWriter.writeAttribute("Value", GetValue()); // value

    //======NODE=======Temporary Data Variables=========================
    if (CompleteData)
    {
        // No Temporary Data elements
    }
    //======NODE=END====Temporary Data Variables=========================
    XmlStreamWriter.writeEndElement(); //RCDataItem

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads from the XML file to a CRCDataItem Class.
 *
 *  \iparam XmlStreamReader = Instance of XmlStreamReader Class
 *  \iparam CompleteData = Complete class Data , true or false
 *
 *  \return True or False
 */
/****************************************************************************/
bool CRCDataItem::DeserializeContent(QXmlStreamReader& XmlStreamReader, bool CompleteData)
{
    // name
    if (!XmlStreamReader.attributes().hasAttribute("Name"))
    {
        qDebug() << "### attribute <Name> is missing => abort reading";
        return false;
    }
    SetName(XmlStreamReader.attributes().value("Name").toString());

    // read type
    if (!XmlStreamReader.attributes().hasAttribute("Type"))
    {
        qDebug() << "CRCDataItem::DeserializeContent(:### attribute <Type> is missing => abort reading";
        return false;
    }
    QString Type = XmlStreamReader.attributes().value("Type").toString();
    if (NameOfRDIType[RemoteCare::RDI_Analog] == Type)
    {
        SetType(RemoteCare::RDI_Analog);
    }
    else if (NameOfRDIType[RemoteCare::RDI_Digital] == Type)
    {
        SetType(RemoteCare::RDI_Digital);
    }
    else if (NameOfRDIType[RemoteCare::RDI_String] == Type)
    {
        SetType(RemoteCare::RDI_String);
    }
    else if (NameOfRDIType[RemoteCare::RDI_Undefined] == Type)
    {
        SetType(RemoteCare::RDI_Undefined);
    }
    else
    {
        SetType(RemoteCare::RDI_TypeInvalid);
    }

    // read quality
    if (!XmlStreamReader.attributes().hasAttribute("Quality"))
    {
        qDebug() << "CRCDataItem::DeserializeContent(:### attribute <Quality> is missing => abort reading";
        return false;
    }
    QString Quality = XmlStreamReader.attributes().value("Quality").toString();
    if (NameOfRDIQuality[RemoteCare::RDI_DataGood] == Quality)
    {
        SetQuality(RemoteCare::RDI_DataGood);
    }
    else if (NameOfRDIQuality[RemoteCare::RDI_DataBad] == Quality)
    {
        SetQuality(RemoteCare::RDI_DataBad);
    }
    else if (NameOfRDIQuality[RemoteCare::RDI_DataUncertain] == Quality)
    {
        SetQuality(RemoteCare::RDI_DataUncertain);
    }
    else
    {
        SetQuality(RemoteCare::RDI_DataInvalid);
    }

    // value
    if (!XmlStreamReader.attributes().hasAttribute("Value"))
    {
        qDebug() << "### attribute <Value> is missing => abort reading";
        return false;
    }
    SetValue(XmlStreamReader.attributes().value("Value").toString());

    //======NODE=======Temporary Data Variables=========================
    if(CompleteData)
    {

    }
    //======NODE=END====Temporary Data Variables=========================
    return true;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam RCDataItem = RCDataItem class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator <<(QDataStream& OutDataStream, const CRCDataItem& RCDataItem)
{
    CRCDataItem *p_TempRCDataItem = const_cast<CRCDataItem*>(&RCDataItem);

    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(OutDataStream.device());

    if (!p_TempRCDataItem->SerializeContent(XmlStreamWriter, true))
    {
        qDebug() << "CRCDataItem::Operator Streaming (SerializeContent) failed.";
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam RCDataItem = CRCDataItem class object
 *
 *  \return Input Stream
 */
/****************************************************************************/

QDataStream& operator >>(QDataStream& InDataStream, CRCDataItem& RCDataItem)
{
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(InDataStream.device());

    if (!Helper::ReadNode(XmlStreamReader, QString("DataItem")))
    {
        qDebug() << "CRCDataItem::Operator Streaming (DeSerializeContent) Node not found: DataItem";
    }

    if (!RCDataItem.DeserializeContent(XmlStreamReader, true))
    {
        qDebug() << "CRCDataItem::Operator Streaming (DeSerializeContent) failed.";
    }

    return InDataStream;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam SourceRCDataItem = CRCDataItem class object
 *
 *  \return CRCDataItem Class Object
 */
/****************************************************************************/

CRCDataItem& CRCDataItem::operator=(const CRCDataItem& SourceRCDataItem)
{
    // make sure not same object
    if (this != &SourceRCDataItem)
    {
       CopyFromOther(SourceRCDataItem);
    }
    return *this;
}

}  // namespace DataManager


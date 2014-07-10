/****************************************************************************/
/*! \file CmdRCNotifyDataItem.cpp
 *
 *  \brief CmdRCNotifyDataItem command implementation.
 *
 *   $Version: $ 1.0
 *   $Date:    $ 2014-03-13
 *   $Author:  $ Ramya GJ
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

#include <NetCommands/Include/CmdRCNotifyDataItem.h>

#include <QDataStream>
#include <QDebug>
//lint -e1565
namespace NetCommands {

QString CmdRCNotifyDataItem::NAME = "NetCommands::CmdRCNotifyDataItem";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
CmdRCNotifyDataItem::CmdRCNotifyDataItem() :
        Command(Command::NOTIMEOUT),
        m_Name(""),
        m_Type(RemoteCare::RDI_Undefined),
        m_Quality(RemoteCare::RDI_DataUncertain),
        m_Value(""),
        m_Timestamp("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam timeout = Timeout of this command.
 *  \iparam name = data item's name (Axeda parameter)
 *  \iparam type = data item's type (Axeda parameter)
 *  \iparam quality = data item's quality (Axeda parameter)
 *  \iparam value = the value of the data item (Axeda parameter)
 *  \iparam timestamp = timestamp of the data item logging (Axeda parameter)
 *
 ****************************************************************************/
CmdRCNotifyDataItem::CmdRCNotifyDataItem(const int& timeout, const QString& name, const RemoteCare::RCDataItemType_t& type, \
                                   const RemoteCare::RCDataItemQuality_t& quality, const QString& value, const QString& timestamp) :
        Command(timeout),
        m_Name(name),
        m_Type(type),
        m_Quality(quality),
        m_Value(value),
        m_Timestamp(timestamp)
{
}

/****************************************************************************/
/*!
 *  \brief   Assignment operator
 *
 *  \iparam rOther = instance to assign
 *
 *  \return
 *
 ****************************************************************************/
const CmdRCNotifyDataItem & CmdRCNotifyDataItem::operator = (const CmdRCNotifyDataItem& rOther)
{
    if (this != &rOther) {
        Command::operator = (rOther);
        CopyFrom(rOther);
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 *
 *
 ****************************************************************************/
CmdRCNotifyDataItem::~CmdRCNotifyDataItem()
{
}

/****************************************************************************/
/*!
 *  \brief   Copy data from another class instance
 *
 *  \iparam rOther = instance to copy from
 *
 ****************************************************************************/
void CmdRCNotifyDataItem::CopyFrom(const CmdRCNotifyDataItem& rOther)
{
    m_Name = rOther.m_Name;
    m_Type = rOther.m_Type;
    m_Quality = rOther.m_Quality;
    m_Value = rOther.m_Value;
    m_Timestamp = rOther.m_Timestamp;
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 *
 ****************************************************************************/
QString CmdRCNotifyDataItem::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get DataItem name
 *
 *  \return  DataItem name
 *
 ****************************************************************************/
QString CmdRCNotifyDataItem::GetDataItemName() const
{
    return m_Name;
}

/****************************************************************************/
/*!
 *  \brief   Get DataItem type (digital/analog/string)
 *
 *  \return  DataItem type
 *
 ****************************************************************************/
RemoteCare::RCDataItemType_t CmdRCNotifyDataItem::GetDataItemType() const
{
    return m_Type;
}

/****************************************************************************/
/*!
 *  \brief   Get DataItem quality (good/bad/uncertain)
 *
 *  \return  DataItem quality
 *
 ****************************************************************************/
RemoteCare::RCDataItemQuality_t CmdRCNotifyDataItem::GetDataItemQuality() const
{
    return m_Quality;
}

/****************************************************************************/
/*!
 *  \brief   Get DataItem value
 *
 *  \return  DataItem value
 *
 ****************************************************************************/
QString CmdRCNotifyDataItem::GetDataItemValue() const
{
    return m_Value;
}

/****************************************************************************/
/*!
 *  \brief   Get DataItem timestamp
 *
 *  \return  DataItem timestamp
 *
 ****************************************************************************/
QString CmdRCNotifyDataItem::GetDataItemTimestamp() const
{
    return m_Timestamp;
}

} // end namespace

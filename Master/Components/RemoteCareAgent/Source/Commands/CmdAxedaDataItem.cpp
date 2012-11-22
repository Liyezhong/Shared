/****************************************************************************/
/*! \file CmdAxedaDataItem.cpp
 *
 *  \brief CmdAxedaDataItem command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 08.09.2011
 *   $Author:  $ Y.Novak
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

#include <RemoteCareAgent/Include/Commands/CmdAxedaDataItem.h>

#include <QDataStream>
#include <QDebug>

namespace RCAgentNamespace {

QString CmdAxedaDataItem::NAME = "RCAgentNamespace::CmdAxedaDataItem";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
CmdAxedaDataItem::CmdAxedaDataItem() :
        Command(Command::NOTIMEOUT),
        m_Name(""),
        m_Type(ADI_Undefined),
        m_Quality(ADI_DataUncertain),
        m_Value(""),
        m_Timestamp("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *
 ****************************************************************************/
CmdAxedaDataItem::CmdAxedaDataItem(int timeout) :
        Command(timeout),
        m_Name(""),
        m_Type(ADI_Undefined),
        m_Quality(ADI_DataUncertain),
        m_Value(""),
        m_Timestamp("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *  \param[in] name = data item's name (Axeda parameter)
 *  \param[in] type = data item's type (Axeda parameter)
 *  \param[in] quality = data item's quality (Axeda parameter)
 *  \param[in] value = the value of the data item (Axeda parameter)
 *  \param[in] timestamp = timestamp of the data item logging (Axeda parameter)
 *
 ****************************************************************************/
CmdAxedaDataItem::CmdAxedaDataItem(int timeout, const QString &name, AxedaDataItemType_t type, \
                                   AxedaDataItemQuality_t quality, const QString &value, const QString &timestamp) :
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
 *  \brief   Copy Constructor
 *
 *  \param[in] rOther = instance to copy
 *
 ****************************************************************************/
CmdAxedaDataItem::CmdAxedaDataItem(const CmdAxedaDataItem &rOther) :
        Command(rOther)
{
    CopyFrom(rOther);
}

/****************************************************************************/
/*!
 *  \brief   Assignment operator
 *
 *  \param[in] rOther = instance to assign
 *
 ****************************************************************************/
const CmdAxedaDataItem & CmdAxedaDataItem::operator = (const CmdAxedaDataItem &rOther)
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
CmdAxedaDataItem::~CmdAxedaDataItem()
{
}

/****************************************************************************/
/*!
 *  \brief   Copy data from another class instance
 *
 *  \param[in] rOther = instance to copy from
 *
 ****************************************************************************/
void CmdAxedaDataItem::CopyFrom(const CmdAxedaDataItem &rOther)
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
QString CmdAxedaDataItem::GetName() const
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
QString CmdAxedaDataItem::GetDataItemName() const
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
AxedaDataItemType_t CmdAxedaDataItem::GetDataItemType() const
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
AxedaDataItemQuality_t CmdAxedaDataItem::GetDataItemQuality() const
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
QString CmdAxedaDataItem::GetDataItemValue() const
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
QString CmdAxedaDataItem::GetDataItemTimestamp() const
{
    return m_Timestamp;
}

} // end namespace

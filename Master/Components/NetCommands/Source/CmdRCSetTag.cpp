/****************************************************************************/
/*! \file CmdRCSetTag.cpp
 *
 *  \brief CmdRCSetTag command implementation.
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

#include <NetCommands/Include/CmdRCSetTag.h>

#include <QDataStream>
#include <QDebug>
//lint -e1565

namespace NetCommands {

QString CmdRCSetTag::NAME = "NetCommands::CmdRCSetTag";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
CmdRCSetTag::CmdRCSetTag() :
        Command(Command::NOTIMEOUT),
        m_Name(""),
        m_Type(RemoteCare::RDI_Undefined),
        m_Value("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam timeout = Timeout of this command.
 *  \iparam name = data item's name (Axeda parameter)
 *  \iparam type = data item's type (Axeda parameter)
 *  \iparam value = the value of the data item (Axeda parameter)
 *
 ****************************************************************************/
CmdRCSetTag::CmdRCSetTag(const int& timeout, const QString &name, const RemoteCare::RCDataItemType_t& type, const QString &value) :
        Command(timeout),
        m_Name(name),
        m_Type(type),
        m_Value(value)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam timeout = Timeout of this command.
 *
 ****************************************************************************/
CmdRCSetTag::CmdRCSetTag(const int& timeout) :
        Command(timeout),
        m_Name(""),
        m_Type(RemoteCare::RDI_Undefined),
        m_Value("")
{
}

/****************************************************************************/
/*!
 *  \brief   Copy Constructor
 *
 *  \iparam rOther = instance to copy
 *
 ****************************************************************************/
CmdRCSetTag::CmdRCSetTag(const CmdRCSetTag &rOther) :
        Command(rOther)
{
    CopyFrom(rOther);
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
const CmdRCSetTag & CmdRCSetTag::operator = (const CmdRCSetTag &rOther)
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
CmdRCSetTag::~CmdRCSetTag()
{
    try {
        qDebug() << "*** CmdRCSetTag \"" << m_Name << "\" destroyed ***";
    }
    CATCHALL_DTOR();
}

/****************************************************************************/
/*!
 *  \brief   Copy data from another class instance
 *
 *  \iparam rOther = instance to copy from
 *
 ****************************************************************************/
void CmdRCSetTag::CopyFrom(const CmdRCSetTag &rOther)
{
    m_Name = rOther.m_Name;
    m_Type = rOther.m_Type;
    m_Value = rOther.m_Value;
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 *
 ****************************************************************************/
QString CmdRCSetTag::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get Tag name
 *
 *  \return  Tag name
 *
 ****************************************************************************/
QString CmdRCSetTag::GetTagName() const
{
    return m_Name;
}

/****************************************************************************/
/*!
 *  \brief   Get Tag type (digital/analog/string)
 *
 *  \return  Tag type
 *
 ****************************************************************************/
RemoteCare::RCDataItemType_t CmdRCSetTag::GetTagType() const
{
    return m_Type;
}

/****************************************************************************/
/*!
 *  \brief   Get Tag value
 *
 *  \return  Tag value
 *
 ****************************************************************************/
QString CmdRCSetTag::GetTagValue() const
{
    return m_Value;
}

/****************************************************************************/
/*!
 *  \brief   Set command's data
 *
 *  \iparam type = type of data
 *  \iparam value = the data
 *
 ****************************************************************************/
void CmdRCSetTag::SetCommandData(const RemoteCare::RCDataItemType_t& type, const QString &value)
{
    m_Type = type;
    m_Value = value;
}

} // end namespace

/****************************************************************************/
/*! \file CmdAxedaSetTag.cpp
 *
 *  \brief CmdAxedaSetTag command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 15.09.2011
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

#include <RemoteCareAgent/Include/Commands/CmdAxedaSetTag.h>

#include <QDataStream>
#include <QDebug>

namespace RCAgentNamespace {

QString CmdAxedaSetTag::NAME = "RCAgentNamespace::CmdAxedaSetTag";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
CmdAxedaSetTag::CmdAxedaSetTag() :
        Command(Command::NOTIMEOUT),
        m_Name(""),
        m_Type(ADI_Undefined),
        m_Value("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *
 ****************************************************************************/
CmdAxedaSetTag::CmdAxedaSetTag(int timeout) :
        Command(timeout),
        m_Name(""),
        m_Type(ADI_Undefined),
        m_Value("")
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *  \param[in] name = data item's name (Axeda parameter)
 *  \param[in] type = data item's type (Axeda parameter)
 *  \param[in] value = the value of the data item (Axeda parameter)
 *
 ****************************************************************************/
CmdAxedaSetTag::CmdAxedaSetTag(int timeout, const QString &name, AxedaDataItemType_t type, const QString &value) :
        Command(timeout),
        m_Name(name),
        m_Type(type),
        m_Value(value)
{
}

/****************************************************************************/
/*!
 *  \brief   Copy Constructor
 *
 *  \param[in] rOther = instance to copy
 *
 ****************************************************************************/
CmdAxedaSetTag::CmdAxedaSetTag(const CmdAxedaSetTag &rOther) :
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
const CmdAxedaSetTag & CmdAxedaSetTag::operator = (const CmdAxedaSetTag &rOther)
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
CmdAxedaSetTag::~CmdAxedaSetTag()
{
    try {
        qDebug() << "*** CmdAxedaSetTag \"" << m_Name << "\" destroyed ***";
    }
    catch (...) {
        // 4 Lint
    }
}

/****************************************************************************/
/*!
 *  \brief   Copy data from another class instance
 *
 *  \param[in] rOther = instance to copy from
 *
 ****************************************************************************/
void CmdAxedaSetTag::CopyFrom(const CmdAxedaSetTag &rOther)
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
QString CmdAxedaSetTag::GetName() const
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
QString CmdAxedaSetTag::GetTagName() const
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
AxedaDataItemType_t CmdAxedaSetTag::GetTagType() const
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
QString CmdAxedaSetTag::GetTagValue() const
{
    return m_Value;
}

/****************************************************************************/
/*!
 *  \brief   Set command's data
 *
 *  \param[in] type = type of data
 *  \param[in] value = the data
 *
 ****************************************************************************/
void CmdAxedaSetTag::SetCommandData(AxedaDataItemType_t type, const QString &value)
{
    m_Type = type;
    m_Value = value;
}

} // end namespace

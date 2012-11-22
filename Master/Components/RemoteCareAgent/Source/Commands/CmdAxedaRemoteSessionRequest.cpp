/****************************************************************************/
/*! \file CmdAxedaRemoteSessionRequest.cpp
 *
 *  \brief CmdAxedaRemoteSessionRequest command implementation.
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

#include <RemoteCareAgent/Include/Commands/CmdAxedaRemoteSessionRequest.h>

#include <QDataStream>
#include <QDebug>

namespace RCAgentNamespace {

QString CmdAxedaRemoteSessionRequest::NAME = "RCAgentNamespace::CmdAxedaRemoteSessionRequest";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 ****************************************************************************/
CmdAxedaRemoteSessionRequest::CmdAxedaRemoteSessionRequest() :
        CmdAxedaSetTag(),
        m_ReqValue(ADI_DataError)
{
    m_Name = NAME;
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *
 ****************************************************************************/
CmdAxedaRemoteSessionRequest::CmdAxedaRemoteSessionRequest(int timeout) :
        CmdAxedaSetTag(timeout),
        m_ReqValue(ADI_DataError)
{
    m_Name = NAME;
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \param[in] timeout = Timeout of this command.
 *  \param[in] type = data item's type (Axeda parameter)
 *  \param[in] value = the value of the data item (Axeda parameter)
 *
 ****************************************************************************/
CmdAxedaRemoteSessionRequest::CmdAxedaRemoteSessionRequest(int timeout, AxedaDataItemType_t type, const QString &value) :
        CmdAxedaSetTag(timeout, NAME, type, value)
{
    SetCommandData(type, value);
}

/****************************************************************************/
/*!
 *  \brief   Copy Constructor
 *
 *  \param[in] rOther = instance to copy
 *
 ****************************************************************************/
CmdAxedaRemoteSessionRequest::CmdAxedaRemoteSessionRequest(const CmdAxedaRemoteSessionRequest &rOther) :
        CmdAxedaSetTag(rOther)
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
const CmdAxedaRemoteSessionRequest & CmdAxedaRemoteSessionRequest::operator = (const CmdAxedaRemoteSessionRequest &rOther)
{
    if (this != &rOther) {
        CmdAxedaSetTag::operator = (rOther);
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
CmdAxedaRemoteSessionRequest::~CmdAxedaRemoteSessionRequest()
{
}

/****************************************************************************/
/*!
 *  \brief   Copy data from another class instance
 *
 *  \param[in] rOther = instance to copy from
 *
 ****************************************************************************/
void CmdAxedaRemoteSessionRequest::CopyFrom(const CmdAxedaRemoteSessionRequest &rOther)
{
    m_ReqValue = rOther.m_ReqValue;
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 *
 ****************************************************************************/
QString CmdAxedaRemoteSessionRequest::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get Request value
 *
 *  \return  Request value
 *
 ****************************************************************************/
AxedaDigitalDataItemRange_t CmdAxedaRemoteSessionRequest::GetRequestValue() const
{
    return m_ReqValue;
}

/****************************************************************************/
/*!
 *  \brief   Set command's data
 *
 *  \param[in] type = type of data
 *  \param[in] value = the data
 *
 ****************************************************************************/
void CmdAxedaRemoteSessionRequest::SetCommandData(AxedaDataItemType_t type, const QString &value)
{
    if (type == ADI_Digital) {
        bool ok = false;
        m_ReqValue = (AxedaDigitalDataItemRange_t)value.toUShort(&ok, 10);
        if ((!ok) || (m_ReqValue > 1)) {
            m_ReqValue = ADI_DataError;
        }
    }
    else {
        m_ReqValue = ADI_DataError;
    }
}

} // end namespace

/****************************************************************************/
/*! \file CmdAxedaRemoteSessionRequest.h
 *
 *  \brief CmdAxedaRemoteSessionRequest command definition.
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

#ifndef RCAGENTNAMESPACE_CMDAXEDAREMOTESESSIONREQUEST_H
#define RCAGENTNAMESPACE_CMDAXEDAREMOTESESSIONREQUEST_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Command.h>
#include <RemoteCareAgent/Include/Commands/CmdAxedaSetTag.h>

//lint -sem(RCAgentNamespace::CmdAxedaRemoteSessionRequest::CopyFrom,initializer)

namespace RCAgentNamespace {

/****************************************************************************/
/*!
 *  \brief  This class implements a RemoteSessionRequest command for Axeda Agent.
 *
 ****************************************************************************/
class CmdAxedaRemoteSessionRequest : public CmdAxedaSetTag
{
    /// streaming operators are friends
    friend QDataStream & operator << (QDataStream &, const CmdAxedaRemoteSessionRequest &);
    friend QDataStream & operator >> (QDataStream &, CmdAxedaRemoteSessionRequest &);

public:

    static QString NAME; ///< name of this command

    CmdAxedaRemoteSessionRequest();
    CmdAxedaRemoteSessionRequest(int timeout);
    CmdAxedaRemoteSessionRequest(int timeout, AxedaDataItemType_t type, const QString &value);
    CmdAxedaRemoteSessionRequest(const CmdAxedaRemoteSessionRequest &rOther);
    const CmdAxedaRemoteSessionRequest & operator = (const CmdAxedaRemoteSessionRequest &rOther);
    virtual ~CmdAxedaRemoteSessionRequest();

    // inherited function
    virtual QString GetName() const;
    // own functions
    AxedaDigitalDataItemRange_t GetRequestValue() const;
    void SetCommandData(AxedaDataItemType_t type, const QString &value);

private:

    void CopyFrom(const CmdAxedaRemoteSessionRequest &rOther);

private:

    AxedaDigitalDataItemRange_t    m_ReqValue; ///< Request value (0 for disable or 1 for enable)

}; // end class


/****************************************************************************/
/*!
 * \brief   Streaming operator to work with the QDataStream class.
 *     See QDataStream documentation: "Reading and writing other Qt classes"
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \param[in]       Cmd         Command to stream from.
 *
 * \return  Stream with the serialized command
 *
 ****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdAxedaRemoteSessionRequest &Cmd)
{
    // call base's class operator
    Stream << (const CmdAxedaSetTag &)(Cmd);
    // stream the CmdAxedaRemoteSessionRequest private members
    Stream << (quint8&)(Cmd.m_ReqValue);
    return Stream;
}

/****************************************************************************/
/*!
 * \brief   Streaming operator to work with the QDataStream class.
 *     See QDataStream documentation: "Reading and writing other Qt classes"
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in,out]   Cmd         Command to stream into.
 *
 * \return  Stream
 *
 ****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdAxedaRemoteSessionRequest &Cmd)
{
    // call base's class operator
    Stream >> (CmdAxedaSetTag &)(Cmd);
    // stream the CmdAxedaRemoteSessionRequest private members
    Stream >> (quint8&)(Cmd.m_ReqValue);
    return Stream;
}

} // end namespace

#endif

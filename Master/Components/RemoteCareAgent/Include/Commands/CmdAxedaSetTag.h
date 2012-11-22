/****************************************************************************/
/*! \file CmdAxedaSetTag.h
 *
 *  \brief CmdAxedaSetTag command definition.
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

#ifndef RCAGENTNAMESPACE_CMDAXEDASETTAG_H
#define RCAGENTNAMESPACE_CMDAXEDASETTAG_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Command.h>
#include <RemoteCareAgent/Include/AgentController.h>

//lint -sem(RCAgentNamespace::CmdAxedaSetTag::CopyFrom,initializer)

namespace RCAgentNamespace {

/****************************************************************************/
/*!
 *  \brief  This class implements an SetTag command for Axeda Agent.
 *
 ****************************************************************************/
class CmdAxedaSetTag : public Global::Command
{
    /// streaming operators are friends
    friend QDataStream & operator << (QDataStream &, const CmdAxedaSetTag &);
    friend QDataStream & operator >> (QDataStream &, CmdAxedaSetTag &);

public:

    static QString NAME; ///< name of this command

    CmdAxedaSetTag();
    CmdAxedaSetTag(int timeout);
    CmdAxedaSetTag(int timeout, const QString &name, AxedaDataItemType_t type, const QString &value);
    CmdAxedaSetTag(const CmdAxedaSetTag &rOther);
    const CmdAxedaSetTag & operator = (const CmdAxedaSetTag &rOther);
    virtual ~CmdAxedaSetTag();

    // inherited function
    virtual QString GetName() const;
    // own functions
    QString GetTagName() const;
    AxedaDataItemType_t GetTagType() const;
    QString GetTagValue() const;
    virtual void SetCommandData(AxedaDataItemType_t type, const QString &value);

private:

    void CopyFrom(const CmdAxedaSetTag &rOther);

protected:

    QString                 m_Name;       ///< DataItem name
    AxedaDataItemType_t     m_Type;       ///< DataItem type (digital/analog/string)
    QString                 m_Value;      ///< The value of the DataItem

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
inline QDataStream & operator << (QDataStream &Stream, const CmdAxedaSetTag &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // stream the CmdAxedaSetTag private members
    Stream << Cmd.m_Name;
    Stream << (quint8&)(Cmd.m_Type);
    Stream << Cmd.m_Value;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdAxedaSetTag &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // stream the CmdAxedaSetTag private members
    Stream >> Cmd.m_Name;
    Stream >> (quint8&)(Cmd.m_Type);
    Stream >> Cmd.m_Value;
    return Stream;
}

} // end namespace

#endif

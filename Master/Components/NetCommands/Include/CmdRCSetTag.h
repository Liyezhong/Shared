/****************************************************************************/
/*! \file CmdRCSetTag.h
 *
 *  \brief CmdRCSetTag command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.07.2013
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

#ifndef RCAGENTNAMESPACE_CMDRCSETTAG_H
#define RCAGENTNAMESPACE_CMDRCSETTAG_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Command.h>
#include <Global/Include/RemoteCareTypes.h>

//lint -sem(NetCommands::CmdRCSetTag::CopyFrom,initializer)

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements an SetTag command for Axeda Agent.
 *
 ****************************************************************************/
class CmdRCSetTag : public Global::Command
{
    /// streaming operators are friends
    friend QDataStream & operator << (QDataStream &, const CmdRCSetTag &);
    friend QDataStream & operator >> (QDataStream &, CmdRCSetTag &);

public:

    static QString NAME; ///< name of this command

    CmdRCSetTag();
    CmdRCSetTag(int timeout);
    CmdRCSetTag(int timeout, const QString &name, RemoteCare::RCDataItemType_t type, const QString &value);
    CmdRCSetTag(const CmdRCSetTag &rOther);
    const CmdRCSetTag & operator = (const CmdRCSetTag &rOther);
    virtual ~CmdRCSetTag();

    // inherited function
    virtual QString GetName() const;
    // own functions
    QString GetTagName() const;
    RemoteCare::RCDataItemType_t GetTagType() const;
    QString GetTagValue() const;
    virtual void SetCommandData(RemoteCare::RCDataItemType_t type, const QString &value);

private:

    void CopyFrom(const CmdRCSetTag &rOther);

protected:

    QString             m_Name;       ///< DataItem name
    RemoteCare::RCDataItemType_t    m_Type;       ///< DataItem type (digital/analog/string)
    QString             m_Value;      ///< The value of the DataItem

}; // end class


/****************************************************************************/
/*!
 * \brief   Streaming operator to work with the QDataStream class.
 *     See QDataStream documentation: "Reading and writing other Qt classes"
 *
 * \iparam   Stream      Stream to stream into.
 * \iparam       Cmd         Command to stream from.
 *
 * \return  Stream with the serialized command
 *
 ****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRCSetTag &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // stream the CmdRCSetTag private members
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
 * \iparam   Stream      Stream to stream from.
 * \iparam   Cmd         Command to stream into.
 *
 * \return  Stream
 *
 ****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdRCSetTag &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // stream the CmdRCSetTag private members
    Stream >> Cmd.m_Name;
    Stream >> (quint8&)(Cmd.m_Type);
    Stream >> Cmd.m_Value;
    return Stream;
}

} // end namespace

#endif

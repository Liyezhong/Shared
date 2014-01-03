/****************************************************************************/
/*! \file CmdRCNotifyDataItem.h
 *
 *  \brief CmdRCNotifyDataItem command definition.
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

#ifndef RCAGENTNAMESPACE_CMDRCNOTIFYDATAITEM_H
#define RCAGENTNAMESPACE_CMDRCNOTIFYDATAITEM_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Command.h>
#include <Global/Include/RemoteCareTypes.h>

//lint -sem(NetCommands::CmdRCNotifyDataItem::CopyFrom,initializer)

namespace NetCommands {

/****************************************************************************/
/*!
 *  \brief  This class implements an DataItem command for Axeda Agent.
 *
 ****************************************************************************/
class CmdRCNotifyDataItem : public Global::Command
{
    /// streaming operators are friends
    friend QDataStream & operator << (QDataStream &, const CmdRCNotifyDataItem &);
    friend QDataStream & operator >> (QDataStream &, CmdRCNotifyDataItem &);

public:

    static QString NAME; ///< name of this command

    CmdRCNotifyDataItem();
    CmdRCNotifyDataItem(int timeout);
    CmdRCNotifyDataItem(int timeout, const QString &name, RemoteCare::RCDataItemType_t type, \
                     RemoteCare::RCDataItemQuality_t quality, const QString &value, const QString &timestamp);
    CmdRCNotifyDataItem(const CmdRCNotifyDataItem &rOther);
    const CmdRCNotifyDataItem & operator = (const CmdRCNotifyDataItem &rOther);
    ~CmdRCNotifyDataItem();

    // inherited function
    virtual QString GetName() const;
    // own functions
    QString GetDataItemName() const;
    RemoteCare::RCDataItemType_t GetDataItemType() const;
    RemoteCare::RCDataItemQuality_t GetDataItemQuality() const;
    QString GetDataItemValue() const;
    QString GetDataItemTimestamp() const;

private:

    void CopyFrom(const CmdRCNotifyDataItem &rOther);

private:

    QString                 m_Name;       ///< DataItem name
    RemoteCare::RCDataItemType_t        m_Type;       ///< DataItem type (digital/analog/string)
    RemoteCare::RCDataItemQuality_t  m_Quality;    ///< DataItem quality (good/bad/uncertain)
    QString                 m_Value;      ///< The value of the DataItem
    QString                 m_Timestamp;  ///< timestamp of the DataItem logging

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
inline QDataStream & operator << (QDataStream &Stream, const CmdRCNotifyDataItem &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // stream the CmdRCNotifyDataItem private members
    Stream << Cmd.m_Name;
    Stream << (quint8&)(Cmd.m_Type);
    Stream << (quint8&)(Cmd.m_Quality);
    Stream << Cmd.m_Value;
    Stream << Cmd.m_Timestamp;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRCNotifyDataItem &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // stream the CmdRCNotifyDataItem private members
    Stream >> Cmd.m_Name;
    Stream >> (quint8&)(Cmd.m_Type);
    Stream >> (quint8&)(Cmd.m_Quality);
    Stream >> Cmd.m_Value;
    Stream >> Cmd.m_Timestamp;
    return Stream;
}

} // end namespace

#endif

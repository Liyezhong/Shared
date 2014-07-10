/****************************************************************************/
/*! \file CmdRCNotifyDataItem.h
 *
 *  \brief CmdRCNotifyDataItem command definition.
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
    friend QDataStream & operator << (const QDataStream &, const CmdRCNotifyDataItem &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRCNotifyDataItem &);

public:

    static QString NAME; //!< name of this command

    CmdRCNotifyDataItem();
//    CmdRCNotifyDataItem(const int& timeout);
    CmdRCNotifyDataItem(const int& timeout, const QString& name, const RemoteCare::RCDataItemType_t& type, \
                     const RemoteCare::RCDataItemQuality_t& quality, const QString& value, const QString& timestamp);
    CmdRCNotifyDataItem(const CmdRCNotifyDataItem& rOther); //!< Not implemented
    const CmdRCNotifyDataItem & operator = (const CmdRCNotifyDataItem& rOther);
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

    QString                 m_Name;       //!< DataItem name
    RemoteCare::RCDataItemType_t        m_Type;       //!< DataItem type (digital/analog/string)
    RemoteCare::RCDataItemQuality_t  m_Quality;    //!< DataItem quality (good/bad/uncertain)
    QString                 m_Value;      //!< The value of the DataItem
    QString                 m_Timestamp;  //!< timestamp of the DataItem logging

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
inline QDataStream & operator << (const QDataStream &Stream, const CmdRCNotifyDataItem &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
    // stream the CmdRCNotifyDataItem private members
    StreamRef << Cmd.m_Name;
    StreamRef << (quint8&)(Cmd.m_Type);
    StreamRef << (quint8&)(Cmd.m_Quality);
    StreamRef << Cmd.m_Value;
    StreamRef << Cmd.m_Timestamp;
    return StreamRef;
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
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRCNotifyDataItem &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRCNotifyDataItem& CmdRef = const_cast<CmdRCNotifyDataItem &>(Cmd);

    // copy base class data
    CmdRef.CopyFromStream(StreamRef);
    // stream the CmdRCNotifyDataItem private members
    StreamRef >> CmdRef.m_Name;
    StreamRef >> (quint8&)(CmdRef.m_Type);
    StreamRef >> (quint8&)(CmdRef.m_Quality);
    StreamRef >> CmdRef.m_Value;
    StreamRef >> CmdRef.m_Timestamp;
    return StreamRef;
}

} // end namespace

#endif

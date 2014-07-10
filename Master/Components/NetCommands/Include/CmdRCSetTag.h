/****************************************************************************/
/*! \file CmdRCSetTag.h
 *
 *  \brief CmdRCSetTag command definition.
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
    friend QDataStream & operator << (const QDataStream &, const CmdRCSetTag &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRCSetTag &);

public:

    static QString NAME; //!< name of this command

    CmdRCSetTag();
    CmdRCSetTag(const int& timeout);
   CmdRCSetTag(const int& timeout, const QString &name, const RemoteCare::RCDataItemType_t& type, const QString &value);
    CmdRCSetTag(const CmdRCSetTag &rOther);
    const CmdRCSetTag & operator = (const CmdRCSetTag &rOther);
    virtual ~CmdRCSetTag();

    // inherited function
    virtual QString GetName() const;
    // own functions
    QString GetTagName() const;
    RemoteCare::RCDataItemType_t GetTagType() const;
    QString GetTagValue() const;
    virtual void SetCommandData(const RemoteCare::RCDataItemType_t& type, const QString &value);

private:

    void CopyFrom(const CmdRCSetTag &rOther);

protected:

    QString             m_Name;       //!< DataItem name
    RemoteCare::RCDataItemType_t    m_Type;       //!< DataItem type (digital/analog/string)
    QString             m_Value;      //!< The value of the DataItem

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
inline QDataStream & operator << (const QDataStream &Stream, const CmdRCSetTag &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
    // stream the CmdRCSetTag private members
    StreamRef << Cmd.m_Name;
    StreamRef << (quint8&)(Cmd.m_Type);
    StreamRef << Cmd.m_Value;
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
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRCSetTag &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRCSetTag& CmdRef = const_cast<CmdRCSetTag &>(Cmd);

    // copy base class data
    CmdRef.CopyFromStream(StreamRef);
    // stream the CmdRCSetTag private members
    StreamRef >> CmdRef.m_Name;
    StreamRef >> (quint8&)(CmdRef.m_Type);
    StreamRef >> CmdRef.m_Value;
    return StreamRef;
}

} // end namespace

#endif

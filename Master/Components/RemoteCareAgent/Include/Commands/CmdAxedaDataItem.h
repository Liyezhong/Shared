/****************************************************************************/
/*! \file CmdAxedaDataItem.h
 *
 *  \brief CmdAxedaDataItem command definition.
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

#ifndef RCAGENTNAMESPACE_CMDAXEDADATAITEM_H
#define RCAGENTNAMESPACE_CMDAXEDADATAITEM_H

#include <Global/Include/GlobalDefines.h>
#include <Global/Include/Commands/Command.h>
#include <RemoteCareAgent/Include/AgentController.h>

//lint -sem(RCAgentNamespace::CmdAxedaDataItem::CopyFrom,initializer)

namespace RCAgentNamespace {

/****************************************************************************/
/*!
 *  \brief  This class implements an DataItem command for Axeda Agent.
 *
 ****************************************************************************/
class CmdAxedaDataItem : public Global::Command
{
    /// streaming operators are friends
    friend QDataStream & operator << (QDataStream &, const CmdAxedaDataItem &);
    friend QDataStream & operator >> (QDataStream &, CmdAxedaDataItem &);

public:

    static QString NAME; ///< name of this command

    CmdAxedaDataItem();
    CmdAxedaDataItem(int timeout);
    CmdAxedaDataItem(int timeout, const QString &name, AxedaDataItemType_t type, \
                     AxedaDataItemQuality_t quality, const QString &value, const QString &timestamp);
    CmdAxedaDataItem(const CmdAxedaDataItem &rOther);
    const CmdAxedaDataItem & operator = (const CmdAxedaDataItem &rOther);
    ~CmdAxedaDataItem();

    // inherited function
    virtual QString GetName() const;
    // own functions
    QString GetDataItemName() const;
    AxedaDataItemType_t GetDataItemType() const;
    AxedaDataItemQuality_t GetDataItemQuality() const;
    QString GetDataItemValue() const;
    QString GetDataItemTimestamp() const;

private:

    void CopyFrom(const CmdAxedaDataItem &rOther);

private:

    QString                 m_Name;       ///< DataItem name
    AxedaDataItemType_t     m_Type;       ///< DataItem type (digital/analog/string)
    AxedaDataItemQuality_t  m_Quality;    ///< DataItem quality (good/bad/uncertain)
    QString                 m_Value;      ///< The value of the DataItem
    QString                 m_Timestamp;  ///< timestamp of the DataItem logging

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
inline QDataStream & operator << (QDataStream &Stream, const CmdAxedaDataItem &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // stream the CmdAxedaDataItem private members
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
 * \param[in,out]   Stream      Stream to stream from.
 * \param[in,out]   Cmd         Command to stream into.
 *
 * \return  Stream
 *
 ****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdAxedaDataItem &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // stream the CmdAxedaDataItem private members
    Stream >> Cmd.m_Name;
    Stream >> (quint8&)(Cmd.m_Type);
    Stream >> (quint8&)(Cmd.m_Quality);
    Stream >> Cmd.m_Value;
    Stream >> Cmd.m_Timestamp;
    return Stream;
}

} // end namespace

#endif

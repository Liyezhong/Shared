/****************************************************************************/
/*! \file CmdRCSetLogEvent.h
 *
 *  \brief CmdRCSetLogEvent command definition.
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

#ifndef NETCOMMANDS_CMDRCSETLOGEVENT_H
#define NETCOMMANDS_CMDRCSETLOGEVENT_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>
#include <QByteArray>

namespace NetCommands {

//!< Data Structure containing EventReportData
struct RCLogEventReportStruct{
    quint32 EventCode; //!< Event ID
    QList<QString> EventStringList; //!< EventString List
    Global::AlternateEventStringUsage AltStringUsage;  //!< Event String
};


/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCSetLogEvent command.
 */
/****************************************************************************/
class CmdRCSetLogEvent : public Global::Command {
    friend QDataStream & operator << (const QDataStream &, const CmdRCSetLogEvent &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRCSetLogEvent &);
public:
    static QString NAME;    //!< Command name.
    /****************************************************************************/
    CmdRCSetLogEvent(const int& TimeOut, const RCLogEventReportStruct &EventReportData);
    CmdRCSetLogEvent();                                                    //!< Not implemented.
    ~CmdRCSetLogEvent();

    virtual QString GetName() const;

    /****************************************************************************/
    /**
     * \brief Get the EventReport Data
     * \return Byte array.
     */
    /****************************************************************************/
    QByteArray const & GetEventData() const { return m_RCEventReportByteArray;}

private:
    CmdRCSetLogEvent(const CmdRCSetLogEvent &);                       //!< Not implemented.
    const CmdRCSetLogEvent & operator = (const CmdRCSetLogEvent &);   //!< Not implemented.
    QByteArray m_RCEventReportByteArray; //!< Event Report Data ByteArray

}; // end class CmdRCSetLogEvent

/****************************************************************************/
/**
 * \brief Output Stream Operator which streams data
 *
 * \iparam   Stream      Stream to stream into.
 * \iparam   Cmd         The command to stream.
 * \return   Output Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (const QDataStream &Stream, const CmdRCSetLogEvent &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
    // copy internal data
    StreamRef << Cmd.m_RCEventReportByteArray;
    return StreamRef;
}

/****************************************************************************/
/**
 * \brief Input Stream Operator which streams data
 *
 * \iparam   Stream      Stream to stream from.
 * \iparam   Cmd         The command to stream.
 * \return   Input Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRCSetLogEvent &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRCSetLogEvent& CmdRef = const_cast<CmdRCSetLogEvent &>(Cmd);

    // copy base class data
    CmdRef.CopyFromStream(StreamRef);
    // copy internal data
    StreamRef >> CmdRef.m_RCEventReportByteArray;
    return StreamRef;
}


} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCSETLOGEVENT_H

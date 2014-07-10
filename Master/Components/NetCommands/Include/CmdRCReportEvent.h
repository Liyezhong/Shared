/****************************************************************************/
/*! \file CmdRCReportEvent.h
 *
 *  \brief CmdRCReportEvent command definition.
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

#ifndef NETCOMMANDS_CMDRCREPORTEVENT_H
#define NETCOMMANDS_CMDRCREPORTEVENT_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>
#include <QByteArray>

namespace NetCommands {

//!< Data Structure containing EventReportData
struct RCEventReportDataStruct{
    QString m_EventName; //!< Event ID
    QString m_EventMessage;  //!< Event String
    QString m_EventSeverity;  //!< – number as QString, Alarm: 300, Warning: 200, Info: 100
    QString m_TimeStamp; //!< Time at which event occured
};

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCReportEvent command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdRCReportEvent : public Global::Command {
    friend QDataStream & operator << (const QDataStream &, const CmdRCReportEvent &);
    friend QDataStream & operator >> (const QDataStream &, const CmdRCReportEvent &);
public:
    static QString NAME;    //!< Command name.
    /****************************************************************************/
    CmdRCReportEvent(const int& TimeOut, const RCEventReportDataStruct &EventReportData);
    CmdRCReportEvent();                                                    //!< Not implemented.
    ~CmdRCReportEvent();

    virtual QString GetName() const;

    /****************************************************************************/
    /**
     * \brief Get the EventReport Data
     *
     * \return Byte array.
     */
    /****************************************************************************/
    QByteArray const & GetRCEventReportData() const { return m_RCEventReportByteArray;}

    /****************************************************************************/
    /**
     * \brief Get the EventReport Data
     *
     * \return Byte array.
     */
    /****************************************************************************/
    RCEventReportDataStruct GetRCEventReportDataStruct() const {
        RCEventReportDataStruct EventStruct;
        QDataStream DataStream(const_cast<QByteArray *>(&m_RCEventReportByteArray), QIODevice::ReadWrite);
        DataStream >> EventStruct.m_EventName;
        DataStream >> EventStruct.m_EventMessage;
        DataStream >> EventStruct.m_EventSeverity;
        DataStream >> EventStruct.m_TimeStamp;

        return EventStruct;
    }

private:
    CmdRCReportEvent(const CmdRCReportEvent &);                       //!< Not implemented.
    const CmdRCReportEvent & operator = (const CmdRCReportEvent &);   //!< Not implemented.
    QByteArray m_RCEventReportByteArray;  //!< ByteArray for storing Event Report data.
}; // end class CmdRCReportEvent

/****************************************************************************/
/**
 * \brief Output Stream Operator which streams data
 *
 * \iparam   Stream      Stream to stream into.
 * \iparam   Cmd         The command to stream.
 * \return   Output Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (const QDataStream &Stream, const CmdRCReportEvent &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    // copy base class data
    Cmd.CopyToStream(StreamRef);
    // copy internal data
    StreamRef <<Cmd.m_RCEventReportByteArray;
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
inline QDataStream & operator >> (const QDataStream &Stream, const CmdRCReportEvent &Cmd)
{
    QDataStream& StreamRef = const_cast<QDataStream &>(Stream);
    CmdRCReportEvent& CmdRef = const_cast<CmdRCReportEvent &>(Cmd);

    // copy base class data
    CmdRef.CopyFromStream(StreamRef);
    // copy internal data
    StreamRef >> CmdRef.m_RCEventReportByteArray;
    return StreamRef;
}


} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCREPORTEVENT_H

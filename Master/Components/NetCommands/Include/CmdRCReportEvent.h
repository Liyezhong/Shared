/****************************************************************************/
/*! \file CmdRCReportEvent.h
 *
 *  \brief CmdRCReportEvent command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 30.04.2013
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
    friend QDataStream & operator << (QDataStream &, const CmdRCReportEvent &);
    friend QDataStream & operator >> (QDataStream &, CmdRCReportEvent &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdRCReportEvent(int TimeOut, RCEventReportDataStruct &EventReportData);
    CmdRCReportEvent();                                                    ///< Not implemented.
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
    CmdRCReportEvent(const CmdRCReportEvent &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRCReportEvent & operator = (const CmdRCReportEvent &);   ///< Not implemented.
    QByteArray m_RCEventReportByteArray;  ///< ByteArray for storing Event Report data.
}; // end class CmdRCReportEvent

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \iparam   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRCReportEvent &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream <<Cmd.m_RCEventReportByteArray;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \iparam   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdRCReportEvent &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_RCEventReportByteArray;
    return Stream;
}


} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCREPORTEVENT_H

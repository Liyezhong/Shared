/****************************************************************************/
/*! \file CmdRCSetLogEvent.h
 *
 *  \brief CmdRCSetLogEvent command definition.
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

#ifndef NETCOMMANDS_CMDRCSETLOGEVENT_H
#define NETCOMMANDS_CMDRCSETLOGEVENT_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>
#include <QByteArray>

namespace NetCommands {

//!< Data Structure containing EventReportData
struct RCLogEventReportStruct{
    quint32 EventCode; //!< Event ID
    QList<QString> EventStringList; ///< EventString List
    Global::AlternateEventStringUsage AltStringUsage;  //!< Event String
};


/****************************************************************************/
/*!
 *  \brief  This class implements a CmdRCSetLogEvent command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdRCSetLogEvent : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdRCSetLogEvent &);
    friend QDataStream & operator >> (QDataStream &, CmdRCSetLogEvent &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdRCSetLogEvent(int TimeOut, RCLogEventReportStruct &EventReportData);
    CmdRCSetLogEvent();                                                    ///< Not implemented.
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
    CmdRCSetLogEvent(const CmdRCSetLogEvent &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdRCSetLogEvent & operator = (const CmdRCSetLogEvent &);   ///< Not implemented.
    QByteArray m_RCEventReportByteArray; ///< Event Report Data ByteArray

}; // end class CmdRCSetLogEvent

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \iparam   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdRCSetLogEvent &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream << Cmd.m_RCEventReportByteArray;
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
inline QDataStream & operator >> (QDataStream &Stream, CmdRCSetLogEvent &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_RCEventReportByteArray;
    return Stream;
}


} // end namespace NetCommands

#endif // NETCOMMANDS_CMDRCSETLOGEVENT_H

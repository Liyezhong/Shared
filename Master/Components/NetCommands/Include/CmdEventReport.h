/****************************************************************************/
/*! \file CmdEventReport.h
 *
 *  \brief CmdEventReport command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 05.10.2012
 *   $Author:  $ N.Kamath
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

#ifndef NETCOMMANDS_CMDEVENTREPORT_H
#define NETCOMMANDS_CMDEVENTREPORT_H

#include <Global/Include/Commands/Command.h>
#include <Global/Include/GlobalDefines.h>
#include <QByteArray>

namespace NetCommands {

//!< Data Structure containing EventReportData
struct EventReportDataStruct{
    quint64 ID; //!< Event ID
    quint32 EventKey;   //!< Event Key
    QString MsgString;  //!< Event String
    bool EventStatus;  //!< True - event active , else inactive
    Global::EventType EventType; //!< EventType
    Global::GuiButtonType BtnType; //!< GUI button type
    QString Time; //!< Time at which event occured
    bool StatusBarIcon; //!< True to store event and display when status bar icon is clicked
    QString BtnEnableConditions; //!< button enable conditions: RT_LID_OPEN_CLOSE
    QString Timeout; //!< timeout for the message if no user responsing
};

/****************************************************************************/
/*!
 *  \brief  This class implements a CmdEventReport command.
 *
 * \todo implement
 */
/****************************************************************************/
class CmdEventReport : public Global::Command {
    friend QDataStream & operator << (QDataStream &, const CmdEventReport &);
    friend QDataStream & operator >> (QDataStream &, CmdEventReport &);
public:
    static QString NAME;    ///< Command name.
    /****************************************************************************/
    CmdEventReport(int TimeOut, EventReportDataStruct &EventReportData);
    CmdEventReport();                                                    ///< Not implemented.
    ~CmdEventReport();

    virtual QString GetName() const;

    /****************************************************************************/
    /**
     * \brief Get the EventReport Data
     *
     * \return Byte array.
     */
    /****************************************************************************/
    QByteArray const & GetEventReportData() const { return m_EventReportByteArray;}

private:
    CmdEventReport(const CmdEventReport &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CmdEventReport & operator = (const CmdEventReport &);   ///< Not implemented.
    QByteArray m_EventReportByteArray;  ///< ByteArray for storing Event Report data.
}; // end class CmdEventReport

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream into.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator << (QDataStream &Stream, const CmdEventReport &Cmd)
{
    // copy base class data
    Cmd.CopyToStream(Stream);
    // copy internal data
    Stream <<Cmd.m_EventReportByteArray;
    return Stream;
}

/****************************************************************************/
/**
 * \brief Streaming operator.
 *
 * \param[in,out]   Stream      Stream to stream from.
 * \iparam       Cmd         The command to stream.
 * \return                      Stream.
 */
/****************************************************************************/
inline QDataStream & operator >> (QDataStream &Stream, CmdEventReport &Cmd)
{
    // copy base class data
    Cmd.CopyFromStream(Stream);
    // copy internal data
    Stream >> Cmd.m_EventReportByteArray;
    return Stream;
}


} // end namespace NetCommands

#endif // NETCOMMANDS_CMDEVENTREPORT_H

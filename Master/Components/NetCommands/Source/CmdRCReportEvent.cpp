/****************************************************************************/
/*! \file CmdRCReportEvent.cpp
 *
 *  \brief CmdRCReportEvent command implementation.
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

#include <NetCommands/Include/CmdRCReportEvent.h>

namespace NetCommands {

QString CmdRCReportEvent::NAME = "NetCommands::CmdRCReportEvent";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam   Timeout                 Timeout for command.
 *  \iparam   EventReportData         Event Report DataStruct
 */
/****************************************************************************/
CmdRCReportEvent::CmdRCReportEvent(const int& Timeout, const RCEventReportDataStruct &EventReportData) :
    Command(Timeout)
{
    QDataStream Stream(&m_RCEventReportByteArray, QIODevice::ReadWrite);
    // Stream order
    // EventStatus, EventType, EventID, EventString,  Time, btn Type,StatusBar Icon
    Stream << EventReportData.m_EventName << EventReportData.m_EventMessage << EventReportData.m_EventSeverity
            << EventReportData.m_TimeStamp;

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdRCReportEvent::CmdRCReportEvent() : Command(0)
{
}
/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRCReportEvent::~CmdRCReportEvent()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRCReportEvent::GetName() const
{
    return NAME;
}

} // end namespace NetCommands

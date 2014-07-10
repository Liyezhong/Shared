/****************************************************************************/
/*! \file CmdRCSetLogEvent.cpp
 *
 *  \brief CmdRCSetLogEvent command implementation.
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

#include <NetCommands/Include/CmdRCSetLogEvent.h>

namespace NetCommands {

QString CmdRCSetLogEvent::NAME = "NetCommands::CmdRCSetLogEvent";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam   Timeout          Timeout for command.
 *  \iparam   EventReportData  Event Report DataStruct
 */
/****************************************************************************/
CmdRCSetLogEvent::CmdRCSetLogEvent(const int& Timeout, const RCLogEventReportStruct &EventReportData) :
    Command(Timeout)
{
    QDataStream Stream(&m_RCEventReportByteArray, QIODevice::ReadWrite);
    // Stream order
    // Eventcode, event string list, alternate string usage
    Stream << EventReportData.EventCode << EventReportData.EventStringList
           << EventReportData.AltStringUsage;

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdRCSetLogEvent::CmdRCSetLogEvent() : Command(0)
{
}
/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRCSetLogEvent::~CmdRCSetLogEvent()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRCSetLogEvent::GetName() const
{
    return NAME;
}

} // end namespace NetCommands

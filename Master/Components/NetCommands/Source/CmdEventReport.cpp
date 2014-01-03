/****************************************************************************/
/*! \file CmdEventReport.cpp
 *
 *  \brief CmdEventReport command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 05.10.2012
 *   $Author:  $ Bhanuprasad.H
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

#include <NetCommands/Include/CmdEventReport.h>

namespace NetCommands {

QString CmdEventReport::NAME = "NetCommands::CmdEventReport";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout                 Timeout for command.
 * \iparam   EventReportData         Event Report DataStruct
 */
/****************************************************************************/
CmdEventReport::CmdEventReport(int Timeout, EventReportDataStruct &EventReportData) :
    Command(Timeout)
{
    QDataStream Stream(&m_EventReportByteArray, QIODevice::ReadWrite);
    // Stream order
    // EventStatus, EventType, EventID, EventString,  Time, btn Type,StatusBar Icon
    Stream << EventReportData.EventStatus << EventReportData.EventType << EventReportData.ID
           << EventReportData.MsgString << EventReportData.Time << EventReportData.BtnType
           << EventReportData.StatusBarIcon;

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdEventReport::CmdEventReport() : Command(0)
{
}
/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdEventReport::~CmdEventReport()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdEventReport::GetName() const
{
    return NAME;
}

} // end namespace NetCommands

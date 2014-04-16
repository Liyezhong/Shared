/****************************************************************************/
/*! \file CmdGuiEventReport.cpp
 *
 *  \brief CmdGuiEventReport command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-02-23
 *   $Author:  $ Raju
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

#include <NetCommands/Include/CmdGuiEventReport.h>

namespace NetCommands {

QString CmdGuiEventReport::NAME = "NetCommands::CmdGuiEventReport";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout          Timeout for command.
 * \iparam   EventReportData  Event Report DataStruct
 */
/****************************************************************************/
CmdGuiEventReport::CmdGuiEventReport(int Timeout, GuiEventReportStruct &EventReportData) :
    Command(Timeout)
{
    QDataStream Stream(&m_GuiEventReportByteArray, QIODevice::ReadWrite);
    // Stream order
    // EventStatus, EventType, EventID, EventString,  Time, btn Type,StatusBar Icon
    Stream << EventReportData.EventCode << EventReportData.EventStringList
           << EventReportData.AltStringUsage;

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdGuiEventReport::CmdGuiEventReport() : Command(0)
{
}
/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdGuiEventReport::~CmdGuiEventReport()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdGuiEventReport::GetName() const
{
    return NAME;
}

} // end namespace NetCommands

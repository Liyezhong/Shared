/****************************************************************************/
/*! \file CmdProblemReportReply.cpp
 *
 *  \brief CmdProblemReportReply command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.03.2010
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

#include <NetCommands/Include/CmdProblemReportReply.h>

namespace NetCommands {

QString CmdProblemReportReply::NAME = "NetCommands::CmdProblemReportReply";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout     Timeout for command.
 * \iparam   Report      The problem report reply.
 */
/****************************************************************************/
CmdProblemReportReply::CmdProblemReportReply(int Timeout, const QStringList &Report) :
    Command(Timeout),
    m_Report(Report)
{
    setErrorStateAllowed(true);
    setBusyStateAllowed(true);
    setIdleStateAllowed(true);
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdProblemReportReply::~CmdProblemReportReply()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdProblemReportReply::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the problem report reply:
 *               problem type, event, result, timestamp
 *
 *  \return  report
 */
/****************************************************************************/
QStringList CmdProblemReportReply::GetReport() const
{
    return m_Report;
}

} // end namespace NetCommands

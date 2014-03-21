/****************************************************************************/
/*! \file NetCommands/Source/CmdAcknEventReport.cpp
 *
 *  \brief CmdAcknEventReport command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 09.11.2012
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

#include <NetCommands/Include/CmdAcknEventReport.h>

namespace NetCommands {

QString CmdAcknEventReport::NAME = "NetCommands::CmdAcknEventReport";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   CmdType     Type of the command
 * \param[in]   Password    Password of the admin
 */
/****************************************************************************/
CmdAcknEventReport::CmdAcknEventReport(const quint64 EventKey, const ClickedButton_t ClickedButton) :
    Global::Acknowledge(),
    m_ClickedButton(ClickedButton),
    m_EventKey(EventKey)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdAcknEventReport::CmdAcknEventReport() :
    Global::Acknowledge()
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdAcknEventReport::~CmdAcknEventReport()
{
}

} // end namespace NetCommands


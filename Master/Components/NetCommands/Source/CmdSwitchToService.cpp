/****************************************************************************/
/*! \file CmdSwitchToService.cpp
 *
 *  \brief CmdSwitchToService command implementation.
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

#include <NetCommands/Include/CmdSwitchToService.h>

namespace NetCommands {

QString CmdSwitchToService::NAME = "NetCommands::CmdSwitchToService";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   Pin         Pin.
 */
/****************************************************************************/
CmdSwitchToService::CmdSwitchToService(int Timeout, const QString &Pin) :
    Command(Timeout),
    m_Pin(Pin)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdSwitchToService::~CmdSwitchToService()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdSwitchToService::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns pin, entered by user
 *
 *  \return  pin
 */
/****************************************************************************/
QString CmdSwitchToService::GetPin() const
{
    return m_Pin;
}

} // end namespace NetCommands

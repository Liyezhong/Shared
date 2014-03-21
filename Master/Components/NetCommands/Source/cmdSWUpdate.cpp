/****************************************************************************/
/*! \file CmdSWUpdate.cpp
 *
 *  \brief CmdSWUpdate command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.04.2013
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

#include <NetCommands/Include/CmdSWUpdate.h>


namespace NetCommands {

QString CmdSWUpdate::NAME = "NetCommands::CmdSWUpdate";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   Timeout        Timeout for command.
 * \param[in]   USBUpdate      true-Update SW from USB else from Remote care
 *
 */
/****************************************************************************/
CmdSWUpdate::CmdSWUpdate(int Timeout, const bool USBUpdate) :
    Command(Timeout),
    m_USBUpdate(USBUpdate)
{

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdSWUpdate::CmdSWUpdate() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdSWUpdate::~CmdSWUpdate()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdSWUpdate::GetName() const
{
    return NAME;
}

} // end namespace NetCommands




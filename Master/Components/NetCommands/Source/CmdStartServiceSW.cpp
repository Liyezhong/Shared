/****************************************************************************/
/*! \file CmdStartServiceSW.cpp
 *
 *  \brief CmdStartServiceSW command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 09.08.2013
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

#include <NetCommands/Include/CmdStartServiceSW.h>


namespace NetCommands {

QString CmdStartServiceSW::NAME = "NetCommands::CmdStartServiceSW";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \iparam   Timeout        Timeout for command.
 *
 */
/****************************************************************************/
CmdStartServiceSW::CmdStartServiceSW(int Timeout) :
    Command(Timeout)
{

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdStartServiceSW::CmdStartServiceSW() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdStartServiceSW::~CmdStartServiceSW()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdStartServiceSW::GetName() const
{
    return NAME;
}

} // end namespace NetCommands





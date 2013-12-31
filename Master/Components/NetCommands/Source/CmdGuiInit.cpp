/****************************************************************************/
/*! \file CmdGuiInit.cpp
 *
 *  \brief CmdGuiInit command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.12.2012
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

#include <NetCommands/Include/CmdGuiInit.h>


namespace NetCommands {

QString CmdGuiInit::NAME = "NetCommands::CmdGuiInit";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \iparam   Timeout                 Timeout for command.
 * \iparam   InitStatus              Status of GuiInit.True if GUI Init complete
 */
/****************************************************************************/
CmdGuiInit::CmdGuiInit(int Timeout, const bool InitStatus) :
    Command(Timeout),
    m_InitStatus(InitStatus)
{

}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdGuiInit::CmdGuiInit() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdGuiInit::~CmdGuiInit()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdGuiInit::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses



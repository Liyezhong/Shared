/****************************************************************************/
/*! \file NetCommands/Source/CmdRCSoftwareUpdate.cpp
 *
 *  \brief CmdRCSoftwareUpdate command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 30.04.2013
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

#include <NetCommands/Include/CmdRCSoftwareUpdate.h>

namespace RemoteCare {

QString CmdRCSoftwareUpdate::NAME = "NetCommands::CmdRCSoftwareUpdate";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 *  \iparam   Timeout     Timeout for command.
 *  \iparam  SWUpdateType  Update Type
 */
/****************************************************************************/
CmdRCSoftwareUpdate::CmdRCSoftwareUpdate(int Timeout, QString SWUpdateType) :
    Command(Timeout),
    m_SWUpdateType(SWUpdateType)
{
}

/****************************************************************************/
/*!
 *  \brief   Constructor
 */
/****************************************************************************/
CmdRCSoftwareUpdate::CmdRCSoftwareUpdate() :
    Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRCSoftwareUpdate::~CmdRCSoftwareUpdate()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRCSoftwareUpdate::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   Get the update type
 *
 *  \return  sw update type
 */
/****************************************************************************/
QString CmdRCSoftwareUpdate::GetUpdateType() const
{
    return m_SWUpdateType;
}
} // end namespace RemoteCare

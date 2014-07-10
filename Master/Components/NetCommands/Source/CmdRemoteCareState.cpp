/****************************************************************************/
/*! \file CmdRemoteCareState.cpp
 *
 *  \brief CmdRemoteCareState command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 13.03.2013
 *   $Author:  $ Bhanu Prasad H
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

#include <NetCommands/Include/CmdRemoteCareState.h>

namespace NetCommands {

QString CmdRemoteCareState::NAME = "NetCommands::CmdRemoteCareState"; //!< Name of the command

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \iparam   Timeout         Timeout for command.
 * \iparam   RemoteCareState    Flag for the RemoteCare state.
 * \iparam   ConnectionType   Connection type
 */
/****************************************************************************/
CmdRemoteCareState::CmdRemoteCareState(int Timeout, const bool &RemoteCareState, QString ConnectionType)
    : Command(Timeout)
    , m_AgentState(RemoteCareState)
    , m_ConnectionType(ConnectionType)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRemoteCareState::~CmdRemoteCareState()
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor to receive commands
 */
/****************************************************************************/
CmdRemoteCareState::CmdRemoteCareState()
    : Command(0)
    , m_AgentState(false)
    , m_ConnectionType("")
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRemoteCareState::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns RemoteCare state
 *
 *  \return  RemoteCareState
 */
/****************************************************************************/
bool CmdRemoteCareState::GetRemoteCareState() const
{
    return m_AgentState;
}
/****************************************************************************/
/*!
 *  \brief   This function returns Connection type
 *
 *  \return  Connection type
 */
/****************************************************************************/
QString CmdRemoteCareState::GetConnectionType() const
{
    return m_ConnectionType;
}

}//End of namespace NetCommands



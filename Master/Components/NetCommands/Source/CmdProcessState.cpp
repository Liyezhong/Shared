/****************************************************************************/
/*! \file CmdProcessState.cpp
 *
 *  \brief CmdProcessState command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 03.04.2012
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

#include <NetCommands/Include/CmdProcessState.h>

namespace NetCommands {

QString CmdProcessState::NAME = "NetCommands::CmdProcessState"; ///< Name of the command

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout         Timeout for command.
 * \param[in]   ProcessState    Flag for the Process state.
 */
/****************************************************************************/
CmdProcessState::CmdProcessState(int Timeout, const bool &ProcessState) :
    Command(Timeout),
    m_ProcessState(ProcessState)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdProcessState::~CmdProcessState()
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor to receive commands
 */
/****************************************************************************/
CmdProcessState::CmdProcessState() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdProcessState::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns Process state
 *
 *  \return  ProcessState
 */
/****************************************************************************/
bool CmdProcessState::GetProcessState() const
{
    return m_ProcessState;
}

}//End of namespace NetCommands



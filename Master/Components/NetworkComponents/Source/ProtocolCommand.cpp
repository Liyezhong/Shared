/****************************************************************************/
/*! \file ProtocolCommand.cpp
 *
 *  \brief ProtocolCommand base class implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.11.2010
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

#include <NetworkComponents/Include/NetworkDevice.h>
#include <NetworkComponents/Include/ProtocolCommand.h>

namespace NetworkBase {


/****************************************************************************/
/*!
 * \brief Constructor.
 *
 */
/****************************************************************************/
ProtocolCommand::ProtocolCommand() :
        m_myName(""),
        m_myDevice(NULL)
{
}

/****************************************************************************/
/*!
 * \brief Destructor.
 *
 */
/****************************************************************************/
ProtocolCommand::~ProtocolCommand()
{
    m_myDevice = NULL;
}

/****************************************************************************/
/*!
 *  \brief   Get the command name
 *
 *  \return  Command name as string
 *
 ****************************************************************************/
QString ProtocolCommand::GetName()
{
    return m_myName;
}

} // end namespace NetworkBase

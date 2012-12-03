/****************************************************************************/
/*! \file CmdStationsDefect.cpp
 *
 *  \brief CmdStationsDefect command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-10-31
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

#include <NetCommands/Include/CmdStationsDefect.h>

namespace NetCommands {

QString CmdStationsDefect::NAME = "NetCommands::CmdStationsDefect"; ///< Name of the command

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout         Timeout for command.
 * \param[in]   ProcessState    Flag for the Process state.
 */
/****************************************************************************/
CmdStationsDefect::CmdStationsDefect(int Timeout, const QStringList DefectiveStationsList) :
    Command(Timeout),
    m_DefectiveStationList(DefectiveStationsList)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdStationsDefect::~CmdStationsDefect()
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor to receive commands
 */
/****************************************************************************/
CmdStationsDefect::CmdStationsDefect() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdStationsDefect::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns Defective station list
 *
 *  \return  RackRfid
 */
/****************************************************************************/
QStringList CmdStationsDefect::GetDefectiveStations() const
{
    return m_DefectiveStationList;
}

}//End of namespace NetCommands




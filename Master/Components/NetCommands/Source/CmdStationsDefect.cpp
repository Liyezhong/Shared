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
 * \iparam   Timeout                  Timeout for command.
 * \iparam   StationList              List of defective Stations
 * \iparam   IsDefective              true if defective, else false
 */
/****************************************************************************/
CmdStationsDefect::CmdStationsDefect(int Timeout, const QStringList StationList,
                                      bool IsDefective) :
    Command(Timeout),
    m_StationList(StationList),
    m_IsDefective(IsDefective)
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
 *  \brief   Gets the flag indicating station status (Defective or Corrected)
 *
 *  \return  True = Defective station/False = Corrected station
 */
/****************************************************************************/
bool CmdStationsDefect::GetListDefectiveFlag() const
{
    return m_IsDefective;
}

/****************************************************************************/
/*!
 *  \brief   This function returns Defective station list
 *
 *  \return   List of defective Stations
 */
/****************************************************************************/
QStringList CmdStationsDefect::GetStationList() const
{
    return m_StationList;
}

}//End of namespace NetCommands




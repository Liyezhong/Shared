/****************************************************************************/
/*! \file CmdUpdateProgramEndTime.cpp
 *
 *  \brief CmdUpdateProgramEndTime command implementation.
 *
 *   $Version: $ 0.1
*   $Date:    $ 27.03.2013
 *   $Author:  $ Abe Yang
 *
 *  \b Company:
 *
  *       Leica Microsystems Ltd. Shanghai.
 *
 *  (C) Copyright 2013 by Leica Microsystems Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdUpdateProgramEndTime.h"
namespace MsgClasses {

QString CmdUpdateProgramEndTime::NAME = "MsgClasses::CmdUpdateProgramEndTime";


CmdUpdateProgramEndTime::CmdUpdateProgramEndTime(int timeout, int endTimeDiff) :
    Command(timeout),
    m_EndTimeDiff(endTimeDiff)
{
}

CmdUpdateProgramEndTime::CmdUpdateProgramEndTime()
    : Command(0), m_EndTimeDiff(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdUpdateProgramEndTime::~CmdUpdateProgramEndTime()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdUpdateProgramEndTime::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

/****************************************************************************/
/*! \file CmdProgramAborted.cpp
 *
 *  \brief CmdProgramAborted command implementation.
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAborted.h"
namespace MsgClasses {

QString CmdProgramAborted::NAME = "MsgClasses::CmdProgramAborted";


CmdProgramAborted::CmdProgramAborted(int Timeout, bool IsContaminated) :
    Command(Timeout),
    m_IsRetortContaminated(IsContaminated)
{
}

CmdProgramAborted::CmdProgramAborted()
    : Command(0), m_IsRetortContaminated(false)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdProgramAborted::~CmdProgramAborted()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdProgramAborted::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

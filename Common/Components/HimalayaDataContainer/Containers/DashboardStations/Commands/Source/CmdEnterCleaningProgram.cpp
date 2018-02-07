/****************************************************************************/
/*! \file CmdEnterCleaningProgram.cpp
 *
 *  \brief CmdEnterCleaningProgram command implementation.
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdEnterCleaningProgram.h"
namespace MsgClasses {

QString CmdEnterCleaningProgram::NAME = "MsgClasses::CmdEnterCleaningProgram";


CmdEnterCleaningProgram::CmdEnterCleaningProgram(int Timeout, const QString& lastReagentGroupID) :
    Command(Timeout),
    m_LastReagentGroupID(lastReagentGroupID)
{
}

CmdEnterCleaningProgram::CmdEnterCleaningProgram()
    : Command(0), 
	m_LastReagentGroupID("")
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdEnterCleaningProgram::~CmdEnterCleaningProgram()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdEnterCleaningProgram::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

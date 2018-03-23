/****************************************************************************/
/*! \file CmdProgramAction.cpp
 *
 *  \brief CmdProgramAction command implementation.
 *
 *   $Version: $ 0.1
*   $Date:    $ 22.03.2013
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAction.h"
namespace MsgClasses {

QString CmdProgramAction::NAME = "MsgClasses::CmdProgramAction";


CmdProgramAction::CmdProgramAction(const QString& retortName, int timeout, const QString& programID, DataManager::ProgramActionType_t actionType,
                                   int delayTime, int programRunDuration, const QString& reagentExpiredFlag) :
    Command(timeout),
    m_RetortName(retortName),
    m_ProgramID(programID),
    m_ActionType(actionType),
    m_DelayTime(delayTime),
    m_ProgramRunDuration(programRunDuration),
    m_ReagentExpiredFlag(reagentExpiredFlag)
{
}

CmdProgramAction::CmdProgramAction():
    Command(0),
    m_ProgramID(""),
    m_ActionType(DataManager::PROGRAM_UNDEFINED),
    m_DelayTime(0),
    m_ProgramRunDuration(0),
    m_ReagentExpiredFlag("")
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdProgramAction::~CmdProgramAction()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdProgramAction::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

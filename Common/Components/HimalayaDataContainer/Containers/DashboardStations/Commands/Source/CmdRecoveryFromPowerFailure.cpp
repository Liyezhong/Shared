/****************************************************************************/
/*! \file CmdRecoveryFromPowerFailure.cpp
 *
 *  \brief CmdRecoveryFromPowerFailure command implementation.
 *
 *   $Version: $ 0.1
*   $Date:    $ 30.09.2014
 *   $Author:  $ Logan Yang
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdRecoveryFromPowerFailure.h"
namespace MsgClasses {

QString CmdRecoveryFromPowerFailure::NAME = "MsgClasses::CmdRecoveryFromPowerFailure";


CmdRecoveryFromPowerFailure::CmdRecoveryFromPowerFailure(int timeout, const QString& ProgramID,
                                                         int StepIndex, int Scenario, int RemainingTime,
                                                         const QString& ReagentGroupID, const QList<QString>& StationList,
                                                         bool IsErrorHandlingFailed, const QString& StationID) :
    Command(timeout),
    m_ProgramID(ProgramID), m_StepIndex(StepIndex),m_Scenario(Scenario),
    m_RemainingTime(RemainingTime),m_LastReagentGroupID(ReagentGroupID),m_StationList(StationList),
    m_IsErrorHandlingFailed(IsErrorHandlingFailed), m_StationID(StationID)
{
}

CmdRecoveryFromPowerFailure::CmdRecoveryFromPowerFailure()
    : Command(0), m_StepIndex(0), m_Scenario(0), m_RemainingTime(0),
      m_IsErrorHandlingFailed(false)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRecoveryFromPowerFailure::~CmdRecoveryFromPowerFailure()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRecoveryFromPowerFailure::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

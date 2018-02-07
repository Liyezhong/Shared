/****************************************************************************/
/*! \file CmdCurrentProgramStepInfor.cpp
 *
 *  \brief CmdCurrentProgramStepInfor command implementation.
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdCurrentProgramStepInfor.h"
namespace MsgClasses {

QString CmdCurrentProgramStepInfor::NAME = "MsgClasses::CmdCurrentProgramStepInfor";


CmdCurrentProgramStepInfor::CmdCurrentProgramStepInfor(int timeout, const QString& stepName,
                                                       int curProgramStepIndex,
                                                        int curRemainingTime) :
    Command(timeout),
    m_StepName(stepName),
    m_CurProgramStepIndex(curProgramStepIndex),
    m_CurRemainingTime(curRemainingTime)
{
}

CmdCurrentProgramStepInfor::CmdCurrentProgramStepInfor()
    : Command(0)
    , m_CurProgramStepIndex(0), m_CurRemainingTime(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdCurrentProgramStepInfor::~CmdCurrentProgramStepInfor()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdCurrentProgramStepInfor::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

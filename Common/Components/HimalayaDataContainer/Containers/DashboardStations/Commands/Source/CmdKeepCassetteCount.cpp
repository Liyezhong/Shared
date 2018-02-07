/****************************************************************************/
/*! \file CmdKeepCassetteCount.cpp
 *
 *  \brief CmdKeepCassetteCount command implementation.
 *
 *   $Version: $ 0.1
*   $Date:    $ 17.04.2013
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdKeepCassetteCount.h"
namespace MsgClasses {

QString CmdKeepCassetteCount::NAME = "MsgClasses::CmdKeepCassetteCount";


CmdKeepCassetteCount::CmdKeepCassetteCount(int Timeout, int CasseteCount) :
    Command(Timeout),
    m_CassetteCount(CasseteCount)
{
}

CmdKeepCassetteCount::CmdKeepCassetteCount()
    : Command(0), m_CassetteCount(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdKeepCassetteCount::~CmdKeepCassetteCount()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdKeepCassetteCount::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

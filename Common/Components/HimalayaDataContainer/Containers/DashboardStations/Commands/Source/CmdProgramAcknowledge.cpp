/****************************************************************************/
/*! \file CmdProgramAcknowledge.cpp
 *
 *  \brief CmdProgramAcknowledge command implementation.
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAcknowledge.h"
namespace MsgClasses {

QString CmdProgramAcknowledge::NAME = "MsgClasses::CmdProgramAcknowledge";


CmdProgramAcknowledge::CmdProgramAcknowledge(int Timeout, DataManager::ProgramAcknownedgeType_t acknownedgeType) :
    Command(Timeout),
    m_AcknownedgeType(acknownedgeType)
{
}

CmdProgramAcknowledge::CmdProgramAcknowledge()
    : Command(0), m_AcknownedgeType(DataManager::PROGRAM_READY)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdProgramAcknowledge::~CmdProgramAcknowledge()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdProgramAcknowledge::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

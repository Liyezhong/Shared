/****************************************************************************/
/*! \file CmdProgramSelected.cpp
 *
 *  \brief CmdProgramSelected command implementation.
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramSelected.h"

//lint -e578
namespace MsgClasses {

QString CmdProgramSelected::NAME = "MsgClasses::CmdProgramSelected";


CmdProgramSelected::CmdProgramSelected(int Timeout, int retortId, const QString& ProgramID, int ParaffinStepIndex) :
    Command(Timeout),
    m_RetortId(retortId),
    m_ProgramID(ProgramID),
    m_ParaffinStepIndex(ParaffinStepIndex)
{
}

CmdProgramSelected::CmdProgramSelected():
    Command(0),
    m_ProgramID(""),
    m_ParaffinStepIndex(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdProgramSelected::~CmdProgramSelected()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdProgramSelected::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

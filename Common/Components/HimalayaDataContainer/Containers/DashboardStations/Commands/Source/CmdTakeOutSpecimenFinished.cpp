/****************************************************************************/
/*! \file CmdTakeOutSpecimenFinished.cpp
 *
 *  \brief CmdTakeOutSpecimenFinished command implementation.
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdTakeOutSpecimenFinished.h"
namespace MsgClasses {

QString CmdTakeOutSpecimenFinished::NAME = "MsgClasses::CmdTakeOutSpecimenFinished";


CmdTakeOutSpecimenFinished::CmdTakeOutSpecimenFinished(int Timeout) :
    Command(Timeout)
{
}

CmdTakeOutSpecimenFinished::CmdTakeOutSpecimenFinished()
    : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdTakeOutSpecimenFinished::~CmdTakeOutSpecimenFinished()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdTakeOutSpecimenFinished::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

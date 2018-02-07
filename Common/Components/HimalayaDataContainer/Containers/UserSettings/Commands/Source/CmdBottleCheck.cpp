/****************************************************************************/
/*! \file CmdBottleCheck.cpp
 *
 *  \brief this command is used to notify system to run bottle check.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2015-04-30
 *  $Author:    $ Abe Yang
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdBottleCheck.h"

namespace MsgClasses {

QString CmdBottleCheck::NAME = "MsgClasses::CmdBottleCheck";

CmdBottleCheck::CmdBottleCheck(int TimeOut)
    : Command(TimeOut)
{
}

CmdBottleCheck::CmdBottleCheck()
    : Command(0)
{

}

CmdBottleCheck::~CmdBottleCheck(void)
{
}

QString CmdBottleCheck::GetName(void) const
{
    return NAME;
}

} // end namespace MsgClasses

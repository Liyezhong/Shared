/****************************************************************************/
/*! \file CmdBottleCheckReply.cpp
 *
 *  \brief this command is used to reply GUI from scheduler.
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

#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdBottleCheckReply.h"

/*lint -e1401 */

namespace MsgClasses {

QString CmdBottleCheckReply::NAME = "MsgClasses::CmdBottleCheckReply";

CmdBottleCheckReply::CmdBottleCheckReply(int TimeOut,
                                         const QString& stationID,
                                         DataManager::BottleCheckStatusType_t bottleCheckStatusType)
    : Command(TimeOut), m_StationID(stationID), m_BottleCheckStatusType(bottleCheckStatusType)
{
}

CmdBottleCheckReply::CmdBottleCheckReply()
    : Command(0)
{

}

CmdBottleCheckReply::~CmdBottleCheckReply(void)
{
}

QString CmdBottleCheckReply::GetName(void) const
{
    return NAME;
}

} // end namespace MsgClasses

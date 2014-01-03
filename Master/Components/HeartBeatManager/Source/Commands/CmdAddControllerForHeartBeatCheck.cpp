/****************************************************************************/
/*! \file HeartBeatManager/Source/Commands/CmdAddControllerForHeartBeatCheck.cpp
 *
 *  \brief Implementation file for class CmdAddControllerForHeartBeatCheck.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-04-18
 *  $Author:    $ Ramya GJ
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <HeartBeatManager/Include/Commands/CmdAddControllerForHeartBeatCheck.h>

namespace HeartBeatManager {

QString CmdAddControllerForHeartBeatCheck::NAME = "HeartBeatManager::CmdAddControllerForHeartBeatCheck";

/****************************************************************************/
CmdAddControllerForHeartBeatCheck::CmdAddControllerForHeartBeatCheck(int Timeout, quint32 ThreadId) :
    Command(Timeout),
    m_ThreadId(ThreadId)
{
}

/****************************************************************************/
CmdAddControllerForHeartBeatCheck::~CmdAddControllerForHeartBeatCheck() {
}

} // end namespace DataLogging

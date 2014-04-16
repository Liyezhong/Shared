/****************************************************************************/
/*! \file HeartBeatManager/Source/Commands/CmdRemoveControllerForHeartBeatCheck.cpp
 *
 *  \brief Implementation file for class CmdRemoveControllerForHeartBeatCheck.
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

#include <HeartBeatManager/Include/Commands/CmdRemoveControllerForHeartBeatCheck.h>

namespace HeartBeatManager {

QString CmdRemoveControllerForHeartBeatCheck::NAME = "HeartBeatManager::CmdRemoveControllerForHeartBeatCheck";

/****************************************************************************/
CmdRemoveControllerForHeartBeatCheck::CmdRemoveControllerForHeartBeatCheck(int Timeout, quint32 ThreadId) :
    Command(Timeout),
    m_ThreadId(ThreadId)
{
}

/****************************************************************************/
CmdRemoveControllerForHeartBeatCheck::~CmdRemoveControllerForHeartBeatCheck() {
}

} // end namespace HeartBeatManager

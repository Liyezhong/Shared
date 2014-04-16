/****************************************************************************/
/*! \file HeartBeatManager/Source/Commands/CmdHeartBeat.cpp
 *
 *  \brief Implementation file for class CmdHeartBeat.
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

#include <HeartBeatManager/Include/Commands/CmdHeartBeat.h>

namespace HeartBeatManager {

QString CmdHeartBeat::NAME = "HeartBeatManager::CmdHeartBeat";

/****************************************************************************/
CmdHeartBeat::CmdHeartBeat(int Timeout, quint32 ThreadId) :
    Command(Timeout),
    m_ThreadId(ThreadId)
{
}

/****************************************************************************/
CmdHeartBeat::~CmdHeartBeat() {
}

} // end namespace HeartBeatManager

/****************************************************************************/
/*! \file Source/HeartBeatCSVInfo.cpp
 *
 *  \brief Implementation file for class HeartBeatCSVInfo.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-04-21
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

#include <HeartBeatManager//Include/HeartBeatCSVInfo.h>

namespace HeartBeatManager {

/****************************************************************************/
HeartBeatCSVInfo::HeartBeatCSVInfo()
    : m_ThreadId(0)
    , m_ThreadMacroName("")
    , m_ThreadName("")
    , m_Action(Global::HEARTBEATACNTYPE_NONE)
    , m_WaitDurationInHeartBeatTick(0) {
}

/****************************************************************************/
HeartBeatCSVInfo::HeartBeatCSVInfo(const HeartBeatCSVInfo &rOther) {
    CopyFrom(rOther);
}

/****************************************************************************/
HeartBeatCSVInfo::HeartBeatCSVInfo(const quint32 &ThreadId, const QString &ThreadMacroName,
                           const QString &ThreadName, const Global::HeartBeatActionType Action,
                           const quint8 &WaitDurationInHeartBeatTick)
: m_ThreadId(ThreadId)
, m_ThreadMacroName(ThreadMacroName)
, m_ThreadName(ThreadName)
, m_Action(Action)
, m_WaitDurationInHeartBeatTick(WaitDurationInHeartBeatTick) {
}


/****************************************************************************/
HeartBeatCSVInfo::~HeartBeatCSVInfo() {
}

/****************************************************************************/
void HeartBeatCSVInfo::CopyFrom(const HeartBeatCSVInfo &rOther) {

    m_ThreadId = rOther.m_ThreadId;
    m_ThreadMacroName = rOther.m_ThreadMacroName;
    m_ThreadName = rOther.m_ThreadName;
    m_Action = rOther.m_Action;
    m_WaitDurationInHeartBeatTick = rOther.m_WaitDurationInHeartBeatTick;
}


} // end namespace HeartBeatManager


/****************************************************************************/
/*! \file HeartBeatManagerThreadController.cpp
 *
 *  \brief  HeartBeatManager Implementation
 *
 *   $Version: $ 0.1
 *   $Date:    $ 15.11.2012
 *   $Author:  $ Shuvasmita.S
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
//QT Headers
#include <QMetaType>
#include <QDebug>
//Project Headers
#include <HeartBeatManager/Include/HeartBeatThread.h>
#include <Threads/Include/MasterThreadController.h>
#include <Global/Include/Utils.h>

namespace HeartBeatManager{

/****************************************************************************/
/*!
 *  \brief    Constructor for the HeartBeatManagerThreadController
 *
 *  \return
 *
 ****************************************************************************/
HeartBeatThread::HeartBeatThread()

{
}

/****************************************************************************/
/*!
 *  \brief    Destructor for the HeartBeatManagerThreadController
 *
 *  \return
 *
 ****************************************************************************/
HeartBeatThread::~HeartBeatThread()
{

}

void HeartBeatThread::Run()
{

}
void HeartBeatThread::HeartbeatCheck()
{

}
void HeartBeatThread::HeartbeatSlot(const Global::gSourceType &TheHeartBeatSource)
{
}


} //End Of namespace HeartBeatManager

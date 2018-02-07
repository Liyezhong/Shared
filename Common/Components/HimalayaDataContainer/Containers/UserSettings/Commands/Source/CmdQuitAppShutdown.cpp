/****************************************************************************/
/*! \file CmdQuitAppShutdown.cpp
 *
 *  \brief Notice Scheduler that System will shutdown or quit application.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-06-27
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

#include "HimalayaDataContainer/Containers/UserSettings/Commands/Include/CmdQuitAppShutdown.h"



namespace MsgClasses {

QString CmdQuitAppShutdown::NAME = "MsgClasses::CmdQuitAppShutdown";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   TimeOut     Timeout for command.
 *  \param quitAppShutdownActionType =  DataManager::QuitAppShutdownActionType_t type parameter
 */
/****************************************************************************/
CmdQuitAppShutdown::CmdQuitAppShutdown(int TimeOut, DataManager::QuitAppShutdownActionType_t quitAppShutdownActionType)
    : Command(TimeOut), m_quitAppShutdownActionType(quitAppShutdownActionType)
{
}

CmdQuitAppShutdown::CmdQuitAppShutdown(void)
    : Command(0), m_quitAppShutdownActionType(DataManager::QUITAPPSHUTDOWNACTIONTYPE_UNDEFINED)
{
}

CmdQuitAppShutdown::~CmdQuitAppShutdown(void)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdQuitAppShutdown::GetName(void) const
{
    return NAME;
}

} // end namespace MsgClasses

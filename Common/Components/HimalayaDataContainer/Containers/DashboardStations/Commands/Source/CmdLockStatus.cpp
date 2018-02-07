/****************************************************************************/
/*! \file CmdLockStatus.cpp
 *
 *  \brief CmdLockStatus command implementation.
 *
 *   $Version: $ 0.1
*   $Date:    $ 26.03.2013
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdLockStatus.h"
namespace MsgClasses {

QString CmdLockStatus::NAME = "MsgClasses::CmdLockStatus";


CmdLockStatus::CmdLockStatus(int timeout, DataManager::LockType_t lockType, bool isLocked) :
    Command(timeout),
    m_LockType(lockType),
    m_Locked(isLocked)
{
}

CmdLockStatus::CmdLockStatus():
    Command(0),
    m_LockType(DataManager::LOCKTYPE_UNDEFINED),
    m_Locked(false)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdLockStatus::~CmdLockStatus()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdLockStatus::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

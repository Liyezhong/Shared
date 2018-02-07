/****************************************************************************/
/*! \file CmdRetortStatus.cpp
 *
 *  \brief CmdRetortStatus command implementation.
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

#include "HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdRetortStatus.h"
namespace MsgClasses {

QString CmdRetortStatus::NAME = "MsgClasses::CmdRetortStatus";


CmdRetortStatus::CmdRetortStatus(int timeout,
                                    DataManager::RetortStatusType_t retortStatusType) :
    Command(timeout),
    m_RetortStatusType(retortStatusType)
{
}

CmdRetortStatus::CmdRetortStatus()
    : Command(0),
      m_RetortStatusType(DataManager::RETORT_UNDEFINED)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRetortStatus::~CmdRetortStatus()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRetortStatus::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

/****************************************************************************/
/*! \file CmdEventUpdate.cpp
 *
 *  \brief CmdEventUpdate command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.07.2012
 *   $Author:  $ M.Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include "Global/Include/Commands/CmdEventUpdate.h"

namespace Global {

QString CmdEventUpdate::NAME = "MsgClasses::CmdEventUpdate";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout             Timeout for command.
 * \param[in]   eventId             Event ID.
 */
/****************************************************************************/
CmdEventUpdate::CmdEventUpdate(quint32 eventId, QString type, bool stopSystem)
    : Global::Command(0)
    , m_eventId(eventId)
    , m_stopExecution(stopSystem)
    , m_type(type)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdEventUpdate::~CmdEventUpdate()
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor to receive commands
 */
/****************************************************************************/
CmdEventUpdate::CmdEventUpdate()
    : Global::Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdEventUpdate::GetName() const
{
    return NAME;
}

}//End of namespace MsgClasses



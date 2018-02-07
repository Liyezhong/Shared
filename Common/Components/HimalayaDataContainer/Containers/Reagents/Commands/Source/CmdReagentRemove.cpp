/****************************************************************************/
/*! \file CmdReagentRemove.cpp
 *
 *  \brief CmdReagentRemove command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 27.03.2010
 *   $Author:  $ M.Scherer
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

#include <QBuffer>

#include "HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentRemove.h"

namespace MsgClasses {

QString CmdReagentRemove::NAME = "MsgClasses::CmdReagentRemove";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   ReagentID   Unique reagent id.
 */
/****************************************************************************/
CmdReagentRemove::CmdReagentRemove(int Timeout, const QString &ReagentID) :
    Command(Timeout),
    m_ReagentID(ReagentID)
{
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdReagentRemove::CmdReagentRemove() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdReagentRemove::~CmdReagentRemove()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdReagentRemove::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns ID of Reagent to be deleted
 *
 *  \return  Reagent ID
 */
/****************************************************************************/
QString CmdReagentRemove::GetReagentID() const
{
    return m_ReagentID;
}

} // end namespace MsgClasses

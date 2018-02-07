/****************************************************************************/
/*! \file CmdReagentUpdate.cpp
 *
 *  \brief CmdReagentUpdate command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.03.2010
 *   $Author:  $ Y.Novak
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

#include "HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentUpdate.h"

namespace MsgClasses {

QString CmdReagentUpdate::NAME = "MsgClasses::CmdReagentUpdate";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   Timeout             Timeout for command.
 * \param[in]   ReagentDataStream
 */
/****************************************************************************/
CmdReagentUpdate::CmdReagentUpdate(int Timeout, const QDataStream &ReagentDataStream) :
    Command(Timeout)
{
    m_ReagentByteArray = static_cast< QBuffer *>(ReagentDataStream.device())->data();
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdReagentUpdate::CmdReagentUpdate() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdReagentUpdate::~CmdReagentUpdate()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdReagentUpdate::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

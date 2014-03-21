/****************************************************************************/
/*! \file CmdRackUpdate.cpp
 *
 *  \brief CmdRackUpdate command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.01.2012
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

#include "TestStubDataManager/Containers/Racks/Commands/Include/CmdRackUpdate.h"

namespace MsgClasses {

QString CmdRackUpdate::NAME = "MsgClasses::CmdRackUpdate";

/****************************************************************************/
/*!
 * \brief   Constructor for sending
 *
 * \param[in]   Timeout             Timeout for command.
 * \param[in]   ProgramSequence     Program sequence.
 */
/****************************************************************************/
CmdRackUpdate::CmdRackUpdate(int Timeout, const QDataStream &RackStream) :
    Command(Timeout)
{
    m_RackByteArray = static_cast< QBuffer *>(RackStream.device())->data();
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdRackUpdate::CmdRackUpdate() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRackUpdate::~CmdRackUpdate()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRackUpdate::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

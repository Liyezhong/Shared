/****************************************************************************/
/*! \file CmdDataUpdate.cpp
 *
 *  \brief CmdDataUpdate command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-11-29
 *   $Author:  $ Michael Thiel
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
#include "NetCommands/Include/CmdDataUpdate.h"

namespace NetCommands {

QString CmdDataUpdate::NAME = "NetCommands::CmdDataUpdate";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   Station     QByteArray containing station.
 */
/****************************************************************************/
CmdDataUpdate::CmdDataUpdate(int Timeout, const QDataStream &DataUpdateDataStream) :
    Command(Timeout)
{
    m_DataUpdateByteArray = static_cast< QBuffer *>(DataUpdateDataStream.device())->data();
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdDataUpdate::CmdDataUpdate() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdDataUpdate::~CmdDataUpdate()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdDataUpdate::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

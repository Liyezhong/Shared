/****************************************************************************/
/*! \file CmdReagentAdd.cpp
 *
 *  \brief CmdReagentAdd command implementation.
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

#include "HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentAdd.h"
#include <QBuffer>
namespace MsgClasses {

QString CmdReagentAdd::NAME = "MsgClasses::CmdReagentAdd";

/****************************************************************************/
/*!
 *  \brief   Constructor for sending
 *
 * \param[in]   Timeout             Timeout for command.
 * \param[in]   ReagentDataStream
 */
/****************************************************************************/
CmdReagentAdd::CmdReagentAdd(int Timeout, const QDataStream &ReagentDataStream) :
    Command(Timeout)
{
    m_ReagentByteArray = static_cast< QBuffer *>(ReagentDataStream.device())->data();
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdReagentAdd::CmdReagentAdd() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdReagentAdd::~CmdReagentAdd()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdReagentAdd::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

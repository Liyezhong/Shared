/****************************************************************************/
/*! \file CmdProgramDeleteItem.cpp
 *
 *  \brief CmdProgramDeleteItem command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 18.03.2010
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

#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdProgramDeleteItem.h"

namespace MsgClasses {

QString CmdProgramDeleteItem::NAME = "MsgClasses::CmdProgramDeleteItem";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   ItemId      Item ID.
 */
/****************************************************************************/
CmdProgramDeleteItem::CmdProgramDeleteItem(int Timeout, const QString &ItemId) :
    Command(Timeout),
    m_ItemId(ItemId)
{
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdProgramDeleteItem::CmdProgramDeleteItem() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdProgramDeleteItem::~CmdProgramDeleteItem()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdProgramDeleteItem::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns ID of item to be deleted
 *
 *  \return  item ID (program, macro, etc)
 */
/****************************************************************************/
QString CmdProgramDeleteItem::GetItemId() const
{
    return m_ItemId;
}

} // end namespace MsgClasses

/****************************************************************************/
/*! \file CmdRackRemove.cpp
 *
 *  \brief CmdWhiteRackProgram command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 16.03.2011
 *   $Author:  $ N.Kamath
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

#include "DataManager/Containers/Racks/Commands/Include/CmdRackRemove.h"

namespace MsgClasses {

QString CmdRackRemoved::NAME = "MsgClasses::CmdRackRemoved";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   RackRfid      Rack RFID.
 */
/****************************************************************************/
CmdRackRemoved::CmdRackRemoved(int Timeout, const quint32 &RackRfid) :
    Command(Timeout),
    m_RackRfid(RackRfid)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRackRemoved::~CmdRackRemoved()
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor to receive commands
 */
/****************************************************************************/
CmdRackRemoved::CmdRackRemoved() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRackRemoved::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns Rack Rfid
 *
 *  \return  RackRfid
 */
/****************************************************************************/
quint32 CmdRackRemoved::GetRackRfid() const
{
    return m_RackRfid;
}

}//End of namespace MsgClasses


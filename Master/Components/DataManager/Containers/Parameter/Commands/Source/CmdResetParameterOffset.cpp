/****************************************************************************/
/*! \file CmdResetParameterOffset.cpp
 *
 *  \brief ResetParameterOffset command implementation.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2013-09-01
 *   $Author:  $ Thirumalesha R
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

#include "ColoradoDataManager/Containers/Parameter/Commands/Include/CmdResetParameterOffset.h"

namespace MsgClasses {

QString CmdResetParameterOffset::NAME = "MsgClasses::CmdResetParameterOffset";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout             Timeout for command.
 * \param[in]   ParameterOffset     Parameter Offset
 */
/****************************************************************************/
CmdResetParameterOffset::CmdResetParameterOffset(int Timeout, QString ParameterKey) :
    Command(Timeout),
    m_ParameterKey(ParameterKey)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdResetParameterOffset::~CmdResetParameterOffset()
{
}

/****************************************************************************/
/*!
 *  \brief  Constructor to receive commands
 */
/****************************************************************************/
CmdResetParameterOffset::CmdResetParameterOffset() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdResetParameterOffset::GetName() const
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
QString CmdResetParameterOffset::GetParameterKey()
{
    return m_ParameterKey;
}

}//End of namespace MsgClasses


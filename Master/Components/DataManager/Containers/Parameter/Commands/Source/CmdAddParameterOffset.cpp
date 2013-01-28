/****************************************************************************/
/*! \file CmdAddParameterOffset.cpp
 *
 *  \brief CmdAddParameterOffset command implementation.
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

#include <QBuffer>

#include "DataManager/Containers/Parameter/Commands/Include/CmdAddParameterOffset.h"


namespace MsgClasses {

QString CmdAddParameterOffset::NAME = "MsgClasses::CmdAddParameterOffset";

/****************************************************************************/
/*!
 * \brief   Constructor for sending
 *
 * \param[in]   Timeout             Timeout for command.
 * \param[in]   ProgramSequence     Program sequence.
 */
/****************************************************************************/
CmdAddParameterOffset::CmdAddParameterOffset(int Timeout, const QByteArray & ParameterOffset) :
    Command(Timeout)
{
    m_ParameterOffset = ParameterOffset;
}

/****************************************************************************/
/*!
 * \brief   Constructor for receiving
 */
/****************************************************************************/
CmdAddParameterOffset::CmdAddParameterOffset() : Command(0)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdAddParameterOffset::~CmdAddParameterOffset()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdAddParameterOffset::GetName() const
{
    return NAME;
}

} // end namespace MsgClasses

/****************************************************************************/
/*! \file Components/DataManager/Containers/UserSettings/Commands/Source/CmdRmsOnOff.cpp
 *
 *  \brief CmdRmsOnOff command implementation.
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

#include "DataManager/Containers/UserSettings/Commands/Include/CmdRmsOnOff.h"

namespace MsgClasses {

QString CmdRmsOnOff::NAME = "MsgClasses::CmdRmsOnOff";

/****************************************************************************/
/*!
 *  \brief   Constructor
 *
 * \param[in]   Timeout     Timeout for command.
 * \param[in]   RmsState    RMS state: true if on.
 */
/****************************************************************************/
CmdRmsOnOff::CmdRmsOnOff(int Timeout, bool RmsState) :
    Command(Timeout),
    m_RmsState(RmsState)
{
}

/****************************************************************************/
/*!
 *  \brief   Destructor
 */
/****************************************************************************/
CmdRmsOnOff::~CmdRmsOnOff()
{
}

/****************************************************************************/
/*!
 *  \brief   Get command name
 *
 *  \return  command name as string
 */
/****************************************************************************/
QString CmdRmsOnOff::GetName() const
{
    return NAME;
}

/****************************************************************************/
/*!
 *  \brief   This function returns requested RMS state
 *
 *  \return  RMS state
 */
/****************************************************************************/
bool CmdRmsOnOff::GetRmsState() const
{
    return m_RmsState;
}

} // end namespace MsgClasses

/****************************************************************************/
/*! \file CmdDisableGUIAction.cpp
 *
 *  \brief Implementation file for class CmdDisableGUIAction.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-12-03
 *  $Author:    $ C.Adaragunchi
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2011 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <NetCommands/Include/CmdDisableGUIAction.h>

namespace NetCommands {

QString CmdDisableGUIAction::NAME           = "NetCommands::CmdDisableGUIAction";

/****************************************************************************/
CmdDisableGUIAction::CmdDisableGUIAction(int TimeOut, const ActionGroupType_t ActionGroupType, bool Enable) :
    Global::Command(TimeOut),
    m_ActionGroupType(ActionGroupType),
    m_Enable(Enable)
{
}

/****************************************************************************/
CmdDisableGUIAction::CmdDisableGUIAction():
    Global::Command(0),
    m_ActionGroupType(ACTION_GROUP1),
    m_Enable(true)
{

}

/****************************************************************************/
CmdDisableGUIAction::~CmdDisableGUIAction()
{

}

/****************************************************************************/
/*!
 *  \brief   This function returns the GUI Action Group type
 *
 *  \return  GUI Action Group type
 */
/****************************************************************************/
ActionGroupType_t CmdDisableGUIAction::GetActionGroupType() const
{
    return m_ActionGroupType;
}

/****************************************************************************/
/*!
 *  \brief   This function returns the GUI enable state
 *
 *  \return  True for Enable else False for disable.
 */
/****************************************************************************/
bool CmdDisableGUIAction::GetGUIEnableState() const
{
    return m_Enable;
}

} // end namespace NetCommands

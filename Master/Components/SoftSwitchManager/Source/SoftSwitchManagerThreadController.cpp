/****************************************************************************/
/*! \file SoftSwitchManagerThreadController.cpp
 *
 *  \brief SoftSwitchManager Implementation
 *
 *   $Version: $ 0.1
 *   $Date:    $ 15.11.2012
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
//QT Headers
#include <QMetaType>
#include <QDebug>

//Project Headers
#include <SoftSwitchManager/Include/SoftSwitchManagerThreadController.h>
#include <Global/Include/Utils.h>
#include <Global/Include/Commands/CmdSoftSwitchPressed.h>

namespace SoftSwitchManager {

/****************************************************************************/
/*!
 *  \brief    Constructor for the SoftSwitchManagerThreadController
 *
 *  \return
 *
 ****************************************************************************/
SoftSwitchManagerThreadController::SoftSwitchManagerThreadController(Global::gSourceType TheHeartBeatSource) :
    Threads::ThreadController(TheHeartBeatSource, "SoftSwitchManager")
{
}


/****************************************************************************/
/*!
 *  \brief    Destructor for the SoftSwitchManagerThreadController
 *
 *  \return
 *
 ****************************************************************************/
SoftSwitchManagerThreadController::~SoftSwitchManagerThreadController()
{

}

/****************************************************************************/
/*!
 *  \brief    API function to call before EventHandler can be used
 *
 *      This function does everything to initialize the EventHandler. It
 *      shall be called before EventHandler can be used.
 *
 ****************************************************************************/
void SoftSwitchManagerThreadController::CreateAndInitializeObjects()
{
    // now register commands
    RegisterCommands();
}

/****************************************************************************/
/*!
 *  \brief Destroy all used objects and prepare for shutdown.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void SoftSwitchManagerThreadController::CleanupAndDestroyObjects()
{
}
/****************************************************************************/
/*!
 *  \brief Register commands handled by thread Controller
 *
/****************************************************************************/
void SoftSwitchManagerThreadController::RegisterCommands()
{
    RegisterAcknowledgeForProcessing<Global::AckOKNOK, SoftSwitchManagerThreadController>
            (&SoftSwitchManagerThreadController::OnAcknowledge, this);
}

/****************************************************************************/
/*!
 *  \brief This method is called when the base class receives Go signal.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void SoftSwitchManagerThreadController::OnGoReceived()
{
    Global::CmdSoftSwitchPressed *p_SoftSwitchPressedCommand = new Global::CmdSoftSwitchPressed();
    p_SoftSwitchPressedCommand->PressedAtStartUp = true;
    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_SoftSwitchPressedCommand));
}

/****************************************************************************/
/*!
 *  \brief This method is called when the base class receives Stop signal.
 *
 *         See detailed description in the Base Class's header
 *
 *  \return
 */
/****************************************************************************/
void SoftSwitchManagerThreadController::OnStopReceived()
{

}

/****************************************************************************/
/**
 * \brief Power will fail shortly.
 *
 * Power will fail shortly.
 */
/****************************************************************************/
void SoftSwitchManagerThreadController::OnPowerFail()
{
    /// \todo implement
}

void SoftSwitchManagerThreadController::SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd)
{
    qDebug() << "SoftSwitchManagerThreadController::SendCommand" << Ref;
    Threads::ThreadController::SendCommand(Ref, Cmd);
}

Global::tRefType SoftSwitchManagerThreadController::GetNewCommandRef()
{
    return Threads::ThreadController::GetNewCommandRef();
}

void SoftSwitchManagerThreadController::OnAcknowledge(Global::tRefType, const Global::AckOKNOK &)
{

}

} //End Of namespace SoftSwitchManager

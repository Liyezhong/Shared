/****************************************************************************/
/*! \file Include/SoftSwitchManagerThreadController.h
 *
 *  \brief  Definition of class SoftSwitchManager
 *
 *  Version:    0.1
 *  Date:       2012-11-15
 *  Author:     N.Kamath
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
#ifndef SOFTSWITCHMANAGERTHREADCONTROLLER_H
#define SOFTSWITCHMANAGERTHREADCONTROLLER_H
//QT Headers
#include <QObject>

//Project Headers
#include <Global/Include/GlobalDefines.h>
#include <Threads/Include/ThreadController.h>
#include <Global/Include/Commands/AckOKNOK.h>



namespace SoftSwitchManager {

/****************************************************************************/
/**
 * \brief This Class shall be responsible to manage the softswitch related
 *        events.They include
 *        1) Manage LEDS
 *        2) Inform master thread to start initialization
 *        3) Initiate the shutdown flow and inform master thread to shutdown
 */
/****************************************************************************/
class SoftSwitchManagerThreadController : public Threads::ThreadController {
    Q_OBJECT

public:
    SoftSwitchManagerThreadController(Global::gSourceType TheHeartBeatSource);
    virtual ~SoftSwitchManagerThreadController();
    virtual void CreateAndInitializeObjects();
    virtual void CleanupAndDestroyObjects();

    /****************************************************************************/
    /**
      * \brief Send command to external process.
      *
      * Send a command to the external process. Reference mapping does not
      * have to be done since the acknowledge we receive from the external
      * process corresponds with Ref.
      *
      * \param[in]   Ref                 Command reference.
      * \param[in]   Cmd                 Command to send.
      */
    /****************************************************************************/
    void SendCommand(Global::tRefType Ref, const Global::CommandShPtr_t &Cmd);
    Global::tRefType GetNewCommandRef();

protected:
    void OnGoReceived();
    void OnStopReceived();
    virtual void OnPowerFail();

private:
    Q_DISABLE_COPY(SoftSwitchManagerThreadController) //!< Disable copy and assignment
    void RegisterCommands();
    void OnAcknowledge(Global::tRefType, const Global::AckOKNOK &);
};

} // end namespace EventHandler
#endif // SOFTSWITCHMANAGERTHREADCONTROLLER_H

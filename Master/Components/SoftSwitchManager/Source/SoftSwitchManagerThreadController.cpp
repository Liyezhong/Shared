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
//Std lib headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <QSocketNotifier>
//Project Headers
#include <SoftSwitchManager/Include/SoftSwitchManagerThreadController.h>
#include <Global/Include/Utils.h>
#include <Global/Include/Commands/CmdSoftSwitchPressed.h>
#include <SoftSwitchManager/Include/GPIO.h>
#include <EventHandler/Include/StateHandler.h>

namespace SoftSwitchManager {

/****************************************************************************/
/*!
 *  \brief    Constructor for the SoftSwitchManagerThreadController
 *
 *  \return
 *
 ****************************************************************************/
SoftSwitchManagerThreadController::SoftSwitchManagerThreadController(Global::gSourceType TheHeartBeatSource)
    :Threads::ThreadController(TheHeartBeatSource, "SoftSwitchManager")
    ,m_SoftSwitchManager(this)
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
    //Connect SoftSwitchManager to SoftSwitchPressed signal
    //CONNECTSIGNALSIGNAL(this, OnSoftSwitchPressed(), &m_SoftSwitchManager, OnSoftSwitchPressed());

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
 */
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
    CONNECTSIGNALSLOT(&m_SoftSwitchManager, SendSoftSwitchPressedCmd(), this , SendSoftSwitchPressedCmd());
    CONNECTSIGNALSIGNAL(this, OnSoftSwitchPressed(), &m_SoftSwitchManager, SoftSwitchPressed());
    qDebug()<<"Current Thread" << thread();
    qDebug()<<"SoftSwitchMgr thread" <<m_SoftSwitchManager.thread();
    m_SoftSwitchManager.ConnectSignals();
    //struct pollfd fdset[2];
    //int NumbOfFileDesc = 2;
    //char *p_Buf[100];
    Global::EventObject::Instance().RaiseEvent(EVENT_SOFTSWITCH_MONITOR_START);
    EventHandler::StateHandler::Instance().setStateToSoftSwitchMonitorState();
    emit OnSoftSwitchPressed();
    QFile *File = new QFile(this);
    File->open(stdin,QIODevice::ReadOnly);
    QSocketNotifier *notifier = new QSocketNotifier(File->handle(), QSocketNotifier::Read);
    CONNECTSIGNALSLOT(notifier, activated(int), &m_SoftSwitchManager, OnSoftSwitchPressed(int));
//    while (1) {
//        memset((void*)fdset, 0, sizeof(fdset));
//        fdset[0].fd = STDIN_FILENO; //Standard input
//        fdset[0].events = POLLIN;
//        int TimeOut = -1; // Infinite Timeout
//        int  PollReturn = poll(fdset, NumbOfFileDesc, 4000);

//        if (PollReturn < 0) {
//            qDebug("\npoll() failed!\n");
//            return;
//        }

//        if (PollReturn == 0) {
//            qDebug(".");
//        }
//        //SoftSwitch GPIO
//        if (fdset[1].revents & POLLPRI) {
//            int len = read(fdset[1].fd, p_Buf, 100);
//            qDebug()<<"\npoll() GPIO interrupt occurred\n";
//            emit OnSoftSwitchPressed();
//        }

//        if (fdset[0].revents & POLLIN) {
//            (void)read(fdset[0].fd, p_Buf, 1);
//            qDebug()<<"\npoll() stdin read:"<< p_Buf[0];
//            emit OnSoftSwitchPressed();
//        }

//        fflush(stdin);
//        rewind(stdin);
//    }
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

void SoftSwitchManagerThreadController::SendSoftSwitchPressedCmd()
{
    qDebug()<<"Sending SoftSwitchPressed Cmd";
    Global::EventObject::Instance().RaiseEvent(EVENT_SOFTSWITCH_PRESSED_FIRST_TIME);

    Global::CmdSoftSwitchPressed *p_SoftSwitchPressedCommand = new Global::CmdSoftSwitchPressed();
    p_SoftSwitchPressedCommand->PressedAtStartUp = true;
    SendCommand(GetNewCommandRef(), Global::CommandShPtr_t(p_SoftSwitchPressedCommand));
}

void SoftSwitchManagerThreadController::TempInitComplete()
{
    qDebug()<<"\n\n\n SoftSwitchManagerThreadController::TempInitComplete(); \n\n\n";
}

} //End Of namespace SoftSwitchManager

/****************************************************************************/
/*! \file ExtProcessStartRetry.cpp
 *
 *  \brief Implementation file for class ExtProcessStartRetry.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 07.12.2010
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

#include <QDebug>
#include <ExternalProcessController/Include/ExternalProcessStates/ExtProcessStartRetry.h>
#include <Global/Include/Utils.h>

namespace ExternalProcessControl {

/****************************************************************************/
/*!
*  \brief    Constructor.
*
*  \param[in]    wObj = pointer to working object / parent
*
****************************************************************************/
ExtProcessStartRetry::ExtProcessStartRetry(ExternalProcessController *wObj) :
        State::State("ExtProcessStartRetry", wObj),
        m_myController(wObj),
        m_myTimer(this)
{
}

/****************************************************************************/
/*!
*  \brief    Destructor.
*
****************************************************************************/
ExtProcessStartRetry::~ExtProcessStartRetry()
{
    m_myController = NULL;
}

/****************************************************************************/
/*!
*  \brief    This function is called when the State is entered.
*
*  \param[in]    et = the event to process
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool ExtProcessStartRetry::OnEntry(StateMachines::StateEvent et)
{
    qDebug() << "ExtProcessStartRetryState entered.";

    if (m_myController == NULL) {
        /// \todo log error
        //StateMachines::StateEvent et1;
        //et1.SetIndex(EP_NULL_CTRL_POINTER);
        if (!State::DispatchEvent(Global::AsInt(EP_NULL_CTRL_POINTER))) {
            /// \todo log error
        }
        return false;
    }

    // stop device's powerup login timer if it is runnning
    m_myController->m_myDevice->StopLoginGuard();

    if (m_myTimer.isActive()) {
        /// \todo log error
        // the external process needs to be restarted within the
        // guard window --> something is wrong, goto FatalError.
        m_myTimer.stop();
        //StateMachines::StateEvent et0;
        //et0.SetIndex(EP_TOO_MANY_RESTARTS);
        if (!State::DispatchEvent(Global::AsInt(EP_TOO_MANY_RESTARTS))) {
            /// \todo log error
        }
        return false;
    }

    try {
        CONNECTSIGNALSLOT(&m_myTimer, timeout(), this, OnWindowTimerTimeout());
    } catch (...) {
        /// \todo log error
        //StateMachines::StateEvent et2;
        //et2.SetIndex(EP_SIGNALCONNECT_FAILED);
        if (!State::DispatchEvent(Global::AsInt(EP_SIGNALCONNECT_FAILED))) {
            /// \todo log error
        }
        return false;
    }

    return HandleEvent(et);
}

/****************************************************************************/
/*!
*  \brief    This function is called when the State is exited.
*
*  \param[in]    et = the event to process
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool ExtProcessStartRetry::OnExit(StateMachines::StateEvent et)
{
    Q_UNUSED(et)
    qDebug() << "ExtProcessStartRetryState exited.";

    // stop device's powerup login timer: if we exit this state,
    // this means we're either OK or dead. in both cases there is
    // no need for LoginGuard.
    if (m_myController != NULL) {
        m_myController->m_myDevice->StopLoginGuard();
    }
    else {
        return false;
    }

    return true;
}

/****************************************************************************/
/*!
*  \brief    This function processes state's internal event.
*
*  \param[in]    et = the event to process
*
*  \return   TRUE if executed successfully, FALSE otherwise
*
****************************************************************************/
bool ExtProcessStartRetry::HandleEvent(StateMachines::StateEvent et)
{
    qDebug() << "ExtProcessStartRetryState handles internal event " + QString::number(et.GetIndex(), 10);

    if (m_myController == NULL) {
        return false;
    }

    switch (et.GetIndex()) {

    case EP_EXTPROCESS_LOGIN_TIMEOUT:
        // the process, which was restarted in this state, failed to login:
        // nothing can be done. we kill it and goto fatal error
        if (!m_myController->m_myProcess->KillProcess()) {
            qDebug() << (QString)("ExtProcessRestartState: Cannot kill the GUI Process !");
            /// \todo log error
            m_myTimer.stop(); // no need for window timer: stop it
            //StateMachines::StateEvent et1;
            //et1.SetIndex(EP_CANNOT_KILL_EXTPROCESS);
            if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_KILL_EXTPROCESS))) {
                /// \todo log error
            }
            return false;
        }
        break;

    case EP_EXTPROCESS_EXITED:
        // check if we tried to restart the process already:
        if (m_myTimer.isActive()) {
            /// \todo log error
            // the external process needs to be restarted within the
            // guard window --> something is wrong, goto FatalError.
            m_myTimer.stop();
            //StateMachines::StateEvent et0;
            //et0.SetIndex(EP_TOO_MANY_RESTARTS);
            if (!State::DispatchEvent(Global::AsInt(EP_TOO_MANY_RESTARTS))) {
                /// \todo log error
            }
            return false;
        }
        // configure and start the "restart window" timer:
        m_myTimer.setSingleShot(true);
        m_myTimer.setInterval(PROCESS_RESTART_WINDOW);
        m_myTimer.start();
        // WARNING: no break here, we go straight into next case -->
    case EP_CANNOT_START_EXTPROCESS:
        // try to start external process again
        //if (!m_myController->m_myProcess->StartProcess(EP_START_CMD)) {
        if (!m_myController->m_myProcess->StartProcess(m_myController->m_myCommand)) {
            qDebug() << (QString)("ExtProcessRestartState: Cannot start the GUI Process !");
            /// \todo log error
            m_myTimer.stop();
            //StateMachines::StateEvent et2;
            //et2.SetIndex(EP_CANNOT_START_EXTPROCESS);
            if (!State::DispatchEvent(Global::AsInt(EP_CANNOT_START_EXTPROCESS))) {
                /// \todo log error
            }
            return false;
        }

        // restart device's powerup login timer
        m_myController->m_myDevice->StartLoginGuard();
        break;

    default :
        break;
    }

    return true;
}

/****************************************************************************/
/*!
*  \brief    This function stops internal timer on its timeout.
*
*
****************************************************************************/
void ExtProcessStartRetry::OnWindowTimerTimeout()
{
    qDebug() << "ExtProcessStartRetryState stopped the window guard timer.";
    m_myTimer.stop();
}

} // end namespace ExternalProcessControl

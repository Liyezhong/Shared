/****************************************************************************/
/*! \file WaitState.h
 *
 *  \brief Definition file for class WaitState.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 17.02.2011
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

#ifndef EXTERNALPROCESSCONTROL_WAITSTATE_H
#define EXTERNALPROCESSCONTROL_WAITSTATE_H

#include <QObject>
#include <QHash>
#include <StateMachines/Include/State.h>
#include <ExternalProcessController/Include/ExternalProcessController.h>


namespace ExternalProcessControl {

/// number of allowed disconnects during guard window:
const quint8 EPC_NOF_ALLOWED_DISCONNECTS = 1;

/*! Timeout for the connect guard window timer:
    if external process connects and disconnects several times during timer's interval,
    there is something wrong -> goto FatalError */
const int CONNECT_GUARD_WINDOW = 60000; // 60000 ms = 1 minute

/****************************************************************************/
/**
* \brief This class handles state machine events and corresponding
*        actions in the state machine entry state: external process is not yet
*        started.
*
*/
/****************************************************************************/
class WaitState : public StateMachines::State
{
    Q_OBJECT

    /// Test classes are all friends
    friend class TestExternalProcessCtlr;
    friend class TestExternalProcessCtlr2;

public:

    WaitState(ExternalProcessController *wObj);
    ~WaitState();

public:

    bool OnEntry(StateMachines::StateEvent et);
    bool OnExit(StateMachines::StateEvent et);

private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(WaitState)
    bool HandleEvent(StateMachines::StateEvent et);

private:

    /// pointer to the working object
    ExternalProcessController *m_myController;
    /// counter of actual disconnects (if there too many during guard window --> go bum!)
    quint8                     m_NofDisconnects;
    /// connect/disconnect guard window timer
    QTimer                     m_myTimer;
};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_WAITSTATE_H

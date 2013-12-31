/****************************************************************************/
/*! \file ExtProcessStartRetry.h
 *
 *  \brief Definition file for class ExtProcessStartRetry.
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

#ifndef EXTERNALPROCESSCONTROL_EXTPSTARTRETRYSTATE_H
#define EXTERNALPROCESSCONTROL_EXTPSTARTRETRYSTATE_H

#include <QObject>
#include <QHash>
#include <StateMachines/Include/State.h>
#include <ExternalProcessController/Include/ExternalProcessController.h>


namespace ExternalProcessControl {

/*! Timeout for the restart window timer:
    if external process needs to be restarted several times during timer's interval,
    there is something wrong -> goto FatalError */
const int PROCESS_RESTART_WINDOW = 60000; // 60000 ms = 1 minute

/****************************************************************************/
/**
* \brief This class handles state machine events and corresponding
*        actions in case when external process crashed or could not start:
*        external process needs to be restarted.
*
*/
/****************************************************************************/
class ExtProcessStartRetry : public StateMachines::State
{
    Q_OBJECT

    /// Test classes are all friends
    friend class TestExternalProcessCtlr;

public:

    ExtProcessStartRetry(ExternalProcessController *wObj);
    ~ExtProcessStartRetry();

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
    Q_DISABLE_COPY(ExtProcessStartRetry)
    bool HandleEvent(StateMachines::StateEvent et);

    /// pointer to the working object
    ExternalProcessController *m_myController;
};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_EXTPSTARTRETRYSTATE_H

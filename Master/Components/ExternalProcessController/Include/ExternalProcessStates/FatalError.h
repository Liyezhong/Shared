/****************************************************************************/
/*! \file FatalError.h
 *
 *  \brief Definition file for class FatalError.
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

#ifndef EXTERNALPROCESSCONTROL_FATALERRORSTATE_H
#define EXTERNALPROCESSCONTROL_FATALERRORSTATE_H

#include <QObject>
#include <QHash>
#include <StateMachines/Include/State.h>
#include <ExternalProcessController/Include/ExternalProcessController.h>


namespace ExternalProcessControl {

/****************************************************************************/
/**
* \brief This class handles state machine events and corresponding
*        actions in case of Fatal error: external process has a problem and
*        cannot be used.
*
*/
/****************************************************************************/
class FatalError : public StateMachines::State
{
    Q_OBJECT

    /// Test classes are all friends
    friend class TestExternalProcessCtlr;
    friend class TestExternalProcessCtlr2;

public:

    FatalError(ExternalProcessController *wObj);
    ~FatalError();

public:

    bool OnEntry(StateMachines::StateEvent et);
    bool OnExit(StateMachines::StateEvent et);

private:

    bool HandleEvent(StateMachines::StateEvent et);

private:

    /// pointer to the working object
    ExternalProcessController *m_myController;
};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_FATALERRORSTATE_H

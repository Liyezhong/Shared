/****************************************************************************/
/*! \file Working.h
 *
 *  \brief Definition file for class Working.
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

#ifndef EXTERNALPROCESSCONTROL_ALLOKSTATE_H
#define EXTERNALPROCESSCONTROL_ALLOKSTATE_H

#include <QObject>
#include <QHash>
#include <StateMachines/Include/State.h>
#include <ExternalProcessController/Include/ExternalProcessController.h>


namespace ExternalProcessControl {

/****************************************************************************/
/**
* \brief This class handles state machine events and corresponding
*        actions in the normal working state: external process is connected
*        and runs as expected.
*
*/
/****************************************************************************/
class Working : public StateMachines::State
{
    Q_OBJECT

    /// Test classes are all friends
    friend class TestExternalProcessCtlr;
    friend class TestExternalProcessCtlr2;

public:

    Working(ExternalProcessController *wObj);
    ~Working();

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
    Q_DISABLE_COPY(Working)
    bool HandleEvent(StateMachines::StateEvent et);

private:

    /// pointer to the working object
    ExternalProcessController *m_myController;

};

} // end namespace ExternalProcessControl

#endif // EXTERNALPROCESSCONTROL_ALLOKSTATE_H

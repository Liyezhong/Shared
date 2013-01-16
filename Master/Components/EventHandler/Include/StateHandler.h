/****************************************************************************/
/*! \file EventHandler/Include/StateHandler.h
 *
 *  \brief Definition file for class StateHandler (previously ColoradoMasterThread/Include/ColoradoStateHandler.h)
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-07-17
 *  $Author:    $ M.Thiel
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2012 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef EVENTHANDLER_STATEHANDLER_H
#define EVENTHANDLER_STATEHANDLER_H

#include <QStateMachine>
#include "Global/Include/Commands/Command.h"
#include "Global/Include/Commands/CmdEventUpdate.h"
#include "Threads/Include/ThreadController.h"

namespace EventHandler {


/****************************************************************************/
/**
 * \brief State handler for application.
 */
/****************************************************************************/
class StateHandler : public QObject
{
    Q_OBJECT

public:
    /****************************************************************************/
    /**
     * \brief Get reference to instance.
     *
     * \return      Reference to instance.
     */
    /****************************************************************************/
    static inline StateHandler &Instance()
    {
        return m_StateHandlerInstance;
    }

    /**
      @brief Checks whether a specific command is allowed in current state
      */
    bool isAllowed(Global::CommandShPtr_t command);

    /**
      @brief Adds or removes the given eventId to or from list of errors. Triggers then a transition to required state (normal or error)
      */
    void setAvailability(bool active, quint32 eventId);

    void setActivityUpdate(bool active, quint32 activityId);

    void setInitState();
    void setIdleState();
    void setStateToSoftSwitchMonitorState();
    QString getCurrentOperationState();
    QString getCurrentAvailabilityState();

private:
    StateHandler();
    ~StateHandler();
    static StateHandler m_StateHandlerInstance; //!< Static instance
    Q_DISABLE_COPY(StateHandler ) //!< Disable Copy and assignment

    QStateMachine m_operationMachine;
    QStateMachine m_availabilityMachine;

    QState *m_DefaultState;
    QState *m_SoftSwitchMonitorState;
    QState *m_InitState;
    QState *m_idleState;
    QState *m_busyState;

    QState *m_normalState;
    QState *m_errorState;

    //QState *m_startupState;
    QList<quint32> m_errorList;
    QList<quint32> m_rackList;

private slots:
    void onAvailabilityStateChanged();
    void onOperationStateChanged();

//    /**
//      @brief Is called when error state is entered. Will inform GUI and scheduler about stopping the execution
//      */
//    void onErrorState();

//    /**
//      @brief Is called when normal state is entered, the system is error free. Will inform GUI and scheduler about resuming the execution
//      */
//    void onNormalState();

signals:
    void enterErrorState();
    void enterNormalState();

    void enterIdleState();
    void enterBusyState();
    void softSwitchPressed();
    void initComplete();
    void softSwitchMonitorStart();

    void stateChanged(QString);

}; // end class

} // end namespace EventHandler

#endif // EVENTHANDLER_STATEHANDLER_H

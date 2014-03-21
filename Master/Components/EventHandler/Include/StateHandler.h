/****************************************************************************/
/*! \file EventHandler/Include/StateHandler.h
 *
 *  \brief Definition file for class StateHandler (previously HimalayaMasterThread/Include/HimalayaStateHandler.h)
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-07-17
 *  $Author:    $ Michael Thiel
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
#include <QMutex>
#include <QDebug>

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
    friend class TestStateHandler;

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
    /****************************************************************************/
    /**
     * \brief Checks whether a specific command is allowed in current state
     *
     * \iparam  command
     *
     * \return  retVal.
     */
    /****************************************************************************/
     bool isAllowed(Global::CommandShPtr_t command);
     /****************************************************************************/
     /**
      * \brief Adds or removes the given eventId to or from list of errors. Triggers
      *        then a transition to required state (normal or error)
      *
      * \iparam  active
      * \iparam  eventId
      */
     /****************************************************************************/
    void setAvailability(bool active, quint32 eventId);
    /****************************************************************************/
    /**
     * \brief
     *
     * \iparam active
     * \iparam activityId
     */
    /****************************************************************************/
    void setActivityUpdate(bool active, quint32 activityId);
    /****************************************************************************/
    /**
     * \brief sets the state to Init state
     */
    /****************************************************************************/
    void setInitState();
    /****************************************************************************/
    /**
     * \brief sets the state to idle state
     */
    /****************************************************************************/
    void setIdleState();
    /****************************************************************************/
    /**
     * \brief Sets the state to SoftSwitch Monitor state.
     */
    /****************************************************************************/
    void setStateToSoftSwitchMonitorState();
    /****************************************************************************/
    /**
     * \brief  Gets the Current state of Operating machine.
     *
     * \return  Current Operation State
     */
    /****************************************************************************/
    QString getCurrentOperationState();
    /****************************************************************************/
    /**
     * \brief Gets the current state machine availability.
     *
     * \return Current Availability State
     */
    /****************************************************************************/
    QString getCurrentAvailabilityState();
    /****************************************************************************/
    /**
     * \brief sets the Init stage progress
     *
     * \iparam stage
     * \iparam success
     */
    /****************************************************************************/
    void setInitStageProgress(quint8 stage, bool success);
    /****************************************************************************/
    /**
     * \brief Gets the Init stage progress count
     *
     * \return initStage Count
     */
    /****************************************************************************/
    quint8 getInitStageProgress();

    void setSimulationMode(bool simulationOn) { m_SimulationOn = simulationOn; }

    bool getSimulationMode() { return m_SimulationOn; }

    /****************************************************************************/
    /**
     * \brief  Slot called to set state machine to init failed state
     */
    /****************************************************************************/
    void SetInitializationFailed();


private:
    StateHandler();
    ~StateHandler();
    static StateHandler m_StateHandlerInstance; //!< Static instance
    Q_DISABLE_COPY(StateHandler ) //!< Disable Copy and assignment

    QStateMachine m_operationMachine; //!< Stores Operation Machines States
    QStateMachine m_availabilityMachine;//!< Stores Availability machine States.

    QState *mp_DefaultState; //!< Default state of StateMachine
    QState *mp_SoftSwitchMonitorState; //!< SoftSwitchMonitor state of StateMachine
    QState *mp_InitState; //!< Init state of StateMachine
    QState *mp_idleState; //!< Idle state of StateMachine
    QState *mp_UserActionState; //!< UserAction State of StateMachine
    QState *mp_PreBusyState; //!< Prebusy State of StateMachine
    QState *mp_busyState; //!< Busy State of StateMachine
    QState *mp_InitFailedState; //!< State indicating initialization failure

    QState *mp_normalState; //!< Normal State of StateMachine
    QState *mp_errorState; //!< ErrorState of StateMachine

    //QState *m_startupState;
    QList<quint32> m_errorList; //!< ErrorList
    QList<quint32> m_rackList; //!< RackList

    QMap<quint8, bool> m_initStage; //!< Map which stores the Initial Stages

    bool m_SimulationOn;
    bool m_swInitFailed;//!< true indicates software initialization failed
    QMutex m_Lock;  //!< Mutex for thread safety

private slots:
    /****************************************************************************/
    /**
     * \brief  Is called when Availability state is changed.
     */
    /****************************************************************************/
    void onAvailabilityStateChanged();
    /****************************************************************************/
    /**
     * \brief   Is called when Operation state is changed.
     */
    /****************************************************************************/
    void onOperationStateChanged();
    /****************************************************************************/
    /**
     * \brief   Is called when Init state is entered.
     */
    /****************************************************************************/
    void onInitStateEntered();
    /****************************************************************************/
    /**
     * \brief   Is called when SoftSwitchMonitor state is entered.
     */
    /****************************************************************************/
    void onSoftSwitchMonitorStateEntered();



//    /**
//      @brief Is called when error state is entered. Will inform GUI and scheduler about stopping the execution
//      */
//    void onErrorState();

//    /**
//      @brief Is called when normal state is entered, the system is error free. Will inform GUI and scheduler about resuming the execution
//      */
//    void onNormalState();


signals:
    /****************************************************************************/
    /**
     * \brief Signal to enter Error State
     */
    /****************************************************************************/
    void enterErrorState();
    /****************************************************************************/
    /**
     * \brief Signal to enter Normal State
     */
    /****************************************************************************/
    void enterNormalState();
    /****************************************************************************/
    /**
     * \brief signal to enter Idle State.
     */
    /****************************************************************************/
    void enterIdleState();
    /****************************************************************************/
    /**
     * \brief Signal to enter Busy State.
     */
    /****************************************************************************/
    void enterBusyState();
    /****************************************************************************/
    /**
     * \brief Signal to enter Init State.
     */
    /****************************************************************************/
    void enterInitState();
    /****************************************************************************/
    /**
     * \brief Signal sent on SoftSwitch pressed.
     */
    /****************************************************************************/
    void softSwitchPressed();
    /****************************************************************************/
    /**
     * \brief Signal sent on Init complete.
     */
    /****************************************************************************/
    void initComplete();
    /****************************************************************************/
    /**
     * \brief Signal to start SoftSwitchMonitor
     */
    /****************************************************************************/
    void softSwitchMonitorStart();
    /****************************************************************************/
    /**
     * \brief Signal sent on state changed.
     */
    /****************************************************************************/

    void stateChanged(QString);

    /****************************************************************************/
    /**
     * \brief Signal sent on entering Init State.
     */
    /****************************************************************************/
    void enteredInitState();
    /****************************************************************************/
    /**
     * \brief Signal sent on entering SoftSwitchMonitorState.
     *
     */
    /****************************************************************************/
    void enteredSoftSwitchMonitorState();

    /****************************************************************************/
    /**
     * \brief Signal sent on initialization failure
     *
     */
    /****************************************************************************/
    void initFailed();
}; // end class

} // end namespace EventHandler

#endif // EVENTHANDLER_STATEHANDLER_H

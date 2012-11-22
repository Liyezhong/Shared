/****************************************************************************/
/*! \file StateMachinesEventCodes.h
 *
 *  \brief All event codes used by state machines.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-12-08
 *  $Author:    $ J.Bugariu
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

#ifndef STATEMACHINES_STATEMACHINESEVENTCODES_H
#define STATEMACHINES_STATEMACHINESEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

/****************************************************************************/
/**
 * \brief Namespace containing all classes for state machines.
 */
/****************************************************************************/
namespace StateMachines {

const quint32 EVENT_STATEMACHINES_ERROR_TRANSITION_EXISTS               = EVENT_GROUP_PLATFORM_STATEMACHINES + 0x0001; ///< Transition between states '%1' and '%2' for index %3 already exists.
const quint32 EVENT_STATEMACHINES_ERROR_ADD_TRANSITION_FAILED           = EVENT_GROUP_PLATFORM_STATEMACHINES + 0x0002; ///< Error adding transition between states '%1' and '%2' for index %3.
const quint32 EVENT_STATEMACHINES_ERROR_REMOVE_TRANSITION_FAILED        = EVENT_GROUP_PLATFORM_STATEMACHINES + 0x0003; ///< Error removing transition between states '%1' and '%2' for index %3.
const quint32 EVENT_STATEMACHINES_ERROR_REMOVE_ALL_TRANSITIONS_FAILED   = EVENT_GROUP_PLATFORM_STATEMACHINES + 0x0004; ///< Error removing all state transitions between states '%1' and '%2'.
const quint32 EVENT_STATEMACHINES_ERROR_ADD_STATE_FAILED                = EVENT_GROUP_PLATFORM_STATEMACHINES + 0x0005; ///< Error adding state '%1'.
const quint32 EVENT_STATEMACHINES_ERROR_REMOVE_STATE_FAILED             = EVENT_GROUP_PLATFORM_STATEMACHINES + 0x0006; ///< Error removing state '%1'.
const quint32 EVENT_STATEMACHINES_ERROR_REMOVE_CURRENT_STATE            = EVENT_GROUP_PLATFORM_STATEMACHINES + 0x0007; ///< Can not remove state '%1'. It is the current state.
const quint32 EVENT_STATEMACHINES_ERROR_UNKNOWN_STATE                   = EVENT_GROUP_PLATFORM_STATEMACHINES + 0x0008; ///< Unknown state '%1'.

// string constants for logging

} // end namespace StateMachines

#endif // STATEMACHINES_STATEMACHINESEVENTCODES_H

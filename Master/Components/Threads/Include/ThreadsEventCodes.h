/****************************************************************************/
/*! \file Threads/Include/ThreadsEventCodes.h
 *
 *  \brief All event codes used by thread stuff.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
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

#ifndef THREADS_THREADSEVENTCODES_H
#define THREADS_THREADSEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

/****************************************************************************/
/**
 * \brief Namespace containing all classes thread stuff.
 */
/****************************************************************************/
namespace Threads {

const quint32 EVENT_THREADS_ERROR_MASTERTHREAD_STARTUP                      = EVENT_GROUP_PLATFORM_THREADS + 0x0001;    ///< Error starting up master thread.
const quint32 EVENT_THREADS_ERROR_THREAD_STOP_ARGS                          = EVENT_GROUP_PLATFORM_THREADS + 0x0002;    ///< Thread %1 did not stop within %2 milliseconds.
const quint32 EVENT_THREADS_ERROR_NO_HEARTBEAT                              = EVENT_GROUP_PLATFORM_THREADS + 0x0003;    ///< No heartbeat received from %1.
const quint32 EVENT_THREADS_INFO_NO_HEARTBEAT_CHECKING                      = EVENT_GROUP_PLATFORM_THREADS + 0x0004;    ///< Heartbeat checking for thread controllers disabled!
const quint32 EVENT_THREADS_ERROR_UNKNOWN_HEARTBEAT                         = EVENT_GROUP_PLATFORM_THREADS + 0x0005;    ///< Heartbeat received from unknown source %1.
const quint32 EVENT_THREADS_ERROR_SEND_COMMAND                              = EVENT_GROUP_PLATFORM_THREADS + 0x0006;    ///< Error sending command '%1' with reference %2.
const quint32 EVENT_THREADS_ERROR_SEND_ACKNOWLEDGE                          = EVENT_GROUP_PLATFORM_THREADS + 0x0007;    ///< Error sending acknowledge '%1' with reference %2.
const quint32 EVENT_THREADS_ERROR_UNKNOWN_COMMAND_REF                       = EVENT_GROUP_PLATFORM_THREADS + 0x0008;    ///< Unknown command reference: '%1'.
const quint32 EVENT_THREADS_ERROR_UNSUPPORTED_COMMAND                       = EVENT_GROUP_PLATFORM_THREADS + 0x0009;    ///< Unsupported command: '%1'.
const quint32 EVENT_THREADS_ERROR_UNSUPPORTED_ACKNOWLEDGE                   = EVENT_GROUP_PLATFORM_THREADS + 0x000A;    ///< Unsupported acknowledge: '%1'.
const quint32 EVENT_THREADS_ERROR_COMMAND_FUNCTOR_ALREADY_REGISTERED        = EVENT_GROUP_PLATFORM_THREADS + 0x000B;    ///< Functor already registered for command: '%1'.
const quint32 EVENT_THREADS_ERROR_ACKNOWLEDGE_FUNCTOR_ALREADY_REGISTERED    = EVENT_GROUP_PLATFORM_THREADS + 0x000C;    ///< Functor already registered for acknowledge: '%1'.
const quint32 EVENT_THREADS_ERROR_TIMEOUT_FUNCTOR_ALREADY_REGISTERED        = EVENT_GROUP_PLATFORM_THREADS + 0x000D;    ///< Functor already registered for timeout: '%1'.
const quint32 EVENT_THREADS_ERROR_COMMAND_TIMEOUT                           = EVENT_GROUP_PLATFORM_THREADS + 0x000E;    ///< Command timeout for command %1 with reference %2.
const quint32 EVENT_THREADS_INFO_STARTING_STATE_MACHINE                     = EVENT_GROUP_PLATFORM_THREADS + 0x000F;    ///< Starting master state machine.
const quint32 EVENT_THREADS_ERROR_COMMAND_HAS_TIMEOUT                       = EVENT_GROUP_PLATFORM_THREADS + 0x0010;    ///< Error: Command %1 has a timeout.

} // end namespace Threads

#endif // THREADS_THREADSEVENTCODES_H

/****************************************************************************/
/*! \file Global/Include/GlobalEventCodes.h
 *
 *  \brief All event codes used by all.
 *
 *  Version:    0.1
 *  Date:       2010-07-12
 *  Author:     J.Bugariu
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

#ifndef GLOBAL_GLOBALEVENTCODES_H
#define GLOBAL_GLOBALEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

namespace Global {

const quint32 EVENT_GLOBAL_UNDEFINED                        = EVENT_GROUP_PLATFORM_GLOBAL + 0xFFFF; ///< undefined. Used for initialisations
const quint32 EVENT_GLOBAL_UNKNOWN_STRING_ID                = EVENT_GROUP_PLATFORM_GLOBAL + 0x0001; ///< Unknown string id: %1.
const quint32 EVENT_GLOBAL_ERROR_UNKNOWN_EXCEPTION          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0002; ///< Unknown exception caught: file: %1 line: %2.
const quint32 EVENT_GLOBAL_ERROR_MEMORY_ALLOCATION          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0003; ///< Memory allocation error: file: %1 line: %2.
const quint32 EVENT_GLOBAL_ERROR_NULL_POINTER               = EVENT_GROUP_PLATFORM_GLOBAL + 0x0004; ///< Pointer is NULL: %1 file: %2 line: %3.
const quint32 EVENT_GLOBAL_ERROR_NOT_NULL_POINTER           = EVENT_GROUP_PLATFORM_GLOBAL + 0x0005; ///< Pointer is not NULL: %1 file: %2 line: %3.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_NOT_CONNECTED       = EVENT_GROUP_PLATFORM_GLOBAL + 0x0006; ///< Signal not connected: %1.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_SLOT_CONNECT        = EVENT_GROUP_PLATFORM_GLOBAL + 0x0007; ///< Error connecting signal %1::%2 to slot %3::%4 in file: %5 line: %6.
const quint32 EVENT_GLOBAL_ERROR_SIGNAL_SIGNAL_CONNECT      = EVENT_GROUP_PLATFORM_GLOBAL + 0x0008; ///< Error connecting signal %1::%2 to signal %3::%4 in file: %5 line: %6.
const quint32 EVENT_GLOBAL_ERROR_INVOKEMETHOD               = EVENT_GROUP_PLATFORM_GLOBAL + 0x0009; ///< Call of invokeMethod for method '%1' in object '%2' failed.
const quint32 EVENT_GLOBAL_ERROR_READ_CONFIG_FILE           = EVENT_GROUP_PLATFORM_GLOBAL + 0x000A; ///< Error reading config file %1.
const quint32 EVENT_GLOBAL_ERROR_MONOTONIC_CLOCK            = EVENT_GROUP_PLATFORM_GLOBAL + 0x000B; ///< Error reading monotonic clock.
const quint32 EVENT_GLOBAL_ERROR_FALLBACK_LANGUAGE          = EVENT_GROUP_PLATFORM_GLOBAL + 0x000C; ///< Could not set fallback language to '%1'.
const quint32 EVENT_GLOBAL_ERROR_FILE_CREATE                = EVENT_GROUP_PLATFORM_GLOBAL + 0x000D; ///< Error creating file %1.
const quint32 EVENT_GLOBAL_ERROR_FILE_OPEN                  = EVENT_GROUP_PLATFORM_GLOBAL + 0x000E; ///< Error opening file %1.
const quint32 EVENT_GLOBAL_ERROR_FILE_REMOVE                = EVENT_GROUP_PLATFORM_GLOBAL + 0x000F; ///< Error deleting file %1.
const quint32 EVENT_GLOBAL_ERROR_FILE_WRITE                 = EVENT_GROUP_PLATFORM_GLOBAL + 0x0010; ///< Error writing data: File %1. %2 bytes of %3 written.
const quint32 EVENT_GLOBAL_ERROR_FILE_FLUSH                 = EVENT_GROUP_PLATFORM_GLOBAL + 0x0011; ///< Error flushing file %1.
const quint32 EVENT_GLOBAL_ERROR_FILE_RENAME                = EVENT_GROUP_PLATFORM_GLOBAL + 0x0012; ///< Error could not rename file %1 to %2.
const quint32 EVENT_GLOBAL_ERROR_FILE_SEEK                  = EVENT_GROUP_PLATFORM_GLOBAL + 0x0013; ///< Error seeking in file %1.
const quint32 EVENT_GLOBAL_ERROR_TIME_OFFSET_TOO_LARGE      = EVENT_GROUP_PLATFORM_GLOBAL + 0x0014; ///< Time offset is too large '%1' seconds. Allowed: '%2' seconds.
const quint32 EVENT_GLOBAL_ERROR_READING_SERIAL_NUMBER      = EVENT_GROUP_PLATFORM_GLOBAL + 0x0015; ///< Error reading serial number.
const quint32 EVENT_GLOBAL_ERROR_COMPUTING_NEW_REF          = EVENT_GROUP_PLATFORM_GLOBAL + 0x0016; ///< Unable to compute new command ref.
const quint32 EVENT_GLOBAL_DUMMY_ERROR                      = EVENT_GROUP_PLATFORM_GLOBAL + 0xFFFE; ///< Dummy error. Should be replaced by correct error. File: %1 line: %2.

// now some string IDs
const quint32 EVENT_GLOBAL_STRING_ID_DEBUG_MESSAGE          = EVENT_GROUP_PLATFORM_GLOBAL + 0x1000; ///< Debug message: '%1'
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_UNDEFINED      = EVENT_GROUP_PLATFORM_GLOBAL + 0x1001; ///< Undefined Event type
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_FATAL_ERROR    = EVENT_GROUP_PLATFORM_GLOBAL + 0x1002; ///< Fatal error
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_ERROR          = EVENT_GROUP_PLATFORM_GLOBAL + 0x1003; ///< Error
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_WARNING        = EVENT_GROUP_PLATFORM_GLOBAL + 0x1004; ///< Warning
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_INFO           = EVENT_GROUP_PLATFORM_GLOBAL + 0x1005; ///< Info
const quint32 EVENT_GLOBAL_STRING_ID_EVTTYPE_DEBUG          = EVENT_GROUP_PLATFORM_GLOBAL + 0x1006; ///< Debug
const quint32 EVENT_GLOBAL_STRING_ID_EVTSTAT_ON             = EVENT_GROUP_PLATFORM_GLOBAL + 0x1007; ///< On
const quint32 EVENT_GLOBAL_STRING_ID_EVTSTAT_OFF            = EVENT_GROUP_PLATFORM_GLOBAL + 0x1008; ///< Off
const quint32 EVENT_GLOBAL_STRING_TERMINATING               = EVENT_GROUP_PLATFORM_GLOBAL + 0x1009; ///< Terminating software.
const quint32 EVENT_GLOBAL_SET_DATE_TIME                    = EVENT_GROUP_PLATFORM_GLOBAL + 0x100A; ///< Setting date / time to '%1'.
const quint32 EVENT_GLOBAL_CURRENT_TIME_OFFSET              = EVENT_GROUP_PLATFORM_GLOBAL + 0x100B; ///< Current time offset is '%1' seconds.
const quint32 EVENT_GLOBAL_POWER_FAIL                       = EVENT_GROUP_PLATFORM_GLOBAL + 0x100C; ///< Power fail.


} // end namespace Global

#endif // GLOBAL_GLOBALEVENTCODES_H

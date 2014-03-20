/****************************************************************************/
/*! \file Include/EventHandlerEventCodes.h
 *
 *  \brief Definition of event groups for event codes.
 *
 *  Version:    0.1
 *  Date:       2012-10-23
 *  Author:     Michael Thiel
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

#ifndef EVENTHANDLEREVENTCODES_H
#define EVENTHANDLEREVENTCODES_H

#include <QtGlobal>

const quint32 EVENT_ERROR_STATE                     = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0001; ///< Event for Error State
const quint32 EVENT_NORMAL_STATE                    = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0002; ///< Event for Normal State
const quint32 EVENT_GUI_AVAILABLE                   = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0003; ///< Event for Gui available
const quint32 EVENT_EVENT_ID_MISSING                = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0004; ///< Event for Event Id missing
const quint32 EVENT_INIT_FAILED                     = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0005; ///< Event for Init failed
const quint32 EVENT_INIT_SUCCESS                    = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0006; ///< Event for Init Success
const quint32 EVENT_INIT_STAGE_SUCCESS              = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0007; ///< Init stage completed successfully
const quint32 EVENT_INIT_STAGE_FAILURE              = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0008; ///< Init stage failed
const quint32 EVENT_INIT_RESTRICTED_SUCCESS         = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x0009; ///< Event for Init Success with restrictions
const quint32 EVENT_INIT_RESTRICTED_STAGE_SUCCESS   = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x000A; ///< Init stage completed with restrictions
const quint32 EVENT_CONFIG_FILE                     = EVENT_GROUP_PLATFORM_EVENTHANDLER + 0x000B; ///< Init stage completed with restrictions

const quint32 STR_SCHEDULER_EVENT_SCENARIO_ACTION_RESULT = 50331764; ///< Event Code: %1, Scenario: %2, Action: %3, Action Result: %4.


#endif // EVENTHANDLEREVENTCODES_H

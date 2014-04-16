/****************************************************************************/
/*! \file GPIOManagerEventCodes.h
 *
 *  \brief Events codes for GPIOManager component
 *
 *   $Version: $ 0.1
 *   $Date:    $ 01.01.2013
 *   $Author:  $ N.Kamath
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
#ifndef GPIOMANAGEREVENTCODES_H
#define GPIOMANAGEREVENTCODES_H

#include <../Include/PlatformEventCodes.h>

namespace GPIOManager {

const quint32 EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN                = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0001; //!< Informs user about softswitch press
const quint32 EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN_WITH_WARNING   = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0002; //!< Informs user about softswitch press at Busy state
const quint32 EVENT_CRITICAL_ACTION_WARNING_MESG                = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0003; //!< Critical action warning mesg
const quint32 EVENT_SOFTSWITCH_PRESSED_DURING_INIT_STATE        = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0004; //!< Informs user about softswitch press at Initialization phase
const quint32 EVENT_SOFTSWITCH_PRESSED_DURING_USER_ACTION       = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0005; //!< Informs user about softswitch press during user action
const quint32 EVENT_POWER_FAIL_STAGE1                           = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0006; //!< Power fail stage 1
const quint32 EVENT_POWER_FAIL_STAGE2                           = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0007; //!< Power fail stage 2
const quint32 EVENT_POWER_FAIL_REVERT                           = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0008; //!< Power fail Revert
const quint32 EVENT_GPIO_ERROR                                  = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0009; //!< GPIO Error mesg
const quint32 EVENT_GPIO_INFO                                   = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x000A; //!< GPIO Info mesg
const quint32 EVENT_SHUTDOWN_IN_FEW_SECONDS                     = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x000B; //!< Informs user that software will shutdown in few seconds.
} //End of namespace GPIOManager

#endif // GPIOMANAGEREVENTCODES_H

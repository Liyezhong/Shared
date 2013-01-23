#ifndef GPIOMANAGEREVENTCODES_H
#define GPIOMANAGEREVENTCODES_H
#include <../Include/PlatformEventCodes.h>

const quint32 EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN                = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0001; //! Informs user about softswitch press
const quint32 EVENT_PRESS_SOFTSWITCH_TO_SHUTDOWN_WITH_WARNING   = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0002; //! Informs user about softswitch press at Busy state
const quint32 EVENT_CRITICAL_ACTION_WARNING_MESG                = EVENT_GROUP_PLATFORM_GPIO_MANAGER + 0x0003; //! Informs user about softswitch press at Busy state
#endif // GPIOMANAGEREVENTCODES_H

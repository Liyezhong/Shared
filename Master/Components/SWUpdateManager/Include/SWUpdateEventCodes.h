#ifndef SWUPDATEEVENTCODES_H
#define SWUPDATEEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

namespace SWUpdate {

const quint32 EVENT_SW_UPDATE_STARTER_FAILED         = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0001;//!< Software update could not started due to error.
const quint32 EVENT_SW_UPDATE_FAILED                 = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0002;//!< Software update could not started due to error.
const quint32 EVENT_SW_UPDATE_ROLLBACK_UPDATE_FAILED = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0003;//!< Software update failed.
const quint32 EVENT_SW_UPDATE_SUCCESS                = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0007;//!< Software update successful.
const quint32 EVENT_SW_UPDATE_NOT_PERFORMED          = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0008;//!< Software update exited since never version of Software is not available.
}//End of namespace SWUpdate
#endif // SWUPDATEEVENTCODES_H

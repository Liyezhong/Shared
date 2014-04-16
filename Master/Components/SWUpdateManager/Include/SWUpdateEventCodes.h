#ifndef SWUPDATEEVENTCODES_H
#define SWUPDATEEVENTCODES_H

#include <../Include/PlatformEventCodes.h>

namespace SWUpdate {

const quint32 EVENT_SW_UPDATE_FAILED_DUE_TO_TIMEOUT = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0001;///< Software update failed due to timeout.
const quint32 EVENT_SW_UPDATE_EXITED_WITH_ERROR     = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0002;///< Software update failed. Please contact service.
const quint32 EVENT_SW_UPDATE_FAILED                = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0003;///< Software update failed. Please contact service.
const quint32 EVENT_SW_UPDATE_CHECK_STARTED         = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0004;///< Software update started
const quint32 EVENT_SW_UPDATE_CHECK_SUCCESSFUL      = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0005;///< Software update check successful.
const quint32 EVENT_SW_UPDATE_ROLLBACK_FAILED       = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0006;///< Software update rollback failed
const quint32 EVENT_SW_UPDATE_SUCCESS               = EVENT_GROUP_PLATFORM_SW_UPDATE + 0x0007;///< Software update successful.

}//End of namespace SWUpdate
#endif // SWUPDATEEVENTCODES_H

/****************************************************************************/
/*! \file dbTemperature.c
 *
 *  \brief Debug string table defintion
 *
 *  $Version: $ 0.1
 *  $Date:    $ 25.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module defines text strings to be used during debug only. It
 *       supplies ID-to-string conversion tables to the debug module, to
 *       enable the display of user friendly names instead of numerical
 *       IDs on the debug console. The IDs to be converted are:
 *
 *       - CAN-ID names
 *       - Event messages (Errors, Warnings, Infos)
 *
 *       Each function module can define it's own conversion tables for
 *       it's local defined identifiers, but is also free not to define
 *       strings. In this case IDs are displayed in numerical format.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include "Global.h"
#include "Basemodule.h"
#include "bmStrings.h"
#include "fmTemperature.h"


#ifdef DEBUG    // Omit conversion tables in non-debug builds

//****************************************************************************/
//! CAN-ID to Text String Conversion Table
//****************************************************************************/

static const bmCanIdNames_t CanIdNames[] = {

    { MSG_SRV_STATISTICS,            "MSG_TEMP_SRV_STATISTICS"      },
    { MSG_SRV_CFG_STATISTICS,        "MSG_TEMP_SRV_CFG_STATISTICS"  },
    { MSG_SRV_REQ_RESET_DATA,        "MSG_TEMP_SRV_REQ_RESET_DATA"  },
    { MSG_SRV_ACK_RESET_DATA,        "MSG_TEMP_SRV_ACK_RESET_DATA"  },

    { MSG_PROC_CONFIGURE,            "MSG_TEMP_PROC_CONFIGURE"      },
    { MSG_PROC_REQ_DATA,             "MSG_TEMP_PROC_REQ_DATA"       },
    { MSG_PROC_DATA,                 "MSG_TEMP_PROC_DATA"           },

    { MSG_EVENT_INFO,                "MSG_TEMP_EVENT_INFO"          },
    { MSG_EVENT_WARNING,             "MSG_TEMP_EVENT_WARNING"       },
    { MSG_EVENT_ERROR,               "MSG_TEMP_EVENT_ERROR"         },
    { MSG_EVENT_FATAL,               "MSG_TEMP_EVENT_FATAL"         },

    { MSG_TEMP_SET_TEMP,             "MSG_TEMP_SET_TEMP"            },
    { MSG_TEMP_SET_FAN_WATCHDOG,     "MSG_TEMP_SET_FAN_WATCHDOG"    },
    { MSG_TEMP_SET_CURRENT_WATCHDOG, "MSG_TEMP_SET_CURRENT_WATCHDOG"},
    { MSG_TEMP_SET_PID_PARAMS,       "MSG_TEMP_SET_PID_PARAMS"      },
    { MSG_TEMP_SET_HEATER_TIME,      "MSG_TEMP_SET_HEATER_TIME"     },
    { MSG_TEMP_REQ_TEMP,             "MSG_TEMP_REQ_TEMP"            },
    { MSG_TEMP_REQ_PID_PARAMS,       "MSG_TEMP_REQ_PID_PARAMS"      },
    { MSG_TEMP_REQ_HEATER_TIME,      "MSG_TEMP_REQ_HEATER_TIME"     },
    { MSG_TEMP_REQ_SERVICE_SENSOR,   "MSG_TEMP_REQ_SERVICE_SENSOR"  },
    { MSG_TEMP_REQ_SERVICE_FAN,      "MSG_TEMP_REQ_SERVICE_FAN"     },
    { MSG_TEMP_REQ_HARDWARE,         "MSG_TEMP_REQ_HARDWARE"        },
    { MSG_TEMP_RESP_TEMP,            "MSG_TEMP_RESP_TEMP"           },
    { MSG_TEMP_RESP_PID_PARAMS,      "MSG_TEMP_RESP_PID_PARAMS"     },
    { MSG_TEMP_RESP_HEATER_TIME,     "MSG_TEMP_RESP_HEATER_TIME"    },
    { MSG_TEMP_RESP_SERVICE_SENSOR,  "MSG_TEMP_RESP_SERVICE_SENSOR" },
    { MSG_TEMP_RESP_SERVICE_FAN,     "MSG_TEMP_RESP_SERVICE_FAN"    },
    { MSG_TEMP_RESP_HARDWARE,        "MSG_TEMP_RESP_HARDWARE"       },
    { MSG_TEMP_NOTI_AUTO_TUNE,       "MSG_TEMP_NOTI_AUTO_TUNE"      },
    { MSG_TEMP_NOTI_IN_RANGE,        "MSG_TEMP_NOTI_IN_RANGE"       },
    { MSG_TEMP_NOTI_OUT_OF_RANGE,    "MSG_TEMP_NOTI_OUT_OF_RANGE"   },
    { MSG_TEMP_NOTI_SLOPE_DETECTED,  "MSG_TEMP_NOTI_SLOPE_DETECTED"   }
};


//****************************************************************************/
//! Event-ID to Text Conversion Table (Errors, Warnings, Infos)
//****************************************************************************/

static const ErrorString_t EventNames[] = {
    { E_TEMP_PID_NOT_CONFIGURED,     "E_TEMP_PID_NOT_CONFIGURED"    },
    { E_TEMP_MODULE_ACTIVE,          "E_TEMP_MODULE_ACTIVE"         },
    { E_TEMP_MODULE_INACTIVE,        "E_TEMP_MODULE_INACTIVE"       },
    { E_TEMP_FAN_OUT_OF_RANGE,       "E_TEMP_FAN_OUT_OF_RANGE"      },
    { E_TEMP_CURRENT_OUT_OF_RANGE,   "E_TEMP_CURRENT_OUT_OF_RANGE"  },
    { E_TEMP_SENSOR_OUT_OF_RANGE,    "E_TEMP_SENSOR_OUT_OF_RANGE"   },
    { E_TEMP_SENSORS_INCONSISTENT,   "E_TEMP_SENSORS_INCONSISTENT"  },
    { E_TEMP_SENSOR_NOT_SUPPORTED,   "E_TEMP_SENSOR_NOT_SUPPORTED"  },
    { E_TEMP_HEATER_NOT_SUPPORTED,   "E_TEMP_HEATER_NOT_SUPPORTED"  }
};

#endif   // end of DEBUG section


/*****************************************************************************/
/*!
 *  \brief   Register name strings to the debug module
 *
 *      This function registers CAN-ID and Event name strings to the debug
 *      module, by delivering ID-to-string conversion tables. These tables
 *      are used during debug to display IDs as user friendly text strings
 *      on the debug console. Each function module can register it's local
 *      conversion tables - but this is not a must. If no conversion table
 *      is registered, the IDs are displayed in numerical format.
 *
 *      This function can be called also in the release version, but is
 *      empty in this case (conditional compilation). The module has to
 *      register itself to the base module before calling this function.
 *
 *  \iparam  ModuleID = Module ID
 *
 *  \return  Nothing
 *
 ****************************************************************************/

void tempRegisterDebugNames (UInt16 ModuleID) {

    #ifdef DEBUG
    dbgRegisterCanIdNames(ModuleID, CanIdNames, ELEMENTS(CanIdNames));
    dbgRegisterEventNames(EventNames, ELEMENTS(EventNames));
    #endif
}

//****************************************************************************/

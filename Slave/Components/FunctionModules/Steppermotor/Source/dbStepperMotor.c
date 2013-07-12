/****************************************************************************/
/*! \file dbStepperMotor.c
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
#include "fmStepperMotor.h"
#include "defStepperMotor.h"


#ifdef DEBUG    // Omit conversion tables in non-debug builds

//****************************************************************************/
//! CAN-ID to Text String Conversion Table
//****************************************************************************/

static const bmCanIdNames_t CanIdNames[] = {

// CAN-IDs for service data (handled by base module)
    { MSG_SRV_STATISTICS,           "MSG_SMOT_SRV_STATISTICS"       },
    { MSG_SRV_CFG_STATISTICS,       "MSG_SMOT_SRV_CFG_STATISTICS"   },
    { MSG_SRV_REQ_RESET_DATA,       "MSG_SMOT_SRV_REQ_RESET_DATA"   },
    { MSG_SRV_ACK_RESET_DATA,       "MSG_SMOT_SRV_ACK_RESET_DATA"   },

// CAN-IDs for process data (handled by base module)
    { MSG_PROC_CONFIGURE,           "MSG_SMOT_PROC_CONFIGURE"       },
    { MSG_PROC_REQ_DATA,            "MSG_SMOT_PROC_REQ_DATA"        },
    { MSG_PROC_DATA,                "MSG_SMOT_PROC_DATA"            },

// CAN-IDs for event messages (handled by base module)
    { MSG_EVENT_INFO,               "MSG_SMOT_EVENT_INFO"           },
    { MSG_EVENT_WARNING,            "MSG_SMOT_EVENT_WARNING"        },
    { MSG_EVENT_ERROR,              "MSG_SMOT_EVENT_ERROR"          },
    { MSG_EVENT_FATAL,              "MSG_SMOT_EVENT_FATAL"          },

// CAN-IDs for stepper motor (handled by function module)
    { MSG_SMOT_CONFIG,              "MSG_SMOT_CONFIG"               },
    { MSG_SMOT_CONFIG_ACK,          "MSG_SMOT_CONFIG_ACK"           },
    { MSG_SMOT_SET_ENABLE,          "MSG_SMOT_SET_ENABLE"           },
    { MSG_SMOT_SET_ENABLE_ACK,      "MSG_SMOT_SET_ENABLE_ACK"       },
    { MSG_SMOT_REQ_REF_RUN,         "MSG_SMOT_REQ_REF_RUN"          },
    { MSG_SMOT_REQ_REF_RUN_ACK,     "MSG_SMOT_REQ_REF_RUN_ACK"      },
    { MSG_SMOT_REFERENCE_RUN_ACK,   "MSG_SMOT_REFERENCE_RUN_ACK"    },
    { MSG_SMOT_TARGET_POS,          "MSG_SMOT_TARGET_POS"           },
    { MSG_SMOT_TARGET_POS_ACK,      "MSG_SMOT_TARGET_POS_ACK"       },
    { MSG_SMOT_TARGET_SPEED,        "MSG_SMOT_TARGET_SPEED"         },
    { MSG_SMOT_TARGET_SPEED_ACK,    "MSG_SMOT_TARGET_SPEED_ACK"     },
    { MSG_SMOT_MOVEMENT_ACK,        "MSG_SMOT_MOVEMENT_ACK"         },
    { MSG_SMOT_ACT_POS_REQ,         "MSG_SMOT_ACT_POS_REQ"          },
    { MSG_SMOT_ACT_POS,             "MSG_SMOT_ACT_POS"              },
    { MSG_SMOT_ACT_SPEED_REQ,       "MSG_SMOT_ACT_SPEED_REQ"        },
    { MSG_SMOT_ACT_SPEED,           "MSG_SMOT_ACT_SPEED"            },
    { MSG_SMOT_OPTIME_REQ,          "MSG_SMOT_OPTIME_REQ"           },
    { MSG_SMOT_OPTIME,              "MSG_SMOT_OPTIME"               },
    { MSG_SMOT_REVCOUNT_REQ,        "MSG_SMOT_REVCOUNT_REQ"         },
    { MSG_SMOT_REVCOUNT,            "MSG_SMOT_REVCOUNT"             },
    { MSG_SMOT_DIRCOUNT_REQ,        "MSG_SMOT_DIRCOUNT_REQ"         },
    { MSG_SMOT_DIRCOUNT,            "MSG_SMOT_DIRCOUNT"             },
};


//****************************************************************************/
//! Event-ID to Text Conversion Table (Errors, Warnings, Infos)
//****************************************************************************/

static const ErrorString_t EventNames[] = {

    { E_SMOT_INVALID_STATE,                 "E_SMOT_INVALID_STATE"                  },
    { E_SMOT_CONFIG_INCOMPLETE,             "E_SMOT_CONFIG_INCOMPLETE"              },
    { E_SMOT_CONFIG_LIMITSWITCH,            "E_SMOT_CONFIG_LIMITSWITCH"             },
    { E_SMOT_CONFIG_INVALID_DRIVER,         "E_SMOT_CONFIG_INVALID_DRIVER"          },
    { E_SMOT_CONFIG_REFRUN,                 "E_SMOT_CONFIG_REFRUN"                  },
    { E_SMOT_CONFIG_PROFILES,               "E_SMOT_CONFIG_PROFILES"                },
    { E_SMOT_CONFIG_INVALID_PROFILE,        "E_SMOT_CONFIG_INVALID_PROFILE"         },
    { E_SMOT_CONFIG_INVALID_PROFILE_VMIN,   "E_SMOT_CONFIG_INVALID_PROFILE_VMIN"    },
    { E_SMOT_CONFIG_INVALID_PROFILE_VMAX,   "E_SMOT_CONFIG_INVALID_PROFILE_VMAX"    },
    { E_SMOT_REFRUN_TIMEOUT,                "E_SMOT_REFRUN_TIMEOUT"                 },
    { E_SMOT_REFRUN_MAXDISTANCE,            "E_SMOT_REFRUN_MAXDISTANCE"             },
    { E_SMOT_REFRUN_ERROR,                  "E_SMOT_REFRUN_ERROR"                   },
    { E_SMOT_TIMER_OVERLOAD,                "E_SMOT_TIMER_OVERLOAD"                 },
    { E_SMOT_INVALID_POSCODE,               "E_SMOT_INVALID_POSCODE"                },
    { E_SMOT_INVALID_POSCODE_POSITION,      "E_SMOT_INVALID_POSCODE_POSITION"       },
    { E_SMOT_INVALID_POSCODE_DIR,           "E_SMOT_INVALID_POSCODE_DIR"            },
    { E_SMOT_STOP_BY_POSCODE,               "E_SMOT_STOP_BY_POSCODE"                },
    { E_SMOT_STOP_BY_OFFLIMIT,              "E_SMOT_STOP_BY_OFFLIMIT"               },
    { E_SMOT_STOP_BY_STEPDIFF,              "E_SMOT_STOP_BY_STEPDIFF"               },
    { E_SMOT_STOP_BY_STEPLOSS,              "E_SMOT_STOP_BY_STEPLOSS"               },
    { E_SMOT_STOP_BY_SHUTDOWN,              "E_SMOT_STOP_BY_SHUTDOWN"               },
    { E_SMOT_STOP_BY_EMERGENCYSTOP,         "E_SMOT_STOP_BY_EMERGENCYSTOP"          },
    { E_SMOT_PROFILE_ENCODER_UNFIT,         "E_SMOT_PROFILE_ENCODER_UNFIT"          },
    { E_SMOT_MOTION_PROFILE_UNFIT,          "E_SMOT_MOTION_PROFILE_UNFIT"           },

    { W_SMOT_STEPLOSS,                      "W_SMOT_STEPLOSS"                       },
    { W_SMOT_STEPDIFF,                      "W_SMOT_STEPDIFF"                       },

    { I_SMOT_NEED_INIT,                     "I_SMOT_NEED_INIT"                      },
    { I_SMOT_ROTATION_STOPPED,              "I_SMOT_ROTATION_STOPPED"               },
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

void smRegisterDebugNames (UInt16 ModuleID) {

    dbgRegisterCanIdNames(ModuleID, CanIdNames, ELEMENTS(CanIdNames));
    dbgRegisterEventNames(EventNames, ELEMENTS(EventNames));
}

/****************************************************************************/

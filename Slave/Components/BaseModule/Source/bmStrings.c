/****************************************************************************/
/*! \file bmStrings.c
 *
 *  \brief Debug string table defintion
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
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
 *       it's local defined identifiers, but is also free to not define
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

#include"Global.h"
#include"halError.h"
#include"bmError.h"
#include"bmCan.h"
#include"bmStrings.h"

#ifdef DEBUG    // Omit conversion tables in non-debug versions


//****************************************************************************/
//! Event-ID to Text String Conversion Table (Errors, Warnings, Infos)
//****************************************************************************/

static const ErrorString_t bmEventNames[] = {

    // General errors
    { E_PARAMETER_OUT_OF_RANGE,    "E_PARAMETER_OUT_OF_RANGE"     },
    { E_MEMORY_FULL,               "E_MEMORY_FULL"                },
    { E_TIMEOUT,                   "E_TIMEOUT"                    },
    { E_FUNCTION_UNSUPPORTED,      "E_FUNCTION_UNSUPPORTED"       },            
    
    // Base module related errors
    { E_ERROR_MESSAGE_LOST,        "E_ERROR_MESSAGE_LOST"         },
    { E_MODULE_TABLE_FULL,         "E_MODULE_TABLE_FULL"          },
    { E_MODULE_ALREADY_EXISTS,     "E_MODULE_ALREADY_EXISTS"      },
    { E_TASK_TABLE_FULL,           "E_TASK_TABLE_FULL"            },
    { E_TASK_NOT_EXISTS,           "E_TASK_NOT_EXISTS"            },
    { E_REALTIME_NOT_SET,          "E_REALTIME_NOT_SET"           },
    { E_BAD_TIME_FORMAT,           "E_BAD_TIME_FORMAT"            },
    { E_BAD_DATE_FORMAT,           "E_BAD_DATE_FORMAT"            },
    { E_PROCESS_DATA_UNSUPPORTED,  "E_PROCESS_DATA_UNSUPPORTED"   },
    { E_PROCESS_DATA_DISABLED,     "E_PROCESS_DATA_DISABLED"      },
    { E_STATS_NOT_AVAILABLE,       "E_STATS_NOT_AVAILABLE"        },
    { E_MODULE_INITIALIZATION,     "E_MODULE_INITIALIZATION"      },
    { E_PERSISTENTS_INVALID,       "E_PERSISTENTS_INVALID"        },

    // System command related errors
    { E_PROTECTED_COMMAND,         "E_PROTECTED_COMMAND"          },
    { E_FIRMWARE_UPDATE_FAILED,    "E_FIRMWARE_UPDATE_FAILED"     },
    { E_HARDWARE_RESET_FAILED,     "E_HARDWARE_RESET_FAILED"      },
    { E_RESET_PATTERN_SEQUENCE,    "E_RESET_PATTERN_SEQUENCE"     },
    { E_RESET_PATTERN_TIMEOUT,     "E_RESET_PATTERN_TIMEOUT"      },    
    { E_ILLEGAL_MODE_CHANGE,       "E_ILLEGAL_MODE_CHANGE"        },
    { E_NODE_BUSY,                 "E_NODE_BUSY"                  },

    // Service command related errors
    { E_BOOTLOADER_MISSING,        "E_BOOTLOADER_MISSING"         },
    { E_BOARD_OPTION_MISSING,      "E_BOARD_OPTION_MISSING"       },
    { E_INFOBLOCK_MISSING,         "E_INFOBLOCK_MISSING"          },
    { E_SERIAL_NUMBER_MISSING,     "E_SERIAL_NUMBER_MISSING"      },
    { E_LIFE_TIME_UPDATE,          "E_LIFE_TIME_UPDATE"           },

    // Storage partition errors
    { E_INVALID_PARTITION_ID,      "E_INVALID_PARTITION_ID"       },
    { E_ADDRESS_OUT_OF_RANGE,      "E_ADDRESS_OUT_OF_RANGE"       },
    { E_SIZE_OUT_OF_RANGE,         "E_SIZE_OUT_OF_RANGE"          },
    { E_CHECKSUM_ERROR,            "E_CHECKSUM_ERROR"             },
    { E_CHECKSUM_WRITE_ERROR,      "E_CHECKSUM_WRITE_ERROR"       },
    { E_STORAGE_UNFORMATTED,       "E_STORAGE_UNFORMATTED"        },
    { E_STORAGE_TOO_SMALL,         "E_STORAGE_TOO_SMALL"          },
    { E_STORAGE_READ_ERROR,        "E_STORAGE_READ_ERROR"         },
    { E_STORAGE_WRITE_ERROR,       "E_STORAGE_WRITE_ERROR"        },
    { E_STORAGE_OPEN_ERROR,        "E_STORAGE_OPEN_ERROR"         },
    { E_STORAGE_CLOSE_ERROR,       "E_STORAGE_CLOSE_ERROR"        },
    { E_STORAGE_PROTECTED,         "E_STORAGE_PROTECTED"          },
    { E_PARTITION_EXISTS,          "E_PARTITION_EXISTS"           },
    { E_PARTITION_NOT_EXISTS,      "E_PARTITION_NOT_EXISTS"       },
    { E_PARTITION_ALREADY_OPEN,    "E_PARTITION_ALREADY_OPEN"     },
    { E_PARTITION_NOT_OPENED,      "E_PARTITION_NOT_OPENED"       },
    { E_PARTITION_ACCESS_MODE,     "E_PARTITION_ACCESS_MODE"      },
    { E_PARTITION_TABLE_UPDATE,    "E_PARTITION_TABLE_UPDATE"     },
    { E_PARTITION_TABLE_FULL,      "E_PARTITION_TABLE_FULL"       },
    { E_PARTITION_TABLE_CORRUPT,   "E_PARTITION_TABLE_CORRUPT"    },
    { E_PARTITION_IS_OPEN,         "E_PARTITION_IS_OPEN"          },
    { E_PARTITION_RESET_FAILED,    "E_PARTITION_RESET_FAILED"     },
    { E_PARTITION_HANDLE_INVALID,  "E_PARTITION_HANDLE_INVALID"   },

    // CAN related errors
    { E_UNKNOWN_MESSAGE,           "E_UNKNOWN_MESSAGE"            },
    { E_ILLEGAL_NODE_INDEX,        "E_ILLEGAL_NODE_INDEX"         },
    { E_ILLEGAL_NODE_TYPE,         "E_ILLEGAL_NODE_TYPE"          },
    { E_MISSING_PARAMETERS,        "E_MISSING_PARAMETERS"         },
    { E_RECEIVE_MESSAGE_LOST,      "E_RECEIVE_MESSAGE_LOST"       },
    { E_TRANSMIT_MESSAGE_LOST,     "E_TRANSMIT_MESSAGE_LOST"      },
    { E_COMMAND_REJECTED,          "E_COMMAND_REJECTED"           },
    { E_QUEUE_ALREADY_CREATED,     "E_QUEUE_ALREADY_CREATED"      },
    { E_QUEUE_NOT_ALLOCATED,       "E_QUEUE_NOT_ALLOCATED"        },
    { E_QUEUE_FULL,                "E_QUEUE_FULL"                 },

    // Power monitor related errors
    { E_VOLTAGE_MONITOR_OFF,       "E_VOLTAGE_MONITOR_OFF"        },
    { E_CURRENT_MONITOR_OFF,       "E_CURRENT_MONITOR_OFF"        },
    { E_VOLTAGE_MONITOR_PORT,      "E_VOLTAGE_MONITOR_PORT"       },
    { E_CURRENT_MONITOR_PORT,      "E_CURRENT_MONITOR_PORT"       },
    { E_VOLTAGE_MONITOR_FAILURE,   "E_VOLTAGE_MONITOR_FAILURE"    },
    { E_CURRENT_MONITOR_FAILURE,   "E_CURRENT_MONITOR_FAILURE"    },
    { E_SUPPLY_VOLTAGE_LOW,        "E_SUPPLY_VOLTAGE_LOW"         },
    { E_SUPPLY_CURRENT_HIGH,       "E_SUPPLY_CURRENT_HIGH"        },

    // Common function module errors
    { E_MODULE_NOT_CONFIGURED,     "E_MODULE_NOT_CONFIGURED"      },
    { E_MODULE_NOT_ENABLED,        "E_MODULE_NOT_ENABLED"         },
    { E_MODULE_NOT_USEABLE,        "E_MODULE_NOT_USEABLE"         },

    // Base module info messages
    { I_INITIALIZATION_START,      "I_INITIALIZATION_START"       },
    { I_INITIALIZATION_DONE,       "I_INITIALIZATION_DONE"        },
    { I_PARTITION_CONVERTED,       "I_PARTITION_CONVERTED"        },
    { I_PARTITION_RESET,           "I_PARTITION_RESET"            }
};


//****************************************************************************/
//! CAN-ID to Text String Conversion Table
//****************************************************************************/

static const bmCanIdNames_t bmCanIdNames[] = {

    { MSG_SYS_HARDWARE_ID,         "MSG_SYS_HARDWARE_ID"          },
    { MSG_SYS_ACK_HARDWARE_ID,     "MSG_SYS_ACK_HARDWARE_ID"      },
    { MSG_SYS_CONFIGURATION,       "MSG_SYS_CONFIGURATION"        },
    { MSG_SYS_REQ_CONFIGURATION,   "MSG_SYS_REQ_CONFIGURATION"    },
    { MSG_SYS_SET_REALTIME,        "MSG_SYS_SET_REALTIME"         },
    { MSG_SYS_SET_SYSCLOCK,        "MSG_SYS_SET_SYSCLOCK"         },
    { MSG_SYS_NODESTATE,           "MSG_SYS_NODESTATE"            },
    { MSG_SYS_REQ_NODESTATE,       "MSG_SYS_REQ_NODESTATE"        },
    { MSG_SYS_SET_NODESTATE,       "MSG_SYS_SET_NODESTATE"        },
    { MSG_SYS_MASTER_HEARTBEAT,    "MSG_SYS_MASTER_HEARTBEAT"     },
    { MSG_SYS_SLAVE_HEARTBEAT,     "MSG_SYS_SLAVE_HEARTBEAT"      },
    { MSG_SYS_CFG_HEARTBEAT,       "MSG_SYS_CFG_HEARTBEAT"        },
    { MSG_SYS_NOTSTOP,             "MSG_SYS_NOTSTOP"              },
    { MSG_SYS_ABORTED_BY_NOTSTOP,  "MSG_SYS_ABORTED_BY_NOTSTOP"   },
    { MSG_SYS_MASTER_POWER,        "MSG_SYS_MASTER_POWER"         },
    { MSG_SYS_RESET,               "MSG_SYS_RESET"                },
    { MSG_SYS_ACK_RESET,           "MSG_SYS_ACK_RESET"            },
    
    { MSG_EVENT_INFO,              "MSG_EVENT_INFO"               },
    { MSG_EVENT_WARNING,           "MSG_EVENT_WARNING"            },
    { MSG_EVENT_ERROR,             "MSG_EVENT_ERROR"              },
    { MSG_EVENT_FATAL,             "MSG_EVENT_FATAL"              },

    { MSG_SRV_CFG_STATISTICS,      "MSG_SRV_CFG_STATISTICS"       },
    { MSG_SRV_STATISTICS,          "MSG_SRV_STATISTICS"           },
    { MSG_SRV_REQ_RESET_DATA,      "MSG_SRV_REQ_RESET_DATA"       },
    { MSG_SRV_ACK_RESET_DATA,      "MSG_SRV_ACK_RESET_DATA"       },
    { MSG_SRV_REQ_MEMORY_FORMAT,   "MSG_SRV_REQ_MEMORY_FORMAT"    },
    { MSG_SRV_ACK_MEMORY_FORMAT,   "MSG_SRV_ACK_MEMORY_FORMAT"    },
    { MSG_SRV_REQ_SERIAL_NUMBER,   "MSG_SRV_REQ_SERIAL_NUMBER"    },
    { MSG_SRV_SERIAL_NUMBER,       "MSG_SRV_SERIAL_NUMBER"        },
    { MSG_SRV_REQ_ENDTEST_RESULT,  "MSG_SRV_REQ_ENDTEST_RESULT"   },
    { MSG_SRV_ENDTEST_RESULT,      "MSG_SRV_ENDTEST_RESULT"       },
    { MSG_SRV_REQ_HARDWARE_INFO,   "MSG_SRV_REQ_HARDWARE_INFO"    },
    { MSG_SRV_HARDWARE_INFO,       "MSG_SRV_HARDWARE_INFO"        },
    { MSG_SRV_REQ_SOFTWARE_INFO,   "MSG_SRV_REQ_SOFTWARE_INFO"    },
    { MSG_SRV_SOFTWARE_INFO,       "MSG_SRV_SOFTWARE_INFO"        },
    { MSG_SRV_REQ_LOADER_INFO,     "MSG_SRV_REQ_LOADER_INFO"      },
    { MSG_SRV_LOADER_INFO,         "MSG_SRV_LOADER_INFO"          },
    { MSG_SRV_REQ_LIFECYCLE_DATA,  "MSG_SRV_REQ_LIFECYCLE_DATA"   },
    { MSG_SRV_LIFECYCLE_DATA,      "MSG_SRV_LIFECYCLE_DATA"       },
    { MSG_SRV_REQ_LAUNCH_DATE,     "MSG_SRV_REQ_LAUNCH_DATE"      },
    { MSG_SRV_LAUNCH_DATE,         "MSG_SRV_LAUNCH_DATE"          },
    { MSG_SRV_REQ_BOARD_NAME,      "MSG_SRV_REQ_BOARD_NAME"       },
    { MSG_SRV_BOARD_NAME,          "MSG_SRV_BOARD_NAME"           },
    { MSG_CFG_VOLTAGE_MONITOR,     "MSG_CFG_VOLTAGE_MONITOR"      },
    { MSG_SRV_REQ_VOLTAGE_STATE,   "MSG_SRV_REQ_VOLTAGE_STATE"    },
    { MSG_SRV_VOLTAGE_STATE,       "MSG_SRV_VOLTAGE_STATE"        },
    { MSG_CFG_CURRENT_MONITOR,     "MSG_CFG_CURRENT_MONITOR"      },
    { MSG_SRV_REQ_CURRENT_STATE,   "MSG_SRV_REQ_CURRENT_STATE"    },
    { MSG_SRV_CURRENT_STATE,       "MSG_SRV_CURRENT_STATE"        },
    { MSG_SRV_REQ_BOARD_OPTIONS,   "MSG_SRV_REQ_BOARD_OPTIONS"    },
    { MSG_SRV_BOARD_OPTIONS,       "MSG_SRV_BOARD_OPTIONS"        },
    { MSG_SRV_REQ_UNIQUE_NUMBER,   "MSG_SRV_REQ_UNIQUE_NUMBER"    },
    { MSG_SRV_UNIQUE_NUMBER,       "MSG_SRV_UNIQUE_NUMBER"        },
    { MSG_SRV_REQ_REALTIME,        "MSG_SRV_REQ_REALTIME"         },
    { MSG_SRV_REALTIME,            "MSG_SRV_REALTIME"             },

    { MSG_PROC_CONFIGURE,          "MSG_PROC_CONFIGURE"           },
    { MSG_PROC_REQ_DATA,           "MSG_PROC_REQ_DATA"            },
    { MSG_PROC_DATA,               "MSG_PROC_DATA"                }
};


/*****************************************************************************/
/*!
 *  \brief   Get error description table
 *
 *      Returns the error string table pointer in ErrorStrings. The size
 *      of the table is returned as return parameter.
 *
 *      This function is available only in debug mode
 *
 *  \oparam  ErrorStrings = Error string table pointer
 *
 *  \return  Size of error string table
 *
 ****************************************************************************/

UInt32 bmGetErrorStrings (const ErrorString_t *ErrorStrings[]) {

    if (ErrorStrings != NULL) {
        *ErrorStrings = bmEventNames;
    }
    return (ELEMENTS(bmEventNames));
}


/*****************************************************************************/
/*!
 *  \brief   Get error description table
 *
 *      Returns the error string table pointer in ErrorStrings. The size
 *      of the table is returned as return parameter.
 *
 *      This function is available only in debug mode
 *
 *  \oparam  CanIdStrings = CAN-ID string table pointer
 *
 *  \return  Size of error string table
 *
 ****************************************************************************/

UInt32 bmGetCanIdStrings (const bmCanIdNames_t *CanIdStrings[]) {

    if (CanIdStrings != NULL) {
        *CanIdStrings = bmCanIdNames;
    }
    return (ELEMENTS(bmCanIdNames));
}

#endif // DEBUG

//****************************************************************************/

/****************************************************************************/
/*! \file dbJoystick.c
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
#include "fmJoystick.h"


#ifdef DEBUG   // Omit conversion tables in non-debug builds

//****************************************************************************/
//! CAN-ID to Text String Conversion Table
//****************************************************************************/

static const bmCanIdNames_t CanIdNames[] = {

    { MSG_SRV_STATISTICS,         "MSG_JOYS_SRV_STATISTICS"     },
    { MSG_SRV_CFG_STATISTICS,     "MSG_JOYS_SRV_CFG_STATISTICS" },
    { MSG_SRV_REQ_RESET_DATA,     "MSG_JOYS_SRV_REQ_RESET_DATA" },
    { MSG_SRV_ACK_RESET_DATA,     "MSG_JOYS_SRV_ACK_RESET_DATA" },

    { MSG_PROC_CONFIGURE,         "MSG_JOYS_PROC_CONFIGURE"     },
    { MSG_PROC_REQ_DATA,          "MSG_JOYS_PROC_REQ_DATA"      },
    { MSG_PROC_DATA,              "MSG_JOYS_PROC_DATA"          },

    { MSG_EVENT_INFO,             "MSG_JOYS_EVENT_INFO"         },
    { MSG_EVENT_WARNING,          "MSG_JOYS_EVENT_WARNING"      },
    { MSG_EVENT_ERROR,            "MSG_JOYS_EVENT_ERROR"        },
    { MSG_EVENT_FATAL,            "MSG_JOYS_EVENT_FATAL"        },

    { MSG_JOYS_SET_CONFIGURATION, "MSG_JOYS_SET_CONFIGURATION"  },
    { MSG_JOYS_SET_MECH_OFFSET,   "MSG_JOYS_SET_MECH_OFFSET"    },
    { MSG_JOYS_REQ_DISPLACEMENT,  "MSG_JOYS_REQ_DISPLACEMENT"   },
    { MSG_JOYS_REQ_MECH_OFFSET,   "MSG_JOYS_REQ_MECH_OFFSET"    },
    { MSG_JOYS_RESP_DISPLACEMENT, "MSG_JOYS_RESP_DISPLACEMENT"  },
    { MSG_JOYS_RESP_MECH_OFFSET,  "MSG_JOYS_RESP_MECH_OFFSET"   }
};


//****************************************************************************/
//! Event-ID to Text Conversion Table (Errors, Warnings, Infos)
//****************************************************************************/

static const ErrorString_t EventNames[] = {
    { E_JOYS_MODULE_DISABLED,     "E_JOYS_MODULE_DISABLED"  }
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

void joysRegisterDebugNames (UInt16 ModuleID) {

    #ifdef DEBUG
    dbgRegisterCanIdNames(ModuleID, CanIdNames, ELEMENTS(CanIdNames));
    dbgRegisterEventNames(EventNames, ELEMENTS(EventNames));
    #endif
}

//****************************************************************************/

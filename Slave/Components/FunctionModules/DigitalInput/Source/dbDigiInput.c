/****************************************************************************/
/*! \file dbDigiInput.c
 *
 *  \brief Debug String Defintion for Digital Input Function Module
 *
 *  $Version: $ 0.3
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
#include "fmDigiInput.h"


#ifdef DEBUG   // Omit conversion tables in non-debug versions

//****************************************************************************/
//! CAN-ID to Text String Conversion Table
//****************************************************************************/

static const bmCanIdNames_t CanIdNames[] = {

    { MSG_DI_SRV_STATISTICS,     "MSG_DI_SRV_STATISTICS"     },
    { MSG_DI_SRV_CFG_STATISTICS, "MSG_DI_SRV_CFG_STATISTICS" },
    { MSG_DI_SRV_REQ_RESET_DATA, "MSG_DI_SRV_REQ_RESET_DATA" },
    { MSG_DI_SRV_ACK_RESET_DATA, "MSG_DI_SRV_ACK_RESET_DATA" },

    { MSG_DI_PROC_CONFIGURE,     "MSG_DI_PROC_CONFIGURE"     },
    { MSG_DI_PROC_REQ_DATA,      "MSG_DI_PROC_REQ_DATA"      },
    { MSG_DI_PROC_DATA,          "MSG_DI_PROC_DATA"          },

    { MSG_DI_EVENT_INFO,         "MSG_DI_EVENT_INFO"         },
    { MSG_DI_EVENT_WARNING,      "MSG_DI_EVENT_WARNING"      },
    { MSG_DI_EVENT_ERROR,        "MSG_DI_EVENT_ERROR"        },
    { MSG_DI_EVENT_FATAL,        "MSG_DI_EVENT_FATAL"        },

    { MSG_DI_INPUT_STATE,        "MSG_DI_INPUT_STATE"        },
    { MSG_DI_REQ_INPUT_STATE,    "MSG_DI_REQ_INPUT_STATE"    },
    { MSG_DI_CONFIG_INPUT,       "MSG_DI_CONFIG_INPUT"       },
    { MSG_DI_CONFIG_LIMITS,      "MSG_DI_CONFIG_LIMITS"      }
};

//****************************************************************************/
//! Event-ID to Text String Conversion Table (Errors, Warnings, Infos)
//****************************************************************************/

static const ErrorString_t EventNames[] = {
    { W_DIGIN_ABOVE_LIMIT,       "W_DIGIN_ABOVE_LIMIT"       },
    { W_DIGIN_BELOW_LIMIT,       "W_DIGIN_BELOW_LIMIT"       }
};


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

void diRegisterDebugNames (UInt16 ModuleID) {

    dbgRegisterCanIdNames(ModuleID, CanIdNames, ELEMENTS(CanIdNames));
    dbgRegisterEventNames(EventNames, ELEMENTS(EventNames));
}

//****************************************************************************/

#endif   // end of DEBUG section

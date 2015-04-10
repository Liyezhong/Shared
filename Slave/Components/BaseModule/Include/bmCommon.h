/********************************************************************************/
/*! \file bmCommon.h
 *
 *  \brief Common Base Module Definitions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This header contains common base module definitions.
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 */
/*********************************************************************************/

#ifndef BM_COMMON_H
#define BM_COMMON_H


//********************************************************************************/
// Public Constants and Definitions
//********************************************************************************/

#define INFOBLOCK_SIGNATURE      0x91827364 //!< Signature of info block

/* basemodule's software version informations */
#ifdef FCT_ASB_15
#define SOFTWARE_VERSION         0x000B     //!< Software version number
#define SOFTWARE_CREATION_YEAR   2015       //!< Software creation date (year)
#define SOFTWARE_CREATION_MONTH  4         //!< Software creation date (month)
#define SOFTWARE_CREATION_DAY    10         //!< Software creation date (day)
#else
#define SOFTWARE_VERSION         0x000A     //!< Software version number
#define SOFTWARE_CREATION_YEAR   2014       //!< Software creation date (year)
#define SOFTWARE_CREATION_MONTH  12         //!< Software creation date (month)
#define SOFTWARE_CREATION_DAY    29         //!< Software creation date (day)
#endif
/* basemodule global identifiers */
#define BASEMODULE_CHANNEL       0          //!< Base module's channel ID
#define BASEMODULE_MODULE_ID     0          //!< Base module's module ID
#define BASEMODULE_TASK_ID       0          //!< Base module's taskID
#define BASEMODULE_VERSION       0x0002     //!< Base module's version number
#define BASEMODULE_PARTITION_ID  0          //!< Base module's partition-ID

/* basemodule pseudo instance numbers for bmGetBoardOptions() */
#define OPTIONS_BASEMODULE       0          //!< Board option bit field
#define OPTIONS_NODE_INDEX       1          //!< Node index (if no DIP present)
#define OPTIONS_VOLTAGE_LIMIT1   2          //!< Supply voltage warning limit
#define OPTIONS_VOLTAGE_LIMIT2   3          //!< Supply voltage failure limit
#define OPTIONS_CURRENT_LIMIT1   4          //!< Supply current warning limit
#define OPTIONS_CURRENT_LIMIT2   5          //!< Supply current failure limit

/* bitmasks for basemodule's board option word */
#define OPTION_NODE_INDEX_DIP    0x0001   //!< DIP switch present
#define OPTION_STATUS_LED        0x0002   //!< Status LED present

#define OPTION_PROCESS_DATA      0x0010   //!< Enable process data
#define OPTION_TRIGGER_EVENTS    0x0020   //!< Enable event handling
#define OPTION_LIFECYCLE_DATA    0x0040   //!< Enable life cycle counters
#define OPTION_IDENTIFICATION    0x0080   //!< Enable identification phase

#define OPTION_TASK_STATISTICS   0x0100   //!< Enable task statistics
#define OPTION_CURRENT_MONITOR   0x0200   //!< Enable current monitoring
#define OPTION_VOLTAGE_MONITOR   0x0400   //!< Enable voltage monitoring
#define OPTION_SAFE_MONITOR      0x0800   //!< Enable safe voltage monitoring
#define OPTION_INFO_MESSAGES     0x1000   //!< Enable info messages
#define OPTION_WAIT_POWER_UP     0x2000   //!< Enable wait-for-power-good


//********************************************************************************/
// Default Board Options for Base Module
//********************************************************************************/

//! Definition of standard board options
#define STANDARD_BOARD_OPTIONS  (OPTION_IDENTIFICATION  | \
                                 OPTION_NODE_INDEX_DIP  | \
                                 OPTION_STATUS_LED      | \
                                 OPTION_LIFECYCLE_DATA  | \
                                 OPTION_PROCESS_DATA    | \
                                 OPTION_TRIGGER_EVENTS  | \
                                 OPTION_TASK_STATISTICS | \
                                 OPTION_VOLTAGE_MONITOR | \
                                 OPTION_INFO_MESSAGES   | \
                                 OPTION_CURRENT_MONITOR)

//! Definition of board options for testing
#define TESTING_BOARD_OPTIONS   (OPTION_STATUS_LED      | \
                                 OPTION_TRIGGER_EVENTS  | \
                                 OPTION_PROCESS_DATA    | \
                                 OPTION_TASK_STATISTICS | \
                                 OPTION_VOLTAGE_MONITOR | \
                                 OPTION_LIFECYCLE_DATA)

//! Currently selected board option set
#define DEFAULT_BOARD_OPTIONS    STANDARD_BOARD_OPTIONS

#endif /*BM_COMMON_H*/

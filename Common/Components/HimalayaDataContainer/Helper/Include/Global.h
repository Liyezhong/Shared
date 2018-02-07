/****************************************************************************/
/*! \file Global.h
 *
 *  \brief Definition file for global variables and methods.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-25
 *  $Author:    $ R. Wu
 *
 *  \b Company:
 *
 *       Leica Biosystems R&D Center Shanghai.
 *
 *  (C) Copyright 2010 by LBS R&D Center Shanghai. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef DATAMANAGER_GLOBAL_H
#define DATAMANAGER_GLOBAL_H

#include <QString>

namespace DataManager {

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum ReagentStatusType_t
 */
/****************************************************************************/
 typedef enum  {
        PROGRAM_START = 0,
        PROGRAM_PAUSE,
        PROGRAM_ABORT,
        PROGRAM_DRAIN,
        PROGRAM_DRAIN_SR,
        PROGRAM_OVEN_COVER_OPEN,
        PROGRAM_RETORT_COVER_OPEN,
        PROGRAM_POWER_FAILURE_MSG,
        PROGRAM_UNDEFINED
    } ProgramActionType_t;
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum LockType_t
 */
/****************************************************************************/
typedef enum  {
    RETORT_LOCK = 0,
    PARAFFIN_BATH_LOCK,
    LOCKTYPE_UNDEFINED
   } LockType_t;
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum ProgramStatusType_t
 */
/****************************************************************************/
typedef enum  {
        PROGRAM_STATUS_COMPLETED = 0,
        PROGRAM_STATUS_ABORTED,
        PROGRAM_STATUS_PAUSED
   } ProgramStatusType_t;

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum ProgramStatusType_t
 */
/****************************************************************************/
typedef enum  {
        COMPLETED_PROGRAM_GENERAL = 0,
        COMPLETED_PROGRAM_SAFE_REAGENT,
        COMPLETED_PROGRAM_POWER_FAILURE
   } CompletedProgramType_t;

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum ProgramAcknownedgeType_t
 */
/****************************************************************************/
typedef enum  {
        PROGRAM_READY = 0,
        PROGRAM_WILL_COMPLETE,
        PROGRAM_CLEANING_FINISHED,

        PROGRAM_RUN_BEGIN,
        PROGRAM_RUN_FINISHED,
        PROGRAM_RUN_FINISHED_NO_CONTAMINATED,
        PROGRAM_RUN_FINISHED_AS_SAFE_REAGENT,
        PROGRAM_RUN_FINISHED_AS_SAFE_REAGENT_NO_CONTAMINATED,
        PROGRAM_RUN_FINISHED_AS_POWER_FAILURE,
        PROGRAM_RUN_FINISHED_AS_POWER_FAILURE_NO_CONTAMINATED,
        CLEANING_PROGRAM_COMPLETE_AS_SAFE_REAGENT,

        PROGRAM_PAUSE_BEGIN,
        PROGRAM_PAUSE_FINISHED,

        PROGRAM_PAUSE_ENABLE,
        PROGRAM_PAUSE_DISABLE,
        PROGRAM_PAUSE_TIMEOUT_15MINTUES,

        PROGRAM_START_ENABLE,
        PROGRAM_START_DISABLE,

        PROGRAM_SYSTEM_EEEOR,
        PROGRAM_SYSTEM_RC_RESTART,

        PROGRAM_FILL_BEGIN,
        PROGRAM_SOAK_BEGIN,
        PROGRAM_DRAIN_BEGIN,

        PROGRAM_ABORT_BEGIN,
        PROGRAM_ABORT_FINISHED,

        PROGRAM_SELFTEST_FAILED,
        PROGRAM_SELFTEST_PASSED,

        TISSUE_PROTECT_PASSED,
        TISSUE_PROTECT_PASSED_WARNING,
		OVEN_COVER_OPEN,
        RETORT_COVER_OPERN,
        TAKE_OUT_SPECIMEN_WAIT_RUN_CLEANING,

        PROGRAM_PRETEST_DONE,

        POWER_FAILURE_MSG,
        CANCEL_PROGRAM_WILL_COMPLETE_PROMPT,
        CANCEL_TISSUE_PROTECT_PASSED_PROMPT,
        CANCEL_RETORT_LID_OPEN_MSG_PROMPT,
        PAUSE_WAITING_FOR_FILLING,
        SHOW_PAUSE_MSG_DLG,
        DISMISS_PAUSING_MSG_DLG,
        WAIT_ROTARY_VALVE_HEATING_PROMPT,
        DISMISS_ROTARY_VALVE_HEATING_PROMPT,
        PROGRAM_COOLING_DWON,
        DISABLE_BOTTLE_CHECK,
        ENABLE_BOTTLE_CHECK,
        SHOW_RESUME_MSG_DLG,
        DISMISS_RESUME_MSG_DLG,
        PAUSE_HAS_BEGUN,
        SYSTEM_BUSY,
        SYSTEM_IDLE,
        PROGRAM_RESUME_AFTER_POWER_FAILURE,
        MAINTAINANCE_TIMECOUNT_START
    } ProgramAcknownedgeType_t;
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum RetortStatusType_t
 */
/****************************************************************************/
typedef enum  {
        RETORT_EMPTYCLEAN = 0,
        RETORT_SCUKING,
        RETORT_DRAINING,
        RETORT_FULL,
        RETORT_CONTAMINATED,
        RETORT_UNDEFINED
    } RetortStatusType_t;
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum ReagentStatusType_t
 */
/****************************************************************************/
typedef enum  {
        CONTAINER_STATUS_EMPTY = 0,
        CONTAINER_STATUS_SCUKING,
        CONTAINER_STATUS_DRAINING,
        CONTAINER_STATUS_FULL,
        CONTAINER_STATUS_EXPIRED,
        CONTAINER_STATUS_CONTAMINATED,
        CONTAINER_STATUS_NOTUSED
    } ContainerStatusType_t;
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum ReagentStatusType_t
 */
/****************************************************************************/
typedef enum {
    RESETOPERATIONHOURS_WHOLEMACHINEOPERATION = 0,
    RESETOPERATIONHOURS_ACTIVECARBONFILTER,
    RESETOPERATIONHOURS_UNDEFINED
} ResetOperationHoursType_t;
/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum ReagentStatusType_t
 */
/****************************************************************************/
typedef enum {
    QUITAPPSHUTDOWNACTIONTYPE_QUITAPP = 0,
    QUITAPPSHUTDOWNACTIONTYPE_PREPARESHUTDOWN,
    QUITAPPSHUTDOWNACTIONTYPE_UNDEFINED
} QuitAppShutdownActionType_t;

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum BottleCheckStatusType_t
 */
/****************************************************************************/
typedef enum {
  	BOTTLECHECK_STARTED = 0,
    BOTTLECHECK_WAITING,
    BOTTLECHECK_BUILDPRESSUREFAILED,
    BOTTLECHECK_EMPTY,
    BOTTLECHECK_PASSED,
    BOTTLECHECK_BLOCKAGE,
    BOTTLECHECK_CHECKING,
    BOTTLECHECK_WILLNOTPROCESS,
    BOTTLECHECK_FAILED,
    BOTTLECHECK_ALLCOMPLETE
} BottleCheckStatusType_t;

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of enum ReagentStatusType_t
 */
/****************************************************************************/
typedef enum  {
    REAGENT_STATUS_NOT_IN_STATION = 0, ///< Reagent is newly created, but has not been placed into a station.
    REAGENT_STATUS_NORMAL, ///< Reagent is in a station, and can be used normally.
    REAGENT_STATUS_EXPIRED, ///< Reagent is in a station, but expired.
    REAGENT_STATUS_UNDEFINE ///< Undefined.
} ReagentStatusType_t;

/****************************************************************************/
/*!
 *  \brief  Definition of enum MsgBoxType_t
 */
/****************************************************************************/
typedef enum {
    PAUSE_MSG, ///< pause message dialog
    REUSME_MSG ///< resume message dialog
} MsgBoxType_t;

/****************************************************************************/
/*!
 *  \brief  Definition/Declaration of function LEICA
 *
 *  \return from LEICA
 */
/****************************************************************************/
const QString LEICA("leica");

/****************************************************************************/
/*!
 *  \brief  Check if the system is now idle,
 *          which means no processing is running.
 *  \return idle flag
 */
/****************************************************************************/
bool IsSystemIdle(void);

/****************************************************************************/
/*!
 *  \brief  Set the system idle status.
 */
/****************************************************************************/
void SetSystemIdle(bool idle);



}

#endif // DATAMANAGER_GLOBAL_H

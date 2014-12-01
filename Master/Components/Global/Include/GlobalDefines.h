/****************************************************************************/
/*! \file Global/Include/GlobalDefines.h
 *
 *  \brief Some global definitions.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2010-07-12
 *  $Author:    $ J.Bugariu
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#ifndef GLOBAL_GLOBALDEFINES_H
#define GLOBAL_GLOBALDEFINES_H

#include <QtGlobal>
#include <QString>
#include <QHash>
#include <Global/Include/TranslatableString.h>

/****************************************************************************/
/**
 * \brief In this namespace live globally used classes and defines.
 */
/****************************************************************************/
namespace Global {

typedef quint16 gSourceType;            //!< typedef for HeartBeat source: source type.
typedef quint16 gSubComponentType;      //!< typedef for logging source: subcomponent type.
typedef quint64 tRefType;               //!< typedef for tRefType.

const QString NO_NUMERIC_DATA = "";     //!< Self explaining

/****************************************************************************/
/**
 * \brief Enum containing all known return codes from the application.
 */
/****************************************************************************/
enum {
    RETCODE_OK                  = 0,    //!< Everything OK.
    RETCODE_TIME_OFFSET         = 1,    //!< Reading or writing to time offset file failed.
    RETCODE_CONNECT_FAILED      = 2,    //!< A connect between a signal and a slot failed.
    RETCODE_MASTERTHREAD_INIT   = 3,    //!< Master thread initialization failed.
    RETCODE_MASTERTHREAD_WAIT   = 4     //!< Waiting for master thread failed.
};

/****************************************************************************/
/**
 * \brief Enum containing all known event Types.
 */
/****************************************************************************/
enum EventType {
    EVTTYPE_UNDEFINED,      ///< Undefined. Used for initialisation.
    EVTTYPE_DEBUG,           ///< Debug message. Should be used only for debugging purposes.
    EVTTYPE_INFO,           ///< Info.
    EVTTYPE_WARNING,        ///< Warning.
    EVTTYPE_ERROR,          ///< "Normal" error.
    EVTTYPE_FATAL_ERROR    ///< Fatal error.
};

/****************************************************************************/
/**
 * \brief Enum for event logging priority
 */
/****************************************************************************/
enum EventLogLevel {
    LOGLEVEL_NONE,      ///< none
    LOGLEVEL_LOW,       ///< low priority
    LOGLEVEL_MEDIUM,    ///< medium priority
    LOGLEVEL_HIGH,      ///< high priority
    LOGLEVEL_UNEXPECTED ///< unexpected event
};

/****************************************************************************/
/**
 * \brief Enum containing Text message types displayed in the wait dialogs.
 */
/****************************************************************************/

enum WaitDialogText_t{
    DEFAULT_TEXT,
    INITIALIZING_TEXT, ///< Sytem Initializing text
    PROCESSING_TEXT,   ///< Text related to processing
    SOFTWARE_UPDATE_TEXT,  ///< Text is displayed for SW update
    PROCESSING_TEXT_2     ///< Text related to Fill Level Scanning processing
};

/****************************************************************************/
/**
 * \brief Enum containing all known action Types.
 */
/****************************************************************************/

enum ActionType {
    ACNTYPE_SHUTDOWN,   ///< Proceed for System Shutdown
    ACNTYPE_STOP,       ///< Proceed for System stop (System is Paused)
    ACNTYPE_ERROR,      ///< Action for Normal Error Message
    ACNTYPE_INFO,       ///< Send Info if required, log info
    ACNTYPE_RETRY,      ///< Send a Retry Msg, needs acknowledgement
    ACNTYPE_REBOOT,     ///< Proceed for System Reboot
    ACNTYPE_WARNING,    ///< Action for Normal Warning Message
    ACNTYPE_BUSY,       ///< TBD after discussion
    ACNTYPE_IDLE,       ///< TBD
    ACNTYPE_NONE,        ///< No action, just log
    ACNTYPE_REMOVEALLRACKS,
    ACNTYPE_REMOVERACK,
    ACNTYPE_CONTINUE_STAINING,
    ACNTYPE_DONT_PROCESS_RACK,
    ACNTYPE_RESCAN,
    ACNTYPE_FINISH_INIT,
    ACNTYPE_REINIT,
    ACNTYPE_CHECK_OVERFILL,
    ACNTYPE_USE_UNLOADER,
    ACNTYPE_NO_RESCAN,
    ACNTYPE_REJECT_RETRY,
    ACNTYPE_FORWARD,
    ACNTYPE_UNEXPECTED        /// < Unexpected text in action column, raise an error
};

/****************************************************************************/
/**
 * \brief Enum for event source
 */
/****************************************************************************/
enum EventSourceType {
    EVENTSOURCE_MAIN,
    EVENTSOURCE_DEVICECOMMANDPROCESSOR,
    EVENTSOURCE_SCHEDULER,
    EVENTSOURCE_EXPORT,
    EVENTSOURCE_IMPORTEXPORT,
    EVENTSOURCE_BLG,
    EVENTSOURCE_EVENTHANDLER,
    EVENTSOURCE_SEPIA,
    EVENTSOURCE_NONE,
    EVENTSOURCE_DATALOGGER,
    EVENTSOURCE_NOTEXIST,
    EVENTSOURCE_COLORADO
};

/****************************************************************************/
/**
 * \brief Enum describing if an event "turned on" or "turned off".
 */
/****************************************************************************/
enum EventStatus {
    EVTSTAT_OFF = 0,        ///< Event "turned off". For example: now there is glass in cover slipper.
    EVTSTAT_ON  = 1         ///< Event "turned on". For example: no glass in cover slipper.
};

/****************************************************************************/
/**
 * \brief Enum containing all known GUI message Types.
 */
/****************************************************************************/
enum GUIMessageType {
    GUIMSGTYPE_UNDEFINED,   ///< Undefined. Used for initialisation.
    GUIMSGTYPE_ERROR,       ///< Error.
    GUIMSGTYPE_WARNING,     ///< Warning.
    GUIMSGTYPE_INFO         ///< Info.
};

/****************************************************************************/
/**
 * \brief Enum for temperature formats.
 */
/****************************************************************************/
enum TemperatureFormat {
    TEMP_FORMAT_UNDEFINED,  ///< Undefined. Used for initialization.
    TEMP_FORMAT_CELSIUS,    ///< Celsius.
    TEMP_FORMAT_FAHRENHEIT  ///< Fahrenheit.
};

/****************************************************************************/
/**
 * \brief Enum for on / off state.
 */
/****************************************************************************/
enum OnOffState {
    ONOFFSTATE_UNDEFINED,   ///< Undefined. Used for initialization.
    ONOFFSTATE_ON,          ///< On.
    ONOFFSTATE_OFF          ///< Off.
};

/****************************************************************************/
/**
 * \brief Enum for date format.
 */
/****************************************************************************/
enum DateFormat {
    DATE_UNDEFINED,     ///< Undefined. Used for initialization.
    DATE_INTERNATIONAL, ///< "DD.MM.YYYY" with leading zeroes.
    DATE_ISO,           ///< "YYYY-MM-DD" with leading zeroes.
    DATE_US             ///< "MM/DD/YYYY" with leading zeroes.
};

/****************************************************************************/
/**
 * \brief Enum for time format.
 */
/****************************************************************************/
enum TimeFormat {
    TIME_UNDEFINED, ///< Undefined. Used for initialization.
    TIME_24,        ///< 17:01:42
    TIME_12         ///< "05:01:42 p.m."
};

/****************************************************************************/
/**
 * \brief Enum for oven start mode.
 */
/****************************************************************************/
enum OvenStartMode {
    OVENSTART_UNDEFINED,        ///< Undefined. Used for initialization.
    OVENSTART_AFTER_STARTUP,    ///< After startup.
    OVENSTART_BEFORE_PROGRAM    ///< Before program start.
};

/****************************************************************************/
/**
 * \brief Enum for date time format.
 */
/****************************************************************************/
enum DateTimeFormat {
    DATE_TIME_UNDEFINED, ///< Undefined. Used for initialization.
    DATE_INTERNATIONAL_TIME_24, ///< "DD.MM.YYYY" with leading zeroes. ///< 17:01:42
    DATE_INTERNATIONAL_12, ///< "DD.MM.YYYY" with leading zeroes. ///< 05:01:42 p.m.
    DATE_ISO_TIME_24, ///< "YYYY-MM-DD" with leading zeroes ///< 17:01:42
    DATE_ISO_12, ///< "YYYY-MM-DD" with leading zeroes ///< 05:01:42 p.m.
    DATE_US_TIME_24, ///< "MM/DD/YYYY" with leading zeroes ///< 17:01:42
    DATE_US_12 ///< "MM/DD/YYYY" with leading zeroes ///< 05:01:42 p.m.
};

/****************************************************************************/
/**
 * \brief Enum for Water type.
 */
/****************************************************************************/
enum WaterType {
    WATER_TYPE_UNDEFINED, ///< Undefined. Used for initialization.
    WATER_TYPE_TAP, ///< Tap water
    WATER_TYPE_DISTILLED ///< Distilled water
};

/****************************************************************************/
/**
 * \brief Enum for Water type.
 */
/****************************************************************************/
enum LogLevel {
    LOG_ENABLED, ///< Enable logging
    LOG_DISABLED,///< Disable logging
    LOG_DEBUG,  ///< Debugging
    LOG_CONSOLE ///< Dump log messages to console
};

/****************************************************************************/
/**
 * \brief Enum for Heating state
 */
/****************************************************************************/
enum HeatingState{
    OFF,                    ///< Heating is off
    TEMPERATURE_IN_RANGE,   ///< Self explaining
    TEMPERATURE_OUT_OF_RANGE ///< Self explaining
};

/****************************************************************************/
/**
 * \brief Enum for liquid level state
 */
/****************************************************************************/
enum LiquidLevelState {
    LIQUID_LEVEL_UNKNOWN,
    LIQUID_LEVEL_EMPTY,
    LIQUID_LEVEL_FULL,
    LIQUID_LEVEL_COVERED,
    LIQUID_LEVEL_RACK,
    LIQUID_LEVEL_MINIMUM,
    LIQUID_LEVEL_CUVETTE_MISSING,
    LIQUID_LEVEL_MOVEFAIL,
    LIQUID_LEVEL_READFAIL
};

/****************************************************************************/
/**
 * \brief Enum for Heating mode
 */
/****************************************************************************/
enum HeatingMode {
    HeatingModeOff,
    HeatingModeHeat,        ///< heating without oven, all cuvettes are allowed to heat up at the same time
    HeatingModeHold         ///< heating includes oven, cuvettes are heated exclusively one by one
};

/****************************************************************************/
/**
 * \brief Enum for station defect reason
 */
/****************************************************************************/
enum StationDefectReason_t {
    STATION_DEFECT_NONE,
    STATION_DEFECT_UNDEFINED,
    STATION_DEFECT_EMPTY,
    STATION_DEFECT_RACK_IN_STATION,
    STATION_DEFECT_CUVETTE_MISSING,
    STATION_DEFECT_COVERED,
    STATION_DEFECT_UNREADABLE,
    STATION_DEFECT_TEMPERATURE_OVERSHOOT,
    STATION_DEFECT_TEMPERATURE_NOT_REACHED,
    STATION_DEFECT_COVER_DEFECT,
    STATION_DEFECT_OVERFILL,
    STATION_DEFECT_MOTOR,
    STATION_DEFECT_INIT,
    STATION_DEFECT_NO_RESPONSE,
    STATION_DEFECT_WORKFLOW,         //!< defect as consequence of workflow interruption
    STATION_DEFECT_REAGENT_EXPIRED
};


/****************************************************************************/
/**
 * \brief Enum for Hood cover state
 */
/****************************************************************************/
enum CoverState {
    OPEN,   ///< Cover opened
    CLOSED  ///< Cover closed
};

/****************************************************************************/
/**
 * \brief Enum for execution state
 */
/****************************************************************************/
enum ExecutionState {
    START,  ///< Start execution
    STOP   ///< Stop execution
};

/****************************************************************************/
/**
 * \brief Enum for alarm type
 */
/****************************************************************************/
enum AlarmType {
    ALARM_NONE,     //!< none
    ALARM_WARNING,  //!< warnings
    ALARM_ERROR     //!< error
};

/****************************************************************************/
/**
 * \brief Enum containing all alarm position Types.
 */
/****************************************************************************/
enum AlarmPosType {
    ALARMPOS_NONE,          ///< No alarm
    ALARMPOS_DEVICE,        ///< only device alarm .
    ALARMPOS_LOCAL,         ///< alarm includes device and local
    ALARMPOS_REMOTE         ///< alarm includes device, local and remote site
};

/****************************************************************************/
/**
 * \brief Enum for GUI button type in Msg Box
 */
/****************************************************************************/
enum GuiButtonType {
    NOT_SPECIFIED,   //!< Msg Box with buttons not specified
    OK,              //!< Msg Box with Ok
    YES_NO,          //!< Msg Box with Yes and No
    RETRY,           //!< Msg Box with retry
    CONTINUE_STOP,   //!< Msg Box with Continue and Stop
    OK_CANCEL,       //!< Msg Box with Ok and Cancel
    WITHOUT_BUTTONS,  //!< Msg box without buttons
    INVISIBLE
};

/****************************************************************************/
/**
 * \brief Enum for GUI user roles
 */
/****************************************************************************/
enum GuiUserLevel{
    ADMIN,    //!< Admin user
    OPERATOR, //!< Operator (normal user)
    SERVICE   //!< Service user
};

/****************************************************************************/
/**
 * \brief Enum for GUI user roles
 */
/****************************************************************************/
enum RMSOptions_t {
    RMS_UNDEFINED = 0,
    RMS_OFF,
    RMS_CASSETTES,
    RMS_CYCLES,
    RMS_DAYS
};

/****************************************************************************/
/**
 * \brief Enum for using Alternate EventStrings
 */
/****************************************************************************/
enum AlternateEventStringUsage {
    NOT_APPLICABLE,
    GUI_MSG_BOX,
    USER_RESPONSE,
    LOGGING,
    GUI_MSG_BOX_AND_USER_RESPONSE,
    GUI_MSG_BOX_AND_LOGGING,
    GUI_MSG_BOX_LOGGING_AND_USER_RESPONSE
};

/****************************************************************************/
/**
 * \brief Enum for Program not startable reasons
 */
/****************************************************************************/
enum NonStartableProgramReasons {
    BLG_NOT_GENERATED = -1,               //!< BathLayout was not generated for the program
    PROGRAM_READY_TO_START,               //!< Program is ready to start
    HEATED_CUVETTES_NOT_AVAILABLE,        //!< Heated Cuvettes not available
    DI_WATER_NOT_AVAILABLE,               //!< DI water not available
    STATION_NOT_AVAILABLE,                //!< Station not available
    OVEN_DEFECT,                          //!< Oven defect
    HEATED_CUVETTES_DEFECT,               //!< Heated cuvettes defect
    WATER_STATION_DEFECT,                 //!< Wtaer station defect
    REAGENTS_NOT_IN_BATHLAYOUT,           //!< Reagents not in BathLayout
    REAGENTS_EXPIRED,                     //!< Reagents expired
    PROGRAM_NOT_STARTABLE_VARIOUS_REASONS,//!< Not startable for various reasons
    PROGRAM_NOT_SELECTED_FOR_USAGE        //!< Program not selected for usage
};

/****************************************************************************/
/**
 * \brief Enum for reagent class.
 */
/****************************************************************************/
enum ReagentClasses{
    DEWAXING_CLASS = 1,         //!< Dewaxing reagent class
    NEUTRALISING_CLASS,     //!< Neutralising reagent class
    RINSING_CLASS,          //!< Rinsing reagent class
    STAINING_CLASS,         //!< Staining reagent class
    DIFFERENTIATING_CLASS,  //!< Differentialing reagent class
    DEHYDRATING_CLASS,      //!< Dehydrating reagent class
    NO_CLASS                //!< No class defined for the reagent
};


/****************************************************************************/
/**
 * \brief Enumeration of power fail stages
 */
/****************************************************************************/
enum PowerFailStages{
    POWER_FAIL_STAGE_1, //!< power went off, don't shut down yet wait for a few seconds
    POWER_FAIL_STAGE_2, //!< Looks like power won't be back! shutdown!!!
    POWER_FAIL_REVERT,   //!< Power is back, revert to normal state
    POWER_FAIL_NONE
};

/****************************************************************************/
/**
 * \brief class for application setting
 */
/****************************************************************************/
class AppSettings
{
public:
    static bool SimulationEnabled;  //!< simulation enabled
    static int SimulationAccelerationFactor; //!< simulation acceleration factor
    static bool ExtendedLoggingEnabled; //!<  extended logging enabled like scheduler files
    static bool TraceDebugMessagesEnabled; //!< debug messages enabled
    static bool DCPLoggingEnabled;          //!<  dcp log file be created
    static QString RevisionNumber;          //!<  revision number
    static bool SettingsRolledBack; //!< Settings /configuration rolled back to last known valid settings.
};

/****************************************************************************/
/**
 * \brief Enum refers to number of cover lines
 */
/****************************************************************************/
enum CoverSlipLinesNumber
{
    UNDEFINED,       //!< no cover  slip line
    ONELINE,        //!< One cover slip line
    TWOLINE        //!< two cover slip line
};

/****************************************************************************/
/**
 * \brief Enum refers to Service Software Mode
 */
/****************************************************************************/
enum ServiceSoftwareMode {
    SERVICE_MODE,               //!< Service Mode
    MANUFACTURING_MODE,         //!< Manufacturing Mode
    INVALID_MODE                //!< Invalid Mode
};

} // end namespace Global

#endif // GLOBAL_GLOBALDEFINES_H

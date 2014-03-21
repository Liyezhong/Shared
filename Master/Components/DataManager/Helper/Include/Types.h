/****************************************************************************/
/*! \file Types.h
 *
 *  \brief Definition file for Types used from DataManager
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-20
 *  $Author:    $ F. Toth
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

#ifndef DATAMANAGER_TYPES_H
#define DATAMANAGER_TYPES_H


#include "Global/Include/TranslatableString.h"
typedef QList<QString> ListOfIDs_t;
typedef QList<QString> ListOfKeys_t;
typedef QHash<QString, int> ListOfForecastValues_t;
typedef QHash<quint32, Global::tTranslatableStringList> ErrorHash_t; //!< Self Explaining.
//!< List of hash tables with Error IDs
typedef QList<ErrorHash_t *>  ListOfErrors_t;

//Types of Stations
typedef enum {
    DASHBOARD_STATION_BOTTLE,
    DASHBOARD_STATION_PARAFFIN,
    DASHBOARD_STATION_RETORT,
    INVALID_STATION
}DashboardStationType_t;

//Station Status
typedef enum {
    DASHBOARD_RETORT_EMPTY_CLEAN,
    DASHBOARD_RETORT_FULL,
    DASHBOARD_RETORT_PARTIALLY_FILLED,
    DASHBOARD_RETORT_EMPTY_POLLUTED,
    DASHBOARD_PARAFFIN_EMPTY,
    DASHBOARD_PARAFFIN_FULL,
    DASHBOARD_PARAFFIN_PARTIALLY_FILLED,
    DASHBOARD_PARAFFIN_EXPIRED,
    DASHBOARD_PARAFFIN_NOT_IN_USE,
    DASHBOARD_REAGENT_BOTTLE_EMPTY,
    DASHBOARD_REAGENT_BOTTLE_FULL,
    DASHBOARD_REAGENT_BOTTLE_PARTIALLY_FILLED,
    DASHBOARD_REAGENT_BOTTLE_EXPIRED,
    DASHBOARD_REAGENT_BOTTLE_NOT_IN_USE,
    INVALID_STATUS
}DashboardStationStatus_t;

// Types of reagents
typedef enum
{
    SPECIAL_REAGENT,
    LEICA_REAGENT,
    USER_REAGENT,
    UNDEFINED_REAGENT_TYPE
} ReagentType_t;

// Types of stations
typedef enum
{
    DRY,
    HEATED,
    LOAD,
    OVEN,
    PARK,
    REAGENT,
    SLIDE,
    TRANSFER,
    UNLOAD,
    WATER,
    MACRO,      // only admissible in programs/macros
    INVALID_TYPE
} StationsType_t;

const int NUMBER_OF_STATION_TYPES = 12;
const QString NameOfStationTypes[12] = {"Dry", "Heated", "Load", "Oven", "Park", "Reagent", "Slide", "Transfer", "Unload", "Water", "Macro", "Invalid_Type"};

typedef QHash<QString, StationsType_t> HashOfStationTypes_t;  // filled in CDataProgramList::Init()   // TODO: move to better place

// Types of heating start modes
typedef enum
{
    AT_DEVICE_START,
    AT_PROGRAM_START,
    UNDEFINED
} HeatingStartMode_t;
const int NUMBER_OF_HEATING_START_MODES = 2;
const QString NameOfHeatingStartModes[2] = {"AtDeviceStart", "AtProgramStart"};

typedef QHash<QString, HeatingStartMode_t> HashOfHeatingStartMode_t;  // filled in CDataReagentList::Init()   // TODO: move to better place

typedef enum
{
    PROGRAMS,
    PROGRAMSEQUENCE,
    RACKS,
    REAGENTS,
    REAGENTGROUP,
    REAGENTGROUPCOLOR,
    STATIONS,
    STATIONSGRID,
    BLGCONFIGURATION,
    USERSETTINGS,
    EXPORT,
    DEVICECONFIGURATION,
    SWVERSION,
    ADJUSTMENT,
    PARAMETER,
    INVALID_CONTAINER_TYPE,
    INSTRUMENTHISTORY
} DataContainerType_t;


typedef enum
{
    NODE_PROGRAM_LIST,
    NODE_NEXT_FREE_PROG_ID,
    NODE_PROGRAM,
    NODE_MACRO,
    NODE_STEP,
    NODE_PROG_ID,
    NODE_MACRO_ID,
    NODE_SHORT_NAME,
    NODE_LONG_NAME,
    NODE_COLOR,
    NODE_NEXT_FREE_STEP_ID,
    NODE_LOCKED,
    NODE_STEP_ID,
    NODE_STATION_TYPE,
    NODE_MIN_DURATION,
    NODE_MAX_DURATION,
    NODE_REAGENT_ID,
    NODE_REAGENT_TEMP,
    NODE_EXCLUSIVE_FLAG,
    NODE_OPERATING_MODE
} NodesOfProgramList_t;

typedef enum
{
    RL_NODE_REAGENT_LIST,
    RL_NODE_NEXT_FREE_REAGENT_ID,
    RL_NODE_REAGENT,
    RL_NODE_REAGENT_ID,
    RL_NODE_LOCKED,
    RL_NODE_SHORT_NAME,
    RL_NODE_LONG_NAME,
    RL_NODE_MAX_TIME,
    RL_NODE_MAX_CYCLES,
    RL_NODE_CLASS,
    RL_NODE_RACK5,
    RL_NODE_EXPIRY_DATE
} NodesOfReagentList_t;

typedef enum
{
    LEFT_ZONE,
    COLLISION_ZONE,
    RIGHT_ZONE,
    INVALID_ZONE
} StationZoneType_t;

// Types of SW type
typedef enum
{
    MASTERSOFTWARE,
    FIRMWARE
} SWType_t;


#endif // DATAMANAGER_TYPES_H

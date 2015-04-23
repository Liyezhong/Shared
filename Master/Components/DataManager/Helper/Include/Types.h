/****************************************************************************/
/*! \file DataManager/Helper/Include/Types.h
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

#include <QString>
#include <QList>
#include <QHash>
#include "Global/Include/TranslatableString.h"

typedef QList<QString> ListOfIDs_t;     //!< Self Explaining.
typedef QList<QString> ListOfKeys_t;    //!< Self Explaining.
typedef QHash<QString, int> ListOfForecastValues_t; //!< Self Explaining.
typedef QMap<quint32, Global::tTranslatableStringList> ErrorMap_t; //!< Self Explaining.
//!< List of hash tables with Error IDs
typedef QList<ErrorMap_t *>  ListOfErrors_t;    //!< Self Explaining.


/**
 * \brief Types of Stations
 */
typedef enum {
    DASHBOARD_STATION_BOTTLE, //!< DASHBOARD_STATION_BOTTLE
    DASHBOARD_STATION_PARAFFIN, //!< DASHBOARD_STATION_PARAFFIN
    DASHBOARD_STATION_RETORT, //!< DASHBOARD_STATION_RETORT
    INVALID_STATION //!< INVALID_STATION
}DashboardStationType_t;

/**
 * \brief Station Status
 */
typedef enum {
    DASHBOARD_RETORT_EMPTY_CLEAN, //!< DASHBOARD_RETORT_EMPTY_CLEAN
    DASHBOARD_RETORT_FULL,  //!< DASHBOARD_RETORT_FULL
    DASHBOARD_RETORT_PARTIALLY_FILLED, //!< DASHBOARD_RETORT_PARTIALLY_FILLED
    DASHBOARD_RETORT_EMPTY_POLLUTED, //!< DASHBOARD_RETORT_EMPTY_POLLUTED
    DASHBOARD_PARAFFIN_EMPTY, //!< DASHBOARD_PARAFFIN_EMPTY
    DASHBOARD_PARAFFIN_FULL, //!< DASHBOARD_PARAFFIN_FULL
    DASHBOARD_PARAFFIN_PARTIALLY_FILLED, //!< DASHBOARD_PARAFFIN_PARTIALLY_FILLED
    DASHBOARD_PARAFFIN_EXPIRED, //!< DASHBOARD_PARAFFIN_EXPIRED
    DASHBOARD_PARAFFIN_NOT_IN_USE, //!< DASHBOARD_PARAFFIN_NOT_IN_USE
    DASHBOARD_REAGENT_BOTTLE_EMPTY, //!< DASHBOARD_REAGENT_BOTTLE_EMPTY
    DASHBOARD_REAGENT_BOTTLE_FULL, //!< DASHBOARD_REAGENT_BOTTLE_FULL
    DASHBOARD_REAGENT_BOTTLE_PARTIALLY_FILLED, //!< DASHBOARD_REAGENT_BOTTLE_PARTIALLY_FILLED
    DASHBOARD_REAGENT_BOTTLE_EXPIRED, //!< DASHBOARD_REAGENT_BOTTLE_EXPIRED
    DASHBOARD_REAGENT_BOTTLE_NOT_IN_USE, //!< DASHBOARD_REAGENT_BOTTLE_NOT_IN_USE
    INVALID_STATUS //!< INVALID_STATUS
}DashboardStationStatus_t;


typedef qint32 Position_t;          //!< Stepper motor position, [half steps]
typedef quint8 MotionProfileIdx_t;  //!< Stepper motor motion profile index

/****************************************************************************/
/**
 * \brief Types of reagents
 */
/****************************************************************************/
typedef enum
{
    SPECIAL_REAGENT,
    LEICA_REAGENT,
    USER_REAGENT,
    CONSUMABLES_REAGENT,
    UNDEFINED_REAGENT_TYPE
} ReagentType_t;

/****************************************************************************/
/**
 * \brief Types of stations
 */
/****************************************************************************/
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
    CONSUMABLES,
    MACRO,      // only admissible in programs/macros
    ROTATION,
    COVERSLIP,
    EMPTY,
    INVALID_TYPE
} StationsType_t;


const int NUMBER_OF_STATION_TYPES = 12;     //!< Self Explaining.

const QString NameOfStationTypes[12] = {"Dry", "Heated", "Load", "Oven",
                                        "Park", "Reagent", "Slide", "Transfer",
                                        "Unload", "Water", "Macro", "Invalid_Type"}; //!< Self Explaining.

// filled in CDataProgramList::Init()   // TODO: move to better place
typedef QHash<QString, StationsType_t> HashOfStationTypes_t;    //!< Self Explaining.

/****************************************************************************/
/**
 * \brief Types of heating start modes
 */
/****************************************************************************/
typedef enum
{
    AT_DEVICE_START,
    AT_PROGRAM_START,
    AT_UNDEFINED
} HeatingStartMode_t;

const int NUMBER_OF_HEATING_START_MODES = 2;     //!< Self Explaining.

const QString NameOfHeatingStartModes[2] = {"AtDeviceStart", "AtProgramStart"}; //!< Self Explaining.

// filled in CDataReagentList::Init()   // TODO: move to better place
typedef QHash<QString, HeatingStartMode_t> HashOfHeatingStartMode_t;  //!< Self Explaining.

/****************************************************************************/
/**
 * \brief Types of data containers
 */
/****************************************************************************/
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
    INSTRUMENTHISTORY,
    RCCONFIGURATION,
    ADJUSTMENT_XYZG
} DataContainerType_t;


/****************************************************************************/
/**
 * \brief Types of nodes in programs xml file
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 * \brief Types of nodes in reagent xml file
 */
/****************************************************************************/
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

/****************************************************************************/
/**
 * \brief Types of station zone types
 */
/****************************************************************************/
typedef enum
{
    LEFT_ZONE,
    COLLISION_ZONE,
    RIGHT_ZONE,
    MID_ZONE,
    INVALID_ZONE
} StationZoneType_t;




/****************************************************************************/
/**
 * \brief Types of SW types
 */
/****************************************************************************/
typedef enum
{
    MASTERSOFTWARE,
    FIRMWARE,
    INITSCRPITS
} SWType_t;

#endif // DATAMANAGER_TYPES_H

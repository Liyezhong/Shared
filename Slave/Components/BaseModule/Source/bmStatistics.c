/****************************************************************************/
/*! \file bmStatistics.c
 *
 *  \brief Task statistic measurement
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to control meassurement of task
 *       statistics. Following informations will be collected for each task:
 *
 *       - Run time of task (minimum, average, maximum)
 *       - Interval time of task (minimum, average, maximum)
 *       - Number of task calls
 *
 *       Statistics can be configured and read using dedicated CAN system
 *       messages. If statistics are not required, don't call the init
 *       function of this module.
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

#include <stdlib.h>
#include "Global.h"
#include "Basemodule.h"
#include "bmStatistics.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define STAT_FLAG_ENABLE     0x0080  //!< Enable statistics collection
#define STAT_FLAG_CLEAR      0x0040  //!< Clear statistics to zero
#define STAT_FLAG_RESYNC     0x0100  //!< Resync statistics collection
#define STAT_FLAG_SCALING    0x0008  //!< Scale run count by 1000
#define STAT_FLAG_SELECT     0x0003  //!< Data request selector bitmask

#define STAT_FLAG_NO_DATA    0       //!< Data request: no data
#define STAT_FLAG_RUNTIME    1       //!< Data request: run time data
#define STAT_FLAG_INTERVAL   2       //!< Data request: interval data

//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Data structure to hold minimal, average, and maximal times
typedef struct {
    UInt32 MinTime;         //!< Minimal time measured
    UInt32 AvgTime;         //!< Average time measured
    UInt32 MaxTime;         //!< Maximal time measured
} bmTaskTimes_t;

//! Data structure to hold statistics for a task
typedef struct {
    UInt16 Flags;           //!< Statistics control flags
    UInt32 RunCount;        //!< Number of task runs
    bmTaskTimes_t RunTime;  //!< Task run times 
    bmTaskTimes_t Interval; //!< Task interval times
    UInt32 LastTime;        //!< Time of measurement start
} bmTaskStatistics_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

static bmTaskStatistics_t *StatisticsTable; //!< Statistics table
static int StatisticsTableSize = 0;         //!< Size of statistics table


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmUpdateTaskInterval (UInt16 TaskID, UInt32 Time);
static Error_t bmUpdateTaskRunTime  (UInt16 TaskID, UInt32 Time);
static Error_t bmConfigStatistics   (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSendStatistics     (UInt16 Channel, bmTaskTimes_t *Times);
static void    bmClearStatistics    (UInt16 Channel);

static UInt16  bmGetAverage (UInt16 Average, UInt16 Value, UInt16 Count);


/*****************************************************************************/
/*!
 *  \brief   Control colletion of task statistics
 *
 *      This function controls statistic data collection. It must be called
 *      once with ControlID set to STAT_TASK_START before the task is going
 *      to be started, and once with ControlID set to STAT_TASK_STOP, after
 *      the task has finished.
 *
 *      If statistics collection in general or for that particular task is
 *      disabled, this function does nothing.
 *
 *  \iparam  TaskID    = Task identifier
 *  \iparam  ControlID = Start/stop measurement control
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmControlStatistics (UInt16 TaskID, bmStatCtrlID_t ControlID) {

    UInt32 Timestamp;

    if (TaskID < StatisticsTableSize) {
        if (StatisticsTable[TaskID].Flags & STAT_FLAG_ENABLE) {

            Timestamp = halGetFastTick();

            if (ControlID == STAT_TASK_START) {
                bmUpdateTaskInterval (TaskID, Timestamp);
            }
            else {
                bmUpdateTaskRunTime (TaskID, Timestamp);
            }
            StatisticsTable[TaskID].RunCount++;
        }
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Update task interval time statistics
 *
 *      Updates interval time statistics for a particular task. The minimal
 *      maximal, and average interval times are collected. When the run
 *      counter reaches it's maximum, futher statistics updates will be
 *      stopped automatically. Statistics must be cleared and started
 *      again, to initiate an new measurement.
 *
 *  \iparam  TaskID = Task identifier
 *  \iparam  Time   = Current time (time stamp)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmUpdateTaskInterval (UInt16 TaskID, UInt32 Time) {

    bmTaskStatistics_t *Statistics = &StatisticsTable[TaskID];
    UInt32 Duration;

    if (Statistics->RunCount < MAX_UINT32) {

        Duration = Time - Statistics->LastTime;

        if ((Statistics->Flags & STAT_FLAG_RESYNC) == 0) {

            if (Duration < Statistics->Interval.MinTime) {
                Statistics->Interval.MinTime = Duration;
            }
            if (Duration > Statistics->Interval.MaxTime) {
                Statistics->Interval.MaxTime = Duration;
            }
            Statistics->Interval.AvgTime = bmGetAverage (
                Statistics->Interval.AvgTime, Duration, Statistics->RunCount);
        }
        Statistics->Flags &= ~STAT_FLAG_RESYNC;
        Statistics->LastTime = Time;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Updates task runtime statistics
 *
 *      Updates run time statistics for a particular task. The minimal
 *      maximal, and average run times were collected. When the run
 *      counter reaches it's maximum, futher statistics updates will be
 *      stopped automatically. Statistics must be cleared and started
 *      again, to initiate an new measurement.
 *
 *  \iparam  TaskID = Task identifier
 *  \iparam  Time   = Current time (time stamp)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmUpdateTaskRunTime (UInt16 TaskID, UInt32 Time) {

    bmTaskStatistics_t *Statistics = &StatisticsTable[TaskID];
    UInt32 Duration;

    if (Statistics->RunCount < MAX_UINT32) {

        Duration = Time - Statistics->LastTime;

        if (Duration < Statistics->RunTime.MinTime) {
            Statistics->RunTime.MinTime = Duration;
        }
        if (Duration > Statistics->RunTime.MaxTime) {
            Statistics->RunTime.MaxTime = Duration;
        }
        Statistics->RunTime.AvgTime = bmGetAverage (
            Statistics->RunTime.AvgTime, Duration, Statistics->RunCount);

        Statistics->LastTime = Time;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Clear statistics
 *
 *      Clears statistic data of the task associated with the given channel.
 *
 *  \iparam  Channel = Logical channel number
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static void bmClearStatistics (UInt16 Channel) {

    bmTaskStatistics_t *Statistics = &StatisticsTable[Channel];

    if (Channel < StatisticsTableSize) {
        Statistics->RunTime.MinTime  = MAX_UINT32;
        Statistics->RunTime.AvgTime  = 0;
        Statistics->RunTime.MaxTime  = 0;
    
        Statistics->Interval.MinTime = MAX_UINT32;
        Statistics->Interval.AvgTime = 0;
        Statistics->Interval.MaxTime = 0;

        Statistics->RunCount = 0;
    }
}


/*****************************************************************************/
/*!
 *  \brief   Send statistics data
 *
 *      Sends statistic data collected for the task associated with the
 *      given channel to the master. The following data will be send:
 *
 *      - Minimal, average and maximal runtimes/intervals
 *      - Number of task calls
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Times   = Measured times (min, avg, max)
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSendStatistics (UInt16 Channel, bmTaskTimes_t *Times) {

    CanMessage_t Message = {0};

    if (Times != NULL) {

        UInt32 RunCount = StatisticsTable[Channel].RunCount;

        if (RunCount) {
            bmSetMessageItem (&Message, Times->MinTime, 2, 2);
            bmSetMessageItem (&Message, Times->AvgTime, 4, 2);
            bmSetMessageItem (&Message, Times->MaxTime, 6, 2);
        }
        if (StatisticsTable[Channel].Flags & STAT_FLAG_SCALING) {
            RunCount /= 1000;
        }
        if (RunCount > MAX_UINT16) {
            RunCount = MAX_UINT16;
        }
        bmSetMessageItem (&Message, RunCount, 0, 2);

        Message.CanID = MSG_SRV_STATISTICS;
        Message.Length = 8;

        return (canWriteMessage(Channel, &Message));
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief   Configure and/or request task statistics
 *
 *      Configures the collection of task statistics for a particular task.
 *      The following settings can be configured:
 *
 *      - start/stop measurement
 *      - clear statistics counters
 *      - request sending of statistic data
 *
 *      Two types of statistic data can be requested: task run times and
 *      task run intervalls.
 *
 *      This function is called by the CAN message dispatcher when a
 *      configuration message is received from the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmConfigStatistics (UInt16 Channel, CanMessage_t *Message) {

    if (Message->Length >= 1) {

        UInt16 Flags = bmGetMessageItem(Message, 0, 1);

        if (Channel >= StatisticsTableSize) {
            return (E_STATS_NOT_AVAILABLE);
        }
        if ((Flags & STAT_FLAG_SELECT) == STAT_FLAG_RUNTIME) {
            bmSendStatistics (Channel, &StatisticsTable[Channel].RunTime);
        }
        else if ((Flags & STAT_FLAG_SELECT) == STAT_FLAG_INTERVAL) {
            bmSendStatistics (Channel, &StatisticsTable[Channel].Interval);
        }
        if (Flags & STAT_FLAG_CLEAR) {
            bmClearStatistics (Channel);
        }
        if ((Flags ^ StatisticsTable[Channel].Flags) & STAT_FLAG_ENABLE) {
            Flags |= STAT_FLAG_RESYNC;
        }
        StatisticsTable[Channel].Flags = Flags;

        return (NO_ERROR);
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Calculate the running average
 *
 *      Incrementally calculates the average over a number of values. It
 *      must be called once for each value to be included into calculation,
 *      where Count is the number of values included so far. Average is
 *      the result of the previous calculation (or zero on first call).
 *
 *  \iparam  Average  = Running average over count-1 samples
 *  \iparam  NewValue = New data value to add to the average
 *  \iparam  Count    = Number of samples so far
 *
 *  \return  New average value, including newValue
 *
 ****************************************************************************/

static UInt16 bmGetAverage (UInt16 Average, UInt16 NewValue, UInt16 Count) {

    if (Count) {
        return (((Average * (Count-1)) + NewValue) / Count);
    }
    return (NewValue);
}


/*****************************************************************************/
/*!
 *  \brief   Initializes the statistics module
 *
 *      Initializes this module by allocating memory for the statistic
 *      data for each task. Moreover, CAN messages to be handled by
 *      this module, are registered to the message dispatcher. Since
 *      the base module handles statistics for all modules/instances,
 *      the message handler(s) are registered for each single
 *      module/instance (task).
 *
 *      This function is called once during startup.
 *
 *  \iparam  TaskCount = Number of tasks
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitializeStatistics (UInt16 TaskCount) {

    static bmCallbackEntry_t Commands[] = {
        { MSG_SRV_CFG_STATISTICS, bmConfigStatistics  }
    };
    Error_t Status;
    UInt16 i;

    // allocate statistics table
    StatisticsTable = calloc (TaskCount, sizeof(bmTaskStatistics_t));
    if (StatisticsTable == NULL) {
        return (E_MEMORY_FULL);
    }
    StatisticsTableSize = TaskCount;

    // clear statistics of all tasks
    for (i=0; i < StatisticsTableSize; i++) {
        bmClearStatistics(i);
    }
    // bind CAN commands to handler functions (for every module)
    Status = canRegisterBroadcast (Commands, ELEMENTS(Commands));
    if (Status < 0) {
        return (Status);
    }
    return (NO_ERROR);
}

//****************************************************************************/

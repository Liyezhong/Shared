/****************************************************************************/
/*! \file bmStatistics.h
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

#ifndef BM_STATISTICS_H
#define BM_STATISTICS_H


//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//! Statistic control codes (used by bmControlStatistics)
typedef enum {
    STAT_TASK_START,        //!< Start task statistics measurement
    STAT_TASK_STOP          //!< Stop task statistics measurement
} bmStatCtrlID_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t bmControlStatistics    (UInt16 TaskID, bmStatCtrlID_t ControlID);
Error_t bmInitializeStatistics (UInt16 TaskCount);

//****************************************************************************/

#endif /*BM_STATISTICS_H*/

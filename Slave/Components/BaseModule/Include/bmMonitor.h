/****************************************************************************/
/*! \file bmMonitor.h
 *
 *  \brief Power supply monitor functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to monitor power supply on the
 *       slave. This includes the supply voltage and the maximum allowed
 *       supply current of the board.
 *
 *       If voltage falls below a minimal level, or current exceeds a max.
 *       level, an error message is send to the master. Furthermore, the
 *       master can request the actual voltage and current.
 *
 *       The thresholds for voltage and current are configurable by
 *       the HAL layer (constants), the board option store (board
 *       specific) or can be configured via CAN bus.
 *
 *       Monitoring is possible only if the hardware offers analog
 *       inputs delivering values proportional to the voltage/current.
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

#ifndef BM_MONITOR_H
#define BM_MONITOR_H


//****************************************************************************/
// Public Constants and Macros
//****************************************************************************/

//! Power supply states (applies to both: voltage and current)
typedef enum {
    POWER_GOOD    = 0,   //!< Power is ok
    POWER_WARNING = 1,   //!< Power isn't good, but still acceptable
    POWER_FAILED  = 2,   //!< Power is bad
    POWER_UNKNOWN = 9    //!< Power is unknown (can't be read)
} bmPowerState_t;


//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

bmPowerState_t bmGetPowerSupplyStatus (void);
bmPowerState_t bmGetSafeSupplyStatus (void);

Error_t bmMonitorSupplyCurrent (void);
Error_t bmMonitorSupplyVoltage (void);
Error_t bmMonitorSupplySafe (void);
Error_t bmInitPowerMonitor (void);
void bmInitVoltageMonitor (void);
void bmInitCurrentMonitor (void);
void bmInitSafeMonitor (void);


//****************************************************************************/

#endif /*BM_MONITOR_H*/

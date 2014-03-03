/****************************************************************************/
/*! \file halRealTime.h
 * 
 *  \brief  Real Time Clock Functions
 *
 *  $Version: $ 0.1
 *  $Date:    $ 31.03.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *      This module contains functions to access and manage the processors 
 *      internal real time clock. Since the real time clock is a simple 32
 *      bit counter, the conversion to the separate time and date elements
 *      are implemented in software. If porting to another processor this
 *      conversions might become obsolete, as far as the new processor
 *      contains a real time clock delivering the separate elements.
 *      
 *      The real time clock is available in any case, no matter if a
 *      separate RTC crystal is attached to the processor or not. If no 
 *      extra crystal is attached, the main oscillator is used as clock
 *      source. In case of a dedicated crystal the real time clock may 
 *      continue to run during power off if a battery backup voltage is
 *      supplied to the processor. If this is not the case, the real time 
 *      clock must be adjusted at least once after power up.
 *
 *  \b Company:
 *
 *      Leica Biosystems Nussloch GmbH.
 * 
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice 
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#ifndef HAL_REALTIME_H
#define HAL_REALTIME_H


//****************************************************************************/
// Public Constants
//****************************************************************************/

#define DAY_IN_MILLISECONDS  (24*60*60*1000)  //!< Milliseconds per day
#define DAY_IN_SECONDS       (24*60*60)       //!< Seconds per day


//****************************************************************************/
// Public Type Definitions
//****************************************************************************/

//! Structure to hold date and time 
typedef struct {
    UInt32 Year    :12;  //!< Year (2000..4096)
    UInt32 Month   :4;   //!< Month (1..12)
    UInt32 Day     :5;   //!< Day (1..31)
    UInt32 WeekDay :3;   //!< Day of week (0..6)
    UInt32 Hours   :5;   //!< Hour (0..23)
    UInt32 Minutes :6;   //!< Minute (0..59)
    UInt32 Seconds :6;   //!< Seconds (0..59)
    UInt32 Millis  :10;  //!< Milliseconds (0...999)
} DateTime_t;

//! Enumeration to identify weekdays
typedef enum {
    Monday,              //!< Weekday Monday
    Tuesday,             //!< Weekday Tuesday
    Wednesday,           //!< Weekday Wednesday
    Thursday,            //!< Weekday Thursday
    Friday,              //!< Weekday Friday
    Saturday,            //!< Weekday Saturday
    Sunday               //!< Weekday Sunday
} Weekdays_t;

//****************************************************************************/
// Public Function Prototypes
//****************************************************************************/

Error_t halRtcReadDate  (DateTime_t *Date);
Error_t halRtcWriteDate (const DateTime_t *Date);
Error_t halRtcInit      (void);
Bool    halPowerGood    (void);

//****************************************************************************/

#endif /*HAL_REALTIME_H*/

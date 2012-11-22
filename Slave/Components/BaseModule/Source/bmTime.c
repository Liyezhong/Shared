/****************************************************************************/
/*! \file bmTime.c
 *
 *  \brief Time and timer handling functions
 *
 *  $Version: $ 0.2
 *  $Date:    $ 27.01.2011
 *  $Author:  $ Andreas Menge
 *
 *  \b Description:
 *
 *       This module contains all functions to handle times. This includes
 *       the following:
 *
 *       - Local system ticks (local free running counter)
 *       - System clock (suppied by master)
 *       - Real time and date (supplied by master)
 *
 *       The tick counter is intended to measure time differences, the system
 *       clock can be used to synchronize actions on different slaves. The
 *       real time/date can be used, when absolute time informations are
 *       required. The real time must be send by the master via the CAN
 *       time/date message at least once a day.
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

#include <string.h>
#include "Global.h"
#include "halLayer.h"
#include "bmError.h"
#include "bmCan.h"
#include "bmDispatch.h"
#include "bmUtilities.h"
#include "bmTime.h"


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Indices to address the data/time components in realtime messages
typedef enum {
    YEAR   = 0,   //!< Year index 
    MONTH  = 1,   //!< Month index 
    DAY    = 2,   //!< Day index 
    HOUR   = 3,   //!< Hour index 
    MINUTE = 4,   //!< Minute index 
    SECOND = 5,   //!< Seconds index 
    MILLI  = 6    //!< Milliseconds index
} bmDateIndices_t;

//****************************************************************************/
// Private Variables
//****************************************************************************/

static UInt16 SystemClock = 0;    //!< Last received system clock


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t bmSetSysClock (UInt16 Channel, CanMessage_t *Message);
static Error_t bmSetDateTime (UInt16 Channel, CanMessage_t *Message);
static Error_t bmVerifyDateTime (CanMessage_t *Message);
static UInt32  bmGetMonthLength (UInt32 Year, UInt32 Month);



/*****************************************************************************/
/*!
 *  \brief   Get actual system tick
 *
 *      Returns the actual count of the slave system tick counter, which
 *      counts the number of milliseconds since program start. Since the
 *      counter is unsigned, time differnces can be calculated correctly
 *      even when the counter overflows to zero, as long as the measured
 *      time interval is smaller than approx. 49 days.
 *
 *  \return  Actual count of the hardware timer
 *
 ****************************************************************************/

UInt32 bmGetTime (void) {

    return (halSysTickRead());
}


/*****************************************************************************/
/*!
 *  \brief   Calculate time difference
 *
 *      Returns the number of milliceconds expired since the time given by
 *      SinceTime. SinceTime must be a valid system tick count, as returned
 *      by bmGetTime().
 *
 *  \iparam  SinceTime = Time to calculate the differnece to
 *
 *  \return  Time expired since SinceTime [ms]
 *
 ****************************************************************************/

UInt32 bmTimeExpired (UInt32 SinceTime) {

    return (bmGetTime() - SinceTime);
}


/*****************************************************************************/
/*!
 *  \brief   Get system clock value
 *
 *      Returns the actual state of the system clock counter, maintained by
 *      the master. The system clock is updated, when a system clock message
 *      from the master is received. The interval time of the system clock
 *      is determined by the master and unknown to the slaves. The purpose
 *      of the system clock is to synchronize several action on different
 *      slaves to a common clock.
 *
 *  \return  System clock value
 *
 ****************************************************************************/

UInt16 bmGetSysClock (void) {

    return (SystemClock);
}


/*****************************************************************************/
/*!
 *  \brief   Calculate time difference
 *
 *      Returns the number of system ticks expired since the supplied
 *      reference cock. ReferenceClock must be a valid system tick count,
 *      as returned by bmGetSysClock().
 *
 *  \iparam  ReferenceClock = Clock to calculate the difference to
 *
 *  \return  Clocks ticks since "ReferenceClock"
 *
 ****************************************************************************/

UInt16 bmSysClockExpired (UInt16 ReferenceClock) {

    return (SystemClock - ReferenceClock);
}


/*****************************************************************************/
/*!
 *  \brief   System clock message handler
 *
 *      This function is called by the CAN message dispatcher, when a
 *      system clock message from the master is received. It updates the
 *      module-global SystemClock variable with the value from the
 *      received message.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSetSysClock (UInt16 Channel, CanMessage_t *Message) {

    if (Message->Length >= 2) {
        SystemClock = bmGetMessageItem(Message, 0, 2);
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Get actual time and date
 *
 *      Returns the actual time and date from the real time clock. The
 *      result is copied into the data structure pointed to by DateTime.
 *
 *  \oparam  DateTime = Pointer to structure to copy time/date into
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmReadDateTime (DateTime_t *DateTime) {

    return (halRtcReadDate(DateTime));
}


/*****************************************************************************/
/*!
 *  \brief   Set date/time message handler
 *
 *      This function is called by the CAN message dispatcher, when a
 *      time/date message from the master is received. It updates the
 *      module-global real time data structure with the date/time
 *      contained in the real time system message from the master.
 *
 *      Additionally, the TimeLastUpdated variable is set to current
 *      system tick count. This allows interpolation of time between
 *      the receipt of real time messages, by adding the time since
 *      last update to the time data structure.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmSetDateTime (UInt16 Channel, CanMessage_t *Message) {

    DateTime_t DateTime;
    //UInt16 AgeOfDate;

    if (Message->Length >= 8) {

        Error_t Status = bmVerifyDateTime(Message);
        if (Status != NO_ERROR) {
            return (Status);
        }
        DateTime.Year    = Message->Data[YEAR] + 2000;
        DateTime.Month   = Message->Data[MONTH];
        DateTime.Day     = Message->Data[DAY];
        DateTime.Hours   = Message->Data[HOUR];
        DateTime.Minutes = Message->Data[MINUTE];
        DateTime.Seconds = Message->Data[SECOND];
        DateTime.Millis  = bmGetMessageItem(Message, MILLI, 2);

        //AgeOfDate = bmTimeExpired(Message->Timestamp);

        return (halRtcWriteDate (&DateTime));
    }
    return (E_MISSING_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Request date/time message handler
 *
 *      This function is called by the CAN message dispatcher, when a
 *      time/date request message from the master is received. It reads
 *      the actual date/time from the realtime clock and returns it to
 *      the master.
 *
 *  \iparam  Channel = Logical channel number
 *  \iparam  Message = Received CAN message
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmGetDateTime (UInt16 Channel, CanMessage_t *Message) {

    CanMessage_t Response;
    DateTime_t DateTime;

    if (Message->Length == 0) {

        Error_t Status = halRtcReadDate (&DateTime);
        if (Status != NO_ERROR) {
            return (Status);
        }
        Response.Data[YEAR]   = DateTime.Year - 2000;
        Response.Data[MONTH]  = DateTime.Month;
        Response.Data[DAY]    = DateTime.Day;
        Response.Data[HOUR]   = DateTime.Hours;
        Response.Data[MINUTE] = DateTime.Minutes;
        Response.Data[SECOND] = DateTime.Seconds;
        bmSetMessageItem (&Response, DateTime.Millis, MILLI, 2);

        Response.CanID  = MSG_SRV_REALTIME;
        Response.Length = 8;

        return (canWriteMessage(Channel, &Response));
    }
    return (E_UNEXPECTED_PARAMETERS);
}


/*****************************************************************************/
/*!
 *  \brief   Verify if date/time is valid
 *
 *      Checks if the date and time contained in Message is valid. Each
 *      single component of the date/time is checked. The check is leap
 *      year aware. If the data/time is valid, NO_ERROR is returned.
 *      If not valid, an error is returned.
 *
 *  \iparam  Message = Message containing date/time 
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t bmVerifyDateTime (CanMessage_t *Message) {

    if (Message != NULL) {

        const UInt8 *Date = Message->Data;

        if (Date[HOUR] > 23 || Date[MINUTE] > 59 || Date[SECOND] > 59) {
            return (E_BAD_TIME_FORMAT);
        }
        if (Date[MONTH] > 12 || Date[MONTH] < 1 || Date[DAY] < 1) {
            return (E_BAD_DATE_FORMAT);
        }
        if (Date[DAY] > bmGetMonthLength(Date[DAY], Date[MONTH])) {
            return (E_BAD_DATE_FORMAT);
        }
        if ((Date[MILLI] << 8) + Date[MILLI+1] >= 1000) {
            return (E_BAD_TIME_FORMAT);
        }
        return (NO_ERROR);
    }
    return (E_PARAMETER_OUT_OF_RANGE);
}


/*****************************************************************************/
/*!
 *  \brief   Get milliseconds of the day
 *
 *      Returns the milliseconds of the day. If the real time clock is not
 *      running, 0 is returned.
 *
 *  \return  Milliseconds of the day
 *
 ****************************************************************************/

UInt32 bmGetSecondsOfTheDay (void) {

    UInt32 Milliseconds = 0;
    DateTime_t Time;

    if (halRtcReadDate (&Time) == NO_ERROR) {
        Milliseconds = 
            Time.Seconds * 1000 +
            Time.Minutes * 1000 * 60 +
            Time.Hours   * 1000 * 60 * 60 +
            Time.Millis;
    }
    return (Milliseconds);
}


/*****************************************************************************/
/*!
 *  \brief   Get length of a certain month
 *
 *      Returns the length in days of the selected Month. Year is required
 *      for leap year detection. If Month is outside its valid range [1..12]
 *      0 is returned.
 *
 *  \iparam  Year  = Year the month belongs to
 *  \iparam  Month = Month number [1...12]
 *
 *  \return  Length of month [in days]
 *
 ****************************************************************************/

static UInt32 bmGetMonthLength (UInt32 Year, UInt32 Month) {

    const UInt8 MonthLength[12] = { 
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 
    };

    if (Month == 2 && bmIsLeapYear(Year)) {
        return (MonthLength[Month-1] + 1);
    }
    if (Month > 0 && Month <= 12) {
        return (MonthLength[Month-1]);
    }
    return (0);
}


/*****************************************************************************/
/*!
 *  \brief   Checks if year is a leap year
 *
 *      Checks if a given year is a leap year. Returns TRUE is case of a
 *      leap year or FALSE in the opposite case.
 *
 *  \iparam  Year = Year to check
 *
 *  \return  TRUE: Leap year, FALSE: Not a leap year
 *
 ****************************************************************************/

Bool bmIsLeapYear (UInt32 Year) {

    if (((Year % 100) != 0 && (Year % 4) == 0) ||
         (Year % 400) == 0) {
        return (TRUE);
    }
    return (FALSE);

}


/*****************************************************************************/
/*!
 *  \brief   Initializes the time module
 *
 *      Initializes this module by registering CAN messages to be handled by
 *      this module, to the message dispatcher. Initializes the system tick
 *      timer.
 *
 *      This function is called once during startup.
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t bmInitTimeModule (void) {

    static bmCallbackEntry_t Commands[] = {
        { MSG_SYS_SET_SYSCLOCK, bmSetSysClock },
        { MSG_SYS_SET_REALTIME, bmSetDateTime },
        { MSG_SRV_REQ_REALTIME, bmGetDateTime }
    };
    return (canRegisterMessages (0, Commands, ELEMENTS(Commands)));
}

//****************************************************************************/

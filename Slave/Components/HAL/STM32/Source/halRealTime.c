/****************************************************************************/
/*! \file halRealTime.c
 * 
 *  \brief  Real Time Clock
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
 *      The real time clock is availaible in any case, no matter if a
 *      separate RTC crystal is attached to the processor or not. If no 
 *      extra crystal is present, the main oscillator is used as clock
 *      source. In case of a dedicated crystal the real time clock may 
 *      continue to run during power-off if a batterie backup voltage is 
 *      connected to the processor. If this is not the case, the real  
 *      time clock must be adjusted at least once after power up.
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

#include "Global.h"
#include "halCommon.h"
#include "halError.h"
#include "halClocks.h"
#include "halMain.h"
#include "halRealTime.h"


//****************************************************************************/
// Private Constants and Macros
//****************************************************************************/

#define PVD_MIN_VOLTAGE   2200      //!< Minimal detectable voltage
#define PVD_MAX_VOLTAGE   2900      //!< Maximal detectable voltage

#define PWR_CR_DBP        0x0100    //!< Disable backup write protection
#define PWR_CR_PLS        0x00E0    //!< PVD level selection
#define PWR_CR_PVDE       0x0010    //!< Power voltage detector enable

#define PWR_CSR_PVDO      0x0004    //!< PVD output state
#define PWR_CSR_SBF       0x0002    //!< Standby flag
#define PWR_CSR_WUF       0x0001    //!< Wakeup flag

#define RTC_CRL_RTOFF     0x0020    //!< RTC operation OFF flag
#define RTC_CRL_CNF       0x0010    //!< RTC configuration flag
#define RTC_CRL_RSF       0x0008    //!< RTC registers synchronized flag
#define RTC_CRL_OWF       0x0004    //!< RTC overflow flag
#define RTC_CRL_ALRF      0x0002    //!< RTC alarm flag
#define RTC_CRL_SECF      0x0001    //!< RTC second flag

#define RTC_MIN_YEAR      2000      //!< RTC counter starts with 2000
#define RTC_MAX_YEAR      2135      //!< RTC counter ends with 2135

#define BKP_DATE_VALID    0x8000    //!< Date valid indicator in backup store
#define BKP_DATE_OFFSET   0x7FFF    //!< Date/time millisecond offset


//****************************************************************************/
// Private Type Definitions
//****************************************************************************/

//! Real Time Clock Registers (only low 16 bits of the registers are used)
typedef volatile struct {
    UInt32 CRH;           //!< RTC control register high 
    UInt32 CRL;           //!< RTC control register low
    UInt32 PRLH;          //!< RTC prescaler load register high
    UInt32 PRLL;          //!< RTC prescaler load register low
    UInt32 DIVH;          //!< RTC prescaler divider register high
    UInt32 DIVL;          //!< RTC prescaler divider register low
    UInt32 CNTH;          //!< RTC counter register high
    UInt32 CNTL;          //!< RTC counter register low
    UInt32 ALRH;          //!< RTC alarm register high
    UInt32 ALRL;          //!< RTC alarm register low
} RtcRegFile_t;

//! Backup Domain Registers (only low 16 bits of the registers used)
typedef volatile struct {
    UInt32 Reserved;      //!< Reserved
    UInt32 DATA[10];      //!< Backup data registers
    UInt32 RTCCR;         //!< RTC clock calibration register
    UInt32 CR;            //!< Backup control register
    UInt32 CSR;           //!< Backup control/status register
} BkpRegFile_t;

//! Layout of processor internal power monitor registers 
typedef volatile struct {
    UInt32 CR;            //!< Power control register
    UInt32 CSR;           //!< Power control/status register
} PwrRegFile_t;

//! Register select IDs used by halRtcRead/Write
typedef enum {
    RTC_REG_COUNTER,      //!< Select counter register
    RTC_REG_ALARM,        //!< Select alarm register
    RTC_REG_DIVIDER,      //!< Select divider register
    RTC_REG_PRESCALER     //!< Select prescaler register
} RtcRegID_t;


//****************************************************************************/
// Private Variables
//****************************************************************************/

//! Hardware registers of the internal real time clock */
static RtcRegFile_t *RTC = (RtcRegFile_t*) 0x40002800;

//! Hardware registers of the internal backup storage */
static BkpRegFile_t *BKP = (BkpRegFile_t*) 0x40006C00;

//! Hardware registers of the internal power monitor unit */
static PwrRegFile_t *PWR = (PwrRegFile_t*) 0x40007000;

static UInt32 TimeDivider;   //!< Clock divider


//****************************************************************************/
// Private Function Prototypes
//****************************************************************************/

static Error_t halRtcCalcDate    (DateTime_t *Date, UInt32 SerialDate);
static Error_t halRtcCalcTime    (DateTime_t *Time, UInt32 SerialTime);

static UInt32  halRtcGetRegister (RtcRegID_t RegisterID);
static void    halRtcSetRegister (RtcRegID_t RegisterID, UInt32 Value);

static UInt32  halRtcMonthLength (UInt32 Year, UInt32 Month);
static Bool    halRtcIsLeapYear  (UInt32 Year);


/*****************************************************************************/
/*!
 *  \brief   Write date/time to real time clock
 *
 *      Writes the supplied date and time to the real time clock. The year 
 *      must be 2000 or higher. The Weekday is ignored.
 *
 *  \iparam  Date = Structure containing date/time
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halRtcWriteDate (const DateTime_t *Date) {

    UInt32 Month;
    UInt32 Year;

    if (Date != NULL) {
        UInt32 NumberOfDays = Date->Day;
        UInt32 NumberOfSecs;

        if (Date->Year < RTC_MIN_YEAR || Date->Year > RTC_MAX_YEAR) {
            return (E_RTC_YEAR_OUT_OF_RANGE);
        }
        for (Month=1; Month < Date->Month; Month++) {
            NumberOfDays += halRtcMonthLength(Date->Year, Month);
        }
        for (Year = RTC_MIN_YEAR; Year < Date->Year; Year++) {
            NumberOfDays += halRtcIsLeapYear(Year) ? 366 : 365;
        }
        NumberOfSecs = 
            Date->Seconds + Date->Minutes * 60 + Date->Hours * 3600;

        halRtcSetRegister (RTC_REG_COUNTER, 
            NumberOfDays * DAY_IN_SECONDS + NumberOfSecs);

        BKP->DATA[0] = Date->Millis | BKP_DATE_VALID;
        return (NO_ERROR);
    }
    return (E_INVALID_PARAMETER);
}
   

/*****************************************************************************/
/*!
 *  \brief   Read date/time from real time clock
 *
 *      Returns the actual date and time from the real time clock. The date
 *      is copied into the data structure pointed to by Date. If the date
 *      is not yet set, an error is returned.
 *
 *  \oparam  Date = Pointer to structure to return date/time
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halRtcReadDate (DateTime_t *Date) {

    const UInt16 TimeOffset = BKP->DATA[0] & BKP_DATE_OFFSET;

    if (BKP->DATA[0] & BKP_DATE_VALID) {

        // Divider counts down and is therefore substracted from 1000ms
        UInt64 SerialDate = 
            (UInt64) halRtcGetRegister(RTC_REG_COUNTER) * 1000 -
            (UInt64) halRtcGetRegister(RTC_REG_DIVIDER) * 1000 / TimeDivider + 
            (UInt64) TimeOffset + 1000;
    
        halRtcCalcDate (Date, SerialDate / DAY_IN_MILLISECONDS);
        halRtcCalcTime (Date, SerialDate % DAY_IN_MILLISECONDS);
            
        return (NO_ERROR);
    }
    return (E_RTC_NOT_RUNNING);
}


/*****************************************************************************/
/*!
 *  \brief   Convert serial date to date
 *
 *      Converts the serial date into the individual date components (year,
 *      month, day, and weekday) and returns them in Date. The time part of 
 *      Date is left unchanged. SerialDate must contain the number of days 
 *      elapsed since 1.1.2000.
 *
 *  \oparam  Date       = Pointer to structure to return date
 *  \iparam  SerialDate = Serial date 
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halRtcCalcDate (DateTime_t *Date, UInt32 SerialDate) {

    UInt32 Length;
    UInt32 Month;
    UInt32 Year;

    if (Date != NULL) {
        UInt32 NumberOfDays = SerialDate;

        Date->WeekDay = (NumberOfDays + 4) % 7;
        for (Year = RTC_MIN_YEAR; Year <= RTC_MAX_YEAR; Year++) {
            Length = halRtcIsLeapYear(Year) ? 366 : 365;
    
            if (NumberOfDays <= Length) {
                break;
            }
            NumberOfDays -= Length;
        }
        Date->Year = Year;
    
        for (Month=1; Month <= 12; Month++) {
            Length = halRtcMonthLength(Year, Month);
            if (NumberOfDays <= Length) {
                break;
            }
            NumberOfDays -= Length;
        }
        Date->Month = Month;
        Date->Day   = NumberOfDays;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Convert serial time to time
 *
 *      Converts the serial time into the individual time components (hours,
 *      minutes, seconds, and milliseconds) and returns them in Time. The 
 *      date elements of Time are left unchanged. SerialTime must contain 
 *      the number of milliseconds of the day.
 *
 *  \oparam  Time       = Pointer to structure to return time
 *  \iparam  SerialTime = Serial time (ms of the day) 
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

static Error_t halRtcCalcTime (DateTime_t *Time, UInt32 SerialTime) {

    if (Time != NULL) {
        Time->Millis = SerialTime % 1000;
        SerialTime /= 1000;

        Time->Seconds = SerialTime % 60;
        SerialTime /= 60;
    
        Time->Minutes = SerialTime % 60;
        SerialTime /= 60;
    
        Time->Hours = SerialTime % 24;
    }
    return (NO_ERROR);
}


/*****************************************************************************/
/*!
 *  \brief   Get length of a certain month
 *
 *      Returns the length in days of the selected Month. Year is required
 *      for leap year detection. If Month is outside its valid range 1..12
 *      zero is returned.
 *
 *  \iparam  Year  = Year the month belongs to
 *  \iparam  Month = Month number [1...12]
 *
 *  \return  Length of month [in days]
 *
 ****************************************************************************/

static UInt32 halRtcMonthLength (UInt32 Year, UInt32 Month) {

    const UInt8 MonthLength[12] = { 
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 
    };

    if (Month == 2 && halRtcIsLeapYear(Year)) {
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

static Bool halRtcIsLeapYear (UInt32 Year) {

    if (((Year % 100) != 0 && (Year % 4) == 0) ||
         (Year % 400) == 0) {
        return (TRUE);
    }
    return (FALSE);
}


/*****************************************************************************/
/*!
 *  \brief   Set real time clock register
 *
 *      Writes Value to the real-time-clock register identified by RegisterID.
 *      The follwing registers can be written to:
 *
 *      - Counter register (holding actual date/time)
 *      - Alarm register (holding alarm date/time)
 *      - Prescale register (clock prescaler)
 *
 *  \iparam  RegisterID = Register to write to
 *  \iparam  Value      = Value to write into selected register
 *
 *  \return  Nothing
 *
 ****************************************************************************/

static void halRtcSetRegister (RtcRegID_t RegisterID, UInt32 Value) {

    const UInt32 HiWord = (Value >> 16) & MAX_UINT16;
    const UInt32 LoWord = (Value & MAX_UINT16);

    while (!(RTC->CRL & RTC_CRL_RTOFF)) {}

    RTC->CRL |= RTC_CRL_CNF;
    if (RegisterID == RTC_REG_COUNTER) {
        RTC->CNTH = HiWord;
        RTC->CNTL = LoWord;
    }
    else if (RegisterID == RTC_REG_ALARM) {
        RTC->ALRH = HiWord;
        RTC->ALRL = LoWord;
    }
    else if (RegisterID == RTC_REG_PRESCALER) {
        RTC->PRLH = HiWord;
        RTC->PRLL = LoWord;
    }
    RTC->CRL &= ~RTC_CRL_CNF;
}


/*****************************************************************************/
/*!
 *  \brief   Get real time clock register
 *
 *      Reads the real-time-clock register identified by RegisterID and
 *      returns it's content. The follwing registers can be read:
 *
 *      - Counter register (holding actual date/time)
 *      - Alarm register (holding alarm date/time)
 *      - Prescaler divider (clock prescaler)
 *
 *  \iparam  RegisterID = Register to read from
 *
 *  \return  Register content
 *
 ****************************************************************************/

static UInt32 halRtcGetRegister (RtcRegID_t RegisterID) {

    UInt32 HiWord = 0;
    UInt32 LoWord = 0;

    while (~RTC->CRL & (RTC_CRL_RTOFF | RTC_CRL_RSF)) {}

    if (RegisterID == RTC_REG_COUNTER) {
        HiWord = RTC->CNTH;
        LoWord = RTC->CNTL;
    }
    else if (RegisterID == RTC_REG_DIVIDER) {
        HiWord = RTC->DIVH;
        LoWord = RTC->DIVL;
    }
    else if (RegisterID == RTC_REG_ALARM) {
        HiWord = RTC->ALRH;
        LoWord = RTC->ALRL;
    }
    return ((HiWord & MAX_UINT16) << 16) + (LoWord & MAX_UINT16);
}


//****************************************************************************/
/*!
 *  \brief   Get power supply status
 *
 *      Returns the actual status of the processors core supply voltage.
 *      If the voltage falls below a configured minimal level, FALSE is
 *      returned. As long as voltage is sufficient, TRUE is returned.
 *
 *      Uses the processor internal hardware power monitor. If power
 *      monitor is not enabled, TRUE is returned.
 *
 *  \return  Core power status (True: ok, False: too low)
 *
 ****************************************************************************/

Bool halPowerGood (void) {

    if (PWR->CR & PWR_CR_PVDE) {
        if (PWR->CSR & PWR_CSR_PVDO) {
            return (FALSE);
        }
    }
    return (TRUE);
}

 
/*****************************************************************************/
/*!
 *  \brief   Initialize real time clock module
 *
 *      Initializes the real time clock module. This includes setting up the
 *      real time clock and the processor internal power detector hardware. 
 *
 *      The power detected is programmed to flag Power Low if the processors
 *      core voltage falls more than 10% below the configured voltage.
 *      The core voltage must be configured by setting the variable
 *      halProcessorVoltage to the nominal processor voltage [mV].
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/

Error_t halRtcInit (void) {

    UInt32 MinVoltage = halProcessorVoltage * 10 / 9;

    if (halGetInitState() == INIT_IN_PROGRESS) {

        halPeripheralClockEnable (PERIPHERAL_PWR, ON);
        halPeripheralClockEnable (PERIPHERAL_BKP, ON);
        PWR->CR = PWR_CR_DBP;

        // Setup and enable voltage detector
        if (MinVoltage >= PVD_MIN_VOLTAGE) {
            if (MinVoltage > PVD_MAX_VOLTAGE) {
                MinVoltage = PVD_MAX_VOLTAGE;
            }
            // Setup detect voltage and enable detector
            PWR->CR |= (((MinVoltage - PVD_MIN_VOLTAGE) / 100) << 5) & PWR_CR_PLS;
            PWR->CR |= PWR_CR_PVDE;
        }
        // Reset RTC resynchonization flag
        while (!(RTC->CRL & RTC_CRL_RTOFF)) {}
        RTC->CRL &= ~RTC_CRL_RSF;

        // Setup realtime clock prescaler (if not already done)
        TimeDivider = halGetRealTimeClockRate();
        halRtcSetRegister(RTC_REG_PRESCALER, TimeDivider-1);

    }
    return (NO_ERROR);
}

//****************************************************************************/

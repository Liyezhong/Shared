// ================================================================================================
/*! @file Platform/Master/Components/ExternalUtilities/Test/TestTime.cpp
 *
 *  @brief
 *  Some very rudimentary tests for ExternalUtilities::Time.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2011-01-31
 *  $Author:    $ Martin Weitzel
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *
 * @note
 * This is not a real test.
 * Rather it demonstrates how to use some library classes from Boost.Date_Time
 *
 * ================================================================================================
*/
#include <QTest>

#include "ExternalUtilities/Include/_ExternalUtilities.h"

namespace ExternalUtilities {
namespace UnitTest {

    /*!
     * Unit Test for Class Time (same as boost::posix_time::date_time).
     *
     * (For further information see source code.)
    */
    class TestTime: public QObject {

        Q_OBJECT

    private slots:

        /**
         * Default construction.
         *
         * This results in an invalid ExternalUtilities::Time (= not a date time).
        */
        void Construction() {
            const Time t;

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QVERIFY(    t.is_not_a_date_time()                                          );
            // --------------------------------------------------------------------------
        }

        /**
         * Construct ExternalUtilities::Time from invalid ExternalUtilities::Gregorian.
         *
         * This results in an invalid ExternalUtilities::Time (= not a date time).
        */
        void Construction_from_invalid_GregorianDate() {
            const GregorianDate invalid_gd;
            const Time invalid_t(invalid_gd);

            const GregorianDate valid_gd(2010, 3, 22);
            const Time valid_t(valid_gd);

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QVERIFY(    invalid_t.is_not_a_date_time()                                  );
            QVERIFY(  ! valid_t.is_not_a_date_time()                                    );
            // --------------------------------------------------------------------------
        }

        /**
         * Construct ExternalUtilities::Time from valid ExternalUtilities::Gregorian date.
         *
         * This results in a valid ExternalUtilities::Time for the time given (= day start of day at midnight)
        */
        void Construction_from_GregorianDate() {
            const Time t(GregorianDate(2010, 3, 22));

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QCOMPARE(   t.date().year()                     , greg_year(2010)           );
            QCOMPARE(   t.date().month()                    , greg_month(3)             );
            QCOMPARE(   t.date().day()                      , greg_day(22)              );
            QCOMPARE(   t.time_of_day().hours()             , 0                         );
            QCOMPARE(   t.time_of_day().minutes()           , 0                         );
            QCOMPARE(   t.time_of_day().seconds()           , 0                         );
            // --------------------------------------------------------------------------
        }

        /**
         * Construct ExternalUtilities::Time from valid ExternalUtilities::Gregorian date plus daytime offset.
         *
         * This results in a valid ExternalUtilities::Time for the time given (= time offset within given day).
        */
        void ConstructionFrom_from_GregorianDate_plus_HMS_offset() {
            const Time t(GregorianDate(2010, 3, 22),
                             hours(4) + minutes(7) + seconds(11));

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QCOMPARE(   t.date().year()                     , greg_year(2010)           );
            QCOMPARE(   t.date().month()                    , greg_month(3)             );
            QCOMPARE(   t.date().day()                      , greg_day(22)              );
            QCOMPARE(   t.time_of_day().hours()             , 4                         );
            QCOMPARE(   t.time_of_day().minutes()           , 7                         );
            QCOMPARE(   t.time_of_day().seconds()           , 11                        );
            // --------------------------------------------------------------------------
        }

        /**
         * Construct ExternalUtilities::Time from valid ExternalUtilities::Gregorian date plus overflowing daytime offset.
         *
         * This results in a valid ExternalUtilities::Time for the time given (= given day plus time offset).
         *
         * @note
         * This is demonstration that the time offset may be chosen arbitrary and overflows as necessary.
        */
        void ut_Construction_from_GregorianDate_plus_HMS_24hrOverflow() {
            const Time t(GregorianDate(2010, 3, 22),
                             hours(28) + minutes(7) + seconds(11));
            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QCOMPARE(   t.date().year()                     , greg_year(2010)           );
            QCOMPARE(   t.date().month()                    , greg_month(3)             );
            QCOMPARE(   t.date().day()                      , greg_day(23)              );
            QCOMPARE(   t.time_of_day().hours()             , 4                         );
            QCOMPARE(   t.time_of_day().minutes()           , 7                         );
            QCOMPARE(   t.time_of_day().seconds()           , 11                        );
            // --------------------------------------------------------------------------
        }

        /**
         * Construct ExternalUtilities::Time ExternalUtilities:;:GregorianDate with invalid year, month, day specification.
         *
         * This will throw exceptions (as documented), depending on the kind of error.
        */
        void Construct_from_bad_year_month_day_specification() {

            try {
                Time t(GregorianDate(2010, 0, 1));
                QVERIFY2(false, "expected exception not thrown");
            }
            catch (bad_month &) {
                QVERIFY(true);
            }
            try {
                Time t(GregorianDate(2010, 1, 0));
                QVERIFY2(false, "expected exception not thrown");
            }
            catch (bad_day_of_month &) {
                // SUCCEED();
            }

        }

        /**
         * Convert ExternalUtilities::Time into an 'ymd' struct.
         *
         * A 'ymd'-struct allows access to the individual fields:
         *  - year (20xx for dates in this century)
         *  - month (1..12 as in calendar)
         *  - day (1..28/30/31 depending on month)
        */
        void Conversion_to_struct_ymd() {
            const Time t(GregorianDate(2010, 3, 22), hours(4) + minutes(7) + seconds(11));
            const greg_year_month_day ymd(t.date().year_month_day());

            // ---------ACTUAL--------------------,--EXPECTED----------------------------
            QCOMPARE(   ymd.year                  , greg_year(2010)                     );
            QCOMPARE(   ymd.month                 , greg_month(3)                       );
            QCOMPARE(   ymd.day                   , greg_day(22)                        );
            // -------------------------------------------------------------------------
        }

        /**
         * Convert ExternalUtilities::Time into classical tm-struct.
         *
         * Note that by convention rules for specifiying years, months ans days are different.
         * (See comments source code below.)
        */
        void ut_Conversion_to_struct_tm() {
            const struct tm t= to_tm(Time(GregorianDate(2010, 3, 22),
                                     hours(4) + minutes(7) + seconds(11)));

            // ---------ACTUAL--------------------,--EXPECTED---
            QCOMPARE(   t.tm_year                 , 110        ); // "year 0" is 1900
            QCOMPARE(   t.tm_mon                  , 2          ); // "january" is 0
            QCOMPARE(   t.tm_mday                 , 22         );
            QCOMPARE(   t.tm_wday                 , 1          ); // "sunday" is 0
            QCOMPARE(   t.tm_hour                 , 4          );
            QCOMPARE(   t.tm_min                  , 7          );
            QCOMPARE(   t.tm_sec                  , 11         );
            // -------------------------------------------------
        }

#if 0
        /**
         * Currently excluded because of problems with target platform
        */
        void ut_Conversion_to_iso_string() {
            const GregorianDate gd(2010, 3, 22);
            const Time t1(gd);
            const Time t2(gd, hours(28) + minutes(7) + seconds(11));

            // ---------ACTUAL--------------------,--EXPECTED----------------------------
            QCOMPARE(   to_iso_string(t1)         , std::string("20100322T000000")      );
            QCOMPARE(   to_iso_string(t2)         , std::string("20100323T040711")      );
            // --------------------------------------------------------------------------
        }
#endif

    };

}} // EONS ::ExternalUtilities::UnitTest

QTEST_MAIN(ExternalUtilities::UnitTest::TestTime)
#include "TestTime.moc"

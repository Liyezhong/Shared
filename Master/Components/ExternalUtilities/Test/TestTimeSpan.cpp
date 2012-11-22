// ================================================================================================
/*! @file Platform/Master/Components/ExternalUtilities/Test/TestTimeSpan.cpp
 *
 *  @brief
 *  Some very rudimentary tests for ExternalUtilities::GregorianDate.
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
#include <QtTest/QtTest>

#include "ExternalUtilities/Include/_ExternalUtilities.h"

namespace ExternalUtilities {
namespace UnitTest {

    /*!
     * Unit Test for Class TimeSpan (same as boost::posix_time::time_duration).
     *
     * (For further information see source code.)
    */
    class TestTimeSpan: public QObject {

        Q_OBJECT

    private slots:

        /**
         * Default construction.
         *
         * Results in a time span of length zero.
         *
         * @note
         * This is different from default construction of a ExternalUtilities::Time.
         * (The latter would renturn an invalid date time.)
        */
        void Construction() {
            const TimeSpan t;

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QVERIFY(  ! t.is_not_a_date_time()                                          );
            QVERIFY(  ! t.is_special()                                                  );
            QVERIFY(  ! t.is_neg_infinity()                                             );
            QVERIFY(  ! t.is_pos_infinity()                                             );
            // --------------------------------------------------------------------------
        }

        /**
         * Construct infinite ExternalUtilities::TimeSpan-s.
         *
         * Special values are constructed with c'tor arguments from a special enumeration:
         *  - neg_infin = a time span that compares lesser than all others;
         *  - pos_infin = a time span that compares greater than all other;
        */
        void Construction_from_special_values() {
            const TimeSpan t_neg_infin(neg_infin);
            const TimeSpan t_pos_infin(pos_infin);

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QVERIFY(  ! t_neg_infin.is_not_a_date_time()                                );
            QVERIFY(    t_neg_infin.is_special()                                        );
            QVERIFY(    t_neg_infin.is_neg_infinity()                                   );
            QVERIFY(  ! t_neg_infin.is_pos_infinity()                                   );
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            QVERIFY(  ! t_pos_infin.is_not_a_date_time()                                );
            QVERIFY(    t_pos_infin.is_special()                                        );
            QVERIFY(  ! t_pos_infin.is_neg_infinity()                                   );
            QVERIFY(    t_pos_infin.is_pos_infinity()                                   );
            // --------------------------------------------------------------------------
        }

        /**
         * Construct ExternalUtilities::TimeSpan-s with micro-seconds resolution.
        */
        void Construct_with_microseconds_resolution() {
            const TimeSpan t;

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QCOMPARE(   TimeSpan::num_fractional_digits()   ,  static_cast<ushort>(6)   );
            // --------------------------------------------------------------------------
        }

        /**
         * Access field contents for a positive ExternalUtilities::TimeSpan.
         *
         * (Mostly containe for comparison with the field contents of negative time spans.)
        */
        void Positive_value_formatting() {
            const TimeSpan t(1, 2, 3, 456789);

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QVERIFY(  ! t.is_negative()                                                 ); 
            QCOMPARE(   t.total_seconds()                   ,  (1*60 + 2)*60 + 3        );
            QCOMPARE(   t.fractional_seconds()              ,  456789LL                 );
            QCOMPARE(   t.fractional_seconds() / (10*1000)  ,  45LL                     );
            // --------------------------------------------------------------------------
        }

        /**
         * Access field contents for a negative ExternalUtilities::TimeSpan.
         *
         * This shows that all numeric fields are negative, including the fractions.
        */
        void Negative_value_formatting() {
            const TimeSpan t(-TimeSpan(1, 2, 3, 456789));

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QVERIFY(    t.is_negative()                                                 ); 
            QCOMPARE(   t.total_seconds()                   , -((1*60 + 2)*60 + 3)      );
            QCOMPARE(   t.fractional_seconds()              , -456789LL                 );
            QCOMPARE(   t.fractional_seconds() / (10*1000)  , -45LL                     );
            // --------------------------------------------------------------------------
        }

        /**
         * Helper to demonstrate formatting with sub-seconds.
         *
         * @iparam  tSpan -- time span to print.
         * @returns QString formatted time span.
         * @throws  (never throws)
         *
         * (Only written to try and verify the strategy chosen qin some other module.)
        */
        static
        QString homeGrownFormatting(const TimeSpan tSpan) {
            return QString("%1.%2")
                    .arg(tSpan.total_seconds())
                    .arg(((tSpan.is_negative()
                            ? -tSpan.fractional_seconds()
                            : tSpan.fractional_seconds()) + 5*1000) / (10*1000)
                        );
        }

        /**
         * Format ExternalUtilities::TimeSpan showing sign for negative values only.
         *
         * (See tests for positive and negative value formatting to understand the challenge.)
        */
        void Demonstrate_home_grown_formatting() {
            const TimeSpan ts1(0, 0, 3, 456789);
            const TimeSpan ts2(0, 0, 7, 654321);
            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QCOMPARE(   homeGrownFormatting(ts1)            , QString("3.46")           );
            QCOMPARE(   homeGrownFormatting(-ts1)           , QString("-3.46")          );
            QCOMPARE(   homeGrownFormatting(ts2)            , QString("7.65")           );
            QCOMPARE(   homeGrownFormatting(-ts2)           , QString("-7.65")          );
            // --------------------------------------------------------------------------
        }
    };

}} // EONS ::ExternalUtilities::UnitTest

QTEST_MAIN(ExternalUtilities::UnitTest::TestTimeSpan)
#include "TestTimeSpan.moc"

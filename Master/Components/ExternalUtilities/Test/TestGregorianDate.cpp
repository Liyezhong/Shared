// ================================================================================================
/*! @file Platform/Master/Components/ExternalUtilities/Test/TestGregorianDate.cpp
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
#include <QTest>

#include "ExternalUtilities/Include/_ExternalUtilities.h"

namespace ExternalUtilities {
namespace UnitTest {

    /*!
     * Unit Test for Class GregorianDate (in principle boost::gregorian::date).
     *
     * (For further information see source code.)
    */
    class TestGregorianDate: public QObject {

        Q_OBJECT

    private slots:

        /**
         * Construction by default
         *
         * This results in an invalid ExternalUtilities::GregorianDate (= not a data  time).
        */
        void Construction() {
            GregorianDate gd;

            // ---------ACTUAL------------------------------,--EXPECTED------------------
            QVERIFY(    gd.is_not_a_date()                                              );
            // --------------------------------------------------------------------------

        }

        /**
         * Construction for date specifiied by year, month, and day.
         *
         * This results in a valid ExternalUtilities::GregorianDate for the given time point.
        */
        void Construction_from_year_month_day() {
            GregorianDate gd(2010, 3, 22);

            // ---------ACTUAL-----------------------,--EXPECTED-------------------------
            QCOMPARE(   gd.year()                    ,  greg_year(2010)                 );
            QCOMPARE(   gd.month()                   ,  greg_month(3)                   );
            QCOMPARE(   gd.day()                     ,  greg_day(22)                    );
            // --------------------------------------------------------------------------
        }

    };

}} // EONS ::ExternalUtilities::UnitTest

QTEST_MAIN(ExternalUtilities::UnitTest::TestGregorianDate)
#include "TestGregorianDate.moc"

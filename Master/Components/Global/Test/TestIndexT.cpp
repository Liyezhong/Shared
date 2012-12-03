// ================================================================================================
/*! @file Platform/Master/Components/Global/Test/TestIndexT.cpp
 *
 *  @brief
 *  Unit Tests for class Global::IndexT.
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
 * ontained are COMBINED unit tests for several instantiations of the Global::IndexT template.
 *
 * ================================================================================================
*/
#include <QTest>

#include "Global/Include/IndexT.h"

/*
 * The macro XS defined below is actually the fourth argument of the template instantiation.
 * (To clearly select default behavior, also the comma is added as part of this macro.)
 * All the common tests are written only once below.
 * Where a test has a different outcome depending on XS, there is a conditional compilation.
 * This also will make the DIFFERENCES clearly stand-out.
*/
#if defined(T_Default)
#define                     XS /*empty*/
#elif defined(T_0_Extra)
#define                     XS ,0
#elif defined(T_3_Extra)
#define                     XS ,3
#else
#error "must define: T_Default, T_0_Extra, or T_3_Extra"
#endif

namespace Global {
namespace UnitTest {

    /**
     * Example instatiation of the IndexT template.
     *
     * @note
     * It has a different outcome depending on the conditionally compiled section setting XS.
     * (For more details see comment above.)
    */
    class MyIndexType
        :public IndexT<3, 7, MyIndexType XS> {
    public:
        typedef IndexT<3, 7, MyIndexType XS> Super; //!< shorthand for own class
    // Note THIS ------------------------^^
    //          (explained in comment above)

    /*
     * Everything lifted into public so that everything can be tested.
    */
    public:

        /**
         * Error text for exception eventually thrown.
        */
        static
        const char Not_in_bounded_range[];

        /**
         * Default C'tor.
        */
        MyIndexType() : Super() {}      

        /**
         * Value C'tor.
         *
         * @iparam  iVal -- initial value.
        */
        MyIndexType(int iVal) : Super(iVal) {}

        // Enable features from base class
        //
        using Super::NotValid;
        using Super::ArraySize;
        using Super::ValidRange;
        using Super::intValue;
        using Super::delta;
        using Super::prev;
        using Super::next;
        using Super::operator++;
        using Super::operator--;
        using Super::zeroBasedOffset;
        using Super::eq_cmp;
        using Super::lt_cmp;
    };

    const char MyIndexType::Not_in_bounded_range[] = "MyIndexType not in bounded range";

    /**
     * Unit test the Global::IndexT
     *
     * (For further information see source code.)
    */
    class TestIndexT: public QObject {

        Q_OBJECT

    private slots:

        /**
         * Check for sane values of the compile time constants.
        */
        void CompilteTime_constants() {

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   static_cast<int>(MyIndexType::NotValid)         ,  2            );
            QCOMPARE(   static_cast<int>(MyIndexType::ValidRange)       ,  5            );
            // --------------------------------------------------------------------------

        }

        /**
         * Validate limits specified as tamplate instantiation arguments.
        */
        void Validate_limits() {

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   MyIndexType::first().intValue()                 ,  3            );
            QCOMPARE(   MyIndexType::last().intValue()                  ,  7            );
            // --------------------------------------------------------------------------

        }

        /**
         * Member function MyIndexType::checkValid.
        */
        void MF_checkValid() {

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QVERIFY(  ! MyIndexType::checkValid(0)                                      );
            QVERIFY(  ! MyIndexType::checkValid(1)                                      );
            QVERIFY(  ! MyIndexType::checkValid(2)                                      );
            QVERIFY(    MyIndexType::checkValid(3)                                      );
            QVERIFY(    MyIndexType::checkValid(4)                                      );
            QVERIFY(    MyIndexType::checkValid(5)                                      );
            QVERIFY(    MyIndexType::checkValid(6)                                      );
            QVERIFY(    MyIndexType::checkValid(7)                                      );
            QVERIFY(  ! MyIndexType::checkValid(8)                                      );
            QVERIFY(  ! MyIndexType::checkValid(9)                                      );
            // --------------------------------------------------------------------------

        }

        /**
         * Default and value construction.
        */
        void State_after_constraction() {

            MyIndexType x1;
            MyIndexType x2(4);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QVERIFY(  ! x1.isValid()                                                    );
            QVERIFY(    x2.isValid()                                                    );
            QCOMPARE(   x2.intValue()                                   ,  4            );
            // --------------------------------------------------------------------------

        }

        /**
         * Copy construction.
        */
        void Copy_ctor() {

            MyIndexType x1;
            MyIndexType x1_copy(x1);
            MyIndexType x2(4);
            MyIndexType x2_copy(x2);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QVERIFY(  ! x1_copy.isValid()                                               );
            QVERIFY(    x2_copy.isValid()                                               );
            QCOMPARE(   x2_copy.intValue()                              ,  4            );
            // --------------------------------------------------------------------------

        }

        /**
         * Assigment operation.
        */
        void Assignment() {

            MyIndexType x1;
            MyIndexType x1_assigned;
            x1_assigned = x1;
            MyIndexType x2(4);
            MyIndexType x2_assigned;
            x2_assigned = x2;

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QVERIFY(  ! x1_assigned.isValid()                                           );
            QVERIFY(    x2_assigned.isValid()                                           );
            QCOMPARE(   x2_assigned.intValue()                          ,  4            );
            // --------------------------------------------------------------------------

        }

        /**
         * Explictly setting invalid.
        */
        void Invalidation() {

            MyIndexType x1(4);
            x1.setInvalid();

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QVERIFY(  ! x1.isValid()                                                    );
            // --------------------------------------------------------------------------

        }

        /**
         * Delta calculation.
        */
        void MF_delta() {
            MyIndexType x1(4);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QVERIFY(  ! x1.delta(-100).isValid()                                        );
            QVERIFY(  ! x1.delta(-3).isValid()                                          );
            QVERIFY(  ! x1.delta(-2).isValid()                                          );
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            QCOMPARE(   x1.delta(-1).intValue()                          ,  3           );
            QCOMPARE(   x1.delta( 0).intValue()                          ,  4           );
            QCOMPARE(   x1.delta(+1).intValue()                          ,  5           );
            QCOMPARE(   x1.delta(+2).intValue()                          ,  6           );
            QCOMPARE(   x1.delta(+3).intValue()                          ,  7           );
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            QVERIFY(  ! x1.delta(+4).isValid()                                          );
            // --------------------------------------------------------------------------
        }

        /**
         * Prev calculation.
        */
        void MF_prev() {
            MyIndexType x1(4);

            // ---------ACTUAL-------------------------------------------,--EXPECTED-----
            QVERIFY(  ! x1.prev(100).isValid()                                          );
            QVERIFY(  ! x1.prev(3).isValid()                                            );
            QVERIFY(  ! x1.prev(2).isValid()                                            );
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            QCOMPARE(   x1.prev(1).intValue()                            ,  3           );
            QCOMPARE(   x1.prev().intValue()                             ,  3           );
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            QVERIFY(  ! x1.prev().prev().isValid()                                      );
            // --------------------------------------------------------------------------
        }

        /**
         * Next calculation.
        */
        void MF_next() {
            MyIndexType x1(4);

            // ---------ACTUAL-------------------------------------------,--EXPECTED-----
            QCOMPARE(   x1.next().intValue()                             ,  5           );
            QCOMPARE(   x1.next(1).intValue()                            ,  5           );
            QCOMPARE(   x1.next(2).intValue()                            ,  6           );
            QCOMPARE(   x1.next(3).intValue()                            ,  7           );
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            QVERIFY(  ! x1.next(4).isValid()                                            );
            QVERIFY(  ! x1.next(5).isValid()                                            );
            QVERIFY(  ! x1.next(100).isValid()                                          );
            //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            QCOMPARE(   x1.next().next().intValue()                      ,  6           );
            QCOMPARE(   x1.next().next().next().intValue()               ,  7           );
            QVERIFY(  ! x1.next().next().next().next().isValid()                        );
            // --------------------------------------------------------------------------
        }

        /**
         * Prefix decrement operator.
        */
        void PrefixDecrement() {

            MyIndexType x1(4);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   (--x1).intValue()                               ,  3            );
            QVERIFY(  ! (--x1).isValid()                                                );
            QCOMPARE(   (--x1).intValue()                               ,  7            );
            QCOMPARE(   (--x1).intValue()                               ,  6            );
            QCOMPARE(   (--x1).intValue()                               ,  5            );
            QCOMPARE(   (--x1).intValue()                               ,  4            );
            QCOMPARE(   (--x1).intValue()                               ,  3            );
            // --------------------------------------------------------------------------

        }

        /**
         * Postfix decrement operator.
        */
        void PostfixDecrement() {

            MyIndexType x1(4);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   (x1--).intValue()                               ,  4            );
            QCOMPARE(   (x1--).intValue()                               ,  3            );
            QVERIFY(  ! (x1--).isValid()                                                );
            QCOMPARE(   (x1--).intValue()                               ,  7            );
            QCOMPARE(   (x1--).intValue()                               ,  6            );
            QCOMPARE(   (x1--).intValue()                               ,  5            );
            QCOMPARE(   (x1--).intValue()                               ,  4            );
            // --------------------------------------------------------------------------

        }

        /**
         * Prefix increment operator.
        */
        void PrefixIncrement() {

            MyIndexType x1(4);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   (++x1).intValue()                               ,  5            );
            QCOMPARE(   (++x1).intValue()                               ,  6            );
            QCOMPARE(   (++x1).intValue()                               ,  7            );
            QVERIFY(  ! (++x1).isValid()                                                );
            QCOMPARE(   (++x1).intValue()                               ,  3            );
            QCOMPARE(   (++x1).intValue()                               ,  4            );
            QCOMPARE(   (++x1).intValue()                               ,  5            );
            // --------------------------------------------------------------------------

        }

        /**
         * Postfix increment operator.
        */
        void PostfixIncrement() {

            MyIndexType x1(4);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   (x1++).intValue()                               ,  4            );
            QCOMPARE(   (x1++).intValue()                               ,  5            );
            QCOMPARE(   (x1++).intValue()                               ,  6            );
            QCOMPARE(   (x1++).intValue()                               ,  7            );
            QVERIFY(  ! (x1++).isValid()                                                );
            QCOMPARE(   (x1++).intValue()                               ,  3            );
            QCOMPARE(   (x1++).intValue()                               ,  4            );
            // --------------------------------------------------------------------------

        }

        /**
         * Comparison operators.
        */
        void ComparisonOperators() {

            MyIndexType x0;
            MyIndexType x1(4);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   x0 == x1                                        ,  false        );
            QCOMPARE(   x0 != x1                                        ,  true         );
            // --------------------------------------------------------------------------

            MyIndexType x2(4);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   x1 == x2                                        ,  true         );
            QCOMPARE(   x1 != x2                                        ,  false        );
            QCOMPARE(   x1 <  x2                                        ,  false        );
            QCOMPARE(   x1 <= x2                                        ,  true         );
            QCOMPARE(   x1 >= x2                                        ,  true         );
            QCOMPARE(   x1 >  x2                                        ,  false        );
            // --------------------------------------------------------------------------

            MyIndexType x3(6);

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   x2 == x3                                        ,  false        );
            QCOMPARE(   x2 != x3                                        ,  true         );
            QCOMPARE(   x2 <  x3                                        ,  true         );
            QCOMPARE(   x2 <= x3                                        ,  true         );
            QCOMPARE(   x2 >= x3                                        ,  false        );
            QCOMPARE(   x2 >  x3                                        ,  false        );
            // --------------------------------------------------------------------------

        }

        /**
         * Compile- time constant for array usable as array size.
        */
        void ArraySize() {

#if defined     (T_Default)

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   static_cast<int>(MyIndexType::ArraySize)        ,  6            );
            // --------------------------------------------------------------------------

#elif defined   (T_0_Extra)

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   static_cast<int>(MyIndexType::ArraySize)        ,  5            );
            // --------------------------------------------------------------------------

#elif defined   (T_3_Extra)

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   static_cast<int>(MyIndexType::ArraySize)        ,  8            );
            // --------------------------------------------------------------------------
#endif

        }

        /**
         * Member function to calculate zero based offset (to index inside classical array).
        */
        void MF_zeroBasedOffset() {

            MyIndexType x1(4);

#if defined     (T_Default)

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   x1.zeroBasedOffset<unsigned int>()              ,  2u           );
            QCOMPARE(   x1.zeroBasedOffset<int>()                       ,  2            );
            QCOMPARE(   x1.zeroBasedOffset(5u)                          ,  2u           );
            QCOMPARE(   x1.zeroBasedOffset(5)                           ,  2            );
            QCOMPARE(   x1.zeroBasedOffset(4)                           ,  2            );
            QCOMPARE(   x1.zeroBasedOffset(3)                           ,  2            );
            // --------------------------------------------------------------------------

#elif defined   (T_0_Extra)

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   x1.zeroBasedOffset<unsigned int>()              ,  1u           );
            QCOMPARE(   x1.zeroBasedOffset<int>()                       ,  1            );
            QCOMPARE(   x1.zeroBasedOffset(5u)                          ,  1u           );
            QCOMPARE(   x1.zeroBasedOffset(5)                           ,  1            );
            QCOMPARE(   x1.zeroBasedOffset(4)                           ,  1            );
            QCOMPARE(   x1.zeroBasedOffset(3)                           ,  1            );
            // --------------------------------------------------------------------------

#elif defined   (T_3_Extra)

            // ---------ACTUAL------------------------------------------,--EXPECTED------
            QCOMPARE(   x1.zeroBasedOffset<unsigned int>()              ,  4u           );
            QCOMPARE(   x1.zeroBasedOffset<int>()                       ,  4            );
            QCOMPARE(   x1.zeroBasedOffset(5u)                          ,  4u           );
            QCOMPARE(   x1.zeroBasedOffset(5)                           ,  4            );
            QCOMPARE(   x1.zeroBasedOffset(4)                           ,  4            );
            QCOMPARE(   x1.zeroBasedOffset(3)                           ,  4            );
            // --------------------------------------------------------------------------

#endif

        }

        /**
         * Helper to find out if an exception is actually thrown.
         *
         * @iparam  indexTypeInstance -- instance of the index type.
         * @iparam  zbOffsetValue -- argument value forwarded to zeroBasedOffset.
        */
        QString ensureThrow(
            const MyIndexType& indexTypeInstance,
            int zbOffsetValue
        ) {
            try {
                static_cast<void>(indexTypeInstance.zeroBasedOffset(zbOffsetValue));
                return QString();
            }
            catch (std::out_of_range& e) {
                return e.what();
            }
        }

        /**
         * Make sure out of range errors will be detected.
        */
        void Make_sure_zeroBasedOffset_throws() {

            MyIndexType x1(4);

#if defined     (T_Default)

            // ---------ACTUAL--------------,--EXPECTED--------------------------------------
            QCOMPARE(   ensureThrow(x1, 6)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 5)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 4)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 3)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 2)  ,  QString("MyIndexType not in bounded range")  );
            QCOMPARE(   ensureThrow(x1, 1)  ,  QString("MyIndexType not in bounded range")  );
            QCOMPARE(   ensureThrow(x1, 0)  ,  QString("MyIndexType not in bounded range")  );
            // ------------------------------------------------------------------------------

#elif defined (T_0_Extra)

            // ---------ACTUAL--------------,--EXPECTED--------------------------------------
            QCOMPARE(   ensureThrow(x1, 6)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 5)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 4)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 3)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 2)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 1)  ,  QString("MyIndexType not in bounded range")  );
            QCOMPARE(   ensureThrow(x1, 0)  ,  QString("MyIndexType not in bounded range")  );
            // ------------------------------------------------------------------------------

#elif defined (T_3_Extra)

            // ---------ACTUAL--------------,--EXPECTED--------------------------------------
            QCOMPARE(   ensureThrow(x1, 6)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 5)  ,  QString()                                    );
            QCOMPARE(   ensureThrow(x1, 4)  ,  QString("MyIndexType not in bounded range")  );
            QCOMPARE(   ensureThrow(x1, 3)  ,  QString("MyIndexType not in bounded range")  );
            QCOMPARE(   ensureThrow(x1, 2)  ,  QString("MyIndexType not in bounded range")  );
            QCOMPARE(   ensureThrow(x1, 1)  ,  QString("MyIndexType not in bounded range")  );
            QCOMPARE(   ensureThrow(x1, 0)  ,  QString("MyIndexType not in bounded range")  );
            // ------------------------------------------------------------------------------

#endif

        }

    };

}} // EONS ::Global::UnitTest

QTEST_MAIN(Global::UnitTest::TestIndexT)
#include "TestIndexT.moc"

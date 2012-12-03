/****************************************************************************/
/*! \file TestSharedPointer.cpp
 *
 *  \brief Implementation file for class SharedPointer.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-03-22
 *  $Author:    $ J.Bugariu
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/

#include <QTest>
#include <Global/Include/SharedPointer.h>

namespace Global {

bool Data1 = false; ///< Helper data.
bool Data2 = false; ///< Helper data.

/****************************************************************************/
/**
 * \brief Helper class for testing SharedPointer class.
 */
/****************************************************************************/
class SharedPointerHelper {
private:
    bool    *m_pData;       ///< Pointer to some helper data.
    /****************************************************************************/
    SharedPointerHelper();                                                  ///< Not implemented.
    SharedPointerHelper(const SharedPointerHelper &);                       ///< Not implemented.
    const SharedPointerHelper & operator = (const SharedPointerHelper &);   ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * Sets helper data to true.
     *
     * \param[in]   pData   Pointer to some helper data.
     */
    /****************************************************************************/
    SharedPointerHelper(bool *pData) :
        m_pData(pData)
    {
        Q_ASSERT(m_pData != NULL);
        *m_pData = true;
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     *
     * Sets helper data to false.
     */
    /****************************************************************************/
    ~SharedPointerHelper() {
        *m_pData = false;
    }
}; // end class SharedPointerHelper

typedef SharedPointer<SharedPointerHelper>  tSharedPtr;     ///< Typedef for used shared pointer.

/****************************************************************************/
/**
 * \brief Test class for SharedPointer class.
 */
/****************************************************************************/
class TestSharedPointer : public QObject {
    Q_OBJECT
private:
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     *
     * Here is the only place where we can test the default constructor.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
     *
     * Reset translator to a defined state.
     */
    /****************************************************************************/
    void init();
    /****************************************************************************/
    /**
     * \brief Called after each testfunction was executed.
     */
    /****************************************************************************/
    void cleanup();
    /****************************************************************************/
    /**
     * \brief Called after last testfunction was executed.
     */
    /****************************************************************************/
    void cleanupTestCase();
    /****************************************************************************/
    /**
     * \brief Test of constructors.
     */
    /****************************************************************************/
    void utTestConstructors();
    /****************************************************************************/
    /**
     * \brief Test of assignment operator.
     */
    /****************************************************************************/
    void utTestAssignmentOperator();
    /****************************************************************************/
    /**
     * \brief Test of methods.
     */
    /****************************************************************************/
    void utTestMethods();
}; // end class TestSharedPointer

/****************************************************************************/
void TestSharedPointer::initTestCase() {
}

/****************************************************************************/
void TestSharedPointer::init() {
}

/****************************************************************************/
void TestSharedPointer::cleanup() {
}

/****************************************************************************/
void TestSharedPointer::cleanupTestCase() {
}

/****************************************************************************/
void TestSharedPointer::utTestConstructors() {
    // default constructor
    tSharedPtr Ptr0;
    QVERIFY(Ptr0.m_PointerToRefCounter == NULL);

    // constructor with pointer
    {
        QCOMPARE(Data1, false);
        SharedPointerHelper *pHelper1 = new SharedPointerHelper(&Data1);
        QCOMPARE(Data1, true);
        tSharedPtr Ptr1(pHelper1);
        QVERIFY(Ptr1.m_PointerToRefCounter != NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter, QAtomicInt(1));
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_PointerToUserData, pHelper1);
    }
    QCOMPARE(Data1, false);

    // constructor with NULL pointer
    {
        tSharedPtr Ptr1(NULL);
        QVERIFY(Ptr1.m_PointerToRefCounter != NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter, QAtomicInt(1));
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_PointerToUserData, (SharedPointerHelper *)NULL);
    }

    // copy constructor
    {
        QCOMPARE(Data1, false);
        SharedPointerHelper *pHelper1 = new SharedPointerHelper(&Data1);
        QCOMPARE(Data1, true);
        tSharedPtr Ptr1(pHelper1);
        QVERIFY(Ptr1.m_PointerToRefCounter != NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter, QAtomicInt(1));
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_PointerToUserData, pHelper1);
        {
            tSharedPtr Ptr2(Ptr1);
            QCOMPARE(Data1, true);
            QCOMPARE(Ptr2.m_PointerToRefCounter,                        Ptr1.m_PointerToRefCounter);
            QCOMPARE(Ptr2.m_PointerToRefCounter->m_PointerToUserData,   Ptr1.m_PointerToRefCounter->m_PointerToUserData);
            QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));
        }
        QVERIFY(Ptr1.m_PointerToRefCounter != NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter, QAtomicInt(1));
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_PointerToUserData, pHelper1);
        QCOMPARE(Data1, true);
    }
    QCOMPARE(Data1, false);
}

/****************************************************************************/
void TestSharedPointer::utTestAssignmentOperator() {
    {
        tSharedPtr Ptr0;
        tSharedPtr Ptr3(NULL);
        tSharedPtr Ptr4;

        // first instance
        QCOMPARE(Data1, false);
        SharedPointerHelper *pHelper1 = new SharedPointerHelper(&Data1);
        QCOMPARE(Data1, true);
        tSharedPtr Ptr1(pHelper1);
        QVERIFY(Ptr1.m_PointerToRefCounter != NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter, QAtomicInt(1));
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_PointerToUserData, pHelper1);

        // second instance
        QCOMPARE(Data2, false);
        SharedPointerHelper *pHelper2 = new SharedPointerHelper(&Data2);
        QCOMPARE(Data2, true);
        tSharedPtr Ptr2(pHelper2);
        QVERIFY(Ptr2.m_PointerToRefCounter != NULL);
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter, QAtomicInt(1));
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_PointerToUserData, pHelper2);

        // assign 1
        Ptr0 = Ptr1;
        QCOMPARE(Data1, true);
        QCOMPARE(Data2, true);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        Ptr1.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_PointerToUserData,   Ptr1.m_PointerToRefCounter->m_PointerToUserData);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(1));

        // assign 2
        Ptr0 = Ptr2;
        QCOMPARE(Data1, true);
        QCOMPARE(Data2, true);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        Ptr2.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_PointerToUserData,   Ptr2.m_PointerToRefCounter->m_PointerToUserData);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(1));
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));

        // assign 3
        Ptr1 = Ptr2;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, true);
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        Ptr2.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_PointerToUserData,   Ptr2.m_PointerToRefCounter->m_PointerToUserData);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));

        // assign 4
        Ptr1 = Ptr2;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, true);
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        Ptr2.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_PointerToUserData,   Ptr2.m_PointerToRefCounter->m_PointerToUserData);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));

        // assign 5
        Ptr1 = Ptr1;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, true);
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        Ptr2.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_PointerToUserData,   Ptr2.m_PointerToRefCounter->m_PointerToUserData);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));

        // assign 6
        QCOMPARE(Ptr3.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(1));
        Ptr0 = Ptr3;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, true);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));
        QCOMPARE(Ptr3.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));

        // assign 7
        Ptr1 = Ptr3;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, true);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(1));
        QCOMPARE(Ptr3.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));

        // assign 8
        Ptr2 = Ptr3;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, false);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));
        QCOMPARE(Ptr2.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));
        QCOMPARE(Ptr3.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));

        // assign 9
        Ptr3 = Ptr3;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, false);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));
        QCOMPARE(Ptr2.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));
        QCOMPARE(Ptr3.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));

        // assign 10
        Ptr3 = Ptr2;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, false);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr0.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));
        QCOMPARE(Ptr2.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));
        QCOMPARE(Ptr3.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(4));

        // assign 11
        Ptr0 = Ptr4;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, false);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr1.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr2.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));
        QCOMPARE(Ptr3.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(3));

        // assign 12
        Ptr1 = Ptr4;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, false);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr2.m_PointerToRefCounter,                        Ptr3.m_PointerToRefCounter);
        QCOMPARE(Ptr2.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));
        QCOMPARE(Ptr3.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(2));

        // assign 13
        Ptr2 = Ptr4;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, false);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr2.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr3.m_PointerToRefCounter->m_RefCounter,          QAtomicInt(1));

        // assign 14
        Ptr4 = Ptr4;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, false);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr2.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);

        // assign 14
        Ptr4 = Ptr2;
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, false);
        QCOMPARE(Ptr0.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr1.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
        QCOMPARE(Ptr2.m_PointerToRefCounter,                        (tSharedPtr::PointerRefCounter *)NULL);
    }
    QCOMPARE(Data1, false);
    QCOMPARE(Data2, false);
}

/****************************************************************************/
void TestSharedPointer::utTestMethods() {

    QCOMPARE(Data1, false);
    QCOMPARE(Data2, false);
    {
        tSharedPtr Ptr0;
        tSharedPtr Ptr1;
        tSharedPtr Ptr2(NULL);
        tSharedPtr Ptr3(NULL);
        tSharedPtr Ptr4(NULL);
        tSharedPtr Ptr5(Ptr4);
        SharedPointerHelper *pHelper1 = new SharedPointerHelper(&Data1);
        tSharedPtr Ptr6(pHelper1);
        tSharedPtr Ptr7(Ptr6);
        SharedPointerHelper *pHelper2 = new SharedPointerHelper(&Data2);
        tSharedPtr Ptr8(pHelper2);
        tSharedPtr Ptr9(Ptr8);
        QCOMPARE(Data1, true);
        QCOMPARE(Data2, true);

        // IsNull
        QVERIFY(Ptr0.IsNull());
        QVERIFY(Ptr1.IsNull());
        QVERIFY(Ptr2.IsNull());
        QVERIFY(Ptr3.IsNull());
        QVERIFY(Ptr4.IsNull());
        QVERIFY(Ptr5.IsNull());
        QVERIFY(!Ptr6.IsNull());
        QVERIFY(!Ptr7.IsNull());
        QVERIFY(!Ptr8.IsNull());
        QVERIFY(!Ptr9.IsNull());

        // Operator "->"
        QVERIFY(Ptr0.operator ->() == NULL);
        QVERIFY(Ptr1.operator ->() == NULL);
        QVERIFY(Ptr2.operator ->() == NULL);
        QVERIFY(Ptr3.operator ->() == NULL);
        QVERIFY(Ptr4.operator ->() == NULL);
        QVERIFY(Ptr5.operator ->() == NULL);
        QVERIFY(Ptr6.operator ->() == pHelper1);
        QVERIFY(Ptr7.operator ->() == pHelper1);
        QVERIFY(Ptr8.operator ->() == pHelper2);
        QVERIFY(Ptr9.operator ->() == pHelper2);

        // GetPointerToUserData
        QVERIFY(Ptr0.GetPointerToUserData() == NULL);
        QVERIFY(Ptr1.GetPointerToUserData() == NULL);
        QVERIFY(Ptr2.GetPointerToUserData() == NULL);
        QVERIFY(Ptr3.GetPointerToUserData() == NULL);
        QVERIFY(Ptr4.GetPointerToUserData() == NULL);
        QVERIFY(Ptr5.GetPointerToUserData() == NULL);
        QVERIFY(Ptr6.GetPointerToUserData() == pHelper1);
        QVERIFY(Ptr7.GetPointerToUserData() == pHelper1);
        QVERIFY(Ptr8.GetPointerToUserData() == pHelper2);
        QVERIFY(Ptr9.GetPointerToUserData() == pHelper2);

        // Operator ==
        QVERIFY(  Ptr0 == Ptr0 );
        QVERIFY(  Ptr0 == Ptr1 );
        QVERIFY(!(Ptr0 == Ptr2));
        QVERIFY(!(Ptr0 == Ptr3));
        QVERIFY(!(Ptr0 == Ptr4));
        QVERIFY(!(Ptr0 == Ptr5));
        QVERIFY(!(Ptr0 == Ptr6));
        QVERIFY(!(Ptr0 == Ptr7));
        QVERIFY(!(Ptr0 == Ptr8));
        QVERIFY(!(Ptr0 == Ptr9));

        QVERIFY(  Ptr1 == Ptr0 );
        QVERIFY(  Ptr1 == Ptr1 );
        QVERIFY(!(Ptr1 == Ptr2));
        QVERIFY(!(Ptr1 == Ptr3));
        QVERIFY(!(Ptr1 == Ptr4));
        QVERIFY(!(Ptr1 == Ptr5));
        QVERIFY(!(Ptr1 == Ptr6));
        QVERIFY(!(Ptr1 == Ptr7));
        QVERIFY(!(Ptr1 == Ptr8));
        QVERIFY(!(Ptr1 == Ptr9));

        QVERIFY(!(Ptr2 == Ptr0));
        QVERIFY(!(Ptr2 == Ptr1));
        QVERIFY(  Ptr2 == Ptr2 );
        QVERIFY(!(Ptr2 == Ptr3));   // they point both to a NULL pointer but are not identical
        QVERIFY(!(Ptr2 == Ptr4));
        QVERIFY(!(Ptr2 == Ptr5));
        QVERIFY(!(Ptr2 == Ptr6));
        QVERIFY(!(Ptr2 == Ptr7));
        QVERIFY(!(Ptr2 == Ptr8));
        QVERIFY(!(Ptr2 == Ptr9));

        QVERIFY(!(Ptr3 == Ptr0));
        QVERIFY(!(Ptr3 == Ptr1));
        QVERIFY(!(Ptr3 == Ptr2));   // they point both to a NULL pointer but are not identical
        QVERIFY(  Ptr3 == Ptr3 );
        QVERIFY(!(Ptr3 == Ptr4));
        QVERIFY(!(Ptr3 == Ptr5));
        QVERIFY(!(Ptr3 == Ptr6));
        QVERIFY(!(Ptr3 == Ptr7));

        QVERIFY(!(Ptr4 == Ptr0));
        QVERIFY(!(Ptr4 == Ptr1));
        QVERIFY(!(Ptr4 == Ptr2));
        QVERIFY(!(Ptr4 == Ptr3));
        QVERIFY(  Ptr4 == Ptr4 );
        QVERIFY(  Ptr4 == Ptr5 );
        QVERIFY(!(Ptr4 == Ptr6));
        QVERIFY(!(Ptr4 == Ptr7));
        QVERIFY(!(Ptr4 == Ptr8));
        QVERIFY(!(Ptr4 == Ptr9));

        QVERIFY(!(Ptr5 == Ptr0));
        QVERIFY(!(Ptr5 == Ptr1));
        QVERIFY(!(Ptr5 == Ptr2));
        QVERIFY(!(Ptr5 == Ptr3));
        QVERIFY(  Ptr5 == Ptr4 );
        QVERIFY(  Ptr5 == Ptr5 );
        QVERIFY(!(Ptr5 == Ptr6));
        QVERIFY(!(Ptr5 == Ptr7));
        QVERIFY(!(Ptr5 == Ptr8));
        QVERIFY(!(Ptr5 == Ptr9));

        QVERIFY(!(Ptr6 == Ptr0));
        QVERIFY(!(Ptr6 == Ptr1));
        QVERIFY(!(Ptr6 == Ptr2));
        QVERIFY(!(Ptr6 == Ptr3));
        QVERIFY(!(Ptr6 == Ptr4));
        QVERIFY(!(Ptr6 == Ptr5));
        QVERIFY(  Ptr6 == Ptr6 );
        QVERIFY(  Ptr6 == Ptr7 );
        QVERIFY(!(Ptr6 == Ptr8));
        QVERIFY(!(Ptr6 == Ptr9));

        QVERIFY(!(Ptr7 == Ptr0));
        QVERIFY(!(Ptr7 == Ptr1));
        QVERIFY(!(Ptr7 == Ptr2));
        QVERIFY(!(Ptr7 == Ptr3));
        QVERIFY(!(Ptr7 == Ptr4));
        QVERIFY(!(Ptr7 == Ptr5));
        QVERIFY(  Ptr7 == Ptr6 );
        QVERIFY(  Ptr7 == Ptr7 );
        QVERIFY(!(Ptr7 == Ptr8));
        QVERIFY(!(Ptr7 == Ptr9));

        QVERIFY(!(Ptr8 == Ptr0));
        QVERIFY(!(Ptr8 == Ptr1));
        QVERIFY(!(Ptr8 == Ptr2));
        QVERIFY(!(Ptr8 == Ptr3));
        QVERIFY(!(Ptr8 == Ptr4));
        QVERIFY(!(Ptr8 == Ptr5));
        QVERIFY(!(Ptr8 == Ptr6));
        QVERIFY(!(Ptr8 == Ptr7));
        QVERIFY(  Ptr8 == Ptr8 );
        QVERIFY(  Ptr8 == Ptr9 );

        QVERIFY(!(Ptr9 == Ptr0));
        QVERIFY(!(Ptr9 == Ptr1));
        QVERIFY(!(Ptr9 == Ptr2));
        QVERIFY(!(Ptr9 == Ptr3));
        QVERIFY(!(Ptr9 == Ptr4));
        QVERIFY(!(Ptr9 == Ptr5));
        QVERIFY(!(Ptr9 == Ptr6));
        QVERIFY(!(Ptr9 == Ptr7));
        QVERIFY(  Ptr9 == Ptr8 );
        QVERIFY(  Ptr9 == Ptr9 );

        // Clear
        Ptr0.Clear();
        QVERIFY(Ptr0.m_PointerToRefCounter == NULL);
        Ptr1.Clear();
        QVERIFY(Ptr1.m_PointerToRefCounter == NULL);
        Ptr2.Clear();
        QVERIFY(Ptr2.m_PointerToRefCounter == NULL);
        Ptr3.Clear();
        QVERIFY(Ptr3.m_PointerToRefCounter == NULL);
        Ptr4.Clear();
        QVERIFY(Ptr4.m_PointerToRefCounter == NULL);
        Ptr5.Clear();
        QVERIFY(Ptr5.m_PointerToRefCounter == NULL);
        QCOMPARE(Data1, true);
        QCOMPARE(Data2, true);
        Ptr6.Clear();
        QVERIFY(Ptr6.m_PointerToRefCounter == NULL);
        Ptr7.Clear();
        QVERIFY(Ptr7.m_PointerToRefCounter == NULL);
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, true);
        Ptr8.Clear();
        QVERIFY(Ptr8.m_PointerToRefCounter == NULL);
        Ptr9.Clear();
        QVERIFY(Ptr9.m_PointerToRefCounter == NULL);
        QCOMPARE(Data1, false);
        QCOMPARE(Data2, false);
    }
    QCOMPARE(Data1, false);
    QCOMPARE(Data2, false);

}

} // end namespace Global

QTEST_MAIN(Global::TestSharedPointer)

#include "TestSharedPointer.moc"

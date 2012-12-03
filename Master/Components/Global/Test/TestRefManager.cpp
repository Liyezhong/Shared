/****************************************************************************/
/*! \file TestRefManager.cpp
 *
 *  \brief Implementation file for class TestRefManager.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-09-28
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
#include <Global/Include/RefManager.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Test class for RefManager class.
 */
/****************************************************************************/
class TestRefManager : public QObject {
    Q_OBJECT
private slots:
    /****************************************************************************/
    /**
     * \brief Called before the first testfunction is executed.
     */
    /****************************************************************************/
    void initTestCase();
    /****************************************************************************/
    /**
     * \brief Called before each testfunction is executed.
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
     * \brief Test constructor.
     */
    /****************************************************************************/
    void utTestConstructor();
    /****************************************************************************/
    /**
     * \brief Test functionality.
     */
    /****************************************************************************/
    void utTestRefHandling();
}; // end class TestRefManager

/****************************************************************************/
void TestRefManager::initTestCase() {
}

/****************************************************************************/
void TestRefManager::init() {
}

/****************************************************************************/
void TestRefManager::cleanup() {
}

/****************************************************************************/
void TestRefManager::cleanupTestCase() {
}

/****************************************************************************/
void TestRefManager::utTestConstructor() {
    // check INVALID value
    QCOMPARE(RefManager<unsigned char>::INVALID, (unsigned char)0);

    RefManager<unsigned char> Obj;
    // check constructor
    QCOMPARE(RefManager<unsigned char>::INVALID,    (unsigned char)0);
    QCOMPARE(Obj.m_LastRef,                         RefManager<unsigned char>::INVALID);
    QCOMPARE(Obj.m_BlockedRefs.size(),              1);
    QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));
}

/****************************************************************************/
void TestRefManager::utTestRefHandling()  {
    try {
        // create object
        RefManager<unsigned char> Obj;
        // get first reference
        unsigned char Ref1 = 1;
        unsigned char NewRef = Obj.GetNewRef();
        QCOMPARE(NewRef,                    Ref1);
        QCOMPARE(Obj.m_BlockedRefs.size(),  2);
        QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref1));

        // get second reference
        unsigned char Ref2 = 2;
        NewRef = Obj.GetNewRef();
        QCOMPARE(NewRef,                    Ref2);
        QCOMPARE(Obj.m_BlockedRefs.size(),  3);
        QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref1));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref2));

        // remove first reference
        Obj.UnblockReference(Ref1);
        QCOMPARE(Obj.m_BlockedRefs.size(),  2);
        QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref2));

        // get third reference
        unsigned char Ref3 = 3;
        NewRef = Obj.GetNewRef();
        QCOMPARE(NewRef,                    Ref3);
        QCOMPARE(Obj.m_BlockedRefs.size(),  3);
        QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref2));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref3));

        // remove second reference
        Obj.UnblockReference(Ref2);
        QCOMPARE(Obj.m_BlockedRefs.size(),  2);
        QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref3));

        // remove third reference
        Obj.UnblockReference(Ref3);
        QCOMPARE(Obj.m_BlockedRefs.size(),  1);
        QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));

        // remove third reference again
        Obj.UnblockReference(Ref3);
        QCOMPARE(Obj.m_BlockedRefs.size(),  1);
        QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));

        // get fourth reference
        unsigned char Ref4 = 4;
        NewRef = Obj.GetNewRef();
        QCOMPARE(NewRef,                    Ref4);
        QCOMPARE(Obj.m_BlockedRefs.size(),  2);
        QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref4));

        // fill until end
        for(unsigned long i=Ref4+1; i<256; i++) {
            Obj.m_BlockedRefs.insert((unsigned char)i);
        }
        QCOMPARE(Obj.m_BlockedRefs.size(),  253);

        // get fifth reference
        unsigned char Ref5 = 1;
        NewRef = Obj.GetNewRef();
        QCOMPARE(NewRef,                    Ref5);
        QCOMPARE(Obj.m_BlockedRefs.size(),  254);
        QVERIFY(Obj.m_BlockedRefs.contains(RefManager<unsigned char>::INVALID));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref4));
        QVERIFY(Obj.m_BlockedRefs.contains(Ref5));

        // fill complete
        for(unsigned long i=0; i<256; i++) {
            Obj.m_BlockedRefs.insert((unsigned char)i);
        }
        QCOMPARE(Obj.m_BlockedRefs.size(),  256);

        // get sixth reference
        try {
            NewRef = Obj.GetNewRef();
            QFAIL("You should never get here!");
        } catch(const Exception &E) {
            QCOMPARE(E.GetErrorCode(), EVENT_GLOBAL_ERROR_COMPUTING_NEW_REF);
        } catch (...) {
            QFAIL("You should never get here!");
        }

        // remove fifth reference.
        Obj.UnblockReference(Ref5);
        QCOMPARE(Obj.m_BlockedRefs.size(),  255);
        QVERIFY(!Obj.m_BlockedRefs.contains(Ref5));

        // get seventh reference
        unsigned char Ref7 = Ref5;
        NewRef = Obj.GetNewRef();
        QCOMPARE(NewRef,                    Ref7);
        QCOMPARE(Obj.m_BlockedRefs.size(),  256);
        QVERIFY(Obj.m_BlockedRefs.contains(Ref7));

        // get eighth reference
        try {
            NewRef = Obj.GetNewRef();
            QFAIL("You should never get here!");
        } catch(const Exception &E) {
            QCOMPARE(E.GetErrorCode(), EVENT_GLOBAL_ERROR_COMPUTING_NEW_REF);
        } catch (...) {
            QFAIL("You should never get here!");
        }

        // remove some reference.
        Obj.UnblockReference(123);
        QCOMPARE(Obj.m_BlockedRefs.size(),  255);
        QVERIFY(!Obj.m_BlockedRefs.contains(123));

        // get nineth reference
        unsigned char Ref9 = 123;
        NewRef = Obj.GetNewRef();
        QCOMPARE(NewRef,                    Ref9);
        QCOMPARE(Obj.m_BlockedRefs.size(),  256);
        QVERIFY(Obj.m_BlockedRefs.contains(Ref9));

    } catch(...) {
        QFAIL("You should never get here!");
    }
}

} // end namespace Global

QTEST_MAIN(Global::TestRefManager)

#include "TestRefManager.moc"

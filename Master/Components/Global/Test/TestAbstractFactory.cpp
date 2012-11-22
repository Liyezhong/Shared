/****************************************************************************/
/*! \file TestAbstractFactory.cpp
 *
 *  \brief Implementation file for class TestAbstractFactory.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-01-20
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
#include <Global/Test/TestAbstractFactory.h>

namespace Global {

DEFINE_REG_CLASS(AFTestBaseClass, AFTestDerivedClass1);     ///< Static registration member.
DEFINE_REG_CLASS(AFTestBaseClass, AFTestDerivedClass2);     ///< Static registration member.

/****************************************************************************/
void AFTestBaseClass::LinkerTamer() {
}

/****************************************************************************/
/**
 * \brief Test class for AbstractFactory class.
 */
/****************************************************************************/
class TestAbstractFactory : public QObject {
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
     * \brief Test factory.
     */
    /****************************************************************************/
    void utFactory();
}; // end class TestAbstractFactory

/****************************************************************************/
void TestAbstractFactory::initTestCase() {
}

/****************************************************************************/
void TestAbstractFactory::init() {
}

/****************************************************************************/
void TestAbstractFactory::cleanup() {
}

/****************************************************************************/
void TestAbstractFactory::cleanupTestCase() {
}

/****************************************************************************/
void TestAbstractFactory::utFactory() {
    AFTestAF::Initialize();

    // test CheckClassInstance
    QVERIFY(!AFTestAF::CheckClassInstance("AFTestDerivedClass0"));
    QVERIFY( AFTestAF::CheckClassInstance("AFTestDerivedClass1"));
    QVERIFY( AFTestAF::CheckClassInstance("AFTestDerivedClass2"));


    // test CreateInstance
    AFTestBaseClass *p0 = AFTestAF::CreateInstance("AFTestDerivedClass0");
    AFTestBaseClass *p1 = AFTestAF::CreateInstance("AFTestDerivedClass1");
    AFTestBaseClass *p2 = AFTestAF::CreateInstance("AFTestDerivedClass2");

    QVERIFY(p0 == NULL);
    QVERIFY(p1 != NULL);
    QVERIFY(p2 != NULL);
    QVERIFY(p1 != p2);
    QCOMPARE(p1->GetID(), 1);
    QCOMPARE(p2->GetID(), 2);

    delete p0;
    delete p1;
    delete p2;

    // fiddle around with null-pointer
    delete AFTestAF::m_pMap;
    AFTestAF::m_pMap = NULL;
    // test CheckClassInstance
    QVERIFY(!AFTestAF::CheckClassInstance("AFTestDerivedClass0"));
    QVERIFY(!AFTestAF::CheckClassInstance("AFTestDerivedClass1"));
    QVERIFY(!AFTestAF::CheckClassInstance("AFTestDerivedClass2"));

    // test CreateInstance
    p0 = AFTestAF::CreateInstance("AFTestDerivedClass0");
    p1 = AFTestAF::CreateInstance("AFTestDerivedClass1");
    p2 = AFTestAF::CreateInstance("AFTestDerivedClass2");
    QVERIFY(p0 == NULL);
    QVERIFY(p1 == NULL);
    QVERIFY(p2 == NULL);
}

} // end namespace Global

QTEST_MAIN(Global::TestAbstractFactory)

#include "TestAbstractFactory.moc"

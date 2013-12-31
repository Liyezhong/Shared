/****************************************************************************/
/*! \file TestCreatorFunctor.cpp
 *
 *  \brief Definition file for class TestCreatorFunctor.
 *
 *  $Version: $ 0.1
 *  $Date:    $ 26.04.2013
 *  $Author:  $ Shuvasmita.S
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

#include <QtTest/QTest>
#include <QDebug>
#include <QFile>
#include <QProcess>


#include <NetworkComponents/Include/CreatorFunctor.h>


namespace NetworkBase {

/****************************************************************************/
/**
 * \brief CreatorFunctor is virtual. This is its test realization.
 */
/****************************************************************************/
class NetworkCreatorFunctor : public CreatorFunctor
{
public:
    /// constructor
    inline NetworkCreatorFunctor() :
        CreatorFunctor::CreatorFunctor()  {}
    void CreateAndDeserialize(Global::tRefType Ref, QDataStream &DataStream)
    {
        Q_UNUSED(Ref)
        Q_UNUSED(DataStream)
    }

    /// destructor
    inline ~NetworkCreatorFunctor() {}

};

/****************************************************************************/
/**
 * \brief Test class for Creator Functor class.
 */
/****************************************************************************/
class TestCreatorFunctor: public QObject {
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
     * \brief Test data of CSubModule.
     */
    /****************************************************************************/
    void utTest1();

private:
    NetworkCreatorFunctor   *m_NetworkCreatorFunctor;



}; // end class TestCreatorFunctor

/****************************************************************************/
void TestCreatorFunctor::initTestCase() {
}

/****************************************************************************/
void TestCreatorFunctor::init() {
}

/****************************************************************************/
void TestCreatorFunctor::cleanup() {
}

/****************************************************************************/
void TestCreatorFunctor::cleanupTestCase() {
}

void TestCreatorFunctor::utTest1()
{  
    // check the default constructor
    m_NetworkCreatorFunctor = new NetworkCreatorFunctor();
    delete m_NetworkCreatorFunctor;
}

}//end namespace

QTEST_MAIN(NetworkBase::TestCreatorFunctor)
#include "TestCreatorFunctor.moc"


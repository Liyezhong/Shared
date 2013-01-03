/****************************************************************************/
/*! \file TestAdjustment.cpp
 *
 *  \brief Unit test for Adjustment Container
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-31
 *  $Author:    $ Stalin
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
#include <QDebug>
#include <QFile>
#include <QMap>
#include "DataManager/Containers/Adjustment/Include/Adjustment.h"
#include "DataManager/Containers/Adjustment/Include/AdjustmentVerifier.h"

namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for Program sequence class.
 */
/****************************************************************************/
class TestAdjustment : public QObject
{
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
     * \brief Test Adjustment object
     */
    /****************************************************************************/
    void utTestReadWrite();
    void utTestAssignment();
    void utTestCopyConstructor();
};

/****************************************************************************/
void TestAdjustment::initTestCase()
{
}

/****************************************************************************/
void TestAdjustment::init()
{
}

/****************************************************************************/
void TestAdjustment::cleanup()
{
}

/****************************************************************************/
void TestAdjustment::cleanupTestCase()
{
}

/****************************************************************************/
void TestAdjustment::utTestReadWrite()
{
    CAdjustment *p_AdjustmentSrc = new CAdjustment();
    CAdjustmentVerifier *p_AdjustmentVerifier = new CAdjustmentVerifier();

    PositionXYZ Position_1;

    p_AdjustmentSrc->AddVerifier(p_AdjustmentVerifier);

    if (p_AdjustmentSrc->Read("../../Resources/Adjustment.xml") == false)
    {
        QFAIL("Readind Adjustment.xml failed");
    }

    QCOMPARE(p_AdjustmentSrc->GetVersion(), 1);
    QCOMPARE(p_AdjustmentSrc->GetDataContainerType(), ADJUSTMENT);

    // Read, Increment & Write

    // Left Side
    for (quint8 Row = 0; Row < XYZ_MAX_ROWS; Row++)
    {
        for (quint8 Column = 0; Column < XYZ_MAX_COLS; Column++)
        {
            p_AdjustmentSrc->GetXyzPosition(LEFT_XYZ, Row, Column, Position_1);

            Position_1.PositionX += 1;
            Position_1.PositionY += 1;
            Position_1.PositionZ += 1;

            p_AdjustmentSrc->SetXyzPosition(LEFT_XYZ, Row, Column, Position_1);
        }
    }

    // Right Side
    for (quint8 Row = 0; Row < XYZ_MAX_ROWS; Row++)
    {
        for (quint8 Column = 0; Column < XYZ_MAX_COLS; Column++)
        {
            p_AdjustmentSrc->GetXyzPosition(RIGHT_XYZ, Row, Column, Position_1);

            Position_1.PositionX += 1;
            Position_1.PositionY += 1;
            Position_1.PositionZ += 1;

            p_AdjustmentSrc->SetXyzPosition(RIGHT_XYZ, Row, Column, Position_1);
        }
    }

    p_AdjustmentSrc->Write();
}

void TestAdjustment::utTestAssignment()
{
    CAdjustment *p_AdjustmentSrc = new CAdjustment();
    CAdjustment *p_AdjustmentCpy = new CAdjustment();
    PositionXYZ Position_1;
    PositionXYZ Position_2;

    p_AdjustmentSrc->Read("../../Resources/Adjustment.xml");

    QCOMPARE(p_AdjustmentSrc->GetVersion(), 1);
    QCOMPARE(p_AdjustmentSrc->GetDataContainerType(), ADJUSTMENT);

    // Verify assignment operator
    *p_AdjustmentCpy = *p_AdjustmentSrc;

    for (quint8 Row = 0; Row < XYZ_MAX_ROWS; Row++)
    {
        for (quint8 Column = 0; Column < XYZ_MAX_COLS; Column++)
        {
            p_AdjustmentSrc->GetXyzPosition(RIGHT_XYZ, Row, Column, Position_1);
            p_AdjustmentCpy->GetXyzPosition(RIGHT_XYZ, Row, Column, Position_2);

            QCOMPARE(Position_1.PositionX, Position_2.PositionX);
            QCOMPARE(Position_1.PositionY, Position_2.PositionY);
            QCOMPARE(Position_1.PositionZ, Position_2.PositionZ);
        }
    }
}

void TestAdjustment::utTestCopyConstructor()
{
    CAdjustment *p_AdjustmentSrc = new CAdjustment();
    PositionXYZ Position_1;
    PositionXYZ Position_2;

    p_AdjustmentSrc->Read("../../Resources/Adjustment.xml");

    CAdjustment *p_AdjustmentCpy = new CAdjustment(*p_AdjustmentSrc);

    QCOMPARE(p_AdjustmentSrc->GetVersion(), 1);
    QCOMPARE(p_AdjustmentSrc->GetDataContainerType(), ADJUSTMENT);

    // Verify copy constructor
    for (quint8 Row = 0; Row < XYZ_MAX_ROWS; Row++)
    {
        for (quint8 Column = 0; Column < XYZ_MAX_COLS; Column++)
        {
            p_AdjustmentSrc->GetXyzPosition(RIGHT_XYZ, Row, Column, Position_1);
            p_AdjustmentCpy->GetXyzPosition(RIGHT_XYZ, Row, Column, Position_2);

            QCOMPARE(Position_1.PositionX, Position_2.PositionX);
            QCOMPARE(Position_1.PositionY, Position_2.PositionY);
            QCOMPARE(Position_1.PositionZ, Position_2.PositionZ);
        }
    }
}

} // end namespace DataManager

QTEST_MAIN(DataManager::TestAdjustment)

#include "TestAdjustment.moc"

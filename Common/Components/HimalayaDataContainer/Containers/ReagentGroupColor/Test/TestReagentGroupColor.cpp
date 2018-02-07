/****************************************************************************/
/*! \file TestReagentGroupColor.cpp
 *
 *  \brief Unit test for ReagentGroupColor.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-05-08
 *  $Author:    $ N.Kamath
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

#include <HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColor.h>
#include <HimalayaDataContainer/Containers/ReagentGroupColor/Include/ReagentGroupColorList.h>

namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for Reagent group color.
 */
/****************************************************************************/
class TestReagentGroupColor : public QObject {
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
     * \brief Test ReagentGroupColor object
     */
    /****************************************************************************/
    void utTestReagentGroupColor();

    /****************************************************************************/
    /**
     * \brief Test data ReagentGroupColorList
     */
    /****************************************************************************/
    void utTestReagentGroupColorList();
}; // end class TestDataReagentGroupColorList

/****************************************************************************/
void TestReagentGroupColor::initTestCase() {
}

/****************************************************************************/
void TestReagentGroupColor::init() {
}

/****************************************************************************/
void TestReagentGroupColor::cleanup() {
}

/****************************************************************************/
void TestReagentGroupColor::cleanupTestCase() {
}

/****************************************************************************/
void TestReagentGroupColor::utTestReagentGroupColor() {
    CReagentGroupColor *p_ReagentGroupColor1 = new CReagentGroupColor();
    CReagentGroupColor *p_ReagentGroupColor2 = new CReagentGroupColor("1026");

    p_ReagentGroupColor1->SetColorID("9003");
    p_ReagentGroupColor1->SetColorValue("E3F9FC");

    QCOMPARE(p_ReagentGroupColor1->GetColorID(), QString("9003"));
    QCOMPARE(p_ReagentGroupColor1->GetColorValue(), QString("E3F9FC"));

    p_ReagentGroupColor2->SetColorID("1026");
    p_ReagentGroupColor2->SetColorValue("B2F1FC");

    CReagentGroupColor ReagentGroupColor3(*p_ReagentGroupColor2); // copy constructor of CReagentGroupColor

    QCOMPARE(ReagentGroupColor3.GetColorID(), QString("1026"));
    QCOMPARE(ReagentGroupColor3.GetColorValue(), QString("B2F1FC"));
}

/****************************************************************************/
void TestReagentGroupColor::utTestReagentGroupColorList() {
    CReagentGroupColor *p_ReagentGroupColor4 = new CReagentGroupColor();
    CReagentGroupColorList *p_ReagentGroupColorList = new CReagentGroupColorList();


    p_ReagentGroupColorList->SetDataVerificationMode(false);
    // Checking the Read of CReagentGroupColorList
    QCOMPARE(p_ReagentGroupColorList->Read(":/XML/HimalayaReagentGroupColors.xml"), true);
    p_ReagentGroupColorList->SetDataVerificationMode(true);

    // Checking the basic interfaces of CReagentGroupColorList
    QCOMPARE(p_ReagentGroupColorList->GetDataVerificationMode(), true);
    QCOMPARE(p_ReagentGroupColorList->GetVersion(), 0);
    QCOMPARE(p_ReagentGroupColorList->GetNumberOfReagentGroupColor(), 12);

    p_ReagentGroupColor4->SetColorID("7001");
    p_ReagentGroupColor4->SetColorValue("E3F9FC");

    // Checking the AddReagentGroupColor
    QVERIFY(p_ReagentGroupColorList->AddReagentColorGroup(p_ReagentGroupColor4)); // added the Leica reagent
    QCOMPARE(p_ReagentGroupColorList->GetNumberOfReagentGroupColor(), 13);

    p_ReagentGroupColorList->GetCReagentGroupColor("7001");
    p_ReagentGroupColorList->GetCReagentGroupColor(2);

    // Check the copy constructor of CReagentGroupColorList
    CReagentGroupColorList ReagentGroupList1(*p_ReagentGroupColorList);

    // This file would be created in build folder particular to the Target (bin_dlg)
    QCOMPARE(ReagentGroupList1.Write(":/XML/HimalayaReagentGroupColors.xml"), false);
}


} // end namespace DataManager

QTEST_MAIN(DataManager::TestReagentGroupColor)

#include "TestReagentGroupColor.moc"


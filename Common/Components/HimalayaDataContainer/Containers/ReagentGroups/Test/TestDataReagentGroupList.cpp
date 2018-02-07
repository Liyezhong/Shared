/****************************************************************************/
/*! \file TestDataReagentGroupList.cpp
 *
 *  \brief Unit test for DataReagentGroupList.
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

#include <HimalayaDataContainer/Containers/ReagentGroups/Include/ReagentGroup.h>
#include <HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupList.h>
#include <HimalayaDataContainer/Containers/ReagentGroups/Include/DataReagentGroupVerifier.h>
#include <HimalayaDataContainer/Containers/ReagentGroups/Commands/Include/CmdReagentGroupUpdate.h>

namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for Reagent group list.
 */
/****************************************************************************/
class TestDataReagentGroupList : public QObject {
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
     * \brief Test ReagentGroup object
     */
    /****************************************************************************/
    void utTestReagentGroup();

    /****************************************************************************/
    /**
     * \brief Test data ReagentGroupList
     */
    /****************************************************************************/
    void utTestDataReagentGroupList();

    /****************************************************************************/
    /**
     * \brief Test CmdReagentGroupUpdate
     */
    /****************************************************************************/
    void utTestCmdReagentGroupUpdate();
}; // end class TestDataReagentGroupList

/****************************************************************************/
void TestDataReagentGroupList::initTestCase() {
}

/****************************************************************************/
void TestDataReagentGroupList::init() {
}

/****************************************************************************/
void TestDataReagentGroupList::cleanup() {
}

/****************************************************************************/
void TestDataReagentGroupList::cleanupTestCase() {
}

/****************************************************************************/
void TestDataReagentGroupList::utTestReagentGroup() {
    CReagentGroup *p_ReagentGroup1 = new CReagentGroup();
    CReagentGroup *p_ReagentGroup2 = new CReagentGroup("RG2");

    p_ReagentGroup1->SetGroupID("RG1");
    p_ReagentGroup1->SetGroupNameID("1234");
    p_ReagentGroup1->SetReagentGroupName("Fixation");
    p_ReagentGroup1->CleaningReagentGroup(false);
    p_ReagentGroup1->IsParraffin(false);
    p_ReagentGroup1->SetGroupColor("D9FFD9");

    QCOMPARE(p_ReagentGroup1->GetGroupID(), QString("RG1"));
    QCOMPARE(p_ReagentGroup1->GetGroupNameID(), QString("1234"));
    QCOMPARE(p_ReagentGroup1->GetReagentGroupName(), QString("\"1234\":"));
    QCOMPARE(p_ReagentGroup1->IsCleaningReagentGroup(), false);
    QCOMPARE(p_ReagentGroup1->IsParraffin(), false);
    QCOMPARE(p_ReagentGroup1->GetGroupColor(), QString("D9FFD9"));

    p_ReagentGroup2->SetGroupNameID("50331654");
    p_ReagentGroup2->SetReagentGroupName("Water");
    p_ReagentGroup2->CleaningReagentGroup(false);
    p_ReagentGroup2->IsParraffin(false);
    p_ReagentGroup2->SetGroupColor("A9B7FF");

    CReagentGroup ReagentGroup3(*p_ReagentGroup2); // copy constructor of CReagent

    QCOMPARE(ReagentGroup3.GetGroupID(), QString("RG2"));
    QCOMPARE(ReagentGroup3.GetGroupNameID(), QString("50331654"));
    QCOMPARE(ReagentGroup3.GetReagentGroupName(), QString("\"50331654\":"));
    QCOMPARE(ReagentGroup3.IsCleaningReagentGroup(), false);
    QCOMPARE(ReagentGroup3.IsParraffin(), false);
    QCOMPARE(ReagentGroup3.GetGroupColor(), QString("A9B7FF"));
}

/****************************************************************************/
void TestDataReagentGroupList::utTestDataReagentGroupList() {
    CReagentGroup *p_ReagentGroup4 = new CReagentGroup();
    CReagentGroup *p_ReagentGroup5 = new CReagentGroup();
    CDataReagentGroupList *p_ReagentGroupList = new CDataReagentGroupList();
    IVerifierInterface *p_DataReagentGroupListVerifier;
    p_DataReagentGroupListVerifier = new CDataReagentGroupListVerifier();
    p_ReagentGroupList->AddVerifier(p_DataReagentGroupListVerifier);
    p_ReagentGroupList->Init();

    p_ReagentGroupList->SetDataVerificationMode(false);
    // Checking the Read of CDataReagentGroupList
    QCOMPARE(p_ReagentGroupList->Read(":/Xml/HimalayaReagentGroups.xml"), true);
    p_ReagentGroupList->SetDataVerificationMode(true);

    // Checking the basic interfaces of CDataReagentGroupList
    QCOMPARE(p_ReagentGroupList->GetDataVerificationMode(), true);
    QCOMPARE(p_ReagentGroupList->GetVersion(), 1);
    QCOMPARE(p_ReagentGroupList->GetNumberOfReagentGroups(), 8);
    QCOMPARE(p_ReagentGroupList->GetReagentGroupIndex("RG8"), 7);

    p_ReagentGroup4->SetGroupID("RG123");
    p_ReagentGroup4->SetGroupNameID("1234123");
    p_ReagentGroup4->SetReagentGroupName("Fixation_1");
    p_ReagentGroup4->CleaningReagentGroup(false);
    p_ReagentGroup4->IsParraffin(false);
    p_ReagentGroup4->SetGroupColor("D9FFD9");

    // Checking the AddReagent
    QVERIFY(p_ReagentGroupList->AddReagentGroup(p_ReagentGroup4)); // added the Leica reagent

    p_ReagentGroup5->SetGroupID("RG456");
    p_ReagentGroup5->SetGroupNameID("456789");
    p_ReagentGroup5->SetReagentGroupName("Fixation_2");
    p_ReagentGroup5->CleaningReagentGroup(false);
    p_ReagentGroup5->IsParraffin(false);
    p_ReagentGroup5->SetGroupColor("D9FFD9");

    QVERIFY(p_ReagentGroupList->AddReagentGroup(p_ReagentGroup5)); // added the Special reagent
    QCOMPARE(p_ReagentGroupList->GetNumberOfReagentGroups(), 10);


    CReagentGroup reagentGroup6;
    CReagentGroup reagentGroup7;

    p_ReagentGroupList->GetReagentGroup("RG1");
    p_ReagentGroupList->GetReagentGroup(2);
    QCOMPARE(p_ReagentGroupList->GetReagentGroup(3, reagentGroup6), true);
    QCOMPARE(p_ReagentGroupList->GetReagentGroup("RG7", reagentGroup7), true);

    reagentGroup7.SetReagentGroupName("Cleaning Solvent-111");
    reagentGroup7.CleaningReagentGroup(false);

    // Checking the Update ReagentGroup
    //QCOMPARE(p_ReagentGroupList->UpdateReagentGroup(reagentGroup7), true);

    //Checking the Delete Reagent group
    QVERIFY(p_ReagentGroupList->DeleteReagentGroup("RG123")); // deleted special reagent group
    QCOMPARE(p_ReagentGroupList->GetNumberOfReagentGroups(), 9);

    QVERIFY(p_ReagentGroupList->DeleteReagentGroup(8)); // deleted special reagent group
    QCOMPARE(p_ReagentGroupList->GetNumberOfReagentGroups(), 8);

    // Check the copy constructor of CDataReagentGroupList
    CDataReagentGroupList ReagentGroupList1(*p_ReagentGroupList);

    // This file would be created in build folder particular to the Target (bin_dlg)
    QCOMPARE(ReagentGroupList1.Write(":/Xml/HimalayaReagentGroups.xml"), false);
}

void TestDataReagentGroupList::utTestCmdReagentGroupUpdate() {
    MsgClasses::CmdReagentGroupUpdate *p_CmdReagentGroupUpdate = new MsgClasses::CmdReagentGroupUpdate(0, "RG1", "D9FFD9");
    p_CmdReagentGroupUpdate->NAME = "CmdReagentGroupUpdate";

    QCOMPARE(p_CmdReagentGroupUpdate->GetName(), QString("CmdReagentGroupUpdate"));
    QCOMPARE(p_CmdReagentGroupUpdate->GetTimeout(), 0);
    QCOMPARE(p_CmdReagentGroupUpdate->GroupId(), QString("RG1"));
    QCOMPARE(p_CmdReagentGroupUpdate->GroupColor(), QString("D9FFD9"));

    MsgClasses::CmdReagentGroupUpdate CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdReagentGroupUpdate;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdReagentGroupUpdate->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdReagentGroupUpdate->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdReagentGroupUpdate->GroupColor(), CmdData.GroupColor());
    delete p_CmdReagentGroupUpdate;

}

} // end namespace DataManager

QTEST_MAIN(DataManager::TestDataReagentGroupList)

#include "TestDataReagentGroupList.moc"


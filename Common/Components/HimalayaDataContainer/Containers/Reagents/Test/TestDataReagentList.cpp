/****************************************************************************/
/*! \file TestDataReagentList.cpp
 *
 *  \brief Implementation file for class TestDataCReagent.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-04-23
 *  $Author:    $ Vikram MK
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
#include <QFile>
#include <HimalayaDataContainer/Containers/Reagents/Include/Reagent.h>
#include <HimalayaDataContainer/Containers/Reagents/Include/DataReagentList.h>
#include <HimalayaDataContainer/Containers/Reagents/Include/DataReagentListVerifier.h>
#include <HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentAdd.h>
#include <HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentUpdate.h>
#include <HimalayaDataContainer/Containers/Reagents/Commands/Include/CmdReagentRemove.h>



namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for CReagent class.
 */
/****************************************************************************/
class TestDataReagentList : public QObject {
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
     * \brief Test data CReagent.
     */
    /****************************************************************************/
    void utTestDataReagent();
    /****************************************************************************/
    /**
     * \brief Test data CDataReagentList.
     */
    /****************************************************************************/
    void utTestDataReagentList();
    /****************************************************************************/
    /**
     * \brief Test CmdReagentAdd.
     */
    /****************************************************************************/
    void utTestCmdReagentAdd();
    /****************************************************************************/
    /**
     * \brief Test CmdReagentRemove.
     */
    /****************************************************************************/
    void utTestCmdReagentRemove();
    /****************************************************************************/
    /**
     * \brief Test CmdReagentUpdate.
     */
    /****************************************************************************/
    void utTestCmdReagentUpdate();

}; // end class TestDataReagentList

/****************************************************************************/
void TestDataReagentList::initTestCase() {
}

/****************************************************************************/
void TestDataReagentList::init() {
}

/****************************************************************************/
void TestDataReagentList::cleanup() {
}

/****************************************************************************/
void TestDataReagentList::cleanupTestCase() {
}

/****************************************************************************/
void TestDataReagentList::utTestDataReagent() {

    CReagent *p_Reagent1 = new CReagent();
    CReagent *p_Reagent2 = new CReagent();

    p_Reagent1->SetReagentID("L1");
    p_Reagent1->SetReagentName("Formalin");
    p_Reagent1->SetReagentNameID("1234");
    p_Reagent1->SetVisibleState(false);
    p_Reagent1->SetGroupID("RG1");
    p_Reagent1->SetMaxCycles(500);
    p_Reagent1->SetMaxCassettes(1000);
    p_Reagent1->SetMaxDays(720);

    QCOMPARE(p_Reagent1->GetReagentID(), QString("L1"));

    QCOMPARE(p_Reagent1->GetReagentName(), QString("\"1234\":"));
    QCOMPARE(p_Reagent1->GetReagentNameID(), QString("1234"));
    QCOMPARE(p_Reagent1->GetVisibleState(), false);
    QCOMPARE(p_Reagent1->GetGroupID(), QString("RG1"));
    QCOMPARE(p_Reagent1->GetMaxCycles(), 500);
    QCOMPARE(p_Reagent1->GetMaxCassettes(), 1000);
    QCOMPARE(p_Reagent1->GetMaxDays(), 720);


    p_Reagent2->SetReagentID("L2");
    p_Reagent2->SetReagentName("Ethanol 70%");
    p_Reagent2->SetReagentNameID("5678");
    p_Reagent2->SetVisibleState(false);
    p_Reagent2->SetGroupID("RG3");
    p_Reagent2->SetMaxCycles(500);
    p_Reagent2->SetMaxCassettes(1000);
    p_Reagent2->SetMaxDays(720);

    CReagent Reagent3(*p_Reagent2); // copy constructor of CReagent

    QCOMPARE(Reagent3.GetReagentID(), QString("L2"));
    QCOMPARE(Reagent3.GetReagentName(), QString("\"5678\":"));
    QCOMPARE(Reagent3.GetReagentNameID(), QString("5678"));
    QCOMPARE(Reagent3.GetGroupID(), QString("RG3"));
    QCOMPARE(Reagent3.GetMaxCycles(), 500);
    QCOMPARE(Reagent3.GetMaxCassettes(), 1000);
    QCOMPARE(Reagent3.GetMaxDays(), 720);
}
/****************************************************************************/
void TestDataReagentList::utTestDataReagentList() {

    CReagent *p_Reagent4 = new CReagent();
    CReagent *p_Reagent5 = new CReagent();
    CDataReagentList *p_ReagentList = new CDataReagentList();
    IVerifierInterface *p_DataReagentListVerifier;
    p_DataReagentListVerifier = new CDataReagentListVerifier();
    p_ReagentList->AddVerifier(p_DataReagentListVerifier);

    p_ReagentList->SetDataVerificationMode(false);
    // Checking the Read of CDataReagentList
    QCOMPARE(p_ReagentList->Read(":/Xml/HimalayaReagents.xml"), true);
    p_ReagentList->SetDataVerificationMode(true);

    // Checking the basic interfaces of CDataReagentList
    QCOMPARE(p_ReagentList->GetNumberOfReagents(), 11);
    QCOMPARE(p_ReagentList->GetVersion(), 1);
    QCOMPARE(p_ReagentList->GetReagentType("L1"), LEICA_REAGENT);
    QCOMPARE(p_ReagentList->GetReagentType(2), LEICA_REAGENT);
    QCOMPARE(p_ReagentList->ReagentExists("L5"), true);
    QCOMPARE(p_ReagentList->ReagentExists("U78"), false);

    p_Reagent4->SetReagentID("U101");
    p_Reagent4->SetReagentName("Formalin-1");
    p_Reagent4->SetReagentNameID("12345");
    p_Reagent4->SetVisibleState(true);
    p_Reagent4->SetGroupID("RG1");
    p_Reagent4->SetMaxCycles(500);
    p_Reagent4->SetMaxCassettes(1000);
    p_Reagent4->SetMaxDays(720);
    // Checking the AddReagent
    QVERIFY(p_ReagentList->AddReagent(p_Reagent4)); // added the Leica reagent

    p_Reagent5->SetReagentID("U102");
    p_Reagent5->SetReagentName("Ethanol 70%-1");
    p_Reagent5->SetReagentNameID("123467");
    p_Reagent5->SetVisibleState(true);
    p_Reagent5->SetGroupID("RG3");

    QVERIFY(p_ReagentList->AddReagent(p_Reagent5)); // added the Special reagent
    QCOMPARE(p_ReagentList->GetNumberOfReagents(), 13);

    // Create a User Reagent
    CReagent *p_Reagent3 = p_ReagentList->CreateReagent();
    p_Reagent3->SetReagentName("NEWLYADDED");
    QVERIFY(p_ReagentList->AddReagent(p_Reagent3)); // added the User reagent
    QCOMPARE(p_ReagentList->GetNumberOfReagents(), 14);

    CReagent *p_Reagent6 = new CReagent();
    CReagent *p_Reagent7 = new CReagent();
    CReagent *p_Reagent8 = new CReagent();

    QCOMPARE(p_ReagentList->GetReagent(12, *p_Reagent6) , true);
    QCOMPARE(p_ReagentList->GetReagent("L3", *p_Reagent7) , true);
    QCOMPARE(p_ReagentList->GetReagent("U101", *p_Reagent8) , true);

    p_Reagent8->SetReagentName("Formalin-111");
    p_Reagent8->SetReagentNameID("123459");
    p_Reagent8->SetMaxCycles(700);
    p_Reagent8->SetMaxCassettes(1500);
    p_Reagent8->SetMaxDays(900);

    // Checking the Update Reagent
    QCOMPARE(p_ReagentList->UpdateReagent(p_Reagent8), true);
    QCOMPARE(p_ReagentList->ReagentExists(p_Reagent8->GetReagentID()), true);
    // checking to update a non existing reagent
    CReagent *p_Reagent9 = new CReagent();
    p_Reagent9->SetReagentID("U113");
    QCOMPARE(p_ReagentList->UpdateReagent(p_Reagent9), false);

    //Checking the Delete Reagent
    QVERIFY(p_ReagentList->DeleteReagent(p_Reagent6->GetReagentID())); // deleted special reagent
    QCOMPARE(p_ReagentList->ReagentExists("U101"), true);
    QCOMPARE(p_ReagentList->GetNumberOfReagents(), 13);

    // Check the copy constructor of CDataReagentList
    CDataReagentList ReagentList1(*p_ReagentList);

    // This file would be created in build folder particular to the Target (bin_dlg)
    QCOMPARE(ReagentList1.Write(":/Xml/HimalayaReagents.xml"), false);

    //check the Write Method
    CDataReagentList *p_ReagentList2 = new CDataReagentList();
    IVerifierInterface *p_DataReagentListVerifier1;
    p_DataReagentListVerifier1 = new CDataReagentListVerifier();
    p_ReagentList2->AddVerifier(p_DataReagentListVerifier1);

    p_ReagentList2->SetDataVerificationMode(false);
    // Checking the Read of CDataReagentList
    QCOMPARE(p_ReagentList2->Read(":/Xml/HimalayaReagents.xml"), true);
    p_ReagentList2->SetDataVerificationMode(true);

    //delete a Reagent from the list
    QCOMPARE(p_ReagentList2->DeleteReagent("U102"), false);
    QCOMPARE(static_cast<CDataContainerBase*>(p_ReagentList2)->Write(), false);

}

/****************************************************************************/
void TestDataReagentList::utTestCmdReagentAdd() {
    QDataStream DataStream(QByteArray("No reagent data"));

    MsgClasses::CmdReagentAdd *p_CmdReagentAdd = new MsgClasses::CmdReagentAdd(0, DataStream);
    p_CmdReagentAdd->NAME = "CmdReagentAdd";

    QCOMPARE(p_CmdReagentAdd->GetName(), QString("CmdReagentAdd"));
    QCOMPARE(p_CmdReagentAdd->GetTimeout(), 0);
    QCOMPARE(p_CmdReagentAdd->GetReagentData(), QByteArray("No reagent data"));


    MsgClasses::CmdReagentAdd CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdReagentAdd;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdReagentAdd->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdReagentAdd->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdReagentAdd->GetReagentData(), CmdData.GetReagentData());

    delete p_CmdReagentAdd;
}

/****************************************************************************/
void TestDataReagentList::utTestCmdReagentRemove(){
    MsgClasses::CmdReagentRemove *p_CmdReagentRemove = new MsgClasses::CmdReagentRemove();
    p_CmdReagentRemove->NAME = "CmdReagentRemove";

    QCOMPARE(p_CmdReagentRemove->GetName(), QString("CmdReagentRemove"));
    QCOMPARE(p_CmdReagentRemove->GetTimeout(), 0);

    MsgClasses::CmdReagentRemove *p_CmdReagentRemoveParam = new MsgClasses::CmdReagentRemove(1000, "Id");
    QCOMPARE(p_CmdReagentRemoveParam->GetTimeout(), 1000);
    QCOMPARE(p_CmdReagentRemoveParam->GetReagentID(), QString("Id"));

    MsgClasses::CmdReagentRemove CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdReagentRemoveParam;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdReagentRemoveParam->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdReagentRemoveParam->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdReagentRemoveParam->GetReagentID(), CmdData.GetReagentID());


    delete p_CmdReagentRemoveParam;
    delete p_CmdReagentRemove;
}

/****************************************************************************/
void TestDataReagentList::utTestCmdReagentUpdate(){
    QDataStream DataStream(QByteArray("No reagent data"));

    MsgClasses::CmdReagentUpdate *p_CmdReagentUpdate = new MsgClasses::CmdReagentUpdate(0, DataStream);
    p_CmdReagentUpdate->NAME = "CmdReagentUpdate";

    QCOMPARE(p_CmdReagentUpdate->GetName(), QString("CmdReagentUpdate"));
    QCOMPARE(p_CmdReagentUpdate->GetTimeout(), 0);
    QCOMPARE(p_CmdReagentUpdate->GetReagentData(), QByteArray("No reagent data"));

    MsgClasses::CmdReagentUpdate CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdReagentUpdate;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdReagentUpdate->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdReagentUpdate->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdReagentUpdate->GetReagentData(), CmdData.GetReagentData());
    delete p_CmdReagentUpdate;
}

} // end namespace DataManager

QTEST_MAIN(DataManager::TestDataReagentList)

#include "TestDataReagentList.moc"


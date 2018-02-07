/****************************************************************************/
/*! \file TestDashboardStationList.cpp
 *
 *  \brief Unit test for DashboardDataStationList.
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

#include <HimalayaDataContainer/Containers/DashboardStations/Include/DashboardStation.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Include/DashboardDataStationList.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdCurrentProgramStepInfor.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdKeepCassetteCount.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdLockStatus.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAborted.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAcknowledge.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramAction.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramSelected.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdProgramSelectedReply.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdRecoveryFromPowerFailure.h>
#include <HimalayaDataContainer/Containers/DashboardStations/Commands/Include/CmdRetortStatus.h>


namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for DashboardStation list.
 */
/****************************************************************************/
class TestDashboardStationList : public QObject {
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
     * \brief Test DashboardStation object
     */
    /****************************************************************************/
    void utTestDashboardStation();

    /****************************************************************************/
    /**
     * \brief Test DashboardStationList
     */
    /****************************************************************************/
    void utTestDashboardStationList();

    void utTestCmdCurrentProgramStepInfor();

    void utTestCmdKeepCassetteCount();

    void utTestCmdLockStatus();

    void utTestCmdProgramAborted();

    void utTestCmdProgramAcknowledge();

    void utTestCmdProgramAction();

    void utTestCmdProgramSelected();

    void utTestCmdProgramSelectedReply();

    void utTestCmdRecoveryFromPowerFailure();

    void utTestCmdRetortStatus();

}; // end class TestDataReagentGroupList

/****************************************************************************/
void TestDashboardStationList::initTestCase() {
}

/****************************************************************************/
void TestDashboardStationList::init() {
}

/****************************************************************************/
void TestDashboardStationList::cleanup() {
}

/****************************************************************************/
void TestDashboardStationList::cleanupTestCase() {
}

/****************************************************************************/
void TestDashboardStationList::utTestDashboardStation() {
    CDashboardStation *p_DashboardStation1 = new CDashboardStation();
    CDashboardStation *p_DashboardStation2 = new CDashboardStation("S2");

    p_DashboardStation1->SetDashboardStationID("S1");
    p_DashboardStation1->SetDashboardStationName("S1");
    p_DashboardStation1->IsParaffinBath(false);
    p_DashboardStation1->SetDashboardReagentID("U1");
    p_DashboardStation1->SetDashboardReagentStatus("Empty");
    p_DashboardStation1->SetDashboardReagentExchangeDate(QDateTime::currentDateTime());
    p_DashboardStation1->SetDashboardReagentActualCassettes(600);
    p_DashboardStation1->SetDashboardReagentActualCycles(600);
    p_DashboardStation1->SetDashboardStationStatus(DASHBOARD_RETORT_FULL);

    QCOMPARE(p_DashboardStation1->GetDashboardStationID(), QString("S1"));
    QCOMPARE(p_DashboardStation1->GetDashboardStationName(), QString("S1"));
    QCOMPARE(p_DashboardStation1->IsParaffinBath(), false);
    QCOMPARE(p_DashboardStation1->GetDashboardReagentID(), QString("U1"));
    QCOMPARE(p_DashboardStation1->GetDashboardReagentStatus(), QString("Empty"));
    QCOMPARE(p_DashboardStation1->GetDashboardReagentExchangeDate(), QDateTime::currentDateTime());
    QCOMPARE(p_DashboardStation1->GetDashboardReagentActualCassettes(), 600);
    QCOMPARE(p_DashboardStation1->GetDashboardReagentActualCycles(), 600);
    QCOMPARE(p_DashboardStation1->GetDashboardStationStatus(), DASHBOARD_RETORT_FULL);

    p_DashboardStation2->SetDashboardStationName("P1");
    p_DashboardStation2->IsParaffinBath(true);
    p_DashboardStation2->SetDashboardReagentID("L8");
    p_DashboardStation2->SetDashboardReagentStatus("Empty");
    p_DashboardStation2->SetDashboardReagentExchangeDate(QDateTime::currentDateTime());
    p_DashboardStation2->SetDashboardReagentActualCassettes(200);
    p_DashboardStation2->SetDashboardReagentActualCycles(0);
    p_DashboardStation2->SetDashboardStationStatus(DASHBOARD_RETORT_FULL);

    CDashboardStation DashboardStation3(*p_DashboardStation2); // copy constructor of CReagent

    QCOMPARE(DashboardStation3.GetDashboardStationID(), QString("S2"));
    QCOMPARE(DashboardStation3.GetDashboardStationName(), QString("P1"));
    QCOMPARE(DashboardStation3.IsParaffinBath(), true);
    QCOMPARE(DashboardStation3.GetDashboardReagentID(), QString("L8"));
    QCOMPARE(DashboardStation3.GetDashboardReagentStatus(), QString("Empty"));
    QCOMPARE(DashboardStation3.GetDashboardReagentActualCassettes(), 200);
}

/****************************************************************************/
void TestDashboardStationList::utTestDashboardStationList() {
    CDashboardStation *p_DashboardStation4 = new CDashboardStation();
    CDashboardStation *p_DashboardStation5 = new CDashboardStation();
    CDashboardDataStationList *p_DashboardDataStationList = new CDashboardDataStationList();
    p_DashboardDataStationList->Init();

    p_DashboardDataStationList->SetDataVerificationMode(false);
    // Checking the Read of CDashboardDataStationList
    QCOMPARE(p_DashboardDataStationList->Read(":/Xml/HimalayaStations.xml"), true);
    p_DashboardDataStationList->SetDataVerificationMode(true);

    // Checking the basic interfaces of CDashboardDataStationList
    QCOMPARE(p_DashboardDataStationList->GetDataVerificationMode(), true);
    QCOMPARE(p_DashboardDataStationList->GetVersion(), 1);
    QCOMPARE(p_DashboardDataStationList->GetNumberOfDashboardStations(), 16);

    p_DashboardStation4->SetDashboardStationID("S114");
    p_DashboardStation4->SetDashboardStationName("S4");
    p_DashboardStation4->IsParaffinBath(false);
    p_DashboardStation4->SetDashboardReagentID("L3");
    p_DashboardStation4->SetDashboardReagentStatus("Full");
    p_DashboardStation4->SetDashboardReagentExchangeDate(QDateTime::currentDateTime());
    p_DashboardStation4->SetDashboardReagentActualCassettes(600);
    p_DashboardStation4->SetDashboardReagentActualCycles(600);
    p_DashboardStation4->SetDashboardStationStatus(DASHBOARD_RETORT_FULL);

    // Checking the AddDashboardStation
    QVERIFY(p_DashboardDataStationList->AddDashboardStation(p_DashboardStation4));

    p_DashboardStation5->SetDashboardStationID("S555");
    p_DashboardStation5->SetDashboardStationName("S5");
    p_DashboardStation5->IsParaffinBath(false);
    p_DashboardStation5->SetDashboardReagentID("L5");
    p_DashboardStation5->SetDashboardReagentStatus("Full");

    QVERIFY(p_DashboardDataStationList->AddDashboardStation(p_DashboardStation5)); // added the Special reagent
    QCOMPARE(p_DashboardDataStationList->GetNumberOfDashboardStations(), 18);


    CDashboardStation dashboardStation6;
    CDashboardStation dashboardStation7;

    p_DashboardDataStationList->GetDashboardStation("S4");
    p_DashboardDataStationList->GetDashboardStation(0);
    QCOMPARE(p_DashboardDataStationList->GetDashboardStation(3, dashboardStation6), true);
    QCOMPARE(p_DashboardDataStationList->GetDashboardStation("S5", dashboardStation7), true);

    dashboardStation7.SetDashboardStationName("9");
    dashboardStation7.SetDashboardReagentID("L2");

    // Check the copy constructor of CDashboardDataStationList
    CDashboardDataStationList dashboardDataStationList1(*p_DashboardDataStationList);

    // This file would be created in build folder particular to the Target (bin_dlg)
    QCOMPARE(dashboardDataStationList1.Write(":/Xml/HimalayaStations.xml"), false);
}

void TestDashboardStationList::utTestCmdCurrentProgramStepInfor() {
    MsgClasses::CmdCurrentProgramStepInfor *p_CmdCurrentProgramStepInfor = new MsgClasses::CmdCurrentProgramStepInfor(0, "Paraffin", 0, 5000);
    p_CmdCurrentProgramStepInfor->NAME = "CmdCurrentProgramStepInfor";

    QCOMPARE(p_CmdCurrentProgramStepInfor->GetName(), QString("CmdCurrentProgramStepInfor"));
    QCOMPARE(p_CmdCurrentProgramStepInfor->GetTimeout(), 0);
    QCOMPARE(p_CmdCurrentProgramStepInfor->CurProgramStepIndex(), 0);
    QCOMPARE(p_CmdCurrentProgramStepInfor->CurRemainingTime(), 5000);
    QCOMPARE(p_CmdCurrentProgramStepInfor->StepName(), QString("Paraffin"));

    MsgClasses::CmdCurrentProgramStepInfor CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdCurrentProgramStepInfor;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdCurrentProgramStepInfor->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdCurrentProgramStepInfor->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdCurrentProgramStepInfor->StepName(), CmdData.StepName());
    delete p_CmdCurrentProgramStepInfor;
}

void TestDashboardStationList::utTestCmdKeepCassetteCount()
{
    MsgClasses::CmdKeepCassetteCount *p_CmdKeepCassetteCount = new MsgClasses::CmdKeepCassetteCount(0, 200);
    p_CmdKeepCassetteCount->NAME = "CmdKeepCassetteCount";

    QCOMPARE(p_CmdKeepCassetteCount->GetName(), QString("CmdKeepCassetteCount"));
    QCOMPARE(p_CmdKeepCassetteCount->GetTimeout(), 0);
    QCOMPARE(p_CmdKeepCassetteCount->CassetteCount(), 200);

    MsgClasses::CmdKeepCassetteCount CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdKeepCassetteCount;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdKeepCassetteCount->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdKeepCassetteCount->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdKeepCassetteCount->CassetteCount(), CmdData.CassetteCount());
    delete p_CmdKeepCassetteCount;
}
void TestDashboardStationList::utTestCmdLockStatus()
{
    MsgClasses::CmdLockStatus *p_CmdLockStatus = new MsgClasses::CmdLockStatus(0, RETORT_LOCK, true);
    p_CmdLockStatus->NAME = "CmdLockStatus";

    QCOMPARE(p_CmdLockStatus->GetName(), QString("CmdLockStatus"));
    QCOMPARE(p_CmdLockStatus->GetTimeout(), 0);
    QCOMPARE(p_CmdLockStatus->IsLocked(), true);
    QCOMPARE(p_CmdLockStatus->LockType(), RETORT_LOCK);

    MsgClasses::CmdLockStatus CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdLockStatus;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdLockStatus->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdLockStatus->LockType(), CmdData.LockType());
    QCOMPARE(p_CmdLockStatus->IsLocked(), CmdData.IsLocked());
    QCOMPARE(p_CmdLockStatus->GetTimeout(), CmdData.GetTimeout());
    delete p_CmdLockStatus;
}
void TestDashboardStationList::utTestCmdProgramAborted()
{
    MsgClasses::CmdProgramAborted *p_CmdProgramAborted = new MsgClasses::CmdProgramAborted(0, true);
    p_CmdProgramAborted->NAME = "CmdProgramAborted";

    QCOMPARE(p_CmdProgramAborted->GetName(), QString("CmdProgramAborted"));
    QCOMPARE(p_CmdProgramAborted->GetTimeout(), 0);
    QCOMPARE(p_CmdProgramAborted->IsRetortContaminated(), true);

    MsgClasses::CmdProgramAborted CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdProgramAborted;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdProgramAborted->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdProgramAborted->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdProgramAborted->IsRetortContaminated(), CmdData.IsRetortContaminated());
    delete p_CmdProgramAborted;
}

void TestDashboardStationList::utTestCmdProgramAcknowledge()
{
    MsgClasses::CmdProgramAcknowledge *p_CmdProgramAcknowledge = new MsgClasses::CmdProgramAcknowledge(0, PROGRAM_READY);
    p_CmdProgramAcknowledge->NAME = "CmdProgramAcknowledge";

    QCOMPARE(p_CmdProgramAcknowledge->GetName(), QString("CmdProgramAcknowledge"));
    QCOMPARE(p_CmdProgramAcknowledge->GetTimeout(), 0);
    QCOMPARE(p_CmdProgramAcknowledge->AcknownedgeType(), PROGRAM_READY);

    MsgClasses::CmdProgramAcknowledge CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdProgramAcknowledge;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdProgramAcknowledge->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdProgramAcknowledge->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdProgramAcknowledge->AcknownedgeType(), CmdData.AcknownedgeType());
    delete p_CmdProgramAcknowledge;
}

void TestDashboardStationList::utTestCmdProgramAction()
{
    MsgClasses::CmdProgramAction *p_CmdProgramAction = new MsgClasses::CmdProgramAction(0, "L02", PROGRAM_START, 300, 6000, "");
    p_CmdProgramAction->NAME = "CmdProgramAction";

    QCOMPARE(p_CmdProgramAction->GetName(), QString("CmdProgramAction"));
    QCOMPARE(p_CmdProgramAction->GetTimeout(), 0);
    QCOMPARE(p_CmdProgramAction->GetProgramID(), QString("L02"));
    QCOMPARE(p_CmdProgramAction->ProgramActionType(), PROGRAM_START);
    QCOMPARE(p_CmdProgramAction->DelayTime(), 300);

    MsgClasses::CmdProgramAction CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdProgramAction;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdProgramAction->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdProgramAction->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdProgramAction->ProgramActionType(), CmdData.ProgramActionType());
    QCOMPARE(p_CmdProgramAction->DelayTime(), CmdData.DelayTime());
    QCOMPARE(p_CmdProgramAction->GetProgramID(), CmdData.GetProgramID());

    delete p_CmdProgramAction;
}

void TestDashboardStationList::utTestCmdProgramSelected()
{
    MsgClasses::CmdProgramSelected *p_CmdProgramSelected = new MsgClasses::CmdProgramSelected(0, "L02", 2);
    p_CmdProgramSelected->NAME = "CmdProgramSelected";

    QCOMPARE(p_CmdProgramSelected->GetName(), QString("CmdProgramSelected"));
    QCOMPARE(p_CmdProgramSelected->GetTimeout(), 0);
    QCOMPARE(p_CmdProgramSelected->ParaffinStepIndex(), 2);
    QCOMPARE(p_CmdProgramSelected->GetProgramID(), QString("L02"));

    MsgClasses::CmdProgramSelected CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdProgramSelected;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdProgramSelected->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdProgramSelected->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdProgramSelected->ParaffinStepIndex(), CmdData.ParaffinStepIndex());
    QCOMPARE(p_CmdProgramSelected->GetProgramID(), CmdData.GetProgramID());
    delete p_CmdProgramSelected;
}

void TestDashboardStationList::utTestCmdProgramSelectedReply()
{
    QList<QString> stationList;
    MsgClasses::CmdProgramSelectedReply *p_CmdProgramSelectedReply = new MsgClasses::CmdProgramSelectedReply(0, 56789,
                                                                                                             3000, 2000,
                                                                                                             1, 212112,
                                                                                                             stationList, 0);
    p_CmdProgramSelectedReply->NAME = "CmdProgramSelectedReply";

    QCOMPARE(p_CmdProgramSelectedReply->GetName(), QString("CmdProgramSelectedReply"));
    QCOMPARE(p_CmdProgramSelectedReply->GetTimeout(), 0);
    QCOMPARE(p_CmdProgramSelectedReply->TimeProposed(), 56789);
    QCOMPARE(p_CmdProgramSelectedReply->ParaffinMeltCostedTime(), 3000);
    QCOMPARE(p_CmdProgramSelectedReply->CostedTimeBeforeParaffin(), 2000);
    QCOMPARE(p_CmdProgramSelectedReply->WhichStepHasNoSafeReagent(), 1);
    p_CmdProgramSelectedReply->StationList();
    QCOMPARE(p_CmdProgramSelectedReply->GetSecondsForMeltingParaffin(), 212112);
    QCOMPARE(p_CmdProgramSelectedReply->GetFirstProgramStepIndex(), 0);


    MsgClasses::CmdProgramSelectedReply CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdProgramSelectedReply;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdProgramSelectedReply->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdProgramSelectedReply->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdProgramSelectedReply->TimeProposed(), CmdData.TimeProposed());
    delete p_CmdProgramSelectedReply;
}

void TestDashboardStationList::utTestCmdRecoveryFromPowerFailure()
{
    QList<QString> stationList;
    MsgClasses::CmdRecoveryFromPowerFailure *p_CmdRecoveryFromPowerFailure = new MsgClasses::CmdRecoveryFromPowerFailure(0, "L02",
                                                                                                        0, 206, 50000,
                                                                                                                         "RG1", stationList, false, "");
    p_CmdRecoveryFromPowerFailure->NAME = "CmdRecoveryFromPowerFailure";

    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetName(), QString("CmdRecoveryFromPowerFailure"));
    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetTimeout(), 0);
    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetProgramID(), QString("L02"));
    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetStepIndex(), 0);
    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetScenario(), 206);
    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetRemainingTime(), 50000);
    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetLastReagentGroupID(), QString("RG1"));
    p_CmdRecoveryFromPowerFailure->GetStationList();


    MsgClasses::CmdRecoveryFromPowerFailure CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdRecoveryFromPowerFailure;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdRecoveryFromPowerFailure->GetProgramID(), CmdData.GetProgramID());
    delete p_CmdRecoveryFromPowerFailure;
}

void TestDashboardStationList::utTestCmdRetortStatus()
{
    MsgClasses::CmdRetortStatus *p_CmdRetortStatus = new MsgClasses::CmdRetortStatus(0, RETORT_SCUKING);
    p_CmdRetortStatus->NAME = "CmdRetortStatus";

    QCOMPARE(p_CmdRetortStatus->GetName(), QString("CmdRetortStatus"));
    QCOMPARE(p_CmdRetortStatus->GetTimeout(), 0);
    QCOMPARE(p_CmdRetortStatus->RetortStatusType(), RETORT_SCUKING);

    MsgClasses::CmdRetortStatus CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdRetortStatus;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdRetortStatus->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdRetortStatus->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdRetortStatus->RetortStatusType(), CmdData.RetortStatusType());
    delete p_CmdRetortStatus;
}

} // end namespace DataManager

QTEST_MAIN(DataManager::TestDashboardStationList)

#include "TestDashboardStationList.moc"


/****************************************************************************/
/*! \file TestDataStationList.cpp
 *
 *  \brief Implementation file for class TestDataCReagent.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-05-02
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
#include <HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationChangeReagent.h>
#include <HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationResetData.h>
#include <HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationSetAsEmpty.h>
#include <HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdStationSetAsFull.h>
#include <HimalayaDataContainer/Containers/ReagentStations/Commands/Include/CmdUpdateStationReagentStatus.h>


namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for CStation class.
 */
/****************************************************************************/
class TestDataStationList : public QObject {
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
     * \brief Test Station Commands.
     */
    /****************************************************************************/
    void utTestStationCommands();

    void utTestCmdStationSetAsEmpty();
    void utTestCmdStationSetAsFull();
    void utTestCmdUpdateStationReagentStatus();
}; // end class TestDataStationList

/****************************************************************************/
void TestDataStationList::initTestCase() {
}

/****************************************************************************/
void TestDataStationList::init() {
}

/****************************************************************************/
void TestDataStationList::cleanup() {
}

/****************************************************************************/
void TestDataStationList::cleanupTestCase() {
}

/****************************************************************************/
void TestDataStationList::utTestStationCommands() {

    MsgClasses::CmdStationChangeReagent *p_CmdStationChangeReagent = new MsgClasses::CmdStationChangeReagent(0, "S1", "L1");
    p_CmdStationChangeReagent->NAME = "CmdStationChangeReagent";

    QCOMPARE(p_CmdStationChangeReagent->GetName(), QString("CmdStationChangeReagent"));
    QCOMPARE(p_CmdStationChangeReagent->GetTimeout(), 0);
    QCOMPARE(p_CmdStationChangeReagent->StationID(), QString("S1"));
    QCOMPARE(p_CmdStationChangeReagent->ReagentID(), QString("L1"));

    MsgClasses::CmdStationChangeReagent CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdStationChangeReagent;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdStationChangeReagent->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdStationChangeReagent->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdStationChangeReagent->StationID(), CmdData.StationID());
    QCOMPARE(p_CmdStationChangeReagent->ReagentID(), CmdData.ReagentID());
    delete p_CmdStationChangeReagent;


    MsgClasses::CmdStationResetData *p_CmdStationResetData = new MsgClasses::CmdStationResetData(0, "S1");
    p_CmdStationResetData->NAME = "CmdStationResetData";

    QCOMPARE(p_CmdStationResetData->GetName(), QString("CmdStationResetData"));
    QCOMPARE(p_CmdStationResetData->GetTimeout(), 0);
    QCOMPARE(p_CmdStationResetData->StationID(), QString("S1"));

    MsgClasses::CmdStationResetData CmdResetData;
    QByteArray Data2;
    // create data stream
    QDataStream DataCmdStream2(&Data2, QIODevice::ReadWrite);
    DataCmdStream2.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream2 << *p_CmdStationResetData;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream2.device()->reset();

    DataCmdStream2 >> CmdResetData;
    QCOMPARE(p_CmdStationResetData->GetName(), CmdResetData.GetName());
    QCOMPARE(p_CmdStationResetData->GetTimeout(), CmdResetData.GetTimeout());
    QCOMPARE(p_CmdStationResetData->StationID(), CmdResetData.StationID());
    delete p_CmdStationResetData;
}

void TestDataStationList::utTestCmdStationSetAsEmpty() {

    MsgClasses::CmdStationSetAsEmpty *p_CmdStationSetAsEmpty = new MsgClasses::CmdStationSetAsEmpty(0, "S1");
    p_CmdStationSetAsEmpty->NAME = "CmdStationSetAsEmpty";

    QCOMPARE(p_CmdStationSetAsEmpty->GetName(), QString("CmdStationSetAsEmpty"));
    QCOMPARE(p_CmdStationSetAsEmpty->GetTimeout(), 0);
    QCOMPARE(p_CmdStationSetAsEmpty->StationID(), QString("S1"));

    MsgClasses::CmdStationSetAsEmpty CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdStationSetAsEmpty;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdStationSetAsEmpty->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdStationSetAsEmpty->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdStationSetAsEmpty->StationID(), CmdData.StationID());
    delete p_CmdStationSetAsEmpty;
}

void TestDataStationList::utTestCmdStationSetAsFull() {

    MsgClasses::CmdStationSetAsFull *p_CmdStationSetAsFull = new MsgClasses::CmdStationSetAsFull(0, "S1");
    p_CmdStationSetAsFull->NAME = "CmdStationSetAsFull";

    QCOMPARE(p_CmdStationSetAsFull->GetName(), QString("CmdStationSetAsFull"));
    QCOMPARE(p_CmdStationSetAsFull->GetTimeout(), 0);
    QCOMPARE(p_CmdStationSetAsFull->StationID(), QString("S1"));

    MsgClasses::CmdStationSetAsFull CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdStationSetAsFull;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdStationSetAsFull->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdStationSetAsFull->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdStationSetAsFull->StationID(), CmdData.StationID());
    delete p_CmdStationSetAsFull;
}

void TestDataStationList::utTestCmdUpdateStationReagentStatus()
{
    QStringList stationIDs;
    MsgClasses::CmdUpdateStationReagentStatus *p_CmdUpdateStationReagentStatus = new MsgClasses::CmdUpdateStationReagentStatus(0, stationIDs, 200);
    p_CmdUpdateStationReagentStatus->NAME = "CmdUpdateStationReagentStatus";

    QCOMPARE(p_CmdUpdateStationReagentStatus->GetName(), QString("CmdUpdateStationReagentStatus"));
    QCOMPARE(p_CmdUpdateStationReagentStatus->GetTimeout(), 0);
    QCOMPARE(p_CmdUpdateStationReagentStatus->CassetteCount(), 200);
    p_CmdUpdateStationReagentStatus->StationIDs();

    MsgClasses::CmdUpdateStationReagentStatus CmdData;
    QByteArray Data;
    // create data stream
    QDataStream DataCmdStream(&Data, QIODevice::ReadWrite);
    DataCmdStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    DataCmdStream << *p_CmdUpdateStationReagentStatus;
    /// reset the position of the data stream otherwise data stream will be pointing to last index in the IODevice
    DataCmdStream.device()->reset();

    DataCmdStream >> CmdData;
    QCOMPARE(p_CmdUpdateStationReagentStatus->GetName(), CmdData.GetName());
    QCOMPARE(p_CmdUpdateStationReagentStatus->GetTimeout(), CmdData.GetTimeout());
    QCOMPARE(p_CmdUpdateStationReagentStatus->CassetteCount(), CmdData.CassetteCount());
    delete p_CmdUpdateStationReagentStatus;
}

} // end namespace DataManager

QTEST_MAIN(DataManager::TestDataStationList)

#include "TestDataStationList.moc"


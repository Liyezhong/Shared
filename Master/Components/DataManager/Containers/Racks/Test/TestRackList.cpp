/****************************************************************************/
/*! \file TestRackList.cpp
 *
 *  \brief Implementation file for class TestRackList.
 *
 *  $Version:   $ 0.2
 *  $Date:      $ 2012-04-23
 *  $Author:    $ M.Scherer, Raju
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

#include <QDebug>
#include <QTest>

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

#include "DataManager/Containers/Racks/Include/RackList.h"
#include "DataManager/Containers/Racks/Include/RackListVerifier.h"




namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for DataRackList class.
 */
/****************************************************************************/
class TestRackList : public QObject {
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
     * \brief Test data for racks.
     */
    /****************************************************************************/
    void utTestRack();
    /****************************************************************************/
    /**
     * \brief Test data for rack list.
     */
    /****************************************************************************/
    void utTestRackList();
    /****************************************************************************/
    /**
     * \brief Test data for verifier of the rack list.
     */
    /****************************************************************************/
    void utTestRackListVerifier();
}; // end class TestRackList

/****************************************************************************/
void TestRackList::initTestCase() {
}

/****************************************************************************/
void TestRackList::init() {
}

/****************************************************************************/
void TestRackList::cleanup() {
}

/****************************************************************************/
void TestRackList::cleanupTestCase() {
}

/****************************************************************************/
void TestRackList::utTestRack() {

    CRack *Rack1 = new CRack(500, 38);
    CRack *Rack2 = new CRack(105, 12);
    CRack *Rack3 = new CRack(43, 85);

    // check whether all the data is working or not Get/Set
    //Rack1->SetRackRFID(500);
    Rack1->SetProgramID("L99");
    Rack1->SetTransported(true);
    Rack1->SetSepiaStation(true);
    Rack1->SetStartTime(Global::AdjustedTime::Instance().GetCurrentDateTime());
    Rack1->SetEndTimeColorado(Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(500));
    Rack1->SetStationID("S12");
    Rack1->SetProgramStepIndex(1);
    Rack1->SetEndTimeStation(Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(1800));
    //Rack1->SetColor("red");
    Rack1->SetEndTimeSepia(Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(1000));

    QCOMPARE(Rack1->GetRFIDUniqueID(), (quint32)500);
    QCOMPARE(Rack1->GetProgramID(), (QString)"L99");
    QCOMPARE(Rack1->GetTransported(), true);
    QCOMPARE(Rack1->GetSepiaStation(), true);
    QCOMPARE(Rack1->GetStationID(), (QString)"S12");
    QCOMPARE(Rack1->GetProgramStepIndex(), (quint16)1);
    QCOMPARE(Rack1->GetColor(), (QString)"white");

    // use assignment operator to copy the data
    *Rack2 = *Rack1;

    // check whether all the values are properly copied or not
    QCOMPARE(Rack1->GetRFIDUniqueID(), Rack2->GetRFIDUniqueID());
    QCOMPARE(Rack1->GetProgramID(), Rack2->GetProgramID());
    QCOMPARE(Rack1->GetTransported(), Rack2->GetTransported());
    QCOMPARE(Rack1->GetSepiaStation(), Rack2->GetSepiaStation());
    QCOMPARE(Rack1->GetStationID(), Rack2->GetStationID());
    QCOMPARE(Rack1->GetProgramStepIndex(), Rack2->GetProgramStepIndex());
    QCOMPARE(Rack1->GetColor(), Rack2->GetColor());
    QCOMPARE(Rack1->GetStartTime().toString(), Rack2->GetStartTime().toString());
    QCOMPARE(Rack1->GetEndTimeStation().toString(), Rack2->GetEndTimeStation().toString());
    QCOMPARE(Rack1->GetEndTimeColorado().toString(), Rack2->GetEndTimeColorado().toString());
    QCOMPARE(Rack1->GetEndTimeSepia().toString(), Rack2->GetEndTimeSepia().toString());

    //CRack *Rack4(Rack3);

    // create the datastream and check whether copying data is working or not
    QByteArray* p_TempByteArray = new QByteArray();
    QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
    DataStream.setVersion(QDataStream::Qt_4_0);
    p_TempByteArray->clear();
    DataStream << *Rack3;
    DataStream.device()->reset();
    DataStream >> *Rack2;
    QCOMPARE(Rack3->GetRFIDUniqueID(), Rack2->GetRFIDUniqueID());

    delete Rack1;
    delete Rack2;
    delete Rack3;
    //delete Rack4;
}


/****************************************************************************/
void TestRackList::utTestRackList() {

    // create the Rack list container
    CRackList RackList;
    CRack *Rack1 = RackList.CreateRack(1234, 237);
    CRack *Rack2 = RackList.CreateRack(4321, 789);
    CRack *Rack3 = RackList.CreateRack(789, 456);

    // make the Data verification mode as false
    RackList.SetDataVerificationMode(false);

    // compare the number of racks
    QCOMPARE(RackList.GetNumberOfRacks(), 0);
    // add rack
    QCOMPARE(RackList.AddRack(Rack1), true);

    // compare the number of racks
    QCOMPARE(RackList.GetNumberOfRacks(), 1);
    // check the rack existence
    QCOMPARE(RackList.RackExists(1234), true);
    QCOMPARE(RackList.RackExists(4321), false);
    // get the specified rack data and check the result
    QCOMPARE(RackList.GetRack((quint32)1234, Rack2), true);
    QCOMPARE(RackList.GetRack((quint32)4321, Rack2), false);

    // get the rack for the given specied location and check the result
    QCOMPARE(RackList.GetRack(0, Rack2), true);
    QCOMPARE(RackList.GetRack(1, Rack2), false);
    QCOMPARE(RackList.AddRack(Rack2), false);
    QCOMPARE(RackList.GetNumberOfRacks(), 1);

    // change the RFID
    //Rack2->SetRackRFID(4321);
    // check RFID whether it is changed or not
    QCOMPARE(Rack3->GetRFIDUniqueID(), (quint32) 789);
    QCOMPARE(RackList.AddRack(Rack3), true);

    QCOMPARE(RackList.GetNumberOfRacks(), 2);
    QCOMPARE(RackList.RackExists(789), true);
    QCOMPARE(RackList.GetRack((quint32)789, Rack2), true);
    QCOMPARE(Rack3->GetTransported(), false);
    Rack3->SetTransported(true);
    QCOMPARE(Rack3->GetTransported(), true);

    QCOMPARE(RackList.UpdateRack(Rack3), true);
    QCOMPARE(RackList.GetRack((quint32)789, Rack3), true);
    QCOMPARE(Rack3->GetTransported(), true);

    QCOMPARE(RackList.DeleteRack(0), true);
    QCOMPARE(RackList.GetNumberOfRacks(), 1);
    QCOMPARE(RackList.DeleteRack((quint32)789), true);
    QCOMPARE(RackList.GetNumberOfRacks(), 0);

    Rack1 = RackList.CreateRack(5678, 234);
    Rack2 = RackList.CreateRack(8765, 789);

    QCOMPARE(RackList.AddRack(Rack1), true);
    QCOMPARE(RackList.GetNumberOfRacks(), 1);
    QCOMPARE(RackList.AddRack(Rack2), true);
    QCOMPARE(RackList.GetNumberOfRacks(), 2);

    delete Rack1;
    delete Rack2;
    delete Rack3;
}

/****************************************************************************/
void TestRackList::utTestRackListVerifier() {

    // create the Rack list container
    CRackList RackList;
    CRackList RackList1;
    IVerifierInterface *p_RackListVerifier;
    p_RackListVerifier = new CRackListVerifier();
    if (!RackList.AddVerifier(p_RackListVerifier)) {
        QFAIL("Compiler never should come to this statement");
    }
    CRack *Rack1 = RackList.CreateRack(1234, 789);
    CRack *Rack2 = RackList.CreateRack(2345, 6547);
    CRack *Rack3 = RackList.CreateRack(4567, 7894);

    //Rack1->SetRackRFID(500);
    Rack1->SetProgramID("L99");
    Rack1->SetTransported(false);
    Rack1->SetSepiaStation(false);
    Rack1->SetStartTime(Global::AdjustedTime::Instance().GetCurrentDateTime());
    Rack1->SetEndTimeColorado(Rack1->GetStartTime()); // Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(500));
    Rack1->SetStationID("S12");
    Rack1->SetProgramStepIndex(1);
    Rack1->SetEndTimeStation(Rack1->GetStartTime()); // (Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(1800));
    //Rack1->SetColor("6");
    Rack1->SetEndTimeSepia(Rack1->GetStartTime()); // (Global::AdjustedTime::Instance().GetCurrentDateTime().addSecs(1000));

    QCOMPARE(RackList.AddRack(Rack1), true);
    QCOMPARE(RackList.AddRack(Rack1), false);
    QCOMPARE(RackList.VerifyData(), true);

    // check for the verification
    //QCOMPARE(DataRackList.VerifyData(), false);
    Rack2->SetSepiaStation(true);
    Rack2->SetEndTimeSepia(Rack2->GetStartTime().addDays(2));;
    QCOMPARE(RackList.AddRack(Rack2), false);

    RackList1 = RackList;

    //QStringList ErrorList = RackList.GetLastErrors().split("\n");
    // check the error list count
    //QCOMPARE(ErrorList.count(), 3);

    //ErrorList = RackList1.GetLastErrors().split("\n");
    //qDebug() << ErrorList.count();

   // QCOMPARE(RackList.VerifyData(), true);

    // need to write the codes for the verifier

    delete Rack1;
    delete Rack2;
    delete Rack3;
}

} // end namespace DataManagement

QTEST_MAIN(DataManager::TestRackList)

#include "TestRackList.moc"


/****************************************************************************/
/*! \file TestDataProgramList.cpp
 *
 *  \brief Implementation file for class TestDataCReagent.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-04-23
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

#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramListVerifier.h"



namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for CReagent class.
 */
/****************************************************************************/
class TestDataProgramList : public QObject {
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
     * \brief Test data of CProgramStep.
     */
    /****************************************************************************/
    void utTestDataProgramStep();

    /****************************************************************************/
    /**
     * \brief Test data CProgram
     */
    /****************************************************************************/
    void utTestProgram();
    /****************************************************************************/
    /**
     * \brief Test data CProgramList
     */
    /****************************************************************************/
    void utTestDataProgramList();

    /****************************************************************************/
    /**
     * \brief Test data of CDataProgramListVerifier.
     */
    /****************************************************************************/
    void utTestDataProgramListVerifier();
}; // end class TestDataReagentList

/****************************************************************************/
void TestDataProgramList::initTestCase() {
}

/****************************************************************************/
void TestDataProgramList::init() {
}

/****************************************************************************/
void TestDataProgramList::cleanup() {
}

/****************************************************************************/
void TestDataProgramList::cleanupTestCase() {
}

/****************************************************************************/
void TestDataProgramList::utTestDataProgramStep() {
    CProgramStep *p_ProgramStep1 = new CProgramStep();
//    p_ProgramStep1->SetExclusive(false);
//    p_ProgramStep1->SetIntensity(0);
//    p_ProgramStep1->SetMaxDurationInPercent("100%");
//    p_ProgramStep1->SetMinDuration("1m");
//    p_ProgramStep1->SetNumberOfParallelStations(1);
    p_ProgramStep1->SetReagentID("S8");
    p_ProgramStep1->SetStepID("1");
    //Test getters
//    QCOMPARE(p_ProgramStep1->GetExclusive(), false);
//    QCOMPARE(p_ProgramStep1->GetIntensity(), 0);
//    QCOMPARE(p_ProgramStep1->GetMaxDurationInPercent(), 100);
//    QCOMPARE(p_ProgramStep1->GetMinDuration(), QString("1m"));
//    QCOMPARE(p_ProgramStep1->GetNumberOfParallelStations(), 1);
    QCOMPARE(p_ProgramStep1->GetReagentID(), QString("S8"));
    QCOMPARE(p_ProgramStep1->GetStepID(), QString("1"));

    CProgramStep ProgramStep(*p_ProgramStep1);
    //Test getters
//    QCOMPARE(ProgramStep.GetExclusive(), false);
//    QCOMPARE(ProgramStep.GetIntensity(), 0);
//    QCOMPARE(ProgramStep.GetMaxDurationInPercent(), 100);
//    QCOMPARE(ProgramStep.GetMinDuration(), QString("1m"));
//    QCOMPARE(ProgramStep.GetNumberOfParallelStations(), 1);
    QCOMPARE(ProgramStep.GetReagentID(), QString("S8"));
    QCOMPARE(ProgramStep.GetStepID(), QString("1"));

}

/****************************************************************************/
void TestDataProgramList::utTestProgram() {
    CProgram *p_Program1 = new CProgram();
    CProgram *p_Program2 = new CProgram();
    // Test all setters and getters for CProgram
    p_Program1->SetID("L100");
    p_Program1->SetLongName("Eosin");
//    p_Program1->SetShortName("E");
//    p_Program1->SetColor("Green");
//    p_Program1->LockProgram();

    // Test assignment operator
    *p_Program2 = *p_Program1;
    QCOMPARE(p_Program2->GetLongName(),p_Program1->GetLongName());
//    QCOMPARE(p_Program2->GetShortName(), p_Program1->GetShortName());
//    QCOMPARE(p_Program2->GetColor(),p_Program1->GetColor());
    QCOMPARE(p_Program2->GetID(),p_Program1->GetID());
//    QCOMPARE(p_Program2->IsLocked(), p_Program1->IsLocked());

    // Test copy constructor
//    CProgram *p_Program3 = new CProgram("Black", false, "U100", "H&E", "EOSIN");
//    CProgram Program4(*p_Program3);
//    QCOMPARE(Program4.GetLongName(),p_Program3->GetLongName());
//    QCOMPARE(Program4.GetShortName(), p_Program3->GetShortName());
//    QCOMPARE(Program4.GetColor(),p_Program3->GetColor());
//    QCOMPARE(Program4.GetID(),p_Program3->GetID());
//    QCOMPARE(Program4.IsLocked(), p_Program3->IsLocked());
    delete p_Program2;
    delete p_Program1;
}

/****************************************************************************/
void TestDataProgramList::utTestDataProgramList() {
    //Test program list
    CDataProgramList *p_ProgramList = new CDataProgramList();
    CDataProgramListVerifier *p_ProgramListVerifier = new CDataProgramListVerifier();
    p_ProgramList->AddVerifier(p_ProgramListVerifier);
    p_ProgramList->SetDataVerificationMode(false);
    QCOMPARE(p_ProgramList->Read(":/Xml/Programs.xml"), true);

    p_ProgramList->SetDataVerificationMode(true);

    // Verify the data to be checked
    QVERIFY(p_ProgramListVerifier->VerifyData(p_ProgramList));

    QCOMPARE(p_ProgramList->GetDataContainerType(), PROGRAMS);
    QCOMPARE(p_ProgramList->GetDataVerificationMode(), true);
    QCOMPARE(p_ProgramList->GetNextFreeProgID(false),QString("U33"));
    QCOMPARE(p_ProgramList->GetNumberOfPrograms(), 20);
    QCOMPARE(p_ProgramList->GetVersion(), 4);

    //Test Copy constructor
    CDataProgramList *p_ProgramList2 = new CDataProgramList(*p_ProgramList);
    QCOMPARE(p_ProgramList2->GetFilename(),QString(":/Xml/Programs.xml"));
    QCOMPARE(p_ProgramList2->GetDataContainerType(), PROGRAMS);
    QCOMPARE(p_ProgramList2->GetDataVerificationMode(), true);
    QCOMPARE(p_ProgramList2->GetNextFreeProgID(false), p_ProgramList->GetNextFreeProgID(false));
    QCOMPARE(p_ProgramList2->GetNumberOfPrograms(), p_ProgramList->GetNumberOfPrograms());
    QCOMPARE(p_ProgramList2->GetVersion(), p_ProgramList->GetVersion());

    // Test Assignment operator
    CDataProgramList *p_ProgramList3 = new CDataProgramList();
    *p_ProgramList3 = *p_ProgramList2;
    QCOMPARE(p_ProgramList3->GetDataContainerType(), PROGRAMS);
    QCOMPARE(p_ProgramList3->GetDataVerificationMode(), true);
    QCOMPARE(p_ProgramList3->GetNextFreeProgID(false), p_ProgramList->GetNextFreeProgID(false));
    QCOMPARE(p_ProgramList3->GetNumberOfPrograms(), p_ProgramList->GetNumberOfPrograms());
    QCOMPARE(p_ProgramList3->GetVersion(), p_ProgramList->GetVersion());


    // Verifying the new method Leica Reagents
    CProgram Program10;
    QVERIFY(p_ProgramList3->GetProgram("L1", Program10));
    CProgramStep ProgramStep1;
    CProgramStep ProgramStep2;

//    QVERIFY(Program10.GetLeicaReagents(ProgramStep1, ProgramStep2));
    qDebug() << "The Program Steps are:" << ProgramStep1.GetStepID() << ProgramStep2.GetStepID();

    //Modify the last step and check. This test is used to change last step from Unloader to transfer
    //  station and vice versa
    CProgram *p_ProgramX = const_cast<CProgram *>(p_ProgramList->GetProgram(QString("U1")));
//    p_ProgramX->SetProgramModified(true);
//    CProgramStep *p_ModifiedLastStep = p_ProgramX->GetProgramStepExpanded(p_ProgramX->GetNumberOfStepsInExpandedList() - 1);
//    p_ModifiedLastStep->SetReagentID("S8");
    const CProgramStep *p_OriginalStep = p_ProgramX->GetProgramStep(p_ProgramX->GetNumberOfSteps() - 1);
//    QCOMPARE(p_ModifiedLastStep->GetReagentID(), QString("S8"));
//    QCOMPARE(p_OriginalStep->GetReagentID(), QString("S7"));

    p_ProgramList->UpdateProgram(p_ProgramX);
    //Now repeat the procedure, check status of modified
    p_ProgramX = const_cast<CProgram *>(p_ProgramList->GetProgram(QString("U1")));
//    QCOMPARE(p_ProgramX->GetModifiedStatus(), true);
//    p_ModifiedLastStep = p_ProgramX->GetProgramStepExpanded(p_ProgramX->GetNumberOfStepsInExpandedList() - 1);
    p_OriginalStep = const_cast<CProgramStep *>(p_ProgramX->GetProgramStep(p_ProgramX->GetNumberOfSteps() - 1));
//    QCOMPARE(p_ModifiedLastStep->GetReagentID(), QString("S8"));
    QCOMPARE(p_OriginalStep->GetReagentID(), QString("S7"));

    //Now test Serialization and deseiralization
    QByteArray ProgramData;
    QDataStream ProgramDataStream(&ProgramData, QIODevice::ReadWrite);
    ProgramDataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
    ProgramData.clear();

    ProgramDataStream << *p_ProgramX;
    ProgramDataStream.device()->reset();
    CProgram Program;
    ProgramDataStream >> Program;
    ProgramDataStream.device()->reset();
//    CProgramStep *p_DeserializedModifiedProgramStep = Program.GetProgramStepExpanded(Program.GetNumberOfStepsInExpandedList() -1);
//    const CProgramStep *p_DeserializedOriginalProgramStep = Program.GetProgramStep(Program.GetNumberOfSteps() -1);
//    QCOMPARE(p_DeserializedModifiedProgramStep->GetReagentID(), QString("S8"));
//    QCOMPARE(p_DeserializedOriginalProgramStep->GetReagentID(), QString("S7"));

    //Now revert back the modification and test
//    p_ProgramX->SetProgramModified(false);
//    p_ModifiedLastStep->SetReagentID("S7");
    p_ProgramList->UpdateProgram(p_ProgramX);
    //Reassign programX
    p_ProgramX = const_cast<CProgram *>(p_ProgramList->GetProgram(QString("U1")));
//    QCOMPARE(p_ProgramX->GetModifiedStatus(), false);
//    p_ModifiedLastStep = p_ProgramX->GetProgramStepExpanded(p_ProgramX->GetNumberOfStepsInExpandedList() - 1);
    p_OriginalStep = const_cast<CProgramStep *>(p_ProgramX->GetProgramStep(p_ProgramX->GetNumberOfSteps() - 1));
//    QCOMPARE(p_ModifiedLastStep->GetReagentID(), QString("S7"));
    QCOMPARE(p_OriginalStep->GetReagentID(), QString("S7"));
    //Again test Serialization and deseiralization
    ProgramData.clear();
    ProgramDataStream.device()->reset();
    ProgramDataStream << *p_ProgramX;
    ProgramDataStream.device()->reset();
    ProgramDataStream >> Program;
    ProgramDataStream.device()->reset();
//    p_DeserializedModifiedProgramStep = Program.GetProgramStepExpanded(Program.GetNumberOfStepsInExpandedList() -1);
//    p_DeserializedOriginalProgramStep = const_cast<CProgramStep*>(Program.GetProgramStep(Program.GetNumberOfSteps() -1));
//    QCOMPARE(p_DeserializedModifiedProgramStep->GetReagentID(), QString("S7"));
//    QCOMPARE(p_DeserializedOriginalProgramStep->GetReagentID(), QString("S7"));



    CProgram *p_Program1 = new CProgram();

    p_Program1->SetID("U100");
    p_Program1->SetLongName("Eosin");
//    p_Program1->SetShortName("E");
//    p_Program1->SetColor("Green");
//    p_Program1->LockProgram();

    CProgramStep *p_ProgStep = new CProgramStep(p_Program1->GetNextFreeStepID(true),
                                                QString("U2"), 10, 100, false, 0, 0);
    p_Program1->AddProgramStep(p_ProgStep);
    qint32 NumberOfPrograms = p_ProgramList3->GetNumberOfPrograms();
    p_ProgramList3->AddProgram(p_Program1);
    QCOMPARE(p_ProgramList3->GetNumberOfPrograms(), NumberOfPrograms + 1);

    //Check contents of program added
    CProgram *p_Program2 = new CProgram();
    p_Program2 = const_cast<CProgram*>(p_ProgramList3->GetProgram("U100"));
    QCOMPARE(p_Program2->GetLongName(), QString("Eosin") );
//    QCOMPARE(p_Program2->GetShortName(), QString("E"));
//    QCOMPARE(p_Program2->GetColor(), QString("Green"));
    QCOMPARE(p_Program2->GetID(), QString("U100"));
//    QCOMPARE(p_Program2->IsLocked(), true);
    QCOMPARE(p_Program2->GetNumberOfSteps(), 1);

    //Check contents of the step added
    CProgramStep *p_ProgStep1 = new CProgramStep();
    p_ProgStep1 = const_cast<CProgramStep*>(p_Program2->GetProgramStep(0));
//    QCOMPARE(p_ProgStep1->GetExclusive(), false);
    QCOMPARE(p_ProgStep1->GetReagentID(), QString("U2"));
//    QCOMPARE(p_ProgStep1->GetNumberOfParallelStations(), p_ProgStep->GetNumberOfParallelStations());
//    QCOMPARE(p_ProgStep1->GetMinDurationInSeconds(), p_ProgStep->GetMinDurationInSeconds());
//    QCOMPARE(p_ProgStep1->GetMinDuration(), p_ProgStep->GetMinDuration());
//    QCOMPARE(p_ProgStep1->GetIntensity(), p_ProgStep->GetIntensity());

    //check for the update program an dprogram step methods
    p_Program2->UpdateProgramStep(p_ProgStep1);
    p_ProgramList3->UpdateProgram(p_Program2);

    QCOMPARE(p_ProgramList->GetNextFreeProgID(true), QString("U33"));
    QCOMPARE(p_ProgramList->DeleteAllPrograms(), true);
    QCOMPARE(p_ProgramList2->DeleteAllPrograms(), true);
    QCOMPARE(p_ProgramList3->DeleteAllPrograms(), true);


    //delete p_Program2;
    delete p_ProgramList3;
    delete p_ProgramList2;
    delete p_ProgramList;
}

void TestDataProgramList::utTestDataProgramListVerifier() {
    CDataProgramList *p_ProgramList = new CDataProgramList();
    CDataProgramListVerifier *p_ProgramListVerifier = new CDataProgramListVerifier();
    p_ProgramList->AddVerifier(p_ProgramListVerifier);
    p_ProgramList->SetDataVerificationMode(false);
    QCOMPARE(p_ProgramList->Read(":/Xml/ProgramsNOK.xml"), true);
    p_ProgramList->SetDataVerificationMode(true);
    CProgram *p_Program1 = new CProgram();

    p_Program1->SetID("U01");
    p_Program1->SetLongName("Eosinaaaaaaaaaaaaaaaaaaaaaaaa");
//    p_Program1->SetShortName("E11");
//    p_Program1->SetColor("green");
//    p_Program1->LockProgram();

//    CProgramStep *p_ProgStep = new CProgramStep(p_Program1->GetNextFreeStepID(true),
//                                                QString("U2"), 86400, 10, false, 1, 0);
//    QCOMPARE(p_Program1->AddProgramStep(p_ProgStep), true);
//    QCOMPARE(p_ProgramList->AddProgram(p_Program1), false);
//    QCOMPARE(p_ProgramList->GetNextFreeProgID(true), QString("U4711"));
}

} // end namespace DataManager

QTEST_MAIN(DataManager::TestDataProgramList)

#include "TestDataProgramList.moc"


/****************************************************************************/
/*! \file TestDataModuleList.cpp
 *
 *  \brief Implementation file for class TestDataCReagent.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2013-01-09
 *  $Author:    $ Soumya. D
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

#include "Global/Include/Exception.h"
#include "Global/Include/Utils.h"

#include "DataManager/Containers/InstrumentHistory/Include/ModuleDataList.h"
#include "DataManager/Containers/InstrumentHistory/Include/ModuleDataListVerifier.h"
#include "DataManager/Containers/InstrumentHistory/Commands/Include/CmdModuleListUpdate.h"

namespace DataManager {

/****************************************************************************/
/**
 * \brief Test class for CModuleDataList class.
 */
/****************************************************************************/
class TestDataModuleList : public QObject {
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
    void utTestSubModule();

    /****************************************************************************/
    /**
     * \brief Test data CModule
     */
    /****************************************************************************/
    void utTestModule();
    /****************************************************************************/
    /**
     * \brief Test data CModuleList
     */
    /****************************************************************************/
    void utTestDataModuleList();

    /****************************************************************************/
    /**
     * \brief Test data for CDataModuleListVerifier.
     */
    /****************************************************************************/
    void utTestDataModuleListVerifier();

    /****************************************************************************/
    /**
     * \brief Test data for CommandModuleListUpdate.
     */
    /****************************************************************************/
    void utTestCommandModuleListUpdate();

}; // end class TestDataModuleList

/****************************************************************************/
void TestDataModuleList::initTestCase() {
}

/****************************************************************************/
void TestDataModuleList::init() {
}

/****************************************************************************/
void TestDataModuleList::cleanup() {
}

/****************************************************************************/
void TestDataModuleList::cleanupTestCase() {
}

/****************************************************************************/
void TestDataModuleList::utTestSubModule()
{    

    CSubModule *p_SubModule1 = new CSubModule();

    p_SubModule1->SetSubModuleName("asb4_0");
    p_SubModule1->SetSubModuleType("Board");
    p_SubModule1->SetSubModuleDescription("DisplayName");

    p_SubModule1->AddParameterInfo("SerialNumber","SN");
    p_SubModule1->AddParameterInfo("EndTestResult","TestRes");
    p_SubModule1->AddParameterInfo("EndTestDate","Date");
    p_SubModule1->AddParameterInfo("HardwareMajorVersion","HardwareVer");

    //Test Getters
    QCOMPARE(p_SubModule1->GetSubModuleName(), QString(tr("asb4_0")));
    QCOMPARE(p_SubModule1->GetSubModuleType(), QString(tr("Board")));
    QCOMPARE(p_SubModule1->GetSubModuleDescription(), QString(tr("DisplayName")));

    QCOMPARE(p_SubModule1->GetNumberOfParameters(),4);

    QCOMPARE(p_SubModule1->GetParameterInfo(0)->ParameterName, QString(tr("SerialNumber")));
    QCOMPARE(p_SubModule1->GetParameterInfo(0)->ParameterValue, QString(tr("SN")));

    QCOMPARE(p_SubModule1->GetParameterInfo(1)->ParameterName, QString(tr("EndTestResult")));
    QCOMPARE(p_SubModule1->GetParameterInfo(1)->ParameterValue, QString(tr("TestRes")));

    QCOMPARE(p_SubModule1->GetParameterInfo(2)->ParameterName, QString(tr("EndTestDate")));
    QCOMPARE(p_SubModule1->GetParameterInfo(2)->ParameterValue, QString(tr("Date")));

    QCOMPARE(p_SubModule1->GetParameterInfo(3)->ParameterName, QString(tr("HardwareMajorVersion")));
    QCOMPARE(p_SubModule1->GetParameterInfo(3)->ParameterValue, QString(tr("HardwareVer")));       


    CSubModule *p_SubModule2 = new CSubModule(*p_SubModule1);
    delete p_SubModule1;

    //Test Getters
    QCOMPARE(p_SubModule2->GetSubModuleName(),QString(tr("asb4_0")));
    QCOMPARE(p_SubModule2->GetSubModuleType(),QString(tr("Board")));
    QCOMPARE(p_SubModule2->GetSubModuleDescription(),QString(tr("DisplayName")));

    QCOMPARE(p_SubModule2->GetNumberOfParameters(),4);

    QCOMPARE(p_SubModule2->GetParameterInfo(0)->ParameterName, QString(tr("SerialNumber")));
    QCOMPARE(p_SubModule2->GetParameterInfo(0)->ParameterValue, QString(tr("SN")));

    QCOMPARE(p_SubModule2->GetParameterInfo(1)->ParameterName, QString(tr("EndTestResult")));
    QCOMPARE(p_SubModule2->GetParameterInfo(1)->ParameterValue, QString(tr("TestRes")));

    QCOMPARE(p_SubModule2->GetParameterInfo(2)->ParameterName, QString(tr("EndTestDate")));
    QCOMPARE(p_SubModule2->GetParameterInfo(2)->ParameterValue, QString(tr("Date")));

    QCOMPARE(p_SubModule2->GetParameterInfo(3)->ParameterName, QString(tr("HardwareMajorVersion")));
    QCOMPARE(p_SubModule2->GetParameterInfo(3)->ParameterValue, QString(tr("HardwareVer")));

    p_SubModule2->UpdateParameterInfo("EndTestResult", "TestResult");

    CSubModule *p_SubModule3 = new CSubModule();
    *p_SubModule3 = *p_SubModule2;
    delete p_SubModule2;

    //Test Getters
    QCOMPARE(p_SubModule3->GetSubModuleName(),QString(tr("asb4_0")));
    QCOMPARE(p_SubModule3->GetSubModuleType(),QString(tr("Board")));
    QCOMPARE(p_SubModule3->GetSubModuleDescription(),QString(tr("DisplayName")));

    QCOMPARE(p_SubModule3->GetNumberOfParameters(),4);

    QCOMPARE(p_SubModule3->GetParameterInfo(0)->ParameterName, QString(tr("SerialNumber")));
    QCOMPARE(p_SubModule3->GetParameterInfo(0)->ParameterValue, QString(tr("SN")));

    QCOMPARE(p_SubModule3->GetParameterInfo(1)->ParameterName, QString(tr("EndTestResult")));
    QCOMPARE(p_SubModule3->GetParameterInfo(1)->ParameterValue, QString(tr("TestResult")));

    QCOMPARE(p_SubModule3->GetParameterInfo(2)->ParameterName, QString(tr("EndTestDate")));
    QCOMPARE(p_SubModule3->GetParameterInfo(2)->ParameterValue, QString(tr("Date")));

    QCOMPARE(p_SubModule3->GetParameterInfo(3)->ParameterName, QString(tr("HardwareMajorVersion")));
    QCOMPARE(p_SubModule3->GetParameterInfo(3)->ParameterValue, QString(tr("HardwareVer")));

    CSubModule *p_SubModule4 = new CSubModule("asb4_0");
    QCOMPARE(p_SubModule4->GetSubModuleName(), QString(tr("asb4_0")));

    QByteArray *p_TempByteArray = new QByteArray();
    QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
    DataStream.setVersion(QDataStream::Qt_4_0);
    p_TempByteArray->clear();
    DataStream << *p_SubModule3;
    DataStream.device()->reset();
    DataStream >> *p_SubModule4;
    QCOMPARE(p_SubModule4->GetNumberOfParameters(), p_SubModule3->GetNumberOfParameters());

    delete p_SubModule4;
    delete p_SubModule3;

}

/****************************************************************************/
void TestDataModuleList::utTestModule()
{

    CModule *p_Module1 = new CModule();

    p_Module1->SetModuleName(QString(tr("Drawer Left")));
    p_Module1->SetModuleDescription(QString(tr("Loader")));
    p_Module1->SetSerialNumber(QString(tr("SN_LOADER_00001")));
    p_Module1->SetOperatingHours(QString(tr("Hrs")));
    p_Module1->SetCalibrationDate(QString(tr("DATE2")));
    p_Module1->SetCalibrationResult(QString(tr("PASS")));
    p_Module1->SetTestDate(QString(tr("DATE2")));
    p_Module1->SetTestResult(QString(tr("PASS")));

    CSubModule *p_SubModule1 = new CSubModule();
    p_SubModule1->SetSubModuleName("asb4_0");
    p_SubModule1->SetSubModuleType("Board");
    p_SubModule1->SetSubModuleDescription("DisplayName");
    p_Module1->AddSubModuleInfo(p_SubModule1);

    CSubModule *p_SubModule2 = new CSubModule();
    p_SubModule2->SetSubModuleName("asb5_0");
    p_SubModule2->SetSubModuleType("Board");
    p_SubModule2->SetSubModuleDescription("DisplayName");
    p_Module1->AddSubModuleInfo(p_SubModule2);

    //Test Getters
    QCOMPARE(p_Module1->GetModuleName(),QString(tr("Drawer Left")));
    QCOMPARE(p_Module1->GetModuleDescription(),QString(tr("Loader")));
    QCOMPARE(p_Module1->GetSerialNumber(),QString(tr("SN_LOADER_00001")));
    QCOMPARE(p_Module1->GetOperatingHours(),QString(tr("Hrs")));
    QCOMPARE(p_Module1->GetCalibrationDate(),QString(tr("DATE2")));
    QCOMPARE(p_Module1->GetCalibrationResult(),QString(tr("PASS")));
    QCOMPARE(p_Module1->GetTestDate(),QString(tr("DATE2")));
    QCOMPARE(p_Module1->GetTestResult(),QString(tr("PASS")));

    QCOMPARE(p_Module1->GetNumberofSubModules(), 2);

    QCOMPARE(p_Module1->GetSubModuleInfo(0)->GetSubModuleName(),QString(tr("asb4_0")));
    QCOMPARE(p_Module1->GetSubModuleInfo(0)->GetSubModuleType(),QString(tr("Board")));
    QCOMPARE(p_Module1->GetSubModuleInfo(0)->GetSubModuleDescription(),QString(tr("DisplayName")));

    QCOMPARE(p_Module1->GetSubModuleInfo(1)->GetSubModuleName(),QString(tr("asb5_0")));
    QCOMPARE(p_Module1->GetSubModuleInfo(1)->GetSubModuleType(),QString(tr("Board")));
    QCOMPARE(p_Module1->GetSubModuleInfo(1)->GetSubModuleDescription(),QString(tr("DisplayName")));


    CModule *p_Module2 = new CModule(*p_Module1);
    delete p_Module1;

    //Test Getters
    QCOMPARE(p_Module2->GetModuleName(),QString(tr("Drawer Left")));
    QCOMPARE(p_Module2->GetModuleDescription(),QString(tr("Loader")));
    QCOMPARE(p_Module2->GetSerialNumber(),QString(tr("SN_LOADER_00001")));
    QCOMPARE(p_Module2->GetOperatingHours(),QString(tr("Hrs")));
    QCOMPARE(p_Module2->GetCalibrationDate(),QString(tr("DATE2")));
    QCOMPARE(p_Module2->GetCalibrationResult(),QString(tr("PASS")));
    QCOMPARE(p_Module2->GetTestDate(),QString(tr("DATE2")));
    QCOMPARE(p_Module2->GetTestResult(),QString(tr("PASS")));

    QCOMPARE(p_Module2->GetNumberofSubModules(), 2);

    QCOMPARE(p_Module2->GetSubModuleInfo(0)->GetSubModuleName(),QString(tr("asb4_0")));
    QCOMPARE(p_Module2->GetSubModuleInfo(0)->GetSubModuleType(),QString(tr("Board")));
    QCOMPARE(p_Module2->GetSubModuleInfo(0)->GetSubModuleDescription(),QString(tr("DisplayName")));

    QCOMPARE(p_Module2->GetSubModuleInfo(1)->GetSubModuleName(),QString(tr("asb5_0")));
    QCOMPARE(p_Module2->GetSubModuleInfo(1)->GetSubModuleType(),QString(tr("Board")));
    QCOMPARE(p_Module2->GetSubModuleInfo(1)->GetSubModuleDescription(),QString(tr("DisplayName")));


    CModule* p_Module3 = new CModule();
    *p_Module3 = *p_Module2;
    delete p_Module2;

    //Test Getters
    QCOMPARE(p_Module3->GetModuleName(),QString(tr("Drawer Left")));
    QCOMPARE(p_Module3->GetModuleDescription(),QString(tr("Loader")));
    QCOMPARE(p_Module3->GetSerialNumber(),QString(tr("SN_LOADER_00001")));
    QCOMPARE(p_Module3->GetOperatingHours(),QString(tr("Hrs")));
    QCOMPARE(p_Module3->GetCalibrationDate(),QString(tr("DATE2")));
    QCOMPARE(p_Module3->GetCalibrationResult(),QString(tr("PASS")));
    QCOMPARE(p_Module3->GetTestDate(),QString(tr("DATE2")));
    QCOMPARE(p_Module3->GetTestResult(),QString(tr("PASS")));

    QCOMPARE(p_Module3->GetNumberofSubModules(), 2);

    QCOMPARE(p_Module3->GetSubModuleInfo(0)->GetSubModuleName(),QString(tr("asb4_0")));
    QCOMPARE(p_Module3->GetSubModuleInfo(0)->GetSubModuleType(),QString(tr("Board")));
    QCOMPARE(p_Module3->GetSubModuleInfo(0)->GetSubModuleDescription(),QString(tr("DisplayName")));

    CSubModule *p_GetSubModuleInfo = new CSubModule();
    *p_GetSubModuleInfo = *p_Module3->GetSubModuleInfo("asb5_0");

    QCOMPARE(p_GetSubModuleInfo->GetSubModuleName(), QString(tr("asb5_0")));
    QCOMPARE(p_GetSubModuleInfo->GetSubModuleType(), QString(tr("Board")));
    QCOMPARE(p_GetSubModuleInfo->GetSubModuleDescription(), QString(tr("DisplayName")));

    delete p_GetSubModuleInfo;
    delete p_Module3;

    CModule *p_Module = new CModule("Agitator");
    p_Module->SetOperatingHours("Hrs");

    CSubModule *p_subModule = new CSubModule();
    p_subModule->SetSubModuleName("motor_agitation");
    p_subModule->SetSubModuleType("Stepper Motor");
    p_subModule->SetSubModuleDescription("displayName");
    p_Module->AddSubModuleInfo(p_subModule);

    p_subModule->SetSubModuleDescription("Display Name");
    bool Result = p_Module->UpdateSubModule(p_subModule);
    QCOMPARE(Result, true);

    CModule *p_TempModule = new CModule();

    QByteArray *p_TempByteArray = new QByteArray();
    QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
    DataStream.setVersion(QDataStream::Qt_4_0);
    p_TempByteArray->clear();
    DataStream << *p_Module;
    DataStream.device()->reset();
    DataStream >> *p_TempModule;
    QCOMPARE(p_TempModule->GetNumberofSubModules(), p_Module->GetNumberofSubModules());

    delete p_subModule;
    delete p_TempModule;
    delete p_Module;
}

/****************************************************************************/
void TestDataModuleList::utTestDataModuleList()
{
    CModuleDataList* p_ModuleList = new CModuleDataList("2012-08-12 09:19:49.611");      

    //get the container type
    QCOMPARE(p_ModuleList->GetDataContainerType(), INSTRUMENTHISTORY);

    p_ModuleList->SetFileName("");

    QCOMPARE(p_ModuleList->GetFilename(), QString(""));

    p_ModuleList->SetInstrumentName("ST8200");
    QCOMPARE(p_ModuleList->GetInstrumentName(), QString(tr("ST8200")));

    QCOMPARE(p_ModuleList->GetModuleTimeStamp(), QString(tr("2012-08-12 09:19:49.611")));

    p_ModuleList->SetModuleTimeStamp("2012-10-12 10:20:21.521");

    QCOMPARE(p_ModuleList->GetModuleTimeStamp(), QString(tr("2012-10-12 10:20:21.521")));

    // make the Data verification mode as false
    p_ModuleList->SetDataVerificationMode(false);
    QCOMPARE(p_ModuleList->GetDataVerificationMode(), false);

    // compare the number of modules
    QCOMPARE(p_ModuleList->GetNumberofModules(), 0);

    CModule* p_Module = new CModule("Drawer Left", "loader", "SN_LOADER_00001", "Hrs");

    CSubModule* p_SubModule1 = new CSubModule("asb4_0", "Board", "displayName");
    p_SubModule1->AddParameterInfo("SerialNumber", "QString", "SN");
    p_SubModule1->AddParameterInfo("EndTestResult", "TestResult_t", "TestResult");
    p_SubModule1->AddParameterInfo("EndTestDate", "QDate", "Date");
    p_Module->AddSubModuleInfo(p_SubModule1);

    CSubModule* p_SubModule2 = new CSubModule("asb4_0_motor_loader", "Stepper Motor", "displayName");
    p_SubModule2->AddParameterInfo("SoftwareVersion", "quint16", "VERSION");
    p_SubModule2->AddParameterInfo("RevolutionCounter", "quint32", "COUNT");
    p_SubModule2->AddParameterInfo("OperationTime", "minutes", "MINUTES");
    p_Module->AddSubModuleInfo(p_SubModule2);

    // add module
    QCOMPARE(p_ModuleList->AddModule(p_Module), true);

    // compare the number of modules
    QCOMPARE(p_ModuleList->GetNumberofModules(), 1);

    p_ModuleList->SetDataVerificationMode(true);
    QCOMPARE(p_ModuleList->GetDataVerificationMode(), true);

    CModule* p_Module1 = new CModule("Oven", "Oven", "SN_OVEN_00001", "Hrs");

    // add module
    QCOMPARE(p_ModuleList->AddModule(p_Module1), true);

    // compare the number of modules
    QCOMPARE(p_ModuleList->GetNumberofModules(), 2);

    QCOMPARE(p_ModuleList->DeleteModule("Oven"), true);
    QCOMPARE(p_ModuleList->GetNumberofModules(), 1);

    delete p_Module;
    delete p_Module1;
    delete p_ModuleList;

    CModuleDataList *p_ModuleDataList = new CModuleDataList();

    p_ModuleDataList->SetFileName(":/InstrumentHistory.xml");
    QFile File(p_ModuleDataList->GetFilename());
    p_ModuleDataList->ReadFile(p_ModuleDataList->GetFilename());
    p_ModuleDataList->DeserializeContent(File, false);

    p_ModuleDataList->SetFileName(":/InstrumentHistoryOut.xml");
    QFile OutFile(p_ModuleDataList->GetFilename());
    p_ModuleDataList->SerializeContent(OutFile, false);

    CModule *p_ModuleData = new CModule();
    p_ModuleData->SetModuleName(QString(tr("Drawer Left")));
    p_ModuleData->SetModuleDescription(QString(tr("Loader")));
    p_ModuleData->SetSerialNumber(QString(tr("SN_LOADER_00001")));
    p_ModuleData->SetOperatingHours(QString(tr("Hrs")));

    CSubModule *p_SubModuleData = new CSubModule();
    p_SubModuleData->SetSubModuleName("asb4_0");
    p_SubModuleData->SetSubModuleType("Board");
    p_SubModuleData->SetSubModuleDescription("DisplayName");
    p_ModuleData->AddSubModuleInfo(p_SubModuleData);

    p_ModuleDataList->AddModule(p_ModuleData);

    p_ModuleData->SetSerialNumber(QString(tr("SN_LOADER_00002")));
    p_ModuleData->SetOperatingHours(QString(tr("Hours")));
    p_ModuleDataList->UpdateModule(p_ModuleData);

    p_ModuleData->SetSerialNumber(QString(tr("SN_LOADER_00003")));
    p_ModuleData->SetOperatingHours(QString(tr("Hours2")));
    p_ModuleDataList->SetDataVerificationMode(true);
    p_ModuleDataList->UpdateModule(p_ModuleData);

    bool Res = p_ModuleDataList->GetModule("Drawer Left", *p_ModuleData);
    QCOMPARE(Res, true);

    CModuleDataList *p_ModuleListCopy = new CModuleDataList(*p_ModuleDataList);
    p_ModuleListCopy->SetDataVerificationMode(true);
    p_ModuleListCopy->SetFileName(":/InstrumentHistory.xml");

    QFile FileOpen(p_ModuleListCopy->GetFilename());
    p_ModuleListCopy->ReadFile(p_ModuleListCopy->GetFilename());
    p_ModuleListCopy->DeserializeContent(FileOpen, true);

    p_ModuleListCopy->SetFileName(":/InstrumentHistoryOut.xml");
    QFile OutFileOpen(p_ModuleDataList->GetFilename());
    p_ModuleListCopy->SerializeContent(OutFileOpen, true);

    QByteArray *p_TempByteArray = new QByteArray();
    QDataStream DataStream(p_TempByteArray, QIODevice::ReadWrite);
    DataStream.setVersion(QDataStream::Qt_4_0);
    p_TempByteArray->clear();
    DataStream << *p_ModuleDataList;
    DataStream.device()->reset();
    DataStream >> *p_ModuleListCopy;
    QCOMPARE(p_ModuleDataList->GetInstrumentName(), p_ModuleListCopy->GetInstrumentName());

    delete p_ModuleListCopy;
    delete p_SubModuleData;
    delete p_ModuleData;
    delete p_ModuleDataList;

}

/****************************************************************************/
void TestDataModuleList::utTestDataModuleListVerifier()
{
    CModuleDataList *p_ModuleList = new CModuleDataList();
    CModuleDataListVerifier *p_ModuleListVerifier = new CModuleDataListVerifier();

    p_ModuleList->SetInstrumentName("ST8200");

    if (!p_ModuleList->AddVerifier(p_ModuleListVerifier)) {
        QFAIL("Compiler never should come to this statement");
    }

    p_ModuleList->SetDataVerificationMode(false);
    CModule *p_Module1 = new CModule();

    p_Module1->SetModuleName(QString(tr("Drawer Left")));
    p_Module1->SetModuleDescription(QString(tr("Loader")));
    p_Module1->SetOperatingHours(QString(tr("Hrs")));

    CSubModule *p_SubModule = new CSubModule();
    p_SubModule->SetSubModuleName(QString(tr("asb4_0")));
    p_SubModule->SetSubModuleType(QString(tr("Board")));
    p_SubModule->SetSubModuleDescription(QString(tr("displayName")));

    p_SubModule->AddParameterInfo("SerialNumber", "QString", "SN");

    QCOMPARE(p_Module1->AddSubModuleInfo(p_SubModule), true);
    QCOMPARE(p_ModuleList->AddModule(p_Module1), true);

    QCOMPARE(p_ModuleList->VerifyData(), true);

    delete p_SubModule;
    delete p_Module1;
    delete p_ModuleList;
    delete p_ModuleListVerifier;
}

/****************************************************************************/
void TestDataModuleList::utTestCommandModuleListUpdate()
{
    MsgClasses::CmdModuleListUpdate *p_CmdModuleList = new MsgClasses::CmdModuleListUpdate();
    p_CmdModuleList->NAME = "MsgClasses::CmdModuleListUpdate";

    QCOMPARE(p_CmdModuleList->GetName(), QString(tr("MsgClasses::CmdModuleListUpdate")));

    CModule *p_Module1 = new CModule();

    p_Module1->SetModuleName(QString(tr("Drawer Left")));
    p_Module1->SetModuleDescription(QString(tr("Loader")));
    p_Module1->SetOperatingHours(QString(tr("Hrs")));

    CSubModule *p_SubModule = new CSubModule();
    p_SubModule->SetSubModuleName(QString(tr("asb4_0")));
    p_SubModule->SetSubModuleType(QString(tr("Board")));
    p_SubModule->SetSubModuleDescription(QString(tr("displayName")));

    p_SubModule->AddParameterInfo("SerialNumber", "QString", "SN");

    QCOMPARE(p_Module1->AddSubModuleInfo(p_SubModule), true);

    MsgClasses::CmdModuleListUpdate *p_CmdModuleListParam = new MsgClasses::CmdModuleListUpdate(1000,*p_Module1);
    QCOMPARE(p_CmdModuleListParam->GetName(), QString(tr("MsgClasses::CmdModuleListUpdate")));

    delete p_CmdModuleListParam;
    delete p_SubModule;
    delete p_Module1;
    delete p_CmdModuleList;
}

}


QTEST_MAIN(DataManager::TestDataModuleList)

#include "TestDataModuleList.moc"


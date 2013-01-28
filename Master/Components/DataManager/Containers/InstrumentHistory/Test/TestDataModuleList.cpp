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
     * \brief Test data of CDataModuleListVerifier.
     */
    /****************************************************************************/
    void utTestDataModuleListVerifier();

}; // end class TestDataModuleList

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
    QCOMPARE(p_SubModule3->GetParameterInfo(1)->ParameterValue, QString(tr("TestRes")));

    QCOMPARE(p_SubModule3->GetParameterInfo(2)->ParameterName, QString(tr("EndTestDate")));
    QCOMPARE(p_SubModule3->GetParameterInfo(2)->ParameterValue, QString(tr("Date")));

    QCOMPARE(p_SubModule3->GetParameterInfo(3)->ParameterName, QString(tr("HardwareMajorVersion")));
    QCOMPARE(p_SubModule3->GetParameterInfo(3)->ParameterValue, QString(tr("HardwareVer")));

    delete p_SubModule3;
}

void TestDataModuleList::utTestModule()
{

    CModule *p_Module1 = new CModule();

    p_Module1->SetModuleName(QString(tr("Drawer Left")));
    p_Module1->SetModuleDescription(QString(tr("Loader")));
    p_Module1->SetSerialNumber(QString(tr("SN_LOADER_00001")));
    p_Module1->SetOperatingHours(QString(tr("Hrs")));
    p_Module1->SetDateOfProduction(QString(tr("DATE2")));

    //Test Getters
    QCOMPARE(p_Module1->GetModuleName(),QString(tr("Drawer Left")));
    QCOMPARE(p_Module1->GetModuleDescription(),QString(tr("Loader")));
    QCOMPARE(p_Module1->GetSerialNumber(),QString(tr("SN_LOADER_00001")));
    QCOMPARE(p_Module1->GetOperatingHours(),QString(tr("Hrs")));
    QCOMPARE(p_Module1->GetDateOfProduction(),QString(tr("DATE2")));


    CModule *p_Module2 = new CModule(*p_Module1);
    delete p_Module1;

    //Test Getters
    QCOMPARE(p_Module2->GetModuleName(),QString(tr("Drawer Left")));
    QCOMPARE(p_Module2->GetModuleDescription(),QString(tr("Loader")));
    QCOMPARE(p_Module2->GetSerialNumber(),QString(tr("SN_LOADER_00001")));
    QCOMPARE(p_Module2->GetOperatingHours(),QString(tr("Hrs")));
    QCOMPARE(p_Module2->GetDateOfProduction(),QString(tr("DATE2")));


    CModule* p_Module3 = new CModule();
    *p_Module3 = *p_Module2;
    delete p_Module2;

    //Test Getters
    QCOMPARE(p_Module3->GetModuleName(),QString(tr("Drawer Left")));
    QCOMPARE(p_Module3->GetModuleDescription(),QString(tr("Loader")));
    QCOMPARE(p_Module3->GetSerialNumber(),QString(tr("SN_LOADER_00001")));
    QCOMPARE(p_Module3->GetOperatingHours(),QString(tr("Hrs")));
    QCOMPARE(p_Module3->GetDateOfProduction(),QString(tr("DATE2")));

    delete p_Module3;

}
void TestDataModuleList::utTestDataModuleList()
{


    CModuleDataList* p_ModuleList = new CModuleDataList("2012-08-12 09:19:49.611");

    // make the Data verification mode as false
    p_ModuleList->SetDataVerificationMode(false);

    // compare the number of modules
    QCOMPARE(p_ModuleList->GetNumberofModules(), 0);

    CModule* p_Module = new CModule("Drawer Left", "loader", "SN_LOADER_00001", "Hrs", "DATE2");


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

    CModule* p_Module1 = new CModule("Oven", "Oven", "SN_OVEN_00001", "Hrs", "DATE2");

    // add module
    QCOMPARE(p_ModuleList->AddModule(p_Module1), true);

    // compare the number of modules
    QCOMPARE(p_ModuleList->GetNumberofModules(), 2);

    QCOMPARE(p_ModuleList->DeleteModule("Oven"), true);
    QCOMPARE(p_ModuleList->GetNumberofModules(), 1);

    delete p_Module1;
    delete p_Module;
    delete p_ModuleList;


}

void TestDataModuleList::utTestDataModuleListVerifier()
{

    CModuleDataList *p_ModuleList = new CModuleDataList();
    CModuleDataListVerifier *p_ModuleListVerifier = new CModuleDataListVerifier();
    p_ModuleList->AddVerifier(p_ModuleListVerifier);

    p_ModuleList->SetDataVerificationMode(true);
    CModule *p_Module1 = new CModule();

    p_Module1->SetModuleName(QString(tr("Drawer Left")));
    p_Module1->SetModuleDescription(QString(tr("Loader")));
    p_Module1->SetOperatingHours(QString(tr("Hrs")));
    p_Module1->SetDateOfProduction(QString(tr("DATE2")));

    CSubModule *p_SubModule = new CSubModule();
    p_SubModule->SetSubModuleName(QString(tr("asb4_0")));
    p_SubModule->SetSubModuleName(QString(tr("Board")));
    p_SubModule->SetSubModuleName(QString(tr("displayName")));

    QCOMPARE(p_Module1->AddSubModuleInfo(p_SubModule), true);
    QCOMPARE(p_ModuleList->AddModule(p_Module1), false);


}

}


QTEST_MAIN(DataManager::TestDataModuleList)

#include "TestDataModuleList.moc"


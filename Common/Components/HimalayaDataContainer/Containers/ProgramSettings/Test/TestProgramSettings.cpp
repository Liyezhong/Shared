/****************************************************************************/
/*! \file TestProcessSettings.cpp
 *
 *  \brief Unit test for Process settings Container
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
#include <QDataStream>
#include <QByteArray>
#include <QFile>
#include <stdio.h>
#include "DataManager/Helper/Include/Types.h"

#include "HimalayaDataContainer/Containers/ProgramSettings/Include/ProgramSettings.h"
#include "DeviceControl/Include/Global/DeviceControlGlobal.h"

#include "HimalayaDataContainer/SpecialVerifiers/Include/SpecialVerifierGroupA.h"
#include "HimalayaDataContainer/SpecialVerifiers/Include/SpecialVerifierGroupC.h"
#include "HimalayaDataContainer/SpecialVerifiers/Include/SpecialVerifierGroupD.h"

#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdNewProgram.h"
#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdProgramDeleteItem.h"
#include "HimalayaDataContainer/Containers/Programs/Commands/Include/CmdProgramUpdate.h"

#include "HimalayaDataContainer/Containers/Programs/Include/Program.h"
#include "HimalayaDataContainer/Containers/Programs/Include/ProgramBase.h"
#include "HimalayaDataContainer/Containers/Programs/Include/ProgramStep.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramList.h"
#include "HimalayaDataContainer/Containers/Programs/Include/DataProgramListVerifier.h"

namespace MsgClasses {
    class CmdNewProgram;
    class CmdProgramDeleteItem;
    class CmdProgramUpdate;
}


namespace DataManager {

static QString FileNameWrite;
const QString XmlFileName = ":/Xml/ProgramSettings.xml";

/****************************************************************************/
/**
 * \brief Test class for Program sequence class.
 */
/****************************************************************************/
class TestProgramSettings : public QObject
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
    void utTestReadXml();
    void utTestLeftWriteXml();
    void utTestRightWriteXml();
    void utTestWriteXml();

    void utTestSpecialVerifyA();
    void utTestSpecialVerifyC();
    void utTestSpecialVerifyD();

    void utTestCmdNewProgram();
    void utTestCmdProgramDel();
    void utTestCmdProgramUpdate();

    void utTestCProgram();
    void utTestCProgramBase();
    void utTestCProgramStep();

    void utTestDataProgramList();
    void utTestDataProgramListVerify();
};

/****************************************************************************/
void TestProgramSettings::initTestCase()
{
    FileNameWrite = QCoreApplication::applicationDirPath() + "/ProgramSettings.xml";
}

/****************************************************************************/
void TestProgramSettings::init()
{
}

/****************************************************************************/
void TestProgramSettings::cleanup()
{
    QFile::remove("ProgramSettings.xml");
}

/****************************************************************************/
void TestProgramSettings::cleanupTestCase()
{
}

/****************************************************************************/
void TestProgramSettings::utTestReadXml()
{
    CProgramSettings *p_ParameterSrc = new CProgramSettings();

    if (p_ParameterSrc->Read(XmlFileName) == false)
    {
       ;//QFAIL("Reading ProcessSettings.xml failed");
    }

    p_ParameterSrc->GetVersion();
    p_ParameterSrc->GetDataContainerType();
    p_ParameterSrc->VerifyData(true, true);
    p_ParameterSrc->VerifyData(true, false);
    p_ParameterSrc->VerifyData(false, true);
    p_ParameterSrc->VerifyData(false, false);

    p_ParameterSrc->Write(FileNameWrite);
}

void TestProgramSettings::utTestLeftWriteXml()
{
    CProgramSettings *p_ParameterSrc = new CProgramSettings();

    if (p_ParameterSrc->Read(XmlFileName) == false)
    {
        ;//QFAIL("Reading ProgramSettings.xml failed");
    }

    p_ParameterSrc->GetVersion();
    p_ParameterSrc->GetDataContainerType();
}

void TestProgramSettings::utTestRightWriteXml()
{
    CProgramSettings *p_ParameterSrc = new CProgramSettings();
    if (p_ParameterSrc->Read(XmlFileName) == false)
    {
        ;//QFAIL("Reading ProcessSettings.xml failed");
    }

    p_ParameterSrc->GetVersion();
    p_ParameterSrc->GetDataContainerType();
}


void TestProgramSettings::utTestWriteXml()
{
    CProgramSettings *p_ParameterSrc = new CProgramSettings();
    if (p_ParameterSrc->Read(XmlFileName) == false)
    {
        ;//QFAIL("Reading ProcessSettings.xml failed");
    }

    p_ParameterSrc->GetVersion();
    p_ParameterSrc->GetDataContainerType();
}

void TestProgramSettings::utTestSpecialVerifyA()
{
    CSpecialVerifierGroupA *veriA  = new CSpecialVerifierGroupA(NULL, NULL, NULL,NULL);
    CProgramSettings *p_ParameterSrc = new CProgramSettings();
    p_ParameterSrc->AddVerifier(veriA);
    if (p_ParameterSrc->Read(XmlFileName) == false)
    {
     ;//QFAIL("Reading ProcessSettings.xml failed");
    }
}

void TestProgramSettings::utTestSpecialVerifyC()
{
    CSpecialVerifierGroupC *veriC  = new CSpecialVerifierGroupC(NULL, NULL);
    CProgramSettings *p_ParameterSrc = new CProgramSettings();
    p_ParameterSrc->AddVerifier(veriC);
    if (p_ParameterSrc->Read(XmlFileName) == false)
    {
        ;//QFAIL("Reading ProcessSettings.xml failed");
    }
}

void TestProgramSettings::utTestSpecialVerifyD()
{
    CSpecialVerifierGroupD *veriD  = new CSpecialVerifierGroupD(NULL);
    CProgramSettings *p_ParameterSrc = new CProgramSettings();
    p_ParameterSrc->AddVerifier(veriD);
    if (p_ParameterSrc->Read(XmlFileName) == false)
    {
        ;//QFAIL("Reading ProcessSettings.xml failed");
    }
}

void TestProgramSettings::utTestCmdNewProgram()
{
    MsgClasses::CmdNewProgram *newProgram = new MsgClasses::CmdNewProgram;
    newProgram->GetName();
    newProgram->GetProgramData();
    newProgram->GetProgramSequenceData();
    newProgram->GetTimeout();

    QDataStream ds;
    ds << (*newProgram);
    ds >> (*newProgram);
    delete newProgram;

}

void TestProgramSettings::utTestCmdProgramDel()
{
    MsgClasses::CmdProgramDeleteItem * delProgram = new MsgClasses::CmdProgramDeleteItem();
    delProgram->GetItemId();
    delProgram->GetName();
    delProgram->GetTimeout();

    QDataStream ds;
    ds << (*delProgram);
    ds >> (*delProgram);

    delete delProgram;

    // contructor
    MsgClasses::CmdProgramDeleteItem * delProgram2 = new MsgClasses::CmdProgramDeleteItem(10, QString("UT"));
    delete delProgram2;
}

void TestProgramSettings::utTestCmdProgramUpdate()
{
    MsgClasses::CmdProgramUpdate *updateProgram = new MsgClasses::CmdProgramUpdate();
    updateProgram->GetColorAssignedProgramData();
    updateProgram->GetName();
    updateProgram->GetProgramColorReplaced();
    updateProgram->GetProgramData();
    updateProgram->GetTimeout();

    updateProgram->setBusyStateAllowed(true);
    updateProgram->setErrorStateAllowed(true);
    updateProgram->setIdleStateAllowed(true);
    updateProgram->setInitFailedStateAllowed(true);
    updateProgram->SetProgramColorReplaced(true);
    updateProgram->setSoftSwitchMonitorStateAllowed(true);
    updateProgram->setUserActionStateAllowed(true);

    updateProgram->isBusyStateAllowed();
    updateProgram->isErrorStateAllowed();
    updateProgram->isIdleStateAllowed();
    updateProgram->isInitFailedStateAllowed();
    updateProgram->isSoftSwitchStateAllowed();
    updateProgram->isStateAllowed(QString("state"));
    updateProgram->isUserActionAllowed();

    QDataStream ds;
    ds << (*updateProgram);
    ds >> (*updateProgram);
    delete updateProgram;
}

void TestProgramSettings::utTestCProgram()
{
    CProgram prog2(QString("UT"));
    CProgram prog3(QString("UT"), QString("UT-Name"), QString("Longname"), false, QString("Nullicon"), false);
    prog2 = prog3;

    CProgram *program = new CProgram;
    program->GetIcon();
    program->GetID();
    program->GetLongName();
    program->GetNameID();
    program->GetName();
    program->GetNextFreeStepID(false);
    program->GetNumberOfSteps();
    program->GetProgramDurationInSeconds();

    CProgramStep s;
    program->GetProgramStep(0, s);
    program->GetReagentIDList();
    program->SetFavorite(false);
    program->SetIcon(QString("nullicon"));
    program->SetID(QString("id"));
    program->SetLeicaProgram(false);
    program->SetLongName(QString("longname"));
    program->SetName(QString("name"));
    program->SetNameID(QString("nameid"));
    program->SetNextFreeStepID(QString("L1"));

    CProgramStep *step = new CProgramStep;
    program->AddProgramStep(0, step);
    program->CreateProgramStep();
    program->CreateProgramStep(QString("L1"));
    program->DeleteAllProgramSteps();
    program->DeleteProgramStep(0); // step deleted!
    program->DeleteProgramStep(QString("L1"));
    program->InitObject();
    program->IsCleaningProgram();
    program->IsFavorite();
    program->IsLeicaProgram();
    program->UpdateProgramStep(NULL);

    QDataStream ds;
//    ds << prog3;
//    ds >> prog3;
    delete program;
}

void TestProgramSettings::utTestCProgramBase()
{
    CProgramBase progBase1(QString("id"), QString("name"));
    CProgramBase progBase2(QString("id"), QString("name"), QString("longname"));

    CProgramBase *progBase = new CProgramBase;
    progBase->GetID();
    progBase->GetLongName();
    progBase->GetName();
    progBase->GetNameID();
    progBase->GetNextFreeStepID(false);
    progBase->GetNumberOfSteps();

    CProgramStep step;
    progBase->GetProgramStep(0, step);
    progBase->GetProgramStep(0);
    progBase->GetProgramStep(QString("stepid"));

    progBase->SetID(QString("id"));
    progBase->SetLongName(QString("longname"));
    progBase->SetName(QString("name"));
    progBase->SetNameID(QString("nameid"));
    progBase->SetNextFreeStepID(QString("freestepid"));

    CProgramStep *s = new CProgramStep;
    progBase->AddProgramStep(0, s);
    progBase->CreateProgramStep();
    progBase->DeleteAllProgramSteps();
    progBase->DeleteProgramStep(0);
    progBase->DeleteProgramStep(QString("stepid"));

    progBase->InitObject();
    progBase->MoveProgramStep(0, 1);
    progBase->UpdateProgramStep(NULL);

    delete progBase;
}

void TestProgramSettings::utTestCProgramStep()
{
    CProgramStep pstep(QString("stepid"));
    CProgramStep pstep1(QString("stepid"), QString("reagentid"), QString("duration"), QString("temp"), QString("pressure"), QString("vaccum"));
    CProgramStep pstep2(pstep1);

    CProgramStep *step = new CProgramStep();
    step->GetDuration();
    step->GetDurationInSeconds();
    step->GetNumberOfSteps();
    step->GetPressure();
    step->GetReagentID();
    step->GetStationIDList();
    step->GetStepID();
    step->GetTemperature();
    step->GetVacuum();

    step->SetDuration(QString("duration"));
    step->SetDurationInSeconds(10);
    step->SetPressure(QString("pressure"));
    step->SetReagentID(QString("reagentid"));
    step->SetStationIDList(QStringList());
    step->SetStepID(QString("stepid"));
    step->SetTemperature(QString("temp"));
    step->SetVacuum(QString("vaccum"));

    delete step;
}

void TestProgramSettings::utTestDataProgramList()
{
    CDataProgramList dpl1;
    CDataProgramList dpl2 = CDataProgramList(dpl1);

    CDataProgramList *dataProgList = new CDataProgramList;
    dataProgList->SetDataVerificationMode(false);

    ErrorMap_t errtype;
    dataProgList->SetErrorList(&errtype);
    dataProgList->SetNextFreeProgID(QString("nextfreeid"));

    dataProgList->GetDataContainerType();
    dataProgList->GetDataVerificationMode();
    dataProgList->GetErrorList();
    dataProgList->GetFavoriteProgramIDs();
    dataProgList->GetFilename();
    dataProgList->GetNextFreeProgID(false);
    dataProgList->GetNumberOfPrograms();

    CProgram program;
    dataProgList->GetProgram(0, program);
    dataProgList->GetProgram(QString("programid"), program);
    dataProgList->GetProgram(0);
    dataProgList->GetProgram(QString("programid"));

    dataProgList->GetReagentIDList();
    dataProgList->GetVersion();

    dataProgList->CheckForUniquePropeties(&program, false);
    dataProgList->CreateProgram();
    dataProgList->DeleteProgram(0);
    dataProgList->DeleteProgram(QString("programid"));
    dataProgList->DeleteAllPrograms();

    dataProgList->DoGroupVerification();
    dataProgList->DoLocalVerification();
    dataProgList->Init();

    QString Filename("./test.xml");
    QFile File(Filename);
    if (File.open(QFile::ReadWrite | QFile::Text)) {
        dataProgList->Read(Filename);
        QFile::remove(Filename);
    }
    dataProgList->ProgramExists(QString("programid"));
    dataProgList->AddVerifier(NULL);

    delete dataProgList;
}

void TestProgramSettings::utTestDataProgramListVerify()
{
    CDataProgramListVerifier dplv(NULL);
    CDataProgramListVerifier *dataProgListV = new CDataProgramListVerifier;
    dataProgListV->GetErrors();
    dataProgListV->IsLocalVerifier();
    dataProgListV->ResetErrors();

    CDataProgramList dpl;
    dataProgListV->VerifyData(&dpl);
    delete dataProgListV;
}

} // end namespace DataManager

QTEST_MAIN(DataManager::TestProgramSettings)

#include "TestProgramSettings.moc"

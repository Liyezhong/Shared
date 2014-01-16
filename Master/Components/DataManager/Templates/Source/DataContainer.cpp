/****************************************************************************/
/*! \file DataContainer.cpp
 *
 *  \brief Implementation file for class CDataContainer.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-07-20
 *  $Author:    $ F. Toth
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
#include "DataManager/Templates/Include/DataContainer.h"
#include "TestStubDataManager/Containers/Racks/Include/RackList.h"
#include "TestStubDataManager/Containers/Racks/Include/RackListVerifier.h"

namespace DataManager {

CDataContainer::CDataContainer() : m_IsInitialized(false), ProgramList(NULL),
                                    ReagentList(NULL), StationList(NULL),
                                    Settings(NULL)
{
    if (!InitContainers()) {
        qDebug() << "CDataContainer::Constructor / InitContainers failed";
        Q_ASSERT(false);
    }
}

CDataContainer::~CDataContainer()
{
    if (!DeinitContainers()) {
        qDebug() << "CDataContainer::Destructor / DeinitContainers failed";
        Q_ASSERT(false);
    }
}

bool CDataContainer::InitContainers()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::InitContainers was already called";
        Q_ASSERT(false);
        return false;
    }

    ProgramList = new CDataProgramList();
    if (!ResetDCProgramList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCProgramList failed.";
        return false;
    }

    ReagentList = new CDataReagentList();
    if (!ResetDCReagentList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCReagentList failed.";
        return false;
    }

    StationList = new CDataStationList();
    if (!ResetDCStationList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCStationList failed!";
        return false;
    }

    ProgramSequenceList = new CProgramSequenceList();
    if (!ResetDCProgramSequenceList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetProgramSequenceList failed.";
        return false;
    }

    Settings = new CUserSettings();
    if (!ResetDCUserSettings()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCSettings failed!";
        return false;
    }

    RackList = new CDataRackList();
    if (!ResetDCRackList()) {
        qDebug() << "CDataContainer::InitContainers failed, because ResetDCRackList failed.";
        return false;
    }
//    // create special verifier for reagents and programs
//    IVerifierInterface *p_SpecialVerifier = new CSpecialVerifierGroupA(ProgramList, ReagentList);
//    // register this verifier object in the data containers (=> dependency injection)
//    if (!ProgramList->AddVerifier(p_SpecialVerifier)) {
//        qDebug() << "CDataContainer::InitContainers failed, because ProgramList->AddVerifier failed.";
//        return false;
//    }
//    if (!ReagentList->AddVerifier(p_SpecialVerifier)) {
//        qDebug() << "CDataContainer::InitContainers failed, because ReagentList->AddVerifier failed.";
//        return false;
//    }

    m_IsInitialized = true;
    return true;
}

bool CDataContainer::DeinitContainers()
{
    if (m_IsInitialized != true) {
        // nothing to do
        return true;
    }

    delete ProgramList;

    delete ReagentList;
    delete StationList;
    delete ProgramSequenceList;
    delete Settings;

    return true;
}



bool CDataContainer::ResetDCProgramList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCProgramList was already called";
        return false;
    }

    // init program list
    ProgramList->Init();


    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataProgramListVerifier = NULL;
    if (p_DataProgramListVerifier == NULL) {
        p_DataProgramListVerifier = new CDataProgramListVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    ProgramList->AddVerifier(p_DataProgramListVerifier);

    return true;
}

bool CDataContainer::ResetDCProgramSequenceList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCProgramList was already called";
        return false;
    }

    // init program list
    ProgramSequenceList->Init();


    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_ProgramSequenceListVerifier = NULL;
    if (p_ProgramSequenceListVerifier == NULL) {
        p_ProgramSequenceListVerifier = new CProgramSequenceListVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    ProgramSequenceList->AddVerifier(p_ProgramSequenceListVerifier);

    return true;
}

bool CDataContainer::ResetDCReagentList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCReagentList was already called";
        return false;
    }

    // init reagent list
    ReagentList->Init();

    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataReagentListVerifier = NULL;
    if (p_DataReagentListVerifier == NULL) {
        p_DataReagentListVerifier = new CDataReagentListVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    ReagentList->AddVerifier(p_DataReagentListVerifier);

    return true;
}

bool CDataContainer::ResetDCRackList()
{
    if (m_IsInitialized) {
        qDebug() << "CDataContainer::ResetDCReagentList was already called";
        return false;
    }

    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataRackListVerifier = NULL;
    if (p_DataRackListVerifier == NULL) {
        p_DataRackListVerifier = new CDataRackListVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    RackList->AddVerifier(p_DataRackListVerifier);

     return true;
}

bool CDataContainer::ResetDCStationList()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCStationList was already called";
        return false;
    }

    StationList->Init();

    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataStationListVerifier = NULL;
    if (p_DataStationListVerifier == NULL) {
        p_DataStationListVerifier = new CDataStationListVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    StationList->AddVerifier(p_DataStationListVerifier);

      return true;
}

bool CDataContainer::ResetDCUserSettings()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataContainer::ResetDCUserSettings was already called";
        return false;
    }
//
//    // init reagent list
//    Settings->SetDefaultValues();

    // create a verifier object for this data container, if not done before
    static IVerifierInterface *p_DataReagentListVerifier = NULL;
    if (p_DataReagentListVerifier == NULL) {
        p_DataReagentListVerifier = new CDataReagentListVerifier();
    }
    // register this verifier object in the data container (=> dependency injection)
    ReagentList->AddVerifier(p_DataReagentListVerifier);

    return true;
}

bool CDataContainer::RefreshProgramStepStationlist()
{
    bool Result = true;

    //Get Program ID's of Programs which are startable and used flag is set
    //from program sequence list
    QStringList StartableProgramList;
    StartableProgramList = ProgramSequenceList->GetStartableProgramIDList();
    CProgram* p_Program = NULL;
    CProgramStep* p_ProgStep = NULL;
    //For all the steps in the above programs generate possible station list
    for (qint32 I = 0; I < StartableProgramList.count(); I++) {
        p_Program = ProgramList->GetProgram(I);
        if (p_Program) {
            for (qint32 J = 0; J < p_Program->GetNumberOfStepsInExpandedList(); J++) {
                if (p_Program->GetProgramStepExpanded(J, p_ProgStep)) {
                    QString ReagentID = p_ProgStep->GetReagentID();
                    QString ProgramID = p_Program->GetID();
                    QString StepID = p_ProgStep->GetStepID();
                    QStringList PossibleStations = StationList->GetPossibleStations(ReagentID, ProgramID, StepID);
                    p_ProgStep->SetStationIDList(PossibleStations);
                    //Update the program steps of the program so that
                    //Possible station
                    if (!(p_Program->UpdateProgramStep(p_ProgStep))) {
                        qDebug("### CDataContainer::RefreshProgramStepStationlist() failed");
                        return false;
                    }
                }
                else {
                    return false;
                }
            }//End of prog step for loop
        }
        else {
            Result = false;
        }

    }// end of program for loop
    return Result;
}



bool CDataContainer::AddStepsToExpandedStepList(CProgram &Program)
{
    for (int I = 0; I < Program.GetNumberOfSteps(); I++) {

        CProgramStep* p_CurrentProgStep = new CProgramStep();
        CProgramStep NextProgStep;

        if (Program.GetProgramStep(I, *p_CurrentProgStep)) {
            qDebug()<<"\nCurrent PRogram"<<Program.GetID();
            qDebug()<<"\n\n p_CurrentProgStep StationID List"<< p_CurrentProgStep->GetStationIDList();
            m_ExpandedStepList.append(p_CurrentProgStep);

            /* I am not worrying about Index crossing the maximum , since GetProgramStep checks validity of index */
            if (Program.GetProgramStep(I + 1, NextProgStep)) {
                bool OK = true;
                //if the current step(i.e Next Prog Step) and previous step(i.e Current Prog Step) is in different zone
                //insert an dry station step inbetween
                if (!CompareSteps(*p_CurrentProgStep, NextProgStep, OK)) {
                    if (OK) {
                        CProgramStep* p_DryStationStep = new CProgramStep();
                        p_DryStationStep->SetStepID(Program.GetNextFreeStepID(true));
                        p_DryStationStep->SetReagentID("S9");
                        p_DryStationStep->SetMinDurationInSeconds(1);
                        p_DryStationStep->SetMaxDurationInPercent(MAX_DURATION_PERCENTAGE_DRY_STATION_STEP);
                        p_DryStationStep->SetNumberOfParallelStations(0);
                        p_DryStationStep->SetIntensity(0);
                        p_DryStationStep->SetStationIDList(StationList->GetPossibleStations(p_DryStationStep->GetReagentID(), Program.GetID(),
                                                  p_DryStationStep->GetStepID()));
                        m_ExpandedStepList.append(p_DryStationStep);
                    }
                    else {
                        return false;
                    }
                }
                //else no need to add dry station steps
            }
        }
    }//End of for loop
    return true;
}

bool CDataContainer::CompareSteps(CProgramStep& CurrentProgramStep, CProgramStep& NextProgramStep, bool& OK)
{
    QStringList StationIDList = CurrentProgramStep.GetStationIDList();
    StationZoneType_t CurrentZone;
    StationZoneType_t TemporaryCurrentZone;
    StationZoneType_t CurrentStepZone;
    StationZoneType_t NextStepZone;
    //To make sure that all the stations in the list are in the same zone.
    //Else the stations xml generated by BLG has a bug
    for (qint32 I = 0; I < StationIDList.count(); I++) {
        CurrentZone = StationList->GetStationZone(StationIDList.at(I));
        if ((I != 0) && CurrentZone != TemporaryCurrentZone) {
            qDebug("### FATAL ERROR Possible Stations for program step in different zone ");
            OK = false;
            return false;
        }
        TemporaryCurrentZone = CurrentZone;
    }
    CurrentStepZone = CurrentZone;
    StationIDList.clear();

    StationIDList = NextProgramStep.GetStationIDList();
    //To make sure that all the stations in the list are in the same zone.
    //Else the stations xml generated by BLG has a bug
    for (qint32 I = 0; I < StationIDList.count(); I++) {
        CurrentZone = StationList->GetStationZone(StationIDList.at(I));
        if ((I != 0) && CurrentZone != TemporaryCurrentZone) {
            qDebug("### FATAL ERROR Possible Stations for program step in different zone ");
            OK = false;
            return false;
        }
        TemporaryCurrentZone = CurrentZone;
    }

    NextStepZone = CurrentZone;

    //if zones are different , need to insert dry station step
    if ((CurrentZone == LEFT_ZONE && NextStepZone == RIGHT_ZONE) ||
        (CurrentZone == RIGHT_ZONE && NextStepZone == LEFT_ZONE))  {
        return false;
    }
    else {
        return true;
    }
}

}// namespace DataManager

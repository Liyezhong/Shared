/****************************************************************************/
/*! \file DataManager.cpp
 *
 *  \brief Implementation file for class CDataManager.
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
#include <Global/Include/SystemPaths.h>
#include "DataManager/Templates/Include/DataManager.h"


namespace DataManager {

CDataManager::CDataManager() : m_IsInitialized(false), mp_DataContainer(NULL)
{
    if (!InitDataContainer()) {
        qDebug() << "CDataManager::Constructor / InitDataContainer failed";
        Q_ASSERT(false);
    }
}

CDataManager::~CDataManager()
{
    if (!DeinitDataContainer()) {
        qDebug() << "CDataManager::Destructor / DeinitDataContainer failed";
        Q_ASSERT(false);
    }
}

bool CDataManager::InitDataContainer()
{
    if (m_IsInitialized == true) {
        qDebug() << "CDataManager::InitDataContainer was already called";
        Q_ASSERT(false);
        return false;
    }

    mp_DataContainer = new CDataContainer();

    mp_DataContainer->ProgramList->SetDataVerificationMode(false);
  //  QString FilenameProgramList = "C:\\programs_V3.xml";
    //QString FilenameProgramList = ":/Xml/Programs_V3.xml";
    QString FilenameProgramList = Global::SystemPaths::Instance().GetSettingsPath() + "/Programs.xml";
    if (!mp_DataContainer->ProgramList->Read(FilenameProgramList)) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->ProgramList->Read failed with filename: " << FilenameProgramList;
        Q_ASSERT(false);
        return false;
    }

    mp_DataContainer->ReagentList->SetDataVerificationMode(false);
   // QString FilenameReagentList = "C:\\reagents_V3.xml";""
    //QString FilenameReagentList = ":/Xml/Reagents_V3.xml";
    QString FilenameReagentList = Global::SystemPaths::Instance().GetSettingsPath() + "/Reagents.xml";
    if (!mp_DataContainer->ReagentList->Read(FilenameReagentList)) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->ReagentList->Read failed with filename: " << FilenameReagentList;
        Q_ASSERT(false);
        return false;
    }
 
    mp_DataContainer->StationList->SetDataVerificationMode(false);
  //  QString FilenameStationList = "C:\\Stations.xml";""
    //QString FilenameStationList = ":/Xml/Stations.xml";
    QString FilenameStationList = Global::SystemPaths::Instance().GetSettingsPath() + "/Stations.xml";
    if (!mp_DataContainer->StationList->Read(FilenameStationList)) {
        qDebug() << "CDataManager::InitDataContainer failed, \
                because mp_DataContainer->StationtList->Read failed with filename: "
                << FilenameStationList;
        Q_ASSERT(false);
        return false;
    }

    // when all containers are loaded, activate verification mode and verify there initial content
    mp_DataContainer->ProgramList->SetDataVerificationMode(true);
    mp_DataContainer->ReagentList->SetDataVerificationMode(true);
    mp_DataContainer->StationList->SetDataVerificationMode(true);

    if (!mp_DataContainer->ProgramList->VerifyData()) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->ProgramList->VerifyData failed.";
        return false;
    }
    if (!mp_DataContainer->ReagentList->VerifyData()) {
        qDebug() << "CDataManager::InitDataContainer failed, because mp_DataContainer->ReagentList->VerifyData failed.";
        return false;
    }
    if (!mp_DataContainer->StationList->VerifyData()) {
        qDebug() << "CDataManager::InitDataContainer failed,\
                    because mp_DataContainer->StationList->VerifyData failed.";
        return false;
    }
    qDebug() << "CDataManager::InitDataContainer stations passed";

    m_IsInitialized = true;
    return true;
}

bool CDataManager::DeinitDataContainer()
{
    if (m_IsInitialized != true) {
        // nothing to do
        return true;
    }

    delete mp_DataContainer;

    return true;
}


//********************************************************************
//some function to access the program list
//********************************************************************
//get copy of complete program list
bool CDataManager::GetProgramList(CDataProgramList* p_ProgramListCopy)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetProgramList failed. Not initialized!";
        return false;
    }

    if (p_ProgramListCopy == NULL) {
        qDebug() << "CDataManager::GetProgramList failed: p_ProgramListCopy must not be NULL!";
        return false;
    }

    *p_ProgramListCopy = *(mp_DataContainer->ProgramList);
     return true;
}

//delete program with given ID
bool CDataManager::DeleteProgram(QString ID)
{
//    if (m_IsInitialized != true) {
//        qDebug() << "CDataManager::DeleteProgram failed. Not initialized!";
//        return false;
//    }

//    if (!mp_DataContainer->ProgramList->DeleteProgram(ID)) {
//        qDebug() << "CDataManager::DeleteProgram failed.";
//        return false;
//    }

//    // TODO prepare command and send to master
//    // RESULT = DELETE_PROG(ID)
//    // return RESULT;

      return true;
}

//update program
bool CDataManager::UpdateProgram(CProgram* p_Program)
{
//    if (m_IsInitialized != true) {
//        qDebug() << "CDataManager::UpdateProgram failed. Not initialized!";
//        return false;
//    }

//    if (!mp_DataContainer->ProgramList->UpdateProgram(p_Program)) {
//        qDebug() << "CDataManager::UpdateProgram failed.";
//        return false;
//    }

//    // TODO prepare command and send to master
//    // RESULT = UPDATE_PROG(p_Program)
//    // return RESULT;
    return true;
}

// write reagent list to file
bool CDataManager::WriteProgramList(QString Filename)
{
    if (!mp_DataContainer->ProgramList->Write(Filename)) {
        qDebug() << "CDataManager::WriteProgramList failed.";
        return false;
    } else  {
        return true;
    }
}

// SerializeProgramList: content of program list => QByteArray
bool CDataManager::SerializeProgramList(QByteArray* p_ByteArray)
{
//    if (m_IsInitialized != true) {
//        qDebug() << "CDataManager::SerializeProgramList failed. Not initialized!";
//        return false;
//    }

//    if (!mp_DataContainer->ProgramList->SerializeContent(p_ByteArray)) {
//        qDebug() << "CDataManager::SerializeContent failed.";
//        return false;
//    }

    return true;
}

// DeserializeProgramList: QByteArray => content of program list
bool CDataManager::DeserializeProgramList(QByteArray* p_ByteArray)
{
//   if (!mp_DataContainer->ProgramList->DeserializeContent(p_ByteArray)) {
//        qDebug() << "CDataManager::DeserializeContent failed.";
//        return false;
//    }

    return true;
}


//********************************************************************
//some function to access the reagent list
//********************************************************************

//get copy of complete reagent list
bool CDataManager::GetReagentList(CDataReagentList* p_ReagentListCopy)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetReagentList failed. Not initialized!";
        return false;
    }

    if (p_ReagentListCopy == NULL) {
        qDebug() << "CDataManager::GetReagentList failed: p_ReagentListCopy must not be NULL!";
        return false;
    }

    p_ReagentListCopy = mp_DataContainer->ReagentList;
    return true; // p_ReagentListCopy->CloneFrom(mp_DataContainer->ReagentList);
}

//delete reagent with given ID   (called from GUI)
bool CDataManager::DeleteReagent(QString ID)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::DeleteReagent failed. Not initialized!";
        return false;
    }

    // let master do the work ...
    Q_UNUSED(ID);
//    if (!mp_DataContainer->ReagentList->DeleteReagent(ID)) {
//        qDebug() << "CDataManager::DeleteReagent failed.";
//        return false;
//    }

    // TODO prepare command and send to master
    // RESULT = DELETE_REAGENT(ID)

    return true;

}

//update reagent  (called from GUI)
bool CDataManager::UpdateReagent(CReagent* p_Reagent)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::UpdateReagent failed. Not initialized!";
        return false;
    }

    // let master do the work ...
//    if (!mp_DataContainer->ReagentList->UpdateReagent(p_Reagent)) {
//        qDebug() << "CDataManager::UpdateReagent failed.";
//        return false;
//    }

    //prepare transfer command and send to master
    // TODO Implement the below
    /*
    QByteArray* p_ByteArray = new QByteArray();
    if (!p_Reagent->SerializeContent(p_ByteArray)) {
        qDebug() << "CDataManager::UpdateReagent (SerializeContent) failed.";
        return false;
    }
    qDebug() << "CDataManager::GetReagent(ByteArray): " << *p_ByteArray
    */

    // TODO send command with byte array to master ...
    // ...
    // if master accepts the changes, he will call the following function for the same ID
    // GUI has to block user actions during this time

    return true;
}

//update reagent  (called from Master via Network-Layer)
bool CDataManager::UpdateReagent(QByteArray* p_ByteArray)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::UpdateReagent failed. Not initialized!";
        return false;
    }

    CReagent *p_Reagent = new CReagent();
    // TODO Implement the matching
    /*
    if (!p_Reagent->DeserializeContent(p_ByteArray)) {
        qDebug() << "CDataManager::UpdateReagent (DeserializeContent) failed.";
        return false;
    }
    */

    if (!mp_DataContainer->ReagentList->UpdateReagent(p_Reagent)) {
        qDebug() << "CDataManager::UpdateReagent failed.";
        return false;
    }

    delete p_Reagent;

    return true;
}

// SerializeReagentList: content of reagent list => QByteArray
bool CDataManager::SerializeReagentList(QByteArray* p_ByteArray)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::SetReagentList failed. Not initialized!";
        return false;
    }
    /*if (!mp_DataContainer->ReagentList->SerializeContent(p_ByteArray)) {
        qDebug() << "CDataManager::SerializeContent failed.";
        return false;
    }*/

    return true;
}

// DeserializeReagentList: QByteArray => content of reagent list
bool CDataManager::DeserializeReagentList(QByteArray* p_ByteArray)
{
    // this method can be called
//    if (m_IsInitialized != true) {
//        qDebug() << "CDataManager::SetReagentList failed. Not initialized!";
//        return false;
//    }

    /*if (!mp_DataContainer->ReagentList->DeserializeContent(p_ByteArray)) {
        qDebug() << "CDataManager::DeserializeContent failed.";
        return false;
    }*/ // need to look -- MKV

    return true;
}

// write reagent list to file
bool CDataManager::WriteReagentList(QString Filename)
{
    if (!mp_DataContainer->ReagentList->Write(Filename)) {
        qDebug() << "CDataManager::WriteReagentList failed.";
        return false;
    } else  {
        return true;
    }
}

//********************************************************************
// some function to access the station list
//********************************************************************

/****************************************************************************/
/*!
 *  \brief Gets a copy of complete station list.
 *
 *  \iparam p_SourceStationList = Instance of CDataStationList class
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataManager::GetStationList(CDataStationList* p_StationListCopy)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::GetStationList failed. Not initialized!";
        return false;
    }

    if (p_StationListCopy == NULL) {
        qDebug() << "CDataManager::GetStationList failed: p_StationListCopy must not be NULL!";
        return false;
    }

    *p_StationListCopy = *mp_DataContainer->StationList;
    return true;
//    return p_StationListCopy->CloneFrom(mp_DataContainer->StationList);
}

/****************************************************************************/
/*!
 *  \brief Delete station with given ID called from GUI.
 *
 *  \iparam p_SourceStationList = Station ID
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataManager::DeleteStation(QString ID)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::DeleteStation failed. Not initialized!";
        return false;
    }

    Q_UNUSED(ID);
    if (!mp_DataContainer->ReagentList->DeleteReagent(ID)) {
            qDebug() << "CDataManager::DeleteReagent failed.";
            return false;
        }

    // TODO prepare command and send to master

   return true;
}

/****************************************************************************/
/*!
 *  \brief Update station called from the GUI.
 *
 *  \iparam p_Station = Instance of station class
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataManager::UpdateStation(CStation* p_Station)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::UpdateStation failed. Not initialized!";
        return false;
    }

    // let master do the work ...
//    if (!mp_DataContainer->ReagentList->UpdateStation(p_Reagent)) {
//        qDebug() << "CDataManager::UpdateStation failed.";
//        return false;
//    }

    //prepare transfer command and send to master
//    QByteArray* p_ByteArray = new QByteArray();
//    if (!p_Station->SerializeContent(p_ByteArray)) {
//        qDebug() << "CDataManager::UpdateStation (SerializeContent) failed.";
//        return false;
//    }
//    qDebug() << "CDataManager::GetStation(ByteArray): " << *p_ByteArray;

    // TODO send command with byte array to master ...
    // ...
    // if master accepts the changes, he will call the following function for the same ID
    // GUI has to block user actions during this time

    return true;
}

/****************************************************************************/
/*!
 *  \brief Update station called from Master via Network-Layer.
 *
 *  \iparam p_ByteArray = Instance of to Qt array of bytes
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataManager::UpdateStation(QByteArray* p_ByteArray)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::UpdateStation failed. Not initialized!";
        return false;
    }

//    CStation *p_Station = new CStation();

//    if (!p_Station->DeserializeContent(p_ByteArray)) {
//        qDebug() << "CDataManager::UpdateStation (DeserializeContent) failed.";
//        return false;
//    }

//    if (!mp_DataContainer->StationList->UpdateStation(p_Station)) {
//        qDebug() << "CDataManager::UpdateStation failed.";
//        return false;
//    }

//    delete p_Station;

    return true;
}

/****************************************************************************/
/*!
 *  \brief Copies content of station list to QByteArray.
 *
 *  \iparam p_ByteArray = Instance of to Qt array of bytes
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataManager::SerializeStationList(QByteArray* p_ByteArray)
{
    if (m_IsInitialized != true) {
        qDebug() << "CDataManager::SerializeStationList failed. Not initialized!";
        return false;
    }

//    if (!mp_DataContainer->StationList->SerializeContent(p_ByteArray)) {
//        qDebug() << "CDataManager::SerializeContent failed.";
//        return false;
//    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Copies QByteArray to station list.
 *
 *  \iparam p_ByteArray = Instance of to Qt array of bytes
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataManager::DeserializeStationList(QByteArray* p_ByteArray)
{
//    if (!mp_DataContainer->StationList->DeserializeContent(p_ByteArray)) {
//        qDebug() << "CDataManager::DeserializeContent failed.";
//        return false;
//    }

    return true;
}

/****************************************************************************/
/*!
 *  \brief Writes station list to file.
 *
 *  \iparam Filename = Station file name
 *
 *  \return True or False
 */
/****************************************************************************/
bool CDataManager::WriteStationList(QString Filename)
{
    if (!mp_DataContainer->StationList->Write(Filename)) {
        qDebug() << "CDataManager::WriteStationList failed.";
        return false;
    } else  {
        return true;
    }
}

} // namespace DataManager

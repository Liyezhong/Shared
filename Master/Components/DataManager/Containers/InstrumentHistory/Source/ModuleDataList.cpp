/****************************************************************************/
/*! \file DataManager/Containers/InstrumentHistory/Source/ModuleDataList.cpp
 *
 *  \brief Implementation file for class CModuleDataList.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2012-12-21
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

#include "DataManager/Containers/InstrumentHistory/Include/ModuleDataList.h"
#include "DataManager/Helper/Include/Helper.h"
#include "Global/Include/Utils.h"
//#include "Global/Include/Exception.h"
//#include "Global/Include/EventObject.h"

#include <QDebug>
#include <QFile>
#include <QBuffer>
#include <QReadLocker>
#include <QWriteLocker>
#include <QReadWriteLock>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace DataManager
{

/****************************************************************************/
/*!
 *  \brief Default Constructor
 */
/****************************************************************************/
CModuleDataList::CModuleDataList() : CDataContainerBase(), m_InstrumentName(""),
    m_ModuleTimeStamp(""),
    m_DataVerificationMode(true),
    m_FileName(""),
    mp_ReadWriteLock(NULL)
{
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);

}

/****************************************************************************/
/*!
 *  \brief Parameterized Constructor
 *  \iparam TimeStamp
 */
/****************************************************************************/
CModuleDataList::CModuleDataList(QString TimeStamp) : CDataContainerBase(), m_InstrumentName(""),
    m_ModuleTimeStamp(""),
    m_DataVerificationMode(true),
    m_FileName(""),
    mp_ReadWriteLock(NULL)
{
    m_ModuleTimeStamp = TimeStamp;
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ModuleDataList = Instance of the ModuleList class
 *
 */
/****************************************************************************/
CModuleDataList::CModuleDataList(const CModuleDataList& ModuleDataList) : CDataContainerBase(),
    m_InstrumentName(""),
    m_ModuleTimeStamp(""),
    m_DataVerificationMode(true),
    m_FileName(""),
    mp_ReadWriteLock(NULL)
{
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    CopyFromOther(ModuleDataList);
}
/****************************************************************************/
/*!
 *  \brief Copy Data from another instance.
 *         This function should be called from CopyConstructor or
 *         Assignment operator only.
 *
 *  \iparam ListofModules = Instance of the CModuleDataList class
.*  \note  Method for internal use only
 *
 *  \return
 */
/****************************************************************************/
void CModuleDataList::CopyFromOther(const CModuleDataList &Other)
{
    QString InstrumentName = const_cast<CModuleDataList&>(Other).GetInstrumentName();
    QString TimeStamp = const_cast<CModuleDataList&>(Other).GetModuleTimeStamp();

    this->SetInstrumentName(InstrumentName);
    this->SetModuleTimeStamp(TimeStamp);

    //qDebug() <<"ModuleList Keys"<< m_ModuleList.keys().toSet();
    //qDebug()<<"Other ModuleList keys" << Other.GetModuleIdList().toSet();
    QSet<QString> ModulesInOurList(static_cast< QSet<QString> >(m_ModuleList.keys().toSet()));
    QSet<QString> ModulesInOtherList(Other.GetModuleIdList().toSet());
    //we get a list of Modules that are not there in new list but present in our list, they are to be deleted.
    QSet<QString> ModulesToDelete(ModulesInOurList.subtract(ModulesInOtherList));
    QSetIterator<QString> Itr(ModulesToDelete);
    while (Itr.hasNext()) {
        DeleteModule(Itr.next());
    }

    //Recreate Modules in our list
    ModulesInOurList = m_ModuleList.keys().toSet();
    //Update Modules which are present in our list and other list
    QSet<QString> ModulesToUpdate(static_cast< QSet<QString> >(ModulesInOurList.intersect(ModulesInOtherList)));
    QSetIterator<QString> UpdateItr(ModulesToUpdate);
    while (UpdateItr.hasNext()) {
        const QString ModuleID = UpdateItr.next();
        const CModule *p_OtherModule = Other.GetModule(ModuleID);
        CModule *p_OurModule = GetModule(ModuleID);
        if (p_OtherModule && p_OurModule) {
            //There exist a Module with similar ID in the other Modulelist.
            //update our Module with values from other.
             *p_OurModule = *p_OtherModule;
        }
    }

    //Finally create Modules which are not present in our list but in other list.
    QSet<QString> ModulesToCreate(ModulesInOtherList.subtract(ModulesInOurList));
    QSetIterator<QString> CreateItr(ModulesToCreate);
    while (CreateItr.hasNext()) {
        const CModule *p_OtherModule = Other.GetModule(CreateItr.next());
        AddModuleWithoutVerification(p_OtherModule);
    }

    m_ListofModules.clear();
    m_ListofModules = Other.m_ListofModules;
}

/****************************************************************************/
/*!
 *  \brief  Returns the ID's of all modules present in the list
 *
 *  \return QStringList = list of moduled Ids
 */
/****************************************************************************/
QStringList CModuleDataList::GetModuleIdList() const
{
    return m_ModuleList.keys();
}

/****************************************************************************/
/*!
 *  \brief Adds Module to ModuleList
 *  \iparam p_Module = Module to add
 *  \warning
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
void CModuleDataList::AddModuleWithoutVerification(CModule const* p_Module)
{
    try {
        CHECKPTR(p_Module)

        CModule *p_TempModule = new CModule();
        *p_TempModule = *p_Module;

        QWriteLocker locker(mp_ReadWriteLock);
        m_ModuleList.insert(p_TempModule->GetModuleName(), p_TempModule);
        m_ListofModules.append(p_TempModule->GetModuleName());
    }
    CATCHALL()
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CModuleDataList::~CModuleDataList()
{    
    //Ignoring return value
    try {
        (void)DeleteAllModules(); //to avoid lint warning 534
    }
    CATCHALL_DTOR();

    if (mp_ReadWriteLock != NULL) {
        try {
            delete mp_ReadWriteLock;
        }
        CATCHALL_DTOR();
        mp_ReadWriteLock = NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief Reads an xml file and fill ModuleList object
 *
 *  \iparam FileName = Filename to read
 *
 *  \return true on success, false on failure
 */
/****************************************************************************/
bool CModuleDataList::ReadFile(const QString FileName)
{

    SetFileName(FileName);
    QFile File(FileName);
    if (!QFile::exists(FileName)) {
        return false;
    }

    bool Result = true;
    if (m_DataVerificationMode) {
        //To make sure other threads cant write when reading is active
        QWriteLocker locker(mp_ReadWriteLock);

        // create instance of CModuleDataList for verification
        CModuleDataList* p_MDL_Verification = new CModuleDataList();

        // create clone from current state
        *p_MDL_Verification = *this;

        // disable verification in clone
        p_MDL_Verification->SetDataVerificationMode(false);

        // execute required action (Read) in clone
        Result = true;
        if (!p_MDL_Verification->ReadFile(FileName)) {
            Result = false;
        } else {

            // now check new content => call all active verifiers
            if (DoLocalVerification(p_MDL_Verification)) {
                // if content ok, clone backwards
                *this = *p_MDL_Verification;
                Result = true;
            }
            else {
                Result = false;
            }
        }
        // delete test clone
        delete p_MDL_Verification;
    }
    else {
        QWriteLocker locker(mp_ReadWriteLock);

        // Initialise the m_Filename to a known string "UNDEFINED"
        m_FileName = "UNDEFINED";


        if (!File.open(QFile::ReadOnly | QFile::Text )) {
            qDebug() << "Open file to read failed " << FileName;
            return false;
        }

        if (!DeserializeContent(File, false)) {
            qDebug() << " CModuleDataList::Read failed for file during deserializing: " << FileName;
            return false;
        }

        m_FileName = FileName;

        File.close();
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Reads the CModuleDataList Data from QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice - Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CModuleDataList::DeserializeContent(QIODevice& IODevice, bool CompleteData)
{   
    bool Result = true;
    QXmlStreamReader XmlStreamReader;
    XmlStreamReader.setDevice(&IODevice);

    // look for node <InstrumentHistory>
    if (!Helper::ReadNode(XmlStreamReader, "InstrumentHistory")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: System";
        return false;
    }

    // Read attribute Name
    if (!XmlStreamReader.attributes().hasAttribute("name")) {
        qDebug() << " attribute System <name> is missing => abort reading";
        return false;
    }

    SetInstrumentName(XmlStreamReader.attributes().value("name").toString());

    // Look for node <ModuleList>
    if (!Helper::ReadNode(XmlStreamReader, "ModuleList")) {
        qDebug() << "DeserializeContent: abort reading. Node not found: ModuleList";
        return false;
    }

    // Read Attribute TimeStamp
    if (!XmlStreamReader.attributes().hasAttribute("timestamp")) {
        qDebug() << " attribute <timestamp> is missing => abort reading";
        return false;
    }

    SetModuleTimeStamp(XmlStreamReader.attributes().value("timestamp").toString());

    Result = ReadModules(XmlStreamReader, CompleteData);

    if (CompleteData && Result) {
        if (!Helper::ReadNode(XmlStreamReader, "NonXmlData")) {
            return false;
        }

        if (!XmlStreamReader.attributes().hasAttribute("VerificationMode")) {
            qDebug() << "### attribute <VerificationMode> is missing";
            return false;
        }

        if (XmlStreamReader.attributes().value("VerificationMode").toString() == "true") {
            SetDataVerificationMode(true);
        }
        else {
            SetDataVerificationMode(false);
        }

        if (!XmlStreamReader.attributes().hasAttribute("FileName")) {
            qDebug() << "### attribute <FileName> is missing";
            return false;
        }
        SetFileName(XmlStreamReader.attributes().value("FileName").toString());

    }
    (void)XmlStreamReader.device()->reset();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Writes the CModuleDataList Data to QIODevice
 *
 *  \iparam IODevice = Instance of the IODevice might be Buffer or File
 *  \iparam CompleteData = bool type if true writes Complete data of object
 *
 *  \return True or False
 */
/****************************************************************************/
bool CModuleDataList::SerializeContent(QIODevice& IODevice, bool CompleteData)
{
    QXmlStreamWriter XmlStreamWriter;
    XmlStreamWriter.setDevice(&IODevice);

    XmlStreamWriter.setAutoFormatting(true);
    XmlStreamWriter.writeStartDocument();

    // XmlStreamWriter.writeDTD("<InstrumentHistory>");
    XmlStreamWriter.writeStartElement("InstrumentHistory");

    // write attribute name
    XmlStreamWriter.writeAttribute("name", GetInstrumentName());

    // write section ModuleList
    XmlStreamWriter.writeStartElement("ModuleList");

    // write attribute timestamp
    XmlStreamWriter.writeAttribute("timestamp", GetModuleTimeStamp());

    // write all modules
    for (int i = 0; i < GetNumberofModules(); i++)
    {
        DataManager::CModule *p_Module = GetModule(i);
        if (p_Module != NULL) {
            if (!p_Module->SerializeContent(XmlStreamWriter, CompleteData)) {
                qDebug("DataManager::CModule SerializeContent failed ");
                return false;
            }
        }
    }

    XmlStreamWriter.writeEndElement(); // ModuleList

    if (CompleteData) {
        XmlStreamWriter.writeStartElement("NonXmlData");
        if (GetDataVerificationMode()) {
            XmlStreamWriter.writeAttribute("VerificationMode", "true");
        } else {
            XmlStreamWriter.writeAttribute("VerificationMode", "false");
        }

        XmlStreamWriter.writeAttribute("FileName", GetFilename());
        XmlStreamWriter.writeEndElement();
    }

    XmlStreamWriter.writeEndElement(); // for Instrument History
    XmlStreamWriter.writeEndDocument(); // End of Document

    return true;
}

/****************************************************************************/
/*!
 *  \brief Reads all the modules from the xml stream
 *
 *  \iparam XmlStreamReader = Reference to xml stream to read from
 *  \iparam CompleteData = true to read Complete data of object
 *
 *  \return true - read success or false - read failure
 */
/****************************************************************************/
bool CModuleDataList::ReadModules(QXmlStreamReader &XmlStreamReader, bool CompleteData)
{
    bool Result = true;

    // Look for node <Module>
    while(!XmlStreamReader.atEnd())
    {
        if (static_cast<int>(XmlStreamReader.readNext()) == 1) {
            qDebug() << "Reading " << XmlStreamReader.name() << " at line number: " << XmlStreamReader.lineNumber();
            qDebug() << "Invalid Token. Error: " << XmlStreamReader.errorString();
        }

        if (XmlStreamReader.isStartElement()) {
            if (XmlStreamReader.name() == "Module") {
                DataManager::CModule Module;

                if (!Module.DeserializeContent(XmlStreamReader, CompleteData)) {
                    qDebug() << "second " << "CModule Deserialize failed!";
                    return false;
                }

                if (!AddModule(&Module)) {
                    qDebug() << "CModuleDataList::Add Module failed!";
                    return false;
                }
            } else {
                qDebug() << " Unknown node name <" << XmlStreamReader.name().toString() << "> at line number: " << XmlStreamReader.lineNumber();
                return false;
            }
        } else if (XmlStreamReader.isEndElement() && XmlStreamReader.name().toString() == "ModuleList") {
            //            qDebug() << XmlStreamReader.name().toString();
            break; // exit from while loop
        }
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Adds Module to ModuleList
 *  \iparam p_Module = Module to add
 *  \warning
 *  \return true = add success , false - add failed
 */
/****************************************************************************/
bool CModuleDataList::AddModule(CModule const* p_Module)
{
    bool Result = true;
    if(p_Module == NULL) return false;

    QString name = const_cast<CModule*>(p_Module)->GetModuleName();
    if(m_ListofModules.contains(name)) {
        qDebug() << "Name already exists";
        return false;
    }

    CModule *p_TempModule = new CModule();
    *p_TempModule = *p_Module;

    Result = true;
    if ((m_DataVerificationMode)) {
        CModuleDataList* p_MDL_Verification = new CModuleDataList();
        CHECKPTR(p_MDL_Verification);
        //p_MDL_Verification = NULL;

        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_MDL_Verification = *this;

            // disable verification in clone
            p_MDL_Verification->SetDataVerificationMode(false);

            // execute required action (AddModule) in clone
            Result = p_MDL_Verification->AddModule(p_TempModule);

            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_MDL_Verification);
            }
        }
        if (Result) {
            // if content ok, clone backwards
            *this = *p_MDL_Verification;
        }
        else {
            //Store errors.
            // Since we are going to delete p_MDL_Verification
            // We need to store the errors generated by it.
            // For now Errors are not copied by assignment or copy constructors.
            ListOfErrors_t ErrorList = p_MDL_Verification->GetErrorList();
            if (!ErrorList.isEmpty()) {
                // Control reaches here means Error list is not empty.
                // Considering only the first element in list since
                // verfier can atmost add only one Hash has to the error list
                m_ErrorMap = *(ErrorList.first());
                SetErrorList(&m_ErrorMap);
            }
        }

        // delete test clone
        delete p_MDL_Verification;
        delete p_TempModule;

    } else {

        //QWriteLocker locker(mp_ReadWriteLock);
        m_ModuleList.insert(p_TempModule->GetModuleName(), p_TempModule);
        m_ListofModules.append(p_TempModule->GetModuleName());

        Result = true;
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief  Updates Module
 *  \iparam p_Module = Module which needs to be updated
 *  \return true - update success, false - update failed
 */
/****************************************************************************/
bool CModuleDataList::UpdateModule(CModule const* p_Module)
{

    CHECKPTR_RETURN(p_Module, false);

    QString ModuleName = p_Module->GetModuleName();
    if (!m_ModuleList.contains(ModuleName))
    {
        return false;
    }

    bool Result = false;
    if (m_DataVerificationMode) {

        //CModuleDataList* p_MDL_Verification = new CModuleDataList();
        CModuleDataList MDL_Verification;
        //p_MDL_Verification = NULL;
        //        CModule* p_ModuleData = new CModule();
        //        CHECKPTR(p_ModuleData);

        //CModule* p_ModuleData = m_ModuleList.value(ModuleName, NULL);

        //QString SerialNumber = p_Module->GetSerialNumber();
        //        QString DateOfProd = p_Module->GetDateOfProduction();
        //QString OperatingHrs = p_Module->GetOperatingHours();

        // CHECKPTR(p_ModuleData);
        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            MDL_Verification = *this;

            // disable verification in clone
            MDL_Verification.SetDataVerificationMode(false);

            // execute required action (UpdateModule) in clone
            Result = MDL_Verification.UpdateModule(p_Module);

            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(&MDL_Verification);
                if (!Result) {
                    //Store errors.
                    // Since we are going to delete p_DPL_Verification
                    // We need to store the errors generated by it.
                    // For now Errors are not copied by assignment or copy constructors.
                    ListOfErrors_t ErrorList = MDL_Verification.GetErrorList();
                    if (!ErrorList.isEmpty()) {
                        // Control reaches here means Error list is not empty.
                        // Considering only the first element in list since
                        // verfier can atmost add only one Hash has to the error list
                        m_ErrorMap = *(ErrorList.first());
                        SetErrorList(&m_ErrorMap);
                    }
                }
            }

            if (Result) {
                *this = MDL_Verification;
            }

//            //        p_ModuleData = m_ModuleList.value(ModuleName, NULL);
//            //int Index = m_ModuleList.

//            if (SerialNumber != p_ModuleData->GetSerialNumber()) {
//                p_ModuleData->SetSerialNumber(SerialNumber);
//            }

//            if (OperatingHrs != p_ModuleData->GetOperatingHours()) {
//                p_ModuleData->SetOperatingHours(OperatingHrs);
//            }

//            //        if (DateOfProd != p_ModuleData->GetDateOfProduction()) {
//            //            p_ModuleData->SetDateOfProduction(DateOfProd);
//            //        }

//            *m_ModuleList.value(ModuleName) = *p_ModuleData;

//            delete p_MDL_Verification;

        }
    } else {
        QWriteLocker locker(mp_ReadWriteLock);

        // do a deep copy
        *m_ModuleList.value(ModuleName) = *p_Module;
        Result = true;
    }

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Function to retrieve module for given index
 *  \iparam Index = Index in module list
 *
 *  \return Module associated with the index
 */
/****************************************************************************/
CModule* CModuleDataList::GetModule(const unsigned int Index)
{
    if (Index < (unsigned int)m_ListofModules.count()) {
        QString Key = m_ListofModules.value(Index);
        CModule* p_Module = m_ModuleList.value(Key, NULL);

        return p_Module;
    }
    return NULL;
}

//deep copy
/****************************************************************************/
/*!
 *  \brief Function to retrieve module Infor for given Module Name. Module
 *  passed is Updated with a ModuleInfo having ModuleName in the list
 *
 *  \iparam ModuleName = Name of the Module
 *  \iparam Module = Module reference
 *
 *  \return true - success , false - failure
 */
/****************************************************************************/
bool CModuleDataList::GetModule(const QString ModuleName, CModule& Module)
{
    QReadLocker locker(mp_ReadWriteLock);

    if (m_ModuleList.contains(ModuleName)) {
        Module = *(m_ModuleList.value(ModuleName));
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the Module
 *  \iparam Index
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CModuleDataList::DeleteModule(const unsigned int Index)
{
    if (Index < (unsigned int)m_ListofModules.count()) {
        QString ModuleName = m_ListofModules.value(Index);
        return DeleteModule(ModuleName);
    }
    else {
        return false;
    }
}

/****************************************************************************/
/*!
 *  \brief  Deletes the Module
 *  \iparam ModuleName
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CModuleDataList::DeleteModule(const QString ModuleName)
{
    if(m_ModuleList.contains(ModuleName)) {
        //get Module from ModuleList and free memory
        delete m_ModuleList.value(ModuleName);

        //remove pointer to Module from ModuleList
        m_ModuleList.remove(ModuleName);

        //remove Module from list
        int IndexCount = -1;
        for (int i=0; i<m_ListofModules.count(); i++)
        {
            if (m_ListofModules[i] == ModuleName) {
                IndexCount = i;
                break;
            }
        }

        // IndexCount MUST never be -1
        Q_ASSERT(IndexCount != -1);
        m_ListofModules.removeAt(IndexCount);
        return true;
    }
    else {
        return false;
    }

    //return true;
}

/****************************************************************************/
/*!
 *  \brief  Deletes all the programs in the list
 *  \return true - delete success , false - delete failure
 */
/****************************************************************************/
bool CModuleDataList::DeleteAllModules()
{
    bool Result = true;

    while (m_ModuleList.size() > 0)
    {
        if (DeleteModule(0) == false) {
            Result = false;
        }
    }

    m_ModuleList.clear();
    m_ListofModules.clear();

    return Result;
}

/****************************************************************************/
/*!
 *  \brief Assignment Operator which copies from rhs to lhs.
 *
 *  \iparam ListofModules = CModuleDataList class object
 *
 *  \return CModuleDataList Class Object
 */
/****************************************************************************/
CModuleDataList& CModuleDataList::operator=(const CModuleDataList& ListofModules)
{
    if (this != &ListofModules) {

        //this->DeleteAllModules();
        CopyFromOther(ListofModules);
    }
    return *this;
}

/****************************************************************************/
/*!
 *  \brief Output Stream Operator which streams data
 *
 *  \iparam OutDataStream = Instance of the QDataStream
 *  \iparam ModuleList = CModuleDataList class object
 *
 *  \return Output Stream
 */
/****************************************************************************/
QDataStream& operator << (QDataStream& OutDataStream, const CModuleDataList& ModuleList)
{
    CModuleDataList* p_TempModuleList = const_cast<CModuleDataList*>(&ModuleList);

    if (!p_TempModuleList->SerializeContent(*OutDataStream.device(), true)) {
        qDebug() << "CModuleList::Operator Streaming (SerializeContent) failed.";
    }
    return OutDataStream;
}

/****************************************************************************/
/*!
 *  \brief Input stream Operator which reads the data from Input parameter.
 *
 *  \iparam InDataStream = Instance of the DataStream
 *  \iparam ModuleList = CModuleDataList class object
 *
 *  \return Input Stream
 */
/****************************************************************************/
QDataStream& operator >> (QDataStream& InDataStream, CModuleDataList& ModuleList)
{
    if (!ModuleList.DeserializeContent(*InDataStream.device(), true)) {
        qDebug() << "CModuleList::Operator Streaming (DeSerializeContent) failed.";
    }
    return InDataStream;
}

}   // namespace DataManager


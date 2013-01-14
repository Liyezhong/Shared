/****************************************************************************/
/*! \file CModuleDataList.cpp
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
CModuleDataList::CModuleDataList() :m_InstrumentName(""),
    m_ModuleTimeStamp(""),
    m_DataVerificationMode(true)
{
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);

}

/****************************************************************************/
/*!
 *  \brief Copy Constructor
 *
 *  \iparam ModuleList = Instance of the ModuleList class
 *
 */
/****************************************************************************/
CModuleDataList::CModuleDataList(const CModuleDataList& ModuleDataList) :m_InstrumentName(""),
    m_ModuleTimeStamp(""),
    m_DataVerificationMode(true)
{
    mp_ReadWriteLock = new QReadWriteLock(QReadWriteLock::Recursive);
    *this = ModuleDataList;
}

/****************************************************************************/
/*!
 *  \brief Destructor
 */
/****************************************************************************/
CModuleDataList::~CModuleDataList()
{
    //DeleteAllModules();

    //Ignoring return value
    try {
        (void)DeleteAllModules(); //to avoid lint warning 534
    }
    catch(...) {
        //to please PClint
    }

    if (mp_ReadWriteLock != NULL) {
       try {
           delete mp_ReadWriteLock;
       }
       catch(...) {
           //to please PClint
       }
       mp_ReadWriteLock = NULL;
    }
}

/****************************************************************************/
/*!
 *  \brief Reads an xml file and fill ModuleList object
 *
 *  \iparam Filename = Filename to read
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

        // clear content
        //Init();

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
        return true;
    }
    return Result;
}

/****************************************************************************/
/*!
 *  \brief Writes the data from the container to the file
 *
 *  \iparam Filename = name of the Output file
 *
 *  \return true on success, false on failure
 */
/****************************************************************************/
bool CModuleDataList::WriteFile(const QString FileName)
{
    if (QFile::exists(FileName)) {
        if (!QFile::remove(FileName)) {
            qDebug() << "File remove failed in Write: " << FileName;
            return false;
        }
    }

    // create the file
    QFile File(FileName);
    if (!File.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "open file failed in Write: " << FileName;
        return false;
    }

    if (!SerializeContent(File, false)) {
        qDebug() << " CModuledataList::Write failed for file: " << FileName;
        File.close();
        return false;
    }
    File.close();

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
    qDebug() << "Deserialization";

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

        XmlStreamWriter.writeAttribute("FileName", GetFileName());
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
            qDebug() << XmlStreamReader.name().toString();
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
    if(m_ModuleList.contains(name)) {
        qDebug() << "Name already exists";
        return false;
    }

    CModule *p_TempModule = new CModule();
    *p_TempModule = *p_Module;

    m_ModuleList.insert(p_TempModule->GetModuleName(), p_TempModule);
    m_ListofModules.append(p_TempModule->GetModuleName());

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

    CHECKPTR(p_Module);

    QString ModuleName = p_Module->GetModuleName();
    if (!m_ModuleList.contains(ModuleName))
    {
        return false;
    }

    bool Result = false;
    if (m_DataVerificationMode) {

        CModuleDataList* p_MDL_Verification = new CModuleDataList();
        CModule* p_ModuleData;

        QString SerialNumber = p_ModuleData->GetSerialNumber();
        QString DateOfProd = p_ModuleData->GetDateOfProduction();
        QString OperatingHrs = p_ModuleData->GetOperatingHours();

        CHECKPTR(p_ModuleData);
        // first lock current state for reading
        {   // code block defined for QReadLocker.
            QReadLocker locker(mp_ReadWriteLock);

            // create clone from current state
            *p_MDL_Verification = *this;

            // disable verification in clone
            p_MDL_Verification->SetDataVerificationMode(false);

            // execute required action (UpdateModule) in clone
            Result = p_MDL_Verification->UpdateModule(p_Module);

            if (Result) {
                // now check new content => call all active verifiers
                Result = DoLocalVerification(p_MDL_Verification);
                if (!Result) {
                    //Store errors.
                    // Since we are going to delete p_DPL_Verification
                    // We need to store the errors generated by it.
                    // For now Errors are not copied by assignment or copy constructors.
                    ListOfErrors_t ErrorList = p_MDL_Verification->GetErrorList();
                    if (!ErrorList.isEmpty()) {
                        // Control reaches here means Error list is not empty.
                        // Considering only the first element in list since
                        // verfier can atmost add only one Hash has to the error list
                        m_ErrorHash = *(ErrorList.first());
                        SetErrorList(&m_ErrorHash);
                    }
                }
            }
        }

        if (Result) {
            *this = *p_MDL_Verification;
        }

        p_ModuleData = m_ModuleList.value(ModuleName, NULL);
        //int Index = m_ModuleList.

        if (SerialNumber != p_ModuleData->GetSerialNumber()) {
            p_ModuleData->SetSerialNumber(SerialNumber);
        }

        if (OperatingHrs != p_ModuleData->GetOperatingHours()) {
            p_ModuleData->SetOperatingHours(OperatingHrs);
        }

        if (DateOfProd != p_ModuleData->GetDateOfProduction()) {
            p_ModuleData->SetDateOfProduction(DateOfProd);
        }

        *m_ModuleList.value(ModuleName) = *p_ModuleData;

        delete p_MDL_Verification;

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
        }
    } else {
        return false;
    }
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
 *  \iparam ListofModule = CModuleDataList class object
 *
 *  \return CModuleDataList Class Object
 */
/****************************************************************************/
CModuleDataList& CModuleDataList::operator=(const CModuleDataList& ListofModules)
{
    if(this != &ListofModules) {
        QByteArray TempByteArray;
        QDataStream DataStream(&TempByteArray, QIODevice::ReadWrite);
        DataStream.setVersion(static_cast<int>(QDataStream::Qt_4_0));
        TempByteArray.clear();

        //Serialize
        DataStream << ListofModules;
        (void)DataStream.device()->reset();

        //Deserialize
        DataStream >> *this;
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

